#ifndef TIMER_WHEEL_H
#define TIMER_WHEEL_H

#include <time.h>

#ifndef container_of
#ifndef offsetof
#define offsetof(st, m) ((size_t)(&((st *)0)->m))
#endif
#define container_of(ptr, type, member) ({ \
                const typeof( ((type *)0)->member ) *__mptr = (ptr); \
                (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define list_for_each_safe(head, item, tmp) \
    for (item = (head)->next, tmp = (item)->next; (item) != (head); (item) = (tmp), (tmp) = (item)->next)

typedef void (*timer_callback)(void *data);

struct list_base
{
  struct list_base *next;
  struct list_base *prev;
};

struct timer_list
{
  timer_callback callback;
  void *data;
  unsigned long expire;
  struct list_base list;
};

static inline void list_init(struct list_base *l)
{
  l->next = l;
  l->prev = l;
}

void init_timers(unsigned long now);

int set_timer(struct timer_list *timer, unsigned long expire, timer_callback callback, void *data);
int add_timer(struct timer_list *timer);
void del_timer(struct timer_list *timer);

unsigned long get_next_timeout(unsigned long now);
void process_timers(unsigned long now);
void process_timers_ex(struct list_base *expired, unsigned long now);

#endif /* TIMER_WHEEL_H */
