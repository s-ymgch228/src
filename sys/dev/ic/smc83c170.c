/*	$NetBSD: smc83c170.c,v 1.24 2000/02/02 08:05:27 thorpej Exp $	*/

/*-
 * Copyright (c) 1998, 1999 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
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
 * Device driver for the Standard Microsystems Corp. 83C170
 * Ethernet PCI Integrated Controller (EPIC/100).
 */

#include "opt_inet.h"
#include "opt_ns.h"
#include "bpfilter.h"

#include <sys/param.h>
#include <sys/systm.h> 
#include <sys/mbuf.h>   
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <sys/device.h>
 
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_ether.h>

#if NBPFILTER > 0 
#include <net/bpf.h>
#endif 

#ifdef INET
#include <netinet/in.h> 
#include <netinet/if_inarp.h>
#endif

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#include <machine/bus.h>
#include <machine/intr.h>

#include <dev/mii/miivar.h>

#include <dev/ic/smc83c170reg.h>
#include <dev/ic/smc83c170var.h>

void	epic_start __P((struct ifnet *));
void	epic_watchdog __P((struct ifnet *));
int	epic_ioctl __P((struct ifnet *, u_long, caddr_t));

void	epic_shutdown __P((void *));

void	epic_reset __P((struct epic_softc *));
int	epic_init __P((struct epic_softc *));
void	epic_rxdrain __P((struct epic_softc *));
void	epic_stop __P((struct epic_softc *, int));
int	epic_add_rxbuf __P((struct epic_softc *, int));
void	epic_read_eeprom __P((struct epic_softc *, int, int, u_int16_t *));
void	epic_set_mchash __P((struct epic_softc *));
void	epic_fixup_clock_source __P((struct epic_softc *));
int	epic_mii_read __P((struct device *, int, int));
void	epic_mii_write __P((struct device *, int, int, int));
int	epic_mii_wait __P((struct epic_softc *, u_int32_t));
void	epic_tick __P((void *));

void	epic_statchg __P((struct device *));
int	epic_mediachange __P((struct ifnet *));
void	epic_mediastatus __P((struct ifnet *, struct ifmediareq *));

#define	INTMASK	(INTSTAT_FATAL_INT | INTSTAT_TXU | \
	    INTSTAT_TXC | INTSTAT_RXE | INTSTAT_RQE | INTSTAT_RCC)

int	epic_copy_small = 0;

/*
 * Attach an EPIC interface to the system.
 */
void
epic_attach(sc)
	struct epic_softc *sc;
{
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	int i, rseg, error;
	bus_dma_segment_t seg;
	u_int8_t enaddr[ETHER_ADDR_LEN], devname[12 + 1];
	u_int16_t myea[ETHER_ADDR_LEN / 2], mydevname[6];

	/*
	 * Allocate the control data structures, and create and load the
	 * DMA map for it.
	 */
	if ((error = bus_dmamem_alloc(sc->sc_dmat,
	    sizeof(struct epic_control_data), NBPG, 0, &seg, 1, &rseg,
	    BUS_DMA_NOWAIT)) != 0) {
		printf("%s: unable to allocate control data, error = %d\n",
		    sc->sc_dev.dv_xname, error);
		goto fail_0;
	}

	if ((error = bus_dmamem_map(sc->sc_dmat, &seg, rseg,
	    sizeof(struct epic_control_data), (caddr_t *)&sc->sc_control_data,
	    BUS_DMA_NOWAIT|BUS_DMA_COHERENT)) != 0) {
		printf("%s: unable to map control data, error = %d\n",
		    sc->sc_dev.dv_xname, error);
		goto fail_1;
	}

	if ((error = bus_dmamap_create(sc->sc_dmat,
	    sizeof(struct epic_control_data), 1,
	    sizeof(struct epic_control_data), 0, BUS_DMA_NOWAIT,
	    &sc->sc_cddmamap)) != 0) {
		printf("%s: unable to create control data DMA map, "
		    "error = %d\n", sc->sc_dev.dv_xname, error);
		goto fail_2;
	}

	if ((error = bus_dmamap_load(sc->sc_dmat, sc->sc_cddmamap,
	    sc->sc_control_data, sizeof(struct epic_control_data), NULL,
	    BUS_DMA_NOWAIT)) != 0) {
		printf("%s: unable to load control data DMA map, error = %d\n",
		    sc->sc_dev.dv_xname, error);
		goto fail_3;
	}

	/*
	 * Create the transmit buffer DMA maps.
	 */
	for (i = 0; i < EPIC_NTXDESC; i++) {
		if ((error = bus_dmamap_create(sc->sc_dmat, MCLBYTES,
		    EPIC_NFRAGS, MCLBYTES, 0, BUS_DMA_NOWAIT,
		    &EPIC_DSTX(sc, i)->ds_dmamap)) != 0) {
			printf("%s: unable to create tx DMA map %d, "
			    "error = %d\n", sc->sc_dev.dv_xname, i, error);
			goto fail_4;
		}
	}

	/*
	 * Create the recieve buffer DMA maps.
	 */
	for (i = 0; i < EPIC_NRXDESC; i++) {
		if ((error = bus_dmamap_create(sc->sc_dmat, MCLBYTES, 1,
		    MCLBYTES, 0, BUS_DMA_NOWAIT,
		    &EPIC_DSRX(sc, i)->ds_dmamap)) != 0) {
			printf("%s: unable to create rx DMA map %d, "
			    "error = %d\n", sc->sc_dev.dv_xname, i, error);
			goto fail_5;
		}
		EPIC_DSRX(sc, i)->ds_mbuf = NULL;
	}


	/*
	 * Bring the chip out of low-power mode and reset it to a known state.
	 */
	bus_space_write_4(st, sh, EPIC_GENCTL, 0);
	epic_reset(sc);

	/*
	 * Read the Ethernet address from the EEPROM.
	 */
	epic_read_eeprom(sc, 0, (sizeof(myea) / sizeof(myea[0])), myea);
	bcopy(myea, enaddr, sizeof(myea));

	/*
	 * ...and the device name.
	 */
	epic_read_eeprom(sc, 0x2c, (sizeof(mydevname) / sizeof(mydevname[0])),
	    mydevname);
	bcopy(mydevname, devname, sizeof(mydevname));
	devname[sizeof(mydevname)] = '\0';
	for (i = sizeof(mydevname) - 1; i >= 0; i--) {
		if (devname[i] == ' ')
			devname[i] = '\0';
		else
			break;
	}

	printf("%s: %s, Ethernet address %s\n", sc->sc_dev.dv_xname,
	    devname, ether_sprintf(enaddr));

	/*
	 * Initialize our media structures and probe the MII.
	 */
	sc->sc_mii.mii_ifp = ifp;
	sc->sc_mii.mii_readreg = epic_mii_read;
	sc->sc_mii.mii_writereg = epic_mii_write;
	sc->sc_mii.mii_statchg = epic_statchg;
	ifmedia_init(&sc->sc_mii.mii_media, 0, epic_mediachange,
	    epic_mediastatus);
	mii_attach(&sc->sc_dev, &sc->sc_mii, 0xffffffff, MII_PHY_ANY,
	    MII_OFFSET_ANY);
	if (LIST_FIRST(&sc->sc_mii.mii_phys) == NULL) {
		ifmedia_add(&sc->sc_mii.mii_media, IFM_ETHER|IFM_NONE, 0, NULL);
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_NONE);
	} else
		ifmedia_set(&sc->sc_mii.mii_media, IFM_ETHER|IFM_AUTO);

	ifp = &sc->sc_ethercom.ec_if;
	strcpy(ifp->if_xname, sc->sc_dev.dv_xname);
	ifp->if_softc = sc;
	ifp->if_flags = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	ifp->if_ioctl = epic_ioctl;
	ifp->if_start = epic_start;
	ifp->if_watchdog = epic_watchdog;

	/*
	 * Attach the interface.
	 */
	if_attach(ifp);
	ether_ifattach(ifp, enaddr);
#if NBPFILTER > 0
	bpfattach(&sc->sc_ethercom.ec_if.if_bpf, ifp, DLT_EN10MB,
	    sizeof(struct ether_header));
#endif

	/*
	 * Make sure the interface is shutdown during reboot.
	 */
	sc->sc_sdhook = shutdownhook_establish(epic_shutdown, sc);
	if (sc->sc_sdhook == NULL)
		printf("%s: WARNING: unable to establish shutdown hook\n",
		    sc->sc_dev.dv_xname);
	return;

	/*
	 * Free any resources we've allocated during the failed attach
	 * attempt.  Do this in reverse order and fall through.
	 */
 fail_5:
	for (i = 0; i < EPIC_NRXDESC; i++) {
		if (EPIC_DSRX(sc, i)->ds_dmamap != NULL)
			bus_dmamap_destroy(sc->sc_dmat,
			    EPIC_DSRX(sc, i)->ds_dmamap);
	}
 fail_4:
	for (i = 0; i < EPIC_NTXDESC; i++) {
		if (EPIC_DSTX(sc, i)->ds_dmamap != NULL)
			bus_dmamap_destroy(sc->sc_dmat,
			    EPIC_DSTX(sc, i)->ds_dmamap);
	}
	bus_dmamap_unload(sc->sc_dmat, sc->sc_cddmamap);
 fail_3:
	bus_dmamap_destroy(sc->sc_dmat, sc->sc_cddmamap);
 fail_2:
	bus_dmamem_unmap(sc->sc_dmat, (caddr_t)sc->sc_control_data,
	    sizeof(struct epic_control_data));
 fail_1:
	bus_dmamem_free(sc->sc_dmat, &seg, rseg);
 fail_0:
	return;
}

/*
 * Shutdown hook.  Make sure the interface is stopped at reboot.
 */
void
epic_shutdown(arg)
	void *arg;
{
	struct epic_softc *sc = arg;

	epic_stop(sc, 1);
}

/*
 * Start packet transmission on the interface.
 * [ifnet interface function]
 */
void
epic_start(ifp)
	struct ifnet *ifp;
{
	struct epic_softc *sc = ifp->if_softc;
	struct mbuf *m0, *m;
	struct epic_txdesc *txd;
	struct epic_descsoft *ds;
	struct epic_fraglist *fr;
	bus_dmamap_t dmamap;
	int error, firsttx, nexttx, opending, seg;

	/*
	 * Remember the previous txpending and the first transmit
	 * descriptor we use.
	 */
	opending = sc->sc_txpending;
	firsttx = EPIC_NEXTTX(sc->sc_txlast);

	/*
	 * Loop through the send queue, setting up transmit descriptors
	 * until we drain the queue, or use up all available transmit
	 * descriptors.
	 */
	while (sc->sc_txpending < EPIC_NTXDESC) {
		/*
		 * Grab a packet off the queue.
		 */
		IF_DEQUEUE(&ifp->if_snd, m0);
		if (m0 == NULL)
			break;

		/*
		 * Get the last and next available transmit descriptor.
		 */
		nexttx = EPIC_NEXTTX(sc->sc_txlast);
		txd = EPIC_CDTX(sc, nexttx);
		fr = EPIC_CDFL(sc, nexttx);
		ds = EPIC_DSTX(sc, nexttx);
		dmamap = ds->ds_dmamap;

		/*
		 * Load the DMA map.  If this fails, the packet either
		 * didn't fit in the alloted number of frags, or we were
		 * short on resources.  In this case, we'll copy and try
		 * again.
		 */
		if (bus_dmamap_load_mbuf(sc->sc_dmat, dmamap, m0,
		    BUS_DMA_NOWAIT) != 0) {
			MGETHDR(m, M_DONTWAIT, MT_DATA);
			if (m == NULL) {
				printf("%s: unable to allocate Tx mbuf\n",
				    sc->sc_dev.dv_xname);
				IF_PREPEND(&ifp->if_snd, m0);
				break;
			}
			if (m0->m_pkthdr.len > MHLEN) {
				MCLGET(m, M_DONTWAIT);
				if ((m->m_flags & M_EXT) == 0) {
					printf("%s: unable to allocate Tx "
					    "cluster\n", sc->sc_dev.dv_xname);
					m_freem(m);
					IF_PREPEND(&ifp->if_snd, m0);
					break;
				}
			}
			m_copydata(m0, 0, m0->m_pkthdr.len, mtod(m, caddr_t));
			m->m_pkthdr.len = m->m_len = m0->m_pkthdr.len;
			m_freem(m0);
			m0 = m;
			error = bus_dmamap_load_mbuf(sc->sc_dmat, dmamap,
			    m0, BUS_DMA_NOWAIT);
			if (error) {
				printf("%s: unable to load Tx buffer, "
				    "error = %d\n", sc->sc_dev.dv_xname, error);
				IF_PREPEND(&ifp->if_snd, m0);
				break;
			}
		}

		/* Initialize the fraglist. */
		fr->ef_nfrags = dmamap->dm_nsegs;
		for (seg = 0; seg < dmamap->dm_nsegs; seg++) {
			fr->ef_frags[seg].ef_addr =
			    dmamap->dm_segs[seg].ds_addr;
			fr->ef_frags[seg].ef_length =
			    dmamap->dm_segs[seg].ds_len;
		}

		EPIC_CDFLSYNC(sc, nexttx, BUS_DMASYNC_PREWRITE);

		/* Sync the DMA map. */
		bus_dmamap_sync(sc->sc_dmat, dmamap, 0, dmamap->dm_mapsize,
		    BUS_DMASYNC_PREWRITE);

		/*
		 * Store a pointer to the packet so we can free it later.
		 */
		ds->ds_mbuf = m0;

		/*
		 * Fill in the transmit descriptor.  The EPIC doesn't
		 * auto-pad, so we have to do this ourselves.
		 */
		txd->et_control = ET_TXCTL_LASTDESC | ET_TXCTL_FRAGLIST;
		txd->et_txlength = max(m0->m_pkthdr.len,
		    ETHER_MIN_LEN - ETHER_CRC_LEN);

		/*
		 * If this is the first descriptor we're enqueueing,
		 * don't give it to the EPIC yet.  That could cause
		 * a race condition.  We'll do it below.
		 */
		if (nexttx == firsttx)
			txd->et_txstatus = 0;
		else
			txd->et_txstatus = ET_TXSTAT_OWNER;

		EPIC_CDTXSYNC(sc, nexttx,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/* Advance the tx pointer. */
		sc->sc_txpending++;
		sc->sc_txlast = nexttx;

#if NBPFILTER > 0
		/*
		 * Pass the packet to any BPF listeners.
		 */
		if (ifp->if_bpf)
			bpf_mtap(ifp->if_bpf, m0);
#endif
	}

	if (sc->sc_txpending == EPIC_NTXDESC) {
		/* No more slots left; notify upper layer. */
		ifp->if_flags |= IFF_OACTIVE;
	}

	if (sc->sc_txpending != opending) {
		/*
		 * We enqueued packets.  If the transmitter was idle,
		 * reset the txdirty pointer.
		 */
		if (opending == 0)
			sc->sc_txdirty = firsttx;

		/*
		 * Cause a transmit interrupt to happen on the
		 * last packet we enqueued.
		 */
		EPIC_CDTX(sc, sc->sc_txlast)->et_control |= ET_TXCTL_IAF;
		EPIC_CDTXSYNC(sc, sc->sc_txlast,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/*
		 * The entire packet chain is set up.  Give the
		 * first descriptor to the EPIC now.
		 */
		EPIC_CDTX(sc, firsttx)->et_txstatus = ET_TXSTAT_OWNER;
		EPIC_CDTXSYNC(sc, firsttx,
		    BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);

		/* Start the transmitter. */
		bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_COMMAND,
		    COMMAND_TXQUEUED);

		/* Set a watchdog timer in case the chip flakes out. */
		ifp->if_timer = 5;
	}
}

/*
 * Watchdog timer handler.
 * [ifnet interface function]
 */
void
epic_watchdog(ifp)
	struct ifnet *ifp;
{
	struct epic_softc *sc = ifp->if_softc;

	printf("%s: device timeout\n", sc->sc_dev.dv_xname);
	ifp->if_oerrors++;

	(void) epic_init(sc);
}

/*
 * Handle control requests from the operator.
 * [ifnet interface function]
 */
int
epic_ioctl(ifp, cmd, data)
	struct ifnet *ifp;
	u_long cmd;
	caddr_t data;
{
	struct epic_softc *sc = ifp->if_softc;
	struct ifreq *ifr = (struct ifreq *)data;
	struct ifaddr *ifa = (struct ifaddr *)data;
	int s, error = 0;

	s = splnet();

	switch (cmd) {
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;

		switch (ifa->ifa_addr->sa_family) {
#ifdef INET
		case AF_INET:
			if ((error = epic_init(sc)) != 0)
				break;
			arp_ifinit(ifp, ifa);
			break;
#endif /* INET */
#ifdef NS
		case AF_NS:
		    {
			struct ns_addr *ina = &IA_SNS(ifa)->sns_addr;

			if (ns_nullhost(*ina))
				ina->x_host = *(union ns_host *)
				    LLADDR(ifp->if_sadl);
			else
				bcopy(ina->x_host.c_host, LLADDR(ifp->if_sadl),
				    ifp->if_addrlen);
			/* Set new address. */
			error = epic_init(sc);
			break;
		    }
#endif /* NS */
		default:
			error = epic_init(sc);
			break;
		}
		break;

	case SIOCSIFMTU:
		if (ifr->ifr_mtu > ETHERMTU)
			error = EINVAL;
		else
			ifp->if_mtu = ifr->ifr_mtu;
		break;

	case SIOCSIFFLAGS:
		if ((ifp->if_flags & IFF_UP) == 0 &&
		    (ifp->if_flags & IFF_RUNNING) != 0) {
			/*
			 * If interface is marked down and it is running, then
			 * stop it.
			 */
			epic_stop(sc, 1);
		} else if ((ifp->if_flags & IFF_UP) != 0 &&
			   (ifp->if_flags & IFF_RUNNING) == 0) {
			/*
			 * If interfase it marked up and it is stopped, then
			 * start it.
			 */
			error = epic_init(sc);
		} else if ((ifp->if_flags & IFF_UP) != 0) {
			/*
			 * Reset the interface to pick up changes in any other
			 * flags that affect the hardware state.
			 */
			error = epic_init(sc);
		}
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		error = (cmd == SIOCADDMULTI) ?
		    ether_addmulti(ifr, &sc->sc_ethercom) :
		    ether_delmulti(ifr, &sc->sc_ethercom);

		if (error == ENETRESET) {
			/*
			 * Multicast list has changed; set the hardware filter
			 * accordingly.  Update our idea of the current media;
			 * epic_set_mchash() needs to know what it is.
			 */
			mii_pollstat(&sc->sc_mii);
			epic_set_mchash(sc);
			error = 0;
		}
		break;

	case SIOCSIFMEDIA:
	case SIOCGIFMEDIA:
		error = ifmedia_ioctl(ifp, ifr, &sc->sc_mii.mii_media, cmd);
		break;

	default:
		error = EINVAL;
		break;
	}

	splx(s);
	return (error);
}

/*
 * Interrupt handler.
 */
int
epic_intr(arg)
	void *arg;
{
	struct epic_softc *sc = arg;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct ether_header *eh;
	struct epic_rxdesc *rxd;
	struct epic_txdesc *txd;
	struct epic_descsoft *ds;
	struct mbuf *m;
	u_int32_t intstat;
	int i, len, claimed = 0;

 top:
	/*
	 * Get the interrupt status from the EPIC.
	 */
	intstat = bus_space_read_4(sc->sc_st, sc->sc_sh, EPIC_INTSTAT);
	if ((intstat & INTSTAT_INT_ACTV) == 0)
		return (claimed);

	claimed = 1;

	/*
	 * Acknowledge the interrupt.
	 */
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_INTSTAT,
	    intstat & INTMASK);

	/*
	 * Check for receive interrupts.
	 */
	if (intstat & (INTSTAT_RCC | INTSTAT_RXE | INTSTAT_RQE)) {
		for (i = sc->sc_rxptr;; i = EPIC_NEXTRX(i)) {
			rxd = EPIC_CDRX(sc, i);
			ds = EPIC_DSRX(sc, i);

			EPIC_CDRXSYNC(sc, i,
			    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);

			if (rxd->er_rxstatus & ER_RXSTAT_OWNER) {
				/*
				 * We have processed all of the
				 * receive buffers.
				 */
				break;
			}

			/*
			 * Make sure the packet arrived intact.  If an error
			 * occurred, update stats and reset the descriptor.
			 * The buffer will be reused the next time the
			 * descriptor comes up in the ring.
			 */
			if ((rxd->er_rxstatus & ER_RXSTAT_PKTINTACT) == 0) {
				if (rxd->er_rxstatus & ER_RXSTAT_CRCERROR)
					printf("%s: CRC error\n",
					    sc->sc_dev.dv_xname);
				if (rxd->er_rxstatus & ER_RXSTAT_ALIGNERROR)
					printf("%s: alignment error\n",
					    sc->sc_dev.dv_xname);
				ifp->if_ierrors++;
				EPIC_INIT_RXDESC(sc, i);
				continue;
			}

			bus_dmamap_sync(sc->sc_dmat, ds->ds_dmamap, 0,
			    ds->ds_dmamap->dm_mapsize, BUS_DMASYNC_POSTREAD);

			/*
			 * The EPIC includes the CRC with every packet;
			 * trim it.
			 */
			len = rxd->er_rxlength - ETHER_CRC_LEN;

			if (len < sizeof(struct ether_header)) {
				/*
				 * Runt packet; drop it now.
				 */
				ifp->if_ierrors++;
				EPIC_INIT_RXDESC(sc, i);
				bus_dmamap_sync(sc->sc_dmat, ds->ds_dmamap, 0,
				    ds->ds_dmamap->dm_mapsize,
				    BUS_DMASYNC_PREREAD);
				continue;
			}

			/*
			 * If the packet is small enough to fit in a
			 * single header mbuf, allocate one and copy
			 * the data into it.  This greatly reduces
			 * memory consumption when we receive lots
			 * of small packets.
			 *
			 * Otherwise, we add a new buffer to the receive
			 * chain.  If this fails, we drop the packet and
			 * recycle the old buffer.
			 */
			if (epic_copy_small != 0 && len <= MHLEN) {
				MGETHDR(m, M_DONTWAIT, MT_DATA);
				if (m == NULL)
					goto dropit;
				memcpy(mtod(m, caddr_t),
				    mtod(ds->ds_mbuf, caddr_t), len);
				EPIC_INIT_RXDESC(sc, i);
				bus_dmamap_sync(sc->sc_dmat, ds->ds_dmamap, 0,
				    ds->ds_dmamap->dm_mapsize,
				    BUS_DMASYNC_PREREAD);
			} else {
				m = ds->ds_mbuf;
				if (epic_add_rxbuf(sc, i) != 0) {
 dropit:
					ifp->if_ierrors++;
					EPIC_INIT_RXDESC(sc, i);
					bus_dmamap_sync(sc->sc_dmat,
					    ds->ds_dmamap, 0,
					    ds->ds_dmamap->dm_mapsize,
					    BUS_DMASYNC_PREREAD);
					continue;
				}
			}

			m->m_pkthdr.rcvif = ifp;
			m->m_pkthdr.len = m->m_len = len;
			eh = mtod(m, struct ether_header *);

#if NBPFILTER > 0
			/*
			 * Pass this up to any BPF listeners, but only
			 * pass it up the stack if its for us.
			 */
			if (ifp->if_bpf) {
				bpf_mtap(ifp->if_bpf, m);
				if ((ifp->if_flags & IFF_PROMISC) != 0 &&
				    bcmp(LLADDR(ifp->if_sadl), eh->ether_dhost,
					 ETHER_ADDR_LEN) != 0 &&
				    (rxd->er_rxstatus &
				     (ER_RXSTAT_BCAST|ER_RXSTAT_MCAST)) == 0) {
					m_freem(m);
					continue;
				}
			}
#endif /* NPBFILTER > 0 */
			
			/* Pass it on. */
			(*ifp->if_input)(ifp, m);
			ifp->if_ipackets++;
		}

		/* Update the recieve pointer. */
		sc->sc_rxptr = i;

		/*
		 * Check for receive queue underflow.
		 */
		if (intstat & INTSTAT_RQE) {
			printf("%s: receiver queue empty\n",
			    sc->sc_dev.dv_xname);
			/*
			 * Ring is already built; just restart the
			 * receiver.
			 */
			bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_PRCDAR,
			    EPIC_CDRXADDR(sc, sc->sc_rxptr));
			bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_COMMAND,
			    COMMAND_RXQUEUED | COMMAND_START_RX);
		}
	}

	/*
	 * Check for transmission complete interrupts.
	 */
	if (intstat & (INTSTAT_TXC | INTSTAT_TXU)) {
		ifp->if_flags &= ~IFF_OACTIVE;
		for (i = sc->sc_txdirty; sc->sc_txpending != 0;
		     i = EPIC_NEXTTX(i), sc->sc_txpending--) {
			txd = EPIC_CDTX(sc, i);
			ds = EPIC_DSTX(sc, i);

			EPIC_CDTXSYNC(sc, i,
			    BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE);

			if (txd->et_txstatus & ET_TXSTAT_OWNER)
				break;

			EPIC_CDFLSYNC(sc, i, BUS_DMASYNC_POSTWRITE);

			bus_dmamap_sync(sc->sc_dmat, ds->ds_dmamap,
			    0, ds->ds_dmamap->dm_mapsize,
			    BUS_DMASYNC_POSTWRITE);
			bus_dmamap_unload(sc->sc_dmat, ds->ds_dmamap);
			m_freem(ds->ds_mbuf);
			ds->ds_mbuf = NULL;

			/*
			 * Check for errors and collisions.
			 */
			if ((txd->et_txstatus & ET_TXSTAT_PACKETTX) == 0)
				ifp->if_oerrors++;
			else
				ifp->if_opackets++;
			ifp->if_collisions +=
			    TXSTAT_COLLISIONS(txd->et_txstatus);
			if (txd->et_txstatus & ET_TXSTAT_CARSENSELOST)
				printf("%s: lost carrier\n",
				    sc->sc_dev.dv_xname);
		}
		
		/* Update the dirty transmit buffer pointer. */
		sc->sc_txdirty = i;

		/*
		 * Cancel the watchdog timer if there are no pending
		 * transmissions.
		 */
		if (sc->sc_txpending == 0)
			ifp->if_timer = 0;

		/*
		 * Kick the transmitter after a DMA underrun.
		 */
		if (intstat & INTSTAT_TXU) {
			printf("%s: transmit underrun\n", sc->sc_dev.dv_xname);
			bus_space_write_4(sc->sc_st, sc->sc_sh,
			    EPIC_COMMAND, COMMAND_TXUGO);
			if (sc->sc_txpending)
				bus_space_write_4(sc->sc_st, sc->sc_sh,
				    EPIC_COMMAND, COMMAND_TXQUEUED);
		}

		/*
		 * Try to get more packets going.
		 */
		epic_start(ifp);
	}

	/*
	 * Check for fatal interrupts.
	 */
	if (intstat & INTSTAT_FATAL_INT) {
		if (intstat & INTSTAT_PTA)
			printf("%s: PCI target abort error\n",
			    sc->sc_dev.dv_xname);
		else if (intstat & INTSTAT_PMA)
			printf("%s: PCI master abort error\n",
			    sc->sc_dev.dv_xname);
		else if (intstat & INTSTAT_APE)
			printf("%s: PCI address parity error\n",
			    sc->sc_dev.dv_xname);
		else if (intstat & INTSTAT_DPE)
			printf("%s: PCI data parity error\n",
			    sc->sc_dev.dv_xname);
		else
			printf("%s: unknown fatal error\n",
			    sc->sc_dev.dv_xname);
		(void) epic_init(sc);
	}

	/*
	 * Check for more interrupts.
	 */
	goto top;
}

/*
 * One second timer, used to tick the MII.
 */
void
epic_tick(arg)
	void *arg;
{
	struct epic_softc *sc = arg;
	int s;

	s = splnet();
	mii_tick(&sc->sc_mii);
	splx(s);

	timeout(epic_tick, sc, hz);
}

/*
 * Fixup the clock source on the EPIC.
 */
void
epic_fixup_clock_source(sc)
	struct epic_softc *sc;
{
	int i;

	/*
	 * According to SMC Application Note 7-15, the EPIC's clock
	 * source is incorrect following a reset.  This manifests itself
	 * as failure to recognize when host software has written to
	 * a register on the EPIC.  The appnote recommends issuing at
	 * least 16 consecutive writes to the CLOCK TEST bit to correctly
	 * configure the clock source.
	 */
	for (i = 0; i < 16; i++)
		bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_TEST,
		    TEST_CLOCKTEST);
}

/*
 * Perform a soft reset on the EPIC.
 */
void
epic_reset(sc)
	struct epic_softc *sc;
{

	epic_fixup_clock_source(sc);

	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_GENCTL, 0);
	delay(100);
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_GENCTL, GENCTL_SOFTRESET);
	delay(100);

	epic_fixup_clock_source(sc);
}

/*
 * Initialize the interface.  Must be called at splnet().
 */
int
epic_init(sc)
	struct epic_softc *sc;
{
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	u_int8_t *enaddr = LLADDR(ifp->if_sadl);
	struct epic_txdesc *txd;
	struct epic_descsoft *ds;
	u_int32_t genctl, reg0;
	int i, error = 0;

	/*
	 * Cancel any pending I/O.
	 */
	epic_stop(sc, 0);

	/*
	 * Reset the EPIC to a known state.
	 */
	epic_reset(sc);

	/*
	 * Magical mystery initialization.
	 */
	bus_space_write_4(st, sh, EPIC_TXTEST, 0);

	/*
	 * Initialize the EPIC genctl register:
	 *
	 *	- 64 byte receive FIFO threshold
	 *	- automatic advance to next receive frame
	 */
	genctl = GENCTL_RX_FIFO_THRESH0 | GENCTL_ONECOPY;
#if BYTE_ORDER == BIG_ENDIAN
	genctl |= GENCTL_BIG_ENDIAN;
#endif
	bus_space_write_4(st, sh, EPIC_GENCTL, genctl);

	/*
	 * Reset the MII bus and PHY.
	 */
	reg0 = bus_space_read_4(st, sh, EPIC_NVCTL);
	bus_space_write_4(st, sh, EPIC_NVCTL, reg0 | NVCTL_GPIO1 | NVCTL_GPOE1);
	bus_space_write_4(st, sh, EPIC_MIICFG, MIICFG_ENASER);
	bus_space_write_4(st, sh, EPIC_GENCTL, genctl | GENCTL_RESET_PHY);
	delay(100);
	bus_space_write_4(st, sh, EPIC_GENCTL, genctl);
	delay(100);
	bus_space_write_4(st, sh, EPIC_NVCTL, reg0);

	/*
	 * Initialize Ethernet address.
	 */
	reg0 = enaddr[1] << 8 | enaddr[0];
	bus_space_write_4(st, sh, EPIC_LAN0, reg0);
	reg0 = enaddr[3] << 8 | enaddr[2];
	bus_space_write_4(st, sh, EPIC_LAN1, reg0);
	reg0 = enaddr[5] << 8 | enaddr[4];
	bus_space_write_4(st, sh, EPIC_LAN2, reg0);

	/*
	 * Initialize receive control.  Remember the external buffer
	 * size setting.
	 */
	reg0 = bus_space_read_4(st, sh, EPIC_RXCON) &
	    (RXCON_EXTBUFSIZESEL1 | RXCON_EXTBUFSIZESEL0);
	reg0 |= (RXCON_RXMULTICAST | RXCON_RXBROADCAST);
	if (ifp->if_flags & IFF_PROMISC)
		reg0 |= RXCON_PROMISCMODE;
	bus_space_write_4(st, sh, EPIC_RXCON, reg0);

	/* Set the current media. */
	mii_mediachg(&sc->sc_mii);

	/* Set up the multicast hash table. */
	epic_set_mchash(sc);

	/*
	 * Initialize the transmit descriptor ring.  txlast is initialized
	 * to the end of the list so that it will wrap around to the first
	 * descriptor when the first packet is transmitted.
	 */
	for (i = 0; i < EPIC_NTXDESC; i++) {
		txd = EPIC_CDTX(sc, i);
		memset(txd, 0, sizeof(struct epic_txdesc));
		txd->et_bufaddr = EPIC_CDFLADDR(sc, i);
		txd->et_nextdesc = EPIC_CDTXADDR(sc, EPIC_NEXTTX(i));
		EPIC_CDTXSYNC(sc, i, BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE);
	}
	sc->sc_txpending = 0;
	sc->sc_txdirty = 0;
	sc->sc_txlast = EPIC_NTXDESC - 1;

	/*
	 * Initialize the receive descriptor ring.
	 */
	for (i = 0; i < EPIC_NRXDESC; i++) {
		ds = EPIC_DSRX(sc, i);
		if (ds->ds_mbuf == NULL) {
			if ((error = epic_add_rxbuf(sc, i)) != 0) {
				printf("%s: unable to allocate or map rx "
				    "buffer %d error = %d\n",
				    sc->sc_dev.dv_xname, i, error);
				/*
				 * XXX Should attempt to run with fewer receive
				 * XXX buffers instead of just failing.
				 */
				epic_rxdrain(sc);
				goto out;
			}
		}
	}
	sc->sc_rxptr = 0;

	/*
	 * Initialize the interrupt mask and enable interrupts.
	 */
	bus_space_write_4(st, sh, EPIC_INTMASK, INTMASK);
	bus_space_write_4(st, sh, EPIC_GENCTL, genctl | GENCTL_INTENA);

	/*
	 * Give the transmit and receive rings to the EPIC.
	 */
	bus_space_write_4(st, sh, EPIC_PTCDAR,
	    EPIC_CDTXADDR(sc, EPIC_NEXTTX(sc->sc_txlast)));
	bus_space_write_4(st, sh, EPIC_PRCDAR,
	    EPIC_CDRXADDR(sc, sc->sc_rxptr));

	/*
	 * Set the EPIC in motion.
	 */
	bus_space_write_4(st, sh, EPIC_COMMAND,
	    COMMAND_RXQUEUED | COMMAND_START_RX);

	/*
	 * ...all done!
	 */
	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;

	/*
	 * Start the one second clock.
	 */
	timeout(epic_tick, sc, hz);

	/*
	 * Attempt to start output on the interface.
	 */
	epic_start(ifp);

 out:
	if (error)
		printf("%s: interface not running\n", sc->sc_dev.dv_xname);
	return (error);
}

/*
 * Drain the receive queue.
 */
void
epic_rxdrain(sc)
	struct epic_softc *sc;
{
	struct epic_descsoft *ds;
	int i;

	for (i = 0; i < EPIC_NRXDESC; i++) {
		ds = EPIC_DSRX(sc, i);
		if (ds->ds_mbuf != NULL) {
			bus_dmamap_unload(sc->sc_dmat, ds->ds_dmamap);
			m_freem(ds->ds_mbuf);
			ds->ds_mbuf = NULL;
		}
	}
}

/*
 * Stop transmission on the interface.
 */
void
epic_stop(sc, drain)
	struct epic_softc *sc;
	int drain;
{
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct epic_descsoft *ds;
	u_int32_t reg;
	int i;

	/*
	 * Stop the one second clock.
	 */
	untimeout(epic_tick, sc);

	/* Down the MII. */
	mii_down(&sc->sc_mii);

	/* Paranoia... */
	epic_fixup_clock_source(sc);

	/*
	 * Disable interrupts.
	 */
	reg = bus_space_read_4(st, sh, EPIC_GENCTL);
	bus_space_write_4(st, sh, EPIC_GENCTL, reg & ~GENCTL_INTENA);
	bus_space_write_4(st, sh, EPIC_INTMASK, 0);

	/*
	 * Stop the DMA engine and take the receiver off-line.
	 */
	bus_space_write_4(st, sh, EPIC_COMMAND, COMMAND_STOP_RDMA |
	    COMMAND_STOP_TDMA | COMMAND_STOP_RX);

	/*
	 * Release any queued transmit buffers.
	 */
	for (i = 0; i < EPIC_NTXDESC; i++) {
		ds = EPIC_DSTX(sc, i);
		if (ds->ds_mbuf != NULL) {
			bus_dmamap_unload(sc->sc_dmat, ds->ds_dmamap);
			m_freem(ds->ds_mbuf);
			ds->ds_mbuf = NULL;
		}
	}

	if (drain) {
		/*
		 * Release the receive buffers.
		 */
		epic_rxdrain(sc);
	}

	/*
	 * Mark the interface down and cancel the watchdog timer.
	 */
	ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
	ifp->if_timer = 0;
}

/*
 * Read the EPIC Serial EEPROM.
 */
void
epic_read_eeprom(sc, word, wordcnt, data)
	struct epic_softc *sc;
	int word, wordcnt;
	u_int16_t *data;
{
	bus_space_tag_t st = sc->sc_st;
	bus_space_handle_t sh = sc->sc_sh;
	u_int16_t reg;
	int i, x;

#define	EEPROM_WAIT_READY(st, sh) \
	while ((bus_space_read_4((st), (sh), EPIC_EECTL) & EECTL_EERDY) == 0) \
		/* nothing */

	/*
	 * Enable the EEPROM.
	 */
	bus_space_write_4(st, sh, EPIC_EECTL, EECTL_ENABLE);
	EEPROM_WAIT_READY(st, sh);

	for (i = 0; i < wordcnt; i++) {
		/* Send CHIP SELECT for one clock tick. */
		bus_space_write_4(st, sh, EPIC_EECTL, EECTL_ENABLE|EECTL_EECS);
		EEPROM_WAIT_READY(st, sh);

		/* Shift in the READ opcode. */
		for (x = 3; x > 0; x--) {
			reg = EECTL_ENABLE|EECTL_EECS;
			if (EPIC_EEPROM_OPC_READ & (1 << (x - 1)))
				reg |= EECTL_EEDI;
			bus_space_write_4(st, sh, EPIC_EECTL, reg);
			EEPROM_WAIT_READY(st, sh);
			bus_space_write_4(st, sh, EPIC_EECTL, reg|EECTL_EESK);
			EEPROM_WAIT_READY(st, sh);
			bus_space_write_4(st, sh, EPIC_EECTL, reg);
			EEPROM_WAIT_READY(st, sh);
		}

		/* Shift in address. */
		for (x = 6; x > 0; x--) {
			reg = EECTL_ENABLE|EECTL_EECS;
			if ((word + i) & (1 << (x - 1)))
				reg |= EECTL_EEDI; 
			bus_space_write_4(st, sh, EPIC_EECTL, reg);
			EEPROM_WAIT_READY(st, sh);
			bus_space_write_4(st, sh, EPIC_EECTL, reg|EECTL_EESK);
			EEPROM_WAIT_READY(st, sh);
			bus_space_write_4(st, sh, EPIC_EECTL, reg);
			EEPROM_WAIT_READY(st, sh);
		}

		/* Shift out data. */
		reg = EECTL_ENABLE|EECTL_EECS;
		data[i] = 0;
		for (x = 16; x > 0; x--) {
			bus_space_write_4(st, sh, EPIC_EECTL, reg|EECTL_EESK);
			EEPROM_WAIT_READY(st, sh);
			if (bus_space_read_4(st, sh, EPIC_EECTL) & EECTL_EEDO)
				data[i] |= (1 << (x - 1));
			bus_space_write_4(st, sh, EPIC_EECTL, reg);
			EEPROM_WAIT_READY(st, sh);
		}

		/* Clear CHIP SELECT. */
		bus_space_write_4(st, sh, EPIC_EECTL, EECTL_ENABLE);
		EEPROM_WAIT_READY(st, sh);
	}

	/*
	 * Disable the EEPROM.
	 */
	bus_space_write_4(st, sh, EPIC_EECTL, 0);

#undef EEPROM_WAIT_READY
}

/*
 * Add a receive buffer to the indicated descriptor.
 */
int
epic_add_rxbuf(sc, idx)	
	struct epic_softc *sc;
	int idx;
{
	struct epic_descsoft *ds = EPIC_DSRX(sc, idx);
	struct mbuf *m;
	int error;

	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if (m == NULL)
		return (ENOBUFS);

	MCLGET(m, M_DONTWAIT);
	if ((m->m_flags & M_EXT) == 0) {
		m_freem(m);
		return (ENOBUFS);
	}

	if (ds->ds_mbuf != NULL)
		bus_dmamap_unload(sc->sc_dmat, ds->ds_dmamap);

	ds->ds_mbuf = m;

	error = bus_dmamap_load(sc->sc_dmat, ds->ds_dmamap,
	    m->m_ext.ext_buf, m->m_ext.ext_size, NULL, BUS_DMA_NOWAIT);
	if (error) {
		printf("%s: can't load rx DMA map %d, error = %d\n",
		    sc->sc_dev.dv_xname, idx, error);
		panic("epic_add_rxbuf");	/* XXX */
	}

	bus_dmamap_sync(sc->sc_dmat, ds->ds_dmamap, 0,
	    ds->ds_dmamap->dm_mapsize, BUS_DMASYNC_PREREAD);

	EPIC_INIT_RXDESC(sc, idx);

	return (0);
}

/*
 * Set the EPIC multicast hash table.
 *
 * NOTE: We rely on a recently-updated mii_media_active here!
 */
void
epic_set_mchash(sc)
	struct epic_softc *sc;
{
	struct ethercom *ec = &sc->sc_ethercom;
	struct ifnet *ifp = &sc->sc_ethercom.ec_if;
	struct ether_multi *enm;
	struct ether_multistep step;
	u_int8_t *cp;
	u_int32_t crc, mchash[4];
	int len;
	static const u_int32_t crctab[] = {
		0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
		0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
		0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
		0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
	};

	/*
	 * Set up the multicast address filter by passing all multicast
	 * addresses through a CRC generator, and then using the high-order
	 * 6 bits as an index into the 64 bit multicast hash table (only
	 * the lower 16 bits of each 32 bit multicast hash register are
	 * valid).  The high order bit selects the register, while the
	 * rest of the bits select the bit within the register.
	 */

	if (ifp->if_flags & IFF_PROMISC)
		goto allmulti;

	if (IFM_SUBTYPE(sc->sc_mii.mii_media_active) == IFM_10_T) {
		/* XXX hardware bug in 10Mbps mode. */
		goto allmulti;
	}

	mchash[0] = mchash[1] = mchash[2] = mchash[3] = 0;

	ETHER_FIRST_MULTI(step, ec, enm);
	while (enm != NULL) {
		if (bcmp(enm->enm_addrlo, enm->enm_addrhi, ETHER_ADDR_LEN)) {
			/*
			 * We must listen to a range of multicast addresses.
			 * For now, just accept all multicasts, rather than
			 * trying to set only those filter bits needed to match
			 * the range.  (At this time, the only use of address
			 * ranges is for IP multicast routing, for which the
			 * range is big enough to require all bits set.)
			 */
			goto allmulti;
		}

		cp = enm->enm_addrlo;
		crc = 0xffffffff;
		for (len = sizeof(enm->enm_addrlo); --len >= 0;) {
			crc ^= *cp++;
			crc = (crc >> 4) ^ crctab[crc & 0xf];
			crc = (crc >> 4) ^ crctab[crc & 0xf];
		}
		/* Just want the 6 most significant bits. */
		crc >>= 26;

		/* Set the corresponding bit in the hash table. */
		mchash[crc >> 4] |= 1 << (crc & 0xf);

		ETHER_NEXT_MULTI(step, enm);
	}

	ifp->if_flags &= ~IFF_ALLMULTI;
	goto sethash;

 allmulti:
	ifp->if_flags |= IFF_ALLMULTI;
	mchash[0] = mchash[1] = mchash[2] = mchash[3] = 0xffff;

 sethash:
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MC0, mchash[0]);
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MC1, mchash[1]);
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MC2, mchash[2]);
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MC3, mchash[3]);
}

/*
 * Wait for the MII to become ready.
 */
int
epic_mii_wait(sc, rw)
	struct epic_softc *sc;
	u_int32_t rw;
{
	int i;

	for (i = 0; i < 50; i++) {
		if ((bus_space_read_4(sc->sc_st, sc->sc_sh, EPIC_MMCTL) & rw)
		    == 0)
			break;
		delay(2);
	}
	if (i == 50) {
		printf("%s: MII timed out\n", sc->sc_dev.dv_xname);
		return (1);
	}

	return (0);
}

/*
 * Read from the MII.
 */
int
epic_mii_read(self, phy, reg)
	struct device *self;
	int phy, reg;
{
	struct epic_softc *sc = (struct epic_softc *)self;

	if (epic_mii_wait(sc, MMCTL_WRITE))
		return (0);

	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MMCTL,
	    MMCTL_ARG(phy, reg, MMCTL_READ));

	if (epic_mii_wait(sc, MMCTL_READ))
		return (0);

	return (bus_space_read_4(sc->sc_st, sc->sc_sh, EPIC_MMDATA) &
	    MMDATA_MASK);
}

/*
 * Write to the MII.
 */
void
epic_mii_write(self, phy, reg, val)
	struct device *self;
	int phy, reg, val;
{
	struct epic_softc *sc = (struct epic_softc *)self;

	if (epic_mii_wait(sc, MMCTL_WRITE))
		return;

	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MMDATA, val);
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_MMCTL,
	    MMCTL_ARG(phy, reg, MMCTL_WRITE));
}

/*
 * Callback from PHY when media changes.
 */
void
epic_statchg(self)
	struct device *self;
{
	struct epic_softc *sc = (struct epic_softc *)self;
	u_int32_t txcon;

	/*
	 * Update loopback bits in TXCON to reflect duplex mode.
	 */
	txcon = bus_space_read_4(sc->sc_st, sc->sc_sh, EPIC_TXCON);
	if (sc->sc_mii.mii_media_active & IFM_FDX)
		txcon |= (TXCON_LOOPBACK_D1|TXCON_LOOPBACK_D2);
	else
		txcon &= ~(TXCON_LOOPBACK_D1|TXCON_LOOPBACK_D2);
	bus_space_write_4(sc->sc_st, sc->sc_sh, EPIC_TXCON, txcon);

	/*
	 * There is a multicast filter bug in 10Mbps mode.  Kick the
	 * multicast filter in case the speed changed.
	 */
	epic_set_mchash(sc);

	/* XXX Update ifp->if_baudrate */
}

/*
 * Callback from ifmedia to request current media status.
 */
void
epic_mediastatus(ifp, ifmr)
	struct ifnet *ifp;
	struct ifmediareq *ifmr;
{
	struct epic_softc *sc = ifp->if_softc;

	mii_pollstat(&sc->sc_mii);
	ifmr->ifm_status = sc->sc_mii.mii_media_status;
	ifmr->ifm_active = sc->sc_mii.mii_media_active;
}

/*
 * Callback from ifmedia to request new media setting.
 */
int
epic_mediachange(ifp)
	struct ifnet *ifp;
{
	struct epic_softc *sc = ifp->if_softc;

	if (ifp->if_flags & IFF_UP)
		mii_mediachg(&sc->sc_mii);
	return (0);
}
