/*	$NetBSD: kern_exit.c,v 1.215 2008/11/01 05:59:33 wrstuden Exp $	*/

/*-
 * Copyright (c) 1998, 1999, 2006, 2007, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center, and by Andrew Doran.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Copyright (c) 1982, 1986, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
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
 *	@(#)kern_exit.c	8.10 (Berkeley) 2/23/95
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: kern_exit.c,v 1.215 2008/11/01 05:59:33 wrstuden Exp $");

#include "opt_ktrace.h"
#include "opt_perfctrs.h"
#include "opt_sa.h"
#include "opt_sysv.h"

#include <sys/param.h>
#include <sys/aio.h>
#include <sys/systm.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/syslog.h>
#include <sys/malloc.h>
#include <sys/pool.h>
#include <sys/uidinfo.h>
#if defined(PERFCTRS)
#include <sys/pmc.h>
#endif
#include <sys/ptrace.h>
#include <sys/acct.h>
#include <sys/filedesc.h>
#include <sys/ras.h>
#include <sys/signalvar.h>
#include <sys/sched.h>
#include <sys/sa.h>
#include <sys/savar.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <sys/kauth.h>
#include <sys/sleepq.h>
#include <sys/lockdebug.h>
#include <sys/ktrace.h>
#include <sys/cpu.h>
#include <sys/lwpctl.h>
#include <sys/atomic.h>

#include <uvm/uvm_extern.h>

#define DEBUG_EXIT

#ifdef DEBUG_EXIT
int debug_exit = 0;
#define DPRINTF(x) if (debug_exit) printf x
#else
#define DPRINTF(x)
#endif

static int find_stopped_child(struct proc *, pid_t, int, struct proc **, int *);
static void proc_free(struct proc *, struct rusage *);

/*
 * Fill in the appropriate signal information, and signal the parent.
 */
static void
exit_psignal(struct proc *p, struct proc *pp, ksiginfo_t *ksi)
{

	KSI_INIT(ksi);
	if ((ksi->ksi_signo = P_EXITSIG(p)) == SIGCHLD) {
		if (WIFSIGNALED(p->p_xstat)) {
			if (WCOREDUMP(p->p_xstat))
				ksi->ksi_code = CLD_DUMPED;
			else
				ksi->ksi_code = CLD_KILLED;
		} else {
			ksi->ksi_code = CLD_EXITED;
		}
	}
	/*
	 * We fill those in, even for non-SIGCHLD.
	 * It's safe to access p->p_cred unlocked here.
	 */
	ksi->ksi_pid = p->p_pid;
	ksi->ksi_uid = kauth_cred_geteuid(p->p_cred);
	ksi->ksi_status = p->p_xstat;
	/* XXX: is this still valid? */
	ksi->ksi_utime = p->p_stats->p_ru.ru_utime.tv_sec;
	ksi->ksi_stime = p->p_stats->p_ru.ru_stime.tv_sec;
}

/*
 * exit --
 *	Death of process.
 */
int
sys_exit(struct lwp *l, const struct sys_exit_args *uap, register_t *retval)
{
	/* {
		syscallarg(int)	rval;
	} */
	struct proc *p = l->l_proc;

	/* Don't call exit1() multiple times in the same process. */
	mutex_enter(p->p_lock);
	if (p->p_sflag & PS_WEXIT) {
		mutex_exit(p->p_lock);
		lwp_exit(l);
	}

	/* exit1() will release the mutex. */
	exit1(l, W_EXITCODE(SCARG(uap, rval), 0));
	/* NOTREACHED */
	return (0);
}

/*
 * Exit: deallocate address space and other resources, change proc state
 * to zombie, and unlink proc from allproc and parent's lists.  Save exit
 * status and rusage for wait().  Check for child processes and orphan them.
 *
 * Must be called with p->p_lock held.  Does not return.
 */
void
exit1(struct lwp *l, int rv)
{
	struct proc	*p, *q, *nq;
	struct pgrp	*pgrp;
	ksiginfo_t	ksi;
	ksiginfoq_t	kq;
	int		wakeinit, sa;

	p = l->l_proc;

	KASSERT(mutex_owned(p->p_lock));

	if (__predict_false(p == initproc))
		panic("init died (signal %d, exit %d)",
		    WTERMSIG(rv), WEXITSTATUS(rv));

	/*
	 * Disable scheduler activation upcalls.  We're trying to get out of
	 * here.
	 */
	sa = 0;
#ifdef KERN_SA
	if ((p->p_sa != NULL)) {
		l->l_pflag |= LP_SA_NOBLOCK;
		sa = 1;
	}
#endif

	p->p_sflag |= PS_WEXIT;

	/*
	 * Force all other LWPs to exit before we do.  Only then can we
	 * begin to tear down the rest of the process state.
	 */
	if (sa || p->p_nlwps > 1)
		exit_lwps(l);

	ksiginfo_queue_init(&kq);

	/*
	 * If we have been asked to stop on exit, do so now.
	 */
	if (__predict_false(p->p_sflag & PS_STOPEXIT)) {
		KERNEL_UNLOCK_ALL(l, &l->l_biglocks);
		sigclearall(p, &contsigmask, &kq);
		p->p_waited = 0;
		membar_producer();
		p->p_stat = SSTOP;
		lwp_lock(l);
		p->p_nrlwps--;
		l->l_stat = LSSTOP;
		mutex_exit(p->p_lock);
		mi_switch(l);
		KERNEL_LOCK(l->l_biglocks, l);
		mutex_enter(p->p_lock);
	}

	/*
	 * Bin any remaining signals and mark the process as dying so it will
	 * not be found for, e.g. signals. 
	 */
	sigfillset(&p->p_sigctx.ps_sigignore);
	sigclearall(p, NULL, &kq);
	p->p_stat = SDYING;
	mutex_exit(p->p_lock);
	ksiginfo_queue_drain(&kq);

	/* Destroy any lwpctl info. */
	if (p->p_lwpctl != NULL)
		lwp_ctl_exit();

	/* Destroy all AIO works */
	aio_exit(p, p->p_aio);

	/*
	 * Drain all remaining references that procfs, ptrace and others may
	 * have on the process.
	 */
	rw_enter(&p->p_reflock, RW_WRITER);

	DPRINTF(("exit1: %d.%d exiting.\n", p->p_pid, l->l_lid));

	timers_free(p, TIMERS_ALL);
#if defined(__HAVE_RAS)
	ras_purgeall();
#endif

	/*
	 * Close open files, release open-file table and free signal
	 * actions.  This may block!
	 */
	fd_free();
	cwdfree(p->p_cwdi);
	p->p_cwdi = NULL;
	doexithooks(p);
	sigactsfree(p->p_sigacts);

	/*
	 * Write out accounting data.
	 */
	(void)acct_process(l);

#ifdef KTRACE
	/*
	 * Release trace file.
	 */
	if (p->p_tracep != NULL) {
		mutex_enter(&ktrace_lock);
		ktrderef(p);
		mutex_exit(&ktrace_lock);
	}
#endif

	/*
	 * If emulation has process exit hook, call it now.
	 * Set the exit status now so that the exit hook has
	 * an opportunity to tweak it (COMPAT_LINUX requires
	 * this for thread group emulation)
	 */
	p->p_xstat = rv;
	if (p->p_emul->e_proc_exit)
		(*p->p_emul->e_proc_exit)(p);

	/*
	 * Free the VM resources we're still holding on to.
	 * We must do this from a valid thread because doing
	 * so may block. This frees vmspace, which we don't
	 * need anymore. The only remaining lwp is the one
	 * we run at this moment, nothing runs in userland
	 * anymore.
	 */
	uvm_proc_exit(p);

	/*
	 * Stop profiling.
	 */
	if (__predict_false((p->p_stflag & PST_PROFIL) != 0)) {
		mutex_spin_enter(&p->p_stmutex);
		stopprofclock(p);
		mutex_spin_exit(&p->p_stmutex);
	}

	/*
	 * If parent is waiting for us to exit or exec, PL_PPWAIT is set; we
	 * wake up the parent early to avoid deadlock.  We can do this once
	 * the VM resources are released.
	 */
	mutex_enter(proc_lock);
	if (p->p_lflag & PL_PPWAIT) {
		p->p_lflag &= ~PL_PPWAIT;
		cv_broadcast(&p->p_pptr->p_waitcv);
	}

	if (SESS_LEADER(p)) {
		struct vnode *vprele = NULL, *vprevoke = NULL;
		struct session *sp = p->p_session;
		struct tty *tp;

		if (sp->s_ttyvp) {
			/*
			 * Controlling process.
			 * Signal foreground pgrp,
			 * drain controlling terminal
			 * and revoke access to controlling terminal.
			 */
			tp = sp->s_ttyp;
			mutex_spin_enter(&tty_lock);
			if (tp->t_session == sp) {
				/* we can't guarantee the revoke will do this */
				pgrp = tp->t_pgrp;
				tp->t_pgrp = NULL;
				tp->t_session = NULL;
				mutex_spin_exit(&tty_lock);
				if (pgrp != NULL) {
					pgsignal(pgrp, SIGHUP, 1);
				}
				mutex_exit(proc_lock);
				(void) ttywait(tp);
				mutex_enter(proc_lock);

				/* The tty could have been revoked. */
				vprevoke = sp->s_ttyvp;
			} else
				mutex_spin_exit(&tty_lock);
			vprele = sp->s_ttyvp;
			sp->s_ttyvp = NULL;
			/*
			 * s_ttyp is not zero'd; we use this to indicate
			 * that the session once had a controlling terminal.
			 * (for logging and informational purposes)
			 */
		}
		sp->s_leader = NULL;

		if (vprevoke != NULL || vprele != NULL) {
			if (vprevoke != NULL) {
				SESSRELE(sp);
				mutex_exit(proc_lock);
				VOP_REVOKE(vprevoke, REVOKEALL);
			} else
				mutex_exit(proc_lock);
			if (vprele != NULL)
				vrele(vprele);
			mutex_enter(proc_lock);
		}
	}
	fixjobc(p, p->p_pgrp, 0);

	/*
	 * Finalize the last LWP's specificdata, as well as the
	 * specificdata for the proc itself.
	 */
	lwp_finispecific(l);
	proc_finispecific(p);

	/*
	 * Notify interested parties of our demise.
	 */
	KNOTE(&p->p_klist, NOTE_EXIT);

#if PERFCTRS
	/*
	 * Save final PMC information in parent process & clean up.
	 */
	if (PMC_ENABLED(p)) {
		pmc_save_context(p);
		pmc_accumulate(p->p_pptr, p);
		pmc_process_exit(p);
	}
#endif

	/*
	 * Reset p_opptr pointer of all former children which got
	 * traced by another process and were reparented. We reset
	 * it to NULL here; the trace detach code then reparents
	 * the child to initproc. We only check allproc list, since
	 * eventual former children on zombproc list won't reference
	 * p_opptr anymore.
	 */
	if (__predict_false(p->p_slflag & PSL_CHTRACED)) {
		PROCLIST_FOREACH(q, &allproc) {
			if ((q->p_flag & PK_MARKER) != 0)
				continue;
			if (q->p_opptr == p)
				q->p_opptr = NULL;
		}
	}

	/*
	 * Give orphaned children to init(8).
	 */
	q = LIST_FIRST(&p->p_children);
	wakeinit = (q != NULL);
	for (; q != NULL; q = nq) {
		nq = LIST_NEXT(q, p_sibling);

		/*
		 * Traced processes are killed since their existence
		 * means someone is screwing up. Since we reset the
		 * trace flags, the logic in sys_wait4() would not be
		 * triggered to reparent the process to its
		 * original parent, so we must do this here.
		 */
		if (__predict_false(q->p_slflag & PSL_TRACED)) {
			mutex_enter(p->p_lock);
			q->p_slflag &= ~(PSL_TRACED|PSL_FSTRACE|PSL_SYSCALL);
			mutex_exit(p->p_lock);
			if (q->p_opptr != q->p_pptr) {
				struct proc *t = q->p_opptr;
				proc_reparent(q, t ? t : initproc);
				q->p_opptr = NULL;
			} else
				proc_reparent(q, initproc);
			killproc(q, "orphaned traced process");
		} else
			proc_reparent(q, initproc);
	}

	/*
	 * Move proc from allproc to zombproc, it's now nearly ready to be
	 * collected by parent.
	 */
	LIST_REMOVE(l, l_list);
	LIST_REMOVE(p, p_list);
	LIST_INSERT_HEAD(&zombproc, p, p_list);

	/*
	 * Mark the process as dead.  We must do this before we signal
	 * the parent.
	 */
	p->p_stat = SDEAD;

	/* Put in front of parent's sibling list for parent to collect it */
	q = p->p_pptr;
	q->p_nstopchild++;
	if (LIST_FIRST(&q->p_children) != p) {
		/* Put child where it can be found quickly */
		LIST_REMOVE(p, p_sibling);
		LIST_INSERT_HEAD(&q->p_children, p, p_sibling);
	}

	/*
	 * Notify parent that we're gone.  If parent has the P_NOCLDWAIT
	 * flag set, notify init instead (and hope it will handle
	 * this situation).
	 */
	if (q->p_flag & (PK_NOCLDWAIT|PK_CLDSIGIGN)) {
		proc_reparent(p, initproc);
		wakeinit = 1;

		/*
		 * If this was the last child of our parent, notify
		 * parent, so in case he was wait(2)ing, he will
		 * continue.
		 */
		if (LIST_FIRST(&q->p_children) == NULL)
			cv_broadcast(&q->p_waitcv);
	}

	/* Reload parent pointer, since p may have been reparented above */
	q = p->p_pptr;

	if (__predict_false((p->p_slflag & PSL_FSTRACE) == 0 && p->p_exitsig != 0)) {
		exit_psignal(p, q, &ksi);
		kpsignal(q, &ksi, NULL);
	}

	/* Calculate the final rusage info.  */
	calcru(p, &p->p_stats->p_ru.ru_utime, &p->p_stats->p_ru.ru_stime,
	    NULL, NULL);

	if (wakeinit)
		cv_broadcast(&initproc->p_waitcv);

	callout_destroy(&l->l_timeout_ch);

	/*
	 * Remaining lwp resources will be freed in lwp_exit2() once we've
	 * switch to idle context; at that point, we will be marked as a
	 * full blown zombie.
	 */
	mutex_enter(p->p_lock);
	lwp_drainrefs(l);
	lwp_lock(l);
	l->l_prflag &= ~LPR_DETACHED;
	l->l_stat = LSZOMB;
	lwp_unlock(l);
	KASSERT(curlwp == l);
	KASSERT(p->p_nrlwps == 1);
	KASSERT(p->p_nlwps == 1);
	p->p_stat = SZOMB;
	p->p_nrlwps--;
	p->p_nzlwps++;
	p->p_ndlwps = 0;
	mutex_exit(p->p_lock);

	/*
	 * Signal the parent to collect us, and drop the proclist lock.
	 * Drop debugger/procfs lock; no new references can be gained.
	 */
	cv_broadcast(&p->p_pptr->p_waitcv);
	mutex_exit(proc_lock);
	rw_exit(&p->p_reflock);

	/* Verify that we hold no locks other than the kernel lock. */
	LOCKDEBUG_BARRIER(&kernel_lock, 0);

	/*
	 * NOTE: WE ARE NO LONGER ALLOWED TO SLEEP!
	 */

	/*
	 * Give machine-dependent code a chance to free any MD LWP
	 * resources.  This must be done before uvm_lwp_exit(), in
	 * case these resources are in the PCB.
	 */
#ifndef __NO_CPU_LWP_FREE
	cpu_lwp_free(l, 1);
#endif
	pmap_deactivate(l);

	/* This process no longer needs to hold the kernel lock. */
#ifdef notyet
	/* XXXSMP hold in lwp_userret() */
	KERNEL_UNLOCK_LAST(l);
#else
	KERNEL_UNLOCK_ALL(l, NULL);
#endif

	lwp_exit_switchaway(l);
}

void
exit_lwps(struct lwp *l)
{
	struct proc *p;
	struct lwp *l2;
	int error;
	lwpid_t waited;
	int nlocks;

	KERNEL_UNLOCK_ALL(l, &nlocks);

	p = l->l_proc;
	KASSERT(mutex_owned(p->p_lock));

#ifdef KERN_SA
	if (p->p_sa != NULL) {
		struct sadata_vp *vp;
		SLIST_FOREACH(vp, &p->p_sa->sa_vps, savp_next) {
			/*
			 * Make SA-cached LWPs normal process interruptable
			 * so that the exit code can wake them. Locking
			 * savp_mutex locks all the lwps on this vp that
			 * we need to adjust.
			 */
			mutex_enter(&vp->savp_mutex);
			DPRINTF(("exit_lwps: Making cached LWPs of %d on "
			    "VP %d interruptable: ", p->p_pid, vp->savp_id));
			TAILQ_FOREACH(l2, &vp->savp_lwpcache, l_sleepchain) {
				l2->l_flag |= LW_SINTR;
				DPRINTF(("%d ", l2->l_lid));
			}
			DPRINTF(("\n"));

			DPRINTF(("exit_lwps: Making unblocking LWPs of %d on "
			    "VP %d interruptable: ", p->p_pid, vp->savp_id));
			TAILQ_FOREACH(l2, &vp->savp_woken, l_sleepchain) {
				vp->savp_woken_count--;
				l2->l_flag |= LW_SINTR;
				DPRINTF(("%d ", l2->l_lid));
			}
			DPRINTF(("\n"));
			mutex_exit(&vp->savp_mutex);
		}
	}
#endif

 retry:
	/*
	 * Interrupt LWPs in interruptable sleep, unsuspend suspended
	 * LWPs and then wait for everyone else to finish.
	 */
	LIST_FOREACH(l2, &p->p_lwps, l_sibling) {
		if (l2 == l)
			continue;
		lwp_lock(l2);
		l2->l_flag &= ~LW_SA;
		l2->l_flag |= LW_WEXIT;
		if ((l2->l_stat == LSSLEEP && (l2->l_flag & LW_SINTR)) ||
		    l2->l_stat == LSSUSPENDED || l2->l_stat == LSSTOP) {
		    	/* setrunnable() will release the lock. */
			setrunnable(l2);
			DPRINTF(("exit_lwps: Made %d.%d runnable\n",
			    p->p_pid, l2->l_lid));
			continue;
		}
		lwp_unlock(l2);
	}
	while (p->p_nlwps > 1) {
		DPRINTF(("exit_lwps: waiting for %d LWPs (%d zombies)\n",
		    p->p_nlwps, p->p_nzlwps));
		error = lwp_wait1(l, 0, &waited, LWPWAIT_EXITCONTROL);
		if (p->p_nlwps == 1)
			break;
		if (error == EDEADLK) {
			/*
			 * LWPs can get suspended/slept behind us.
			 * (eg. sa_setwoken)
			 * kick them again and retry.
			 */
			goto retry;
		}
		if (error)
			panic("exit_lwps: lwp_wait1 failed with error %d",
			    error);
		DPRINTF(("exit_lwps: Got LWP %d from lwp_wait1()\n", waited));
	}

	KERNEL_LOCK(nlocks, l);
	KASSERT(p->p_nlwps == 1);
}

int
do_sys_wait(struct lwp *l, int *pid, int *status, int options,
    struct rusage *ru, int *was_zombie)
{
	struct proc	*child;
	int		error;

	mutex_enter(proc_lock);
	error = find_stopped_child(l->l_proc, *pid, options, &child, status);

	if (child == NULL) {
		mutex_exit(proc_lock);
		*pid = 0;
		return error;
	}

	*pid = child->p_pid;

	if (child->p_stat == SZOMB) {
		/* proc_free() will release the proc_lock. */
		*was_zombie = 1;
		if (options & WNOWAIT)
			mutex_exit(proc_lock);
		else {
			proc_free(child, ru);
		}
	} else {
		/* Child state must have been SSTOP. */
		*was_zombie = 0;
		mutex_exit(proc_lock);
		*status = W_STOPCODE(*status);
	}

	return 0;
}

int
sys_wait4(struct lwp *l, const struct sys_wait4_args *uap, register_t *retval)
{
	/* {
		syscallarg(int)			pid;
		syscallarg(int *)		status;
		syscallarg(int)			options;
		syscallarg(struct rusage *)	rusage;
	} */
	int		status, error;
	int		was_zombie;
	struct rusage	ru;
	int pid = SCARG(uap, pid);

	error = do_sys_wait(l, &pid, &status, SCARG(uap, options),
	    SCARG(uap, rusage) != NULL ? &ru : NULL, &was_zombie);

	retval[0] = pid;
	if (pid == 0)
		return error;

	if (SCARG(uap, rusage))
		error = copyout(&ru, SCARG(uap, rusage), sizeof(ru));

	if (error == 0 && SCARG(uap, status))
		error = copyout(&status, SCARG(uap, status), sizeof(status));

	return error;
}

/*
 * Scan list of child processes for a child process that has stopped or
 * exited.  Used by sys_wait4 and 'compat' equivalents.
 *
 * Must be called with the proc_lock held, and may release while waiting.
 */
static int
find_stopped_child(struct proc *parent, pid_t pid, int options,
		   struct proc **child_p, int *status_p)
{
	struct proc *child, *dead;
	int error;

	KASSERT(mutex_owned(proc_lock));

	if (options & ~(WUNTRACED|WNOHANG|WALTSIG|WALLSIG)
	    && !(options & WOPTSCHECKED)) {
		*child_p = NULL;
		return EINVAL;
	}

	if (pid == 0 && !(options & WOPTSCHECKED))
		pid = -parent->p_pgid;

	for (;;) {
		error = ECHILD;
		dead = NULL;

		LIST_FOREACH(child, &parent->p_children, p_sibling) {
			if (pid >= 0) {
				if (child->p_pid != pid) {
					child = p_find(pid, PFIND_ZOMBIE |
					    PFIND_LOCKED);
					if (child == NULL ||
					    child->p_pptr != parent) {
						child = NULL;
						break;
					}
				}
			} else if (pid != WAIT_ANY && child->p_pgid != -pid) {
				/* Child not in correct pgrp */
				continue;
			}

			/*
			 * Wait for processes with p_exitsig != SIGCHLD
			 * processes only if WALTSIG is set; wait for
			 * processes with p_exitsig == SIGCHLD only
			 * if WALTSIG is clear.
			 */
			if (((options & WALLSIG) == 0) &&
			    (options & WALTSIG ? child->p_exitsig == SIGCHLD
						: P_EXITSIG(child) != SIGCHLD)){
				if (child->p_pid == pid) {
					child = NULL;
					break;
				}
				continue;
			}

			error = 0;
			if ((options & WNOZOMBIE) == 0) {
				if (child->p_stat == SZOMB)
					break;
				if (child->p_stat == SDEAD) {
					/*
					 * We may occasionally arrive here
					 * after receiving a signal, but
					 * immediatley before the child
					 * process is zombified.  The wait
					 * will be short, so avoid returning
					 * to userspace.
					 */
					dead = child;
				}
			}

			if (child->p_stat == SSTOP &&
			    child->p_waited == 0 &&
			    (child->p_slflag & PSL_TRACED ||
			    options & WUNTRACED)) {
				if ((options & WNOWAIT) == 0) {
					child->p_waited = 1;
					parent->p_nstopchild--;
				}
				break;
			}
			if (parent->p_nstopchild == 0 || child->p_pid == pid) {
				child = NULL;
				break;
			}
		}

		if (child != NULL || error != 0 ||
		    ((options & WNOHANG) != 0 && dead == NULL)) {
		    	if (child != NULL) {
			    	*status_p = child->p_xstat;
			}
			*child_p = child;
			return error;
		}

		/*
		 * Wait for another child process to stop.
		 */
		error = cv_wait_sig(&parent->p_waitcv, proc_lock);

		if (error != 0) {
			*child_p = NULL;
			return error;
		}
	}
}

/*
 * Free a process after parent has taken all the state info.  Must be called
 * with the proclist lock held, and will release before returning.
 *
 * *ru is returned to the caller, and must be freed by the caller.
 */
static void
proc_free(struct proc *p, struct rusage *ru)
{
	struct proc *parent;
	struct lwp *l;
	ksiginfo_t ksi;
	kauth_cred_t cred1, cred2;
	uid_t uid;

	KASSERT(mutex_owned(proc_lock));
	KASSERT(p->p_nlwps == 1);
	KASSERT(p->p_nzlwps == 1);
	KASSERT(p->p_nrlwps == 0);
	KASSERT(p->p_stat == SZOMB);

	/*
	 * If we got the child via ptrace(2) or procfs, and
	 * the parent is different (meaning the process was
	 * attached, rather than run as a child), then we need
	 * to give it back to the old parent, and send the
	 * parent the exit signal.  The rest of the cleanup
	 * will be done when the old parent waits on the child.
	 */
	if ((p->p_slflag & PSL_TRACED) != 0) {
		parent = p->p_pptr;
		if (p->p_opptr != parent){
			mutex_enter(p->p_lock);
			p->p_slflag &= ~(PSL_TRACED|PSL_FSTRACE|PSL_SYSCALL);
			mutex_exit(p->p_lock);
			parent = p->p_opptr;
			if (parent == NULL)
				parent = initproc;
			proc_reparent(p, parent);
			p->p_opptr = NULL;
			if (p->p_exitsig != 0) {
				exit_psignal(p, parent, &ksi);
				kpsignal(parent, &ksi, NULL);
			}
			cv_broadcast(&parent->p_waitcv);
			mutex_exit(proc_lock);
			return;
		}
	}

	/*
	 * Finally finished with old proc entry.  Unlink it from its process
	 * group.
	 */
	leavepgrp(p);

	parent = p->p_pptr;
	sched_proc_exit(parent, p);

	/*
	 * Add child times of exiting process onto its own times.
	 * This cannot be done any earlier else it might get done twice.
	 */
	l = LIST_FIRST(&p->p_lwps);
	p->p_stats->p_ru.ru_nvcsw += (l->l_ncsw - l->l_nivcsw);
	p->p_stats->p_ru.ru_nivcsw += l->l_nivcsw;
	ruadd(&p->p_stats->p_ru, &l->l_ru);
	ruadd(&p->p_stats->p_ru, &p->p_stats->p_cru);
	ruadd(&parent->p_stats->p_cru, &p->p_stats->p_ru);
	if (ru != NULL)
		*ru = p->p_stats->p_ru;
	p->p_xstat = 0;

	/* Release any SA state. */
#ifdef KERN_SA
	if (p->p_sa)
		sa_release(p);
#endif

	/*
	 * At this point we are going to start freeing the final resources. 
	 * If anyone tries to access the proc structure after here they will
	 * get a shock - bits are missing.  Attempt to make it hard!  We
	 * don't bother with any further locking past this point.
	 */
	p->p_stat = SIDL;		/* not even a zombie any more */
	LIST_REMOVE(p, p_list);	/* off zombproc */
	parent = p->p_pptr;
	p->p_pptr->p_nstopchild--;
	LIST_REMOVE(p, p_sibling);

	/*
	 * Let pid be reallocated.
	 */
	proc_free_pid(p);
	mutex_exit(proc_lock);

	/*
	 * Delay release until after lwp_free.
	 */
	cred2 = l->l_cred;

	/*
	 * Free the last LWP's resources.
	 *
	 * lwp_free ensures the LWP is no longer running on another CPU.
	 */
	lwp_free(l, false, true);

	/*
	 * Now no one except us can reach the process p.
	 */

	/*
	 * Decrement the count of procs running with this uid.
	 */
	cred1 = p->p_cred;
	uid = kauth_cred_getuid(cred1);
	(void)chgproccnt(uid, -1);

	/*
	 * Release substructures.
	 */

	limfree(p->p_limit);
	pstatsfree(p->p_stats);
	kauth_cred_free(cred1);
	kauth_cred_free(cred2);

	/*
	 * Release reference to text vnode
	 */
	if (p->p_textvp)
		vrele(p->p_textvp);

	mutex_destroy(&p->p_auxlock);
	mutex_obj_free(p->p_lock);
	mutex_destroy(&p->p_stmutex);
	cv_destroy(&p->p_waitcv);
	cv_destroy(&p->p_lwpcv);
	rw_destroy(&p->p_reflock);

	proc_free_mem(p);
}

/*
 * make process 'parent' the new parent of process 'child'.
 *
 * Must be called with proc_lock held.
 */
void
proc_reparent(struct proc *child, struct proc *parent)
{

	KASSERT(mutex_owned(proc_lock));

	if (child->p_pptr == parent)
		return;

	if (child->p_stat == SZOMB ||
	    (child->p_stat == SSTOP && !child->p_waited)) {
		child->p_pptr->p_nstopchild--;
		parent->p_nstopchild++;
	}
	if (parent == initproc)
		child->p_exitsig = SIGCHLD;

	LIST_REMOVE(child, p_sibling);
	LIST_INSERT_HEAD(&parent->p_children, child, p_sibling);
	child->p_pptr = parent;
	child->p_ppid = parent->p_pid;
}
