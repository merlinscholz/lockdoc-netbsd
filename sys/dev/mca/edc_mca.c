/*	$NetBSD: edc_mca.c,v 1.28 2005/02/27 00:27:21 perry Exp $	*/

/*
 * Copyright (c) 2001 The NetBSD Foundation, Inc.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jaromir Dolecek.
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
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
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

/*
 * Driver for MCA ESDI controllers and disks conforming to IBM DASD
 * spec.
 *
 * The driver was written with DASD Storage Interface Specification
 * for MCA rev. 2.2 in hands, thanks to Scott Telford <st@epcc.ed.ac.uk>.
 *
 * TODO:
 * - improve error recovery
 *   Issue soft reset on error or timeout?
 * - test with > 1 disk (this is supported by some controllers)
 * - test with > 1 ESDI controller in machine; shared interrupts
 *   necessary for this to work should be supported - edc_intr() specifically
 *   checks if the interrupt is for this controller
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: edc_mca.c,v 1.28 2005/02/27 00:27:21 perry Exp $");

#include "rnd.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/buf.h>
#include <sys/bufq.h>
#include <sys/errno.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/endian.h>
#include <sys/disklabel.h>
#include <sys/disk.h>
#include <sys/syslog.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/kernel.h>
#include <sys/kthread.h>
#if NRND > 0
#include <sys/rnd.h>
#endif

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/mca/mcareg.h>
#include <dev/mca/mcavar.h>
#include <dev/mca/mcadevs.h>

#include <dev/mca/edcreg.h>
#include <dev/mca/edvar.h>
#include <dev/mca/edcvar.h>

#include "locators.h"

#define EDC_ATTN_MAXTRIES	10000	/* How many times check for unbusy */
#define EDC_MAX_CMD_RES_LEN	8

struct edc_mca_softc {
	struct device sc_dev;

	bus_space_tag_t	sc_iot;
	bus_space_handle_t sc_ioh;

	/* DMA related stuff */
	bus_dma_tag_t sc_dmat;		/* DMA tag as passed by parent */
	bus_dmamap_t  sc_dmamap_xfer;	/* transfer dma map */

	void	*sc_ih;				/* interrupt handle */

	int	sc_flags;
#define	DASD_QUIET	0x01		/* don't dump cmd error info */

#define DASD_MAXDEVS	8
	struct ed_softc *sc_ed[DASD_MAXDEVS];
	int sc_maxdevs;			/* max number of disks attached to this
					 * controller */

	/* I/O results variables */
	volatile int sc_stat;
#define	STAT_START	0
#define	STAT_ERROR	1
#define	STAT_DONE	2
	volatile int sc_resblk;		/* residual block count */

	/* CMD status block - only set & used in edc_intr() */
	u_int16_t status_block[EDC_MAX_CMD_RES_LEN];
};

int	edc_mca_probe(struct device *, struct cfdata *, void *);
void	edc_mca_attach(struct device *, struct device *, void *);

CFATTACH_DECL(edc_mca, sizeof(struct edc_mca_softc),
    edc_mca_probe, edc_mca_attach, NULL, NULL);

static int	edc_intr(void *);
static void	edc_dump_status_block(struct edc_mca_softc *,
		    u_int16_t *, int);
static int	edc_do_attn(struct edc_mca_softc *, int, int, int);
static void	edc_cmd_wait(struct edc_mca_softc *, int, int);
static void	edcworker(void *);
static void	edc_spawn_worker(void *);

int
edc_mca_probe(parent, match, aux)
	struct device *parent;
	struct cfdata *match;
	void *aux;
{
	struct mca_attach_args *ma = aux;

	switch (ma->ma_id) {
	case MCA_PRODUCT_IBM_ESDIC:
	case MCA_PRODUCT_IBM_ESDIC_IG:
		return (1);
	default:
		return (0);
	}
}

static int
edcsubmatch(struct device *parent, struct cfdata *cf,
	    const locdesc_t *ldesc, void *aux)
{

	if (cf->cf_loc[EDCCF_DRIVE] != EDCCF_DRIVE_DEFAULT &&
	    cf->cf_loc[EDCCF_DRIVE] != ldesc->locs[EDCCF_DRIVE])
		return (0);

	return (config_match(parent, cf, aux));
}

void
edc_mca_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct edc_mca_softc *sc = (void *) self;
	struct mca_attach_args *ma = aux;
	struct ed_attach_args eda;
	int pos2, pos3, pos4;
	int irq, drq, iobase;
	const char *typestr;
	int devno, error;
	int help[2];
	locdesc_t *ldesc = (void *)help; /* XXX */

	pos2 = mca_conf_read(ma->ma_mc, ma->ma_slot, 2);
	pos3 = mca_conf_read(ma->ma_mc, ma->ma_slot, 3);
	pos4 = mca_conf_read(ma->ma_mc, ma->ma_slot, 4);

	/*
	 * POS register 2: (adf pos0)
	 *
	 * 7 6 5 4 3 2 1 0
	 *   \ \____/  \ \__ enable: 0=adapter disabled, 1=adapter enabled
	 *    \     \   \___ Primary/Alternate Port Addresses:
	 *     \     \		0=0x3510-3517 1=0x3518-0x351f
	 *      \     \_____ DMA Arbitration Level: 0101=5 0110=6 0111=7
	 *       \              0000=0 0001=1 0011=3 0100=4
	 *        \_________ Fairness On/Off: 1=On 0=Off
	 *
	 * POS register 3: (adf pos1)
	 *
	 * 7 6 5 4 3 2 1 0
	 * 0 0 \_/
	 *       \__________ DMA Burst Pacing Interval: 10=24ms 11=31ms
	 *                     01=16ms 00=Burst Disabled
	 *
	 * POS register 4: (adf pos2)
	 *
	 * 7 6 5 4 3 2 1 0
	 *           \_/ \__ DMA Pacing Control: 1=Disabled 0=Enabled
	 *             \____ Time to Release: 1X=6ms 01=3ms 00=Immediate
	 *
	 * IRQ is fixed to 14 (0x0e).
	 */

	switch (ma->ma_id) {
	case MCA_PRODUCT_IBM_ESDIC:
		typestr = "IBM ESDI Fixed Disk Controller";
		break;
	case MCA_PRODUCT_IBM_ESDIC_IG:
		typestr = "IBM Integ. ESDI Fixed Disk & Controller";
		break;
	default:
		typestr = NULL;
		break;
	}

	irq = ESDIC_IRQ;
	iobase = (pos2 & IO_IS_ALT) ? ESDIC_IOALT : ESDIC_IOPRM;
	drq = (pos2 & DRQ_MASK) >> 2;

	printf(" slot %d irq %d drq %d: %s\n", ma->ma_slot+1,
		irq, drq, typestr);

#ifdef DIAGNOSTIC
	/*
	 * It's not strictly necessary to check this, machine configuration
	 * utility uses only valid addresses.
	 */
	if (drq == 2 || drq >= 8) {
		printf("%s: invalid DMA Arbitration Level %d\n",
			sc->sc_dev.dv_xname, drq);
		return;
	}
#endif

	printf("%s: Fairness %s, Release %s, ",
		sc->sc_dev.dv_xname,
		(pos2 & FAIRNESS_ENABLE) ? "On" : "Off",
		(pos4 & RELEASE_1) ? "6ms"
				: ((pos4 & RELEASE_2) ? "3ms" : "Immediate")
		);
	if ((pos4 & PACING_CTRL_DISABLE) == 0) {
		static const char * const pacint[] =
			{ "disabled", "16ms", "24ms", "31ms"};
		printf("DMA burst pacing interval %s\n",
			pacint[(pos3 & PACING_INT_MASK) >> 4]);
	} else
		printf("DMA pacing control disabled\n");

	sc->sc_iot = ma->ma_iot;

	if (bus_space_map(sc->sc_iot, iobase,
	    ESDIC_REG_NPORTS, 0, &sc->sc_ioh)) {
		printf("%s: couldn't map registers\n",
		    sc->sc_dev.dv_xname);
		return;
	}

	sc->sc_ih = mca_intr_establish(ma->ma_mc, irq, IPL_BIO, edc_intr, sc);
	if (sc->sc_ih == NULL) {
		printf("%s: couldn't establish interrupt handler\n",
			sc->sc_dev.dv_xname);
		return;
	}

	/* Create a MCA DMA map, used for data transfer */
	sc->sc_dmat = ma->ma_dmat;
	if ((error = mca_dmamap_create(sc->sc_dmat, MAXPHYS,
	    BUS_DMA_NOWAIT | BUS_DMA_ALLOCNOW | MCABUS_DMA_16BIT,
	    &sc->sc_dmamap_xfer, drq)) != 0){
		printf("%s: couldn't create DMA map - error %d\n",
			sc->sc_dev.dv_xname, error);
		return;
	}

	/*
	 * Integrated ESDI controller supports only one disk, other
	 * controllers support two disks.
	 */
	if (ma->ma_id == MCA_PRODUCT_IBM_ESDIC_IG)
		sc->sc_maxdevs = 1;
	else
		sc->sc_maxdevs = 2;

	/*
	 * Reset controller and attach individual disks. ed attach routine
	 * uses polling so that this works with interrupts disabled.
	 */

	/* Do a reset to ensure sane state after warm boot. */
	if (bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR) & BSR_BUSY) {
		/* hard reset */
		printf("%s: controller busy, performing hardware reset ...\n",
			sc->sc_dev.dv_xname);
		bus_space_write_1(sc->sc_iot, sc->sc_ioh, BCR,
			BCR_INT_ENABLE|BCR_RESET);
	} else {
		/* "SOFT" reset */
		edc_do_attn(sc, ATN_RESET_ATTACHMENT, DASD_DEVNO_CONTROLLER,0);
	}

	/*
	 * Since interrupts are disabled, it's necessary
	 * to detect the interrupt request and call edc_intr()
	 * explicitly. See also edc_run_cmd().
	 */
	while (bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR) & BSR_BUSY) {
		if (bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR) & BSR_INTR)
			edc_intr(sc);

		delay(100);
	}

	/* be quiet during probes */
	sc->sc_flags |= DASD_QUIET;

	/* check for attached disks */
	for (devno = 0; devno < sc->sc_maxdevs; devno++) {
		eda.edc_drive = devno;
		ldesc->len = 1;
		ldesc->locs[EDCCF_DRIVE] = devno;
		sc->sc_ed[devno] =
			(void *) config_found_sm_loc(self, "edc", ldesc, &eda,
						     NULL, edcsubmatch);

		/* If initialization did not succeed, NULL the pointer. */
		if (sc->sc_ed[devno]
		    && (sc->sc_ed[devno]->sc_flags & EDF_INIT) == 0)
			sc->sc_ed[devno] = NULL;
	}

	/* enable full error dumps again */
	sc->sc_flags &= ~DASD_QUIET;

	/*
	 * Check if there are any disks attached. If not, disestablish
	 * the interrupt.
	 */
	for (devno = 0; devno < sc->sc_maxdevs; devno++) {
		if (sc->sc_ed[devno])
			break;
	}

	if (devno == sc->sc_maxdevs) {
		printf("%s: disabling controller (no drives attached)\n",
			sc->sc_dev.dv_xname);
		mca_intr_disestablish(ma->ma_mc, sc->sc_ih);
		return;
	}

	/*
	 * Run the worker thread.
	 */
	config_pending_incr();
	kthread_create(edc_spawn_worker, (void *) sc);
}

void
edc_add_disk(sc, ed)
	struct edc_mca_softc *sc;
	struct ed_softc *ed;
{
	sc->sc_ed[ed->sc_devno] = ed;
}

static int
edc_intr(arg)
	void *arg;
{
	struct edc_mca_softc *sc = arg;
	u_int8_t isr, intr_id;
	u_int16_t sifr;
	int cmd=-1, devno;

	/*
	 * Check if the interrupt was for us.
	 */
	if ((bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR) & BSR_INTR) == 0)
		return (0);

	/*
	 * Read ISR to find out interrupt type. This also clears the interrupt
	 * condition and BSR_INTR flag. Accordings to docs interrupt ID of 0, 2
	 * and 4 are reserved and not used.
	 */
	isr = bus_space_read_1(sc->sc_iot, sc->sc_ioh, ISR);
	intr_id = isr & ISR_INTR_ID_MASK;

#ifdef EDC_DEBUG
	if (intr_id == 0 || intr_id == 2 || intr_id == 4) {
		printf("%s: bogus interrupt id %d\n", sc->sc_dev.dv_xname,
			(int) intr_id);
		return (0);
	}
#endif

	/* Get number of device whose intr this was */
	devno = (isr & 0xe0) >> 5;

	/*
	 * Get Status block. Higher byte always says how long the status
	 * block is, rest is device number and command code.
	 * Check the status block length against our supported maximum length
	 * and fetch the data.
	 */
	if (bus_space_read_1(sc->sc_iot, sc->sc_ioh,BSR) & BSR_SIFR_FULL) {
		size_t len;
		int i;

		sifr = le16toh(bus_space_read_2(sc->sc_iot, sc->sc_ioh, SIFR));
		len = (sifr & 0xff00) >> 8;
#ifdef DEBUG
		if (len > EDC_MAX_CMD_RES_LEN)
			panic("%s: maximum Status Length exceeded: %d > %d",
				sc->sc_dev.dv_xname,
				len, EDC_MAX_CMD_RES_LEN);
#endif

		/* Get command code */
		cmd = sifr & SIFR_CMD_MASK;

		/* Read whole status block */
		sc->status_block[0] = sifr;
		for(i=1; i < len; i++) {
			while((bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR)
				& BSR_SIFR_FULL) == 0)
				;

			sc->status_block[i] = le16toh(
				bus_space_read_2(sc->sc_iot, sc->sc_ioh, SIFR));
		}
		/* zero out rest */
		if (i < EDC_MAX_CMD_RES_LEN) {
			memset(&sc->status_block[i], 0,
				(EDC_MAX_CMD_RES_LEN-i)*sizeof(u_int16_t));
		}
	}

	switch (intr_id) {
	case ISR_DATA_TRANSFER_RDY:
		/*
		 * Ready to do DMA. The DMA controller has already been
		 * setup, now just kick disk controller to do the transfer.
		 */
		bus_space_write_1(sc->sc_iot, sc->sc_ioh, BCR,
			BCR_INT_ENABLE|BCR_DMA_ENABLE);
		break;

	case ISR_COMPLETED:
	case ISR_COMPLETED_WITH_ECC:
	case ISR_COMPLETED_RETRIES:
	case ISR_COMPLETED_WARNING:
		/*
		 * Copy device config data if appropriate. sc->sc_ed[]
		 * entry might be NULL during probe.
		 */
		if (cmd == CMD_GET_DEV_CONF && sc->sc_ed[devno]) {
			memcpy(sc->sc_ed[devno]->sense_data, sc->status_block,
				sizeof(sc->sc_ed[devno]->sense_data));
		}

		sc->sc_stat = STAT_DONE;
		break;

	case ISR_RESET_COMPLETED:
	case ISR_ABORT_COMPLETED:
		/* nothing to do */
		break;

	case ISR_ATTN_ERROR:
		/*
		 * Basically, this means driver bug or something seriously
		 * hosed. panic rather than extending the lossage.
		 * No status block available, so no further info.
		 */
		panic("%s: dev %d: attention error",
			sc->sc_dev.dv_xname,
			devno);
		/* NOTREACHED */
		break;

	default:
		if ((sc->sc_flags & DASD_QUIET) == 0)
			edc_dump_status_block(sc, sc->status_block, intr_id);

		sc->sc_stat = STAT_ERROR;
		break;
	}

	/*
	 * Unless the interrupt is for Data Transfer Ready or
	 * Attention Error, finish by assertion EOI. This makes
	 * attachment aware the interrupt is processed and system
	 * is ready to accept another one.
	 */
	if (intr_id != ISR_DATA_TRANSFER_RDY && intr_id != ISR_ATTN_ERROR)
		edc_do_attn(sc, ATN_END_INT, devno, intr_id);

	/* If Read or Write Data, wakeup worker thread to finish it */
	if (intr_id != ISR_DATA_TRANSFER_RDY) {
	    	if (cmd == CMD_READ_DATA || cmd == CMD_WRITE_DATA)
			sc->sc_resblk = sc->status_block[SB_RESBLKCNT_IDX];
		wakeup_one(sc);
	}

	return (1);
}

/*
 * This follows the exact order for Attention Request as
 * written in DASD Storage Interface Specification MC (Rev 2.2).
 */
static int
edc_do_attn(sc, attn_type, devno, intr_id)
	struct edc_mca_softc *sc;
	int attn_type, devno, intr_id;
{
	int tries;

	/* 1. Disable interrupts in BCR. */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, BCR, 0);

	/*
	 * 2. Assure NOT BUSY and NO INTERRUPT PENDING, unless acknowledging
	 *    a RESET COMPLETED interrupt.
	 */
	if (intr_id != ISR_RESET_COMPLETED) {
#ifdef EDC_DEBUG
		if (attn_type == ATN_CMD_REQ
		    && (bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR)
			    & BSR_INT_PENDING))
			panic("%s: edc int pending", sc->sc_dev.dv_xname);
#endif

		for(tries=1; tries < EDC_ATTN_MAXTRIES; tries++) {
			if ((bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR)
			     & BSR_BUSY) == 0)
				break;
		}

		if (tries == EDC_ATTN_MAXTRIES) {
			printf("%s: edc_do_attn: timeout waiting for attachment to become available\n",
					sc->sc_ed[devno]->sc_dev.dv_xname);
			return (EIO);
		}
	}

	/*
	 * 3. Write proper DEVICE NUMBER and Attention number to ATN.
	 */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, ATN, attn_type | (devno<<5));

	/*
	 * 4. Enable interrupts via BCR.
	 */
	bus_space_write_1(sc->sc_iot, sc->sc_ioh, BCR, BCR_INT_ENABLE);

	return (0);
}

/*
 * Wait until command is processed, timeout after 'secs' seconds.
 * We use mono_time, since we don't need actual RTC, just time
 * interval.
 */
static void
edc_cmd_wait(sc, secs, poll)
	struct edc_mca_softc *sc;
	int secs, poll;
{
	int val;

	if (!poll) {
		int s;

		/* Not polling, can sleep. Sleep until we are awakened,
		 * but maximum secs seconds.
		 */
		s = splbio();
		if (sc->sc_stat != STAT_DONE)
			(void) tsleep(sc, PRIBIO, "edcwcmd", secs * hz);
		splx(s);
	}

	/* Wait until the command is completely finished */
	while((val = bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR))
	    & BSR_CMD_INPROGRESS) {
		if (poll && (val & BSR_INTR))
			edc_intr(sc);
	}
}

/*
 * Command controller to execute specified command on a device.
 */
int
edc_run_cmd(sc, cmd, devno, cmd_args, cmd_len, poll)
	struct edc_mca_softc *sc;
	int cmd;
	int devno;
	u_int16_t cmd_args[];
	int cmd_len, poll;
{
	int i, error, tries;
	u_int16_t cmd0;

	sc->sc_stat = STAT_START;

	/* Do Attention Request for Command Request. */
	if ((error = edc_do_attn(sc, ATN_CMD_REQ, devno, 0)))
		return (error);

	/*
	 * Construct the command. The bits are like this:
	 *
	 * 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
	 *  \_/   0  0       1 0 \__/   \_____/
	 *    \    \__________/     \         \_ Command Code (see CMD_*)
	 *     \              \      \__ Device: 0 common, 7 controller
	 *      \              \__ Options: reserved, bit 10=cache bypass bit
	 *       \_ Type: 00=2B, 01=4B, 10 and 11 reserved
	 *
	 * We always use device 0 or 1, so difference is made only by Command
	 * Code, Command Options and command length.
	 */
	cmd0 = ((cmd_len == 4) ? (CIFR_LONG_CMD) : 0)
		| (devno <<  5)
		| (cmd_args[0] << 8) | cmd;
	cmd_args[0] = cmd0;

	/*
	 * Write word of CMD to the CIFR. This sets "Command
	 * Interface Register Full (CMD IN)" in BSR. Once the attachment
	 * detects it, it reads the word and clears CMD IN. This all should
	 * be quite fast, so don't sleep in !poll case neither.
	 */
	for(i=0; i < cmd_len; i++) {
		bus_space_write_2(sc->sc_iot, sc->sc_ioh, CIFR,
			htole16(cmd_args[i]));

		/* Wait until CMD IN is cleared. */
		tries = 0;
		for(; (bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR)
		    & BSR_CIFR_FULL) && tries < 10000 ; tries++)
			delay(poll ? 1000 : 1);
			;

		if (tries == 10000
		    && bus_space_read_1(sc->sc_iot, sc->sc_ioh, BSR)
		       & BSR_CIFR_FULL) {
			printf("%s: device too slow to accept command %d\n",
				sc->sc_dev.dv_xname, cmd);
			return (EIO);
		}
	}

	/* Wait for command to complete, but maximum 15 seconds. */
	edc_cmd_wait(sc, 15, poll);

	return ((sc->sc_stat != STAT_DONE) ? EIO : 0);
}

#ifdef EDC_DEBUG
static const char * const edc_commands[] = {
	"Invalid Command",
	"Read Data",
	"Write Data",
	"Read Verify",
	"Write with Verify",
	"Seek",
	"Park Head",
	"Get Command Complete Status",
	"Get Device Status",
	"Get Device Configuration",
	"Get POS Information",
	"Translate RBA",
	"Write Attachment Buffer",
	"Read Attachment Buffer",
	"Run Diagnostic Test",
	"Get Diagnostic Status Block",
	"Get MFG Header",
	"Format Unit",
	"Format Prepare",
	"Set MAX RBA",
	"Set Power Saving Mode",
	"Power Conservation Command",
};

static const char * const edc_cmd_status[256] = {
	"Reserved",
	"Command completed successfully",
	"Reserved",
	"Command completed successfully with ECC applied",
	"Reserved",
	"Command completed successfully with retries",
	"Format Command partially completed",	/* Status available */
	"Command completed successfully with ECC and retries",
	"Command completed with Warning", 	/* Command Error is available */
	"Aborted",
	"Reset completed",
	"Data Transfer Ready",		/* No Status Block available */
	"Command terminated with failure",	/* Device Error is available */
	"DMA Error",			/* Retry entire command as recovery */
	"Command Block Error",
	"Attention Error (Illegal Attention Code)",
	/* 0x14 - 0xff reserved */
};

static const char * const edc_cmd_error[256] = {
	"No Error",
	"Invalid parameter in the command block",
	"Reserved",
	"Command not supported",
	"Command Aborted per request",
	"Reserved",
	"Command rejected",	/* Attachment diagnostic failure */
	"Format Rejected",	/* Prepare Format command is required */
	"Format Error (Primary Map is not readable)",
	"Format Error (Secondary map is not readable)",
	"Format Error (Diagnostic Failure)",
	"Format Warning (Secondary Map Overflow)",
	"Reserved"
	"Format Error (Host Checksum Error)",
	"Reserved",
	"Format Warning (Push table overflow)",
	"Format Warning (More pushes than allowed)",
	"Reserved",
	"Format Warning (Error during verifying)",
	"Invalid device number for the command",
	/* 0x14-0xff reserved */
};

static const char * const edc_dev_errors[] = {
	"No Error",
	"Seek Fault",	/* Device report */
	"Interface Fault (Parity, Attn, or Cmd Complete Error)",
	"Block not found (ID not found)",
	"Block not found (AM not found)",
	"Data ECC Error (hard error)",
	"ID CRC Error",
	"RBA Out of Range",
	"Reserved",
	"Defective Block",
	"Reserved",
	"Selection Error",
	"Reserved",
	"Write Fault",
	"No index or sector pulse",
	"Device Not Ready",
	"Seek Error",	/* Attachment report */
	"Bad Format",
	"Volume Overflow",
	"No Data AM Found",
	"Block not found (No ID AM or ID CRC error occurred)",
	"Reserved",
	"Reserved",
	"No ID found on track (ID search)",
	/* 0x19 - 0xff reserved */
};
#endif /* EDC_DEBUG */

static void
edc_dump_status_block(sc, status_block, intr_id)
	struct edc_mca_softc *sc;
	u_int16_t *status_block;
	int intr_id;
{
#ifdef EDC_DEBUG
	printf("%s: Command: %s, Status: %s (intr %d)\n",
		sc->sc_dev.dv_xname,
		edc_commands[status_block[0] & 0x1f],
		edc_cmd_status[SB_GET_CMD_STATUS(status_block)],
		intr_id
		);
#else
	printf("%s: Command: %d, Status: %d (intr %d)\n",
		sc->sc_dev.dv_xname,
		status_block[0] & 0x1f,
		SB_GET_CMD_STATUS(status_block),
		intr_id
		);
#endif
	printf("%s: # left blocks: %u, last processed RBA: %u\n",
		sc->sc_dev.dv_xname,
		status_block[SB_RESBLKCNT_IDX],
		(status_block[5] << 16) | status_block[4]);

	if (intr_id == ISR_COMPLETED_WARNING) {
#ifdef EDC_DEBUG
		printf("%s: Command Error Code: %s\n",
			sc->sc_dev.dv_xname,
			edc_cmd_error[status_block[1] & 0xff]);
#else
		printf("%s: Command Error Code: %d\n",
			sc->sc_dev.dv_xname,
			status_block[1] & 0xff);
#endif
	}

	if (intr_id == ISR_CMD_FAILED) {
#ifdef EDC_DEBUG
		char buf[100];

		printf("%s: Device Error Code: %s\n",
			sc->sc_dev.dv_xname,
			edc_dev_errors[status_block[2] & 0xff]);
		bitmask_snprintf((status_block[2] & 0xff00) >> 8,
			"\20"
			"\01SeekOrCmdComplete"
			"\02Track0Flag"
			"\03WriteFault"
			"\04Selected"
			"\05Ready"
			"\06Reserved0"
			"\07STANDBY"
			"\010Reserved0",
			buf, sizeof(buf));
		printf("%s: Device Status: %s\n",
			sc->sc_dev.dv_xname, buf);
#else
		printf("%s: Device Error Code: %d, Device Status: %d\n",
			sc->sc_dev.dv_xname,
			status_block[2] & 0xff,
			(status_block[2] & 0xff00) >> 8);
#endif
	}
}

static void
edc_spawn_worker(arg)
	void *arg;
{
	struct edc_mca_softc *sc = (struct edc_mca_softc *) arg;
	int error;
	struct proc *wrk;

	/* Now, everything is ready, start a kthread */
	if ((error = kthread_create1(edcworker, sc, &wrk,
			"%s", sc->sc_dev.dv_xname))) {
		printf("%s: cannot spawn worker thread: errno=%d\n",
			sc->sc_dev.dv_xname, error);
		panic("edc_spawn_worker");
	}
}

/*
 * Main worker thread function.
 */
void
edcworker(arg)
	void *arg;
{
	struct edc_mca_softc *sc = (struct edc_mca_softc *) arg;
	struct ed_softc *ed;
	struct buf *bp;
	int i, error;

	config_pending_decr();

	for(;;) {
		/* Wait until awakened */
		(void) tsleep(sc, PRIBIO, "edcidle", 0);

		for(i=0; i<sc->sc_maxdevs; ) {
			if ((ed = sc->sc_ed[i]) == NULL) {
				i++;
				continue;
			}

			/* Is there a buf for us ? */
			simple_lock(&ed->sc_q_lock);
			if ((bp = BUFQ_GET(&ed->sc_q)) == NULL) {
				simple_unlock(&ed->sc_q_lock);
				i++;
				continue;
			}
			simple_unlock(&ed->sc_q_lock);

			/* Instrumentation. */
			disk_busy(&ed->sc_dk);

			error = edc_bio(sc, ed, bp->b_data, bp->b_bcount,
				bp->b_rawblkno, (bp->b_flags & B_READ), 0);

			if (error) {
				bp->b_error = error;
				bp->b_flags |= B_ERROR;
			} else {
				/* Set resid, most commonly to zero. */
				bp->b_resid = sc->sc_resblk * DEV_BSIZE;
			}

			disk_unbusy(&ed->sc_dk, (bp->b_bcount - bp->b_resid),
			    (bp->b_flags & B_READ));
#if NRND > 0
			rnd_add_uint32(&ed->rnd_source, bp->b_blkno);
#endif
			biodone(bp);
		}
	}
}

int
edc_bio(struct edc_mca_softc *sc, struct ed_softc *ed, void *data,
	size_t bcount, daddr_t rawblkno, int isread, int poll)
{
	u_int16_t cmd_args[4];
	int error=0, fl;
	u_int16_t track;
	u_int16_t cyl;
	u_int8_t head;
	u_int8_t sector;

	mca_disk_busy();

	/* set WAIT and R/W flag appropriately for the DMA transfer */
	fl = ((poll) ? BUS_DMA_NOWAIT : BUS_DMA_WAITOK)
		| ((isread) ? BUS_DMA_READ : BUS_DMA_WRITE);

	/* Load the buffer for DMA transfer. */
	if ((error = bus_dmamap_load(sc->sc_dmat, sc->sc_dmamap_xfer, data,
	    bcount, NULL, BUS_DMA_STREAMING|fl))) {
		printf("%s: ed_bio: unable to load DMA buffer - error %d\n",
			ed->sc_dev.dv_xname, error);
		goto out;
	}

	bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap_xfer, 0,
		bcount, (isread) ? BUS_DMASYNC_PREREAD : BUS_DMASYNC_PREWRITE);

	track = rawblkno / ed->sectors;
	head = track % ed->heads;
	cyl = track / ed->heads;
	sector = rawblkno % ed->sectors;

	/* Read or Write Data command */
	cmd_args[0] = 2;	/* Options 0000010 */
	cmd_args[1] = bcount / DEV_BSIZE;
	cmd_args[2] = ((cyl & 0x1f) << 11) | (head << 5) | sector;
	cmd_args[3] = ((cyl & 0x3E0) >> 5);
	error = edc_run_cmd(sc,
			(isread) ? CMD_READ_DATA : CMD_WRITE_DATA,
			ed->sc_devno, cmd_args, 4, poll);

	/* Sync the DMA memory */
	if (!error)  {
		bus_dmamap_sync(sc->sc_dmat, sc->sc_dmamap_xfer, 0, bcount,
			(isread)? BUS_DMASYNC_POSTREAD : BUS_DMASYNC_POSTWRITE);
	}

	/* We are done, unload buffer from DMA map */
	bus_dmamap_unload(sc->sc_dmat, sc->sc_dmamap_xfer);

    out:
	mca_disk_unbusy();

	return (error);
}
