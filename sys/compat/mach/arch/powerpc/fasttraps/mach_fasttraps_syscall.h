/* $NetBSD: mach_fasttraps_syscall.h,v 1.4 2002/12/07 19:05:11 manu Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.2 2002/12/07 15:33:03 manu Exp 
 */

/* syscall: "cthread_set_self" ret: "void" args: "mach_cproc_t" */
#define	MACH_FASTTRAPS_SYS_cthread_set_self	1

/* syscall: "cthread_self" ret: "mach_cproc_t" args: */
#define	MACH_FASTTRAPS_SYS_cthread_self	2

/* syscall: "processor_facilities_used" ret: "int" args: */
#define	MACH_FASTTRAPS_SYS_processor_facilities_used	3

/* syscall: "load_msr" ret: "void" args: */
#define	MACH_FASTTRAPS_SYS_load_msr	4

#define	MACH_FASTTRAPS_SYS_MAXSYSCALL	16
#define	MACH_FASTTRAPS_SYS_NSYSENT	16
