/* $NetBSD: aoutm68k_syscallargs.h,v 1.12 2003/09/22 14:32:16 cl Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.9 2003/09/22 14:31:27 cl Exp 
 */

#ifndef _AOUTM68K_SYS__SYSCALLARGS_H_
#define	_AOUTM68K_SYS__SYSCALLARGS_H_

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
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(mode_t) mode;
};
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_link_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct aoutm68k_sys_unlink_args {
	syscallarg(const char *) path;
};
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_chdir_args {
	syscallarg(const char *) path;
};
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_chmod_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
};

struct aoutm68k_sys_chown_args {
	syscallarg(const char *) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_access_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct aoutm68k_sys_chflags_args {
	syscallarg(const char *) path;
	syscallarg(u_long) flags;
};
#else
#endif
#ifdef COMPAT_43

struct aoutm68k_compat_43_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct aoutm68k_stat43 *) ub;
};
#else
#endif
#ifdef COMPAT_43

struct aoutm68k_compat_43_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct aoutm68k_stat43 *) ub;
};
#else
#endif
#if defined(KTRACE) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_13
#else
#endif
#ifdef COMPAT_13
#else
#endif
#ifdef COMPAT_13
#else
#endif

struct aoutm68k_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(void *) data;
};
#ifdef COMPAT_12
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_revoke_args {
	syscallarg(const char *) path;
};

struct aoutm68k_sys_symlink_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct aoutm68k_sys_readlink_args {
	syscallarg(const char *) path;
	syscallarg(char *) buf;
	syscallarg(size_t) count;
};

struct aoutm68k_sys_execve_args {
	syscallarg(const char *) path;
	syscallarg(char *const *) argp;
	syscallarg(char *const *) envp;
};
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_chroot_args {
	syscallarg(const char *) path;
};
#else
#endif
#ifdef COMPAT_43

struct aoutm68k_compat_43_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct aoutm68k_stat43 *) sb;
};
#else
#endif
#ifdef COMPAT_12
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_12
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_13
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_13
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_rename_args {
	syscallarg(const char *) from;
	syscallarg(const char *) to;
};
#else
#endif
#ifdef COMPAT_43
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_compat_43_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(long) length;
};
#else
#endif
#else
#ifdef COMPAT_AOUT_ALTPATH
#else
#endif
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_rmdir_args {
	syscallarg(const char *) path;
};

struct aoutm68k_sys_utimes_args {
	syscallarg(const char *) path;
	syscallarg(const struct timeval *) tptr;
};
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_43
#else
#endif
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_43
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_statfs_args {
	syscallarg(const char *) path;
	syscallarg(struct statfs *) buf;
};
#else
#endif
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_getfh_args {
	syscallarg(const char *) fname;
	syscallarg(fhandle_t *) fhp;
};
#else
#endif
#else
#endif
#ifdef COMPAT_09
#else
#endif
#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(_LP64) && defined(COMPAT_10)
#else
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(_LP64) && defined(COMPAT_10)
#else
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(_LP64) && defined(COMPAT_10)
#else
#endif
#if defined(NTP) || !defined(_KERNEL)
#else
#endif
#if defined(LFS) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_12

struct aoutm68k_compat_12_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct aoutm68k_stat12 *) ub;
};

struct aoutm68k_compat_12_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct aoutm68k_stat12 *) sb;
};

struct aoutm68k_compat_12_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct aoutm68k_stat12 *) ub;
};
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_pathconf_args {
	syscallarg(const char *) path;
	syscallarg(int) name;
};
#else
#endif
#ifdef COMPAT_12
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(int) pad;
	syscallarg(off_t) length;
};
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_undelete_args {
	syscallarg(const char *) path;
};
#else
#endif
#if defined(LKM) || !defined(_KERNEL)
#else	/* !LKM */
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
#ifdef COMPAT_14
#else
#endif
#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
#ifdef COMPAT_14
#else
#endif
#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
#ifdef COMPAT_14
#else
#endif
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys___posix_rename_args {
	syscallarg(const char *) from;
	syscallarg(const char *) to;
};
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys_lchmod_args {
	syscallarg(const char *) path;
	syscallarg(mode_t) mode;
};

struct aoutm68k_sys_lchown_args {
	syscallarg(const char *) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};

struct aoutm68k_sys_lutimes_args {
	syscallarg(const char *) path;
	syscallarg(const struct timeval *) tptr;
};
#else
#endif

struct aoutm68k_sys___stat13_args {
	syscallarg(const char *) path;
	syscallarg(struct aoutm68k_stat *) ub;
};

struct aoutm68k_sys___fstat13_args {
	syscallarg(int) fd;
	syscallarg(struct aoutm68k_stat *) sb;
};

struct aoutm68k_sys___lstat13_args {
	syscallarg(const char *) path;
	syscallarg(struct aoutm68k_stat *) ub;
};
#ifdef COMPAT_AOUT_ALTPATH

struct aoutm68k_sys___posix_chown_args {
	syscallarg(const char *) path;
	syscallarg(uid_t) uid;
	syscallarg(gid_t) gid;
};
#else
#endif
#if defined(KTRACE) || !defined(_KERNEL)
#else
#endif
#ifdef COMPAT_16
#else
#endif

struct aoutm68k_sys_fhstat_args {
	syscallarg(const fhandle_t *) fhp;
	syscallarg(struct aoutm68k_stat *) sb;
};
#if defined(SYSVSEM) || !defined(_KERNEL)
#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
#else
#endif

/*
 * System call prototypes.
 */

int	sys_exit(struct lwp *, void *, register_t *);
int	sys_fork(struct lwp *, void *, register_t *);
int	sys_read(struct lwp *, void *, register_t *);
int	sys_write(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_open(struct lwp *, void *, register_t *);
#else
int	sys_open(struct lwp *, void *, register_t *);
#endif
int	sys_close(struct lwp *, void *, register_t *);
int	sys_wait4(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_creat(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_link(struct lwp *, void *, register_t *);
int	aoutm68k_sys_unlink(struct lwp *, void *, register_t *);
#else
int	sys_link(struct lwp *, void *, register_t *);
int	sys_unlink(struct lwp *, void *, register_t *);
#endif
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_chdir(struct lwp *, void *, register_t *);
#else
int	sys_chdir(struct lwp *, void *, register_t *);
#endif
int	sys_fchdir(struct lwp *, void *, register_t *);
int	sys_mknod(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_chmod(struct lwp *, void *, register_t *);
int	aoutm68k_sys_chown(struct lwp *, void *, register_t *);
#else
int	sys_chmod(struct lwp *, void *, register_t *);
int	sys_chown(struct lwp *, void *, register_t *);
#endif
int	sys_obreak(struct lwp *, void *, register_t *);
int	sys_getfsstat(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_lseek(struct lwp *, void *, register_t *);
#else
#endif
int	sys_getpid(struct lwp *, void *, register_t *);
int	sys_mount(struct lwp *, void *, register_t *);
int	sys_unmount(struct lwp *, void *, register_t *);
int	sys_setuid(struct lwp *, void *, register_t *);
int	sys_getuid(struct lwp *, void *, register_t *);
int	sys_geteuid(struct lwp *, void *, register_t *);
int	sys_ptrace(struct lwp *, void *, register_t *);
int	sys_recvmsg(struct lwp *, void *, register_t *);
int	sys_sendmsg(struct lwp *, void *, register_t *);
int	sys_recvfrom(struct lwp *, void *, register_t *);
int	sys_accept(struct lwp *, void *, register_t *);
int	sys_getpeername(struct lwp *, void *, register_t *);
int	sys_getsockname(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_access(struct lwp *, void *, register_t *);
int	aoutm68k_sys_chflags(struct lwp *, void *, register_t *);
#else
int	sys_access(struct lwp *, void *, register_t *);
int	sys_chflags(struct lwp *, void *, register_t *);
#endif
int	sys_fchflags(struct lwp *, void *, register_t *);
int	sys_sync(struct lwp *, void *, register_t *);
int	sys_kill(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	aoutm68k_compat_43_sys_stat(struct lwp *, void *, register_t *);
#else
#endif
int	sys_getppid(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	aoutm68k_compat_43_sys_lstat(struct lwp *, void *, register_t *);
#else
#endif
int	sys_dup(struct lwp *, void *, register_t *);
int	sys_pipe(struct lwp *, void *, register_t *);
int	sys_getegid(struct lwp *, void *, register_t *);
int	sys_profil(struct lwp *, void *, register_t *);
#if defined(KTRACE) || !defined(_KERNEL)
int	sys_ktrace(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_13
int	compat_13_sys_sigaction(struct lwp *, void *, register_t *);
#else
#endif
int	sys_getgid(struct lwp *, void *, register_t *);
#ifdef COMPAT_13
int	compat_13_sys_sigprocmask(struct lwp *, void *, register_t *);
#else
#endif
int	sys___getlogin(struct lwp *, void *, register_t *);
int	sys___setlogin(struct lwp *, void *, register_t *);
int	sys_acct(struct lwp *, void *, register_t *);
#ifdef COMPAT_13
int	compat_13_sys_sigpending(struct lwp *, void *, register_t *);
int	compat_13_sys_sigaltstack(struct lwp *, void *, register_t *);
#else
#endif
int	aoutm68k_sys_ioctl(struct lwp *, void *, register_t *);
#ifdef COMPAT_12
int	compat_12_sys_reboot(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_revoke(struct lwp *, void *, register_t *);
int	aoutm68k_sys_symlink(struct lwp *, void *, register_t *);
int	aoutm68k_sys_readlink(struct lwp *, void *, register_t *);
int	aoutm68k_sys_execve(struct lwp *, void *, register_t *);
#else
int	sys_revoke(struct lwp *, void *, register_t *);
int	sys_symlink(struct lwp *, void *, register_t *);
int	sys_readlink(struct lwp *, void *, register_t *);
int	sys_execve(struct lwp *, void *, register_t *);
#endif
int	sys_umask(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_chroot(struct lwp *, void *, register_t *);
#else
int	sys_chroot(struct lwp *, void *, register_t *);
#endif
#ifdef COMPAT_43
int	aoutm68k_compat_43_sys_fstat(struct lwp *, void *, register_t *);
int	compat_43_sys_getkerninfo(struct lwp *, void *, register_t *);
int	compat_43_sys_getpagesize(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_12
int	compat_12_sys_msync(struct lwp *, void *, register_t *);
#else
#endif
int	sys_vfork(struct lwp *, void *, register_t *);
int	sys_sbrk(struct lwp *, void *, register_t *);
int	sys_sstk(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_mmap(struct lwp *, void *, register_t *);
#else
#endif
int	sys_ovadvise(struct lwp *, void *, register_t *);
int	sys_munmap(struct lwp *, void *, register_t *);
int	sys_mprotect(struct lwp *, void *, register_t *);
int	sys_madvise(struct lwp *, void *, register_t *);
int	sys_mincore(struct lwp *, void *, register_t *);
int	sys_getgroups(struct lwp *, void *, register_t *);
int	sys_setgroups(struct lwp *, void *, register_t *);
int	sys_getpgrp(struct lwp *, void *, register_t *);
int	sys_setpgid(struct lwp *, void *, register_t *);
int	sys_setitimer(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_wait(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_12
int	compat_12_sys_swapon(struct lwp *, void *, register_t *);
#else
#endif
int	sys_getitimer(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_gethostname(struct lwp *, void *, register_t *);
int	compat_43_sys_sethostname(struct lwp *, void *, register_t *);
int	compat_43_sys_getdtablesize(struct lwp *, void *, register_t *);
#else
#endif
int	sys_dup2(struct lwp *, void *, register_t *);
int	sys_fcntl(struct lwp *, void *, register_t *);
int	sys_select(struct lwp *, void *, register_t *);
int	sys_fsync(struct lwp *, void *, register_t *);
int	sys_setpriority(struct lwp *, void *, register_t *);
int	sys_socket(struct lwp *, void *, register_t *);
int	sys_connect(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_accept(struct lwp *, void *, register_t *);
#else
#endif
int	sys_getpriority(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_send(struct lwp *, void *, register_t *);
int	compat_43_sys_recv(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_13
int	compat_13_sys_sigreturn(struct lwp *, void *, register_t *);
#else
#endif
int	sys_bind(struct lwp *, void *, register_t *);
int	sys_setsockopt(struct lwp *, void *, register_t *);
int	sys_listen(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_sigvec(struct lwp *, void *, register_t *);
int	compat_43_sys_sigblock(struct lwp *, void *, register_t *);
int	compat_43_sys_sigsetmask(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_13
int	compat_13_sys_sigsuspend(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_43
int	compat_43_sys_sigstack(struct lwp *, void *, register_t *);
int	compat_43_sys_recvmsg(struct lwp *, void *, register_t *);
int	compat_43_sys_sendmsg(struct lwp *, void *, register_t *);
#else
#endif
int	sys_gettimeofday(struct lwp *, void *, register_t *);
int	sys_getrusage(struct lwp *, void *, register_t *);
int	sys_getsockopt(struct lwp *, void *, register_t *);
int	sys_readv(struct lwp *, void *, register_t *);
int	sys_writev(struct lwp *, void *, register_t *);
int	sys_settimeofday(struct lwp *, void *, register_t *);
int	sys_fchown(struct lwp *, void *, register_t *);
int	sys_fchmod(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_recvfrom(struct lwp *, void *, register_t *);
#else
#endif
int	sys_setreuid(struct lwp *, void *, register_t *);
int	sys_setregid(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_rename(struct lwp *, void *, register_t *);
#else
int	sys_rename(struct lwp *, void *, register_t *);
#endif
#ifdef COMPAT_43
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_compat_43_sys_truncate(struct lwp *, void *, register_t *);
#else
int	compat_43_sys_truncate(struct lwp *, void *, register_t *);
#endif
int	compat_43_sys_ftruncate(struct lwp *, void *, register_t *);
#else
#ifdef COMPAT_AOUT_ALTPATH
#else
#endif
#endif
int	sys_flock(struct lwp *, void *, register_t *);
int	sys_mkfifo(struct lwp *, void *, register_t *);
int	sys_sendto(struct lwp *, void *, register_t *);
int	sys_shutdown(struct lwp *, void *, register_t *);
int	sys_socketpair(struct lwp *, void *, register_t *);
int	sys_mkdir(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_rmdir(struct lwp *, void *, register_t *);
int	aoutm68k_sys_utimes(struct lwp *, void *, register_t *);
#else
int	sys_rmdir(struct lwp *, void *, register_t *);
int	sys_utimes(struct lwp *, void *, register_t *);
#endif
int	sys_adjtime(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_getpeername(struct lwp *, void *, register_t *);
int	compat_43_sys_gethostid(struct lwp *, void *, register_t *);
int	compat_43_sys_sethostid(struct lwp *, void *, register_t *);
int	compat_43_sys_getrlimit(struct lwp *, void *, register_t *);
int	compat_43_sys_setrlimit(struct lwp *, void *, register_t *);
int	compat_43_sys_killpg(struct lwp *, void *, register_t *);
#else
#endif
int	sys_setsid(struct lwp *, void *, register_t *);
int	sys_quotactl(struct lwp *, void *, register_t *);
#ifdef COMPAT_43
int	compat_43_sys_quota(struct lwp *, void *, register_t *);
int	compat_43_sys_getsockname(struct lwp *, void *, register_t *);
#else
#endif
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
int	sys_nfssvc(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_43
int	compat_43_sys_getdirentries(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_statfs(struct lwp *, void *, register_t *);
#else
int	sys_statfs(struct lwp *, void *, register_t *);
#endif
int	sys_fstatfs(struct lwp *, void *, register_t *);
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_getfh(struct lwp *, void *, register_t *);
#else
int	sys_getfh(struct lwp *, void *, register_t *);
#endif
#else
#endif
#ifdef COMPAT_09
int	compat_09_sys_getdomainname(struct lwp *, void *, register_t *);
int	compat_09_sys_setdomainname(struct lwp *, void *, register_t *);
int	compat_09_sys_uname(struct lwp *, void *, register_t *);
#else
#endif
int	sys_sysarch(struct lwp *, void *, register_t *);
#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(_LP64) && defined(COMPAT_10)
int	compat_10_sys_semsys(struct lwp *, void *, register_t *);
#else
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(_LP64) && defined(COMPAT_10)
int	compat_10_sys_msgsys(struct lwp *, void *, register_t *);
#else
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(_LP64) && defined(COMPAT_10)
int	compat_10_sys_shmsys(struct lwp *, void *, register_t *);
#else
#endif
int	sys_pread(struct lwp *, void *, register_t *);
int	sys_pwrite(struct lwp *, void *, register_t *);
int	sys_ntp_gettime(struct lwp *, void *, register_t *);
#if defined(NTP) || !defined(_KERNEL)
int	sys_ntp_adjtime(struct lwp *, void *, register_t *);
#else
#endif
int	sys_setgid(struct lwp *, void *, register_t *);
int	sys_setegid(struct lwp *, void *, register_t *);
int	sys_seteuid(struct lwp *, void *, register_t *);
#if defined(LFS) || !defined(_KERNEL)
int	sys_lfs_bmapv(struct lwp *, void *, register_t *);
int	sys_lfs_markv(struct lwp *, void *, register_t *);
int	sys_lfs_segclean(struct lwp *, void *, register_t *);
int	sys_lfs_segwait(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_12
int	aoutm68k_compat_12_sys_stat(struct lwp *, void *, register_t *);
int	aoutm68k_compat_12_sys_fstat(struct lwp *, void *, register_t *);
int	aoutm68k_compat_12_sys_lstat(struct lwp *, void *, register_t *);
#else
#endif
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_pathconf(struct lwp *, void *, register_t *);
#else
int	sys_pathconf(struct lwp *, void *, register_t *);
#endif
int	sys_fpathconf(struct lwp *, void *, register_t *);
int	sys_getrlimit(struct lwp *, void *, register_t *);
int	sys_setrlimit(struct lwp *, void *, register_t *);
#ifdef COMPAT_12
int	compat_12_sys_getdirentries(struct lwp *, void *, register_t *);
#else
#endif
int	sys_mmap(struct lwp *, void *, register_t *);
int	sys_lseek(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_truncate(struct lwp *, void *, register_t *);
#else
int	sys_truncate(struct lwp *, void *, register_t *);
#endif
int	sys_ftruncate(struct lwp *, void *, register_t *);
int	sys___sysctl(struct lwp *, void *, register_t *);
int	sys_mlock(struct lwp *, void *, register_t *);
int	sys_munlock(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_undelete(struct lwp *, void *, register_t *);
#else
int	sys_undelete(struct lwp *, void *, register_t *);
#endif
int	sys_futimes(struct lwp *, void *, register_t *);
int	sys_getpgid(struct lwp *, void *, register_t *);
int	sys_reboot(struct lwp *, void *, register_t *);
int	sys_poll(struct lwp *, void *, register_t *);
#if defined(LKM) || !defined(_KERNEL)
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
int	sys_lkmnosys(struct lwp *, void *, register_t *);
#else	/* !LKM */
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
#ifdef COMPAT_14
int	compat_14_sys___semctl(struct lwp *, void *, register_t *);
#else
#endif
int	sys_semget(struct lwp *, void *, register_t *);
int	sys_semop(struct lwp *, void *, register_t *);
int	sys_semconfig(struct lwp *, void *, register_t *);
#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
#ifdef COMPAT_14
int	compat_14_sys_msgctl(struct lwp *, void *, register_t *);
#else
#endif
int	sys_msgget(struct lwp *, void *, register_t *);
int	sys_msgsnd(struct lwp *, void *, register_t *);
int	sys_msgrcv(struct lwp *, void *, register_t *);
#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
int	sys_shmat(struct lwp *, void *, register_t *);
#ifdef COMPAT_14
int	compat_14_sys_shmctl(struct lwp *, void *, register_t *);
#else
#endif
int	sys_shmdt(struct lwp *, void *, register_t *);
int	sys_shmget(struct lwp *, void *, register_t *);
#else
#endif
int	sys_clock_gettime(struct lwp *, void *, register_t *);
int	sys_clock_settime(struct lwp *, void *, register_t *);
int	sys_clock_getres(struct lwp *, void *, register_t *);
int	sys_nanosleep(struct lwp *, void *, register_t *);
int	sys_fdatasync(struct lwp *, void *, register_t *);
int	sys_mlockall(struct lwp *, void *, register_t *);
int	sys_munlockall(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys___posix_rename(struct lwp *, void *, register_t *);
#else
int	sys___posix_rename(struct lwp *, void *, register_t *);
#endif
int	sys_swapctl(struct lwp *, void *, register_t *);
int	sys_getdents(struct lwp *, void *, register_t *);
int	sys_minherit(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys_lchmod(struct lwp *, void *, register_t *);
int	aoutm68k_sys_lchown(struct lwp *, void *, register_t *);
int	aoutm68k_sys_lutimes(struct lwp *, void *, register_t *);
#else
int	sys_lchmod(struct lwp *, void *, register_t *);
int	sys_lchown(struct lwp *, void *, register_t *);
int	sys_lutimes(struct lwp *, void *, register_t *);
#endif
int	sys___msync13(struct lwp *, void *, register_t *);
int	aoutm68k_sys___stat13(struct lwp *, void *, register_t *);
int	aoutm68k_sys___fstat13(struct lwp *, void *, register_t *);
int	aoutm68k_sys___lstat13(struct lwp *, void *, register_t *);
int	sys___sigaltstack14(struct lwp *, void *, register_t *);
int	sys___vfork14(struct lwp *, void *, register_t *);
#ifdef COMPAT_AOUT_ALTPATH
int	aoutm68k_sys___posix_chown(struct lwp *, void *, register_t *);
#else
int	sys___posix_chown(struct lwp *, void *, register_t *);
#endif
int	sys___posix_fchown(struct lwp *, void *, register_t *);
int	sys___posix_lchown(struct lwp *, void *, register_t *);
int	sys_getsid(struct lwp *, void *, register_t *);
#if defined(KTRACE) || !defined(_KERNEL)
int	sys_fktrace(struct lwp *, void *, register_t *);
#else
#endif
int	sys_preadv(struct lwp *, void *, register_t *);
int	sys_pwritev(struct lwp *, void *, register_t *);
int	sys___sigaction14(struct lwp *, void *, register_t *);
int	sys___sigpending14(struct lwp *, void *, register_t *);
int	sys___sigprocmask14(struct lwp *, void *, register_t *);
int	sys___sigsuspend14(struct lwp *, void *, register_t *);
#ifdef COMPAT_16
int	compat_16_sys___sigreturn14(struct lwp *, void *, register_t *);
#else
#endif
int	sys___getcwd(struct lwp *, void *, register_t *);
int	sys_fchroot(struct lwp *, void *, register_t *);
int	sys_fhopen(struct lwp *, void *, register_t *);
int	aoutm68k_sys_fhstat(struct lwp *, void *, register_t *);
int	sys_fhstatfs(struct lwp *, void *, register_t *);
#if defined(SYSVSEM) || !defined(_KERNEL)
int	sys_____semctl13(struct lwp *, void *, register_t *);
#else
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
int	sys___msgctl13(struct lwp *, void *, register_t *);
#else
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
int	sys___shmctl13(struct lwp *, void *, register_t *);
#else
#endif
int	sys_lchflags(struct lwp *, void *, register_t *);
int	sys_issetugid(struct lwp *, void *, register_t *);
int	sys_getcontext(struct lwp *, void *, register_t *);
int	sys_setcontext(struct lwp *, void *, register_t *);
#endif /* _AOUTM68K_SYS__SYSCALLARGS_H_ */
