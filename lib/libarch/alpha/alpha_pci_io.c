/*	$NetBSD: alpha_pci_io.c,v 1.1 2000/02/26 18:59:36 thorpej Exp $	*/

/*-
 * Copyright (c) 2000 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

/*
 * Support for x86-style programmed I/O to PCI/EISA/ISA I/O space.  This
 * is currently used to provide such support for XFree86.  In a perfect
 * world, this would go away in favor of a real bus space mapping framework.
 */

#include <sys/param.h>

#include <machine/bwx.h>
#include <machine/sysarch.h>
#include <machine/pio.h>

#include <err.h>
#include <stdlib.h>

struct alpha_bus_window *alpha_pci_io_windows;
int alpha_pci_io_window_count;

__inline struct alpha_bus_window *alpha_pci_io_findwindow __P((bus_addr_t));
__inline u_int32_t *alpha_pci_io_swiz __P((bus_addr_t, int));

u_int8_t	alpha_pci_io_swiz_inb __P((bus_addr_t));
u_int16_t	alpha_pci_io_swiz_inw __P((bus_addr_t));
u_int32_t	alpha_pci_io_swiz_inl __P((bus_addr_t));
void		alpha_pci_io_swiz_outb __P((bus_addr_t, u_int8_t));
void		alpha_pci_io_swiz_outw __P((bus_addr_t, u_int16_t));
void		alpha_pci_io_swiz_outl __P((bus_addr_t, u_int32_t));

const struct alpha_pci_io_ops alpha_pci_io_swiz_ops = {
	alpha_pci_io_swiz_inb,
	alpha_pci_io_swiz_inw,
	alpha_pci_io_swiz_inl,
	alpha_pci_io_swiz_outb,
	alpha_pci_io_swiz_outw,
	alpha_pci_io_swiz_outl,
};

u_int8_t	alpha_pci_io_bwx_inb __P((bus_addr_t));
u_int16_t	alpha_pci_io_bwx_inw __P((bus_addr_t));
u_int32_t	alpha_pci_io_bwx_inl __P((bus_addr_t));
void		alpha_pci_io_bwx_outb __P((bus_addr_t, u_int8_t));
void		alpha_pci_io_bwx_outw __P((bus_addr_t, u_int16_t));
void		alpha_pci_io_bwx_outl __P((bus_addr_t, u_int32_t));

const struct alpha_pci_io_ops alpha_pci_io_bwx_ops = {
	alpha_pci_io_bwx_inb,
	alpha_pci_io_bwx_inw,
	alpha_pci_io_bwx_inl,
	alpha_pci_io_bwx_outb,
	alpha_pci_io_bwx_outw,
	alpha_pci_io_bwx_outl,
};

const struct alpha_pci_io_ops *alpha_pci_io_switch;

int
alpha_pci_io_enable(onoff)
	int onoff;
{
	struct alpha_bus_window *abw;
	int i, count;

	if (onoff == 0 && alpha_pci_io_windows != NULL) {
		for (i = 0; i < alpha_pci_io_window_count; i++)
			alpha_bus_unmapwindow(&alpha_pci_io_windows[i]);
		free(alpha_pci_io_windows);
		alpha_pci_io_windows = NULL;
		alpha_pci_io_window_count = 0;
		alpha_pci_io_switch = NULL;
		return (0);
	} else if (onoff == 0)
		return (0);
	else if (alpha_pci_io_windows != NULL)
		return (0);

	count = alpha_bus_getwindows(ALPHA_BUS_TYPE_PCI_IO, &abw);
	if (count <= 0)
		return (-1);

	for (i = 0; i < count; i++) {
		if (alpha_bus_mapwindow(&abw[i]) == -1) {
			free(abw);
			return (-1);
		}
	}

	alpha_pci_io_windows = abw;
	alpha_pci_io_window_count = count;

	if (abw->abw_abst.abst_flags & ABST_BWX)
		alpha_pci_io_switch = &alpha_pci_io_bwx_ops;
	else
		alpha_pci_io_switch = &alpha_pci_io_swiz_ops;

	return (0);
}

__inline struct alpha_bus_window *
alpha_pci_io_findwindow(ioaddr)
	bus_addr_t ioaddr;
{
	struct alpha_bus_window *abw;
	int i;

	/* XXX Cache the last hit? */

	for (i = 0; i < alpha_pci_io_window_count; i++) {
		abw = &alpha_pci_io_windows[i];
		if (ioaddr >= abw->abw_abst.abst_bus_start &&
		    ioaddr <= abw->abw_abst.abst_bus_end)
			return (abw);
	}

	warnx("alpha_pci_io_findwindow: no window for 0x%lx, ABORTING!",
	    (u_long) ioaddr);
	abort();
}

__inline u_int32_t *
alpha_pci_io_swiz(ioaddr, size)
	bus_addr_t ioaddr;
	int size;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int32_t *port;

	port = (u_int32_t *) (abw->abw_addr +
	    (((ioaddr - abw->abw_abst.abst_bus_start) <<
	      abw->abw_abst.abst_addr_shift) |
	     (size << abw->abw_abst.abst_size_shift)));

	return (port);
}

u_int8_t
alpha_pci_io_swiz_inb(ioaddr)
	bus_addr_t ioaddr;
{
	u_int32_t *port = alpha_pci_io_swiz(ioaddr, 0);
	int offset = ioaddr & 3;

	alpha_mb();

	return ((*port >> (8 * offset)) & 0xff);
}

u_int16_t
alpha_pci_io_swiz_inw(ioaddr)
	bus_addr_t ioaddr;
{
	u_int32_t *port = alpha_pci_io_swiz(ioaddr, 1);
	int offset = ioaddr & 3;

	alpha_mb();

	return ((*port >> (8 * offset)) & 0xffff);
}

u_int32_t
alpha_pci_io_swiz_inl(ioaddr)
	bus_addr_t ioaddr;
{
	u_int32_t *port = alpha_pci_io_swiz(ioaddr, 3);

	alpha_mb();

	return (*port);
}

void
alpha_pci_io_swiz_outb(ioaddr, val)
	bus_addr_t ioaddr;
	u_int8_t val;
{
	u_int32_t *port = alpha_pci_io_swiz(ioaddr, 0);
	int offset = ioaddr & 3;
	u_int32_t nval = val << (8 * offset);

	*port = nval;
	alpha_mb();
}

void
alpha_pci_io_swiz_outw(ioaddr, val)
	bus_addr_t ioaddr;
	u_int16_t val;
{
	u_int32_t *port = alpha_pci_io_swiz(ioaddr, 1);
	int offset = ioaddr & 3;
	u_int32_t nval = val << (8 * offset);

	*port = nval;
	alpha_mb();
}

void
alpha_pci_io_swiz_outl(ioaddr, val)
	bus_addr_t ioaddr;
	u_int32_t val;
{
	u_int32_t *port = alpha_pci_io_swiz(ioaddr, 3);

	*port = val;
	alpha_mb();
}

/*
 * The following functions are used only on EV56 and greater CPUs,
 * and the assembler requires going to EV56 mode in order to emit
 * these instructions.
 */
__asm(".arch ev56");

u_int8_t
alpha_pci_io_bwx_inb(ioaddr)
	bus_addr_t ioaddr;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int8_t *port = (u_int8_t *) (abw->abw_addr +
	    (ioaddr - abw->abw_abst.abst_bus_start));

	alpha_mb();

	return (alpha_ldbu(port));
}

u_int16_t
alpha_pci_io_bwx_inw(ioaddr)
	bus_addr_t ioaddr;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int16_t *port = (u_int16_t *) (abw->abw_addr +
	    (ioaddr - abw->abw_abst.abst_bus_start));

	alpha_mb();

	return (alpha_ldwu(port));
}

u_int32_t
alpha_pci_io_bwx_inl(ioaddr)
	bus_addr_t ioaddr;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int32_t *port = (u_int32_t *) (abw->abw_addr +
	    (ioaddr - abw->abw_abst.abst_bus_start));

	alpha_mb();

	return (*port);
}

void
alpha_pci_io_bwx_outb(ioaddr, val)
	bus_addr_t ioaddr;
	u_int8_t val;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int8_t *port = (u_int8_t *) (abw->abw_addr +
	    (ioaddr - abw->abw_abst.abst_bus_start));

	alpha_stb(port, val);
	alpha_mb();
}

void
alpha_pci_io_bwx_outw(ioaddr, val)
	bus_addr_t ioaddr;
	u_int16_t val;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int16_t *port = (u_int16_t *) (abw->abw_addr +
	    (ioaddr - abw->abw_abst.abst_bus_start));

	alpha_stw(port, val);
	alpha_mb();
}

void
alpha_pci_io_bwx_outl(ioaddr, val)
	bus_addr_t ioaddr;
	u_int32_t val;
{
	struct alpha_bus_window *abw = alpha_pci_io_findwindow(ioaddr);
	u_int32_t *port = (u_int32_t *) (abw->abw_addr +
	    (ioaddr - abw->abw_abst.abst_bus_start));

	*port = val;
	alpha_mb();
}
