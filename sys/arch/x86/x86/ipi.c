/*	$NetBSD: ipi.c,v 1.14 2009/03/30 22:28:40 rmind Exp $	*/

/*-
 * Copyright (c) 2000, 2008, 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by RedBack Networks Inc.
 *
 * Author: Bill Sommerfeld
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
__KERNEL_RCSID(0, "$NetBSD: ipi.c,v 1.14 2009/03/30 22:28:40 rmind Exp $");

#include "opt_mtrr.h"

#include <sys/param.h> 
#include <sys/device.h>
#include <sys/systm.h>
#include <sys/atomic.h>
#include <sys/intr.h>
#include <sys/cpu.h>
 
#ifdef MULTIPROCESSOR

#include <machine/cpufunc.h>
#include <machine/cpuvar.h>
#include <machine/i82093var.h>
#include <machine/i82489reg.h>
#include <machine/i82489var.h>
#include <machine/mtrr.h>
#include <machine/gdt.h>

#include <x86/cpu_msr.h>

#include "acpi.h"

#ifdef __x86_64__
#include <machine/fpu.h>
static void	x86_ipi_synch_fpu(struct cpu_info *);
#else
/* XXXfpu */
#include "npx.h"
#if NNPX > 0
static void	x86_ipi_synch_fpu(struct cpu_info *);
#define		fpusave_cpu(x)		npxsave_cpu(x)
#else
#define		x86_ipi_synch_fpu	NULL
#endif
#endif

static void	x86_ipi_halt(struct cpu_info *);
static void	x86_ipi_kpreempt(struct cpu_info *);

#ifdef MTRR
static void	x86_ipi_reload_mtrr(struct cpu_info *);
#else
#define		x86_ipi_reload_mtrr	NULL
#endif

#if NACPI > 0
void	acpi_cpu_sleep(struct cpu_info *);
#else
#define	acpi_cpu_sleep	NULL
#endif

void (*ipifunc[X86_NIPI])(struct cpu_info *) =
{
	x86_ipi_halt,
	NULL,
	NULL,
	x86_ipi_synch_fpu,
	x86_ipi_reload_mtrr,
	gdt_reload_cpu,
	msr_write_ipi,
	acpi_cpu_sleep,
	x86_ipi_kpreempt
};

/*
 * x86 IPI interface.
 */

int
x86_send_ipi(struct cpu_info *ci, int ipimask)
{
	int ret;

	atomic_or_32(&ci->ci_ipis, ipimask);

	/* Don't send IPI to CPU which isn't (yet) running. */
	if (!(ci->ci_flags & CPUF_RUNNING))
		return ENOENT;

	ret = x86_ipi(LAPIC_IPI_VECTOR, ci->ci_cpuid, LAPIC_DLMODE_FIXED);
	if (ret != 0) {
		printf("ipi of %x from %s to %s failed\n",
		    ipimask,
		    device_xname(curcpu()->ci_dev),
		    device_xname(ci->ci_dev));
	}

	return ret;
}

void
x86_broadcast_ipi(int ipimask)
{
	struct cpu_info *ci, *self = curcpu();
	int count = 0;
	CPU_INFO_ITERATOR cii;

	for (CPU_INFO_FOREACH(cii, ci)) {
		if (ci == self)
			continue;
		if ((ci->ci_flags & CPUF_RUNNING) == 0)
			continue;
		atomic_or_32(&ci->ci_ipis, ipimask);
		count++;
	}
	if (!count)
		return;

	x86_ipi(LAPIC_IPI_VECTOR, LAPIC_DEST_ALLEXCL, LAPIC_DLMODE_FIXED);
}

void
x86_multicast_ipi(int cpumask, int ipimask)
{
	struct cpu_info *ci;
	CPU_INFO_ITERATOR cii;

	if ((cpumask &= ~curcpu()->ci_cpumask) == 0)
		return;

	for (CPU_INFO_FOREACH(cii, ci)) {
		if ((cpumask & ci->ci_cpumask) != 0)
			x86_send_ipi(ci, ipimask);
	}
}

void
x86_ipi_handler(void)
{
	struct cpu_info *ci = curcpu();
	uint32_t pending;
	int bit;

	pending = atomic_swap_32(&ci->ci_ipis, 0);

	KDASSERT((pending >> X86_NIPI) == 0);
	while ((bit = ffs(pending)) != 0) {
		bit--;
		pending &= ~(1<<bit);
		ci->ci_ipi_events[bit].ev_count++;
		(*ipifunc[bit])(ci);
	}
}

/*
 * Common x86 IPI handlers.
 */

static void
x86_ipi_halt(struct cpu_info *ci)
{

	x86_disable_intr();
	atomic_and_32(&ci->ci_flags, ~CPUF_RUNNING);

	for(;;) {
		x86_hlt();
	}
}

#if defined(__x86_64__) || NNPX > 0	/* XXXfpu */
static void
x86_ipi_synch_fpu(struct cpu_info *ci)
{

	fpusave_cpu(true);
}
#endif

#ifdef MTRR
static void
x86_ipi_reload_mtrr(struct cpu_info *ci)
{

	if (mtrr_funcs != NULL) {
		/*
		 * mtrr_reload_cpu() is a macro in mtrr.h which picks
		 * the appropriate function to use.
		 */
		mtrr_reload_cpu(ci);
	}
}
#endif

static void
x86_ipi_kpreempt(struct cpu_info *ci)
{

	softint_trigger(1 << SIR_PREEMPT);
}

#else

int
x86_send_ipi(struct cpu_info *ci, int ipimask)
{

	return 0;
}

void
x86_broadcast_ipi(int ipimask)
{

}

void
x86_multicast_ipi(int cpumask, int ipimask)
{

}

#endif
