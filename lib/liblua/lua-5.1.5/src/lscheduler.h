#ifndef lscheduler_h
#define lscheduler_h

#ifndef WIN32
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/times.h>
#include <syslog.h>
#else
#include <windows.h>
#include <time.h>
#endif

#include "lua.h"
#include "ltimer.h"

struct wait_ctx {
#ifndef WIN32
  struct epoll_event *event;
#else
  OVERLAPPED ov;
#endif
  struct timer_list timer;
  int timeout;
  int cancel;
  lua_State *L_thread;
  int fd;
  int suspended;
};

#endif /* lscheduler_h */