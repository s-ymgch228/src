/* $NetBSD: uvm_stat.c,v 1.7 1998/02/13 05:33:55 thorpej Exp $	 */

/*
 * XXXCDC: "ROUGH DRAFT" QUALITY UVM PRE-RELEASE FILE!
 *	   >>>USE AT YOUR OWN RISK, WORK IS NOT FINISHED<<<
 */
/*
 *
 * Copyright (c) 1997 Charles D. Cranor and Washington University.
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
 *      This product includes software developed by Charles D. Cranor and
 *      Washington University.
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
 * from: Id: uvm_stat.c,v 1.1.2.3 1997/12/19 15:01:00 mrg Exp
 */

#include "opt_uvmhist.h"

/*
 * uvm_stat.c
 */

#include <sys/param.h>
#include <sys/systm.h>

#include <vm/vm.h>

#include <uvm/uvm.h>

/*
 * globals
 */

struct uvm_cnt *uvm_cnt_head = NULL;

#ifdef UVMHIST
struct uvm_history_head uvm_histories;
#endif

#ifdef UVMHIST_PRINT
int uvmhist_print_enabled = 1;
#endif

/*
 * prototypes
 */

#ifdef UVMHIST
void uvmhist_dump __P((struct uvm_history *));
#endif
void uvmcnt_dump __P((void));
void uvmmap_dump __P((vm_map_t));
void uvm_dump   __P((void));

#ifdef UVMHIST
/* call this from ddb */
void
uvmhist_dump(l)
	struct uvm_history *l;
{
	int lcv, s;

	s = splhigh();
	lcv = l->f;
	do {
		if (l->e[lcv].fmt)
			uvmhist_print(&l->e[lcv]);
		lcv = (lcv + 1) % l->n;
	} while (lcv != l->f);
	splx(s);
}
#endif /* UVMHIST */

void
uvmcnt_dump()
{
	struct uvm_cnt *uvc = uvm_cnt_head;

	while (uvc) {
		if ((uvc->t & UVMCNT_MASK) != UVMCNT_CNT)
			continue;
		printf("%s = %d\n", uvc->name, uvc->c);
		uvc = uvc->next;
	}
}

/*
 * uvm_dump: ddb hook to dump interesting uvm counters
 */
void 
uvm_dump()
{

	printf("Current UVM status:\n");
	printf("  pagesize=%d (0x%x), pagemask=0x%x, pageshift=%d\n",
	    uvmexp.pagesize, uvmexp.pagesize, uvmexp.pagemask,
	    uvmexp.pageshift);
	printf("  %d VM pages: %d active, %d inactive, %d wired, %d free\n",
	    uvmexp.npages, uvmexp.active, uvmexp.inactive, uvmexp.wired,
	    uvmexp.free);
	printf("  freemin=%d, free-target=%d, inactive-target=%d, "
	    "wired-max=%d\n", uvmexp.freemin, uvmexp.freetarg, uvmexp.inactarg,
	    uvmexp.wiredmax);
	printf("  faults=%d, traps=%d, intrs=%d, ctxswitch=%d\n",
	    uvmexp.faults, uvmexp.traps, uvmexp.intrs, uvmexp.swtch);
	printf("  softint=%d, syscalls=%d, swapins=%d, swapouts=%d\n",
	    uvmexp.softs, uvmexp.syscalls, uvmexp.swapins, uvmexp.swapouts);

	printf("  fault counts:\n");
	printf("    noram=%d, noanon=%d, pgwait=%d, pgrele=%d\n",
	    uvmexp.fltnoram, uvmexp.fltnoanon, uvmexp.fltpgwait,
	    uvmexp.fltpgrele);
	printf("    ok relocks(total)=%d(%d), anget(retrys)=%d(%d), "
	    "amapcopy=%d\n", uvmexp.fltrelckok, uvmexp.fltrelck,
	    uvmexp.fltanget, uvmexp.fltanretry, uvmexp.fltamcopy);
	printf("    neighbor anon/obj pg=%d/%d, gets(lock/unlock)=%d/%d\n",
	    uvmexp.fltnamap, uvmexp.fltnomap, uvmexp.fltlget, uvmexp.fltget);
	printf("    cases: anon=%d, anoncow=%d, obj=%d, prcopy=%d, przero=%d\n",
	    uvmexp.flt_anon, uvmexp.flt_acow, uvmexp.flt_obj, uvmexp.flt_prcopy,
	    uvmexp.flt_przero);

	printf("  daemon and swap counts:\n");
	printf("    woke=%d, revs=%d, scans=%d, swout=%d\n", uvmexp.pdwoke,
	    uvmexp.pdrevs, uvmexp.pdscans, uvmexp.pdswout);
	printf("    busy=%d, freed=%d, reactivate=%d, deactivate=%d\n",
	    uvmexp.pdbusy, uvmexp.pdfreed, uvmexp.pdreact, uvmexp.pddeact);
	printf("    pageouts=%d, pending=%d, nswget=%d\n", uvmexp.pdpageouts,
	    uvmexp.pdpending, uvmexp.nswget);
	printf("    nswapdev=%d, nanon=%d, nfreeanon=%d\n", uvmexp.nswapdev,
	    uvmexp.nanon, uvmexp.nfreeanon);
}
