/* $NetBSD: netbsd32_sysent.c,v 1.25 2002/04/29 09:55:13 mrg Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.21 2002/04/29 09:53:41 mrg Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: netbsd32_sysent.c,v 1.25 2002/04/29 09:55:13 mrg Exp $");

#if defined(_KERNEL_OPT)
#include "opt_ktrace.h"
#include "opt_nfsserver.h"
#include "opt_compat_netbsd.h"
#include "opt_ntp.h"
#include "opt_sysv.h"
#include "opt_compat_43.h"
#include "fs_lfs.h"
#include "fs_nfs.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>

#ifdef COMPAT_43
#define	compat_43(func) __CONCAT(compat_43_,func)
#else
#define	compat_43(func) sys_nosys
#endif

#ifdef COMPAT_09
#define	compat_09(func) __CONCAT(compat_09_,func)
#else
#define	compat_09(func) sys_nosys
#endif

#ifdef COMPAT_10
#define	compat_10(func) __CONCAT(compat_10_,func)
#else
#define	compat_10(func) sys_nosys
#endif

#ifdef COMPAT_11
#define	compat_11(func) __CONCAT(compat_11_,func)
#else
#define	compat_11(func) sys_nosys
#endif

#ifdef COMPAT_12
#define	compat_12(func) __CONCAT(compat_12_,func)
#else
#define	compat_12(func) sys_nosys
#endif

#ifdef COMPAT_13
#define	compat_13(func) __CONCAT(compat_13_,func)
#else
#define	compat_13(func) sys_nosys
#endif

#ifdef COMPAT_14
#define	compat_14(func) __CONCAT(compat_14_,func)
#else
#define	compat_14(func) sys_nosys
#endif

#define	s(type)	sizeof(type)

struct sysent netbsd32_sysent[] = {
	{ 0, 0, 0,
	    sys_nosys },			/* 0 = syscall (indir) */
	{ 1, s(struct netbsd32_exit_args), 0,
	    netbsd32_exit },			/* 1 = netbsd32_exit */
	{ 0, 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct netbsd32_read_args), 0,
	    netbsd32_read },			/* 3 = netbsd32_read */
	{ 3, s(struct netbsd32_write_args), 0,
	    netbsd32_write },			/* 4 = netbsd32_write */
	{ 3, s(struct netbsd32_open_args), 0,
	    netbsd32_open },			/* 5 = netbsd32_open */
	{ 1, s(struct netbsd32_close_args), 0,
	    netbsd32_close },			/* 6 = netbsd32_close */
	{ 4, s(struct netbsd32_wait4_args), 0,
	    netbsd32_wait4 },			/* 7 = netbsd32_wait4 */
	{ 2, s(struct compat_43_netbsd32_ocreat_args), 0,
	    compat_43(netbsd32_ocreat) },	/* 8 = compat_43 netbsd32_ocreat */
	{ 2, s(struct netbsd32_link_args), 0,
	    netbsd32_link },			/* 9 = netbsd32_link */
	{ 1, s(struct netbsd32_unlink_args), 0,
	    netbsd32_unlink },			/* 10 = netbsd32_unlink */
	{ 0, 0, 0,
	    sys_nosys },			/* 11 = obsolete execv */
	{ 1, s(struct netbsd32_chdir_args), 0,
	    netbsd32_chdir },			/* 12 = netbsd32_chdir */
	{ 1, s(struct netbsd32_fchdir_args), 0,
	    netbsd32_fchdir },			/* 13 = netbsd32_fchdir */
	{ 3, s(struct netbsd32_mknod_args), 0,
	    netbsd32_mknod },			/* 14 = netbsd32_mknod */
	{ 2, s(struct netbsd32_chmod_args), 0,
	    netbsd32_chmod },			/* 15 = netbsd32_chmod */
	{ 3, s(struct netbsd32_chown_args), 0,
	    netbsd32_chown },			/* 16 = netbsd32_chown */
	{ 1, s(struct netbsd32_break_args), 0,
	    netbsd32_break },			/* 17 = netbsd32_break */
	{ 3, s(struct netbsd32_getfsstat_args), 0,
	    netbsd32_getfsstat },		/* 18 = netbsd32_getfsstat */
	{ 3, s(struct compat_43_netbsd32_olseek_args), 0,
	    compat_43(netbsd32_olseek) },	/* 19 = compat_43 netbsd32_olseek */
	{ 0, 0, SYCALL_MPSAFE | 0,
	    sys_getpid },			/* 20 = getpid */
	{ 4, s(struct netbsd32_mount_args), 0,
	    netbsd32_mount },			/* 21 = netbsd32_mount */
	{ 2, s(struct netbsd32_unmount_args), 0,
	    netbsd32_unmount },			/* 22 = netbsd32_unmount */
	{ 1, s(struct netbsd32_setuid_args), 0,
	    netbsd32_setuid },			/* 23 = netbsd32_setuid */
	{ 0, 0, 0,
	    sys_getuid },			/* 24 = getuid */
	{ 0, 0, 0,
	    sys_geteuid },			/* 25 = geteuid */
	{ 4, s(struct netbsd32_ptrace_args), 0,
	    netbsd32_ptrace },			/* 26 = netbsd32_ptrace */
	{ 3, s(struct netbsd32_recvmsg_args), 0,
	    netbsd32_recvmsg },			/* 27 = netbsd32_recvmsg */
	{ 3, s(struct netbsd32_sendmsg_args), 0,
	    netbsd32_sendmsg },			/* 28 = netbsd32_sendmsg */
	{ 6, s(struct netbsd32_recvfrom_args), 0,
	    netbsd32_recvfrom },		/* 29 = netbsd32_recvfrom */
	{ 3, s(struct netbsd32_accept_args), 0,
	    netbsd32_accept },			/* 30 = netbsd32_accept */
	{ 3, s(struct netbsd32_getpeername_args), 0,
	    netbsd32_getpeername },		/* 31 = netbsd32_getpeername */
	{ 3, s(struct netbsd32_getsockname_args), 0,
	    netbsd32_getsockname },		/* 32 = netbsd32_getsockname */
	{ 2, s(struct netbsd32_access_args), 0,
	    netbsd32_access },			/* 33 = netbsd32_access */
	{ 2, s(struct netbsd32_chflags_args), 0,
	    netbsd32_chflags },			/* 34 = netbsd32_chflags */
	{ 2, s(struct netbsd32_fchflags_args), 0,
	    netbsd32_fchflags },		/* 35 = netbsd32_fchflags */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct netbsd32_kill_args), 0,
	    netbsd32_kill },			/* 37 = netbsd32_kill */
	{ 2, s(struct compat_43_netbsd32_stat43_args), 0,
	    compat_43(netbsd32_stat43) },	/* 38 = compat_43 netbsd32_stat43 */
	{ 0, 0, 0,
	    sys_getppid },			/* 39 = getppid */
	{ 2, s(struct compat_43_netbsd32_lstat43_args), 0,
	    compat_43(netbsd32_lstat43) },	/* 40 = compat_43 netbsd32_lstat43 */
	{ 1, s(struct netbsd32_dup_args), 0,
	    netbsd32_dup },			/* 41 = netbsd32_dup */
	{ 0, 0, 0,
	    sys_pipe },				/* 42 = pipe */
	{ 0, 0, 0,
	    sys_getegid },			/* 43 = getegid */
	{ 4, s(struct netbsd32_profil_args), 0,
	    netbsd32_profil },			/* 44 = netbsd32_profil */
#if defined(KTRACE) || !defined(_KERNEL)
	{ 4, s(struct netbsd32_ktrace_args), 0,
	    netbsd32_ktrace },			/* 45 = netbsd32_ktrace */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 45 = excluded netbsd32_ktrace */
#endif
	{ 3, s(struct netbsd32_sigaction_args), 0,
	    netbsd32_sigaction },		/* 46 = netbsd32_sigaction */
	{ 0, 0, 0,
	    sys_getgid },			/* 47 = getgid */
	{ 2, s(struct compat_13_netbsd32_sigprocmask_args), 0,
	    compat_13(netbsd32_sigprocmask) },	/* 48 = compat_13 sigprocmask13 */
	{ 2, s(struct netbsd32___getlogin_args), 0,
	    netbsd32___getlogin },		/* 49 = netbsd32___getlogin */
	{ 1, s(struct netbsd32_setlogin_args), 0,
	    netbsd32_setlogin },		/* 50 = netbsd32_setlogin */
	{ 1, s(struct netbsd32_acct_args), 0,
	    netbsd32_acct },			/* 51 = netbsd32_acct */
	{ 0, 0, 0,
	    compat_13(sys_sigpending) },	/* 52 = compat_13 sigpending13 */
	{ 2, s(struct compat_13_netbsd32_sigaltstack13_args), 0,
	    compat_13(netbsd32_sigaltstack13) },/* 53 = compat_13 netbsd32_sigaltstack13 */
	{ 3, s(struct netbsd32_ioctl_args), 0,
	    netbsd32_ioctl },			/* 54 = netbsd32_ioctl */
	{ 1, s(struct compat_12_netbsd32_reboot_args), 0,
	    compat_12(netbsd32_reboot) },	/* 55 = compat_12 netbsd32_reboot */
	{ 1, s(struct netbsd32_revoke_args), 0,
	    netbsd32_revoke },			/* 56 = netbsd32_revoke */
	{ 2, s(struct netbsd32_symlink_args), 0,
	    netbsd32_symlink },			/* 57 = netbsd32_symlink */
	{ 3, s(struct netbsd32_readlink_args), 0,
	    netbsd32_readlink },		/* 58 = netbsd32_readlink */
	{ 3, s(struct netbsd32_execve_args), 0,
	    netbsd32_execve },			/* 59 = netbsd32_execve */
	{ 1, s(struct netbsd32_umask_args), 0,
	    netbsd32_umask },			/* 60 = netbsd32_umask */
	{ 1, s(struct netbsd32_chroot_args), 0,
	    netbsd32_chroot },			/* 61 = netbsd32_chroot */
	{ 2, s(struct compat_43_netbsd32_fstat43_args), 0,
	    compat_43(netbsd32_fstat43) },	/* 62 = compat_43 netbsd32_fstat43 */
	{ 4, s(struct compat_43_netbsd32_ogetkerninfo_args), 0,
	    compat_43(netbsd32_ogetkerninfo) },	/* 63 = compat_43 netbsd32_ogetkerninfo */
	{ 0, 0, 0,
	    compat_43(sys_getpagesize) },	/* 64 = compat_43 ogetpagesize */
	{ 2, s(struct compat_12_netbsd32_msync_args), 0,
	    compat_12(netbsd32_msync) },	/* 65 = compat_12 netbsd32_msync */
	{ 0, 0, 0,
	    sys_vfork },			/* 66 = vfork */
	{ 0, 0, 0,
	    sys_nosys },			/* 67 = obsolete vread */
	{ 0, 0, 0,
	    sys_nosys },			/* 68 = obsolete vwrite */
	{ 1, s(struct netbsd32_sbrk_args), 0,
	    netbsd32_sbrk },			/* 69 = netbsd32_sbrk */
	{ 1, s(struct netbsd32_sstk_args), 0,
	    netbsd32_sstk },			/* 70 = netbsd32_sstk */
	{ 6, s(struct compat_43_netbsd32_ommap_args), 0,
	    compat_43(netbsd32_ommap) },	/* 71 = compat_43 netbsd32_ommap */
	{ 1, s(struct netbsd32_ovadvise_args), 0,
	    netbsd32_ovadvise },		/* 72 = vadvise */
	{ 2, s(struct netbsd32_munmap_args), 0,
	    netbsd32_munmap },			/* 73 = netbsd32_munmap */
	{ 3, s(struct netbsd32_mprotect_args), 0,
	    netbsd32_mprotect },		/* 74 = netbsd32_mprotect */
	{ 3, s(struct netbsd32_madvise_args), 0,
	    netbsd32_madvise },			/* 75 = netbsd32_madvise */
	{ 0, 0, 0,
	    sys_nosys },			/* 76 = obsolete vhangup */
	{ 0, 0, 0,
	    sys_nosys },			/* 77 = obsolete vlimit */
	{ 3, s(struct netbsd32_mincore_args), 0,
	    netbsd32_mincore },			/* 78 = netbsd32_mincore */
	{ 2, s(struct netbsd32_getgroups_args), 0,
	    netbsd32_getgroups },		/* 79 = netbsd32_getgroups */
	{ 2, s(struct netbsd32_setgroups_args), 0,
	    netbsd32_setgroups },		/* 80 = netbsd32_setgroups */
	{ 0, 0, 0,
	    sys_getpgrp },			/* 81 = getpgrp */
	{ 2, s(struct netbsd32_setpgid_args), 0,
	    netbsd32_setpgid },			/* 82 = netbsd32_setpgid */
	{ 3, s(struct netbsd32_setitimer_args), 0,
	    netbsd32_setitimer },		/* 83 = netbsd32_setitimer */
	{ 0, 0, 0,
	    compat_43(sys_wait) },		/* 84 = compat_43 owait */
	{ 1, s(struct compat_12_netbsd32_oswapon_args), 0,
	    compat_12(netbsd32_oswapon) },	/* 85 = compat_12 netbsd32_oswapon */
	{ 2, s(struct netbsd32_getitimer_args), 0,
	    netbsd32_getitimer },		/* 86 = netbsd32_getitimer */
	{ 2, s(struct compat_43_netbsd32_ogethostname_args), 0,
	    compat_43(netbsd32_ogethostname) },	/* 87 = compat_43 netbsd32_ogethostname */
	{ 2, s(struct compat_43_netbsd32_osethostname_args), 0,
	    compat_43(netbsd32_osethostname) },	/* 88 = compat_43 netbsd32_osethostname */
	{ 0, 0, 0,
	    compat_43(sys_getdtablesize) },	/* 89 = compat_43 ogetdtablesize */
	{ 2, s(struct netbsd32_dup2_args), 0,
	    netbsd32_dup2 },			/* 90 = netbsd32_dup2 */
	{ 0, 0, 0,
	    sys_nosys },			/* 91 = unimplemented getdopt */
	{ 3, s(struct netbsd32_fcntl_args), 0,
	    netbsd32_fcntl },			/* 92 = netbsd32_fcntl */
	{ 5, s(struct netbsd32_select_args), 0,
	    netbsd32_select },			/* 93 = netbsd32_select */
	{ 0, 0, 0,
	    sys_nosys },			/* 94 = unimplemented setdopt */
	{ 1, s(struct netbsd32_fsync_args), 0,
	    netbsd32_fsync },			/* 95 = netbsd32_fsync */
	{ 3, s(struct netbsd32_setpriority_args), 0,
	    netbsd32_setpriority },		/* 96 = netbsd32_setpriority */
	{ 3, s(struct netbsd32_socket_args), 0,
	    netbsd32_socket },			/* 97 = netbsd32_socket */
	{ 3, s(struct netbsd32_connect_args), 0,
	    netbsd32_connect },			/* 98 = netbsd32_connect */
	{ 3, s(struct compat_43_netbsd32_oaccept_args), 0,
	    compat_43(netbsd32_oaccept) },	/* 99 = compat_43 netbsd32_oaccept */
	{ 2, s(struct netbsd32_getpriority_args), 0,
	    netbsd32_getpriority },		/* 100 = netbsd32_getpriority */
	{ 4, s(struct compat_43_netbsd32_osend_args), 0,
	    compat_43(netbsd32_osend) },	/* 101 = compat_43 netbsd32_osend */
	{ 4, s(struct compat_43_netbsd32_orecv_args), 0,
	    compat_43(netbsd32_orecv) },	/* 102 = compat_43 netbsd32_orecv */
	{ 1, s(struct compat_13_netbsd32_sigreturn_args), 0,
	    compat_13(netbsd32_sigreturn) },	/* 103 = compat_13 sigreturn13 */
	{ 3, s(struct netbsd32_bind_args), 0,
	    netbsd32_bind },			/* 104 = netbsd32_bind */
	{ 5, s(struct netbsd32_setsockopt_args), 0,
	    netbsd32_setsockopt },		/* 105 = netbsd32_setsockopt */
	{ 2, s(struct netbsd32_listen_args), 0,
	    netbsd32_listen },			/* 106 = netbsd32_listen */
	{ 0, 0, 0,
	    sys_nosys },			/* 107 = obsolete vtimes */
	{ 3, s(struct compat_43_netbsd32_osigvec_args), 0,
	    compat_43(netbsd32_osigvec) },	/* 108 = compat_43 netbsd32_osigvec */
#ifdef COMPAT_43
	{ 1, s(struct compat_43_netbsd32_sigblock_args), 0,
	    compat_43(netbsd32_sigblock) },	/* 109 = compat_43 netbsd32_sigblock */
	{ 1, s(struct compat_43_netbsd32_sigsetmask_args), 0,
	    compat_43(netbsd32_sigsetmask) },	/* 110 = compat_43 netbsd32_sigsetmask */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 109 = obsolete sigblock */
	{ 0, 0, 0,
	    sys_nosys },			/* 110 = obsolete sigsetmask */
#endif
	{ 1, s(struct compat_13_netbsd32_sigsuspend_args), 0,
	    compat_13(netbsd32_sigsuspend) },	/* 111 = compat_13 sigsuspend13 */
	{ 2, s(struct compat_43_netbsd32_osigstack_args), 0,
	    compat_43(netbsd32_osigstack) },	/* 112 = compat_43 netbsd32_osigstack */
	{ 3, s(struct compat_43_netbsd32_orecvmsg_args), 0,
	    compat_43(netbsd32_orecvmsg) },	/* 113 = compat_43 netbsd32_orecvmsg */
	{ 3, s(struct compat_43_netbsd32_osendmsg_args), 0,
	    compat_43(netbsd32_osendmsg) },	/* 114 = compat_43 netbsd32_osendmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 115 = obsolete vtrace */
	{ 2, s(struct netbsd32_gettimeofday_args), 0,
	    netbsd32_gettimeofday },		/* 116 = netbsd32_gettimeofday */
	{ 2, s(struct netbsd32_getrusage_args), 0,
	    netbsd32_getrusage },		/* 117 = netbsd32_getrusage */
	{ 5, s(struct netbsd32_getsockopt_args), 0,
	    netbsd32_getsockopt },		/* 118 = netbsd32_getsockopt */
	{ 0, 0, 0,
	    sys_nosys },			/* 119 = obsolete resuba */
	{ 3, s(struct netbsd32_readv_args), 0,
	    netbsd32_readv },			/* 120 = netbsd32_readv */
	{ 3, s(struct netbsd32_writev_args), 0,
	    netbsd32_writev },			/* 121 = netbsd32_writev */
	{ 2, s(struct netbsd32_settimeofday_args), 0,
	    netbsd32_settimeofday },		/* 122 = netbsd32_settimeofday */
	{ 3, s(struct netbsd32_fchown_args), 0,
	    netbsd32_fchown },			/* 123 = netbsd32_fchown */
	{ 2, s(struct netbsd32_fchmod_args), 0,
	    netbsd32_fchmod },			/* 124 = netbsd32_fchmod */
	{ 6, s(struct compat_43_netbsd32_orecvfrom_args), 0,
	    compat_43(netbsd32_orecvfrom) },	/* 125 = compat_43 netbsd32_orecvfrom */
	{ 2, s(struct netbsd32_setreuid_args), 0,
	    netbsd32_setreuid },		/* 126 = netbsd32_setreuid */
	{ 2, s(struct netbsd32_setregid_args), 0,
	    netbsd32_setregid },		/* 127 = netbsd32_setregid */
	{ 2, s(struct netbsd32_rename_args), 0,
	    netbsd32_rename },			/* 128 = netbsd32_rename */
	{ 2, s(struct compat_43_netbsd32_otruncate_args), 0,
	    compat_43(netbsd32_otruncate) },	/* 129 = compat_43 netbsd32_otruncate */
	{ 2, s(struct compat_43_netbsd32_oftruncate_args), 0,
	    compat_43(netbsd32_oftruncate) },	/* 130 = compat_43 netbsd32_oftruncate */
	{ 2, s(struct netbsd32_flock_args), 0,
	    netbsd32_flock },			/* 131 = netbsd32_flock */
	{ 2, s(struct netbsd32_mkfifo_args), 0,
	    netbsd32_mkfifo },			/* 132 = netbsd32_mkfifo */
	{ 6, s(struct netbsd32_sendto_args), 0,
	    netbsd32_sendto },			/* 133 = netbsd32_sendto */
	{ 2, s(struct netbsd32_shutdown_args), 0,
	    netbsd32_shutdown },		/* 134 = netbsd32_shutdown */
	{ 4, s(struct netbsd32_socketpair_args), 0,
	    netbsd32_socketpair },		/* 135 = netbsd32_socketpair */
	{ 2, s(struct netbsd32_mkdir_args), 0,
	    netbsd32_mkdir },			/* 136 = netbsd32_mkdir */
	{ 1, s(struct netbsd32_rmdir_args), 0,
	    netbsd32_rmdir },			/* 137 = netbsd32_rmdir */
	{ 2, s(struct netbsd32_utimes_args), 0,
	    netbsd32_utimes },			/* 138 = netbsd32_utimes */
	{ 0, 0, 0,
	    sys_nosys },			/* 139 = obsolete 4.2 sigreturn */
	{ 2, s(struct netbsd32_adjtime_args), 0,
	    netbsd32_adjtime },			/* 140 = netbsd32_adjtime */
	{ 3, s(struct compat_43_netbsd32_ogetpeername_args), 0,
	    compat_43(netbsd32_ogetpeername) },	/* 141 = compat_43 netbsd32_ogetpeername */
	{ 0, 0, 0,
	    compat_43(sys_gethostid) },		/* 142 = compat_43 ogethostid */
#ifdef COMPAT_43
	{ 1, s(struct compat_43_netbsd32_sethostid_args), 0,
	    compat_43(netbsd32_sethostid) },	/* 143 = compat_43 netbsd32_sethostid */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 143 = obsolete sethostid */
#endif
	{ 2, s(struct compat_43_netbsd32_ogetrlimit_args), 0,
	    compat_43(netbsd32_ogetrlimit) },	/* 144 = compat_43 netbsd32_ogetrlimit */
	{ 2, s(struct compat_43_netbsd32_osetrlimit_args), 0,
	    compat_43(netbsd32_osetrlimit) },	/* 145 = compat_43 netbsd32_osetrlimit */
#ifdef COMPAT_43
	{ 2, s(struct compat_43_netbsd32_killpg_args), 0,
	    compat_43(netbsd32_killpg) },	/* 146 = compat_43 netbsd32_killpg */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 146 = obsolete killpg */
#endif
	{ 0, 0, 0,
	    sys_setsid },			/* 147 = setsid */
	{ 4, s(struct netbsd32_quotactl_args), 0,
	    netbsd32_quotactl },		/* 148 = netbsd32_quotactl */
	{ 0, 0, 0,
	    compat_43(sys_quota) },		/* 149 = compat_43 oquota */
	{ 3, s(struct compat_43_netbsd32_ogetsockname_args), 0,
	    compat_43(netbsd32_ogetsockname) },	/* 150 = compat_43 netbsd32_ogetsockname */
	{ 0, 0, 0,
	    sys_nosys },			/* 151 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 152 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 153 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 154 = unimplemented */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	{ 2, s(struct netbsd32_nfssvc_args), 0,
	    netbsd32_nfssvc },			/* 155 = netbsd32_nfssvc */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 155 = excluded netbsd32_nfssvc */
#endif
	{ 4, s(struct compat_43_netbsd32_ogetdirentries_args), 0,
	    compat_43(netbsd32_ogetdirentries) },/* 156 = compat_43 netbsd32_ogetdirentries */
	{ 2, s(struct netbsd32_statfs_args), 0,
	    netbsd32_statfs },			/* 157 = netbsd32_statfs */
	{ 2, s(struct netbsd32_fstatfs_args), 0,
	    netbsd32_fstatfs },			/* 158 = netbsd32_fstatfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 159 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 160 = unimplemented */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	{ 2, s(struct netbsd32_getfh_args), 0,
	    netbsd32_getfh },			/* 161 = netbsd32_getfh */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 161 = excluded netbsd32_getfh */
#endif
	{ 2, s(struct compat_09_netbsd32_ogetdomainname_args), 0,
	    compat_09(netbsd32_ogetdomainname) },/* 162 = compat_09 netbsd32_ogetdomainname */
	{ 2, s(struct compat_09_netbsd32_osetdomainname_args), 0,
	    compat_09(netbsd32_osetdomainname) },/* 163 = compat_09 netbsd32_osetdomainname */
	{ 1, s(struct compat_09_netbsd32_uname_args), 0,
	    compat_09(netbsd32_uname) },	/* 164 = compat_09 netbsd32_uname */
	{ 2, s(struct netbsd32_sysarch_args), 0,
	    netbsd32_sysarch },			/* 165 = netbsd32_sysarch */
	{ 0, 0, 0,
	    sys_nosys },			/* 166 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 167 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 168 = unimplemented */
#if defined(SYSVSEM) || !defined(_KERNEL)
	{ 5, s(struct compat_10_netbsd32_sys_semsys_args), 0,
	    compat_10(netbsd32_sys_semsys) },	/* 169 = compat_10 osemsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 169 = excluded netbsd32_sys_semsys */
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	{ 6, s(struct compat_10_netbsd32_sys_msgsys_args), 0,
	    compat_10(netbsd32_sys_msgsys) },	/* 170 = compat_10 omsgsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 170 = excluded netbsd32_sys_msgsys */
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	{ 4, s(struct compat_10_netbsd32_sys_shmsys_args), 0,
	    compat_10(netbsd32_sys_shmsys) },	/* 171 = compat_10 oshmsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 171 = excluded netbsd32_sys_shmsys */
#endif
	{ 0, 0, 0,
	    sys_nosys },			/* 172 = unimplemented */
	{ 5, s(struct netbsd32_pread_args), 0,
	    netbsd32_pread },			/* 173 = netbsd32_pread */
	{ 5, s(struct netbsd32_pwrite_args), 0,
	    netbsd32_pwrite },			/* 174 = netbsd32_pwrite */
	{ 1, s(struct netbsd32_ntp_gettime_args), 0,
	    netbsd32_ntp_gettime },		/* 175 = netbsd32_ntp_gettime */
	{ 1, s(struct netbsd32_ntp_adjtime_args), 0,
	    netbsd32_ntp_adjtime },		/* 176 = netbsd32_ntp_adjtime */
	{ 0, 0, 0,
	    sys_nosys },			/* 177 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 178 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 179 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 180 = unimplemented */
	{ 1, s(struct netbsd32_setgid_args), 0,
	    netbsd32_setgid },			/* 181 = netbsd32_setgid */
	{ 1, s(struct netbsd32_setegid_args), 0,
	    netbsd32_setegid },			/* 182 = netbsd32_setegid */
	{ 1, s(struct netbsd32_seteuid_args), 0,
	    netbsd32_seteuid },			/* 183 = netbsd32_seteuid */
#if defined(LFS) || !defined(_KERNEL)
	{ 3, s(struct netbsd32_sys_lfs_bmapv_args), 0,
	    netbsd32_sys_lfs_bmapv },		/* 184 = lfs_bmapv */
	{ 3, s(struct netbsd32_sys_lfs_markv_args), 0,
	    netbsd32_sys_lfs_markv },		/* 185 = lfs_markv */
	{ 2, s(struct netbsd32_sys_lfs_segclean_args), 0,
	    netbsd32_sys_lfs_segclean },	/* 186 = lfs_segclean */
	{ 2, s(struct netbsd32_sys_lfs_segwait_args), 0,
	    netbsd32_sys_lfs_segwait },		/* 187 = lfs_segwait */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 184 = excluded netbsd32_sys_lfs_bmapv */
	{ 0, 0, 0,
	    sys_nosys },			/* 185 = excluded netbsd32_sys_lfs_markv */
	{ 0, 0, 0,
	    sys_nosys },			/* 186 = excluded netbsd32_sys_lfs_segclean */
	{ 0, 0, 0,
	    sys_nosys },			/* 187 = excluded netbsd32_sys_lfs_segwait */
#endif
	{ 2, s(struct compat_12_netbsd32_stat12_args), 0,
	    compat_12(netbsd32_stat12) },	/* 188 = compat_12 netbsd32_stat12 */
	{ 2, s(struct compat_12_netbsd32_fstat12_args), 0,
	    compat_12(netbsd32_fstat12) },	/* 189 = compat_12 netbsd32_fstat12 */
	{ 2, s(struct compat_12_netbsd32_lstat12_args), 0,
	    compat_12(netbsd32_lstat12) },	/* 190 = compat_12 netbsd32_lstat12 */
	{ 2, s(struct netbsd32_pathconf_args), 0,
	    netbsd32_pathconf },		/* 191 = netbsd32_pathconf */
	{ 2, s(struct netbsd32_fpathconf_args), 0,
	    netbsd32_fpathconf },		/* 192 = netbsd32_fpathconf */
	{ 0, 0, 0,
	    sys_nosys },			/* 193 = unimplemented */
	{ 2, s(struct netbsd32_getrlimit_args), 0,
	    netbsd32_getrlimit },		/* 194 = netbsd32_getrlimit */
	{ 2, s(struct netbsd32_setrlimit_args), 0,
	    netbsd32_setrlimit },		/* 195 = netbsd32_setrlimit */
	{ 4, s(struct compat_12_netbsd32_getdirentries_args), 0,
	    compat_12(netbsd32_getdirentries) },/* 196 = compat_12 netbsd32_getdirentries */
	{ 7, s(struct netbsd32_mmap_args), 0,
	    netbsd32_mmap },			/* 197 = netbsd32_mmap */
	{ 0, 0, 0,
	    sys_nosys },			/* 198 = __syscall (indir) */
	{ 4, s(struct netbsd32_lseek_args), 0,
	    netbsd32_lseek },			/* 199 = netbsd32_lseek */
	{ 3, s(struct netbsd32_truncate_args), 0,
	    netbsd32_truncate },		/* 200 = netbsd32_truncate */
	{ 3, s(struct netbsd32_ftruncate_args), 0,
	    netbsd32_ftruncate },		/* 201 = netbsd32_ftruncate */
	{ 6, s(struct netbsd32___sysctl_args), 0,
	    netbsd32___sysctl },		/* 202 = netbsd32___sysctl */
	{ 2, s(struct netbsd32_mlock_args), 0,
	    netbsd32_mlock },			/* 203 = netbsd32_mlock */
	{ 2, s(struct netbsd32_munlock_args), 0,
	    netbsd32_munlock },			/* 204 = netbsd32_munlock */
	{ 1, s(struct netbsd32_undelete_args), 0,
	    netbsd32_undelete },		/* 205 = netbsd32_undelete */
	{ 2, s(struct netbsd32_futimes_args), 0,
	    netbsd32_futimes },			/* 206 = netbsd32_futimes */
	{ 1, s(struct netbsd32_getpgid_args), 0,
	    netbsd32_getpgid },			/* 207 = netbsd32_getpgid */
	{ 2, s(struct netbsd32_reboot_args), 0,
	    netbsd32_reboot },			/* 208 = netbsd32_reboot */
	{ 3, s(struct netbsd32_poll_args), 0,
	    netbsd32_poll },			/* 209 = netbsd32_poll */
#if defined(LKM) || !defined(_KERNEL)
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 210 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 211 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 212 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 213 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 214 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 215 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 216 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 217 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 218 = lkmnosys */
	{ 0, 0, 0,
	    sys_lkmnosys },			/* 219 = lkmnosys */
#else	/* !LKM || !_KERNEL */
	{ 0, 0, 0,
	    sys_nosys },			/* 210 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 211 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 212 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 213 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 214 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 215 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 216 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 217 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 218 = excluded lkmnosys */
	{ 0, 0, 0,
	    sys_nosys },			/* 219 = excluded lkmnosys */
#endif	/* !LKM || !_KERNEL */
#if defined(SYSVSEM) || !defined(_KERNEL)
	{ 4, s(struct compat_14_netbsd32___semctl_args), 0,
	    compat_14(netbsd32___semctl) },	/* 220 = compat_14 netbsd32___semctl */
	{ 3, s(struct netbsd32_semget_args), 0,
	    netbsd32_semget },			/* 221 = netbsd32_semget */
	{ 3, s(struct netbsd32_semop_args), 0,
	    netbsd32_semop },			/* 222 = netbsd32_semop */
	{ 1, s(struct netbsd32_semconfig_args), 0,
	    netbsd32_semconfig },		/* 223 = netbsd32_semconfig */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 220 = excluded compat_14_netbsd32_semctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 221 = excluded netbsd32_semget */
	{ 0, 0, 0,
	    sys_nosys },			/* 222 = excluded netbsd32_semop */
	{ 0, 0, 0,
	    sys_nosys },			/* 223 = excluded netbsd32_semconfig */
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	{ 3, s(struct compat_14_netbsd32_msgctl_args), 0,
	    compat_14(netbsd32_msgctl) },	/* 224 = compat_14 netbsd32_msgctl */
	{ 2, s(struct netbsd32_msgget_args), 0,
	    netbsd32_msgget },			/* 225 = netbsd32_msgget */
	{ 4, s(struct netbsd32_msgsnd_args), 0,
	    netbsd32_msgsnd },			/* 226 = netbsd32_msgsnd */
	{ 5, s(struct netbsd32_msgrcv_args), 0,
	    netbsd32_msgrcv },			/* 227 = netbsd32_msgrcv */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 224 = excluded compat_14_netbsd32_msgctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 225 = excluded netbsd32_msgget */
	{ 0, 0, 0,
	    sys_nosys },			/* 226 = excluded netbsd32_msgsnd */
	{ 0, 0, 0,
	    sys_nosys },			/* 227 = excluded netbsd32_msgrcv */
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	{ 3, s(struct netbsd32_shmat_args), 0,
	    netbsd32_shmat },			/* 228 = netbsd32_shmat */
	{ 3, s(struct compat_14_netbsd32_shmctl_args), 0,
	    compat_14(netbsd32_shmctl) },	/* 229 = compat_14 netbsd32_shmctl */
	{ 1, s(struct netbsd32_shmdt_args), 0,
	    netbsd32_shmdt },			/* 230 = netbsd32_shmdt */
	{ 3, s(struct netbsd32_shmget_args), 0,
	    netbsd32_shmget },			/* 231 = netbsd32_shmget */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 228 = excluded netbsd32_shmat */
	{ 0, 0, 0,
	    sys_nosys },			/* 229 = excluded compat_14_netbsd32_shmctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 230 = excluded netbsd32_shmdt */
	{ 0, 0, 0,
	    sys_nosys },			/* 231 = excluded netbsd32_shmget */
#endif
	{ 2, s(struct netbsd32_clock_gettime_args), 0,
	    netbsd32_clock_gettime },		/* 232 = netbsd32_clock_gettime */
	{ 2, s(struct netbsd32_clock_settime_args), 0,
	    netbsd32_clock_settime },		/* 233 = netbsd32_clock_settime */
	{ 2, s(struct netbsd32_clock_getres_args), 0,
	    netbsd32_clock_getres },		/* 234 = netbsd32_clock_getres */
	{ 0, 0, 0,
	    sys_nosys },			/* 235 = unimplemented timer_create */
	{ 0, 0, 0,
	    sys_nosys },			/* 236 = unimplemented timer_delete */
	{ 0, 0, 0,
	    sys_nosys },			/* 237 = unimplemented timer_settime */
	{ 0, 0, 0,
	    sys_nosys },			/* 238 = unimplemented timer_gettime */
	{ 0, 0, 0,
	    sys_nosys },			/* 239 = unimplemented timer_getoverrun */
	{ 2, s(struct netbsd32_nanosleep_args), 0,
	    netbsd32_nanosleep },		/* 240 = netbsd32_nanosleep */
	{ 1, s(struct netbsd32_fdatasync_args), 0,
	    netbsd32_fdatasync },		/* 241 = netbsd32_fdatasync */
	{ 0, 0, 0,
	    sys_nosys },			/* 242 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 243 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 244 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 245 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 246 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 247 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 248 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 249 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 250 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 251 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 252 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 253 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 254 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 255 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 256 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 257 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 258 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 259 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 260 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 261 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 262 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 263 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 264 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 265 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 266 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 267 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 268 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 269 = unimplemented */
	{ 2, s(struct netbsd32___posix_rename_args), 0,
	    netbsd32___posix_rename },		/* 270 = netbsd32___posix_rename */
	{ 3, s(struct netbsd32_swapctl_args), 0,
	    netbsd32_swapctl },			/* 271 = netbsd32_swapctl */
	{ 3, s(struct netbsd32_getdents_args), 0,
	    netbsd32_getdents },		/* 272 = netbsd32_getdents */
	{ 3, s(struct netbsd32_minherit_args), 0,
	    netbsd32_minherit },		/* 273 = netbsd32_minherit */
	{ 2, s(struct netbsd32_lchmod_args), 0,
	    netbsd32_lchmod },			/* 274 = netbsd32_lchmod */
	{ 3, s(struct netbsd32_lchown_args), 0,
	    netbsd32_lchown },			/* 275 = netbsd32_lchown */
	{ 2, s(struct netbsd32_lutimes_args), 0,
	    netbsd32_lutimes },			/* 276 = netbsd32_lutimes */
	{ 3, s(struct netbsd32___msync13_args), 0,
	    netbsd32___msync13 },		/* 277 = netbsd32___msync13 */
	{ 2, s(struct netbsd32___stat13_args), 0,
	    netbsd32___stat13 },		/* 278 = netbsd32___stat13 */
	{ 2, s(struct netbsd32___fstat13_args), 0,
	    netbsd32___fstat13 },		/* 279 = netbsd32___fstat13 */
	{ 2, s(struct netbsd32___lstat13_args), 0,
	    netbsd32___lstat13 },		/* 280 = netbsd32___lstat13 */
	{ 2, s(struct netbsd32___sigaltstack14_args), 0,
	    netbsd32___sigaltstack14 },		/* 281 = netbsd32___sigaltstack14 */
	{ 0, 0, 0,
	    sys___vfork14 },			/* 282 = __vfork14 */
	{ 3, s(struct netbsd32___posix_chown_args), 0,
	    netbsd32___posix_chown },		/* 283 = netbsd32___posix_chown */
	{ 3, s(struct netbsd32___posix_fchown_args), 0,
	    netbsd32___posix_fchown },		/* 284 = netbsd32___posix_fchown */
	{ 3, s(struct netbsd32___posix_lchown_args), 0,
	    netbsd32___posix_lchown },		/* 285 = netbsd32___posix_lchown */
	{ 1, s(struct netbsd32_getsid_args), 0,
	    netbsd32_getsid },			/* 286 = netbsd32_getsid */
	{ 0, 0, 0,
	    sys_nosys },			/* 287 = unimplemented */
#if defined(KTRACE) || !defined(_KERNEL)
	{ 4, s(struct netbsd32_fktrace_args), 0,
	    netbsd32_fktrace },			/* 288 = netbsd32_fktrace */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 288 = excluded netbsd32_fktrace */
#endif
	{ 5, s(struct netbsd32_preadv_args), 0,
	    netbsd32_preadv },			/* 289 = netbsd32_preadv */
	{ 5, s(struct netbsd32_pwritev_args), 0,
	    netbsd32_pwritev },			/* 290 = netbsd32_pwritev */
	{ 3, s(struct netbsd32___sigaction14_args), 0,
	    netbsd32___sigaction14 },		/* 291 = netbsd32___sigaction14 */
	{ 1, s(struct netbsd32___sigpending14_args), 0,
	    netbsd32___sigpending14 },		/* 292 = netbsd32___sigpending14 */
	{ 3, s(struct netbsd32___sigprocmask14_args), 0,
	    netbsd32___sigprocmask14 },		/* 293 = netbsd32___sigprocmask14 */
	{ 1, s(struct netbsd32___sigsuspend14_args), 0,
	    netbsd32___sigsuspend14 },		/* 294 = netbsd32___sigsuspend14 */
	{ 1, s(struct netbsd32___sigreturn14_args), 0,
	    netbsd32___sigreturn14 },		/* 295 = netbsd32___sigreturn14 */
	{ 2, s(struct netbsd32___getcwd_args), 0,
	    netbsd32___getcwd },		/* 296 = netbsd32___getcwd */
	{ 1, s(struct netbsd32_fchroot_args), 0,
	    netbsd32_fchroot },			/* 297 = netbsd32_fchroot */
	{ 2, s(struct netbsd32_fhopen_args), 0,
	    netbsd32_fhopen },			/* 298 = netbsd32_fhopen */
	{ 2, s(struct netbsd32_fhstat_args), 0,
	    netbsd32_fhstat },			/* 299 = netbsd32_fhstat */
	{ 2, s(struct netbsd32_fhstatfs_args), 0,
	    netbsd32_fhstatfs },		/* 300 = netbsd32_fhstatfs */
#if defined(SYSVSEM) || !defined(_KERNEL)
	{ 4, s(struct netbsd32___semctl14_args), 0,
	    netbsd32___semctl14 },		/* 301 = netbsd32___semctl14 */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 301 = excluded __semctl14 */
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	{ 3, s(struct netbsd32___msgctl13_args), 0,
	    netbsd32___msgctl13 },		/* 302 = netbsd32___msgctl13 */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 302 = excluded __msgctl13 */
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	{ 3, s(struct netbsd32___shmctl13_args), 0,
	    netbsd32___shmctl13 },		/* 303 = netbsd32___shmctl13 */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 303 = excluded __shmctl13 */
#endif
	{ 2, s(struct netbsd32_lchflags_args), 0,
	    netbsd32_lchflags },		/* 304 = netbsd32_lchflags */
	{ 0, 0, 0,
	    sys_issetugid },			/* 305 = issetugid */
	{ 3, s(struct netbsd32_utrace_args), 0,
	    netbsd32_utrace },			/* 306 = netbsd32_utrace */
	{ 0, 0, 0,
	    sys_nosys },			/* 307 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 308 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 309 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 310 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 311 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 312 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 313 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 314 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 315 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 316 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 317 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 318 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 319 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 320 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 321 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 322 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 323 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 324 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 325 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 326 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 327 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 328 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 329 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 330 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 331 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 332 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 333 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 334 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 335 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 336 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 337 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 338 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 339 = unimplemented */
};

