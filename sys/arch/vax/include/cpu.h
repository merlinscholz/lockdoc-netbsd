/*      $NetBSD: cpu.h,v 1.5 1995/02/13 00:43:20 ragge Exp $      */

/*
 * Copyright (c) 1994 Ludd, University of Lule}, Sweden
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed at Ludd, University of Lule}
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

 /* All bugs are subject to removal without further notice */

#include "sys/cdefs.h"
#include "vax/include/mtpr.h"

#define enablertclock()
#define	cpu_swapin(p)
#define	cpu_set_init_frame(p,f) {extern u_int scratch;mtpr(scratch,PR_SSP);}
#define	cpu_wait(p)


extern volatile int cpunumber;
extern struct cpu_dep cpu_calls[];

struct	cpu_dep {
	int	(*cpu_loinit)(); /* Locore init before everything else */
	int	(*cpu_clock)();	 /* CPU dependent clock handling */
	int	(*cpu_mchk)();   /* Machine check handling */
	int	(*cpu_memerr)(); /* Memory subsystem errors */
	int	(*cpu_conf)();	 /* Autoconfiguration */
};

struct clockframe {
        int     pc;
        int     ps;
};

#define todr()		mfpr(PR_TODR)
#define	setsoftnet()	mtpr(12,PR_SIRR)
#define setsoftclock()	mtpr(8,PR_SIRR)

/*
 * Preempt the current process if in interrupt from user mode,
 * or after the current trap/syscall if in system mode.
 */

#define need_resched(){ \
	want_resched++; \
	mtpr(AST_OK,PR_ASTLVL); \
	}

/*
 * Notify the current process (p) that it has a signal pending,
 * process as soon as possible.
 */

#define signotify(p)     mtpr(AST_OK,PR_ASTLVL);

extern	int     want_resched;   /* resched() was called */

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the hp300, request an ast to send us
 * through trap, marking the proc as needing a profiling tick.
 */
#define need_proftick(p) {(p)->p_flag |= P_OWEUPC; mtpr(AST_OK,PR_ASTLVL); }

