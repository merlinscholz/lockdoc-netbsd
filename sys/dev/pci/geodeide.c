/*	$NetBSD: geodeide.c,v 1.8 2005/02/27 00:27:32 perry Exp $	*/

/*
 * Copyright (c) 2004 Manuel Bouyer.
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
 *	This product includes software developed by Manuel Bouyer.
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
 *
 */

/*
 * Driver for the IDE part of the AMD Geode CS5530A companion chip
 * and AMD Geode SC1100.
 * Docs available from AMD's web site
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: geodeide.c,v 1.8 2005/02/27 00:27:32 perry Exp $");

#include <sys/param.h>
#include <sys/systm.h>

#include <uvm/uvm_extern.h>

#include <dev/pci/pcivar.h>
#include <dev/pci/pcidevs.h>
#include <dev/pci/pciidereg.h>
#include <dev/pci/pciidevar.h>

#include <dev/pci/pciide_geode_reg.h>

static void geodeide_chip_map(struct pciide_softc *,
				 struct pci_attach_args *);
static void geodeide_setup_channel(struct ata_channel *);

static int  geodeide_match(struct device *, struct cfdata *, void *);
static void geodeide_attach(struct device *, struct device *, void *);

CFATTACH_DECL(geodeide, sizeof(struct pciide_softc),
    geodeide_match, geodeide_attach, NULL, NULL);

static const struct pciide_product_desc pciide_geode_products[] = {
	{ PCI_PRODUCT_CYRIX_CX5530_IDE,
	  0,
	  "AMD Geode CX5530 IDE controller",
	  geodeide_chip_map,
	},
	{ PCI_PRODUCT_NS_SC1100_IDE,
	  0,
	  "AMD Geode SC1100 IDE controller",
	  geodeide_chip_map,
	},
	{ 0,
	  0,
	  NULL,
	  NULL,
	},
};

static int
geodeide_match(struct device *parent, struct cfdata *match, void *aux)
{
	struct pci_attach_args *pa = aux;

	if ((PCI_VENDOR(pa->pa_id) == PCI_VENDOR_CYRIX ||
	     PCI_VENDOR(pa->pa_id) == PCI_VENDOR_NS) &&
	     PCI_CLASS(pa->pa_class) == PCI_CLASS_MASS_STORAGE &&
	     PCI_SUBCLASS(pa->pa_class) == PCI_SUBCLASS_MASS_STORAGE_IDE &&
	     pciide_lookup_product(pa->pa_id, pciide_geode_products))
		return(2);
	return (0);
}

static void
geodeide_attach(struct device *parent, struct device *self, void *aux)
{
	struct pci_attach_args *pa = aux;
	struct pciide_softc *sc = (void *)self;

	pciide_common_attach(sc, pa,
	    pciide_lookup_product(pa->pa_id, pciide_geode_products));
}

static void
geodeide_chip_map(struct pciide_softc *sc, struct pci_attach_args *pa)
{
	struct pciide_channel *cp;
	int channel;
	bus_size_t cmdsize, ctlsize;

	if (pciide_chipen(sc, pa) == 0)
		return;

	aprint_normal("%s: bus-master DMA support present",
	    sc->sc_wdcdev.sc_atac.atac_dev.dv_xname);
	pciide_mapreg_dma(sc, pa);
	aprint_normal("\n");
	if (sc->sc_dma_ok) {
		sc->sc_wdcdev.sc_atac.atac_cap = ATAC_CAP_DMA | ATAC_CAP_UDMA;
		sc->sc_wdcdev.irqack = pciide_irqack;
	}
	sc->sc_wdcdev.sc_atac.atac_pio_cap = 4;
	sc->sc_wdcdev.sc_atac.atac_dma_cap = 2;
	sc->sc_wdcdev.sc_atac.atac_udma_cap = 2;
	sc->sc_wdcdev.sc_atac.atac_set_modes = geodeide_setup_channel;
	sc->sc_wdcdev.sc_atac.atac_channels = sc->wdc_chanarray;
	sc->sc_wdcdev.sc_atac.atac_nchannels = PCIIDE_NUM_CHANNELS;
	sc->sc_wdcdev.sc_atac.atac_cap |= ATAC_CAP_DATA16 | ATAC_CAP_DATA32;

	/*
	 * Soekris Engineering Issue #0003:
	 * 	"The SC1100 built in busmaster IDE controller is pretty
	 *	 standard, but have two bugs: data transfers need to be
	 *	 dword aligned and it cannot do an exact 64Kbyte data
	 *	 transfer."
	 */
	if (sc->sc_pp->ide_product == PCI_PRODUCT_NS_SC1100_IDE) {
		if (sc->sc_dma_boundary == 0x10000)
			sc->sc_dma_boundary -= PAGE_SIZE;

		if (sc->sc_dma_maxsegsz == 0x10000)
			sc->sc_dma_maxsegsz -= PAGE_SIZE;
	}

	wdc_allocate_regs(&sc->sc_wdcdev);

	for (channel = 0; channel < sc->sc_wdcdev.sc_atac.atac_nchannels;
	     channel++) {
		cp = &sc->pciide_channels[channel];
		/* controller is compat-only */
		if (pciide_chansetup(sc, channel, 0) == 0)
			continue;
		pciide_mapchan(pa, cp, 0, &cmdsize, &ctlsize, pciide_pci_intr);
	}
}

static void
geodeide_setup_channel(struct ata_channel *chp)
{
	struct ata_drive_datas *drvp;
	struct pciide_channel *cp = CHAN_TO_PCHAN(chp);
	struct pciide_softc *sc = CHAN_TO_PCIIDE(chp);
	int channel = chp->ch_channel;
	int drive, s;
	u_int32_t dma_timing;
	u_int8_t idedma_ctl;
	const int32_t *geode_pio;
	const int32_t *geode_dma;
	const int32_t *geode_udma;
	bus_size_t dmaoff, piooff;

	switch (sc->sc_pp->ide_product) {
	case PCI_PRODUCT_CYRIX_CX5530_IDE:
		geode_pio = geode_cs5530_pio;
		geode_dma = geode_cs5530_dma;
		geode_udma = geode_cs5530_udma;
		break;

	case PCI_PRODUCT_NS_SC1100_IDE:
	default: /* XXX gcc */
		geode_pio = geode_sc1100_pio;
		geode_dma = geode_sc1100_dma;
		geode_udma = geode_sc1100_udma;
		break;
	}

	/* setup DMA if needed */
	pciide_channel_dma_setup(cp);

	idedma_ctl = 0;

	/* Per drive settings */
	for (drive = 0; drive < 2; drive++) {
		drvp = &chp->ch_drive[drive];
		/* If no drive, skip */
		if ((drvp->drive_flags & DRIVE) == 0)
			continue;

		switch (sc->sc_pp->ide_product) {
		case PCI_PRODUCT_CYRIX_CX5530_IDE:
			dmaoff = CS5530_DMA_REG(channel, drive);
			piooff = CS5530_PIO_REG(channel, drive);
			dma_timing = CS5530_DMA_REG_PIO_FORMAT;
			break;

		case PCI_PRODUCT_NS_SC1100_IDE:
		default: /* XXX gcc */
			dmaoff = SC1100_DMA_REG(channel, drive);
			piooff = SC1100_PIO_REG(channel, drive);
			dma_timing = 0;
			break;
		}

		/* add timing values, setup DMA if needed */
		if (drvp->drive_flags & DRIVE_UDMA) {
			/* Use Ultra-DMA */
			dma_timing |= geode_udma[drvp->UDMA_mode];
			idedma_ctl |= IDEDMA_CTL_DRV_DMA(drive);
		} else if (drvp->drive_flags & DRIVE_DMA) {
			/* use Multiword DMA */
			dma_timing |= geode_dma[drvp->DMA_mode];
			idedma_ctl |= IDEDMA_CTL_DRV_DMA(drive);
		} else {
			/* PIO only */
			s = splbio();
			drvp->drive_flags &= ~(DRIVE_UDMA | DRIVE_DMA);
			splx(s);
		}

		switch (sc->sc_pp->ide_product) {
		case PCI_PRODUCT_CYRIX_CX5530_IDE:
			bus_space_write_4(sc->sc_dma_iot, sc->sc_dma_ioh,
			    dmaoff, dma_timing);
			bus_space_write_4(sc->sc_dma_iot, sc->sc_dma_ioh,
			    piooff, geode_pio[drvp->PIO_mode]);
			break;

		case PCI_PRODUCT_NS_SC1100_IDE:
			pci_conf_write(sc->sc_pc, sc->sc_tag, dmaoff,
			    dma_timing);
			pci_conf_write(sc->sc_pc, sc->sc_tag, piooff,
			    geode_pio[drvp->PIO_mode]);
			break;
		}
	}

	if (idedma_ctl != 0) {
		/* Add software bits in status register */
		bus_space_write_1(sc->sc_dma_iot, cp->dma_iohs[IDEDMA_CTL], 0,
		    idedma_ctl);
	}
}
