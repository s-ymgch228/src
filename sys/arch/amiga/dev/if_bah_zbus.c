/*	$NetBSD: if_bah_zbus.c,v 1.1 1998/09/02 22:32:06 is Exp $ */

/*
 * Copyright (c) 1994, 1995, 1998 Ignatios Souvatzis
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
 *      This product includes software developed by Ignatios Souvatzis
 *      for the NetBSD Project.
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

/*
 * Driver frontend for the Commodore Busines Machines and the
 * Ameristar ARCnet card.
 */

#include <sys/types.h>
#include <sys/param.h>

#include <sys/conf.h>
#include <sys/device.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/systm.h>
#include <sys/kernel.h>

#include <machine/bus.h>
#include <machine/cpu.h>
#include <machine/intr.h>

#include <net/if.h>
#include <net/route.h>

#include <net/if_arc.h>

#include <amiga/amiga/device.h>
#include <amiga/dev/zbusvar.h>

#include <dev/ic/smc90cx6var.h>

/*
 * A2060 software status per interface
 */
struct bah_zbus_softc {
	struct	bah_softc	sc_bah;
	struct	bus_space_tag	sc_bst;
	struct	isr		sc_isr;
};

int	bah_zbus_match __P((struct device *, struct cfdata *, void *));
void	bah_zbus_attach __P((struct device *, struct device *, void *));
void	bah_zbus_reset __P((struct bah_softc *, int));

struct cfattach bah_zbus_ca = {
	sizeof(struct bah_zbus_softc), bah_zbus_match, bah_zbus_attach
};

int
bah_zbus_match(parent, cfp, aux)
	struct device *parent;
	struct cfdata *cfp;
	void *aux;
{
	struct zbus_args *zap = aux;

	if ((zap->manid == 514 || zap->manid == 1053) && zap->prodid == 9)
		return (1);

	return (0);
}

void
bah_zbus_attach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct bah_zbus_softc *bsc = (void *)self;
	struct bah_softc *sc = &bsc->sc_bah;
	struct zbus_args *zap = aux;

#if (defined(BAH_DEBUG) && (BAH_DEBUG > 2))
	printf("\n%s: attach(0x%x, 0x%x, 0x%x)\n",
	    sc->sc_dev.dv_xname, parent, self, aux);
#endif
	bsc->sc_bst.base = (bus_addr_t)zap->va;
	bsc->sc_bst.stride = 1;

	sc->sc_bst_r = &bsc->sc_bst;
	sc->sc_regs = bsc->sc_bst.base + 0x4000;

	sc->sc_bst_m = &bsc->sc_bst;
	sc->sc_mem = bsc->sc_bst.base + 0x8000;

	sc->sc_reset = bah_zbus_reset;

	bah_attach_subr(sc);

	bsc->sc_isr.isr_intr = bahintr;
	bsc->sc_isr.isr_arg = sc;
	bsc->sc_isr.isr_ipl = 2;
	add_isr(&bsc->sc_isr);
}

void
bah_zbus_reset(sc, onoff)
	struct bah_softc *sc;
	int onoff;
{
	struct bah_zbus_softc *bsc;
	volatile u_int8_t *p;

	bsc = (struct bah_zbus_softc *)sc;

	p = (volatile u_int8_t *)bsc->sc_bst.base;

	p[0x0000] = 0;	/* A2060 reset flipflop */
	p[0xc000] = 0;	/* A560 reset flipflop */
	p[12228] = 0;	/* unknown magic */

	if (!onoff)
		return;

	DELAY(200);

	p[0x0000] = 0xff;
	p[0xc000] = 0xff;
	p[12228] = 0xff;

	return;
}
