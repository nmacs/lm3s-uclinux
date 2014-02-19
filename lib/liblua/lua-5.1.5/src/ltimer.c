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

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

static unsigned long cascade_bucket(struct timer_context *ctx, unsigned n, unsigned long now);

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

static inline void list_append_and_init(struct list_base *to, struct list_base *list)
{
  if (list_empty(list))
    return;
  list_append(list, to, to->next);
  list_init(list);
}

static inline unsigned long expire_in(struct timer_list *timer, unsigned long now)
{
  if (time_before(timer->expire, now))
    return 0;
  return (unsigned long)((long)timer->expire - (long)now);
}

int set_timer(struct timer_list *timer, unsigned long expire, timer_callback callback, void *data)
{
  timer->expire = expire;
  timer->callback = callback;
  timer->data = data;
  list_init(&timer->list);
  return 0;
}

int add_timer(struct timer_context *ctx, struct timer_list *timer)
{
  unsigned index;
  struct list_base *list;
  unsigned long timeout;

  timeout = expire_in(timer, ctx->root.base_time);
  if (timeout < UPPER_LIMIT(0)) {
    index = (ctx->root.cur + (unsigned)timeout) & ROOT_BUCKET_MASK;
    list = &ctx->root.v[index];
  }
  else {
  timeout = expire_in(timer, ctx->buckets[0].base_time);
  if (timeout < UPPER_LIMIT(1)) {

    index = (ctx->buckets[0].cur + (timeout >> UPPER_LIMIT_POW2(0))) & BUCKET_MASK;
    list = &ctx->buckets[0].v[index];
  }
  else {
  timeout = expire_in(timer, ctx->buckets[1].base_time);
  if (timeout < UPPER_LIMIT(2)) {
    index = (ctx->buckets[1].cur + (timeout >> UPPER_LIMIT_POW2(1))) & BUCKET_MASK;
    list = &ctx->buckets[1].v[index];
  }
  else {
  timeout = expire_in(timer, ctx->buckets[2].base_time);
  if (timeout < UPPER_LIMIT(3)) {
    index = (ctx->buckets[2].cur + (timeout >> UPPER_LIMIT_POW2(2))) & BUCKET_MASK;
    list = &ctx->buckets[2].v[index];
  }
  else {
  timeout = expire_in(timer, ctx->buckets[3].base_time);
  if (timeout < UPPER_LIMIT(4)) {
    index = (ctx->buckets[3].cur + (timeout >> UPPER_LIMIT_POW2(3))) & BUCKET_MASK;
    list = &ctx->buckets[3].v[index];
  }
  else {
    return -1;
  }}}}}

  list_insert(list, &timer->list);
  if (time_before(timer->expire, ctx->next_timer))
      ctx->next_timer = timer->expire;

  return 0;
}

void del_timer(struct timer_list *timer)
{
  list_del(&timer->list);
}

static unsigned long forward_bucket(struct timer_context *ctx, unsigned n, unsigned long now, unsigned long forward)
{
  struct timer_bucket *b = &ctx->buckets[n];
  unsigned long step = UPPER_LIMIT(n);
  unsigned index;
  unsigned long result = 0;

  if (forward < step)
    return 0;

  index = b->cur;
  do {
      if (forward < step || time_after(b->base_time, now) || !list_empty(&b->v[b->cur])) return result;
      b->base_time += step;
      forward -= step;
      result += step;
      b->cur = (b->cur + 1) & ROOT_BUCKET_MASK;
  } while (b->cur != index);

  if (time_before(b->base_time, now)) {
      unsigned long max_forward = now - b->base_time;
      if (forward > max_forward)
        forward = max_forward;
      b->base_time += forward;
      result += forward;
  }

  return result;
}

static unsigned long cascade_bucket(struct timer_context *ctx, unsigned n, unsigned long now)
{
  unsigned index;
  struct timer_bucket *b;
  unsigned long step;
  unsigned long forward = 0;

  b = &ctx->buckets[n];
  step = UPPER_LIMIT(n);
  index = b->cur;
  do {
    if (!list_empty(&b->v[b->cur])) {
      unsigned long prev_upper = step;
      if (n == 0) {
          struct list_base *item, *tmp;
          list_for_each_safe(&b->v[b->cur], item, tmp) {
            struct timer_list *timer = container_of(item, struct timer_list, list);
            if (time_before(timer->expire, ctx->root.base_time + prev_upper)) {
                unsigned index = (ctx->root.cur + (timer->expire - ctx->root.base_time)) & ROOT_BUCKET_MASK;
                list_del(&timer->list);
                list_insert(&ctx->root.v[index], &timer->list);
            }
          }
      }
      else {
          struct list_base *item, *tmp;
          list_for_each_safe(&b->v[b->cur], item, tmp) {
            struct timer_list *timer = container_of(item, struct timer_list, list);
            if (time_before(timer->expire, ctx->buckets[n-1].base_time + prev_upper)) {
                unsigned index = (b->cur + ((timer->expire - ctx->buckets[n-1].base_time) >> UPPER_LIMIT_POW2(n-1))) & BUCKET_MASK;
                list_del(&timer->list);
                list_insert(&ctx->buckets[n-1].v[index], &timer->list);
            }
          }
      }

      if (list_empty(&b->v[b->cur])) {
          b->base_time += step;
          b->cur = (b->cur + 1) & BUCKET_MASK;
          forward += step;
          if (n < (BUCKET_COUNT-1))
              cascade_bucket(ctx, n + 1, now);
      }

      return forward;
    }

    if (time_after(b->base_time + step, now))
        return forward;

    b->base_time += step;
    b->cur = (b->cur + 1) & BUCKET_MASK;
    forward += step;

    if (n < (BUCKET_COUNT-1)) {
      unsigned long next_forward = cascade_bucket(ctx, n + 1, now);
      forward += forward_bucket(ctx, n, now, next_forward);
    }
  } while (index != b->cur);

  return forward;
}

static void forward_root_bucket(struct timer_root_bucket *root, unsigned long forward, unsigned long now)
{
  unsigned index, slot;
  index = slot = root->cur;

  do {
      if (!forward || time_after(root->base_time, now) || !list_empty(&root->v[slot])) return;
      slot = (slot + 1) & ROOT_BUCKET_MASK;
      root->base_time++;
      root->cur = slot;
      forward--;
  } while (index != slot);

  if (time_before_eq(root->base_time, now)) {
      unsigned long max_forward = now - root->base_time + 1;
      if (forward > max_forward)
        forward = max_forward;
      root->base_time += forward;
  }
}

void process_timers_ex(struct timer_context *ctx, struct list_base *expired, unsigned long now)
{
  unsigned long forward;
  struct timer_root_bucket *root = &ctx->root;
  while (!time_after(root->base_time, now)) {
      if (!list_empty(&root->v[root->cur]))
          list_append_and_init(expired, &root->v[root->cur]);
      root->cur = (root->cur + 1) & (ROOT_BUCKET_SIZE - 1);
      root->base_time++;
      forward = cascade_bucket(ctx, 0, now);
      forward_root_bucket(root, forward, now);
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
  unsigned index, slot;
  unsigned i;
  unsigned long next_timer;
  int found = 0;

  index = slot = ctx->root.cur;
  do {
      struct list_base *item;
      list_for_each(&ctx->root.v[slot], item) {
          struct timer_list *timer = container_of(item, struct timer_list, list);
          if (time_before_eq(timer->expire, ctx->buckets[0].base_time))
            return timer->expire;

          next_timer = timer->expire;
          found = 1;
          goto cascade;
      }
      slot = (slot + 1) & (ROOT_BUCKET_SIZE - 1);
  } while (index != slot);

cascade:
  for (i = 0; i < BUCKET_COUNT; i++) {
      struct timer_bucket *b = &ctx->buckets[i];
      if (!found)
          next_timer = b->base_time + NEXT_TIMER_MAX_DELTA;
      index = slot = b->cur;
      do {
          struct list_base *item;
          list_for_each(&b->v[slot], item) {
              struct timer_list *timer = container_of(item, struct timer_list, list);
              if (time_before(timer->expire, next_timer))
                  next_timer = timer->expire;
              found = 1;
          }

          if (found) {
              if (i == 3 || time_before(next_timer, ctx->buckets[i+1].base_time))
                  return next_timer;
              break;
          }

          slot = (slot + 1) % BUCKET_SIZE;
      } while (index != slot);
  }

  return ctx->root.base_time + NEXT_TIMER_MAX_DELTA;
}

unsigned long get_next_timeout(struct timer_context *ctx, unsigned long now)
{
  if (time_before_eq(ctx->next_timer, ctx->root.base_time))
    ctx->next_timer = get_next_timer(ctx);

  if (time_before_eq(now, ctx->next_timer))
      return ctx->next_timer - now;
  else
      return 0;
}

static void init_root_bucket(struct timer_root_bucket *b, unsigned long now)
{
  int i;
  b->cur = 0;
  b->base_time = now;
  for (i = 0; i < ROOT_BUCKET_SIZE; i++)
    list_init(&b->v[i]);
}

static void init_bucket(struct timer_bucket *b, unsigned long base)
{
  int i;
  b->cur = 0;
  b->base_time = base;
  for (i = 0; i < BUCKET_SIZE; i++)
    list_init(&b->v[i]);
}

void init_timers(struct timer_context *ctx, unsigned long now)
{
  unsigned i;
  ctx->next_timer = now;
  init_root_bucket(&ctx->root, now);
  for (i = 0; i < BUCKET_COUNT; i++)
    init_bucket(&ctx->buckets[i], now + UPPER_LIMIT(i));
}
