/*	$NetBSD: signal.h,v 1.2 2003/01/17 22:28:48 thorpej Exp $	*/

/*
 * Copyright (c) 1994-1996 Mark Brinicombe.
 * Copyright (c) 1994 Brini.
 * All rights reserved.
 *
 * This code is derived from software written for Brini by Mark Brinicombe
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
 *	This product includes software developed by Brini.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY BRINI ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL BRINI OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * RiscBSD kernel project
 *
 * signal.h
 *
 * Architecture dependant signal types and structures
 *
 * Created      : 30/09/94
 */

#ifndef _ARM32_SIGNAL_H_
#define _ARM32_SIGNAL_H_

#ifndef _LOCORE
typedef int sig_atomic_t;
#endif

#if !defined(_ANSI_SOURCE) && !defined(_POSIX_C_SOURCE) && \
    !defined(_XOPEN_SOURCE)
#ifndef _LOCORE
/*
 * Information pushed on stack when a signal is delivered.
 * This is used by the kernel to restore state following
 * execution of the signal handler.  It is also made available
 * to the handler to allow it to restore state properly if
 * a non-standard exit is performed.
 */

#if defined(__LIBC12_SOURCE__) || defined(_KERNEL)
struct sigcontext13 {
	int	sc_onstack;		/* sigstack state to restore */
	int	sc_mask;		/* signal mask to restore (old style) */

	unsigned int sc_spsr;
	unsigned int sc_r0;
	unsigned int sc_r1;
	unsigned int sc_r2;
	unsigned int sc_r3;
	unsigned int sc_r4;
	unsigned int sc_r5;
	unsigned int sc_r6;
	unsigned int sc_r7;
	unsigned int sc_r8;
	unsigned int sc_r9;
	unsigned int sc_r10;
	unsigned int sc_r11;
	unsigned int sc_r12;
	unsigned int sc_usr_sp;
	unsigned int sc_usr_lr;
	unsigned int sc_svc_lr;
	unsigned int sc_pc;
};
#endif /* __LIBC12_SOURCE__ || _KERNEL */

struct sigcontext {
	int	sc_onstack;		/* sigstack state to restore */
	int	__sc_mask13;		/* signal mask to restore (old style) */

	unsigned int sc_spsr;
	unsigned int sc_r0;
	unsigned int sc_r1;
	unsigned int sc_r2;
	unsigned int sc_r3;
	unsigned int sc_r4;
	unsigned int sc_r5;
	unsigned int sc_r6;
	unsigned int sc_r7;
	unsigned int sc_r8;
	unsigned int sc_r9;
	unsigned int sc_r10;
	unsigned int sc_r11;
	unsigned int sc_r12;
	unsigned int sc_usr_sp;
	unsigned int sc_usr_lr;
	unsigned int sc_svc_lr;
	unsigned int sc_pc;
	
	sigset_t sc_mask;		/* signal mask to restore (new style) */
};

/*
 * The following macros are used to convert from a ucontext to sigcontext,
 * and vice-versa.  This is for building a sigcontext to deliver to old-style
 * signal handlers, and converting back (in the event the handler modifies
 * the context).
 */
#define	_MCONTEXT_TO_SIGCONTEXT(uc, sc)					\
do {									\
	(sc)->sc_spsr = (uc)->uc_mcontext.__gregs[_REG_CPSR];		\
	memcpy(&(sc)->sc_r0, (uc)->uc_mcontext.__gregs,			\
	    15 * sizeof(unsigned int));					\
	(sc)->sc_svc_lr = 0;		/* XXX */			\
	(sc)->sc_pc = (uc)->uc_mcontext.__gregs[_REG_PC];		\
} while (/*CONSTCOND*/0)

#define	_SIGCONTEXT_TO_MCONTEXT(sc, uc)					\
do {									\
	(uc)->uc_mcontext.__gregs[_REG_CPSR] = (sc)->sc_spsr;		\
	memcpy((uc)->uc_mcontext.__gregs, &(sc)->sc_r0,			\
	    15 * sizeof(unsigned int));					\
	(uc)->uc_mcontext.__gregs[_REG_PC] = (sc)->sc_pc;		\
} while (/*CONSTCOND*/0)

#endif /* !_LOCORE */

/* Signals codes */

/*
 * SIGFPE codes
 *
 * see ieeefp.h for definition of FP exception codes
 */

#define SIG_CODE_FPE_CODE_MASK	0x00000f00	/* Mask for exception code */
#define SIG_CODE_FPE_CODE_SHIFT	8		/* Shift for exception code */
#define SIG_CODE_FPE_TYPE_MASK	0x000000ff	/* Mask for specific code */

/*
 * SIGILL codes
 *
 * the signal code is the instruction that raised the signal
 */

/*
 * SIGBUS and SIGSEGV codes
 *
 * The signal code is combination of the fault address and the fault code.
 *
 * The fault code is the coproc #15 fault status code
 *
 * The exception to this is a SIGBUS or SIGSEGV from a prefetch abort.
 * In this case the fault status code is not valid so the TYPE_MASK
 * should be treated as undefined (in practice it is the bottom 4 bits
 * of the fault address).
 */

#define SIG_CODE_BUS_ADDR_MASK	0xfffffff0
#define SIG_CODE_BUS_TYPE_MASK	0x0000000f
#define SIG_CODE_SEGV_ADDR_MASK	SIG_CODE_BUS_ADDR_MASK
#define SIG_CODE_SEGV_TYPE_MASK	SIG_CODE_BUS_TYPE_MASK

#endif	/* !_ANSI_SOURCE && !_POSIX_C_SOURCE && !_XOPEN_SOURCE */
#endif	/* !_ARM_SIGNAL_H_ */

/* End of signal.h */
