/* $NetBSD: mach_fasttraps_syscallargs.h,v 1.4 2002/12/07 19:05:11 manu Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.2 2002/12/07 15:33:03 manu Exp 
 */

#ifndef _MACH_FASTTRAPS_SYS__SYSCALLARGS_H_
#define	_MACH_FASTTRAPS_SYS__SYSCALLARGS_H_

#ifdef	syscallarg
#undef	syscallarg
#endif

#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

struct mach_sys_cthread_set_self_args {
	syscallarg(mach_cproc_t) p;
};

/*
 * System call prototypes.
 */

int	mach_sys_cthread_set_self(struct proc *, void *, register_t *);
int	mach_sys_cthread_self(struct proc *, void *, register_t *);
int	mach_sys_processor_facilities_used(struct proc *, void *, register_t *);
int	mach_sys_load_msr(struct proc *, void *, register_t *);
#endif /* _MACH_FASTTRAPS_SYS__SYSCALLARGS_H_ */
