/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 *	from: @(#)vm_meter.c	7.11 (Berkeley) 4/20/91
 *	$Id: vm_meter.c,v 1.10 1994/05/04 03:42:44 cgd Exp $
 */

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/vmmeter.h>

#include <vm/vm_param.h>

struct loadavg averunnable;
#if defined(COMPAT_43) && (defined(vax) || defined(tahoe))
double avenrun[3];
#endif /* COMPAT_43 */

int	maxslp = MAXSLP;

#ifndef MACHINE_NONCONTIG
int	saferss = SAFERSS;
#endif /* MACHINE_NONCONTIG */

static void vmtotal __P((void));
static void loadav __P((fixpt_t *, int));

void
vmmeter()
{
	register unsigned *cp, *rp, *sp;

	if (time.tv_sec % 5 == 0)
		vmtotal();
	if (proc0.p_slptime > maxslp/2)
		wakeup((caddr_t)&proc0);
}

static void
vmtotal()
{
	register struct proc *p;
	int nrun = 0;

	total.t_vm = 0;
	total.t_avm = 0;
	total.t_rm = 0;
	total.t_arm = 0;
	total.t_rq = 0;
	total.t_dw = 0;
	total.t_pw = 0;
	total.t_sl = 0;
	total.t_sw = 0;
	for (p = (struct proc *)allproc; p != NULL; p = p->p_nxt) {
		if (p->p_flag & P_SYSTEM)
			continue;
		if (p->p_stat) {
			switch (p->p_stat) {

			case SSLEEP:
				if (p->p_pri <= PZERO && p->p_slptime == 0)
					nrun++;
				/* fall through */
			case SSTOP:
				if (p->p_flag & P_INMEM) {
					if (p->p_pri <= PZERO)
						total.t_dw++;
					else if (p->p_slptime < maxslp)
						total.t_sl++;
				} else if (p->p_slptime < maxslp)
					total.t_sw++;
				if (p->p_slptime < maxslp)
					goto active;
				break;

			case SRUN:
			case SIDL:
				nrun++;
				if (p->p_flag & P_INMEM)
					total.t_rq++;
				else
					total.t_sw++;
active:
				break;
			}
		}
	}
	/* XXXX */
	loadav(averunnable.ldavg, nrun);
	averunnable.fscale = FSCALE;
}

/*
 * Constants for averages over 1, 5, and 15 minutes
 * when sampling at 5 second intervals.
 */
fixpt_t	cexp[3] = {
	0.9200444146293232 * FSCALE,	/* exp(-1/12) */
	0.9834714538216174 * FSCALE,	/* exp(-1/60) */
	0.9944598480048967 * FSCALE,	/* exp(-1/180) */
};

/*
 * Compute a tenex style load average of a quantity on
 * 1, 5 and 15 minute intervals.
 */
static void
loadav(avg, n)
	register fixpt_t *avg;
	int n;
{
	register int i;

	for (i = 0; i < 3; i++)
		avg[i] = (cexp[i] * avg[i] + n * FSCALE * (FSCALE - cexp[i]))
		         >> FSHIFT;
#if defined(COMPAT_43) && (defined(vax) || defined(tahoe))
	for (i = 0; i < 3; i++)
		avenrun[i] = (double) averunnable.ldavg[i] / FSCALE;
#endif /* COMPAT_43 */
}

/*
 * Get load average (i.e. avenrunnable, via kinfo).
 */
/* ARGSUSED */
kinfo_loadavg(op, where, acopysize, arg, aneeded)
	int op;
	char *where;
	int *acopysize, arg, *aneeded;
{
	int error;

	if (where == NULL || acopysize == NULL) {
		*aneeded = sizeof(struct loadavg);
		return 0;
	}
	if (*acopysize != sizeof(struct loadavg)) {
		*aneeded = sizeof(struct loadavg);
		*acopysize = 0;
		return 0;
	}
	*aneeded = 0;
	return copyout(&averunnable, where, *acopysize);
}
