/*	$NetBSD: vm_machdep.c,v 1.32 2002/04/28 17:10:39 uch Exp $	*/

/*-
 * Copyright (c) 2002 The NetBSD Foundation, Inc. All rights reserved.
 * Copyright (c) 1995 Charles M. Hannum.  All rights reserved.
 * Copyright (c) 1982, 1986 The Regents of the University of California.
 * Copyright (c) 1989, 1990 William Jolitz
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * the Systems Programming Group of the University of Utah Computer
 * Science Department, and William Jolitz.
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
 *	@(#)vm_machdep.c	7.3 (Berkeley) 5/13/91
 */

/*
 *	Utah $Hdr: vm_machdep.c 1.16.1.1 89/06/23$
 */

#include "opt_kstack_debug.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/malloc.h>
#include <sys/vnode.h>
#include <sys/buf.h>
#include <sys/user.h>
#include <sys/core.h>
#include <sys/exec.h>
#include <sys/ptrace.h>

#include <uvm/uvm_extern.h>

#include <sh3/cpu.h>
#include <sh3/reg.h>
#include <sh3/mmu.h>
#include <sh3/locore.h>

/* XXX XXX XXX */
#ifdef SH4
#define	TLBFLUSH()		(cacheflush(), sh_tlb_invalidate_all())
#else
#define	TLBFLUSH()		sh_tlb_invalidate_all()
#endif
/* XXX XXX XXX */

/*
 * Finish a fork operation, with process p2 nearly set up.
 * Copy and update the pcb and trap frame, making the child ready to run.
 *
 * Rig the child's kernel stack so that it will start out in
 * proc_trampoline() and call child_return() with p2 as an
 * argument. This causes the newly-created child process to go
 * directly to user level with an apparent return value of 0 from
 * fork(), while the parent process returns normally.
 *
 * p1 is the process being forked; if p1 == &proc0, we are creating
 * a kernel thread, and the return path and argument are specified with
 * `func' and `arg'.
 *
 * If an alternate user-level stack is requested (with non-zero values
 * in both the stack and stacksize args), set up the user stack pointer
 * accordingly.
 */
void
cpu_fork(struct proc *p1, struct proc *p2, void *stack,
    size_t stacksize, void (*func)(void *), void *arg)
{
	extern void proc_trampoline(void);
	struct pcb *pcb;
	struct trapframe *tf;
	struct switchframe *sf;
	vaddr_t spbase;

	/* XXX vtophys don't return physical addresss */
#define	P1ADDR(x)							\
	((vtophys((vaddr_t)(x)) & SH3_PHYS_MASK) | SH3_P1SEG_BASE)
#ifdef DIAGNOSTIC
	if (p1 != curproc && p1 != &proc0)
		panic("cpu_fork: curproc");
#endif
	/*
	 * wbinv u-area to avoid cache-aliasing, since trapframe
	 * top is accessed from P1 instead of P3.
	 */
	if (CPU_IS_SH4)
		sh_dcache_wbinv_range((vaddr_t)p2->p_addr, USPACE);

	p2->p_md.md_p3 = p2->p_addr;

	/* Copy flags */
	p2->p_md.md_flags = p1->p_md.md_flags;

	/* Sync the switchframe before we copy it. */
	if (p1 == curproc)
		savectx(curpcb);

	/* Copy PCB */
	pcb = &p2->p_addr->u_pcb;
	*pcb = p1->p_addr->u_pcb;
	/* Set page directory base to pcb */
	pmap_activate(p2);

	/* set up the kernel stack pointer */
	spbase = (vaddr_t)p2->p_md.md_p3 + NBPG;
#ifdef P1_STACK
	/* Convert to P1 from P3 */
	spbase = P1ADDR(spbase);
#else /* P1_STACK */
	/* Prepare kernel stack PTEs */
	if (CPU_IS_SH3)
		sh3_switch_setup(p2);
	else
		sh4_switch_setup(p2);
#endif /* P1_STACK */

	pcb->pcb_sp = spbase + USPACE - NBPG;
	/*
	 * Convert frame pointer top to P1. because SH3 can't make
	 * wired TLB entry, context store space accessing must not cause
	 * exception. SH4 can make wired entry, no need to convert to P1.
	 */
	pcb->pcb_fp = (vaddr_t)P1ADDR(pcb) + NBPG;		/* P1 */

#ifdef KSTACK_DEBUG
	memset((char *)pcb->pcb_fp - NBPG + sizeof(struct user), 0x5a,
	    NBPG - sizeof(struct user));
	memset((char *)spbase, 0xa5, (USPACE - NBPG));
#endif
	/*
	 * Copy the trapframe.
	 */
	p2->p_md.md_regs = tf = (struct trapframe *)pcb->pcb_fp - 1;
	*tf = *p1->p_md.md_regs;

	/*
	 * If specified, give the child a different stack.
	 */
	if (stack != NULL)
		tf->tf_r15 = (u_int)stack + stacksize;

	/* Setup switch frame */
	sf = &pcb->pcb_sf;
	sf->sf_r11 = (int)arg;		/* proc_trampoline hook func */
	sf->sf_r12 = (int)func;		/* proc_trampoline hook func's arg */
	sf->sf_r6_bank = (int)tf;	/* frame pointer */
	sf->sf_r15 = pcb->pcb_sp;	/* stack pointer */
	/* when switch to me, jump to proc_trampoline */
	sf->sf_pr  = (int)proc_trampoline;
	sf->sf_sr &= ~0xf0;		/* SR.IMASK = 0 */
}

/*
 * void cpu_exit(sturct proc *p):
 *	+ Change kernel context to proc0's one.
 *	+ Schedule freeing process 'p' resources.
 *	+ switch to another process.
 */
void
cpu_exit(struct proc *p)
{

	splsched();
	uvmexp.swtch++;

	/* Switch to proc0 stack */
	curproc = 0;
	curpcb = (struct pcb *)proc0.p_addr;
	__asm__ __volatile__(
		"mov	%0, r15;"	/* current stack */
		"ldc	%1, r6_bank;"	/* current frame pointer */
		"ldc	%2, r7_bank;"	/* stack top */
		::
		"r"(curpcb->pcb_sf.sf_r15),
		"r"(curpcb->pcb_sf.sf_r6_bank),
		"r"(curpcb->pcb_sp));

	/* Schedule freeing process resources */
	exit2(p);

	cpu_switch(p);
	/* NOTREACHED */
}

/*
 * Dump the machine specific segment at the start of a core dump.
 */
struct md_core {
	struct reg intreg;
};

int
cpu_coredump(struct proc *p, struct vnode *vp, struct ucred *cred,
    struct core *chdr)
{
	struct md_core md_core;
	struct coreseg cseg;
	int error;

	CORE_SETMAGIC(*chdr, COREMAGIC, MID_MACHINE, 0);
	chdr->c_hdrsize = ALIGN(sizeof(*chdr));
	chdr->c_seghdrsize = ALIGN(sizeof(cseg));
	chdr->c_cpusize = sizeof(md_core);

	/* Save integer registers. */
	error = process_read_regs(p, &md_core.intreg);
	if (error)
		return error;


	CORE_SETMAGIC(cseg, CORESEGMAGIC, MID_MACHINE, CORE_CPU);
	cseg.c_addr = 0;
	cseg.c_size = chdr->c_cpusize;

	error = vn_rdwr(UIO_WRITE, vp, (caddr_t)&cseg, chdr->c_seghdrsize,
	    (off_t)chdr->c_hdrsize, UIO_SYSSPACE, IO_NODELOCKED|IO_UNIT, cred,
	    (int *)0, p);
	if (error)
		return error;

	error = vn_rdwr(UIO_WRITE, vp, (caddr_t)&md_core, sizeof(md_core),
	    (off_t)(chdr->c_hdrsize + chdr->c_seghdrsize), UIO_SYSSPACE,
	    IO_NODELOCKED|IO_UNIT, cred, (int *)0, p);
	if (error)
		return error;

	chdr->c_nseg++;
	return 0;
}

/*
 * Move pages from one kernel virtual address to another.
 * Both addresses are assumed to reside in the Sysmap.
 */
void
pagemove(caddr_t from, caddr_t to, size_t size)
{
	pt_entry_t *fpte, *tpte;

	if (size % NBPG)
		panic("pagemove");
	fpte = kvtopte(from);
	tpte = kvtopte(to);
	while (size > 0) {
		*tpte++ = *fpte;
		*fpte++ = 0;
		from += NBPG;
		to += NBPG;
		size -= NBPG;
	}
	TLBFLUSH();
}

/*
 * Map an IO request into kernel virtual address space.  Requests fall into
 * one of five catagories:
 *
 *	B_PHYS|B_UAREA:	User u-area swap.
 *			Address is relative to start of u-area (p_addr).
 *	B_PHYS|B_PAGET:	User page table swap.
 *			Address is a kernel VA in usrpt (Usrptmap).
 *	B_PHYS|B_DIRTY:	Dirty page push.
 *			Address is a VA in proc2's address space.
 *	B_PHYS|B_PGIN:	Kernel pagein of user pages.
 *			Address is VA in user's address space.
 *	B_PHYS:		User "raw" IO request.
 *			Address is VA in user's address space.
 *
 * All requests are (re)mapped into kernel VA space via the phys_map
 * (a name with only slightly more meaning than "kernel_map")
 */

void
vmapbuf(struct buf *bp, vsize_t len)
{
	vaddr_t faddr, taddr, off;
	paddr_t fpa;

	if ((bp->b_flags & B_PHYS) == 0)
		panic("vmapbuf");
	faddr = trunc_page((vaddr_t)bp->b_saveaddr = bp->b_data);
	off = (vaddr_t)bp->b_data - faddr;
	len = round_page(off + len);
	taddr= uvm_km_valloc_wait(phys_map, len);
	bp->b_data = (caddr_t)(taddr + off);
	/*
	 * The region is locked, so we expect that pmap_pte() will return
	 * non-NULL.
	 * XXX: unwise to expect this in a multithreaded environment.
	 * anything can happen to a pmap between the time we lock a
	 * region, release the pmap lock, and then relock it for
	 * the pmap_extract().
	 *
	 * no need to flush TLB since we expect nothing to be mapped
	 * where we we just allocated (TLB will be flushed when our
	 * mapping is removed).
	 */
	while (len) {
		pmap_extract(vm_map_pmap(&bp->b_proc->p_vmspace->vm_map),
			     faddr, &fpa);
		pmap_enter(vm_map_pmap(phys_map), taddr, fpa,
		    VM_PROT_READ | VM_PROT_WRITE, PMAP_WIRED);
		faddr += PAGE_SIZE;
		taddr += PAGE_SIZE;
		len -= PAGE_SIZE;
	}
	pmap_update(vm_map_pmap(phys_map));
}

/*
 * Free the io map PTEs associated with this IO operation.
 * We also invalidate the TLB entries and restore the original b_addr.
 */
void
vunmapbuf(struct buf *bp, vsize_t len)
{
	vaddr_t addr, off;

	if ((bp->b_flags & B_PHYS) == 0)
		panic("vunmapbuf");
	addr = trunc_page((vaddr_t)bp->b_data);
	off = (vaddr_t)bp->b_data - addr;
	len = round_page(off + len);
	pmap_remove(vm_map_pmap(phys_map), addr, addr + len);
	pmap_update(vm_map_pmap(phys_map));
	uvm_km_free_wakeup(phys_map, addr, len);
	bp->b_data = bp->b_saveaddr;
	bp->b_saveaddr = 0;
}
