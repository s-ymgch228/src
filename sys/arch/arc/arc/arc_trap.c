/*	$NetBSD: arc_trap.c,v 1.33 2006/06/24 03:50:38 tsutsui Exp $	*/
/*	$OpenBSD: trap.c,v 1.22 1999/05/24 23:08:59 jason Exp $	*/

/*
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department and Ralph Campbell.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 * from: Utah Hdr: trap.c 1.32 91/04/06
 *
 *	@(#)trap.c	8.5 (Berkeley) 1/11/94
 */
/*
 * Copyright (c) 1988 University of Utah.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department and Ralph Campbell.
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
 * from: Utah Hdr: trap.c 1.32 91/04/06
 *
 *	@(#)trap.c	8.5 (Berkeley) 1/11/94
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: arc_trap.c,v 1.33 2006/06/24 03:50:38 tsutsui Exp $");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>

#include <uvm/uvm_extern.h>

#include <mips/locore.h>

#include <machine/autoconf.h>
#include <machine/intr.h>
#include <machine/pio.h>

#include <arc/jazz/pica.h>
#include <arc/jazz/rd94.h>

struct cpu_inttab {
	uint32_t int_mask;
	uint32_t (*int_hand)(uint32_t, struct clockframe *);
};
static struct cpu_inttab cpu_int_tab[ARC_NINTPRI];

uint32_t cpu_int_mask;	/* External cpu interrupt mask */

/*
 *	Set up handler for external interrupt events.
 *	Events are checked in priority order.
 */
void
arc_set_intr(uint32_t mask, uint32_t (*int_hand)(uint32_t, struct clockframe *),
    int prio)
{

	if (prio >= ARC_NINTPRI)
		panic("arc_set_intr: too high priority");

	if (cpu_int_tab[prio].int_mask != 0 &&
	    (cpu_int_tab[prio].int_mask != mask ||
	     cpu_int_tab[prio].int_hand != int_hand)) {
		panic("set_intr: int already set");
	}

	cpu_int_tab[prio].int_hand = int_hand;
	cpu_int_tab[prio].int_mask = mask;
	cpu_int_mask |= mask >> 10;
}

/*
 * Handle an interrupt.
 * N.B., curlwp might be NULL.
 */
void
cpu_intr(uint32_t status, uint32_t cause, uint32_t pc, uint32_t ipending)
{
	struct clockframe cf;
	struct cpu_inttab *inttab;
	u_int i;

	uvmexp.intrs++;

	cf.pc = pc;
	cf.sr = status;

	/* check MIPS3 internal clock interrupt */
	if (ipending & MIPS_INT_MASK_5) {
		/*
		 *  Writing a value to the Compare register,
		 *  as a side effect, clears the timer interrupt request.
		 */
		mips3_cp0_compare_write(0);
		cause &= ~MIPS_INT_MASK_5;
	}
	_splset((status & MIPS_INT_MASK_5) | MIPS_SR_INT_IE);

	/*
	 *  If there is an independent timer interrupt handler, call it first.
	 *  Called interrupt routine returns mask of interrupts to be reenabled.
	 */
	inttab = &cpu_int_tab[ARC_INTPRI_TIMER_INT];
	if (inttab->int_mask & ipending) {
		if ((ipending & MIPS_INT_MASK & ~inttab->int_mask) == 0) {
			/*
			 * If all interrupts were enabled and there is no
			 * pending interrupts, set MIPS_SR_INT_IE so that
			 * spllowerclock() in hardclock() works properly.
			 */
#if 0			/* MIPS_SR_INT_IE is enabled above */
			_splset(MIPS_SR_INT_IE);
#endif
		} else {
			/*
			 * If there are any pending interrputs, clear
			 * MIPS_SR_INT_IE in cf.sr so that spllowerclock()
			 * in hardclock() will not happen.
			 */
			cf.sr &= ~MIPS_SR_INT_IE;
		}
		cause &= (*inttab->int_hand)(ipending, &cf);
	}
	_splset((status & ~cause & MIPS_HARD_INT_MASK) | MIPS_SR_INT_IE);

	inttab++;

	/*
	 *  Check off all other enabled interrupts.
	 *  Called handlers return mask of interrupts to be reenabled.
	 */
	for (i = ARC_INTPRI_TIMER_INT + 1; i < ARC_NINTPRI; i++) {
		if (inttab->int_mask & ipending) {
			cause &= (*inttab->int_hand)(ipending, &cf);
		}
		inttab++;
	}
	_splset((status & ~cause & MIPS_HARD_INT_MASK) | MIPS_SR_INT_IE);

	/* software interrupts */
	ipending &= (MIPS_SOFT_INT_MASK_1|MIPS_SOFT_INT_MASK_0);
	if (ipending == 0)
		return;

	_clrsoftintr(ipending);

	softintr_dispatch(ipending);
}
