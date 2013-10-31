#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/timex.h>
#include <sys/time.h>
#include <math.h>
#include "lua.h"
#include "lauxlib.h"

#define DEBUG 1

#if DEBUG
#  include <syslog.h>
#  define dprint(...) syslog(LOG_DEBUG, __VA_ARGS__)
#else
#  define dprint(...) do {} while(0)
#endif

#define ADJ_OFFSET_SS_READ      0xa001  /* read-only adjtime */

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))

#define NTP_OK              0
#define NTP_ERR_SYS         (-1)
#define NTP_ERR_IGNORED     (-2)

#define PPM2FREQ(x)             ((x) * 65536)

#define PRECISION               -18     /* precision (log2 s)  */
#define NSTAGE                  8       /* clock register stages */
#define PPM_SCALE               1000000
#define USEC_IN_SEC             1000000
#define CLOCKADJ_MIN_PERIOD_SEC (900)
#define PHI                     (15 / PPM_SCALE)
#define MAX_FREQADJ             PPM2FREQ(2)
#define SGATE                   3
#define PGATE                   4.0
#define MINPOLL                 6       /* % minimum poll interval (64 s)*/
#define MAXPOLL                 17      /* % maximum poll interval (36.4 h) */
#define MAXDISPERSE             16
#define MINDISPERSE             .001
#define ALLAN                   11
#define MAXDISTANCE             1.5
#define FWEIGHT                 0.5
#define CLOCK_LIMIT             30
#define CLOCK_MAX               30

/*
 * Arithmetic conversions
 */
#define LOG2D(a)        ((a) < 0 ? 1. / (1L << -(a)) : 1L << (a))          /* poll, etc. */
#define ULOG2D(a)       (1L << (int)(a))
#define SQUARE(x)       ((x) * (x))
#define SQRT(x)         (sqrt(x))

struct f {
	double  t;              /* update time */
	double  offset;         /* clock ofset */
	double  delay;          /* roundtrip delay */
	double  disp;           /* dispersion */
} f;

struct ff {
	double offset;
	double update;
	double disp[NSTAGE];
	int dispi;
};

struct p {
	double   t;
	double   update;
	struct f f[NSTAGE];     /* clock filter */
	int      nextf;
	struct ff ff;
	double   offset;         /* peer offset */
	double   effective_offset;
	double   delay;          /* peer delay */
	double   disp;           /* peer dispersion */
	double   jitter;         /* RMS jitter */
	char     poll;           /* poll interval */
};

struct c {
	double now;
	long   freq;
};

static struct p peer;
static struct c clock;
static int      tc_counter;

static int gettime(void)
{
	struct timeval tv;
	if (gettimeofday(&tv, 0))
		return NTP_ERR_SYS;
	clock.now = (double)tv.tv_sec + (double)tv.tv_usec / USEC_IN_SEC;
	return NTP_OK;
}

static int settime(double t)
{
	struct timeval tv;
	tv.tv_sec = (time_t)t;
	tv.tv_usec = (suseconds_t)((t - tv.tv_sec) * USEC_IN_SEC);
	if (settimeofday(&tv, 0))
		return NTP_ERR_SYS;
	clock.now = t;
	dprint("settime: t:%f\n", t);
	return NTP_OK;
}

static int movetime(double offset)
{
	double new_time;
	int ret;
	dprint("movetime: offset:%f\n", offset);
	if (offset == 0)
		return NTP_OK;
#if DEBUG
	ret = gettime();
	if (ret)
		return ret;
#endif
	new_time = clock.now + offset;
	ret = settime(new_time);
	if (ret != NTP_OK)
		return ret;
	return NTP_OK;
}

static void peer_clear(struct p *p)
{
	int i;
	memset(p, 0, sizeof(struct p));
	p->update = clock.now;
	for (i = 0; i < NSTAGE; i++) {
		p->f[i].disp = MAXDISPERSE;
		p->ff.disp[i] = MAXDISPERSE;
	}
	p->poll = MINPOLL;
	p->jitter = LOG2D(PRECISION);
	p->disp = MAXDISPERSE;
}

static int freq_adjust(struct p *p)
{
	long freq_adj;
	double period, offset, threshold, drift;
	double disp = 0;
	int i, j;
	struct timex tx;
	dprint("freq_adjust: p->offset:%f\n", p->offset);

	if (p->ff.update == 0) {
		p->ff.update = clock.now;
		return NTP_OK;
	}

	p->ff.offset += p->offset;

	j = p->ff.dispi;
	p->ff.disp[j] = p->disp;
	j = (j + 1) % NSTAGE;
	p->ff.dispi = j;
	for (i = 0; i < NSTAGE; i++)
		disp += p->ff.disp[i];
	disp /= NSTAGE;

	period = clock.now - p->ff.update;
        drift = p->ff.offset / period * PPM_SCALE;

	if (disp > MAXDISTANCE)
		return NTP_OK;

	offset = p->ff.offset;
	threshold = disp * 1.5;

	dprint("freq_adjust: ff->offset:%f, offset:%f, threshold:%f, disp:%f, p->disp:%f, drift:%f\n",
	                                 p->ff.offset, offset, threshold, disp, p->disp, drift);

	if (fabs(offset) <= threshold)
		return NTP_OK;

	dprint("freq_adjust: period:%f, offset:%f\n", period, offset);
	
	freq_adj = (long)PPM2FREQ(drift * PPM_SCALE);
	if (freq_adj > MAX_FREQADJ)
		freq_adj = MAX_FREQADJ;
	else if (freq_adj < -MAX_FREQADJ)
		freq_adj = -MAX_FREQADJ;
	dprint("freq_adjust: freq_adj:%li\n", freq_adj);
	
	p->ff.offset = 0;
	p->ff.update = clock.now;
	
	memset(&tx, 0, sizeof(tx));
	tx.modes = ADJ_FREQUENCY;
	tx.freq = clock.freq + freq_adj;
	if (adjtimex(&tx) >= 0) {
		dprint("freq_adjust: adjtimex freq:%li\n", tx.freq);
		clock.freq = tx.freq;
		return NTP_OK;
	}
	else
		return NTP_ERR_SYS;
}

static int get_old_offset(double *offset)
{
	struct timex tx;
	memset(&tx, 0, sizeof(tx));
	tx.modes = ADJ_OFFSET_SS_READ;
	if (adjtimex(&tx) < 0) return NTP_ERR_SYS;
	*offset = (double)tx.offset / USEC_IN_SEC;
	return NTP_OK;
}

static int clock_adjust(struct p *p)
{
	int i;
	int ret = NTP_OK;
	double old;
	double offset;
	struct timex tx;
	dprint("clock_adjust: p->offset:%f\n", p->offset);
	if (p->offset < CLOCK_MAX) {
		memset(&tx, 0, sizeof(tx));
		tx.modes = ADJ_OFFSET_SINGLESHOT;
		ret = get_old_offset(&old);
		if (ret) return ret;
		offset = p->offset + old;
		tx.offset = (long)(offset * USEC_IN_SEC);
		if (adjtimex(&tx) < 0) return NTP_ERR_SYS;
		for (i = 0; i < NSTAGE; i++)
			if (p->f[i].t > p->t && p->f[i].disp < MAXDISPERSE)
				p->f[i].offset -= p->offset;
		ret = freq_adjust(p);
		dprint("clock_adjust: freq_adjust_ret:%i\n", ret);
	}
	else {
		ret = movetime(p->offset);
		offset = p->offset;
		peer_clear(p);
		if (ret) return ret;
	}

	p->effective_offset = offset;
	return ret;
}

static double root_distance(struct p *p)
{
	double dtemp;
	dtemp = p->delay / 2 + p->disp + p->jitter;
	if (dtemp < MINDISPERSE)
		dtemp = MINDISPERSE;
	return dtemp;
}

/*
 * clock_filter(p, offset, delay, dispersion) - select the best from the
 * latest eight delay/offset samples.
 */
static int clock_filter(struct p *p, double offset, double delay, double disp)
{
	double   dst[NSTAGE];
	int      ord[NSTAGE];
	double   dtemp, etemp;
	int      i, j, k, m, ret;

	ret = get_old_offset(&dtemp);
	if (ret != NTP_OK) return ret;
	dprint("clock_filter: delay:%f, offset:%f, old_offset:%f, real_offset:%f, disp:%f\n", 
	       delay, offset, dtemp, (offset - dtemp), disp);
	offset -= dtemp;

	j = p->nextf;
	p->f[j].offset = offset;
	p->f[j].delay = delay;
	p->f[j].disp = disp;
	p->f[j].t = clock.now;
	j = (j + 1) % NSTAGE;
	p->nextf = j;

	dtemp = PHI * (clock.now - p->update);
	p->update = clock.now;
	for (i = NSTAGE - 1; i >= 0; i--) {
		if (i != 0)
			p->f[j].disp += dtemp;
		if (p->f[j].disp >= MAXDISPERSE) {
			p->f[j].disp = MAXDISPERSE;
			dst[i] = MAXDISPERSE;
		} else if (p->update - p->f[j].t > ULOG2D(ALLAN)) {
			dst[i] = p->f[j].delay + p->f[j].disp;
		} else {
			dst[i] = p->f[j].delay;
		}
		ord[i] = j;
		j = (j + 1) % NSTAGE;
	}

	for (i = 1; i < NSTAGE; i++) {
		for (j = 0; j < i; j++) {
			if (dst[j] > dst[i]) {
				k = ord[j];
				ord[j] = ord[i];
				ord[i] = k;
				etemp = dst[j];
				dst[j] = dst[i];
				dst[i] = etemp;
			}
		}
	}

	m = 0;
	for (i = 0; i < NSTAGE; i++) {
		/*p->f[i].order = (u_char) ord[i];*/
		if (dst[i] >= MAXDISPERSE || (m >= 2 && dst[i] >= MAXDISTANCE))
			continue;
		m++;
	}

	dprint("clock_filter: m:%i\n", m);

	p->disp = p->jitter = 0;
	k = ord[0];
	for (i = NSTAGE - 1; i >= 0; i--) {
		j = ord[i];
		p->disp = FWEIGHT * (p->disp + p->f[j].disp);
		if (i < m)
			p->jitter += SQUARE(p->f[j].offset - p->f[k].offset);
	}

	if (m == 0) {
		return NTP_ERR_IGNORED;
	}

	etemp = fabs(p->offset - p->f[k].offset);
	p->offset = p->f[k].offset;
	p->delay = p->f[k].delay;
	if (m > 1)
		p->jitter /= m - 1;
	p->jitter = max(SQRT(p->jitter), LOG2D(PRECISION));
	
	dprint("clock_filter: p->offset:%f, p->disp:%f, p->jitter:%f\n", p->offset, p->disp, p->jitter);
	
	if (p->disp < MAXDISTANCE && 
	    p->f[k].disp < MAXDISTANCE && 
	    etemp > SGATE * p->jitter &&
	    p->f[k].t - p->t < 2.0 * ULOG2D(p->poll)) {
		return NTP_ERR_IGNORED;
	}

	if (p->f[k].t <= p->t) {
		return NTP_ERR_IGNORED;
	}
	p->t = p->f[k].t;

	if (root_distance(p) >= MAXDISTANCE + PHI * ULOG2D(p->poll))
		return NTP_ERR_IGNORED;

	if (fabs(p->offset) < PGATE * p->jitter) {
		dprint("clock_filter: p->poll++\n");
		tc_counter += p->poll;
		if (tc_counter > CLOCK_LIMIT) {
			tc_counter = CLOCK_LIMIT;
			if (p->poll < MAXPOLL) {
				tc_counter = 0;
				p->poll++;
			}
		}
	} else {
		dprint("clock_filter: p->poll--\n");
		tc_counter -= p->poll << 1;
		if (tc_counter < -CLOCK_LIMIT) {
			tc_counter = -CLOCK_LIMIT;
			if (p->poll > MINPOLL) {
				tc_counter = 0;
				p->poll--;
			}
		}
	}

	dprint("clock_filter: p->poll:%i, tc_counter:%i\n", p->poll, tc_counter);

	return clock_adjust(p);
}

static int ntp_process(struct p *p, const double *t, int force)
{
	double delay = 0;
	double offset = 0;
	double disp = MAXDISPERSE;
	int ret;

	ret = gettime();
	if (ret)
		return ret;

	if (t) {
		delay = (t[3] - t[0]) - (t[2] - t[1]);
		offset = ((t[1] - t[0]) + (t[2] - t[3]))/2.0;
		disp = (delay / 2) + (t[3] - t[0]) * PHI + LOG2D(PRECISION);
		dprint("ntp_process: offset:%f, delay:%f, disp:%f, force:%i\n", offset, delay, disp, force);
	}

		/*if (fabs(offset) < disp)
			return NTP_ERR_IGNORED;
		if (offset > 0)
			offset -= disp;
		else
			offset += disp;*/
	return clock_filter(p, offset, delay, disp);
}

static int ntp_init(void)
{
	int ret = gettime();
	if (ret) return ret;
	peer_clear(&peer);
	return NTP_OK;
}

static int ntp_pushresult (lua_State *L, int i) {
  int en = errno;  /* calls to Lua API may change this value */
  switch(i) {
	case NTP_OK:
		lua_pushboolean(L, 1);
    return 1;
	case NTP_ERR_SYS:
		lua_pushnil(L);
		lua_pushstring(L, "error");
		lua_pushfstring(L, "System error: %i", en);
		return 3;
	case NTP_ERR_IGNORED:
		lua_pushnil(L);
		lua_pushstring(L, "ignored");
		return 2;
	default:
		lua_pushnil(L);
		lua_pushstring(L, "error");
		lua_pushstring(L, "Unknown error");
		return 3;
	}
}

static int l_clock_update(lua_State *L)
{
	double t[4];
	if (lua_gettop(L) < 1)
		return ntp_pushresult(L, ntp_process(&peer, 0, 0));
	else if (lua_gettop(L) < 4) {
		int ret;
		t[0] = luaL_checknumber(L, 1);
		ret = settime(t[0]);
		if (ret)
			return ntp_pushresult(L, ret);
		peer_clear(&peer);
		return ntp_pushresult(L, NTP_OK);
	}
	t[0] = luaL_checknumber(L, 1);
	t[1] = luaL_checknumber(L, 2);
	t[2] = luaL_checknumber(L, 3);
	t[3] = luaL_checknumber(L, 4);
	return ntp_pushresult(L, ntp_process(&peer, t, 0));
}

static int l_get_peer(lua_State *L)
{
	lua_newtable(L);
	lua_pushnumber(L, peer.effective_offset); lua_setfield(L, -2, "offset");
	lua_pushnumber(L, peer.disp);   lua_setfield(L, -2, "disp");
	lua_pushnumber(L, peer.jitter); lua_setfield(L, -2, "jitter");
	lua_pushnumber(L, peer.poll);   lua_setfield(L, -2, "poll");
	lua_pushnumber(L, peer.update); lua_setfield(L, -2, "update");
	lua_pushnumber(L, peer.delay);  lua_setfield(L, -2, "delay");
	return 1;
}

static int l_set_peer(lua_State *L)
{
	double value;
	luaL_argcheck(L, lua_istable(L, 1), 1, "table expected");

	peer_clear(&peer);

	lua_getfield(L, 1, "poll");
	value = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	peer.poll = (long)value;

	return 0;
}

static int l_get_clock(lua_State *L)
{
	lua_newtable(L);
	gettime();
	lua_pushnumber(L, clock.now);    lua_setfield(L, -2, "now");
	lua_pushnumber(L, clock.freq);   lua_setfield(L, -2, "freq");
	return 1;
}

static int l_set_clock(lua_State *L)
{
	double value;
	luaL_argcheck(L, lua_istable(L, 1), 1, "table expected");

	lua_getfield(L, 1, "now");
	value = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	settime(value);

	lua_getfield(L, 1, "freq");
	value = luaL_checknumber(L, -1);
	lua_pop(L, 1);
	clock.freq = (long)value;

	return 0;
}

static int l_clear()
{
  ntp_init();
  return 0;
}

static const struct luaL_Reg mylib [] = {
	{ "clock_update", l_clock_update },
	{ "get_peer", l_get_peer },
	{ "set_peer", l_set_peer },
	{ "get_clock", l_get_clock },
	{ "set_clock", l_set_clock },
	{ "clear", l_clear },
	{ NULL, NULL }
};

int luaopen_ntp(lua_State *L)
{
	ntp_init();
	luaL_register(L, "ntp", mylib);
	return 1;
}
