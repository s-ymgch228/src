/*	$NetBSD: bznsc.c,v 1.3 1996/11/11 15:33:57 is Exp $	*/

/*
 * Copyright (c) 1996 Ignatios Souvatzis
 * Copyright (c) 1982, 1990 The Regents of the University of California.
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
 *	This product contains software written by Ignatios Souvatzis for
 *	the NetBSD project.
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

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/device.h>
#include <scsi/scsi_all.h>
#include <scsi/scsiconf.h>
#include <vm/vm.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <machine/pmap.h>
#include <amiga/amiga/custom.h>
#include <amiga/amiga/cc.h>
#include <amiga/amiga/device.h>
#include <amiga/amiga/isr.h>
#include <amiga/dev/sfasreg.h>
#include <amiga/dev/sfasvar.h>
#include <amiga/dev/zbusvar.h>
#include <amiga/dev/bznscreg.h>
#include <amiga/dev/bznscvar.h>

void bznscattach __P((struct device *, struct device *, void *));
int  bznscmatch  __P((struct device *, void *, void *));

struct scsi_adapter bznsc_scsiswitch = {
	sfas_scsicmd,
	sfas_minphys,
	0,			/* no lun support */
	0,			/* no lun support */
};

struct scsi_device bznsc_scsidev = {
	NULL,		/* use default error handler */
	NULL,		/* do not have a start functio */
	NULL,		/* have no async handler */
	NULL,		/* Use default done routine */
};

struct cfattach bznsc_ca = {
	sizeof(struct bznsc_softc), bznscmatch, bznscattach
};

struct cfdriver bznsc_cd = {
	NULL, "bznsc", DV_DULL, NULL, 0
};

int bznsc_intr		 __P((void *));
void bznsc_set_dma_tc	 __P((struct sfas_softc *sc, unsigned int len));
int bznsc_setup_dma	 __P((struct sfas_softc *sc, vm_offset_t ptr, int len,
			      int mode));
int bznsc_build_dma_chain __P((struct sfas_softc *sc,
			      struct sfas_dma_chain *chain, void *p, int l));
int bznsc_need_bump	 __P((struct sfas_softc *sc, vm_offset_t ptr, int len));
void bznsc_led		 __P((struct sfas_softc *sc, int mode));
void bzndumsc_led	 __P((struct sfas_softc *sc, int mode));

/* Only use this if you already made sure it is one of our boards */
#define BZNCHIPADS(prod) ((prod) == 24 ? 0x1ff00 : 0xff00)

/*
 * If we are an Phase 5 Dev. Blizzard IV/1260 or -2060 SCSI option:
 */
int
bznscmatch(pdp, match, auxp)
	struct device	*pdp;
	void		*match, *auxp;
{
	struct zbus_args *zap;
	volatile u_int8_t *ta;

	zap = auxp;

	if (zap->manid != 0x2140)	/* Phase V ? */
		return(0);

	/* is it B2060? */
	if (zap->prodid == 24)
		goto found;

	/* if we're no 1200, can't be 1230IV/1260 */
	if (!is_a1200())
		return 0;

	/* check product id. olds have Fastlane product id */
	if (zap->prodid != 17 && zap->prodid != 11)
		return(0);

found:
       	ta = (vu_char *)(((char *)zap->va) + BZNCHIPADS(zap->prodid) + 0x20);

	if (badbaddr((caddr_t)ta))
		return(0);
				
	*ta = 0;
	*ta = 1;
	DELAY(5);
	if (*ta != 1)
		return(0);

	return(1);
}

u_int32_t bznsc_flags = 0;

void
bznscattach(pdp, dp, auxp)
	struct device	*pdp;
	struct device	*dp;
	void		*auxp;
{
	struct bznsc_softc *sc;
	struct zbus_args  *zap;
	bznsc_regmap_p	   rp;
	vu_char		  *fas;

	zap = auxp;

	fas = &((vu_char *)zap->va)[BZNCHIPADS(zap->prodid)];

	sc = (struct bznsc_softc *)dp;
	rp = &sc->sc_regmap;

	rp->FAS216.sfas_tc_low	= &fas[0x00];
	rp->FAS216.sfas_tc_mid	= &fas[0x04];
	rp->FAS216.sfas_fifo	= &fas[0x08];
	rp->FAS216.sfas_command	= &fas[0x0C];
	rp->FAS216.sfas_dest_id	= &fas[0x10];
	rp->FAS216.sfas_timeout	= &fas[0x14];
	rp->FAS216.sfas_syncper	= &fas[0x18];
	rp->FAS216.sfas_syncoff	= &fas[0x1C];
	rp->FAS216.sfas_config1	= &fas[0x20];
	rp->FAS216.sfas_clkconv	= &fas[0x24];
	rp->FAS216.sfas_test	= &fas[0x28];
	rp->FAS216.sfas_config2	= &fas[0x2C];
	rp->FAS216.sfas_config3	= &fas[0x30];
	rp->FAS216.sfas_tc_high	= &fas[0x38];
	rp->FAS216.sfas_fifo_bot = &fas[0x3C];

	rp->hardbits		= &fas[0xe0];
	rp->addrport		= &fas[0xf0];

	sc->sc_softc.sc_fas	= (sfas_regmap_p)rp;

	sc->sc_softc.sc_led	= zap->prodid == 24 ? bznsc_led : bzndumsc_led;

	sc->sc_softc.sc_setup_dma	= bznsc_setup_dma;
	sc->sc_softc.sc_build_dma_chain = bznsc_build_dma_chain;
	sc->sc_softc.sc_need_bump	= bznsc_need_bump;

	sc->sc_softc.sc_clock_freq   = 40;   /* Phase5 SCSI all run at 40MHz */
	sc->sc_softc.sc_timeout      = 250;  /* Set default timeout to 250ms */

	sc->sc_softc.sc_config_flags = bznsc_flags;	/* for the moment */

	sc->sc_softc.sc_host_id      = 7;    /* Should check the jumpers */

	sc->sc_softc.sc_bump_sz = NBPG;	/* XXX should be the VM pagesize */
	sc->sc_softc.sc_bump_pa = 0x0;

	sfasinitialize((struct sfas_softc *)sc);

	sc->sc_softc.sc_link.adapter_softc  = sc;
	sc->sc_softc.sc_link.adapter_target = sc->sc_softc.sc_host_id;
	sc->sc_softc.sc_link.adapter	    = &bznsc_scsiswitch;
	sc->sc_softc.sc_link.device	    = &bznsc_scsidev;
	sc->sc_softc.sc_link.openings	    = 1;

	sc->sc_softc.sc_isr.isr_intr = bznsc_intr;
	sc->sc_softc.sc_isr.isr_arg  = &sc->sc_softc;
	sc->sc_softc.sc_isr.isr_ipl  = 2;
	add_isr(&sc->sc_softc.sc_isr);

/* We don't want interrupt until we're initialized! */
	printf("\n%s: B%s60\n", dp->dv_xname, 
	    zap->prodid == 24 ? "20" : "IV/12");

/* attach all scsi units on us */
	config_found(dp, &sc->sc_softc.sc_link, scsiprint);
}

int
bznsc_intr(arg)
	void *arg;
{
	struct sfas_softc *dev = arg;
	bznsc_regmap_p	      rp;
	int		      quickints;

	rp = (bznsc_regmap_p)dev->sc_fas;

	if (*rp->FAS216.sfas_status & SFAS_STAT_INTERRUPT_PENDING) {
		quickints = 16;
		do {
			dev->sc_status = *rp->FAS216.sfas_status;
			dev->sc_interrupt = *rp->FAS216.sfas_interrupt;
	  
			if (dev->sc_interrupt & SFAS_INT_RESELECTED) {
				dev->sc_resel[0] = *rp->FAS216.sfas_fifo;
				dev->sc_resel[1] = *rp->FAS216.sfas_fifo;
			}
			sfasintr(dev);

		} while((*rp->FAS216.sfas_status & SFAS_STAT_INTERRUPT_PENDING)
			&& --quickints);

		return(1);
	} 
	return(0);
}

/* Set DMA transfer counter */
void
bznsc_set_dma_tc(sc, len)
	struct sfas_softc *sc;
	unsigned int	  len;
{
	*sc->sc_fas->sfas_tc_low  = len; len >>= 8;
	*sc->sc_fas->sfas_tc_mid  = len; len >>= 8;
	*sc->sc_fas->sfas_tc_high = len;
}

/* Initialize DMA for transfer */
int
bznsc_setup_dma(sc, ptr, len, mode)
	struct sfas_softc *sc;
	vm_offset_t	  ptr;
	int		  len;
	int		  mode;
{
	int		retval;
	u_int32_t	d;
	bznsc_regmap_p	   rp;

	retval = 0;

	switch(mode) {

	case SFAS_DMA_READ:
	case SFAS_DMA_WRITE:

		rp = (bznsc_regmap_p)sc->sc_fas;

		d = (u_int32_t)ptr;
		d >>= 1;

		if (mode == SFAS_DMA_WRITE)
			d |= (1L << 31);

		rp->addrport[12] = (u_int8_t)d;
		__asm __volatile("nop");

		d >>= 8;
		rp->addrport[8] = (u_int8_t)d;
		__asm __volatile("nop");

		d >>= 8;
		rp->addrport[4] = (u_int8_t)d;
		__asm __volatile("nop");

		d >>= 8;
		rp->addrport[0] = (u_int8_t)d;
		__asm __volatile("nop");

		bznsc_set_dma_tc(sc, len);
		break;

	case SFAS_DMA_CLEAR:
	default:
		retval = (*sc->sc_fas->sfas_tc_high << 16) |
			 (*sc->sc_fas->sfas_tc_mid  <<  8) |
			  *sc->sc_fas->sfas_tc_low;

		bznsc_set_dma_tc(sc, 0);
		break;
	}

	return(retval);
}

/* Check if address and len is ok for DMA transfer */
int
bznsc_need_bump(sc, ptr, len)
	struct sfas_softc *sc;
	vm_offset_t	  ptr;
	int		  len;
{
	int	p;

	p = (int)ptr & 0x03;

	if (p) {
		p = 4-p;
	    
		if (len < 256)
			p = len;
	}
	return(p);
}

/* Interrupt driven routines */
/* XXX some of this is voodoo might be remnants intended for the Fastlane. */
int
bznsc_build_dma_chain(sc, chain, p, l)
	struct sfas_softc	*sc;
	struct sfas_dma_chain	*chain;
	void			*p;
	int			 l;
{
	vm_offset_t  pa, lastpa;
	char	    *ptr;
	int	     len, prelen, max_t, n;

	if (l == 0)
		return(0);

#define set_link(n, p, l, f)\
do { chain[n].ptr = (p); chain[n].len = (l); chain[n++].flg = (f); } while(0)

	n = 0;

	if (l < 512)
		set_link(n, (vm_offset_t)p, l, SFAS_CHAIN_BUMP);
	else if ((p >= (void *)0xFF000000)
#if defined(M68040) || defined(M68060)
		 && ((mmutype == MMU_68040) && (p >= (void *)0xFFFC0000))
#endif
		 ) {
		while(l != 0) {
			len = ((l > sc->sc_bump_sz) ? sc->sc_bump_sz : l);
	  
			set_link(n, (vm_offset_t)p, len, SFAS_CHAIN_BUMP);
	  
			p += len;
			l -= len;
		}
	} else {
		ptr = p;
		len = l;

		pa = kvtop(ptr);
		prelen = ((int)ptr & 0x03);

		if (prelen) {
			prelen = 4-prelen;
			set_link(n, (vm_offset_t)ptr, prelen, SFAS_CHAIN_BUMP);
			ptr += prelen;
			len -= prelen;
		}

		lastpa = 0;
		while(len > 3) {
			pa = kvtop(ptr);
			max_t = NBPG - (pa & PGOFSET);
			if (max_t > len)
			  max_t = len;

			max_t &= ~3;

			if (lastpa == pa)
				sc->sc_chain[n-1].len += max_t;
			else
				set_link(n, pa, max_t, SFAS_CHAIN_DMA);
	  
			lastpa = pa+max_t;
	  
			ptr += max_t;
			len -= max_t;
		}
      
		if (len)
			set_link(n, (vm_offset_t)ptr, len, SFAS_CHAIN_BUMP);
	}

	return(n);
}

/* Turn on/off led: dummy on 1260 */
void
bzndumsc_led(sc, mode)
	struct sfas_softc *sc;
	int		  mode;
{
	/* ARGSUSED */
}

/* real one for 2060 */
void
bznsc_led(sc, mode)
	struct sfas_softc *sc;
	int		  mode;
{
	bznsc_regmap_p		rp;

	rp = (bznsc_regmap_p)sc->sc_fas;

	if (mode)
		*rp->hardbits = 0x00;	/* Led on, Int on */
	else
		*rp->hardbits = 0x02;	/* Led off, Int on */
}
