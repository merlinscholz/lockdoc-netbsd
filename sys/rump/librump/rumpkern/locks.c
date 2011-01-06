/*	$NetBSD: locks.c,v 1.45 2011/01/06 11:22:55 pooka Exp $	*/

/*
 * Copyright (c) 2007, 2008 Antti Kantee.  All Rights Reserved.
 *
 * Development of this software was supported by the
 * Finnish Cultural Foundation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: locks.c,v 1.45 2011/01/06 11:22:55 pooka Exp $");

#include <sys/param.h>
#include <sys/kmem.h>
#include <sys/mutex.h>
#include <sys/rwlock.h>

#include <rump/rumpuser.h>

#include "rump_private.h"

/*
 * Simple lockdebug.  If it's compiled in, it's always active.
 * Currently available only for mtx/rwlock.
 */
#ifdef LOCKDEBUG
#include <sys/lockdebug.h>

static lockops_t mutex_lockops = {
	"mutex",
	LOCKOPS_SLEEP,
	NULL
};
static lockops_t rw_lockops = {
	"mutex",
	LOCKOPS_SLEEP,
	NULL
};

#define ALLOCK(lock, ops)		\
    lockdebug_alloc(lock, ops, (uintptr_t)__builtin_return_address(0))
#define FREELOCK(lock)			\
    lockdebug_free(lock)
#define WANTLOCK(lock, shar, try)	\
    lockdebug_wantlock(lock, (uintptr_t)__builtin_return_address(0), shar, try)
#define LOCKED(lock, shar)		\
    lockdebug_locked(lock, NULL, (uintptr_t)__builtin_return_address(0), shar)
#define UNLOCKED(lock, shar)		\
    lockdebug_unlocked(lock, (uintptr_t)__builtin_return_address(0), shar)
#else
#define ALLOCK(a, b)
#define FREELOCK(a)
#define WANTLOCK(a, b, c)
#define LOCKED(a, b)
#define UNLOCKED(a, b)
#endif

/*
 * We map locks to pthread routines.  The difference between kernel
 * and rumpuser routines is that while the kernel uses static
 * storage, rumpuser allocates the object from the heap.  This
 * indirection is necessary because we don't know the size of
 * pthread objects here.  It is also beneficial, since we can
 * be easily compatible with the kernel ABI because all kernel
 * objects regardless of machine architecture are always at least
 * the size of a pointer.  The downside, of course, is a performance
 * penalty.
 */

#define RUMPMTX(mtx) (*(struct rumpuser_mtx **)(mtx))

void
mutex_init(kmutex_t *mtx, kmutex_type_t type, int ipl)
{

	CTASSERT(sizeof(kmutex_t) >= sizeof(void *));

	rumpuser_mutex_init_kmutex((struct rumpuser_mtx **)mtx);
	ALLOCK(mtx, &mutex_lockops);
}

void
mutex_destroy(kmutex_t *mtx)
{

	FREELOCK(mtx);
	rumpuser_mutex_destroy(RUMPMTX(mtx));
}

void
mutex_enter(kmutex_t *mtx)
{

	WANTLOCK(mtx, false, false);
	rumpuser_mutex_enter(RUMPMTX(mtx));
	LOCKED(mtx, false);
}
__strong_alias(mutex_spin_enter,mutex_enter);

int
mutex_tryenter(kmutex_t *mtx)
{
	int rv;

	rv = rumpuser_mutex_tryenter(RUMPMTX(mtx));
	if (rv) {
		WANTLOCK(mtx, false, true);
		LOCKED(mtx, false);
	}
	return rv;
}

void
mutex_exit(kmutex_t *mtx)
{

	UNLOCKED(mtx, false);
	rumpuser_mutex_exit(RUMPMTX(mtx));
}
__strong_alias(mutex_spin_exit,mutex_exit);

int
mutex_owned(kmutex_t *mtx)
{

	return mutex_owner(mtx) == curlwp;
}

struct lwp *
mutex_owner(kmutex_t *mtx)
{

	return rumpuser_mutex_owner(RUMPMTX(mtx));
}

#define RUMPRW(rw) (*(struct rumpuser_rw **)(rw))

/* reader/writer locks */

void
rw_init(krwlock_t *rw)
{

	CTASSERT(sizeof(krwlock_t) >= sizeof(void *));

	rumpuser_rw_init((struct rumpuser_rw **)rw);
	ALLOCK(rw, &rw_lockops);
}

void
rw_destroy(krwlock_t *rw)
{

	FREELOCK(rw);
	rumpuser_rw_destroy(RUMPRW(rw));
}

void
rw_enter(krwlock_t *rw, const krw_t op)
{


	WANTLOCK(rw, op == RW_READER, false);
	rumpuser_rw_enter(RUMPRW(rw), op == RW_WRITER);
	LOCKED(rw, op == RW_READER);
}

int
rw_tryenter(krwlock_t *rw, const krw_t op)
{
	int rv;

	rv = rumpuser_rw_tryenter(RUMPRW(rw), op == RW_WRITER);
	if (rv) {
		WANTLOCK(rw, op == RW_READER, true);
		LOCKED(rw, op == RW_READER);
	}
	return rv;
}

void
rw_exit(krwlock_t *rw)
{

#ifdef LOCKDEBUG
	bool shared = !rw_write_held(rw);

	if (shared)
		KASSERT(rw_read_held(rw));
	UNLOCKED(rw, shared);
#endif
	rumpuser_rw_exit(RUMPRW(rw));
}

/* always fails */
int
rw_tryupgrade(krwlock_t *rw)
{

	return 0;
}

int
rw_write_held(krwlock_t *rw)
{

	return rumpuser_rw_wrheld(RUMPRW(rw));
}

int
rw_read_held(krwlock_t *rw)
{

	return rumpuser_rw_rdheld(RUMPRW(rw));
}

int
rw_lock_held(krwlock_t *rw)
{

	return rumpuser_rw_held(RUMPRW(rw));
}

/* curriculum vitaes */

#define RUMPCV(cv) (*(struct rumpuser_cv **)(cv))

void
cv_init(kcondvar_t *cv, const char *msg)
{

	CTASSERT(sizeof(kcondvar_t) >= sizeof(void *));

	rumpuser_cv_init((struct rumpuser_cv **)cv);
}

void
cv_destroy(kcondvar_t *cv)
{

	rumpuser_cv_destroy(RUMPCV(cv));
}

void
cv_wait(kcondvar_t *cv, kmutex_t *mtx)
{

	if (__predict_false(rump_threads == 0))
		panic("cv_wait without threads");
	UNLOCKED(mtx, false);
	rumpuser_cv_wait(RUMPCV(cv), RUMPMTX(mtx));
	LOCKED(mtx, false);
}

int
cv_wait_sig(kcondvar_t *cv, kmutex_t *mtx)
{

	if (__predict_false(rump_threads == 0))
		panic("cv_wait without threads");
	UNLOCKED(mtx, false);
	rumpuser_cv_wait(RUMPCV(cv), RUMPMTX(mtx));
	LOCKED(mtx, false);
	return 0;
}

int
cv_timedwait(kcondvar_t *cv, kmutex_t *mtx, int ticks)
{
	struct timespec ts, tick;
	extern int hz;
	int rv;

	if (ticks == 0) {
		cv_wait(cv, mtx);
		rv = 0;
	} else {
		/*
		 * XXX: this fetches rump kernel time, but
		 * rumpuser_cv_timedwait uses host time.
		 */
		nanotime(&ts);
		tick.tv_sec = ticks / hz;
		tick.tv_nsec = (ticks % hz) * (1000000000/hz);
		timespecadd(&ts, &tick, &ts);

		UNLOCKED(mtx, false);
		if (rumpuser_cv_timedwait(RUMPCV(cv), RUMPMTX(mtx),
		    ts.tv_sec, ts.tv_nsec))
			rv = EWOULDBLOCK;
		else
			rv = 0;
		LOCKED(mtx, false);
	}

	return rv;
}
__strong_alias(cv_timedwait_sig,cv_timedwait);

void
cv_signal(kcondvar_t *cv)
{

	rumpuser_cv_signal(RUMPCV(cv));
}

void
cv_broadcast(kcondvar_t *cv)
{

	rumpuser_cv_broadcast(RUMPCV(cv));
}

bool
cv_has_waiters(kcondvar_t *cv)
{

	return rumpuser_cv_has_waiters(RUMPCV(cv));
}

/* this is not much of an attempt, but ... */
bool
cv_is_valid(kcondvar_t *cv)
{

	return RUMPCV(cv) != NULL;
}
