#include "ltimer.h"
#include <assert.h>
#include <limits.h>

static long hit = 0;
static long hit_sum = 0;
struct timer_context ctx;

static void timer_callback_test_1(struct timer_list* timer, void *data)
{
  hit = (long)data;
  hit_sum += hit;
}

/*
 * 0 - 63                (64)
 * 64 - 4095             (64*64)
 * 4096 - 262143         (64*64*64)
 * 262144 - 16777215     (64*64*64*64)
 * 16777216 - 1073741823 (64*64*64*64*64)
 */

static int test_1()
{
  struct timer_list timer1;
  struct timer_list timer2;
  struct timer_list timer3;
  unsigned long t, t1;
  int r;

  init_timers(&ctx, 0);
  t = t1 = 0;
  while(t1 < 30000000) {
      hit = 0;
      set_timer(&timer1, t1 + 60000, timer_callback_test_1, (void*)1);
      add_timer(&ctx, &timer1);
      t = get_next_timeout(&ctx, t1);
      assert(t==60000);
      t1 += t;
      process_timers(&ctx, t1);
      assert(hit);
  }

  hit = 0;
  init_timers(&ctx, 100);
  set_timer(&timer1, 99, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 100);
  set_timer(&timer1, 99, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);

  init_timers(&ctx, 100);
  t = get_next_timeout(&ctx, 100);
  assert(t == (((1UL << 30) - 1)));

  set_timer(&timer1, 110, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  del_timer(&timer1);

  t = get_next_timeout(&ctx, 100);
  assert(t == 10);

  set_timer(&timer1, 110, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 101);
  assert(t == 9);

  t = get_next_timeout(&ctx, 120);
  assert(t == 0);

  t = get_next_timeout(&ctx, 99);
  assert(t == 11);

  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100+64, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == 64);
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 200+64, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == 164);
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100 + 64*64+100, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == 64*64+100);
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100 + 64*64+64*64+100, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == (64*64+64*64+100));
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100 + 64*64*64+100, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == (64*64*64+100));
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100 + 64*64*64, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == (64*64*64));
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100 + 64*64*64*64, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == (64*64*64*64));
  del_timer(&timer1);

  init_timers(&ctx, 100);
  set_timer(&timer1, 100 + 64*64*64*64*64, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 100);
  assert(t == (((1UL << 30) - 1)));
  del_timer(&timer1);

  init_timers(&ctx, 0);
  set_timer(&timer1, LONG_MAX, timer_callback_test_1, 0);
  r = add_timer(&ctx, &timer1);
  assert(r != 0);

  process_timers(&ctx, 100);
  process_timers(&ctx, 200);

  set_timer(&timer1, 200+10, timer_callback_test_1, 0);
  add_timer(&ctx, &timer1);

  t = get_next_timeout(&ctx, 200);
  assert(t == 10);

  process_timers(&ctx, 300);

  init_timers(&ctx, 0);
  hit = 0;
  set_timer(&timer1, 0, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 0);
  assert(hit == 2);

  init_timers(&ctx, 0);
  hit = 0;
  set_timer(&timer1, 0, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 0);
  assert(hit == 2);

  init_timers(&ctx, 0);
  set_timer(&timer1, 64, timer_callback_test_1, (void*)1234);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 64);
  assert(hit == 1234);

  init_timers(&ctx, 0);
  set_timer(&timer1, 0, timer_callback_test_1, (void*)12345);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 1);
  assert(hit == 12345);

  init_timers(&ctx, 0);
  set_timer(&timer1, 0, timer_callback_test_1, (void*)12346);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 63);
  assert(hit == 12346);

  init_timers(&ctx, 0);
  set_timer(&timer1, 0, timer_callback_test_1, (void*)12346);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 1 << 30);
  assert(hit == 12346);

  init_timers(&ctx, 0);
  hit_sum = 0;
  set_timer(&timer1, 63, timer_callback_test_1, (void*)1234);
  add_timer(&ctx, &timer1);
  set_timer(&timer2, 64, timer_callback_test_1, (void*)1234);
  add_timer(&ctx, &timer2);
  process_timers(&ctx, 64);
  assert(hit_sum == 1234*2);

  init_timers(&ctx, 0);
  hit_sum = 0;
  set_timer(&timer1, 62, timer_callback_test_1, (void*)1234);
  add_timer(&ctx, &timer1);
  set_timer(&timer2, 63, timer_callback_test_1, (void*)1234);
  add_timer(&ctx, &timer2);
  set_timer(&timer3, 64, timer_callback_test_1, (void*)1234);
  add_timer(&ctx, &timer3);
  process_timers(&ctx, 64);
  assert(hit_sum == 1234*3);

  init_timers(&ctx, 0);
  hit = hit_sum = 0;
  process_timers(&ctx, 60);
  process_timers(&ctx, 0);
  set_timer(&timer1, 68, timer_callback_test_1, (void*)1);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, 60);
  assert(t == 8);
  t = get_next_timeout(&ctx, 0);
  assert(t == 68);

  init_timers(&ctx, 0);
  hit = hit_sum = 0;
  process_timers(&ctx, 63);
  set_timer(&timer1, 63, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  set_timer(&timer2, 64, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer2);
  process_timers(&ctx, 4096);
  assert(hit_sum == 4);

  init_timers(&ctx, 0);
  hit = hit_sum = 0;
  set_timer(&timer1, 4, 0, 0);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 4);

  init_timers(&ctx, 0);
  hit = hit_sum = 0;
  set_timer(&timer1, 20000, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 26000);
  assert(hit == 2);

  hit = hit_sum = 0;
  set_timer(&timer1, 26064, timer_callback_test_1, (void*)3);
  add_timer(&ctx, &timer1);
  process_timers(&ctx, 30000);
  assert(hit == 3);

  t1 = t = 0;
  init_timers(&ctx, t1);
  hit = hit_sum = 0;
  set_timer(&timer1, t1 + 150, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, t1);
  t1 += t;
  process_timers(&ctx, t1);
  t = get_next_timeout(&ctx, t1);
  t1 += t;
  process_timers(&ctx, t1);
  //t = get_next_timeout(&ctx, t1);
  //t1 += t;
  hit = hit_sum = 0;
  set_timer(&timer1, t1 + 150, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, t1);
  t1 += t;
  process_timers(&ctx, t1);
  t = get_next_timeout(&ctx, t1);
  t1 += t;
  process_timers(&ctx, t1);

  hit = hit_sum = 0;
  set_timer(&timer1, t1 + 150, timer_callback_test_1, (void*)2);
  add_timer(&ctx, &timer1);
  t = get_next_timeout(&ctx, t1);
  t1 += t;
  process_timers(&ctx, t1);
  t = get_next_timeout(&ctx, t1);
  t1 += t;
  process_timers(&ctx, t1);

  return 1;
}

int main(int argc, char *argv[])
{
  assert(test_1());
  return 0;
}
