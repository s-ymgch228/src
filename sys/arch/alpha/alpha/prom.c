/* $NetBSD: prom.c,v 1.21 1998/02/13 02:09:09 cgd Exp $ */

/* 
 * Copyright (c) 1992, 1994, 1995, 1996 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */

__KERNEL_RCSID(0, "$NetBSD: prom.c,v 1.21 1998/02/13 02:09:09 cgd Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <vm/vm.h>
#include <sys/proc.h>
#include <sys/user.h>

#include <machine/rpb.h>
#include <machine/prom.h>
#ifdef NEW_PMAP
#include <vm/pmap.h>
#endif

#include <dev/cons.h>

/* XXX this is to fake out the console routines, while booting. */
struct consdev promcons = { NULL, NULL, promcngetc, promcnputc,
			    nullcnpollc, makedev(23,0), 1 };

struct rpb	*hwrpb;
int		alpha_console;

extern struct prom_vec prom_dispatch_v;

#ifdef _PMAP_MAY_USE_PROM_CONSOLE
int		prom_mapped = 1;	/* Is PROM still mapped? */
pt_entry_t	*rom_ptep, rom_pte, saved_pte;	/* XXX */
#endif

#ifdef NEW_PMAP
#define	rom_ptep   (curproc ? &curproc->p_vmspace->vm_map.pmap->dir[0] : rom_ptep)
#endif

void
init_prom_interface(rpb)
	struct rpb *rpb;
{
	struct crb *c;

	c = (struct crb *)((char *)rpb + rpb->rpb_crb_off);

        prom_dispatch_v.routine_arg = c->crb_v_dispatch;
        prom_dispatch_v.routine = c->crb_v_dispatch->entry_va;
}

void
init_bootstrap_console()
{
	char buf[4];

	init_prom_interface(hwrpb);

	prom_getenv(PROM_E_TTY_DEV, buf, 4);
	alpha_console = buf[0] - '0';

	/* XXX fake out the console routines, for now */
	cn_tab = &promcons;
}

static int  enter_prom __P((void));
static void leave_prom __P((int));
#ifdef _PMAP_MAY_USE_PROM_CONSOLE
static void prom_cache_sync __P((void));
#endif

static int
enter_prom __P((void))
{
	int s = splhigh();

#ifdef _PMAP_MAY_USE_PROM_CONSOLE
	if (!prom_mapped) {
		if (!pmap_uses_prom_console())
			panic("enter_prom");
		saved_pte = *rom_ptep;		/* XXX */
		*rom_ptep = rom_pte;		/* XXX */
		prom_cache_sync();		/* XXX */
	}
#endif
	return s;
}

static void
leave_prom __P((s))
	int s;
{

#ifdef _PMAP_MAY_USE_PROM_CONSOLE
	if (!prom_mapped) {
		if (!pmap_uses_prom_console())
			panic("leave_prom");
		*rom_ptep = saved_pte;		/* XXX */
		prom_cache_sync();		/* XXX */
	}
#endif
	splx(s);
}

#ifdef _PMAP_MAY_USE_PROM_CONSOLE
static void
prom_cache_sync __P((void))
{
	ALPHA_TBIA();
	alpha_pal_imb();
}
#endif

/*
 * promcnputc:
 *
 * Remap char before passing off to prom.
 *
 * Prom only takes 32 bit addresses. Copy char somewhere prom can
 * find it. This routine will stop working after pmap_rid_of_console 
 * is called in alpha_init. This is due to the hard coded address
 * of the console area.
 */
void
promcnputc(dev, c)
	dev_t dev;
	int c;
{
        prom_return_t ret;
	unsigned char *to = (unsigned char *)0x20000000;
	int s;

	s = enter_prom();	/* splhigh() and map prom */
	*to = c;

	do {
		ret.bits = prom_putstr(alpha_console, to, 1);
	} while ((ret.u.retval & 1) == 0);

	leave_prom(s);		/* unmap prom and splx(s) */
}

/*
 * promcngetc:
 *
 * Wait for the prom to get a real char and pass it back.
 */
int
promcngetc(dev)
	dev_t dev;
{
        prom_return_t ret;
	int s;

        for (;;) {
		s = enter_prom();
                ret.bits = prom_getc(alpha_console);
		leave_prom(s);
                if (ret.u.status == 0 || ret.u.status == 1)
                        return (ret.u.retval);
        }
}

/*
 * promcnlookc:
 *
 * See if prom has a real char and pass it back.
 */
int
promcnlookc(dev, cp)
	dev_t dev;
	char *cp;
{
        prom_return_t ret;
	int s;

	s = enter_prom();
	ret.bits = prom_getc(alpha_console);
	leave_prom(s);
	if (ret.u.status == 0 || ret.u.status == 1) {
		*cp = ret.u.retval;
		return 1;
	} else
		return 0;
}

int
prom_getenv(id, buf, len)
	int id, len;
	char *buf;
{
	unsigned char *to = (unsigned char *)0x20000000;
	prom_return_t ret;
	int s;

	s = enter_prom();
	ret.bits = prom_getenv_disp(id, to, len);
	bcopy(to, buf, len);
	leave_prom(s);

	if (ret.u.status & 0x4)
		ret.u.retval = 0;
	buf[ret.u.retval] = '\0';

	return (ret.bits);
}

void
prom_halt(halt)
	int halt;
{
	struct pcs *p;

	/*
	 * Turn off interrupts, for sanity.
	 */
	(void) splhigh();

	/*
	 * Set "boot request" part of the CPU state depending on what
	 * we want to happen when we halt.
	 */
	p = (struct pcs *)((char *)hwrpb + hwrpb->rpb_pcs_off +
	    (hwrpb->rpb_primary_cpu_id * hwrpb->rpb_pcs_size));
	p->pcs_flags &= ~(PCS_RC | PCS_HALT_REQ);
	if (halt)
		p->pcs_flags |= PCS_HALT_STAY_HALTED;
	else
		p->pcs_flags |= PCS_HALT_WARM_BOOT;

	/*
	 * Halt the machine.
	 */
	alpha_pal_halt();
}

u_int64_t
hwrpb_checksum()
{
	u_int64_t *p, sum;
	int i;

	for (i = 0, p = (u_int64_t *)hwrpb, sum = 0;
	    i < (offsetof(struct rpb, rpb_checksum) / sizeof (u_int64_t));
	    i++, p++)
		sum += *p;

	return (sum);
}

void
hwrpb_restart_setup()
{
	struct pcs *p;

	/* Clear bootstrap-in-progress flag since we're done bootstrapping */
	p = (struct pcs *)((char *)hwrpb + hwrpb->rpb_pcs_off);
	p->pcs_flags &= ~PCS_BIP;

	bcopy(&proc0.p_addr->u_pcb.pcb_hw, p->pcs_hwpcb,
	    sizeof proc0.p_addr->u_pcb.pcb_hw);
	hwrpb->rpb_vptb = VPTBASE;

	/* when 'c'ontinuing from console halt, do a dump */
	hwrpb->rpb_rest_term = (u_int64_t)&XentRestart;
	hwrpb->rpb_rest_term_val = 0x1;

#if 0
	/* don't know what this is really used by, so don't mess with it. */
	hwrpb->rpb_restart = (u_int64_t)&XentRestart;
	hwrpb->rpb_restart_val = 0x2;
#endif

	hwrpb->rpb_checksum = hwrpb_checksum();

	p->pcs_flags |= (PCS_RC | PCS_CV);
}

u_int64_t
console_restart(ra, ai, pv)
	u_int64_t ra, ai, pv;
{
	struct pcs *p;

	/* Clear restart-capable flag, since we can no longer restart. */
	p = (struct pcs *)((char *)hwrpb + hwrpb->rpb_pcs_off);
	p->pcs_flags &= ~PCS_RC;

	panic("user requested console halt");

	return (1);
}
