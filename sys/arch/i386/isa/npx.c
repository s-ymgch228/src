/*	$NetBSD: npx.c,v 1.31 1994/11/30 04:42:07 mycroft Exp $	*/

/*-
 * Copyright (c) 1994 Charles Hannum.
 * Copyright (c) 1990 William Jolitz.
 * Copyright (c) 1991 The Regents of the University of California.
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
 *	@(#)npx.c	7.2 (Berkeley) 5/12/91
 */
#include "npx.h"
#if NNPX > 0

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/file.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/ioctl.h>
#include <sys/device.h>

#include <machine/cpu.h>
#include <machine/pio.h>
#include <machine/cpufunc.h>
#include <machine/pcb.h>
#include <machine/trap.h>
#include <machine/specialreg.h>

#include <i386/isa/icu.h>
#include <i386/isa/isavar.h>
#include <i386/isa/isareg.h>

/*
 * 387 and 287 Numeric Coprocessor Extension (NPX) Driver.
 */

#ifdef	__GNUC__

#define	fldcw(addr)		__asm("fldcw %0" : : "m" (*addr))
#define	fnclex()		__asm("fnclex")
#define	fninit()		__asm("fninit")
#define	fnsave(addr)		__asm("fnsave %0" : "=m" (*addr) : "0" (*addr))
#define	fnstcw(addr)		__asm("fnstcw %0" : "=m" (*addr) : "0" (*addr))
#define	fnstsw(addr)		__asm("fnstsw %0" : "=m" (*addr) : "0" (*addr))
#define	fp_divide_by_0()	__asm("fldz; fld1; fdiv %st,%st(1); fwait")
#define	frstor(addr)		__asm("frstor %0" : : "m" (*addr))
#define	fwait()			__asm("fwait")
#define	read_eflags()		({register u_long ef; \
				  __asm("pushfl; popl %0" : "=r" (ef)); \
				  ef;})
#define	write_eflags(x)		({register u_long ef = (x); \
				  __asm("pushl %0; popfl" : : "r" (ef));})
#define	start_emulating()	({register u_short msw; \
				  __asm("smsw %0" : "=r" (msw)); \
				  msw |= CR0_TS; \
				  __asm("lmsw %0" : : "r" (msw));})
#define	stop_emulating()	__asm("clts")

#else	/* not __GNUC__ */

void	disable_intr	__P((void));
void	enable_intr	__P((void));
void	fldcw		__P((caddr_t addr));
void	fnclex		__P((void));
void	fninit		__P((void));
void	fnsave		__P((caddr_t addr));
void	fnstcw		__P((caddr_t addr));
void	fnstsw		__P((caddr_t addr));
void	fp_divide_by_0	__P((void));
void	frstor		__P((caddr_t addr));
void	fwait		__P((void));
u_long	read_eflags	__P((void));
void	start_emulating	__P((void));
void	stop_emulating	__P((void));
void	write_eflags	__P((u_long ef));

#endif	/* __GNUC__ */

typedef u_char bool_t;

extern	struct gate_descriptor idt[];

int	npxdna		__P((void));
void	npxexit		__P((void));
void	npxinit		__P((void));
int	npxintr		__P((struct intrframe *frame));
void	npxsave		__P((struct save87 *addr));
int	npxprobe1	__P((struct isa_attach_args *));

int npxprobe __P((struct device *, void *, void *));
void npxattach __P((struct device *, struct device *, void *));

struct cfdriver npxcd = {
	NULL, "npx", npxprobe, npxattach, DV_DULL, sizeof(struct device)
};

struct proc	*npxproc;

enum npx_type {
	NPX_NONE = 0,
	NPX_INTERRUPT,
	NPX_EXCEPTION,
	NPX_BROKEN,
};

static	enum npx_type		npx_type;
static	int			npx_nointr;
static	volatile u_int		npx_intrs_while_probing;
static	volatile u_int		npx_traps_while_probing;

/*
 * Special interrupt handlers.  Someday intr0-intr15 will be used to count
 * interrupts.  We'll still need a special exception 16 handler.  The busy
 * latch stuff in probintr() can be moved to npxprobe().
 */
void probeintr(void);
asm ("
	.text
_probeintr:
	ss
	incl	_npx_intrs_while_probing
	pushl	%eax
	movb	$0x20,%al	# EOI (asm in strings loses cpp features)
	outb	%al,$0xa0	# IO_ICU2
	outb	%al,$0x20	# IO_ICU1
	movb	$0,%al
	outb	%al,$0xf0	# clear BUSY# latch
	popl	%eax
	iret
");

void probetrap(void);
asm ("
	.text
_probetrap:
	ss
	incl	_npx_traps_while_probing
	fnclex
	iret
");

/*
 * Probe routine.  Initialize cr0 to give correct behaviour for [f]wait
 * whether the device exists or not (XXX should be elsewhere).  Set flags
 * to tell npxattach() what to do.  Modify device struct if npx doesn't
 * need to use interrupts.  Return 1 if device exists.
 */
int
npxprobe(parent, match, aux)
	struct device *parent;
	void *match, *aux;
{
	struct	isa_attach_args *ia = aux;
	int	irq;
	int	result;
	u_long	save_eflags;
	unsigned save_imen;
	struct	gate_descriptor save_idt_npxintr;
	struct	gate_descriptor save_idt_npxtrap;

	/*
	 * This routine is now just a wrapper for npxprobe1(), to install
	 * special npx interrupt and trap handlers, to enable npx interrupts
	 * and to disable other interrupts.  Someday isa_configure() will
	 * install suitable handlers and run with interrupts enabled so we
	 * won't need to do so much here.
	 */
	irq = NRSVIDT + ia->ia_irq;
	save_eflags = read_eflags();
	disable_intr();
	save_imen = imen;
	save_idt_npxintr = idt[irq];
	save_idt_npxtrap = idt[16];
	setgate(&idt[irq], probeintr, 0, SDT_SYS386IGT, SEL_KPL);
	setgate(&idt[16], probetrap, 0, SDT_SYS386TGT, SEL_KPL);
	imen = ~((1 << IRQ_SLAVE) | (1 << ia->ia_irq));
	SET_ICUS();

	/*
	 * Partially reset the coprocessor, if any.  Some BIOS's don't reset
	 * it after a warm boot.
	 */
	outb(0xf1, 0);		/* full reset on some systems, NOP on others */
	delay(1000);
	outb(0xf0, 0);		/* clear BUSY# latch */

	/*
	 * Prepare to trap all ESC (i.e., NPX) instructions and all WAIT
	 * instructions.  We must set the CR0_MP bit and use the CR0_TS
	 * bit to control the trap, because setting the CR0_EM bit does
	 * not cause WAIT instructions to trap.  It's important to trap
	 * WAIT instructions - otherwise the "wait" variants of no-wait
	 * control instructions would degenerate to the "no-wait" variants
	 * after FP context switches but work correctly otherwise.  It's
	 * particularly important to trap WAITs when there is no NPX -
	 * otherwise the "wait" variants would always degenerate.
	 *
	 * Try setting CR0_NE to get correct error reporting on 486DX's.
	 * Setting it should fail or do nothing on lesser processors.
	 */
	lcr0(rcr0() | CR0_MP | CR0_NE);

	/*
	 * But don't trap while we're probing.
	 */
	stop_emulating();

	enable_intr();
	result = npxprobe1(ia);
	disable_intr();

	start_emulating();

	imen = save_imen;
	SET_ICUS();
	idt[irq] = save_idt_npxintr;
	idt[16] = save_idt_npxtrap;
	write_eflags(save_eflags);
	return (result);
}

int
npxprobe1(ia)
	struct isa_attach_args *ia;
{
	int control;
	int status;

	ia->ia_iosize = 16;
	ia->ia_msize = 0;

	/*
	 * Finish resetting the coprocessor, if any.  If there is an error
	 * pending, then we may get a bogus IRQ13, but probeintr() will handle
	 * it OK.  Bogus halts have never been observed, but we enabled
	 * IRQ13 and cleared the BUSY# latch early to handle them anyway.
	 */
	fninit();
	delay(1000);		/* wait for any IRQ13 (fwait might hang) */

	npx_intrs_while_probing = 0;
	npx_traps_while_probing = 0;

	/*
	 * Check for a status of mostly zero.
	 */
	status = 0x5a5a;
	fnstsw(&status);
	if ((status & 0xb8ff) == 0) {
		/*
		 * Good, now check for a proper control word.
		 */
		control = 0x5a5a;	
		fnstcw(&control);
		if ((control & 0x1f3f) == 0x033f) {
			/*
			 * We have an npx, now divide by 0 to see if exception
			 * 16 works.
			 */
			control &= ~(1 << 2);	/* enable divide by 0 trap */
			fldcw(&control);
			npx_traps_while_probing = npx_intrs_while_probing = 0;
			fp_divide_by_0();
			if (npx_traps_while_probing != 0) {
				/*
				 * Good, exception 16 works.
				 */
				npx_type = NPX_EXCEPTION;
				ia->ia_irq = IRQUNK;	/* zap the interrupt */
			} else if (npx_intrs_while_probing != 0) {
				/*
				 * Bad, we are stuck with IRQ13.
				 */
				npx_type = NPX_INTERRUPT;
			} else {
				/*
				 * Worse, even IRQ13 is broken.  Use emulator.
				 */
				npx_type = NPX_BROKEN;
				ia->ia_irq = IRQUNK;
			}
			return 1;
		}
	}
	/*
	 * Probe failed.  There is no usable FPU.
	 */
	npx_type = NPX_NONE;
	return 0;
}

int npx586bug1 __P((int, int));
asm ("
	.text
_npx586bug1:
	fildl	4(%esp)		# x
	fildl	8(%esp)		# y
	fld	%st(1)
	fdiv	%st(1),%st	# x/y
	fmulp	%st,%st(1)	# (x/y)*y
	fsubrp	%st,%st(1)	# x-(x/y)*y
	pushl	$0
	fistpl	(%esp)
	popl	%eax
	ret
");

/*
 * Attach routine - announce which it is, and wire into system
 */
void
npxattach(parent, self, aux)
	struct device *parent, *self;
	void *aux;
{
	struct isa_attach_args *ia = aux;
	static struct intrhand npxhand;

	switch (npx_type) {
	case NPX_INTERRUPT:
		printf("\n");
		npxhand.ih_fun = npxintr;
		npxhand.ih_arg = 0;
		npxhand.ih_level = IPL_NONE;
		intr_establish(ia->ia_irq, &npxhand);
		break;
	case NPX_EXCEPTION:
		printf(": using exception 16\n");
		break;
	case NPX_BROKEN:
		printf(": error reporting broken; using emulator\n");
		npx_type = NPX_NONE;
		return;
	}

	stop_emulating();
	fninit();
	if (npx586bug1(4195835, 3145727) != 0)
		printf("WARNING: Pentium FDIV bug detected!\n");
	start_emulating();

	npxinit();
}

/*
 * Record the FPU state and reinitialize it all except for the control word.
 * Then generate a SIGFPE.
 *
 * Reinitializing the state allows naive SIGFPE handlers to longjmp without
 * doing any fixups.
 *
 * XXX there is currently no way to pass the full error state to signal
 * handlers, and if this is a nested interrupt there is no way to pass even
 * a status code!  So there is no way to have a non-naive SIGFPE handler.  At
 * best a handler could do an fninit followed by an fldcw of a static value.
 * fnclex would be of little use because it would leave junk on the FPU stack.
 * Returning from the handler would be even less safe than usual because
 * IRQ13 exception handling makes exceptions even less precise than usual.
 */
int
npxintr(frame)
	struct intrframe *frame;
{
	register struct proc *p = npxproc;
	register struct save87 *addr;
	int code;

	cnt.v_trap++;

	if (p == 0 || npx_type == NPX_NONE) {
		/* XXX no %p in stand/printf.c.  Cast to quiet gcc -Wall. */
		printf("npxintr: p = %lx, curproc = %lx, npx_type = %d\n",
		       (u_long) p, (u_long) curproc, npx_type);
		panic("npxintr from nowhere");
	}
	/*
	 * Clear the interrupt latch.
	 */
	outb(0xf0, 0);
	/*
	 * If we're saving, ignore the interrupt.  The FPU will happily
	 * generate another one when we restore the state later.
	 */
	if (npx_nointr != 0)
		return (1);
	/*
	 * Find the address of npxproc's savefpu.  This is not necessarily
	 * the one in curpcb.
	 */
	addr = &p->p_addr->u_pcb.pcb_savefpu;
	/*
	 * Save state.  This does an implied fninit.  It had better not halt
	 * the cpu or we'll hang.
	 */
	fnsave(addr);
	fwait();
	/*
	 * Restore control word (was clobbered by fnsave).
	 */
	fldcw(&addr->sv_env.en_cw);
	fwait();
	/*
	 * Remember the exception status word and tag word.  The current
	 * (almost fninit'ed) fpu state is in the fpu and the exception
	 * state just saved will soon be junk.  However, the implied fninit
	 * doesn't change the error pointers or register contents, and we
	 * preserved the control word and will copy the status and tag
	 * words, so the complete exception state can be recovered.
	 */
	addr->sv_ex_sw = addr->sv_env.en_sw;
	addr->sv_ex_tw = addr->sv_env.en_tw;

	/*
	 * Pass exception to process.  If it's the current process, try to do
	 * it immediately.
	 */
	if (p == curproc && ISPL(frame->if_cs) == SEL_UPL) {
		/*
		 * Interrupt is essentially a trap, so we can afford to call
		 * the SIGFPE handler (if any) as soon as the interrupt
		 * returns.
		 *
		 * XXX little or nothing is gained from this, and plenty is
		 * lost - the interrupt frame has to contain the trap frame
		 * (this is otherwise only necessary for the rescheduling trap
		 * in doreti, and the frame for that could easily be set up
		 * just before it is used).
		 */
		p->p_md.md_regs = (int *)&frame->if_es;
#ifdef notyet
		/*
		 * Encode the appropriate code for detailed information on
		 * this exception.
		 */
		code = XXX_ENCODE(addr->sv_ex_sw);
#else
		code = 0;	/* XXX */
#endif
		trapsignal(p, SIGFPE, code);
	} else {
		/*
		 * Nested interrupt.  These losers occur when:
		 *	o an IRQ13 is bogusly generated at a bogus time, e.g.:
		 *		o immediately after an fnsave or frstor of an
		 *		  error state.
		 *		o a couple of 386 instructions after
		 *		  "fstpl _memvar" causes a stack overflow.
		 *	  These are especially nasty when combined with a
		 *	  trace trap.
		 *	o an IRQ13 occurs at the same time as another higher-
		 *	  priority interrupt.
		 *
		 * Treat them like a true async interrupt.
		 */
		psignal(p, SIGFPE);
	}

	return (1);
}

/*
 * Implement device not available (DNA) exception
 *
 * If the we were the last process to use the FPU, we can simply return.
 * Otherwise, we save the previous state, if necessary, and restore our last
 * saved state.
 */
int
npxdna()
{

	if (npx_type == NPX_NONE)
		return (0);

#ifdef DIAGNOSTIC
	if (cpl != 0 || npx_nointr != 0)
		panic("npxdna");
#endif
	stop_emulating();
	if (npxproc != 0) {
		if (npxproc == curproc)
			return (1);
		npx_nointr = 1;
		fnsave(&npxproc->p_addr->u_pcb.pcb_savefpu);
		fwait();
		npx_nointr = 0;
	}
	/*
	 * The following frstor may cause an IRQ13 when the state being
	 * restored has a pending error.  The error will appear to have been
	 * triggered by the current (npx) user instruction even when that
	 * instruction is a no-wait instruction that should not trigger an
	 * error (e.g., fnclex).  On at least one 486 system all of the
	 * no-wait instructions are broken the same as frstor, so our
	 * treatment does not amplify the breakage.  On at least one
	 * 386/Cyrix 387 system, fnclex works correctly while frstor and
	 * fnsave are broken, so our treatment breaks fnclex if it is the
	 * first FPU instruction after a context switch.
	 */
	frstor(&(npxproc = curproc)->p_addr->u_pcb.pcb_savefpu);
	return (1);
}

/*
 * Wrapper for fnsave instruction to handle h/w bugs.  If there is an error
 * pending, then fnsave generates a bogus IRQ13 on some systems.  Force any
 * IRQ13 to be handled immediately, and then ignore it.
 *
 * This routine is always called at spl0.  If it might called with the NPX
 * interrupt masked, it would be necessary to forcibly unmask the NPX interrupt
 * so that it could succeed.
 *
 * The FNSAVE instruction clears the FPU state.  Rather than reloading the FPU
 * immediately, we clear npxproc and turn on CR0_TS to force a DNA and a reload
 * of the FPU state the next time we try to use it.  Since this routine is only
 * called when forking, so this algorithm at worst forces us to trap once per
 * fork(), and at best saves us a reload once per fork().
 */
void
npxsave(addr)
	struct save87 *addr;
{

#ifdef DIAGNOSTIC
	if (cpl != 0 || npx_nointr != 0)
		panic("npxsave");
#endif
	stop_emulating();
	npx_nointr = 1;
	fnsave(addr);
	fwait();
	npx_nointr = 0;
	npxproc = 0;
	start_emulating();
}

/*
 * Initialize floating point unit.
 *
 * This is normally called at spl0, so NPX interrupts will be caught and
 * ignored.  It is called once during boot at splhigh, but there is never any
 * state to save then.
 */
void
npxinit()
{
	static u_short control = __INITIAL_NPXCW__;
#ifdef DIAGNOSTIC
	extern int cold;
#endif

	if (npx_type == NPX_NONE)
		return;

#ifdef DIAGNOSTIC
	if (cpl != 0 && !cold || npx_nointr != 0)
		panic("npxinit");
#endif
	stop_emulating();
	npx_nointr = 1;
	if (npxproc != 0 && npxproc != curproc)
		fnsave(&npxproc->p_addr->u_pcb.pcb_savefpu);
	else
		fninit();
	fwait();
	npx_nointr = 0;
	npxproc = curproc;
	fldcw(&control);
	fwait();
}

#endif /* NNPX > 0 */
