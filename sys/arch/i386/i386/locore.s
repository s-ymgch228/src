#undef DIAGNOSTIC
#define DIAGNOSTIC
/*-
 * Copyright (c) 1993, 1994 Charles Hannum.
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
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
 *	from: @(#)locore.s	7.3 (Berkeley) 5/13/91
 *	$Id: locore.s,v 1.62 1994/04/07 06:48:35 mycroft Exp $
 */

/*
 * locore.s:	4BSD machine support for the Intel 386
 *		Preliminary version
 *		Written by William F. Jolitz, 386BSD Project
 */

#include "npx.h"
#include "assym.s"

#include <sys/errno.h>
#include <sys/syscall.h>

#include <machine/cputypes.h>
#include <machine/param.h>
#include <machine/pte.h>
#include <machine/specialreg.h>
#include <machine/trap.h>

#include <i386/isa/debug.h>

#define	KCSEL		0x08
#define	KDSEL		0x10
#define	SEL_RPL_MASK	0x0003

/* XXX temporary kluge; these should not be here */
#define	IOM_BEGIN	0x0a0000	/* start of I/O memory "hole" */
#define	IOM_END		0x100000	/* end of I/O memory "hole" */
#define	IOM_SIZE	(IOM_END - IOM_BEGIN)


#define	ALIGN_DATA	.align	2
#define	ALIGN_TEXT	.align	2,0x90	/* 4-byte boundaries, NOP-filled */
#define	SUPERALIGN_TEXT	.align	4,0x90	/* 16-byte boundaries better for 486 */

/* NB: NOP now preserves registers so NOPs can be inserted anywhere */
/* XXX: NOP and FASTER_NOP are misleadingly named */
#ifdef DUMMY_NOPS	/* this will break some older machines */
#define	FASTER_NOP
#define	NOP
#else
#define	FASTER_NOP	pushl %eax ; inb $0x84,%al ; popl %eax
#define	NOP	pushl %eax ; inb $0x84,%al ; inb $0x84,%al ; popl %eax
#endif


/*
 * PTmap is recursive pagemap at top of virtual address space.
 * Within PTmap, the page directory can be found (third indirection).
 */
	.globl	_PTmap,_PTD,_PTDpde,_Sysmap
	.set	_PTmap,(PTDPTDI << PDSHIFT)
	.set	_PTD,(_PTmap + PTDPTDI * NBPG)
	.set	_PTDpde,(_PTD + PTDPTDI * 4)		# XXX 4 == sizeof pde
	.set	_Sysmap,(_PTmap + KPTDI * NBPG)

/*
 * APTmap, APTD is the alternate recursive pagemap.
 * It's used when modifying another process's page tables.
 */
	.globl	_APTmap,_APTD,_APTDpde
	.set	_APTmap,(APTDPTDI << PDSHIFT)
	.set	_APTD,(_APTmap + APTDPTDI * NBPG)
	.set	_APTDpde,(_PTD + APTDPTDI * 4)		# XXX 4 == sizeof pde

/*
 * Access to each processes kernel stack is via a region of per-process address
 * space (at the beginning), immediatly above the user process stack.
 */
	.set	_kstack,USRSTACK
	.globl	_kstack

#define	ENTRY(name)	.globl _/**/name; ALIGN_TEXT; _/**/name:
#define	ALTENTRY(name)	.globl _/**/name; _/**/name:

/*
 * Initialization
 */
	.data

	.globl	_cpu,_cpu_vendor,_cold,_esym,_boothowto,_bootdev,_atdevbase
	.globl	_cyloffset,_proc0paddr,_curpcb,_IdlePTD,_KPTphys
_cpu:		.long	0	# are we 386, 386sx, or 486
_cpu_vendor:	.space	16	# vendor string returned by `cpuid' instruction
_cold:		.long	1	# cold till we are not
_esym:		.long	0	# ptr to end of syms
_atdevbase:	.long	0	# location of start of iomem in virtual
_atdevphys:	.long	0	# location of device mapping ptes (phys)
_cyloffset:	.long	0
_proc0paddr:	.long	0
_IdlePTD:	.long	0
_KPTphys:	.long	0

	.space 512
tmpstk:

	.text
	.globl	start
start:	movw	$0x1234,0x472			# warm boot
	/* Skip over BIOS variables. */
	jmp	1f
	.org	0x500
1:

	/*
	 * Load parameters from stack (howto, bootdev, unit, cyloffset, esym).
	 * note: (%esp) is return address of boot
	 * (If we want to hold onto /boot, it's physical %esp up to _end.)
	 */
	movl	4(%esp),%eax
	movl	%eax,_boothowto-KERNBASE
	movl	8(%esp),%eax
	movl	%eax,_bootdev-KERNBASE
	movl	12(%esp),%eax
	movl	%eax,_cyloffset-KERNBASE
 	movl	16(%esp),%eax
 	addl	$(KERNBASE),%eax
 	movl	%eax,_esym-KERNBASE

	/* Find out our CPU type. */

	/* First, clear the alignment check and identification flags. */
	pushfl
	popl	%eax
	andl	$~(PSL_AC|PSL_ID),%eax
	pushl	%eax
	popfl

	/* Try to toggle alignment check flag; does not exist on 386. */
	pushfl
	popl	%eax
	movl	%eax,%ecx
	orl	$PSL_AC,%eax
	pushl	%eax
	popfl
	pushfl
	popl	%eax
	xorl	%ecx,%eax
	andl	$PSL_AC,%eax
	pushl	%ecx
	popfl

	testl	%eax,%eax
	jnz	1f
	movl	$CPU_386,_cpu-KERNBASE
	jmp	2f

1:	/* Try to toggle identification flag; does not exist on early 486s. */
	pushfl
	popl	%eax
	movl	%eax,%ecx
	xorl	$PSL_ID,%eax
	pushl	%eax
	popfl
	pushfl
	popl	%eax
	xorl	%ecx,%eax
	andl	$PSL_ID,%eax
	pushl	%ecx
	popfl

	testl	%eax,%eax
	jnz	1f
	movl	$CPU_486,_cpu-KERNBASE
	jmp	2f

1:	/* Use the `cpuid' instruction. */
	xorl	%eax,%eax
	.byte	0x0f,0xa2		# cpuid 0
	movl	%ebx,_cpu_vendor	# store vendor string
	movl	%edx,_cpu_vendor+4
	movl	%ecx,_cpu_vendor+8
	movb	$0,_cpu_vendor+12

	movl	$1,%eax
	.byte	0x0f,0xa2		# cpuid 1
	rorl	$8,%eax			# extract family type
	andl	$15,%eax
	cmpl	$5,%eax
	jae	1f

	/* less than Pentium; must be 486 */
	movl	$CPU_486,_cpu-KERNBASE
	jmp	2f

1:	movl	$CPU_586,_cpu-KERNBASE
2:

	/*
	 * Finished with old stack; load new %esp now instead of later so we
	 * can trace this code without having to worry about the trace trap
	 * clobbering the memory test or the zeroing of the bss+bootstrap page
	 * tables.
	 *
	 * The boot program should check:
	 *	text+data <= &stack_variable - more_space_for_stack
	 *	text+data+bss+pad+space_for_page_tables <= end_of_memory
	 * Oops, the gdt is in the carcass of the boot program so clearing
	 * the rest of memory is still not possible.
	 */
	movl	$(tmpstk-KERNBASE),%esp	# bootstrap stack end location

/*
 * Virtual address space of kernel:
 *
 * text | data | bss | [syms] | page dir | usr stk map | proc0 kstack | Sysmap
 *			      0          1             2       3      4
 */
	/* Clear the BSS. */
	movl	$(_edata-KERNBASE),%edi
	movl	$(((_end-_edata)+3)>>2),%ecx
	xorl	%eax,%eax
	cld
	rep
	stosl

	/* Find end of kernel image. */
	movl	$(_end-KERNBASE),%ecx
#if defined(DDB) && !defined(SYMTAB_SPACE)
	/* Save the symbols (if loaded). */
	movl	_esym-KERNBASE,%eax
	testl	%eax,%eax
	jz	1f
	movl	%eax,%ecx
	subl	$KERNBASE,%ecx
1:
#endif
	movl	%ecx,%edi			# edi = esym ?: end
	addl	$PGOFSET,%ecx			# page align up
	andl	$(~PGOFSET),%ecx
	movl	%ecx,%esi			# esi = start of tables
	subl	%edi,%ecx
	addl	$((NKPDE+UPAGES+2)*NBPG),%ecx	# size of tables
	
	/* Clear memory for bootstrap tables. */
	shrl	$2,%ecx
	xorl	%eax,%eax
	cld
	rep
	stosl

	movl	%esi,_IdlePTD-KERNBASE		# physaddr of proc0 stack

/*
 * fillkpt
 *	eax = pte (page frame | control | status)
 *	ebx = page table address
 *	ecx = number of pages to map
 */
#define	fillkpt		\
1:	movl	%eax,(%ebx)	; \
	addl	$NBPG,%eax	; /* increment physical address */ \
	addl	$4,%ebx		; /* next pte */ \
	loop	1b		;

/*
 * Build initial page tables.
 */
	/* First, map the kernel text, data, and BSS. */
	movl	%esi,%ecx		# this much memory,
	shrl	$PGSHIFT,%ecx		# for this many pte s
	addl	$(NKPDE+UPAGES+2),%ecx	# including our early context
	movl	$(PG_V|PG_KW),%eax	#  having these bits set,
	leal	((UPAGES+2)*NBPG)(%esi),%ebx	#   physical address of KPT in proc 0,
	movl	%ebx,_KPTphys-KERNBASE	#    in the kernel page table,
	fillkpt

	/* Map ISA I/O memory. */
	movl	$(IOM_SIZE>>PGSHIFT),%ecx	# for this many pte s,
	movl	$(IOM_BEGIN|PG_V|PG_UW/*|PG_N*/),%eax	# having these bits set
	movl	%ebx,_atdevphys-KERNBASE	# remember phys addr of ptes
	fillkpt

	/* Map proc 0's kernel stack into user page table page. */
	movl	$UPAGES,%ecx		# for this many pte s,
	leal	(2*NBPG)(%esi),%eax	# physical address in proc 0
	leal	(KERNBASE)(%eax),%edx
	movl	%edx,_proc0paddr-KERNBASE	# remember VA for 0th process init
	orl	$(PG_V|PG_KW),%eax	#  having these bits set,
	leal	(1*NBPG)(%esi),%ebx	# physical address of stack pt in proc 0
	addl	$((NPTEPD-UPAGES)*4),%ebx
	fillkpt

/*
 * Construct a page table directory.
 */
	/* Install a PDE for temporary double map of kernel text. */
	movl	_KPTphys-KERNBASE,%eax	# physical address of kernel page tables
	orl     $(PG_V|PG_UW),%eax	# pde entry is valid
	movl	%eax,(%esi)		# which is where temp maps!

	/* Map kernel PDEs. */
	movl	$NKPDE,%ecx		# for this many pde s,
	leal	(KPTDI*4)(%esi),%ebx	# offset of pde for kernel
	fillkpt

	/* Install a PDE recursively mapping page directory as a page table! */
	movl	%esi,%eax		# phys address of ptd in proc 0
	orl	$(PG_V|PG_UW),%eax	# pde entry is valid
	movl	%eax,(PTDPTDI*4)(%esi)	# which is where PTmap maps!

	/* Install a PDE to map kernel stack for proc 0. */
	leal	(1*NBPG)(%esi),%eax	# physical address of pt in proc 0
	orl	$(PG_V|PG_KW),%eax	# pde entry is valid
	movl	%eax,(UPTDI*4)(%esi)	# which is where kernel stack maps!

#ifdef BDB
	/* Copy and convert stuff from old GDT and IDT for debugger. */
	cmpl	$0x0375c339,0x96104	# XXX - debugger signature
	jne	1f
	movb	$1,_bdb_exists-KERNBASE

	pushal
	subl	$2*6,%esp

	sgdt	(%esp)
	movl	2(%esp),%esi		# base address of current gdt
	movl	$(_gdt-KERNBASE),%edi
	movl	%edi,2(%esp)
	movl	$8*18/4,%ecx
	rep				# copy gdt
	movsl
	movl	$(_gdt-KERNBASE),-8+2(%edi)	# adjust gdt self-ptr
	movb	$0x92,-8+5(%edi)

	sidt	6(%esp)
	movl	6+2(%esp),%esi		# base address of current idt
	movl	8+4(%esi),%eax		# convert dbg descriptor to ...
	movw	8(%esi),%ax
	movl	%eax,bdb_dbg_ljmp+1-KERNBASE	# ... immediate offset ...
	movl	8+2(%esi),%eax
	movw	%ax,bdb_dbg_ljmp+5-KERNBASE	# ... and selector for ljmp
	movl	24+4(%esi),%eax		# same for bpt descriptor
	movw	24(%esi),%ax
	movl	%eax,bdb_bpt_ljmp+1-KERNBASE
	movl	24+2(%esi),%eax
	movw	%ax,bdb_bpt_ljmp+5-KERNBASE

	movl	$(_idt-KERNBASE),%edi
	movl	%edi,6+2(%esp)
	movl	$8*4/4,%ecx
	rep				# copy idt
	movsl

	lgdt	(%esp)
	lidt	6(%esp)

	addl	$2*6,%esp
	popal

1:
#endif

	/* Load base of page directory and enable mapping. */
	movl	%esi,%eax		# phys address of ptd in proc 0
	movl	%eax,%cr3		# load ptd addr into mmu
	movl	%cr0,%eax		# get control word
	orl	$(CR0_PE|CR0_PG),%eax	# enable paging
	movl	%eax,%cr0		# and let's page NOW!

	pushl	$begin			# jump to high mem
	ret

begin:	/* Now running relocated at KERNBASE. */

	/* Relocate atdevbase. */
	.globl	_Crtat
	movl	_Crtat,%eax
	subl	$(KERNBASE|IOM_BEGIN),%eax
	movl	_atdevphys,%edx		# get pte PA
	subl	_KPTphys,%edx		# remove base of ptes; have phys offset
	shll	$(PGSHIFT-2),%edx	# corresponding to virt offset
	addl	$(KERNBASE),%edx	# add virtual base
	movl	%edx,_atdevbase
	addl	%eax,%edx
	movl	%edx,_Crtat

	/* Set up bootstrap stack. */
	movl	$(_kstack+UPAGES*NBPG-4*12),%esp # bootstrap stack end location
	xorl	%eax,%eax		# mark end of frames
	movl	%eax,%ebp
	movl	_proc0paddr,%eax
	movl	%esi,PCB_CR3(%eax)

#ifdef BDB
	cmpl	$0,_bdb_exists
	jz	1f

	/* relocate debugger gdt entries */
	movl	$(_gdt+8*9),%eax	# adjust slots 9-17
	movl	$9,%ecx
reloc_gdt:
	movb	$(KERNBASE>>24),7(%eax)	# top byte of base addresses, was 0,
	addl	$8,%eax			# now KERNBASE>>24
	loop	reloc_gdt

	int	$3
1:
#endif

	/*
	 * Some BIOSes leave trash in the spare segment registers.  We need to
	 * clear them so we don't get a protection fault in swtch() later on.
	 * Since the kernel itself does not use these except in copyin/out, it
	 * seems best to make them null selectors so we get a trap if they are
	 * accidentally referenced.
	 */
	xorl	%ecx,%ecx
	movl	%cx,%fs
	movl	%cx,%gs

	leal	((NKPDE+UPAGES+2)*NBPG)(%esi),%esi	# skip past stack and page tables
	pushl	%esi
	call	_init386		# wire 386 chip for unix operation
	
	movl	$0,_PTD
	call 	_main
	popl	%esi

#if defined(I486_CPU) || defined(I586_CPU)
	/*
	 * Now we've run main() and determined what CPU type we are, so we can
	 * enable WP mode on i486 CPUs and above.
	 */
	cmpl	$CPUCLASS_386,_cpu_class
	je	1f			# 386s can't handle WP mode
	movl	%cr0,%eax		# get control word
	orl	$CR0_WP,%eax		# enable ring 0 Write Protection
	movl	%eax,%cr0
1:
#endif

	/*
	 * Prepare for initial return to user space.
	 */
	.globl	__ucodesel,__udatasel
	movl	__ucodesel,%eax
	movl	__udatasel,%ecx
	# build outer stack frame
	pushl	%ecx		# user ss
	pushl	$USRSTACK	# user esp
	pushl	%eax		# user cs
	pushl	$0		# user ip
	movl	%cx,%ds
	movl	%cx,%es
	movl	%ax,%fs		# double map cs to fs
	movl	%cx,%gs		# and ds to gs
	lret			# goto user!
	/* NOTREACHED */

/*****************************************************************************/

#define	LCALL(x,y)	.byte 0x9a ; .long y ; .word x

/*
 * This is the initial process used to start init(8).  It's copied out to
 * offset 0 in process 1's address space.
 */
ENTRY(icode)
	pushl	$0			# envp for execve()

#	pushl	$argv-_icode		# can't do this 'cos gas 1.38 is broken
	movl	$argv,%eax
	subl	$_icode,%eax
	pushl	%eax			# argp for execve()

#	pushl	$init-_icode
	movl	$init,%eax
	subl	$_icode,%eax
	pushl	%eax			# fname for execve()
	pushl	%eax			# dummy return address
	movl	$SYS_execve,%eax
	LCALL(7,0)
	/* Exit if something botches up in the above exec. */
	movl	$SYS_exit,%eax
	LCALL(7,0)

init:	.asciz	"/sbin/init"
	ALIGN_DATA
argv:	.long	init+6-_icode		# argv[0] = "init" ("/sbin/init" + 6)
	.long	eicode-_icode		# argv[1] follows icode after copyout
	.long	0
eicode:

	.globl	_szicode
_szicode:
	.long	eicode-_icode

/*
 * Signal trampoline; copied to top of user stack.
 */
ENTRY(sigcode)
	call	SIGF_HANDLER(%esp)
	leal	SIGF_SC(%esp),%eax	# scp (the call may have clobbered the
					# copy at SIGF_SCP(%esp))
	pushl	%eax
	pushl	%eax			# junk to fake return address
	movl	$SYS_sigreturn,%eax
	LCALL(7,0)			# enter kernel with args on stack
	movl	$SYS_exit,%eax
	LCALL(7,0)			# exit if sigreturn fails
	.globl	_esigcode
_esigcode:

/*****************************************************************************/

/*
 * The following primitives are used to fill and copy regions of memory.
 */

/*
 * fillw(short pattern, caddr_t addr, size_t len);
 * Write len copies of pattern at addr.
 */
ENTRY(fillw)
	pushl	%edi
	movl	8(%esp),%eax
	movl	12(%esp),%edi
	movw	%ax,%cx
	rorl	$16,%eax
	movw	%cx,%ax
	cld
	movl	16(%esp),%ecx
	shrl	%ecx			# do longwords
	rep
	stosl
	movl	16(%esp),%ecx
	andl	$1,%ecx			# do remainder
	rep
	stosw
	popl	%edi
	ret

/*
 * bcopyb(caddr_t from, caddr_t to, size_t len);
 * Copy len bytes, one byte at a time.
 */
ENTRY(bcopyb)
	pushl	%esi
	pushl	%edi
	movl	12(%esp),%esi
	movl	16(%esp),%edi
	movl	20(%esp),%ecx
	cmpl	%esi,%edi		# potentially overlapping?
	jnb	1f
	cld				# no; copy forward
	rep
	movsb
	popl	%edi
	popl	%esi
	ret

	ALIGN_TEXT
1:	addl	%ecx,%edi		# copy backward
	addl	%ecx,%esi
	std
	decl	%edi
	decl	%esi
	rep
	movsb
	popl	%edi
	popl	%esi
	cld
	ret

/*
 * bcopyw(caddr_t from, caddr_t to, size_t len);
 * Copy len bytes, two bytes at a time.
 */
ENTRY(bcopyw)
	pushl	%esi
	pushl	%edi
	movl	12(%esp),%esi
	movl	16(%esp),%edi
	movl	20(%esp),%ecx
	cmpl	%esi,%edi		# potentially overlapping?
	jnb	1f
	cld				# no; copy forward
	shrl	$1,%ecx			# copy by 16-bit words
	rep
	movsw
	adc	%ecx,%ecx		# any bytes left?
	rep
	movsb
	popl	%edi
	popl	%esi
	ret

	ALIGN_TEXT
1:	addl	%ecx,%edi		# copy backward
	addl	%ecx,%esi
	std
	andl	$1,%ecx			# any fractional bytes?
	decl	%edi
	decl	%esi
	rep
	movsb
	movl	20(%esp),%ecx		# copy remainder by 16-bit words
	shrl	$1,%ecx
	decl	%esi
	decl	%edi
	rep
	movsw
	popl	%edi
	popl	%esi
	cld
	ret

/*
 * bcopy(caddr_t from, caddr_t to, size_t len);
 * Copy len bytes.
 */
ENTRY(bcopy)
ALTENTRY(ovbcopy)
	pushl	%esi
	pushl	%edi
	movl	12(%esp),%esi
	movl	16(%esp),%edi
	movl	20(%esp),%ecx
	cmpl	%esi,%edi		# potentially overlapping? */
	jnb	1f
	cld				# nope, copy forward
	shrl	$2,%ecx			# copy by 32-bit words
	rep
	movsl
	movl	20(%esp),%ecx
	andl	$3,%ecx			# any bytes left?
	rep
	movsb
	popl	%edi
	popl	%esi
	ret

	ALIGN_TEXT
1:	addl	%ecx,%edi		# copy backward
	addl	%ecx,%esi
	std
	andl	$3,%ecx			# any fractional bytes?
	decl	%edi
	decl	%esi
	rep
	movsb
	movl	20(%esp),%ecx		# copy remainder by 32-bit words
	shrl	$2,%ecx
	subl	$3,%esi
	subl	$3,%edi
	rep
	movsl
	popl	%edi
	popl	%esi
	cld
	ret

/*****************************************************************************/

/*
 * The following primitives are used to copy data in and out of the user's
 * address space.
 */

/*
 * copyout(caddr_t from, caddr_t to, size_t len);
 * Copy len bytes into the user's address space.
 */
ENTRY(copyout)
	movl	_curpcb,%eax
	movl	$_copyout_fault,PCB_ONFAULT(%eax)
	pushl	%esi
	pushl	%edi
	pushl	%ebx
	movl	16(%esp),%esi
	movl	20(%esp),%edi
	movl	24(%esp),%ebx
	testl	%ebx,%ebx		# anything to do?
	jz	done_copyout

	/*
	 * We check that the end of the destination buffer is not past the end
	 * of the user's address space.  If it's not, then we only need to
	 * check that each page is writable.  The 486 will do this for us; the
	 * 386 will not.  (We assume that pages in user space that are not
	 * writable by the user are not writable by the kernel either.)
	 */
	movl	%edi,%eax
	addl	%ebx,%eax
	jc	_copyout_fault
	cmpl	$VM_MAXUSER_ADDRESS,%eax
	ja	_copyout_fault

#if defined(I386_CPU)
#if defined(I486_CPU) || defined(I586_CPU)
	cmpl	$CPUCLASS_386,_cpu_class
	jne	3f
#endif /* I486_CPU || I586_CPU */

	/*
	 * We have to check each PTE for (write) permission, since the CPU
	 * doesn't do it for us.
	 */

	/* Compute number of pages. */
	movl	%edi,%ecx
	andl	$PGOFSET,%ecx
	addl	%ebx,%ecx
	decl	%ecx
	shrl	$PGSHIFT,%ecx
	incl	%ecx

	/* Compute PTE offset for start address. */
	movl	%edi,%edx
	shrl	$PGSHIFT,%edx

1:	/* Check PTE for each page. */
	movb	_PTmap(,%edx,4),%al
	andb	$0x07,%al		# must be valid/user/write
	cmpb	$0x07,%al
	je	2f
				
	/* Simulate a trap. */
	pushl	%edx
	pushl	%ecx
	shll	$PGSHIFT,%edx
	pushl	%edx
	call	_trapwrite		# trapwrite(addr)
	addl	$4,%esp			# pop argument
	popl	%ecx
	popl	%edx

	testl	%eax,%eax		# if not ok, return EFAULT
	jnz	_copyout_fault

2:	incl	%edx
	decl	%ecx
	jnz	1b			# check next page
#endif /* I386_CPU */

3:	/* bcopy(%esi, %edi, %ebx); */
	cld
	movl	%ebx,%ecx
	shrl	$2,%ecx
	rep
	movsl
	movb	%bl,%cl
	andb	$3,%cl
	rep
	movsb

done_copyout:
	popl	%ebx
	popl	%edi
	popl	%esi
	xorl	%eax,%eax
	movl	_curpcb,%edx
	movl	%eax,PCB_ONFAULT(%edx)
	ret

ENTRY(copyout_fault)
	popl	%ebx
	popl	%edi
	popl	%esi
	movl	_curpcb,%edx
	movl	$0,PCB_ONFAULT(%edx)
	movl	$EFAULT,%eax
	ret

/*
 * copyin(caddr_t from, caddr_t to, size_t len);
 * Copy len bytes from the user's address space.
 */
ENTRY(copyin)
	movl	_curpcb,%eax
	movl	$_copyin_fault,PCB_ONFAULT(%eax)
	pushl	%esi
	pushl	%edi
	movl	12(%esp),%esi
	movl	16(%esp),%edi
	movl	20(%esp),%ecx

	movb	%cl,%dl
	shrl	$2,%ecx			# copy longwords
	cld
	gs
	rep
	movsl
	movb	%dl,%cl
	andb	$3,%cl			# copy remainder
	gs
	rep
	movsb

	popl	%edi
	popl	%esi
	xorl	%eax,%eax
	movl	_curpcb,%edx
	movl	%eax,PCB_ONFAULT(%edx)
	ret

ENTRY(copyin_fault)
	popl	%edi
	popl	%esi
	movl	_curpcb,%edx
	movl	$0,PCB_ONFAULT(%edx)
	movl	$EFAULT,%eax
	ret

/*
 * copyoutstr(caddr_t from, caddr_t to, size_t maxlen, size_t int *lencopied);
 * Copy a NUL-terminated string, at most maxlen characters long, into the
 * user's address space.  Return the number of characters copied (including the
 * NUL) in *lencopied.  If the string is too long, return ENAMETOOLONG; else
 * return 0 or EFAULT.
 */
ENTRY(copyoutstr)
	pushl	%esi
	pushl	%edi
	movl	_curpcb,%ecx
	movl	$_copystr_fault,PCB_ONFAULT(%ecx)

	movl	12(%esp),%esi		# esi = from
	movl	16(%esp),%edi		# edi = to
	movl	20(%esp),%edx		# edx = maxlen

#if defined(I386_CPU)
#if defined(I486_CPU) || defined(I586_CPU)
	cmpl	$CPUCLASS_386,_cpu_class
	jne	5f
#endif /* I486_CPU || I586_CPU */

1:	/*
	 * Once per page, check that we are still within the bounds of user
	 * space.
	 */
	cmpl	$VM_MAXUSER_ADDRESS,%edi
	jae	_copyout_fault

	movl	%edi,%eax
	shrl	$PGSHIFT,%eax
	movb	_PTmap(,%eax,4),%al
	andb	$7,%al
	cmpb	$7,%al
	je	2f

	/* Simulate a trap. */
	pushl	%edx
	pushl	%edi
	call	_trapwrite		# trapwrite(addr)
	addl	$4,%esp			# clear argument from stack
	popl	%edx
	testl	%eax,%eax
	jnz	_copystr_fault

2:	/* Copy up to end of this page. */
	movl	%edi,%eax
	andl	$PGOFSET,%eax
	movl	$NBPG,%ecx
	subl	%eax,%ecx		# ecx = NBPG - (src % NBPG)
	cmpl	%ecx,%edx
	jae	3f
	movl	%edx,%ecx		# ecx = min (ecx, edx)

3:	subl	%ecx,%edx		# predecrement total count
	incl	%ecx

3:	decl	%ecx
	jz	4f
	lodsb
	stosb
	testb	%al,%al
	jnz	3b

	/* Success -- 0 byte reached. */
	decl	%ecx
	addl	%ecx,%edx		# add back residual for this page
	xorl	%eax,%eax
	jmp	copystr_return

4:	/* Go to next page, if any. */
	testl	%edx,%edx
	jnz	1b

	/* edx is zero -- return ENAMETOOLONG. */
	movl	$ENAMETOOLONG,%eax
	jmp	copystr_return
#endif /* I386_CPU */

#if defined(I486_CPU) || defined(I586_CPU)
5:	/*
	 * Get min(%edx, VM_MAXUSER_ADDRESS-%edi).
	 */
	movl	$VM_MAXUSER_ADDRESS,%eax
	subl	%edi,%eax
	cmpl	%edx,%eax
	setae	%cl
	jae	1f
	movl	%eax,%edx
	movl	%eax,20(%esp)

1:	incl	%edx

1:	decl	%edx
	jz	2f
	lodsb
	stosb
	testb	%al,%al
	jnz	1b

	/* Success -- 0 byte reached. */
	decl	%edx
	xorl	%eax,%eax
	jmp	copystr_return

2:	/* edx is zero. */
	testb	%cl,%cl
	jnz	1f
	/* edx is zero -- hit end of user space. */
	movl	$EFAULT,%eax
	jmp	copystr_return
1:	/* edx is zero -- return ENAMETOOLONG. */
	movl	$ENAMETOOLONG,%eax
	jmp	copystr_return
#endif /* I486_CPU || I586_CPU */

/*
 * copyinstr(caddr_t from, caddr_t to, size_t maxlen, size_t int *lencopied);
 * Copy a NUL-terminated string, at most maxlen characters long, from the
 * user's address space.  Return the number of characters copied (including the
 * NUL) in *lencopied.  If the string is too long, return ENAMETOOLONG; else
 * return 0 or EFAULT.
 */
ENTRY(copyinstr)
	pushl	%esi
	pushl	%edi
	movl	_curpcb,%ecx
	movl	$_copystr_fault,PCB_ONFAULT(%ecx)

	movl	12(%esp),%esi			# %esi = from
	movl	16(%esp),%edi			# %edi = to
	movl	20(%esp),%edx			# %edx = maxlen

	/*
	 * Get min(%edx, VM_MAXUSER_ADDRESS-%esi).
	 */
	movl	$VM_MAXUSER_ADDRESS,%eax
	subl	%esi,%eax
	cmpl	%edx,%eax
	setae	%cl
	jae	1f
	movl	%eax,%edx
	movl	%eax,20(%esp)

1:	incl	%edx

1:	decl	%edx
	jz	4f
	lodsb
	stosb
	testb	%al,%al
	jnz	1b

	/* Success -- 0 byte reached. */
	decl	%edx
	xorl	%eax,%eax
	jmp	copystr_return

4:	/* edx is zero. */
	testb	%cl,%cl
	jnz	1f
	/* edx is zero -- hit end of user space. */
	movl	$EFAULT,%eax
	jmp	copystr_return
1:	/* edx is zero -- return ENAMETOOLONG. */
	movl	$ENAMETOOLONG,%eax
	jmp	copystr_return

ENTRY(copystr_fault)
	movl	$EFAULT,%eax

copystr_return:	
	/* Set *lencopied and return %eax. */
	movl	_curpcb,%ecx
	movl	$0,PCB_ONFAULT(%ecx)
	movl	20(%esp),%ecx
	subl	%edx,%ecx
	movl	24(%esp),%edx
	testl	%edx,%edx
	jz	8f
	movl	%ecx,(%edx)

8:	popl	%edi
	popl	%esi
	ret

/*
 * copystr(caddr_t from, caddr_t to, size_t maxlen, size_t int *lencopied);
 * Copy a NUL-terminated string, at most maxlen characters long.  Return the
 * number of characters copied (including the NUL) in *lencopied.  If the
 * string is too long, return ENAMETOOLONG; else return 0.
 */
ENTRY(copystr)
	pushl	%esi
	pushl	%edi

	movl	12(%esp),%esi		# esi = from
	movl	16(%esp),%edi		# edi = to
	movl	20(%esp),%edx		# edx = maxlen
	incl	%edx

1:	decl	%edx
	jz	4f
	lodsb
	stosb
	testb	%al,%al
	jnz	1b

	/* Success -- 0 byte reached. */
	decl	%edx
	xorl	%eax,%eax
	jmp	6f

4:	/* edx is zero -- return ENAMETOOLONG. */
	movl	$ENAMETOOLONG,%eax

6:	/* Set *lencopied and return %eax. */
	movl	20(%esp),%ecx
	subl	%edx,%ecx
	movl	24(%esp),%edx
	testl	%edx,%edx
	jz	7f
	movl	%ecx,(%edx)

7:	popl	%edi
	popl	%esi
	ret

/*
 * fuword(caddr_t uaddr);
 * Fetch an int from the user's address space.
 */
ENTRY(fuword)
ALTENTRY(fuiword)
	movl	_curpcb,%ecx
	movl	$_fusufault,PCB_ONFAULT(%ecx)
	movl	4(%esp),%edx
	gs
	movl	(%edx),%eax
	movl	$0,PCB_ONFAULT(%ecx)
	ret
	
/*
 * fusword(caddr_t uaddr);
 * Fetch a short from the user's address space.
 */
ENTRY(fusword)
	movl	_curpcb,%ecx
	movl	$_fusufault,PCB_ONFAULT(%ecx)
fusword1:
	movl	4(%esp),%edx
	gs
	movzwl	(%edx),%eax
	movl	$0,PCB_ONFAULT(%ecx)
	ret
	
/*
 * fuswintr(caddr_t uaddr);
 * Fetch a short from the user's address space.  Can be called during an
 * interrupt.
 */
ENTRY(fuswintr)
	movl	_curpcb,%ecx
	/*
	 * Use a different fault handler than fusword() to signal trap() not to
	 * try to page fault.
	 */
	movl	$_fusubail,PCB_ONFAULT(%ecx)
	jmp	fusword1
	
/*
 * fubyte(caddr_t uaddr);
 * Fetch a byte from the user's address space.
 */
ENTRY(fubyte)
ALTENTRY(fuibyte)
	movl	_curpcb,%ecx
	movl	$_fusufault,PCB_ONFAULT(%ecx)
	movl	4(%esp),%edx
	gs
	movzbl	(%edx),%eax
	movl	$0,PCB_ONFAULT(%ecx)
	ret

/*
 * Handle faults from [fs]u*().  Clean up and return -1.
 */
ENTRY(fusufault)
	movl	_curpcb,%ecx
	xorl	%eax,%eax
	movl	%eax,PCB_ONFAULT(%ecx)
	decl	%eax
	ret

/*
 * Handle faults from [fs]u*().  Clean up and return -1.  This differs from
 * fusufault() in that trap() will recognize it and return immediately rather
 * than trying to page fault.
 */
ENTRY(fusubail)
	movl	_curpcb,%ecx
	xorl	%eax,%eax
	movl	%eax,PCB_ONFAULT(%ecx)
	decl	%eax
	ret

/*
 * suword(caddr_t uaddr, int x);
 * Store an int in the user's address space.
 */
ENTRY(suword)
ALTENTRY(suiword)
	movl	_curpcb,%ecx
	movl	$_fusufault,PCB_ONFAULT(%ecx)
	movl	4(%esp),%edx

#if defined(I386_CPU)
#if defined(I486_CPU) || defined(I586_CPU)
	cmpl	$CPUCLASS_386,_cpu_class
	jne	2f
#endif /* I486_CPU || I586_CPU */

	movl	%edx,%eax
	shrl	$PGSHIFT,%edx		# fetch pte associated with address
	movb	_PTmap(,%edx,4),%dl
	andb	$7,%dl
	cmpb	$7,%dl			# must be valid/user/write
	je	1f

	/* Simulate a trap. */
	pushl	%eax
	call	_trapwrite		# trapwrite(addr)
	addl	$4,%esp			# clear parameter from the stack
	movl	_curpcb,%ecx
	testl	%eax,%eax
	jnz	_fusufault

1:	/* XXX also need to check the following 3 bytes for validity! */
	movl	4(%esp),%edx
#endif

2:	movl	8(%esp),%eax
	gs
	movl	%eax,(%edx)
	xorl	%eax,%eax
	movl	%eax,PCB_ONFAULT(%ecx)
	ret
	
/*
 * susword(caddr_t uaddr, short x);
 * Store a short in the user's address space.
 */
ENTRY(susword)
	movl	_curpcb,%ecx
	movl	$_fusufault,PCB_ONFAULT(%ecx)
susword1:
	movl	4(%esp),%edx

#if defined(I386_CPU)
#if defined(I486_CPU) || defined(I586_CPU)
	cmpl	$CPUCLASS_386,_cpu_class
	jne	2f
#endif /* I486_CPU || I586_CPU */

	movl	%edx,%eax
	shrl	$PGSHIFT,%edx		# calculate pte address
	movb	_PTmap(,%edx,4),%dl
	andb	$7,%dl
	cmpb	$7,%dl			# must be valid/user/write
	je	1f

	/* Simulate a trap. */
	pushl	%eax
	call	_trapwrite		# trapwrite(addr)
	addl	$4,%esp			# clear parameter from the stack
	movl	_curpcb,%ecx
	testl	%eax,%eax
	jnz	_fusufault

1:	/* XXX also need to check the following byte for validity! */
	movl	4(%esp),%edx
#endif

2:	movl	8(%esp),%eax
	gs
	movw	%ax,(%edx)
	xorl	%eax,%eax
	movl	%eax,PCB_ONFAULT(%ecx)
	ret

/*
 * suswintr(caddr_t uaddr, short x);
 * Store a short in the user's address space.  Can be called during an
 * interrupt.
 */
ENTRY(suswintr)
	movl	_curpcb,%ecx
	/*
	 * Use a different fault handler than susword() to signal trap() not to
	 * try to page fault.
	 */
	movl	$_fusubail,PCB_ONFAULT(%ecx)
	jmp	susword1

/*
 * subyte(caddr_t uaddr, char x);
 * Store a byte in the user's address space.
 */
ENTRY(subyte)
ALTENTRY(suibyte)
	movl	_curpcb,%ecx
	movl	$_fusufault,PCB_ONFAULT(%ecx)
	movl	4(%esp),%edx

#if defined(I386_CPU)
#if defined(I486_CPU) || defined(I586_CPU)
	cmpl	$CPUCLASS_386,_cpu_class
	jne	2f
#endif /* I486_CPU || I586_CPU */

	movl	%edx,%eax
	shrl	$PGSHIFT,%edx		# calculate pte address
	movb	_PTmap(,%edx,4),%dl
	andb	$7,%dl
	cmpb	$7,%dl			# must be valid/user/write
	je	1f

	/* Simulate a trap. */
	pushl	%eax
	call	_trapwrite		# trapwrite(addr)
	addl	$4,%esp			# clear parameter from the stack
	movl	_curpcb,%ecx
	testl	%eax,%eax
	jnz	_fusufault

1:	movl	4(%esp),%edx
#endif

2:	movb	8(%esp),%al
	gs
	movb	%al,(%edx)
	xorl	%eax,%eax
	movl	%eax,PCB_ONFAULT(%ecx)
	ret

/*****************************************************************************/

/*
 * The following is i386-specific nonsense.
 */

/*
 * void lgdt(struct region_descriptor *rdp);
 * Change the global descriptor table.
 */
ENTRY(lgdt)
	/* Reload the descriptor table. */
	movl	4(%esp),%eax
	lgdt	(%eax)
	/* Flush the prefetch q. */
	jmp	1f
	nop
1:	/* Reload "stale" selectors. */
	movl	$KDSEL,%eax
	movl	%ax,%ds
	movl	%ax,%es
	movl	%ax,%ss
	/* Reload code selector by doing intersegment return. */
	movl	(%esp),%eax
	pushl	%eax
	movl	$KCSEL,4(%esp)
	lret

ENTRY(rtcin)
	movl	4(%esp),%eax
	outb	%al,$0x70
	xorl	%eax,%eax		# clear eax
	inb	$0x71,%al
	ret

	# ssdtosd(*ssdp,*sdp)
ENTRY(ssdtosd)
	pushl	%ebx
	movl	8(%esp),%ecx
	movl	8(%ecx),%ebx
	shll	$16,%ebx
	movl	(%ecx),%edx
	roll	$16,%edx
	movb	%dh,%bl
	movb	%dl,%bh
	rorl	$8,%ebx
	movl	4(%ecx),%eax
	movw	%ax,%dx
	andl	$0xf0000,%eax
	orl	%eax,%ebx
	movl	12(%esp),%ecx
	movl	%edx,(%ecx)
	movl	%ebx,4(%ecx)
	popl	%ebx
	ret

ENTRY(setjmp)
	movl	4(%esp),%eax
	movl	%ebx,(%eax)		# save ebx
	movl	%esp,4(%eax)		# save esp
	movl	%ebp,8(%eax)		# save ebp
	movl	%esi,12(%eax)		# save esi
	movl	%edi,16(%eax)		# save edi
	movl	(%esp),%edx		# get rta
	movl	%edx,20(%eax)		# save eip
	xorl	%eax,%eax		# return (0);
	ret

ENTRY(longjmp)
	movl	4(%esp),%eax
	movl	(%eax),%ebx		# restore ebx
	movl	4(%eax),%esp		# restore esp
	movl	8(%eax),%ebp		# restore ebp
	movl	12(%eax),%esi		# restore esi
	movl	16(%eax),%edi		# restore edi
	movl	20(%eax),%edx		# get rta
	movl	%edx,(%esp)		# put in return frame
	xorl	%eax,%eax		# return (1);
	incl	%eax
	ret

/*****************************************************************************/

/*
 * The following primitives manipulate the run queues.
 * _whichqs tells which of the 32 queues _qs
 * have processes in them.  Setrq puts processes into queues, Remrq
 * removes them from queues.  The running process is on no queue,
 * other processes are on a queue related to p->p_pri, divided by 4
 * actually to shrink the 0-127 range of priorities into the 32 available
 * queues.
 */
	.globl	_whichqs,_qs,_cnt,_panic

/*
 * setrq(struct proc *p);
 * Insert a process on the appropriate queue.  Should be called at splclock().
 */
ENTRY(setrq)
	movl	4(%esp),%eax
#ifdef DIAGNOSTIC
	cmpl	$0,P_RLINK(%eax)	# should not be on q already
	jne	1f
	cmpl	$0,P_WCHAN(%eax)
	jne	1f
	cmpb	$SRUN,P_STAT(%eax)
	jne	1f
#endif
	movzbl	P_PRI(%eax),%edx
	shrl	$2,%edx
	btsl	%edx,_whichqs		# set q full bit
	leal	_qs(,%edx,8),%edx	# locate q hdr
	movl	%edx,P_LINK(%eax)	# link process on tail of q
	movl	P_RLINK(%edx),%ecx
	movl	%eax,P_RLINK(%edx)
	movl	%ecx,P_RLINK(%eax)
	movl	%eax,P_LINK(%ecx)
	ret
#ifdef DIAGNOSTIC
1:	pushl	$2f
	call	_panic
	/*NOTREACHED*/
2:	.asciz	"setrq"
#endif

/*
 * remrq(struct proc *p);
 * Remove a process from its queue.  Should be called at splclock().
 */
ENTRY(remrq)
	pushl	%esi
	movl	8(%esp),%esi
	movzbl	P_PRI(%esi),%eax
	shrl	$2,%eax
#ifdef DIAGNOSTIC
	btl	%eax,_whichqs
	jnc	1f
#endif
	movl	P_LINK(%esi),%ecx	# unlink process
	movl	P_RLINK(%esi),%edx
	movl	%edx,P_RLINK(%ecx)
	movl	%ecx,P_LINK(%edx)
	movl	$0,P_RLINK(%esi)	# zap reverse link to indicate off list
	cmpl	%edx,%ecx		# q still has something?
	jne	2f
	btrl	%eax,_whichqs		# no; clear bit
2:	popl	%esi
	ret
#ifdef DIAGNOSTIC
1:	pushl	$3f
	call	_panic
	/*NOTREACHED*/
3:	.asciz	"remrq"
#endif

/*
 * When no processes are on the runq, swtch() branches to here to wait for
 * something to come ready.
 */
ENTRY(idle)
	sti
	movl	$0,_cpl
	call	_spllower		# process pending interrupts

	ALIGN_TEXT
1:	cli
	movl	_whichqs,%ecx
	testl	%ecx,%ecx
	jnz	sw1
	sti
	jmp	1b

#ifdef DIAGNOSTIC
ENTRY(swtch_error)
	pushl	$1f
	call	_panic
	/*NOTREACHED*/
1:	.asciz	"swtch"
#endif

/*
 * swtch(void);
 * Find a runnable process and switch to it.  Wait if necessary.  If the new
 * process is the same as the old one, we short-circuit the context save and
 * restore.
 */
ENTRY(swtch)
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	_cpl

	incl	_cnt+V_SWTCH

	/* Don't accumulate system time while idle. */
	movl	_curproc,%esi
	movl	$0,_curproc

swtch_search:
	/*
	 * First phase: find new process.
	 *
	 * Registers:
	 *   %eax - queue head, scratch, then zero
	 *   %ebx - queue number
	 *   %ecx - cached value of whichqs
	 *   %edx - next process in queue
	 *   %esi - old process
	 *   %edi - new process
	 */

	/* Wait for new process. */
	cli				# splhigh doesn't do a cli
	movl	_whichqs,%ecx

sw1:	bsfl	%ecx,%ebx		# find a full q
	jz	_idle			# if none, idle

	leal	_qs(,%ebx,8),%eax	# select q

	movl	P_LINK(%eax),%edi	# unlink from front of process q
#ifdef	DIAGNOSTIC
	cmpl	%edi,%eax		# linked to self (e.g. nothing queued)?
	je	_swtch_error		# not possible
#endif
	movl	P_LINK(%edi),%edx
	movl	%edx,P_LINK(%eax)

	cmpl	%edx,%eax		# q empty
	jne	3f
	btrl	%ebx,%ecx		# yes, clear to indicate empty

3:	movl	P_RLINK(%edi),%eax
	movl	%eax,P_RLINK(%edx)

	movl	%ecx,_whichqs		# update q status

	/* We just did it. */
	xorl	%eax,%eax
	movl	%eax,_want_resched

#ifdef	DIAGNOSTIC
	cmpl	%eax,P_WCHAN(%edi)	# Waiting for something?
	jne	_swtch_error		# Yes; shouldn't be queued.
	cmpb	$SRUN,P_STAT(%edi)	# In run state?
	jne	_swtch_error		# No; shouldn't be queued.
#endif

	/* Isolate process.  XXX Is this necessary? */
	movl	%eax,P_RLINK(%edi)

	/* It's okay to take interrupts here. */
	sti

	/* Skip context switch if same process. */
	cmpl	%edi,%esi
	je	swtch_return

	/* If old process exited, don't bother. */
	testl	%esi,%esi
	jz	swtch_exited

	/*
	 * Second phase: save old context.
	 *
	 * Registers:
	 *   %eax - old process
	 *   %ecx - scratch
	 *   %esi - old pcb
	 *   %edi - new process
	 */

	/* Save context. */
	movl	%esi,%eax
	movl	P_ADDR(%esi),%esi

	movl	%esp,PCB_ESP(%esi)
	movl	%ebp,PCB_EBP(%esi)
	movl	%fs,%cx
	movl	%ecx,PCB_FS(%esi)
	movl	%gs,%cx
	movl	%ecx,PCB_GS(%esi)

#if NNPX > 0
	/* Have we used fp, and need a save? */
	cmpl	%eax,_npxproc
	jne	1f

	leal	PCB_SAVEFPU(%esi),%ecx
	pushl	%ecx
	call	_npxsave		# do it in a big C function
	addl	$4,%esp
1:
#endif

swtch_exited:
	/*
	 * Third phase: restore saved context.
	 *
	 * Registers:
	 *   %ecx - scratch
	 *   %esi - new pcb
	 *   %edi - new process
	 */

	/* No interrupts while loading new state. */
	cli
	movl	P_ADDR(%edi),%esi

	/* Switch address space. */
	movl	PCB_CR3(%esi),%ecx
	movl	%ecx,%cr3

#ifdef	USER_LDT
	cmpl	$0,PCB_USERLDT(%esi)
	jnz	1f
	movl	__default_ldt,%ecx
	cmpl	_currentldt,%ecx
	je	2f
	lldt	%ecx
	movl	%ecx,_currentldt
	jmp	2f
1:	pushl	%esi
	call	_set_user_ldt
	addl	$4,%esp
2:
#endif

	/* Restore stack and segments. */
	movl	PCB_ESP(%esi),%esp
	movl	PCB_EBP(%esi),%ebp
	movl	PCB_FS(%esi),%ecx
	movl	%cx,%fs
	movl	PCB_GS(%esi),%ecx
	movl	%cx,%gs

	/* Record new pcb. */
	movl	%esi,_curpcb

	/* Interrupts are okay again. */
	sti

swtch_return:
	/* Record new process. */
	movl	%edi,_curproc

	/* Old _cpl is already on the stack. */
	popl	_cpl
	call    _spllower		# restore the process's ipl

	movl	%edi,%eax		# return (p);
	popl	%edi
	popl	%esi
	popl	%ebx
	ret

/*
 * swtch_exit(struct proc *p);
 * Switch to proc0's saved context and deallocate the address space and kernel
 * stack for p.  Then jump into swtch(), as if we were in proc0 all along.
 */
	.globl	_proc0,_vmspace_free,_kernel_map,_kmem_free
ENTRY(swtch_exit)
	movl	4(%esp),%edi		# old process
	movl	$_proc0,%ebx

	incl	_cnt+V_SWTCH

	/* Restore proc0's context. */
	cli
	movl	P_ADDR(%ebx),%esi

	/* Switch address space. */
	movl	PCB_CR3(%esi),%ecx
	movl	%ecx,%cr3

	/* Can't have a user-set ldt. */

	/* Restore stack and segments. */
	movl	PCB_ESP(%esi),%esp
	movl	PCB_EBP(%esi),%ebp
	xorl	%ecx,%ecx		# always null in proc0
	movl	%cx,%fs
	movl	%cx,%gs

	/* Record new pcb. */
	movl	%esi,_curpcb

	/* Interrupts are okay again. */
	sti

	/* Thoroughly nuke the old process's resources. */
	pushl	P_VMSPACE(%edi)
	call	_vmspace_free
	pushl	$(UPAGES << PGSHIFT)
	pushl	P_ADDR(%edi)
	pushl	_kernel_map
	call	_kmem_free
	addl	$16,%esp

	/* Jump into swtch() with the right state. */
	movl	%ebx,%esi
	movl	$0,_curproc
	jmp	swtch_search

/*
 * savectx(struct pcb *pcb, int altreturn);
 * Update pcb, saving current processor state and arranging for alternate
 * return in swtch() if altreturn is true.
 */
ENTRY(savectx)
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	pushl	_cpl

	/* Save the context. */
	movl	20(%esp),%esi		# esi = p2->p_addr

	movl	%esp,PCB_ESP(%esi)
	movl	%ebp,PCB_EBP(%esi)
	movl	%fs,%cx
	movl	%ecx,PCB_FS(%esi)
	movl	%gs,%cx
	movl	%ecx,PCB_GS(%esi)

#if NNPX > 0
	/*
	 * If npxproc == NULL, then the npx h/w state is irrelevant and the
	 * state had better already be in the pcb.  This is true for forks
	 * but not for dumps (the old book-keeping with FP flags in the pcb
	 * always lost for dumps because the dump pcb has 0 flags).
	 *
	 * If npxproc != NULL, then we have to save the npx h/w state to
	 * npxproc's pcb and copy it to the requested pcb, or save to the
	 * requested pcb and reload.  Copying is easier because we would
	 * have to handle h/w bugs for reloading.  We used to lose the
	 * parent's npx state for forks by forgetting to reload.
	 */
	movl	_curproc,%edi		# edi = p1
	cmpl	%edi,_npxproc
	jne	1f

	leal	PCB_SAVEFPU(%esi),%ebx	# ebx = esi->u_pcb.pcb_savefpu
	pushl	%ebx
	call	_npxsave
	addl	$4,%esp

	pushl	$108+8*2		# XXX h/w state size + padding
	movl	P_ADDR(%edi),%edi	# edi = p1->p_addr
	leal	PCB_SAVEFPU(%edi),%edi	# edi = edi->u_pcb.pcb_savefpu
	pushl	%edi
	pushl	%ebx
	call	_bcopy
	addl	$12,%esp
1:
#endif

	/* Copy the stack if requested. */
	cmpl	$0,24(%esp)
	je	1f
	movl	%esp,%eax		# eax = stack pointer
	movl	%eax,%edx		# edx = stack offset from bottom
	subl	$_kstack,%edx
	movl	$(UPAGES << PGSHIFT),%ecx	# ecx = ctob(UPAGES) - offset
	subl	%edx,%ecx
	pushl	%ecx
	addl	%edx,%esi		# esi = stack in p2
	pushl	%esi
	pushl	%eax
	call	_bcopy
	addl	$12,%esp
	
1:	/* This is the parent.  The child will return from swtch(). */
	xorl	%eax,%eax		# return 0
	addl	$4,%esp			# drop saved _cpl on the floor
	popl	%edi
	popl	%esi
	popl	%ebx
	ret

/*****************************************************************************/

/*
 * This is used for profiling.
 */

/*
 * addupc(int pc, struct uprof *up, int ticks):
 * Update profiling information for the user process.
 */
ENTRY(addupc)
	movl	4(%esp),%eax		# pc
	movl	8(%esp),%edx		# up

	subl	PR_OFF(%edx),%eax	# pc -= up->pr_off
	jc	1f			# if (pc < 0) return

	shrl	$1,%eax			# praddr = pc >> 1
	imull	PR_SCALE(%edx),%eax	# praddr *= up->pr_scale
	shrl	$15,%eax		# praddr = praddr << 15
	andl	$-2,%eax		# praddr &= ~1

	cmpl	PR_SIZE(%edx),%eax	# if (praddr > up->pr_size) return
	ja	1f

/*	addl	%eax,%eax		# praddr -> word offset */
	addl	PR_BASE(%edx),%eax	# praddr += up-> pr_base
	movl	12(%esp),%ecx		# ticks

	movl	_curpcb,%edx
	movl	$_proffault,PCB_ONFAULT(%edx)
	addl	%ecx,(%eax)		# storage location += ticks
	movl	$0,PCB_ONFAULT(%edx)

1:	ret

ENTRY(proffault)
	/* If we get a fault, then kill profiling all together. */
	movl	$0,PCB_ONFAULT(%edx)	# squish the fault handler
	movl	8(%esp),%ecx
	movl	$0,PR_SCALE(%ecx)	# up->pr_scale = 0
	ret

/*****************************************************************************/

/*
 * Trap and fault vector routines
 *
 * XXX - debugger traps are now interrupt gates so at least bdb doesn't lose
 * control.  The sti's give the standard losing behaviour for ddb and kgdb.
 */ 
#define	IDTVEC(name)	ALIGN_TEXT; .globl _X/**/name; _X/**/name:
#define	INTRENTRY \
	pushal			; \
	pushl	%ds		; \
	pushl	%es		; /* now the stack frame is a trap frame */ \
	movl	$KDSEL,%eax	; \
	movl	%ax,%ds		; \
	movl	%ax,%es
#define	INTREXIT \
	jmp	_Xdoreti
#define	INTRFASTEXIT \
	popl	%es		; \
	popl	%ds		; \
	popal			; \
	addl	$8,%esp		; \
	iret

#define	TRAP(a)		pushl $(a) ; jmp _alltraps
#define	ZTRAP(a)	pushl $0 ; TRAP(a)
#ifdef KGDB
#define	BPTTRAP(a)	testb $(PSL_I>>8),13(%esp) ; jz 1f ; sti ; 1: ; \
			pushl $(a) ; jmp _bpttraps
#else
#define	BPTTRAP(a)	testb $(PSL_I>>8),13(%esp) ; jz 1f ; sti ; 1: ; \
			TRAP(a)
#endif

	.text
IDTVEC(div)
	ZTRAP(T_DIVIDE)
IDTVEC(dbg)
#ifdef BDB
	BDBTRAP(dbg)
#endif
	subl	$4,%esp
	pushl	%eax
/*	movl	%dr6,%eax		# XXX stupid assembler! */
	.byte	0x0f, 0x21, 0xf0
	movl	%eax,4(%esp)
	andb	$~15,%al
/*	movl	%eax,%dr6		# XXX stupid assembler! */
	.byte	0x0f, 0x23, 0xf0
	popl	%eax
	BPTTRAP(T_TRCTRAP)
IDTVEC(nmi)
	ZTRAP(T_NMI)
IDTVEC(bpt)
#ifdef BDB
	BDBTRAP(bpt)
#endif
	pushl	$0
	BPTTRAP(T_BPTFLT)
IDTVEC(ofl)
	ZTRAP(T_OFLOW)
IDTVEC(bnd)
	ZTRAP(T_BOUND)
IDTVEC(ill)
	ZTRAP(T_PRIVINFLT)
IDTVEC(dna)
	ZTRAP(T_DNA)
IDTVEC(dble)
	TRAP(T_DOUBLEFLT)
IDTVEC(fpusegm)
	ZTRAP(T_FPOPFLT)
IDTVEC(tss)
	TRAP(T_TSSFLT)
IDTVEC(missing)
	TRAP(T_SEGNPFLT)
IDTVEC(stk)
	TRAP(T_STKFLT)
IDTVEC(prot)
	TRAP(T_PROTFLT)
IDTVEC(page)
	TRAP(T_PAGEFLT)
IDTVEC(rsvd)
	ZTRAP(T_RESERVED)
IDTVEC(fpu)
#if NNPX > 0
	/*
	 * Handle like an interrupt so that we can call npxintr to clear the
	 * error.  It would be better to handle npx interrupts as traps but
	 * this is difficult for nested interrupts.
	 */
	pushl	$0			# dummy error code
	pushl	$T_ASTFLT
	INTRENTRY
	pushl	_cpl
	pushl	%esp
	incl	_cnt+V_TRAP
	call	_npxintr
	addl	$4,%esp
	INTREXIT
#else
	ZTRAP(T_ARITHTRAP)
#endif
IDTVEC(align)
	ZTRAP(T_ALIGNFLT)
	/* 18 - 31 reserved for future exp */
IDTVEC(rsvd1)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd2)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd3)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd4)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd5)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd6)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd7)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd8)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd9)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd10)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd11)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd12)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd13)
	ZTRAP(T_RESERVED)
IDTVEC(rsvd14)
	ZTRAP(T_RESERVED)

ENTRY(alltraps)
	INTRENTRY
calltrap:
	call	_trap
	/*
	 * Check for ASTs.
	 */
	testb	$SEL_RPL_MASK,TF_CS(%esp)
	jz	1f
	btrl	$0,_astpending
	jnc	1f
	movl	$T_ASTFLT,TF_TRAPNO(%esp)
	call	_trap
1:	INTRFASTEXIT

#ifdef KGDB
/*
 * This code checks for a kgdb trap, then falls through
 * to the regular trap code.
 */
ENTRY(bpttraps)
	INTRENTRY
	testb	$SEL_RPL_MASK,TF_CS(%esp)
	jne	calltrap
	call	_kgdb_trap_glue		
	jmp	calltrap
#endif

/*
 * Call gate entry for syscall
 */
IDTVEC(syscall)
	pushl	$0	# Room for tf_err
	pushfl		# Room for tf_trapno
	pushfl		# turn off trace bit
	andb	$~(PSL_T>>8),1(%esp)
	popfl
	INTRENTRY
	movl	TF_TRAPNO(%esp),%eax	# copy eflags from tf_trapno to tf_eflags
	movl	%eax,TF_EFLAGS(%esp)
	call	_syscall
	/*
	 * Check for ASTs.
	 */
	/* Always returning to user mode here. */
	btrl	$0,_astpending
	jnc	1f
	movl	$T_ASTFLT,TF_TRAPNO(%esp)
	call	_trap
1:	INTRFASTEXIT

#include <i386/isa/vector.s>
#include <i386/isa/icu.s>
