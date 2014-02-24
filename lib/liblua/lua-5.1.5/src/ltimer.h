#ifndef TIMER_WHEEL_H
#define TIMER_WHEEL_H

#include <time.h>

#define ROOT_BUCKET_BITS 8
#define BUCKET_BITS      6
#define ROOT_BUCKET_SIZE (1 << BUCKET_BITS)
#define BUCKET_SIZE      (1 << BUCKET_BITS)
#define ROOT_BUCKET_MASK (ROOT_BUCKET_SIZE - 1)
#define BUCKET_MASK      (BUCKET_SIZE - 1)
#define BUCKET_COUNT     4

#ifndef container_of
#ifndef offsetof
#define offsetof(st, m) ((size_t)(&((st *)0)->m))
#endif
#define container_of(ptr, type, member) ({ \
                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifndef list_for_each_safe
#define list_for_each_safe(head, item, tmp) \
    for (item = (head)->next, tmp = (item)->next; (item) != (head); (item) = (tmp), (tmp) = (item)->next)
#endif

#ifndef list_for_each
#define list_for_each(head, item) \
    for (item = (head)->next; (item) != (head); (item) = (item)->next)
#endif

struct list_base
{
  struct list_base *next;
  struct list_base *prev;
};

struct timer_list;

typedef void (*timer_callback)(struct timer_list* timer, void *data);

struct timer_list
{
  timer_callback callback;
  void *data;
  unsigned long expire;
  struct list_base list;
};

struct timer_bucket
{
  struct list_base v[BUCKET_SIZE];
};

struct timer_root_bucket
{
  struct list_base v[ROOT_BUCKET_SIZE];
};

struct timer_context
{
  struct timer_root_bucket root;
  struct timer_bucket buckets[BUCKET_COUNT];
  unsigned long next_timer;
  unsigned long base_time;
};

static inline void list_init(struct list_base *l)
{
  l->next = l;
  l->prev = l;
}

void init_timers(struct timer_context *ctx, unsigned long now);

int set_timer(struct timer_list *timer, unsigned long expire, timer_callback callback, void *data);
unsigned long add_timer(struct timer_context *ctx, struct timer_list *timer);
void del_timer(struct timer_list *timer);

unsigned long get_next_timeout(struct timer_context *ctx, unsigned long now);
void process_timers(struct timer_context *ctx, unsigned long now);
void process_timers_ex(struct timer_context *ctx, struct list_base *expired, unsigned long now);

#endif /* TIMER_WHEEL_H */
