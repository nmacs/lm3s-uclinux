/*=========================================================================*\
* Socket compatibilization module for Win32
* LuaSocket toolkit
*
* The penalty of calling select to avoid busy-wait is only paid when
* the I/O call fail in the first place. 
*
* RCS ID: $Id: wsocket.c,v 1.36 2007/06/11 23:44:54 diego Exp $
\*=========================================================================*/

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>

#include <string.h>
#include <lauxlib.h>

#include "socket.h"

//#define DEBUG 1

#if DEBUG
//#  include <syslog.h>
#  define dprint(s, ...) { fprintf(stderr, s, ##__VA_ARGS__); /*usleep(10000);*/ }
//#  define dprint(s, ...) { fprintf(stderr, s, ##__VA_ARGS__); usleep(100000); }
#else
#  define dprint(...) while(0) {}
#endif


#ifdef SOCKET_SCHEDULER
static void *
get_extension_function(SOCKET s, const GUID *which_fn)
{
        void *ptr = NULL;
        DWORD bytes=0;
        WSAIoctl(s, SIO_GET_EXTENSION_FUNCTION_POINTER,
            (GUID*)which_fn, sizeof(*which_fn),
            &ptr, sizeof(ptr),
            &bytes, NULL, NULL);

        /* No need to detect errors here: if ptr is set, then we have a good
           function pointer.  Otherwise, we should behave as if we had no
           function pointer.
        */
        return ptr;
}

/* Mingw doesn't have these in its mswsock.h.  The values are copied from
   wine.h.   Perhaps if we copy them exactly, the cargo will come again.
*/
#ifndef WSAID_ACCEPTEX
#define WSAID_ACCEPTEX \
        {0xb5367df1,0xcbac,0x11cf,{0x95,0xca,0x00,0x80,0x5f,0x48,0xa1,0x92}}
#endif
#ifndef WSAID_CONNECTEX
#define WSAID_CONNECTEX \
        {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}}
#endif

/* Mingw's headers don't define LPFN_ACCEPTEX. */

typedef BOOL (WINAPI *AcceptExPtr)(SOCKET, SOCKET, PVOID, DWORD, DWORD, DWORD, LPDWORD, LPOVERLAPPED);
typedef BOOL (WINAPI *ConnectExPtr)(SOCKET, const struct sockaddr *, int, PVOID, DWORD, LPDWORD, LPOVERLAPPED);
typedef void (WINAPI *GetAcceptExSockaddrsPtr)(PVOID, DWORD, DWORD, DWORD, LPSOCKADDR *, LPINT, LPSOCKADDR *, LPINT);

/** Internal use only. Holds pointers to functions that only some versions of
Windows provide.
*/
struct win32_extension_fns {
        AcceptExPtr AcceptEx;
        ConnectExPtr ConnectEx;
        GetAcceptExSockaddrsPtr GetAcceptExSockaddrs;
};

static void
init_extension_functions(struct win32_extension_fns *ext)
{
        const GUID acceptex = WSAID_ACCEPTEX;
        const GUID connectex = WSAID_CONNECTEX;
        const GUID getacceptexsockaddrs = WSAID_GETACCEPTEXSOCKADDRS;
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        if (s == INVALID_SOCKET)
                return;
        ext->AcceptEx = get_extension_function(s, &acceptex);
        ext->ConnectEx = get_extension_function(s, &connectex);
        ext->GetAcceptExSockaddrs = get_extension_function(s,
            &getacceptexsockaddrs);
        closesocket(s);
}

static struct win32_extension_fns ext;

#endif

/* WinSock doesn't have a strerror... */
static const char *wstrerror(int err);

/*-------------------------------------------------------------------------*\
* Initializes module 
\*-------------------------------------------------------------------------*/
int socket_open(void) {
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 0); 
    int err = WSAStartup(wVersionRequested, &wsaData );
    if (err != 0) return 0;
    if ((LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 0) &&
        (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)) {
        WSACleanup();
        return 0; 
    }
#ifdef SOCKET_SCHEDULER
    init_extension_functions(&ext);
#endif
    return 1;
}

/*-------------------------------------------------------------------------*\
* Close module 
\*-------------------------------------------------------------------------*/
int socket_close(void) {
    WSACleanup();
    return 1;
}

/*-------------------------------------------------------------------------*\
* Wait for readable/writable/connected socket with timeout
\*-------------------------------------------------------------------------*/
#define WAITFD_R        1
#define WAITFD_W        2
#define WAITFD_E        4
#define WAITFD_C        (WAITFD_E|WAITFD_W)

int socket_waitfd(p_socket ps, int sw, p_timeout tm) {
    int ret;
    fd_set rfds, wfds, efds, *rp = NULL, *wp = NULL, *ep = NULL;
    struct timeval tv, *tp = NULL;
    double t;
    if (timeout_iszero(tm)) return IO_TIMEOUT;  /* optimize timeout == 0 case */
    if (sw & WAITFD_R) { 
        FD_ZERO(&rfds); 
		FD_SET(*ps, &rfds);
        rp = &rfds; 
    }
    if (sw & WAITFD_W) { FD_ZERO(&wfds); FD_SET(*ps, &wfds); wp = &wfds; }
    if (sw & WAITFD_C) { FD_ZERO(&efds); FD_SET(*ps, &efds); ep = &efds; }
    if ((t = timeout_get(tm)) >= 0.0) {
        tv.tv_sec = (int) t;
        tv.tv_usec = (int) ((t-tv.tv_sec)*1.0e6);
        tp = &tv;
    }
    ret = select(0, rp, wp, ep, tp);
    if (ret == -1) return WSAGetLastError();
    if (ret == 0) return IO_TIMEOUT;
    if (sw == WAITFD_C && FD_ISSET(*ps, &efds)) return IO_CLOSED;
    return IO_DONE;
}

/*-------------------------------------------------------------------------*\
* Select with int timeout in ms
\*-------------------------------------------------------------------------*/
int socket_select(t_socket n, fd_set *rfds, fd_set *wfds, fd_set *efds, 
        p_timeout tm) {
    struct timeval tv; 
    double t = timeout_get(tm);
    tv.tv_sec = (int) t;
    tv.tv_usec = (int) ((t - tv.tv_sec) * 1.0e6);
    if (n <= 0) {
        Sleep((DWORD) (1000*t));
        return 0;
    } else return select(0, rfds, wfds, efds, t >= 0.0? &tv: NULL);
}

/*-------------------------------------------------------------------------*\
* Close and inutilize socket
\*-------------------------------------------------------------------------*/
void socket_destroy(p_socket ps) {
    if (*ps != SOCKET_INVALID) {
        socket_setblocking(ps); /* close can take a long time on WIN32 */
        closesocket(*ps);
        *ps = SOCKET_INVALID;
    }
}

/*-------------------------------------------------------------------------*\
* 
\*-------------------------------------------------------------------------*/
void socket_shutdown(p_socket ps, int how) {
    socket_setblocking(ps);
    shutdown(*ps, how);
    socket_setnonblocking(ps);
}

/*-------------------------------------------------------------------------*\
* Creates and sets up a socket
\*-------------------------------------------------------------------------*/
int socket_create(p_socket ps, int domain, int type, int protocol) {
    *ps = socket(domain, type, protocol);
    if (*ps != SOCKET_INVALID) return IO_DONE;
    else return WSAGetLastError();
}

/*-------------------------------------------------------------------------*\
* Connects or returns error message
\*-------------------------------------------------------------------------*/
int socket_connect(p_socket ps, SA *addr, socklen_t len, p_timeout tm) {
#ifdef SOCKET_SCHEDULER
  lua_State *L;
  BOOL r;
  struct wait_ctx ctx;
  int t;
  int ret;
#endif
  int err;
  /* don't call on closed socket */
  if (*ps == SOCKET_INVALID) return IO_CLOSED;
#ifdef SOCKET_SCHEDULER
  L = lua_this();
  luaL_addfd(L, *ps);
  memset(&ctx, 0, sizeof(ctx));
  dprint("connect ov:%p\n", &ctx.ov);
  if (!ext.ConnectEx(*ps, addr, len, NULL, 0, NULL, &ctx.ov)) {
      err = WSAGetLastError();
      if (err != WSA_IO_PENDING)
          return err > 0 ? err: IO_UNKNOWN;
  }
  t = (int)(timeout_getretry(tm)*1e3);
  ret = luaL_wait(L, *ps, 1, t, &ctx);
  if (ret == -1) {
      int len = sizeof(err);
      /* find out why we failed */
      getsockopt(*ps, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
      /* we KNOW there was an error. if 'why' is 0, we will return
      * "unknown error", but it's not really our fault */
      return err > 0 ? err: IO_UNKNOWN;
  }
  if (ret == 0) {
      socket_destroy(ps);
      return IO_TIMEOUT;
  }
  return IO_DONE;
#else
  /* ask system to connect */
  if (connect(*ps, addr, len) == 0) return IO_DONE;
  /* make sure the system is trying to connect */
  err = WSAGetLastError();
  if (err != WSAEWOULDBLOCK && err != WSAEINPROGRESS) return err;
  /* zero timeout case optimization */
  if (timeout_iszero(tm)) return IO_TIMEOUT;
  /* we wait until something happens */
  err = socket_waitfd(ps, WAITFD_C, tm);
  if (err == IO_CLOSED) {
      int len = sizeof(err);
      /* give windows time to set the error (yes, disgusting) */
      Sleep(10);
      /* find out why we failed */
      getsockopt(*ps, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
      /* we KNOW there was an error. if 'why' is 0, we will return
      * "unknown error", but it's not really our fault */
      return err > 0? err: IO_UNKNOWN;
  } else return err;
#endif
}

/*-------------------------------------------------------------------------*\
* Binds or returns error message
\*-------------------------------------------------------------------------*/
int socket_bind(p_socket ps, SA *addr, socklen_t len) {
    int err = IO_DONE;
    socket_setblocking(ps);
    if (bind(*ps, addr, len) < 0) err = WSAGetLastError();
    socket_setnonblocking(ps);
    return err;
}

/*-------------------------------------------------------------------------*\
* 
\*-------------------------------------------------------------------------*/
int socket_listen(p_socket ps, int backlog) {
    int err = IO_DONE;
    socket_setblocking(ps);
    if (listen(*ps, backlog) < 0) err = WSAGetLastError();
    socket_setnonblocking(ps);
    return err;
}

/*-------------------------------------------------------------------------*\
* Accept with timeout
\*-------------------------------------------------------------------------*/
int socket_accept(p_socket ps, p_socket pa, SA *addr, socklen_t *len, 
        p_timeout tm) {
#ifdef SOCKET_SCHEDULER
    struct wait_ctx ctx;
    BOOL r;
    SOCKET s;
    char lpOutputBuf[1024];
    int outBufLen = 1024;
    int t, ret;
    DWORD dwBytes;
    int err;
    lua_State *L = lua_this();

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET)
      return IO_UNKNOWN;

    luaL_addfd(L, *ps);

    memset(&ctx, 0, sizeof(ctx));

    dprint("accept: ps:%u, pa:%u, ov:%p\n", *ps, s, &ctx.ov);

    r = ext.AcceptEx(*ps, s, lpOutputBuf,
                 0,
                 sizeof (struct sockaddr_in) + 16, sizeof (struct sockaddr_in) + 16,
                 &dwBytes, &ctx.ov);

    if (r) return IO_DONE;

    err = WSAGetLastError();
    if (err != WSA_IO_PENDING) {
          dprint("accept: error 1\n");
          closesocket(s);
          return err > 0 ? err : IO_UNKNOWN;
    }

    t = (int)(timeout_getretry(tm)*1e3);
    ret = luaL_wait(L, *ps, 0, t, &ctx);

    if (ret == -1) {
        int len = sizeof(err);
        /* find out why we failed */
        getsockopt(*ps, SOL_SOCKET, SO_ERROR, (char *)&err, &len);
        /* we KNOW there was an error. if 'why' is 0, we will return
        * "unknown error", but it's not really our fault */
        dprint("accept: error 2\n");
        closesocket(s);
        return err > 0? err: IO_UNKNOWN;
    }

    if (ret == 0) {
        dprint("accept: error 3\n");
        closesocket(s);
        socket_destroy(ps);
        return IO_TIMEOUT;
    }

    setsockopt( s, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char *)ps, sizeof(*ps) );

    dprint("accept OK\n");
    *pa = s;

    return IO_DONE;

#else
    SA daddr;
    socklen_t dlen = sizeof(daddr);
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    if (!addr) addr = &daddr;
    if (!len) len = &dlen;
    for ( ;; ) {
        int err;
        /* try to get client socket */
        if ((*pa = accept(*ps, addr, len)) != SOCKET_INVALID) return IO_DONE;
        /* find out why we failed */
        err = WSAGetLastError(); 
        /* if we failed because there was no connectoin, keep trying */
        if (err != WSAEWOULDBLOCK && err != WSAECONNABORTED) return err;
        /* call select to avoid busy wait */
        if ((err = socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    } 
    /* can't reach here */
    return IO_UNKNOWN; 
#endif
}

/*-------------------------------------------------------------------------*\
* Send with timeout
* On windows, if you try to send 10MB, the OS will buffer EVERYTHING 
* this can take an awful lot of time and we will end up blocked. 
* Therefore, whoever calls this function should not pass a huge buffer.
\*-------------------------------------------------------------------------*/
int socket_send(p_socket ps, const char *data, size_t count, 
        size_t *sent, p_timeout tm)
{
#ifdef SOCKET_SCHEDULER
    DWORD bytes;
    WSABUF buf;
    int ret, t;
    struct wait_ctx ctx;
    lua_State *L = lua_this();

    *sent = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;

    luaL_addfd(L, *ps);

    buf.buf = (char*)data;
    buf.len = count;

    memset(&ctx, 0, sizeof(ctx));

    if (WSASend(*ps, &buf, 1, &bytes, 0, (LPWSAOVERLAPPED)&ctx.ov, NULL)) {
          int err = WSAGetLastError();
          if (err != WSA_IO_PENDING) {
                dprint("send: error 1\n");
                socket_destroy(ps);
                return err > 0 ? err : IO_UNKNOWN;
          }
    }

    t = (int)(timeout_getretry(tm)*1e3);
    ret = luaL_wait(L, *ps, 0, t, &ctx);

    if (ret < 0) {
        int err;
        GetOverlappedResult((HANDLE)*ps, &ctx.ov, &bytes, FALSE);
        err = WSAGetLastError();
        dprint("send: error 2\n");
        socket_destroy(ps);
        return err > 0 ? err : IO_UNKNOWN;
    }
    if (ret == 0) {
        dprint("send: error 3\n");
        socket_destroy(ps);
        return IO_TIMEOUT;
    }

    GetOverlappedResult((HANDLE)*ps, &ctx.ov, &bytes, FALSE);
    dprint("send: bytes:%u\n", bytes);
    *sent = (size_t)bytes;

    return IO_DONE;
#else
    int err;
    *sent = 0;
    /* avoid making system calls on closed sockets */
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    /* loop until we send something or we give up on error */
    for ( ;; ) {
        /* try to send something */
		int put = send(*ps, data, (int) count, 0);
        /* if we sent something, we are done */
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        /* deal with failure */
        err = WSAGetLastError(); 
        /* we can only proceed if there was no serious error */
        if (err != WSAEWOULDBLOCK) return err;
        /* avoid busy wait */
        if ((err = socket_waitfd(ps, WAITFD_W, tm)) != IO_DONE) return err;
    } 
    /* can't reach here */
    return IO_UNKNOWN;
#endif
}

/*-------------------------------------------------------------------------*\
* Sendto with timeout
\*-------------------------------------------------------------------------*/
int socket_sendto(p_socket ps, const char *data, size_t count, size_t *sent, 
        SA *addr, socklen_t len, p_timeout tm)
{
    int err;
    *sent = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
        int put = sendto(*ps, data, (int) count, 0, addr, len);
        if (put > 0) {
            *sent = put;
            return IO_DONE;
        }
        err = WSAGetLastError(); 
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = socket_waitfd(ps, WAITFD_W, tm)) != IO_DONE) return err;
    } 
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Receive with timeout
\*-------------------------------------------------------------------------*/
int socket_recv(p_socket ps, char *data, size_t count, size_t *got, p_timeout tm) {
#if defined(SOCKET_SCHEDULER)
    WSABUF buf;
    DWORD flags = 0;
    DWORD taken;
    int err;
    int ret, t;
    struct wait_ctx ctx;
    lua_State *L = lua_this();

    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;

    dprint("recv: ov:%p\n", &ctx.ov);

    luaL_addfd(L, *ps);

    buf.buf = data;
    buf.len = count;

    memset(&ctx, 0, sizeof(ctx));

    if (WSARecv(*ps, &buf, 1, &taken, &flags, (LPWSAOVERLAPPED)&ctx.ov, NULL)) {
            err = WSAGetLastError();
            if (err != WSA_IO_PENDING) {
              dprint("recv: error 1 err:%i\n", err);
              socket_destroy(ps);
              return err > 0 ? err : IO_UNKNOWN;
            }
    }

    t = (int)(timeout_getretry(tm)*1e3);
    ret = luaL_wait(L, *ps, 0, t, &ctx);

    if (ret < 0) {
        int err;
        DWORD bytes;
        GetOverlappedResult((HANDLE)*ps, &ctx.ov, &bytes, FALSE);
        err = WSAGetLastError();
        dprint("recv: error 2\n");
        socket_destroy(ps);
        return err > 0 ? err : IO_UNKNOWN;
    }
    if (ret == 0) {
        dprint("recv: error 3\n");
        socket_destroy(ps);
        return IO_TIMEOUT;
    }

    GetOverlappedResult((HANDLE)*ps, &ctx.ov, &taken, FALSE);
    dprint("recv: taken:%u\n", taken);
    if (taken == 0)
      return IO_CLOSED;
    *got = (size_t)taken;

    return IO_DONE;
#else
    int err;
    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
        int taken = recv(*ps, data, (int) count, 0);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        if (taken == 0) return IO_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    }
    return IO_UNKNOWN;
#endif
}

/*-------------------------------------------------------------------------*\
* Recvfrom with timeout
\*-------------------------------------------------------------------------*/
int socket_recvfrom(p_socket ps, char *data, size_t count, size_t *got, 
        SA *addr, socklen_t *len, p_timeout tm) {
    int err;
    *got = 0;
    if (*ps == SOCKET_INVALID) return IO_CLOSED;
    for ( ;; ) {
        int taken = recvfrom(*ps, data, (int) count, 0, addr, len);
        if (taken > 0) {
            *got = taken;
            return IO_DONE;
        }
        if (taken == 0) return IO_CLOSED;
        err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return err;
        if ((err = socket_waitfd(ps, WAITFD_R, tm)) != IO_DONE) return err;
    }
    return IO_UNKNOWN;
}

/*-------------------------------------------------------------------------*\
* Put socket into blocking mode
\*-------------------------------------------------------------------------*/
void socket_setblocking(p_socket ps) {
    u_long argp = 0;
    ioctlsocket(*ps, FIONBIO, &argp);
}

/*-------------------------------------------------------------------------*\
* Put socket into non-blocking mode
\*-------------------------------------------------------------------------*/
void socket_setnonblocking(p_socket ps) {
    u_long argp = 1;
    ioctlsocket(*ps, FIONBIO, &argp);
}

/*-------------------------------------------------------------------------*\
* DNS helpers 
\*-------------------------------------------------------------------------*/
int socket_gethostbyaddr(const char *addr, socklen_t len, struct hostent **hp) {
    *hp = gethostbyaddr(addr, len, AF_INET);
    if (*hp) return IO_DONE;
    else return WSAGetLastError();
}

int socket_gethostbyname(const char *addr, struct hostent **hp) {
    *hp = gethostbyname(addr);
    if (*hp) return IO_DONE;
    else return  WSAGetLastError();
}

/*-------------------------------------------------------------------------*\
* Error translation functions
\*-------------------------------------------------------------------------*/
const char *socket_hoststrerror(int err) {
    if (err <= 0) return io_strerror(err);
    switch (err) {
        case WSAHOST_NOT_FOUND: return "host not found";
        default: return wstrerror(err); 
    }
}

const char *socket_strerror(int err) {
    if (err <= 0) return io_strerror(err);
    switch (err) {
        case WSAEADDRINUSE: return "address already in use";
        case WSAECONNREFUSED: return "connection refused";
        case WSAEISCONN: return "already connected";
        case WSAEACCES: return "permission denied";
        case WSAECONNABORTED: return "closed";
        case WSAECONNRESET: return "closed";
        case WSAETIMEDOUT: return "timeout";
        default: return wstrerror(err);
    }
}

const char *socket_ioerror(p_socket ps, int err) {
	(void) ps;
	return socket_strerror(err);
}

static const char *wstrerror(int err) {
    switch (err) {
        case WSAEINTR: return "Interrupted function call";
        case WSAEACCES: return "Permission denied";
        case WSAEFAULT: return "Bad address";
        case WSAEINVAL: return "Invalid argument";
        case WSAEMFILE: return "Too many open files";
        case WSAEWOULDBLOCK: return "Resource temporarily unavailable";
        case WSAEINPROGRESS: return "Operation now in progress";
        case WSAEALREADY: return "Operation already in progress";
        case WSAENOTSOCK: return "Socket operation on nonsocket";
        case WSAEDESTADDRREQ: return "Destination address required";
        case WSAEMSGSIZE: return "Message too long";
        case WSAEPROTOTYPE: return "Protocol wrong type for socket";
        case WSAENOPROTOOPT: return "Bad protocol option";
        case WSAEPROTONOSUPPORT: return "Protocol not supported";
        case WSAESOCKTNOSUPPORT: return "Socket type not supported";
        case WSAEOPNOTSUPP: return "Operation not supported";
        case WSAEPFNOSUPPORT: return "Protocol family not supported";
        case WSAEAFNOSUPPORT: 
            return "Address family not supported by protocol family"; 
        case WSAEADDRINUSE: return "Address already in use";
        case WSAEADDRNOTAVAIL: return "Cannot assign requested address";
        case WSAENETDOWN: return "Network is down";
        case WSAENETUNREACH: return "Network is unreachable";
        case WSAENETRESET: return "Network dropped connection on reset";
        case WSAECONNABORTED: return "Software caused connection abort";
        case WSAECONNRESET: return "Connection reset by peer";
        case WSAENOBUFS: return "No buffer space available";
        case WSAEISCONN: return "Socket is already connected";
        case WSAENOTCONN: return "Socket is not connected";
        case WSAESHUTDOWN: return "Cannot send after socket shutdown";
        case WSAETIMEDOUT: return "Connection timed out";
        case WSAECONNREFUSED: return "Connection refused";
        case WSAEHOSTDOWN: return "Host is down";
        case WSAEHOSTUNREACH: return "No route to host";
        case WSAEPROCLIM: return "Too many processes";
        case WSASYSNOTREADY: return "Network subsystem is unavailable";
        case WSAVERNOTSUPPORTED: return "Winsock.dll version out of range";
        case WSANOTINITIALISED: 
            return "Successful WSAStartup not yet performed";
        case WSAEDISCON: return "Graceful shutdown in progress";
        case WSAHOST_NOT_FOUND: return "Host not found";
        case WSATRY_AGAIN: return "Nonauthoritative host not found";
        case WSANO_RECOVERY: return "Nonrecoverable name lookup error"; 
        case WSANO_DATA: return "Valid name, no data record of requested type";
        default: return "Unknown error";
    }
}
