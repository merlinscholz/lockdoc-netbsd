/* 	$NetBSD: initfini.c,v 1.8 2011/03/07 05:09:11 joerg Exp $	 */

/*-
 * Copyright (c) 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Andrew Doran.
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
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
__RCSID("$NetBSD: initfini.c,v 1.8 2011/03/07 05:09:11 joerg Exp $");

#ifdef _LIBC
#include "namespace.h"
#endif

#include <sys/param.h>
#include <sys/exec.h>
#include <stdbool.h>

void	_libc_init(void) __attribute__((__constructor__, __used__));

void	__guard_setup(void);
void	__libc_thr_init(void);
void	__libc_atomic_init(void);
void	__libc_atexit_init(void);
void	__libc_env_init(void);

static bool libc_initialised;

void _libc_init(void);

__dso_hidden void	*__auxinfo;
struct ps_strings *__ps_strings;

/*
 * _libc_init is called twice.  The first time explicitly by crt0.o
 * (for newer versions) and the second time as indirectly via _init().
 */
void
_libc_init(void)
{

	if (libc_initialised)
		return;

	libc_initialised = 1;

	if (__ps_strings != NULL)
		__auxinfo = __ps_strings->ps_argvstr +
		    __ps_strings->ps_nargvstr + __ps_strings->ps_nenvstr + 2;

	/* For -fstack-protector */
	__guard_setup();

	/* Atomic operations */
	__libc_atomic_init();

	/* Threads */
	__libc_thr_init();

	/* Initialize the atexit mutexes */
	__libc_atexit_init();

	/* Initialize environment memory RB tree. */
	__libc_env_init();
}
