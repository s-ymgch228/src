/*	$NetBSD: cpu.h,v 1.25 1998/09/11 16:46:31 jonathan Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Ralph Campbell and Rick Macklem.
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
 *	@(#)cpu.h	8.4 (Berkeley) 1/4/94
 */

#ifndef _CPU_H_
#define _CPU_H_

/*
 * Exported definitions unique to NetBSD/mips cpu support.
 */

/*
 * Macros to find the CPU architecture we're on at run-time,
 * or if possible, at compile-time.
 */

#if (MIPS1 + MIPS3) == 1
#ifdef MIPS1
# define CPUISMIPS3	0
#endif /* mips1 */

#ifdef MIPS3
#  define CPUISMIPS3	 1
#endif /* mips1 */

#else /* run-time test */
extern int cpu_arch;
#define CPUISMIPS3	(cpu_arch == 3)
#endif /* run-time test */

/*
 * definitions of cpu-dependent requirements
 * referenced in generic code
 */
#define	cpu_wait(p)			/* nothing */
#define	cpu_swapout(p)			panic("cpu_swapout: can't get here");

/*
 * Arguments to hardclock and gatherstats encapsulate the previous
 * machine state in an opaque clockframe.
 */
struct clockframe {
	int	pc;	/* program counter at time of interrupt */
	int	sr;	/* status register at time of interrupt */
};

/*
 * A port must provde CLKF_USERMODE() and CLKF_BASEPRI() for use
 * in machine-independent code. These differ on r4000 and r3000 systems;
 * provide them in the port-dependent file that includes this one, using
 * the macros below.
 */

/* mips1 versions */
#define	MIPS1_CLKF_USERMODE(framep)	((framep)->sr & MIPS_SR_KU_PREV)
#define	MIPS1_CLKF_BASEPRI(framep)	\
	((~(framep)->sr & (MIPS_INT_MASK | MIPS_SR_INT_ENA_PREV)) == 0)

/* mips3 versions */
#define	MIPS3_CLKF_USERMODE(framep)	((framep)->sr & MIPS_SR_KSU_USER)
#define	MIPS3_CLKF_BASEPRI(framep)	\
	((~(framep)->sr & (MIPS_INT_MASK | MIPS_SR_INT_ENAB)) == 0)

#define	CLKF_PC(framep)		((framep)->pc)
#define	CLKF_INTR(framep)	(0)

#if defined(MIPS3) && !defined(MIPS1)
#define	CLKF_USERMODE(framep)	MIPS3_CLKF_USERMODE(framep)
#define	CLKF_BASEPRI(framep)	MIPS3_CLKF_BASEPRI(framep)
#endif

#if !defined(MIPS3) && defined(MIPS1)
#define	CLKF_USERMODE(framep)	MIPS1_CLKF_USERMODE(framep)
#define	CLKF_BASEPRI(framep)	MIPS1_CLKF_BASEPRI(framep)
#endif


#if defined(MIPS3) && defined(MIPS1)
#define CLKF_USERMODE(framep) \
    ((CPUISMIPS3) ? MIPS3_CLKF_USERMODE(framep):  MIPS1_CLKF_USERMODE(framep))
#define CLKF_BASEPRI(framep) \
    ((CPUISMIPS3) ? MIPS3_CLKF_BASEPRI(framep):  MIPS1_CLKF_BASEPRI(framep))
#endif



/*
 * Preempt the current process if in interrupt from user mode,
 * or after the current trap/syscall if in system mode.
 */
#define	need_resched()	{ want_resched = 1; aston(); }

/*
 * Give a profiling tick to the current process when the user profiling
 * buffer pages are invalid.  On the MIPS, request an ast to send us
 * through trap, marking the proc as needing a profiling tick.
 */
#define	need_proftick(p)	{ (p)->p_flag |= P_OWEUPC; aston(); }

/*
 * Notify the current process (p) that it has a signal pending,
 * process as soon as possible.
 */
#define	signotify(p)	aston()

#define aston()		(astpending = 1)

extern int astpending;	/* need to trap before returning to user mode */
extern int want_resched;	/* resched() was called */
#ifdef MIPS3
extern u_int	mips_L2CacheSize;
extern int	mips_L2CacheIsSnooping; /* L2 cache snoops uncached writes ? */
extern int	mips_L2CacheMixed;

#ifdef MIPS3_INTERNAL_TIMER_INTERRUPT
extern u_int32_t mips3_intr_cycle_count;
extern u_int32_t mips3_timer_delta;
#endif
#endif

/*
 * CTL_MACHDEP definitions.
 */
#define	CPU_CONSDEV		1	/* dev_t: console terminal device */
#define	CPU_MAXID		2	/* number of valid machdep ids */

#define CTL_MACHDEP_NAMES { \
	{ 0, 0 }, \
	{ "console_device", CTLTYPE_STRUCT }, \
}

/*
 * Misc prototypes.
 */

struct user;

caddr_t	allocsys __P((caddr_t));
void	dumpsys __P((void));
int	savectx __P((struct user *));
void	mips_init_msgbuf __P((void));
void	mips_init_proc0 __P((caddr_t));

/* locore.S */
extern void savefpregs __P((struct proc *));

/* mips_machdep.c */
extern void cpu_identify __P((void));
extern void mips_vector_init __P((void));

/*
 * MIPS CPU types (cp_imp).
 */
#define	MIPS_R2000	0x01	/* MIPS R2000 CPU		ISA I   */
#define	MIPS_R3000	0x02	/* MIPS R3000 CPU		ISA I   */
#define	MIPS_R6000	0x03	/* MIPS R6000 CPU		ISA II	*/
#define	MIPS_R4000	0x04	/* MIPS R4000/4400 CPU		ISA III	*/
#define MIPS_R3LSI	0x05	/* LSI Logic R3000 derivate	ISA I	*/
#define	MIPS_R6000A	0x06	/* MIPS R6000A CPU		ISA II	*/
#define	MIPS_R3IDT	0x07	/* IDT R3000 derivate		ISA I	*/
#define	MIPS_R10000	0x09	/* MIPS R10000/T5 CPU		ISA IV  */
#define	MIPS_R4200	0x0a	/* MIPS R4200 CPU (ICE)		ISA III */
#define MIPS_R4300	0x0b	/* NEC VR4300 CPU		ISA III */
#define MIPS_UNKC2	0x0c	/* unnanounced product cpu	ISA III */
#define	MIPS_R8000	0x10	/* MIPS R8000 Blackbird/TFP	ISA IV  */
#define	MIPS_R4600	0x20	/* QED R4600 Orion		ISA III */
/* ID conflict */
#define	MIPS_R4700	0x21	/* QED R4700 Orion		ISA III */
#define	MIPS_R3SONY	MIPS_R4700 /* Sony R3000 CPU		ISA I CLASH */

#define	MIPS_R3TOSH	0x22	/* Toshiba R3000 based CPU	ISA I	*/
/* ID conflict */
#define	MIPS_R5000	0x23	/* MIPS R5000 based CPU		ISA IV  */
#define	MIPS_R3NKK	MIPS_R5000 /* NKK R3000 based CPU	ISA I  CLASH */

#define	MIPS_RM5230	0x28	/* QED RM5230 based CPU		ISA IV  */


/*
 * MIPS FPU types
 */
#define	MIPS_SOFT	0x00	/* Software emulation		ISA I   */
#define	MIPS_R2360	0x01	/* MIPS R2360 FPC		ISA I   */
#define	MIPS_R2010	0x02	/* MIPS R2010 FPC		ISA I   */
#define	MIPS_R3010	0x03	/* MIPS R3010 FPC		ISA I   */
#define	MIPS_R6010	0x04	/* MIPS R6010 FPC		ISA II  */
#define	MIPS_R4010	0x05	/* MIPS R4000/R4400 FPC		ISA II  */
#define MIPS_R31LSI	0x06	/* LSI Logic derivate		ISA I	*/
#define	MIPS_R10010	0x09	/* MIPS R10000/T5 FPU		ISA IV  */
#define	MIPS_R4210	0x0a	/* MIPS R4200 FPC (ICE)		ISA III */
#define MIPS_UNKF1	0x0b	/* unnanounced product cpu	ISA III */
#define	MIPS_R8000	0x10	/* MIPS R8000 Blackbird/TFP	ISA IV  */
#define	MIPS_R4600	0x20	/* QED R4600 Orion		ISA III */
#define	MIPS_R3SONY	MIPS_R4700	/* Sony R3000 based FPU		ISA I   */
#define	MIPS_R3TOSH	0x22	/* Toshiba R3000 based FPU	ISA I	*/
/* ID conflict */
#define	MIPS_R3NKK	MIPS_R5000 /* NKK R3000 based CPU	ISA I  CLASH */

#define	MIPS_R5010	0x23	/* MIPS R5000 based FPU		ISA IV  */
#define	MIPS_RM5230	0x28	/* QED RM5230 based FPU		ISA IV  */


/*
 * Enable realtime clock (always enabled).
 */
#define	enablertclock()

#endif /* _CPU_H_ */
