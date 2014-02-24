#include "ltimer.h"
#include <assert.h>
#include <limits.h>

#define time_after(a,b)     ((long)(b) - (long)(a) < 0)
#define time_after_eq(a,b)  ((long)(b) - (long)(a) <= 0)
#define time_before(a,b)    time_after(b, a)
#define time_before_eq(a,b) time_after_eq(b, a)

#define NEXT_TIMER_MAX_DELTA  ((1UL << 30) - 1)
#define UPPER_LIMIT_POW2(n)   (ROOT_BUCKET_BITS + BUCKET_BITS*(n))
#define UPPER_LIMIT(n)        (1UL << UPPER_LIMIT_POW2(n))
#define MAX_TIMEOUT           ((unsigned long)(UPPER_LIMIT(BUCKET_COUNT) - 1))

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

static inline void list_del(struct list_base *l)
{
  struct list_base *prev = l->prev;
  struct list_base *next = l->next;
  next->prev = prev;
  prev->next = next;
  list_init(l);
}

static inline void list_insert(struct list_base *list, struct list_base *l)
{
  l->next = list->next;
  list->next->prev = l;
  list->next = l;
  l->prev = list;
}

static inline int list_empty(struct list_base *list)
{
  return list->next == list;
}

static inline void list_append(const struct list_base *list, struct list_base *prev, struct list_base *next)
{
  struct list_base *first = list->next;
  struct list_base *last = list->prev;
  first->prev = prev;
  prev->next = first;
  last->next = next;
  next->prev = last;
}

static inline void list_move_and_init(struct list_base *to, struct list_base *from)
{
  to->next = from->next;
  to->next->prev = to;
  to->prev = from->prev;
  to->prev->next = to;
  list_init(from);
}

static inline void list_append_and_init(struct list_base *to, struct list_base *list)
{
  if (list_empty(list))
    return;
  list_append(list, to, to->next);
  list_init(list);
}

int set_timer(struct timer_list *timer, unsigned long expire, timer_callback callback, void *data)
{
  timer->expire = expire;
  timer->callback = callback;
  timer->data = data;
  list_init(&timer->list);
  return 0;
}

unsigned long add_timer(struct timer_context *ctx, struct timer_list *timer)
{
  unsigned index;
  struct list_base *list;
  unsigned long expires = timer->expire;
  unsigned long timeout = expires - ctx->base_time;

  if (timeout < UPPER_LIMIT(0)) {
    int index = expires & ROOT_BUCKET_MASK;
    list = &ctx->root.v[index];
  }
  else if (timeout < UPPER_LIMIT(1)) {
    int index = (expires >> UPPER_LIMIT_POW2(0)) & BUCKET_MASK;
    list = &ctx->buckets[0].v[index];
  }
  else if (timeout < UPPER_LIMIT(2)) {
    int index = (expires >> UPPER_LIMIT_POW2(1)) & BUCKET_MASK;
    list = &ctx->buckets[1].v[index];
  }
  else if (timeout < UPPER_LIMIT(3)) {
    int index = (expires >> UPPER_LIMIT_POW2(2)) & BUCKET_MASK;
    list = &ctx->buckets[2].v[index];
  }
  else if ((signed long) timeout < 0) {
    int index = ctx->base_time & ROOT_BUCKET_MASK;
    list = &ctx->root.v[index];
  }
  else {
    int index;
    if (timeout > MAX_TIMEOUT) {
      timeout = MAX_TIMEOUT;
      expires = timeout + ctx->base_time;
    }
    index = (expires >> UPPER_LIMIT_POW2(3)) & BUCKET_MASK;
    list = &ctx->buckets[3].v[index];
  }

  list_insert(list, &timer->list);
  if (time_before(timer->expire, ctx->next_timer))
      ctx->next_timer = timer->expire;

  return timeout;
}

void del_timer(struct timer_list *timer)
{
  list_del(&timer->list);
}

static void cascade_bucket(struct timer_context *ctx, unsigned n, unsigned long now, int may_forward)
{
  int index = (ctx->base_time >> UPPER_LIMIT_POW2(n)) & BUCKET_MASK;
  int forward = 0, pre_forward = 0;
  unsigned long step = UPPER_LIMIT(n);
  struct timer_bucket *b = &ctx->buckets[n];
  struct list_base fwd_list, *item, *tmp;

  if (may_forward) {
      while (1) {
          unsigned long new = ctx->base_time + step;
          if (list_empty(&b->v[index]) && time_before_eq(new, now)) {
              ctx->base_time = new;
              if (index == 0) {
                  if (pre_forward)
                    forward = 1;
                  else
                    pre_forward = 1;
                  if (n < (BUCKET_COUNT - 1))
                    cascade_bucket(ctx, n + 1, now, forward);
              }
              index = (index + 1) & BUCKET_MASK;
          }
          else
              break;
      }
  }

  list_init(&fwd_list);
  list_move_and_init(&fwd_list, &b->v[index]);
  list_for_each_safe(&fwd_list, item, tmp) {
    struct timer_list *timer = container_of(item, struct timer_list, list);
    del_timer(timer);
    add_timer(ctx, timer);
  }

  if (index == 0) {
      if (n < (BUCKET_COUNT - 1))
        cascade_bucket(ctx, n + 1, now, 0);
  }
}

void process_timers_ex(struct timer_context *ctx, struct list_base *expired, unsigned long now)
{
  unsigned long forward = 0, pre_forward = 0;
  struct timer_root_bucket *root = &ctx->root;
  while (time_before_eq(ctx->base_time, now)) {
      int root_index = ctx->base_time & ROOT_BUCKET_MASK;
      if (!root_index) {
        if (pre_forward)
          forward = 1;
        else
          pre_forward = 1;
        cascade_bucket(ctx, 0, now, forward);
      }
      ctx->base_time++;
      if (!list_empty(&root->v[root_index])) {
        list_append_and_init(expired, &root->v[root_index]);
        forward = pre_forward = 0;
      }
  }
}

void process_timers(struct timer_context *ctx, unsigned long now)
{
  struct list_base expired;
  struct list_base *item, *tmp;
  list_init(&expired);
  process_timers_ex(ctx, &expired, now);
  list_for_each_safe(&expired, item, tmp) {
    struct timer_list *timer = container_of(item, struct timer_list, list);
    if (timer->callback)
      timer->callback(timer, timer->data);
  }
}

static unsigned long get_next_timer(struct timer_context *ctx)
{
  unsigned long base_time = ctx->base_time;
  unsigned long next_timer = base_time + NEXT_TIMER_MAX_DELTA;
  int index, slot, found = 0, i;

  index = slot = base_time & ROOT_BUCKET_MASK;
  do {
      struct list_base *item;
      list_for_each(&ctx->root.v[slot], item) {
        struct timer_list *timer = container_of(item, struct timer_list, list);
        found = 1;
        next_timer = timer->expire;
        if (!index || slot < index)
          goto cascade;
      }
      slot = (slot + 1) & ROOT_BUCKET_MASK;
  } while (slot != index);

cascade:
  if (index)
    base_time += ROOT_BUCKET_SIZE - index;
  base_time >>= ROOT_BUCKET_BITS;

  for (i = 0; i < BUCKET_COUNT; i++) {
      index = slot = base_time & BUCKET_MASK;
      do {
          struct list_base *item;
          list_for_each(&ctx->buckets[i].v[slot], item) {
            struct timer_list *timer = container_of(item, struct timer_list, list);
            found = 1;
            if (time_before(timer->expire, next_timer))
              next_timer = timer->expire;
          }
          if (found) {
              if (!index || slot < index)
                break;
              return next_timer;
          }
          slot = (slot + 1) & BUCKET_MASK;
      } while (slot != index);

      if (index)
        base_time += BUCKET_SIZE - index;
      base_time >>= BUCKET_BITS;
  }
  return next_timer;
}

unsigned long get_next_timeout(struct timer_context *ctx, unsigned long now)
{
  if (time_before_eq(ctx->next_timer, ctx->base_time))
    ctx->next_timer = get_next_timer(ctx);

  if (time_before(now, ctx->next_timer))
      return ctx->next_timer - now;
  else
      return 0;
}

static void init_root_bucket(struct timer_root_bucket *b)
{
  int i;
  for (i = 0; i < ROOT_BUCKET_SIZE; i++)
    list_init(&b->v[i]);
}

static void init_bucket(struct timer_bucket *b)
{
  int i;
  for (i = 0; i < BUCKET_SIZE; i++)
    list_init(&b->v[i]);
}

void init_timers(struct timer_context *ctx, unsigned long now)
{
  unsigned i;
  ctx->base_time = ctx->next_timer = now;
  init_root_bucket(&ctx->root);
  for (i = 0; i < BUCKET_COUNT; i++)
    init_bucket(&ctx->buckets[i]);
}
