#include "ltimer.h"
#include <assert.h>

#define BUCKET_BITS 6
#define BUCKET_SIZE (1 << BUCKET_BITS)
#define BUCKET_MASK (BUCKET_SIZE - 1)
#define BUCKET_COUNT 5

#define time_after(a,b)  ((long)(b) - (long)(a) < 0)
#define time_after_eq(a,b)  ((long)(b) - (long)(a) <= 0)
#define time_before(a,b) time_after(b, a)

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

struct timer_bucket
{
  unsigned cur;
  unsigned order;
  struct timer_bucket *next;
  struct list_base v[BUCKET_SIZE];
};

struct timer_context
{
  unsigned long base_time;
  struct timer_bucket buckets[BUCKET_COUNT];
};

static struct timer_context ctx;

static int cascade_bucket(struct timer_context *ctx, struct timer_bucket *b);

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

static inline void list_move_and_init(struct list_base *to, struct list_base *from)
{
  from->next->prev = to;
  from->prev->next = to;
  to->next = from->next;
  to->prev = from->prev;
  list_init(from);
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

static void insert_timer(struct timer_bucket *b, struct timer_list *timer, unsigned index)
{
  index = (index + b->cur) & BUCKET_MASK;
  list_insert(&b->v[index], &timer->list);
}

int add_timer(struct timer_list *timer)
{
  unsigned long timeout = expire_in(timer, ctx.base_time);

  if (timeout < (1 << (1 * BUCKET_BITS)))
    insert_timer(&ctx.buckets[0], timer, (unsigned)timeout);
  else if (timeout < (1 << (2 * BUCKET_BITS)))
    insert_timer(&ctx.buckets[1], timer, (unsigned)((timeout >> BUCKET_BITS)));
  else if (timeout < (1 << (3 * BUCKET_BITS)))
    insert_timer(&ctx.buckets[2], timer, (unsigned)((timeout >> (BUCKET_BITS * 2))));
  else if (timeout < (1 << (4 * BUCKET_BITS)))
    insert_timer(&ctx.buckets[3], timer, (unsigned)((timeout >> (BUCKET_BITS * 3))));
  else if (timeout < (1 << (5 * BUCKET_BITS)))
    insert_timer(&ctx.buckets[4], timer, (unsigned)((timeout >> (BUCKET_BITS * 4))));
  else
      return -1;

  return 0;
}

void del_timer(struct timer_list *timer)
{
  list_del(&timer->list);
}

static int bucket_empty(struct timer_bucket *b)
{
  int i;
  for (i = 0; i < BUCKET_SIZE; i++)
    if (!list_empty(&b->v[i]))
      return 0;
  return 1;
}

static unsigned long fast_forward(struct timer_context *ctx, struct list_base *expired, unsigned long elapsed)
{
  struct timer_bucket *b = ctx->buckets;
  unsigned long sum_forward = 0;
  while (b && elapsed) {
    unsigned order = b->order;
    unsigned long forward = 1U << (order * BUCKET_BITS);
    if (forward > elapsed) break;
    while (elapsed >= forward) {
      if (list_empty(&b->v[b->cur])) {
        sum_forward += forward;
        ctx->base_time += forward;
        elapsed -= forward;
      }
      else
        return sum_forward;
      b->cur = (b->cur + 1) & BUCKET_MASK;
      if (b->cur == 0) {
        if ((b->next && cascade_bucket(ctx, b->next)) || !bucket_empty(b))
          return sum_forward;
        b = b->next;
        break;
      }
    }
  }
  return sum_forward;
}

static int cascade_bucket(struct timer_context *ctx, struct timer_bucket *b)
{
  int ret = 0;
  if (!list_empty(&b->v[b->cur])) {
    struct list_base timers;
    struct list_base *item, *tmp;
    list_move_and_init(&timers, &b->v[b->cur]);
    list_for_each_safe(&timers, item, tmp) {
      struct timer_list *timer = container_of(item, struct timer_list, list);
      del_timer(timer);
      add_timer(timer);
    }
    ret = 1;
  }
  b->cur = (b->cur + 1) & BUCKET_MASK;
  if (b->cur == 0 && b->next)
    ret |= cascade_bucket(ctx, b->next);
  return ret;
}

static void process_buckets(struct timer_context *ctx, struct list_base *expired, unsigned long elapsed)
{
  struct timer_bucket *b = ctx->buckets;
  unsigned long forward;
  while (1) {
    if (!list_empty(&b->v[b->cur])) {
      list_append_and_init(expired, &b->v[b->cur]);
      if (!elapsed) break;
      ctx->base_time++;
      b->cur = (b->cur + 1) & BUCKET_MASK;
      if (b->cur == 0 && b->next)
        cascade_bucket(ctx, b->next);
      elapsed--;
    }
    else {
      if (!elapsed) break;
      forward = fast_forward(ctx, expired, elapsed);
      list_append_and_init(expired, &b->v[b->cur]);
      elapsed -= forward;
    }
  }
}

void process_timers_ex(struct list_base *expired, unsigned long now)
{
  unsigned long elapsed;
  if (time_before(now, ctx.base_time))
    return;
  elapsed = (unsigned long)((long)now - (long)ctx.base_time);
  process_buckets(&ctx, expired, elapsed);
}

void process_timers(unsigned long now)
{
  struct list_base expired;
  struct list_base *item, *tmp;
  list_init(&expired);
  process_timers_ex(&expired, now);
  list_for_each_safe(&expired, item, tmp) {
    struct timer_list *timer = container_of(item, struct timer_list, list);
    if (timer->callback)
      timer->callback(timer->data);
  }
}

/*
 * 0 - 63
 * 64 - 4096
 * 4095 - 262143
 * 262144 - 16777215
 * 16777216 - 1073741823
 */

static unsigned long do_get_next_timeout()
{
  unsigned long timeout = 0;
  struct timer_bucket *b = ctx.buckets;
  while (b) {
    unsigned order = b->order;
    unsigned cur = b->cur;
    unsigned long forward = 1 << (order * BUCKET_BITS);
    int empty = bucket_empty(b);
    if (empty) {
      b = b->next;
      timeout = (forward << BUCKET_BITS) - 1;
    }
    else {
      while (1) {
        if (list_empty(&b->v[cur]))
          timeout += forward;
        else
          return timeout;
        cur = (cur + 1) & BUCKET_MASK;
        if (cur == 0) {
          b = 0;
          break;
        }
      }
    }
  }
  return timeout;
}

unsigned long get_next_timeout(unsigned long now)
{
  unsigned long timeout = do_get_next_timeout();
  if (time_before(now, ctx.base_time))
      return timeout + (unsigned long)(ctx.base_time - now);
  else {
      unsigned long diff = (unsigned long)(now - ctx.base_time);
      if (diff > timeout)
        return 0;
      else
        return timeout - diff;
  }
}

static void init_bucket(struct timer_bucket *b, struct timer_bucket *next, unsigned order)
{
  int i;
  b->cur = 0;
  b->next = next;
  b->order = order;
  for (i = 0; i < BUCKET_SIZE; i++)
    list_init(&b->v[i]);
}

void init_timers(unsigned long now)
{
  unsigned i;
  ctx.base_time = now;
  for (i = 0; i < BUCKET_COUNT; i++)
    init_bucket(&ctx.buckets[i], i < (BUCKET_COUNT - 1) ? &ctx.buckets[i+1] : 0, i);
}
