/*	$NetBSD: vm_machdep.c,v 1.128 2009/11/29 04:11:51 rmind Exp $	*/

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
 * from: Utah Hdr: vm_machdep.c 1.21 91/04/06
 *
 *	@(#)vm_machdep.c	8.3 (Berkeley) 1/4/94
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
 * from: Utah Hdr: vm_machdep.c 1.21 91/04/06
 *
 *	@(#)vm_machdep.c	8.3 (Berkeley) 1/4/94
 */

#include <sys/cdefs.h>			/* RCS ID & Copyright macro defns */
__KERNEL_RCSID(0, "$NetBSD: vm_machdep.c,v 1.128 2009/11/29 04:11:51 rmind Exp $");

#include "opt_ddb.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/buf.h>
#include <sys/vnode.h>
#include <sys/core.h>
#include <sys/exec.h>
#include <sys/sa.h>
#include <sys/savar.h>

#include <uvm/uvm_extern.h>

#include <mips/cache.h>
#include <mips/regnum.h>
#include <mips/locore.h>
#include <mips/pte.h>
#include <mips/psl.h>
#include <machine/cpu.h>

paddr_t kvtophys(vaddr_t);	/* XXX */

/*
 * cpu_lwp_fork: finish a new LWP (l2) operation.
 *
 * First LWP (l1) is the process being forked.  If it is &lwp0, then we
 * are creating a kthread, where return path and argument are specified
 * with `func' and `arg'.
 *
 * If an alternate user-level stack is requested (with non-zero values
 * in both the stack and stacksize arguments), then set up the user stack
 * pointer accordingly.
 */
void
cpu_lwp_fork(struct lwp *l1, struct lwp *l2, void *stack, size_t stacksize,
    void (*func)(void *), void *arg)
{
	struct pcb *pcb1, *pcb2;
	struct frame *f;
	vaddr_t uv;

	KASSERT(l1 == curlwp || l1 == &lwp0);

	pcb1 = lwp_getpcb(l1);
	pcb2 = lwp_getpcb(l2);

	l2->l_md.md_ss_addr = 0;
	l2->l_md.md_ss_instr = 0;
	l2->l_md.md_astpending = 0;

	/* If parent LWP was using FPU, then save the FPU h/w state. */
	if ((l1->l_md.md_flags & MDP_FPUSED) && l1 == fpcurlwp)
		savefpregs(l1);

	/* Copy the PCB from parent. */
	memcpy(pcb2, pcb1, sizeof(struct pcb));

	/*
	 * Copy the trapframe from parent, so that return to userspace
	 * will be to right address, with correct registers.
	 */
	uv = uvm_lwp_getuarea(l2);
	f = (struct frame *)(uv + USPACE) - 1;
	memcpy(f, l1->l_md.md_regs, sizeof(struct frame));

	/* If specified, set a different user stack for a child. */
	if (stack != NULL)
		f->f_regs[_R_SP] = (uintptr_t)stack + stacksize;

	l2->l_md.md_regs = (void *)f;
	l2->l_md.md_flags = l1->l_md.md_flags & MDP_FPUSED;
#if USPACE > PAGE_SIZE
	{
		const int x = (MIPS_HAS_R4K_MMU) ?
		    (MIPS3_PG_G | MIPS3_PG_RO | MIPS3_PG_WIRED) : MIPS1_PG_G;
		pt_entry_t *pte = kvtopte(uv);
		size_t i;

		for (i = 0; i < UPAGES; i++)
			l2->l_md.md_upte[i] = pte[i].pt_entry &~ x;
	}
#endif
	/*
	 * Rig kernel stack so that it would start out in lwp_trampoline()
	 * and call child_return() with l2 as an argument.  This causes the
	 * newly-created child process to go directly to user level with a
	 * parent return value of 0 from fork(), while the parent process
	 * returns normally.
	 */
	pcb2->pcb_context[0] = (intptr_t)func;			/* S0 */
	pcb2->pcb_context[1] = (intptr_t)arg;			/* S1 */
	pcb2->pcb_context[MIPS_CURLWP_CARD - 16] = (intptr_t)l2;/* S? */
	pcb2->pcb_context[8] = (intptr_t)f;			/* SP */
	pcb2->pcb_context[10] = (intptr_t)lwp_trampoline;	/* RA */
#ifdef IPL_ICU_MASK
	/* Machine depenedend interrupt mask. */
	pcb2->pcb_ppl = 0;
#endif
}

void
cpu_setfunc(struct lwp *l, void (*func)(void *), void *arg)
{
	struct pcb *pcb = lwp_getpcb(l);
	struct frame *f = l->l_md.md_regs;

	pcb->pcb_context[0] = (intptr_t)func;			/* S0 */
	pcb->pcb_context[1] = (intptr_t)arg;			/* S1 */
	pcb->pcb_context[MIPS_CURLWP_CARD - 16] = (intptr_t)l;	/* S? */
	pcb->pcb_context[8] = (intptr_t)f;			/* SP */
	pcb->pcb_context[10] = (intptr_t)setfunc_trampoline;	/* RA */
#ifdef IPL_ICU_MASK
	/* Machine depenedend interrupt mask. */
	pcb->pcb_ppl = 0;
#endif
}

void
cpu_lwp_free(struct lwp *l, int proc)
{

	if ((l->l_md.md_flags & MDP_FPUSED) && l == fpcurlwp)
		fpcurlwp = NULL;
}

void
cpu_lwp_free2(struct lwp *l)
{

	(void)l;
}

/*
 * Map a user I/O request into kernel virtual address space.
 */
void
vmapbuf(struct buf *bp, vsize_t len)
{
	struct pmap *upmap;
	vaddr_t uva;	/* User VA (map from) */
	vaddr_t kva;	/* Kernel VA (new to) */
	paddr_t pa;	/* physical address */
	vsize_t off;

	if ((bp->b_flags & B_PHYS) == 0)
		panic("vmapbuf");

	uva = mips_trunc_page(bp->b_saveaddr = bp->b_data);
	off = (vaddr_t)bp->b_data - uva;
	len = mips_round_page(off + len);
	kva = uvm_km_alloc(phys_map, len, 0, UVM_KMF_VAONLY | UVM_KMF_WAITVA);
	bp->b_data = (void *)(kva + off);
	upmap = vm_map_pmap(&bp->b_proc->p_vmspace->vm_map);
	do {
		if (pmap_extract(upmap, uva, &pa) == false)
			panic("vmapbuf: null page frame");
		pmap_enter(vm_map_pmap(phys_map), kva, pa,
		    VM_PROT_READ | VM_PROT_WRITE, PMAP_WIRED);
		uva += PAGE_SIZE;
		kva += PAGE_SIZE;
		len -= PAGE_SIZE;
	} while (len);
	pmap_update(vm_map_pmap(phys_map));
}

/*
 * Unmap a previously-mapped user I/O request.
 */
void
vunmapbuf(struct buf *bp, vsize_t len)
{
	vaddr_t kva;
	vsize_t off;

	if ((bp->b_flags & B_PHYS) == 0)
		panic("vunmapbuf");

	kva = mips_trunc_page(bp->b_data);
	off = (vaddr_t)bp->b_data - kva;
	len = mips_round_page(off + len);
	pmap_remove(vm_map_pmap(phys_map), kva, kva + len);
	pmap_update(pmap_kernel());
	uvm_km_free(phys_map, kva, len, UVM_KMF_VAONLY);
	bp->b_data = bp->b_saveaddr;
	bp->b_saveaddr = NULL;
}

/*
 * Map a (kernel) virtual address to a physical address.
 *
 * MIPS processor has 3 distinct kernel address ranges:
 *
 * - kseg0 kernel "virtual address" for the   cached physical address space.
 * - kseg1 kernel "virtual address" for the uncached physical address space.
 * - kseg2 normal kernel "virtual address" mapped via the TLB.
 */
paddr_t
kvtophys(vaddr_t kva)
{
	pt_entry_t *pte;
	paddr_t phys;

	if (kva >= MIPS_KSEG2_START) {
		if (kva >= VM_MAX_KERNEL_ADDRESS)
			goto overrun;

		pte = kvtopte(kva);
		if ((size_t) (pte - Sysmap) > Sysmapsize)  {
			printf("oops: Sysmap overrun, max %d index %zd\n",
			       Sysmapsize, pte - Sysmap);
		}
		if (!mips_pg_v(pte->pt_entry)) {
			printf("kvtophys: pte not valid for %lx\n", kva);
		}
		phys = mips_tlbpfn_to_paddr(pte->pt_entry) | (kva & PGOFSET);
		return phys;
	}
	if (kva >= MIPS_KSEG1_START)
		return MIPS_KSEG1_TO_PHYS(kva);

	if (kva >= MIPS_KSEG0_START)
		return MIPS_KSEG0_TO_PHYS(kva);

overrun:
	printf("Virtual address %lx: cannot map to physical\n", kva);
#ifdef DDB
	Debugger();
	return 0;	/* XXX */
#endif
	panic("kvtophys");
}
