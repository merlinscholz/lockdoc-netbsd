/*	$NetBSD: wdc_pioc.c,v 1.6 1998/09/22 00:40:37 mark Exp $	*/

/*
 * Copyright (c) 1997-1998 Mark Brinicombe.
 * Copyright (c) 1997 Causality Limited.
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
 *	This product includes software developed by Mark Brinicombe
 *	for the NetBSD Project.
 * 4. The name of the company nor the name of the author may be used to
 *    endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>

#include <machine/bus.h>
#include <machine/irqhandler.h>

#include <arm32/mainbus/piocvar.h>

#include <dev/ic/wdcvar.h>

#include "locators.h"

#define WDC_PIOC_REG_NPORTS	8
#define WDC_PIOC_AUXREG_OFFSET	(0x206 * 4)
#define WDC_PIOC_AUXREG_NPORTS	1

struct wdc_pioc_softc {
	struct wdc_softc sc_wdcdev;
	struct wdc_attachment_data sc_ad;
	void	*sc_ih;
};

/* prototypes for functions */
static int  wdc_pioc_probe  __P((struct device *, struct cfdata *, void *));
static void wdc_pioc_attach __P((struct device *, struct device *, void *));

/* device attach structure */
struct cfattach wdc_pioc_ca = {
	sizeof(struct wdc_pioc_softc), wdc_pioc_probe, wdc_pioc_attach
};

/*
 * int wdc_pioc_probe(struct device *parent, struct cfdata *cf, void *aux)
 *
 * Make sure we are trying to attach a wdc device and then
 * probe for one.
 */

static int
wdc_pioc_probe(parent, cf, aux)
	struct device *parent;
	struct cfdata *cf;
	void *aux;
{
	struct pioc_attach_args *pa = aux;
	struct wdc_attachment_data ad;
	int res;
	u_int iobase;

	if (pa->pa_name && strcmp(pa->pa_name, "wdc") != 0)
		return(0);

	/* We need an offset */
	if (pa->pa_offset == PIOCCF_OFFSET_DEFAULT)
		return(0);

	iobase = pa->pa_iobase + pa->pa_offset;
	memset(&ad, 0, sizeof(ad));
	ad.iot = pa->pa_iot;
	ad.auxiot = pa->pa_iot;

	if (bus_space_map(ad.iot, iobase, WDC_PIOC_REG_NPORTS, 0, &ad.ioh))
		return(0);
	if (bus_space_map(ad.auxiot, iobase + WDC_PIOC_AUXREG_OFFSET,
	    WDC_PIOC_AUXREG_NPORTS, 0, &ad.auxioh)) {
		bus_space_unmap(ad.iot, ad.ioh, WDC_PIOC_REG_NPORTS);
		return(0);
	}

	res = wdcprobe(&ad);

	bus_space_unmap(ad.auxiot, ad.auxioh, WDC_PIOC_AUXREG_NPORTS);
	bus_space_unmap(ad.iot, ad.ioh, WDC_PIOC_REG_NPORTS);

	if (res)
		 pa->pa_iosize = WDC_PIOC_REG_NPORTS;
	return(res);
}

/*
 * void wdc_pioc_attach(struct device *parent, struct device *self, void *aux)
 *
 * attach the wdc device
 */

static void
wdc_pioc_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct wdc_pioc_softc *sc = (void *)self;
	struct pioc_attach_args *pa = aux;
	u_int iobase;

	printf("\n");

	iobase = pa->pa_iobase + pa->pa_offset;
	sc->sc_ad.iot = pa->pa_iot;
	sc->sc_ad.auxiot = pa->pa_iot;
	if (bus_space_map(sc->sc_ad.iot, iobase, WDC_PIOC_REG_NPORTS, 0,
	    &sc->sc_ad.ioh))
		panic("%s: couldn't map drive registers\n", self->dv_xname);
	    
	if (bus_space_map(sc->sc_ad.auxiot, iobase + WDC_PIOC_AUXREG_OFFSET,
	    WDC_PIOC_AUXREG_NPORTS, 0, &sc->sc_ad.auxioh))
		panic("%s: couldn't map aux registers\n", self->dv_xname);

	sc->sc_ih = intr_claim(pa->pa_irq, IPL_BIO, "wdc",  wdcintr, sc);
	if (!sc->sc_ih)
		panic("%s: Cannot claim IRQ %d\n", self->dv_xname, pa->pa_irq);

	wdcattach(&sc->sc_wdcdev, &sc->sc_ad);
}

/* End of wdc_pioc.c */
