/*	$NetBSD: if_ae.c,v 1.24 1995/04/19 04:43:36 briggs Exp $	*/

/*
 * Device driver for National Semiconductor DS8390/WD83C690 based ethernet
 * adapters.
 *
 * Copyright (c) 1994, 1995 Charles M. Hannum.  All rights reserved.
 *
 * Copyright (C) 1993, David Greenman.  This software may be used, modified,
 * copied, distributed, and sold, in both source and binary form provided that
 * the above copyright and these terms are retained.  Under no circumstances is
 * the author responsible for the proper functioning of this software, nor does
 * the author assume any responsibility for damages incurred with its use.
 *
 * Adapted for MacBSD by Brad Parker <brad@fcr.com>.
 *
 * Currently supports:
 *	Apples NB Ethernet card
 *	Interlan A310 Nubus Ethernet card
 *	Cayman Systems GatorCard
 *	Asante MacCon II/E
 */

#include "bpfilter.h"

#include <sys/param.h>
#include <sys/types.h>
#include <sys/systm.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <sys/device.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/netisr.h>

#ifdef INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#endif

#ifdef NS
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

#include "../mac68k/via.h"
#include "nubus.h"
#include <dev/ic/dp8390.h>
#include "if_aereg.h"

/*
 * ae_softc: per line info and status
 */
struct	ae_softc {
	struct	device sc_dev;
/*	struct	nubusdev sc_nu;
	struct	intrhand sc_ih;	*/

	struct	arpcom sc_arpcom;	/* ethernet common */

	char	*type_str;	/* pointer to type string */
	u_char	vendor;		/* interface vendor */
	u_char	type;		/* interface type code */
	u_char	regs_rev;	/* registers are reversed */

#define	REG_MAP(sc, reg)	((sc)->regs_rev ? (0x0f-(reg))<<2 : (reg)<<2)
#define NIC_GET(sc, reg)	((sc)->nic_addr[REG_MAP(sc, reg)])
#define NIC_PUT(sc, reg, val)	((sc)->nic_addr[REG_MAP(sc, reg)] = (val))
	volatile caddr_t nic_addr; /* NIC (DS8390) I/O bus address */
	caddr_t	rom_addr;	/* on board prom address */

	u_char	cr_proto;	/* values always set in CR */

	caddr_t	mem_start;	/* shared memory start address */
	caddr_t	mem_end;	/* shared memory end address */
	u_long	mem_size;	/* total shared memory size */
	caddr_t	mem_ring;	/* start of RX ring-buffer (in smem) */

	u_char	mem_wr_short;	/* card memory requires int16 writes */

	u_char	xmit_busy;	/* transmitter is busy */
	u_char	txb_cnt;	/* Number of transmit buffers */
	u_char	txb_inuse;	/* number of TX buffers currently in-use*/

	u_char 	txb_new;	/* pointer to where new buffer will be added */
	u_char	txb_next_tx;	/* pointer to next buffer ready to xmit */
	u_short	txb_len[8];	/* buffered xmit buffer lengths */
	u_char	tx_page_start;	/* first page of TX buffer area */
	u_char	rec_page_start;	/* first page of RX ring-buffer */
	u_char	rec_page_stop;	/* last page of RX ring-buffer */
	u_char	next_packet;	/* pointer to next unread RX packet */
};

int aeprobe __P((struct device *, void *, void *));
void aeattach __P((struct device *, struct device *, void *));
void aeintr __P((struct ae_softc *));
int ae_ioctl __P((struct ifnet *, u_long, caddr_t));
void ae_start __P((struct ifnet *));
void ae_watchdog __P((/* short */));
void ae_reset __P((struct ae_softc *));
void ae_init __P((struct ae_softc *));
void ae_stop __P((struct ae_softc *));
void ae_getmcaf __P((struct arpcom *, u_long *));
u_short ae_put __P((struct ae_softc *, struct mbuf *, caddr_t));

#define inline	/* XXX for debugging porpoises */

void ae_get_packet __P((/* struct ae_softc *, caddr_t, u_short */));
static inline void ae_rint __P((struct ae_softc *));
static inline void ae_xmit __P((struct ae_softc *));
static inline caddr_t ae_ring_copy __P((/* struct ae_softc *, caddr_t, caddr_t,
					u_short */));

struct cfdriver aecd = {
	NULL, "ae", aeprobe, aeattach, DV_IFNET, sizeof(struct ae_softc)
};

#define	ETHER_MIN_LEN	64
#define ETHER_MAX_LEN	1518
#define	ETHER_ADDR_LEN	6

char ae_name[] = "8390 Nubus Ethernet card";
static char zero = 0;
static u_char ones = 0xff;

struct vendor_S {
	char	*manu;
	int	len;
	int	vendor;
} vend[] = {
	{ "Apple", 5, AE_VENDOR_APPLE },
	{ "3Com",  4, AE_VENDOR_APPLE },
	{ "Dayna", 5, AE_VENDOR_DAYNA },
	{ "Inter", 5, AE_VENDOR_INTERLAN },
	{ "Asant", 5, AE_VENDOR_ASANTE },
};

static int numvend = sizeof(vend)/sizeof(vend[0]);

/*
 * XXX These two should be moved to locore, and maybe changed to use shorts
 * instead of bytes.  The reason for these is that bcopy and bzero use longs,
 * which the ethernet cards can't handle.
 */

void
bszero(u_short *addr, int len)
{

	while (len--)
		*addr++ = 0;
}

/*
 * Memory copy, copies word at time.
 */
static inline void
word_copy(a, b, len)
	caddr_t a, b;
	int len;
{
	u_short *x = (u_short *)a,
		*y = (u_short *)b;

	len >>= 1;
	while (len--)
		*y++ = *x++;
}

/*
 * Memory copy, copies bytes at time.
 */
static inline void
byte_copy(a, b, len)
	caddr_t a, b;
	int len;
{
	while (len--)
		*b++ = *a++;
}

void
ae_id_card(nu, sc)
	struct nubus_hw	*nu;
	struct ae_softc	*sc;
{
	int i;

	/*
	 * Try to determine what type of card this is...
	 */
	sc->vendor = AE_VENDOR_UNKNOWN;
	for (i = 0; i < numvend; i++) {
		if (!strncmp(nu->Slot.manufacturer, vend[i].manu, vend[i].len)) {
			sc->vendor = vend[i].vendor;
			break;
		}
	}
	sc->type_str = (char *)(nu->Slot.manufacturer);

}

int
ae_size_card_memory(sc)
	struct ae_softc	*sc;
{
	u_short *p;
	u_short i1, i2, i3, i4;
	int size;

	p = (u_short *)sc->mem_start;

	/*
	 * very simple size memory, assuming it's installed in 8k
	 * banks; also assume it will generally mirror in upper banks
	 * if not installed.
	 */
	i1 = (8192*0)/2;
	i2 = (8192*1)/2;
	i3 = (8192*2)/2;
	i4 = (8192*3)/2;

	p[i1] = 0x1111;
	p[i2] = 0x2222;
	p[i3] = 0x3333;
	p[i4] = 0x4444;

	if (p[i1] == 0x1111 && p[i2] == 0x2222 &&
	    p[i3] == 0x3333 && p[i4] == 0x4444)
		return 8192*4;

	if ((p[i1] == 0x1111 && p[i2] == 0x2222) ||
	    (p[i1] == 0x3333 && p[i2] == 0x4444))
		return 8192*2;

	if (p[i1] == 0x1111 || p[i1] == 0x4444)
		return 8192;

	return 0;
}

int
aeprobe(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct ae_softc *sc = match;
	register struct nubus_hw *nu = aux;
	int i, memsize;
	int flags = 0;

	if (nu->Slot.type != NUBUS_NETWORK)
		return 0;

	ae_id_card(nu, sc);

	sc->regs_rev = 0;
	sc->mem_wr_short = 0;

	switch (sc->vendor) {
	      case AE_VENDOR_INTERLAN:
		sc->nic_addr = nu->addr + GC_NIC_OFFSET;
		sc->rom_addr = nu->addr + GC_ROM_OFFSET;
		sc->mem_start = nu->addr + GC_DATA_OFFSET;
		if ((memsize = ae_size_card_memory(sc)) == 0)
			return 0;

		/* reset the NIC chip */
		*((caddr_t)nu->addr + GC_RESET_OFFSET) = (char)zero;

		/* Get station address from on-board ROM */
		for (i = 0; i < ETHER_ADDR_LEN; ++i)
			sc->sc_arpcom.ac_enaddr[i] = *(sc->rom_addr + i*4);
		break;

	      case AE_VENDOR_ASANTE:
		/* memory writes require *(u_short *) */
		sc->mem_wr_short = 1;
		/* otherwise, pretend to be an apple card (fall through) */

	      case AE_VENDOR_APPLE:
		sc->regs_rev = 1;
		sc->nic_addr = nu->addr + AE_NIC_OFFSET;
		sc->rom_addr = nu->addr + AE_ROM_OFFSET;
		sc->mem_start = nu->addr + AE_DATA_OFFSET;
		if ((memsize = ae_size_card_memory(sc)) == 0)
			return (0);

		/* Get station address from on-board ROM */
		for (i = 0; i < ETHER_ADDR_LEN; ++i)
			sc->sc_arpcom.ac_enaddr[i] = *(sc->rom_addr + i*2);
		break;

	      case AE_VENDOR_DAYNA:
		printf("We think we are a Dayna card, but ");
		sc->nic_addr = nu->addr + DP_NIC_OFFSET;
		sc->rom_addr = nu->addr + DP_ROM_OFFSET;
		sc->mem_start = nu->addr + DP_DATA_OFFSET;
		memsize = 8192;

		/* Get station address from on-board ROM */
		for (i = 0; i < ETHER_ADDR_LEN; ++i)
			sc->sc_arpcom.ac_enaddr[i] = *(sc->rom_addr + i*2);
		printf("it is dangerous to continue.\n");
		return (0); /* Since we don't work yet... */
		break;

	      default:
		return (0);
		break;
	}

	sc->cr_proto = ED_CR_RD2;

	/* Allocate one xmit buffer if < 16k, two buffers otherwise. */
	if ((memsize < 16384) || (flags & AE_FLAGS_NO_DOUBLE_BUFFERING))
		sc->txb_cnt = 1;
	else
		sc->txb_cnt = 2;

	sc->tx_page_start = 0;
	sc->rec_page_start = sc->tx_page_start + sc->txb_cnt * ED_TXBUF_SIZE;
	sc->rec_page_stop = sc->tx_page_start + (memsize >> ED_PAGE_SHIFT);
	sc->mem_ring = sc->mem_start + (sc->rec_page_start << ED_PAGE_SHIFT);
	sc->mem_size = memsize;
	sc->mem_end = sc->mem_start + memsize;

	/* Now zero memory and verify that it is clear. */
	bszero((u_short *)sc->mem_start, memsize / 2);

	for (i = 0; i < memsize; ++i)
		if (sc->mem_start[i]) {
	        	printf("%s: failed to clear shared memory at %x - check configuration\n",
			    sc->sc_dev.dv_xname,
			    sc->mem_start + i);
			return (0);
		}

	return (1);
}

/*
 * Install interface into kernel networking data structures
 */
void
aeattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct ae_softc *sc = (void *)self;
	struct nubus_hw	*nu = aux;
	struct cfdata *cf = sc->sc_dev.dv_cfdata;
	struct ifnet *ifp = &sc->sc_arpcom.ac_if;

	/* Set interface to stopped condition (reset). */
	ae_stop(sc);

	/* Initialize ifnet structure. */
	ifp->if_unit = sc->sc_dev.dv_unit;
	ifp->if_name = aecd.cd_name;
	ifp->if_start = ae_start;
	ifp->if_ioctl = ae_ioctl;
	ifp->if_watchdog = ae_watchdog;
	ifp->if_flags =
	    IFF_BROADCAST | IFF_SIMPLEX | IFF_NOTRAILERS | IFF_MULTICAST;

	/* Attach the interface. */
	if_attach(ifp);
	ether_ifattach(ifp);

	/* Print additional info when attached. */
	printf(": address %s, ", ether_sprintf(sc->sc_arpcom.ac_enaddr));

	if (sc->type_str && (*sc->type_str != 0))
		printf("type %s", sc->type_str);
	else
		printf("type unknown (0x%x)", sc->type);

	printf(", %dk mem.\n", sc->mem_size / 1024);

#if NBPFILTER > 0
	bpfattach(&ifp->if_bpf, ifp, DLT_EN10MB, sizeof(struct ether_header));
#endif

	/* make sure interrupts are vectored to us */
	add_nubus_intr( (int) sc->rom_addr & 0xFF000000, aeintr, sc);
}

/*
 * Reset interface.
 */
void
ae_reset(sc)
	struct ae_softc *sc;
{
	int s;

	s = splimp();
	ae_stop(sc);
	ae_init(sc);
	splx(s);
}

/*
 * Take interface offline.
 */
void
ae_stop(sc)
	struct ae_softc *sc;
{
	int n = 5000;

	/* Stop everything on the interface, and select page 0 registers. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STP);

	/*
	 * Wait for interface to enter stopped state, but limit # of checks to
	 * 'n' (about 5ms).  It shouldn't even take 5us on modern DS8390's, but
	 * just in case it's an old one.
	 */
	while (((NIC_GET(sc, ED_P0_ISR) & ED_ISR_RST) == 0) && --n);
}

/*
 * Device timeout/watchdog routine.  Entered if the device neglects to generate
 * an interrupt after a transmit has been started on it.
 */
static	int aeintr_ctr = 0;
void
ae_watchdog(unit)
	int unit;
{
	struct ae_softc *sc = aecd.cd_devs[unit];

#if 1
/*
 * This is a kludge!  The via code seems to miss slot interrupts
 * sometimes.  This kludges around that by calling the handler
 * by hand if the watchdog is activated. -- XXX (akb)
 */
	int	i;

	i = aeintr_ctr;

	(*via2itab[1])(1);

	if (i != aeintr_ctr) {
		log(LOG_ERR, "ae%d: device timeout, recovered\n", unit);
		return;
	}
#endif

	log(LOG_ERR, "%s: device timeout\n", sc->sc_dev.dv_xname);
	++sc->sc_arpcom.ac_if.if_oerrors;

	ae_reset(sc);
}

/*
 * Initialize device.
 */
void
ae_init(sc)
	struct ae_softc *sc;
{
	struct ifnet *ifp = &sc->sc_arpcom.ac_if;
	int i, s;
	u_char command;
	u_long mcaf[2];

	/* Address not known. */
	if (ifp->if_addrlist == 0)
		return;

	/*
	 * Initialize the NIC in the exact order outlined in the NS manual.
	 * This init procedure is "mandatory"...don't change what or when
	 * things happen.
	 */
	s = splimp();

	/* Reset transmitter flags. */
	sc->xmit_busy = 0;
	sc->sc_arpcom.ac_if.if_timer = 0;

	sc->txb_inuse = 0;
	sc->txb_new = 0;
	sc->txb_next_tx = 0;

	/* Set interface for page 0, remote DMA complete, stopped. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STP);

	/*
	 * Set FIFO threshold to 8, No auto-init Remote DMA, byte
	 * order=80x86, word-wide DMA xfers,
	 */
	NIC_PUT(sc, ED_P0_DCR,
	    ED_DCR_FT1 | ED_DCR_WTS | ED_DCR_LS);

	/* Clear remote byte count registers. */
	NIC_PUT(sc, ED_P0_RBCR0, 0);
	NIC_PUT(sc, ED_P0_RBCR1, 0);

	/* Tell RCR to do nothing for now. */
	NIC_PUT(sc, ED_P0_RCR, ED_RCR_MON);

	/* Place NIC in internal loopback mode. */
	NIC_PUT(sc, ED_P0_TCR, ED_TCR_LB0);

	/* Initialize receive buffer ring. */
	NIC_PUT(sc, ED_P0_TPSR, sc->rec_page_start);
	NIC_PUT(sc, ED_P0_BNRY, sc->rec_page_start);
	NIC_PUT(sc, ED_P0_PSTART, sc->rec_page_start);
	NIC_PUT(sc, ED_P0_PSTOP, sc->rec_page_stop);

	/*
	 * Clear all interrupts.  A '1' in each bit position clears the
	 * corresponding flag.
	 */
	NIC_PUT(sc, ED_P0_ISR, 0xff);

	/*
	 * Enable the following interrupts: receive/transmit complete,
	 * receive/transmit error, and Receiver OverWrite.
	 *
	 * Counter overflow and Remote DMA complete are *not* enabled.
	 */
	NIC_PUT(sc, ED_P0_IMR,
	    ED_IMR_PRXE | ED_IMR_PTXE | ED_IMR_RXEE | ED_IMR_TXEE |
	    ED_IMR_OVWE);

	/* Program command register for page 1. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_1 | ED_CR_STP);

	/* Copy out our station address. */
	for (i = 0; i < ETHER_ADDR_LEN; ++i)
		NIC_PUT(sc, ED_P1_PAR0 + i, sc->sc_arpcom.ac_enaddr[i]);

	/* Set multicast filter on chip. */
	ae_getmcaf(&sc->sc_arpcom, mcaf);
	for (i = 0; i < 8; i++)
		NIC_PUT(sc, ED_P1_MAR0 + i, ((u_char *)mcaf)[i]);

	/*
	 * Set current page pointer to one page after the boundary pointer, as
	 * recommended in the National manual.
	 */
	sc->next_packet = sc->rec_page_start + 1;
	NIC_PUT(sc, ED_P1_CURR, sc->next_packet);

	/* Program command register for page 0. */
	NIC_PUT(sc, ED_P1_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STP);

	i = ED_RCR_AB | ED_RCR_AM;
	if (ifp->if_flags & IFF_PROMISC) {
		/*
		 * Set promiscuous mode.  Multicast filter was set earlier so
		 * that we should receive all multicast packets.
		 */
		i |= ED_RCR_PRO | ED_RCR_AR | ED_RCR_SEP;
	}
	NIC_PUT(sc, ED_P0_RCR, i);

	/* Take interface out of loopback. */
	NIC_PUT(sc, ED_P0_TCR, 0);

	/* Fire up the interface. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STA);

	/* Set 'running' flag, and clear output active flag. */
	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;

	/* ...and attempt to start output. */
	ae_start(ifp);

	splx(s);
}

/*
 * This routine actually starts the transmission on the interface.
 */
static inline void
ae_xmit(sc)
	struct ae_softc *sc;
{
	struct ifnet *ifp = &sc->sc_arpcom.ac_if;
	u_short len;

	len = sc->txb_len[sc->txb_next_tx];

	/* Set NIC for page 0 register access. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STA);

	/* Set TX buffer start page. */
	NIC_PUT(sc, ED_P0_TPSR, sc->tx_page_start +
	    sc->txb_next_tx * ED_TXBUF_SIZE);

	/* Set TX length. */
	NIC_PUT(sc, ED_P0_TBCR0, len);
	NIC_PUT(sc, ED_P0_TBCR1, len >> 8);

	/* Set page 0, remote DMA complete, transmit packet, and *start*. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_TXP | ED_CR_STA);
	sc->xmit_busy = 1;

	/* Point to next transmit buffer slot and wrap if necessary. */
	sc->txb_next_tx++;
	if (sc->txb_next_tx == sc->txb_cnt)
		sc->txb_next_tx = 0;

	/* Set a timer just in case we never hear from the board again. */
	ifp->if_timer = 2;
}

/*
 * Start output on interface.
 * We make two assumptions here:
 *  1) that the current priority is set to splimp _before_ this code
 *     is called *and* is returned to the appropriate priority after
 *     return
 *  2) that the IFF_OACTIVE flag is checked before this code is called
 *     (i.e. that the output part of the interface is idle)
 */
void
ae_start(ifp)
	struct ifnet *ifp;
{
	struct ae_softc *sc = aecd.cd_devs[ifp->if_unit];
	struct mbuf *m0, *m;
	caddr_t buffer;
	int len;

outloop:
	/*
	 * First, see if there are buffered packets and an idle transmitter -
	 * should never happen at this point.
	 */
	if (sc->txb_inuse && (sc->xmit_busy == 0)) {
		printf("%s: packets buffered, but transmitter idle\n",
		    sc->sc_dev.dv_xname);
		ae_xmit(sc);
	}

	/* See if there is room to put another packet in the buffer. */
	if (sc->txb_inuse == sc->txb_cnt) {
		/* No room.  Indicate this to the outside world and exit. */
		ifp->if_flags |= IFF_OACTIVE;
		return;
	}

	IF_DEQUEUE(&sc->sc_arpcom.ac_if.if_snd, m);
	if (m == 0) {
		/*
		 * We are using the !OACTIVE flag to indicate to the outside
		 * world that we can accept an additional packet rather than
		 * that the transmitter is _actually_ active.  Indeed, the
		 * transmitter may be active, but if we haven't filled all the
		 * buffers with data then we still want to accept more.
		 */
		ifp->if_flags &= ~IFF_OACTIVE;
		return;
	}

	/* Copy the mbuf chain into the transmit buffer. */
	m0 = m;

	/* txb_new points to next open buffer slot. */
	buffer = sc->mem_start + ((sc->txb_new * ED_TXBUF_SIZE) << ED_PAGE_SHIFT);

	len = ae_put(sc, m, buffer);

	sc->txb_len[sc->txb_new] = max(len, ETHER_MIN_LEN);
	sc->txb_inuse++;

	/* Point to next buffer slot and wrap if necessary. */
	if (++sc->txb_new == sc->txb_cnt)
		sc->txb_new = 0;

	if (sc->xmit_busy == 0)
		ae_xmit(sc);

#if NBPFILTER > 0
	/* Tap off here if there is a BPF listener. */
	if (sc->sc_arpcom.ac_if.if_bpf)
		bpf_mtap(sc->sc_arpcom.ac_if.if_bpf, m0);
#endif

	m_freem(m0);

	/* Loop back to the top to possibly buffer more packets. */
	goto outloop;
}

/*
 * Ethernet interface receiver interrupt.
 */
static inline void
ae_rint(sc)
	struct ae_softc *sc;
{
	u_char boundary, current;
	u_short len;
	u_char nlen;
	struct ae_ring packet_hdr;
	caddr_t packet_ptr;

loop:
	/* Set NIC to page 1 registers to get 'current' pointer. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_1 | ED_CR_STA);

	/*
	 * 'sc->next_packet' is the logical beginning of the ring-buffer - i.e.
	 * it points to where new data has been buffered.  The 'CURR' (current)
	 * register points to the logical end of the ring-buffer - i.e. it
	 * points to where additional new data will be added.  We loop here
	 * until the logical beginning equals the logical end (or in other
	 * words, until the ring-buffer is empty).
	 */
	current = NIC_GET(sc, ED_P1_CURR);
	if (sc->next_packet == current)
		return;

	/* Set NIC to page 0 registers to update boundary register. */
	NIC_PUT(sc, ED_P1_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STA);

	do {
		/* Get pointer to this buffer's header structure. */
		packet_ptr = sc->mem_ring +
		    ((sc->next_packet - sc->rec_page_start) << ED_PAGE_SHIFT);

		/*
		 * The byte count includes a 4 byte header that was added by
		 * the NIC.
		 */
		packet_hdr = *(struct ae_ring *)packet_ptr;
		packet_hdr.count =
		    ((packet_hdr.count >> 8) & 0xff) |
		    ((packet_hdr.count & 0xff) << 8);
		len = packet_hdr.count;

		/*
		 * Try do deal with old, buggy chips that sometimes duplicate
		 * the low byte of the length into the high byte.  We do this
		 * by simply ignoring the high byte of the length and always
		 * recalculating it.
		 *
		 * NOTE: sc->next_packet is pointing at the current packet.
		 */
		if (packet_hdr.next_packet >= sc->next_packet)
			nlen = (packet_hdr.next_packet - sc->next_packet);
		else
			nlen = ((packet_hdr.next_packet - sc->rec_page_start) +
				(sc->rec_page_stop - sc->next_packet));
		--nlen;
		if ((len & ED_PAGE_MASK) + sizeof(packet_hdr) > ED_PAGE_SIZE)
			--nlen;
		len = (len & ED_PAGE_MASK) | (nlen << ED_PAGE_SHIFT);
#ifdef DIAGNOSTIC
		if (len != packet_hdr.count) {
			printf("%s: length does not match next packet pointer\n",
			    sc->sc_dev.dv_xname);
			printf("%s: len %04x nlen %04x start %02x first %02x curr %02x next %02x stop %02x\n",
			    sc->sc_dev.dv_xname, packet_hdr.count, len,
			    sc->rec_page_start, sc->next_packet, current,
			    packet_hdr.next_packet, sc->rec_page_stop);
		}
#endif

		/*
		 * Be fairly liberal about what we allow as a "reasonable"
		 * length so that a [crufty] packet will make it to BPF (and
		 * can thus be analyzed).  Note that all that is really
		 * important is that we have a length that will fit into one
		 * mbuf cluster or less; the upper layer protocols can then
		 * figure out the length from their own length field(s).
		 */
		if (len <= MCLBYTES &&
		    packet_hdr.next_packet >= sc->rec_page_start &&
		    packet_hdr.next_packet < sc->rec_page_stop) {
			/* Go get packet. */
			ae_get_packet(sc, packet_ptr + sizeof(struct ae_ring),
			    len - sizeof(struct ae_ring));
			++sc->sc_arpcom.ac_if.if_ipackets;
		} else {
			/* Really BAD.  The ring pointers are corrupted. */
			log(LOG_ERR,
			    "%s: NIC memory corrupt - invalid packet length %d\n",
			    sc->sc_dev.dv_xname, len);
			++sc->sc_arpcom.ac_if.if_ierrors;
			ae_reset(sc);
			return;
		}

		/* Update next packet pointer. */
		sc->next_packet = packet_hdr.next_packet;

		/*
		 * Update NIC boundary pointer - being careful to keep it one
		 * buffer behind (as recommended by NS databook).
		 */
		boundary = sc->next_packet - 1;
		if (boundary < sc->rec_page_start)
			boundary = sc->rec_page_stop - 1;
		NIC_PUT(sc, ED_P0_BNRY, boundary);
	} while (sc->next_packet != current);

	goto loop;
}

/* Ethernet interface interrupt processor. */
void
aeintr(sc)
	struct ae_softc *sc;
{
	u_char isr;

	aeintr_ctr++;

	/* Set NIC to page 0 registers. */
	NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STA);

	isr = NIC_GET(sc, ED_P0_ISR);
	if (!isr)
		return;

	/* Loop until there are no more new interrupts. */
	for (;;) {
		/*
		 * Reset all the bits that we are 'acknowledging' by writing a
		 * '1' to each bit position that was set.
		 * (Writing a '1' *clears* the bit.)
		 */
		NIC_PUT(sc, ED_P0_ISR, isr);

		/*
		 * Handle transmitter interrupts.  Handle these first because
		 * the receiver will reset the board under some conditions.
		 */
		if (isr & (ED_ISR_PTX | ED_ISR_TXE)) {
			u_char collisions = NIC_GET(sc, ED_P0_NCR) & 0x0f;

			/*
			 * Check for transmit error.  If a TX completed with an
			 * error, we end up throwing the packet away.  Really
			 * the only error that is possible is excessive
			 * collisions, and in this case it is best to allow the
			 * automatic mechanisms of TCP to backoff the flow.  Of
			 * course, with UDP we're screwed, but this is expected
			 * when a network is heavily loaded.
			 */
			(void) NIC_GET(sc, ED_P0_TSR);
			if (isr & ED_ISR_TXE) {
				/*
				 * Excessive collisions (16).
				 */
				if ((NIC_GET(sc, ED_P0_TSR) & ED_TSR_ABT)
				    && (collisions == 0)) {
					/*
					 * When collisions total 16, the P0_NCR
					 * will indicate 0, and the TSR_ABT is
					 * set.
					 */
					collisions = 16;
				}

				/* Update output errors counter. */
				++sc->sc_arpcom.ac_if.if_oerrors;
			} else {
				/*
				 * Update total number of successfully
				 * transmitted packets.
				 */
				++sc->sc_arpcom.ac_if.if_opackets;
			}

			/* Reset TX busy and output active flags. */
			sc->xmit_busy = 0;
			sc->sc_arpcom.ac_if.if_flags &= ~IFF_OACTIVE;

			/* Clear watchdog timer. */
			sc->sc_arpcom.ac_if.if_timer = 0;

			/*
			 * Add in total number of collisions on last
			 * transmission.
			 */
			sc->sc_arpcom.ac_if.if_collisions += collisions;

			/*
			 * Decrement buffer in-use count if not zero (can only
			 * be zero if a transmitter interrupt occured while not
			 * actually transmitting).
			 * If data is ready to transmit, start it transmitting,
			 * otherwise defer until after handling receiver.
			 */
			if (sc->txb_inuse && --sc->txb_inuse)
				ae_xmit(sc);
		}

		/* Handle receiver interrupts. */
		if (isr & (ED_ISR_PRX | ED_ISR_RXE | ED_ISR_OVW)) {
			/*
			 * Overwrite warning.  In order to make sure that a
			 * lockup of the local DMA hasn't occurred, we reset
			 * and re-init the NIC.  The NSC manual suggests only a
			 * partial reset/re-init is necessary - but some chips
			 * seem to want more.  The DMA lockup has been seen
			 * only with early rev chips - Methinks this bug was
			 * fixed in later revs.  -DG
			 */
			if (isr & ED_ISR_OVW) {
				++sc->sc_arpcom.ac_if.if_ierrors;
#ifdef DIAGNOSTIC
				log(LOG_WARNING,
				    "%s: warning - receiver ring buffer overrun\n",
				    sc->sc_dev.dv_xname);
#endif
				/* Stop/reset/re-init NIC. */
				ae_reset(sc);
			} else {
				/*
				 * Receiver Error.  One or more of: CRC error,
				 * frame alignment error FIFO overrun, or
				 * missed packet.
				 */
				if (isr & ED_ISR_RXE) {
					++sc->sc_arpcom.ac_if.if_ierrors;
#ifdef AE_DEBUG
					printf("%s: receive error %x\n",
					    sc->sc_dev.dv_xname,
					    NIC_GET(sc, ED_P0_RSR));
#endif
				}

				/*
				 * Go get the packet(s)
				 * XXX - Doing this on an error is dubious
				 * because there shouldn't be any data to get
				 * (we've configured the interface to not
				 * accept packets with errors).
				 */
				ae_rint(sc);
			}
		}

		/*
		 * If it looks like the transmitter can take more data, attempt
		 * to start output on the interface.  This is done after
		 * handling the receiver to give the receiver priority.
		 */
		if ((sc->sc_arpcom.ac_if.if_flags & IFF_OACTIVE) == 0)
			ae_start(&sc->sc_arpcom.ac_if);

		/*
		 * Return NIC CR to standard state: page 0, remote DMA
		 * complete, start (toggling the TXP bit off, even if was just
		 * set in the transmit routine, is *okay* - it is 'edge'
		 * triggered from low to high).
		 */
		NIC_PUT(sc, ED_P0_CR, sc->cr_proto | ED_CR_PAGE_0 | ED_CR_STA);

		/*
		 * If the Network Talley Counters overflow, read them to reset
		 * them.  It appears that old 8390's won't clear the ISR flag
		 * otherwise - resulting in an infinite loop.
		 */
		if (isr & ED_ISR_CNT) {
			(void) NIC_GET(sc, ED_P0_CNTR0);
			(void) NIC_GET(sc, ED_P0_CNTR1);
			(void) NIC_GET(sc, ED_P0_CNTR2);
		}

		isr = NIC_GET(sc, ED_P0_ISR);
		if (!isr)
			return;
	}
}

/*
 * Process an ioctl request.  This code needs some work - it looks pretty ugly.
 */
int
ae_ioctl(ifp, command, data)
	register struct ifnet *ifp;
	u_long command;
	caddr_t data;
{
	struct ae_softc *sc = aecd.cd_devs[ifp->if_unit];
	register struct ifaddr *ifa = (struct ifaddr *)data;
	struct ifreq *ifr = (struct ifreq *)data;
	int s, error = 0;

	s = splimp();

	switch (command) {

	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;

		switch (ifa->ifa_addr->sa_family) {
#ifdef INET
		case AF_INET:
			ae_init(sc);
			arp_ifinit(&sc->sc_arpcom, ifa);
			break;
#endif
#ifdef NS
		/* XXX - This code is probably wrong. */
		case AF_NS:
		    {
			register struct ns_addr *ina = &IA_SNS(ifa)->sns_addr;

			if (ns_nullhost(*ina))
				ina->x_host =
				    *(union ns_host *)(sc->sc_arpcom.ac_enaddr);
			else
				bcopy(ina->x_host.c_host,
				    sc->sc_arpcom.ac_enaddr,
				    sizeof(sc->sc_arpcom.ac_enaddr));
			/* Set new address. */
			ae_init(sc);
			break;
		    }
#endif
		default:
			ae_init(sc);
			break;
		}
		break;

	case SIOCSIFFLAGS:
		if ((ifp->if_flags & IFF_UP) == 0 &&
		    (ifp->if_flags & IFF_RUNNING) != 0) {
			/*
			 * If interface is marked down and it is running, then
			 * stop it.
			 */
			ae_stop(sc);
			ifp->if_flags &= ~IFF_RUNNING;
		} else if ((ifp->if_flags & IFF_UP) != 0 &&
			   (ifp->if_flags & IFF_RUNNING) == 0) {
			/*
			 * If interface is marked up and it is stopped, then
			 * start it.
			 */
			ae_init(sc);
		} else {
			/*
			 * Reset the interface to pick up changes in any other
			 * flags that affect hardware registers.
			 */
			ae_stop(sc);
			ae_init(sc);
		}
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		/* Update our multicast list. */
		error = (command == SIOCADDMULTI) ?
		    ether_addmulti(ifr, &sc->sc_arpcom) :
		    ether_delmulti(ifr, &sc->sc_arpcom);

		if (error == ENETRESET) {
			/*
			 * Multicast list has changed; set the hardware filter
			 * accordingly.
			 */
			ae_stop(sc); /* XXX for ds_setmcaf? */
			ae_init(sc);
			error = 0;
		}
		break;

	default:
		error = EINVAL;
	}

	splx(s);
	return (error);
}

/*
 * Retreive packet from shared memory and send to the next level up via
 * ether_input().  If there is a BPF listener, give a copy to BPF, too.
 */
void
ae_get_packet(sc, buf, len)
	struct ae_softc *sc;
	caddr_t buf;
	u_short len;
{
	struct ether_header *eh;
    	struct mbuf *m, *ae_ring_to_mbuf();

	/* Allocate a header mbuf. */
	MGETHDR(m, M_DONTWAIT, MT_DATA);
	if (m == 0)
		return;
	m->m_pkthdr.rcvif = &sc->sc_arpcom.ac_if;
	m->m_pkthdr.len = len;
	m->m_len = 0;

	/* The following silliness is to make NFS happy. */
#define EROUND	((sizeof(struct ether_header) + 3) & ~3)
#define EOFF	(EROUND - sizeof(struct ether_header))

	/*
	 * The following assumes there is room for the ether header in the
	 * header mbuf.
	 */
	m->m_data += EOFF;
	eh = mtod(m, struct ether_header *);

	word_copy(buf, mtod(m, caddr_t), sizeof(struct ether_header));
	buf += sizeof(struct ether_header);
	m->m_len += sizeof(struct ether_header);
	len -= sizeof(struct ether_header);

	/* Pull packet off interface. */
	if (ae_ring_to_mbuf(sc, buf, m, len) == 0) {
		m_freem(m);
		return;
	}

#if NBPFILTER > 0
	/*
	 * Check if there's a BPF listener on this interface.  If so, hand off
	 * the raw packet to bpf.
	 */
	if (sc->sc_arpcom.ac_if.if_bpf) {
		bpf_mtap(sc->sc_arpcom.ac_if.if_bpf, m);

		/*
		 * Note that the interface cannot be in promiscuous mode if
		 * there are no BPF listeners.  And if we are in promiscuous
		 * mode, we have to check if this packet is really ours.
		 */
		if ((sc->sc_arpcom.ac_if.if_flags & IFF_PROMISC) &&
		    (eh->ether_dhost[0] & 1) == 0 && /* !mcast and !bcast */
		    bcmp(eh->ether_dhost, sc->sc_arpcom.ac_enaddr,
			    sizeof(eh->ether_dhost)) != 0) {
			m_freem(m);
			return;
		}
	}
#endif

	/* Fix up data start offset in mbuf to point past ether header. */
	m_adj(m, sizeof(struct ether_header));
	ether_input(&sc->sc_arpcom.ac_if, eh, m);
}

/*
 * Supporting routines.
 */

/*
 * Given a source and destination address, copy 'amount' of a packet from the
 * ring buffer into a linear destination buffer.  Takes into account ring-wrap.
 */
static inline caddr_t
ae_ring_copy(sc, src, dst, amount)
	struct ae_softc *sc;
	caddr_t src, dst;
	u_short	amount;
{
	u_short	tmp_amount;

	/* Does copy wrap to lower addr in ring buffer? */
	if (src + amount > sc->mem_end) {
		tmp_amount = sc->mem_end - src;

		/* Copy amount up to end of NIC memory. */
		byte_copy(src, dst, tmp_amount);

		amount -= tmp_amount;
		src = sc->mem_ring;
		dst += tmp_amount;
	}

	byte_copy(src, dst, amount);

	return (src + amount);
}

/*
 * Copy data from receive buffer to end of mbuf chain allocate additional mbufs
 * as needed.  Return pointer to last mbuf in chain.
 * sc = ae info (softc)
 * src = pointer in ae ring buffer
 * dst = pointer to last mbuf in mbuf chain to copy to
 * amount = amount of data to copy
 */
struct mbuf *
ae_ring_to_mbuf(sc, src, dst, total_len)
	struct ae_softc *sc;
	caddr_t src;
	struct mbuf *dst;
	u_short total_len;
{
	register struct mbuf *m = dst;

	while (total_len) {
		register u_short amount = min(total_len, M_TRAILINGSPACE(m));

		if (amount == 0) {
			/*
			 * No more data in this mbuf; alloc another.
			 *
			 * If there is enough data for an mbuf cluster, attempt
			 * to allocate one of those, otherwise, a regular mbuf
			 * will do.
			 * Note that a regular mbuf is always required, even if
			 * we get a cluster - getting a cluster does not
			 * allocate any mbufs, and one is needed to assign the
			 * cluster to.  The mbuf that has a cluster extension
			 * can not be used to contain data - only the cluster
			 * can contain data.
			 */
			dst = m;
			MGET(m, M_DONTWAIT, MT_DATA);
			if (m == 0)
				return (0);

			if (total_len >= MINCLSIZE)
				MCLGET(m, M_DONTWAIT);

			m->m_len = 0;
			dst->m_next = m;
			amount = min(total_len, M_TRAILINGSPACE(m));
		}

		src = ae_ring_copy(sc, src, mtod(m, caddr_t) + m->m_len,
		    amount);

		m->m_len += amount;
		total_len -= amount;
	}
	return (m);
}

/*
 * Compute the multicast address filter from the list of multicast addresses we
 * need to listen to.
 */
void
ae_getmcaf(ac, af)
	struct arpcom *ac;
	u_long *af;
{
	struct ifnet *ifp = &ac->ac_if;
	struct ether_multi *enm;
	register u_char *cp, c;
	register u_long crc;
	register int i, len;
	struct ether_multistep step;

	/*
	 * Set up multicast address filter by passing all multicast addresses
	 * through a crc generator, and then using the high order 6 bits as an
	 * index into the 64 bit logical address filter.  The high order bit
	 * selects the word, while the rest of the bits select the bit within
	 * the word.
	 */

	if (ifp->if_flags & IFF_PROMISC) {
		ifp->if_flags |= IFF_ALLMULTI;
		af[0] = af[1] = 0xffffffff;
		return;
	}

	af[0] = af[1] = 0;
	ETHER_FIRST_MULTI(step, ac, enm);
	while (enm != NULL) {
		if (bcmp(enm->enm_addrlo, enm->enm_addrhi,
		    sizeof(enm->enm_addrlo)) != 0) {
			/*
			 * We must listen to a range of multicast addresses.
			 * For now, just accept all multicasts, rather than
			 * trying to set only those filter bits needed to match
			 * the range.  (At this time, the only use of address
			 * ranges is for IP multicast routing, for which the
			 * range is big enough to require all bits set.)
			 */
			ifp->if_flags |= IFF_ALLMULTI;
			af[0] = af[1] = 0xffffffff;
			return;
		}

		cp = enm->enm_addrlo;
		crc = 0xffffffff;
		for (len = sizeof(enm->enm_addrlo); --len >= 0;) {
			c = *cp++;
			for (i = 8; --i >= 0;) {
				if (((crc & 0x80000000) ? 1 : 0) ^ (c & 0x01)) {
					crc <<= 1;
					crc ^= 0x04c11db6 | 1;
				} else
					crc <<= 1;
				c >>= 1;
			}
		}
		/* Just want the 6 most significant bits. */
		crc >>= 26;

		/* Turn on the corresponding bit in the filter. */
		af[crc >> 5] |= 1 << ((crc & 0x1f) ^ 0);

		ETHER_NEXT_MULTI(step, enm);
	}
	ifp->if_flags &= ~IFF_ALLMULTI;
}

/*
 * Copy packet from mbuf to the board memory
 *
 * Currently uses an extra buffer/extra memory copy,
 * unless the whole packet fits in one mbuf.
 *
 */
u_short
ae_put(sc, m, buf)
	struct ae_softc *sc;
	struct mbuf *m;
	caddr_t buf;
{
	u_char *data, savebyte[2];
	int len, wantbyte;
	u_short totlen=0;

	wantbyte = 0;

	for (; m != 0; m = m->m_next) {
		data = mtod(m, u_char *);
		len = m->m_len;
		totlen += len;
		if (len > 0) {
			/* Finish the last word. */
			if (wantbyte) {
				savebyte[1] = *data;
				word_copy(savebyte, buf, 2);
				buf += 2;
				data++;
				len--;
				wantbyte = 0;
			}
			/* Output contiguous words. */
			if (len > 1) {
				word_copy(data, buf, len);
				buf += len & ~1;
				data += len & ~1;
				len &= 1;
			}
			/* Save last byte, if necessary. */
			if (len == 1) {
				savebyte[0] = *data;
				wantbyte = 1;
			}
		}
	}

	if (wantbyte) {
		savebyte[1] = 0;
		word_copy(savebyte, buf, 2);
	}

	return (totlen);
}
