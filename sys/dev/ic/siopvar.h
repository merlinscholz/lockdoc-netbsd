/*	$NetBSD: siopvar.h,v 1.2 2000/04/25 16:27:05 bouyer Exp $	*/

/*
 * Copyright (c) 2000 Manuel Bouyer.
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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/* structure and definitions for the siop driver */

TAILQ_HEAD(cmd_list, siop_cmd);

/* Driver internal state */
struct siop_softc {
	struct device sc_dev;
	struct scsipi_link sc_link;	/* link to upper level */
	int features;			/* chip's features */
	int maxburst;
	int maxoff;
	int clock_div;
	bus_space_tag_t sc_rt;		/* bus_space registers tag */
	bus_space_handle_t sc_rh;	/* bus_space registers handle */
	bus_addr_t sc_raddr;		/* register adresses */
	bus_dma_tag_t sc_dmat;		/* bus DMA tag */
	bus_dmamap_t  sc_scriptdma;	/* DMA map for script */
	u_int32_t *sc_script;		/* script location in memory */
	int sc_nshedslots;		/* number of sheduler slots */
	struct siop_cmd *cmds;		/* commands array */
	struct cmd_list free_list;	/* cmd descr free list */
	struct cmd_list active_list[16]; /* per-target active cmds */
	u_int32_t sc_flags;
};
/* defs for sc_flags */
/* none for now */

/* features */
#define SF_BUS_WIDE	0x00000001 /* wide bus */
#define SF_BUS_ULTRA	0x00000002 /* Ultra (20Mhz) bus */
#define SF_BUS_ULTRA2	0x00000004 /* Ultra2 (40Mhz) bus */
#define SF_BUS_DIFF	0x00000008 /* differential bus */

#define SF_CHIP_LED0	0x00000100 /* led on GPIO0 */
#define SF_CHIP_DBLR	0x00000200 /* clock doubler */
#define SF_CHIP_QUAD	0x00000400 /* clock quadrupler */
#define SF_CHIP_CLK80	0x00000800 /* 80Mhz clock */
#define SF_CHIP_FIFO	0x00001000 /* large fifo */
#define SF_CHIP_PF	0x00002000 /* Intructions prefetch */
#define SF_CHIP_RAM	0x00004000 /* on-board RAM */

#define SF_PCI_RL	0x01000000 /* PCI read line */
#define SF_PCI_RM	0x02000000 /* PCI read multiple */
#define SF_PCI_BOF	0x04000000 /* PCI burst opcode fetch */
#define SF_PCI_CLS	0x08000000 /* PCI cache line size */
#define SF_PCI_WRI	0x10000000 /* PCI write and invalidate */

void    siop_attach __P((struct siop_softc *));
int	siop_intr __P((void *));
