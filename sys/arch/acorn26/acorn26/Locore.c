/*	$NetBSD: Locore.c,v 1.6 2003/01/08 00:00:03 thorpej Exp $	*/

/*
 * Copyright (c) 2000 Ben Harris.
 * Copyright (C) 1995, 1996 Wolfgang Solfrank.
 * Copyright (C) 1995, 1996 TooLs GmbH.
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
 *	This product includes software developed by TooLs GmbH.
 * 4. The name of TooLs GmbH may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY TOOLS GMBH ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TOOLS GMBH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Some additional routines that happened to be in locore.S traditionally,
 * but have no need to be coded in assembly.
 */

#include "opt_lockdebug.h"

#include <sys/param.h>

__RCSID("$NetBSD: Locore.c,v 1.6 2003/01/08 00:00:03 thorpej Exp $");

#include <sys/proc.h>
#include <sys/sched.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/ras.h>

#include <uvm/uvm_extern.h>

#include <machine/frame.h>
#include <machine/machdep.h>

void idle(void);

struct pcb *curpcb;

/*
 * Idle
 */
void
idle()
{

#if defined(LOCKDEBUG)
	sched_unlock_idle();
#endif
	spl0();
	while (sched_whichqs == 0)
		continue;
	splhigh();
#if defined(LOCKDEBUG)
	sched_lock_idle();
#endif
}

extern int want_resched; /* XXX should be in <machine/cpu.h> */

/*
 * Find the highest-priority runnable process and switch to it.
 */
void
cpu_switch(struct proc *p1, struct proc *newp)
{
	int which;
	struct prochd *q;
	struct proc *p2;

	/*
	 * We enter here with interrupts blocked and sched_lock held.
	 */

#if 0
	printf("cpu_switch: %p ->", p1);
#endif
	curproc = NULL;
	curpcb = NULL;
	while (sched_whichqs == 0)
		idle();
	which = ffs(sched_whichqs) - 1;
	q = &sched_qs[which];
	p2 = q->ph_link;
	remrunqueue(p2);
	want_resched = 0;
#ifdef LOCKDEBUG
	sched_unlock_idle();
#endif
	/* p->p_cpu initialized in fork1() for single-processor */
	p2->p_stat = SONPROC;
	curproc = p2;
	curpcb = &curproc->p_addr->u_pcb;
#if 0
	printf(" %p\n", p2);
#endif
	if (p2 == p1)
		return;
	pmap_deactivate(p1);
	pmap_activate(p2);

	/* Check for Restartable Atomic Sequences. */
	if (p2->p_nras != 0) {
		struct trapframe *tf = p2->p_addr->u_pcb.pcb_tf;
		caddr_t pc;

		pc = ras_lookup(p2, (caddr_t) tf->tf_pc);
		if (pc != (caddr_t) -1)
			tf->tf_pc = (register_t) pc;
	}

	cpu_loswitch(&p1->p_addr->u_pcb.pcb_sf, p2->p_addr->u_pcb.pcb_sf);
	/* We only get back here after the other process has run. */
}
