/* $NetBSD: freebsd_syscallargs.h,v 1.70 2007/03/04 06:01:15 christos Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.55 2006/09/01 20:58:18 matt Exp
 */

#ifndef _FREEBSD_SYS_SYSCALLARGS_H_
#define	_FREEBSD_SYS_SYSCALLARGS_H_

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

struct freebsd_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct compat_43_freebsd_sys_creat_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct freebsd_sys_link_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct freebsd_sys_unlink_args {
	syscallarg(const char *) path;
};

struct freebsd_sys_chdir_args {
	syscallarg(const char *) path;
};

struct freebsd_sys_mknod_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
	syscallarg(int) dev;
};

struct freebsd_sys_chmod_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct freebsd_sys_chown_args {
	syscallarg(const char *) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct freebsd_sys_mount_args {
	syscallarg(int) type;
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(void *) data;
};

struct freebsd_sys_unmount_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct freebsd_sys_ptrace_args {
	syscallarg(int) req;
	syscallarg(pid_t) pid;
	syscallarg(void *) addr;
	syscallarg(int) data;
};

struct freebsd_sys_access_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct freebsd_sys_chflags_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct compat_43_freebsd_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat43 *) ub;
};

struct compat_43_freebsd_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat43 *) ub;
};
#ifdef KTRACE
#else
#endif

struct freebsd_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(void *) data;
};

struct freebsd_sys_revoke_args {
	syscallarg(const char *) path;
};

struct freebsd_sys_symlink_args {
	syscallarg(const char *) path;
	syscallarg(const char *) link;
};

struct freebsd_sys_readlink_args {
	syscallarg(const char *) path;
	syscallarg(char *) buf;
	syscallarg(int) count;
};

struct freebsd_sys_execve_args {
	syscallarg(const char *) path;
	syscallarg(char **) argp;
	syscallarg(char **) envp;
};

struct freebsd_sys_chroot_args {
	syscallarg(const char *) path;
};

struct freebsd_sys_msync_args {
	syscallarg(void *) addr;
	syscallarg(size_t) len;
	syscallarg(int) flags;
};

struct freebsd_sys_sigreturn_args {
	syscallarg(struct freebsd_sigcontext *) scp;
};
#ifdef TRACE
#else
#endif

struct freebsd_sys_rename_args {
	syscallarg(const char *) from;
	syscallarg(const char *) to;
};

struct compat_43_freebsd_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(long) length;
};

struct freebsd_sys_mkfifo_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct freebsd_sys_mkdir_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct freebsd_sys_rmdir_args {
	syscallarg(const char *) path;
};
#if defined(NFS) || defined(NFSSERVER)
#else
#endif

struct freebsd_sys_statfs_args {
	syscallarg(const char *) path;
	syscallarg(struct statfs12 *) buf;
};
#ifdef NFS

struct freebsd_sys_getfh_args {
	syscallarg(const char *) fname;
	syscallarg(struct compat_30_fhandle *) fhp;
};
#else
#endif

struct freebsd_sys_rtprio_args {
	syscallarg(int) function;
	syscallarg(pid_t) pid;
	syscallarg(struct freebsd_rtprio *) rtp;
};
#if defined(SYSVSEM) && !defined(_LP64)

struct freebsd_sys_semsys_args {
	syscallarg(int) which;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
	syscallarg(int) a5;
};
#else
#endif
#if defined(SYSVMSG) && !defined(_LP64)

struct freebsd_sys_msgsys_args {
	syscallarg(int) which;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
	syscallarg(int) a5;
	syscallarg(int) a6;
};
#else
#endif
#if defined(SYSVSHM) && !defined(_LP64)

struct freebsd_sys_shmsys_args {
	syscallarg(int) which;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
};
#else
#endif
#ifdef NTP

struct freebsd_ntp_adjtime_args {
	syscallarg(struct freebsd_timex *) tp;
};
#else
#endif
#ifdef LFS
#else
#endif

struct freebsd_sys_stat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat12 *) ub;
};

struct freebsd_sys_lstat_args {
	syscallarg(const char *) path;
	syscallarg(struct stat12 *) ub;
};

struct freebsd_sys_pathconf_args {
	syscallarg(const char *) path;
	syscallarg(int) name;
};

struct freebsd_sys_mmap_args {
	syscallarg(void *) addr;
	syscallarg(size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(long) pad;
	syscallarg(off_t) pos;
};

struct freebsd_sys_truncate_args {
	syscallarg(const char *) path;
	syscallarg(int) pad;
	syscallarg(off_t) length;
};

struct freebsd_sys_sysctl_args {
	syscallarg(int *) name;
	syscallarg(u_int) namelen;
	syscallarg(void *) old;
	syscallarg(size_t *) oldlenp;
	syscallarg(void *) new;
	syscallarg(size_t) newlen;
};
#ifdef FREEBSD_BASED_ON_44LITE_R2

struct freebsd_sys_undelete_args {
	syscallarg(char *) path;
};
#else
#endif
#if 0
#else
#endif
#ifdef SYSVSEM
#else
#endif
#ifdef SYSVMSG
#else
#endif
#ifdef SYSVSHM
#else
#endif

struct freebsd_sys_rfork_args {
	syscallarg(int) flags;
};

struct freebsd_sys_lchown_args {
	syscallarg(const char *) path;
	syscallarg(int) uid;
	syscallarg(int) gid;
};

struct freebsd_sys_sched_setparam_args {
	syscallarg(pid_t) pid;
	syscallarg(const struct freebsd_sched_param *) sp;
};

struct freebsd_sys_sched_getparam_args {
	syscallarg(pid_t) pid;
	syscallarg(struct freebsd_sched_param *) sp;
};

struct freebsd_sys_sched_setscheduler_args {
	syscallarg(pid_t) pid;
	syscallarg(int) policy;
	syscallarg(const struct sched_param *) sp;
};

struct freebsd_sys_sched_getscheduler_args {
	syscallarg(pid_t) pid;
};

struct freebsd_sys_sched_get_priority_max_args {
	syscallarg(int) policy;
};

struct freebsd_sys_sched_get_priority_min_args {
	syscallarg(int) policy;
};

struct freebsd_sys_utrace_args {
	syscallarg(void *) addr;
	syscallarg(size_t) len;
};

struct freebsd_sys_sigaction4_args {
	syscallarg(int) signum;
	syscallarg(const struct freebsd_sigaction4 *) nsa;
	syscallarg(struct freebsd_sigaction4 *) osa;
};
#if defined(P1003_1B_SEMAPHORE) || !defined(_KERNEL)
#else
#endif

/*
 * System call prototypes.
 */

int	sys_nosys(struct lwp *, void *, register_t *);

int	sys_exit(struct lwp *, void *, register_t *);

int	sys_fork(struct lwp *, void *, register_t *);

int	sys_read(struct lwp *, void *, register_t *);

int	sys_write(struct lwp *, void *, register_t *);

int	freebsd_sys_open(struct lwp *, void *, register_t *);

int	sys_close(struct lwp *, void *, register_t *);

int	sys_wait4(struct lwp *, void *, register_t *);

int	compat_43_freebsd_sys_creat(struct lwp *, void *, register_t *);

int	freebsd_sys_link(struct lwp *, void *, register_t *);

int	freebsd_sys_unlink(struct lwp *, void *, register_t *);

int	freebsd_sys_chdir(struct lwp *, void *, register_t *);

int	sys_fchdir(struct lwp *, void *, register_t *);

int	freebsd_sys_mknod(struct lwp *, void *, register_t *);

int	freebsd_sys_chmod(struct lwp *, void *, register_t *);

int	freebsd_sys_chown(struct lwp *, void *, register_t *);

int	sys_obreak(struct lwp *, void *, register_t *);

int	compat_20_sys_getfsstat(struct lwp *, void *, register_t *);

int	compat_43_sys_lseek(struct lwp *, void *, register_t *);

int	sys_getpid_with_ppid(struct lwp *, void *, register_t *);

int	freebsd_sys_mount(struct lwp *, void *, register_t *);

int	freebsd_sys_unmount(struct lwp *, void *, register_t *);

int	sys_setuid(struct lwp *, void *, register_t *);

int	sys_getuid_with_euid(struct lwp *, void *, register_t *);

int	sys_geteuid(struct lwp *, void *, register_t *);

int	freebsd_sys_ptrace(struct lwp *, void *, register_t *);

int	sys_recvmsg(struct lwp *, void *, register_t *);

int	sys_sendmsg(struct lwp *, void *, register_t *);

int	sys_recvfrom(struct lwp *, void *, register_t *);

int	sys_accept(struct lwp *, void *, register_t *);

int	sys_getpeername(struct lwp *, void *, register_t *);

int	sys_getsockname(struct lwp *, void *, register_t *);

int	freebsd_sys_access(struct lwp *, void *, register_t *);

int	freebsd_sys_chflags(struct lwp *, void *, register_t *);

int	sys_fchflags(struct lwp *, void *, register_t *);

int	sys_sync(struct lwp *, void *, register_t *);

int	sys_kill(struct lwp *, void *, register_t *);

int	compat_43_freebsd_sys_stat(struct lwp *, void *, register_t *);

int	sys_getppid(struct lwp *, void *, register_t *);

int	compat_43_freebsd_sys_lstat(struct lwp *, void *, register_t *);

int	sys_dup(struct lwp *, void *, register_t *);

int	sys_pipe(struct lwp *, void *, register_t *);

int	sys_getegid(struct lwp *, void *, register_t *);

int	sys_profil(struct lwp *, void *, register_t *);

#ifdef KTRACE
int	sys_ktrace(struct lwp *, void *, register_t *);

#else
#endif
int	compat_13_sys_sigaction(struct lwp *, void *, register_t *);

int	sys_getgid_with_egid(struct lwp *, void *, register_t *);

int	compat_13_sys_sigprocmask(struct lwp *, void *, register_t *);

int	sys___getlogin(struct lwp *, void *, register_t *);

int	sys___setlogin(struct lwp *, void *, register_t *);

int	sys_acct(struct lwp *, void *, register_t *);

int	compat_13_sys_sigpending(struct lwp *, void *, register_t *);

int	compat_13_sys_sigaltstack(struct lwp *, void *, register_t *);

int	freebsd_sys_ioctl(struct lwp *, void *, register_t *);

int	sys_reboot(struct lwp *, void *, register_t *);

int	freebsd_sys_revoke(struct lwp *, void *, register_t *);

int	freebsd_sys_symlink(struct lwp *, void *, register_t *);

int	freebsd_sys_readlink(struct lwp *, void *, register_t *);

int	freebsd_sys_execve(struct lwp *, void *, register_t *);

int	sys_umask(struct lwp *, void *, register_t *);

int	freebsd_sys_chroot(struct lwp *, void *, register_t *);

int	compat_43_sys_fstat(struct lwp *, void *, register_t *);

int	compat_43_sys_getkerninfo(struct lwp *, void *, register_t *);

int	compat_43_sys_getpagesize(struct lwp *, void *, register_t *);

int	freebsd_sys_msync(struct lwp *, void *, register_t *);

int	sys_vfork(struct lwp *, void *, register_t *);

int	sys_sbrk(struct lwp *, void *, register_t *);

int	sys_sstk(struct lwp *, void *, register_t *);

int	compat_43_sys_mmap(struct lwp *, void *, register_t *);

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

int	compat_43_sys_wait(struct lwp *, void *, register_t *);

int	compat_12_sys_swapon(struct lwp *, void *, register_t *);

int	sys_getitimer(struct lwp *, void *, register_t *);

int	compat_43_sys_gethostname(struct lwp *, void *, register_t *);

int	compat_43_sys_sethostname(struct lwp *, void *, register_t *);

int	compat_43_sys_getdtablesize(struct lwp *, void *, register_t *);

int	sys_dup2(struct lwp *, void *, register_t *);

int	sys_fcntl(struct lwp *, void *, register_t *);

int	sys_select(struct lwp *, void *, register_t *);

int	sys_fsync(struct lwp *, void *, register_t *);

int	sys_setpriority(struct lwp *, void *, register_t *);

int	compat_30_sys_socket(struct lwp *, void *, register_t *);

int	sys_connect(struct lwp *, void *, register_t *);

int	compat_43_sys_accept(struct lwp *, void *, register_t *);

int	sys_getpriority(struct lwp *, void *, register_t *);

int	compat_43_sys_send(struct lwp *, void *, register_t *);

int	compat_43_sys_recv(struct lwp *, void *, register_t *);

int	freebsd_sys_sigreturn(struct lwp *, void *, register_t *);

int	sys_bind(struct lwp *, void *, register_t *);

int	sys_setsockopt(struct lwp *, void *, register_t *);

int	sys_listen(struct lwp *, void *, register_t *);

int	compat_43_sys_sigvec(struct lwp *, void *, register_t *);

int	compat_43_sys_sigblock(struct lwp *, void *, register_t *);

int	compat_43_sys_sigsetmask(struct lwp *, void *, register_t *);

int	compat_13_sys_sigsuspend(struct lwp *, void *, register_t *);

int	compat_43_sys_sigstack(struct lwp *, void *, register_t *);

int	compat_43_sys_recvmsg(struct lwp *, void *, register_t *);

int	compat_43_sys_sendmsg(struct lwp *, void *, register_t *);

#ifdef TRACE
int	sys_vtrace(struct lwp *, void *, register_t *);

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

int	compat_43_sys_recvfrom(struct lwp *, void *, register_t *);

int	sys_setreuid(struct lwp *, void *, register_t *);

int	sys_setregid(struct lwp *, void *, register_t *);

int	freebsd_sys_rename(struct lwp *, void *, register_t *);

int	compat_43_freebsd_sys_truncate(struct lwp *, void *, register_t *);

int	compat_43_sys_ftruncate(struct lwp *, void *, register_t *);

int	sys_flock(struct lwp *, void *, register_t *);

int	freebsd_sys_mkfifo(struct lwp *, void *, register_t *);

int	sys_sendto(struct lwp *, void *, register_t *);

int	sys_shutdown(struct lwp *, void *, register_t *);

int	sys_socketpair(struct lwp *, void *, register_t *);

int	freebsd_sys_mkdir(struct lwp *, void *, register_t *);

int	freebsd_sys_rmdir(struct lwp *, void *, register_t *);

int	sys_utimes(struct lwp *, void *, register_t *);

int	sys_adjtime(struct lwp *, void *, register_t *);

int	compat_43_sys_getpeername(struct lwp *, void *, register_t *);

int	compat_43_sys_gethostid(struct lwp *, void *, register_t *);

int	compat_43_sys_sethostid(struct lwp *, void *, register_t *);

int	compat_43_sys_getrlimit(struct lwp *, void *, register_t *);

int	compat_43_sys_setrlimit(struct lwp *, void *, register_t *);

int	compat_43_sys_killpg(struct lwp *, void *, register_t *);

int	sys_setsid(struct lwp *, void *, register_t *);

int	sys_quotactl(struct lwp *, void *, register_t *);

int	compat_43_sys_quota(struct lwp *, void *, register_t *);

int	compat_43_sys_getsockname(struct lwp *, void *, register_t *);

#if defined(NFS) || defined(NFSSERVER)
int	sys_nfssvc(struct lwp *, void *, register_t *);

#else
#endif
int	compat_43_sys_getdirentries(struct lwp *, void *, register_t *);

int	freebsd_sys_statfs(struct lwp *, void *, register_t *);

int	compat_20_sys_fstatfs(struct lwp *, void *, register_t *);

#ifdef NFS
int	freebsd_sys_getfh(struct lwp *, void *, register_t *);

#else
#endif
int	compat_09_sys_getdomainname(struct lwp *, void *, register_t *);

int	compat_09_sys_setdomainname(struct lwp *, void *, register_t *);

int	compat_09_sys_uname(struct lwp *, void *, register_t *);

int	sys_sysarch(struct lwp *, void *, register_t *);

int	freebsd_sys_rtprio(struct lwp *, void *, register_t *);

#if defined(SYSVSEM) && !defined(_LP64)
int	freebsd_sys_semsys(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVMSG) && !defined(_LP64)
int	freebsd_sys_msgsys(struct lwp *, void *, register_t *);

#else
#endif
#if defined(SYSVSHM) && !defined(_LP64)
int	freebsd_sys_shmsys(struct lwp *, void *, register_t *);

#else
#endif
int	sys_pread(struct lwp *, void *, register_t *);

int	sys_pwrite(struct lwp *, void *, register_t *);

#ifdef NTP
int	freebsd_ntp_adjtime(struct lwp *, void *, register_t *);

#else
#endif
int	sys_setgid(struct lwp *, void *, register_t *);

int	sys_setegid(struct lwp *, void *, register_t *);

int	sys_seteuid(struct lwp *, void *, register_t *);

#ifdef LFS
int	sys_lfs_bmapv(struct lwp *, void *, register_t *);

int	sys_lfs_markv(struct lwp *, void *, register_t *);

int	sys_lfs_segclean(struct lwp *, void *, register_t *);

int	sys_lfs_segwait(struct lwp *, void *, register_t *);

#else
#endif
int	freebsd_sys_stat(struct lwp *, void *, register_t *);

int	compat_12_sys_fstat(struct lwp *, void *, register_t *);

int	freebsd_sys_lstat(struct lwp *, void *, register_t *);

int	freebsd_sys_pathconf(struct lwp *, void *, register_t *);

int	sys_fpathconf(struct lwp *, void *, register_t *);

int	sys_getrlimit(struct lwp *, void *, register_t *);

int	sys_setrlimit(struct lwp *, void *, register_t *);

int	compat_12_sys_getdirentries(struct lwp *, void *, register_t *);

int	freebsd_sys_mmap(struct lwp *, void *, register_t *);

int	sys_lseek(struct lwp *, void *, register_t *);

int	freebsd_sys_truncate(struct lwp *, void *, register_t *);

int	sys_ftruncate(struct lwp *, void *, register_t *);

int	freebsd_sys_sysctl(struct lwp *, void *, register_t *);

int	sys_mlock(struct lwp *, void *, register_t *);

int	sys_munlock(struct lwp *, void *, register_t *);

#ifdef FREEBSD_BASED_ON_44LITE_R2
int	freebsd_sys_undelete(struct lwp *, void *, register_t *);

#else
#endif
int	sys_futimes(struct lwp *, void *, register_t *);

int	sys_getpgid(struct lwp *, void *, register_t *);

#if 0
#else
#endif
int	sys_poll(struct lwp *, void *, register_t *);

#ifdef SYSVSEM
int	compat_14_sys___semctl(struct lwp *, void *, register_t *);

int	sys_semget(struct lwp *, void *, register_t *);

int	sys_semop(struct lwp *, void *, register_t *);

int	sys_semconfig(struct lwp *, void *, register_t *);

#else
#endif
#ifdef SYSVMSG
int	compat_14_sys_msgctl(struct lwp *, void *, register_t *);

int	sys_msgget(struct lwp *, void *, register_t *);

int	sys_msgsnd(struct lwp *, void *, register_t *);

int	sys_msgrcv(struct lwp *, void *, register_t *);

#else
#endif
#ifdef SYSVSHM
int	sys_shmat(struct lwp *, void *, register_t *);

int	compat_14_sys_shmctl(struct lwp *, void *, register_t *);

int	sys_shmdt(struct lwp *, void *, register_t *);

int	sys_shmget(struct lwp *, void *, register_t *);

#else
#endif
int	sys_clock_gettime(struct lwp *, void *, register_t *);

int	sys_clock_settime(struct lwp *, void *, register_t *);

int	sys_clock_getres(struct lwp *, void *, register_t *);

int	sys_nanosleep(struct lwp *, void *, register_t *);

int	sys_minherit(struct lwp *, void *, register_t *);

int	freebsd_sys_rfork(struct lwp *, void *, register_t *);

int	sys_issetugid(struct lwp *, void *, register_t *);

int	freebsd_sys_lchown(struct lwp *, void *, register_t *);

int	compat_30_sys_getdents(struct lwp *, void *, register_t *);

int	sys_lchmod(struct lwp *, void *, register_t *);

int	sys_lchown(struct lwp *, void *, register_t *);

int	sys_lutimes(struct lwp *, void *, register_t *);

int	sys___msync13(struct lwp *, void *, register_t *);

int	compat_30_sys___stat13(struct lwp *, void *, register_t *);

int	compat_30_sys___fstat13(struct lwp *, void *, register_t *);

int	compat_30_sys___lstat13(struct lwp *, void *, register_t *);

int	compat_20_sys_fhstatfs(struct lwp *, void *, register_t *);

int	compat_30_sys_fhopen(struct lwp *, void *, register_t *);

int	compat_30_sys_fhstat(struct lwp *, void *, register_t *);

int	sys_getsid(struct lwp *, void *, register_t *);

int	freebsd_sys_yield(struct lwp *, void *, register_t *);

int	sys_mlockall(struct lwp *, void *, register_t *);

int	sys_munlockall(struct lwp *, void *, register_t *);

int	sys___getcwd(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_setparam(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_getparam(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_setscheduler(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_getscheduler(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_yield(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_get_priority_max(struct lwp *, void *, register_t *);

int	freebsd_sys_sched_get_priority_min(struct lwp *, void *, register_t *);

int	freebsd_sys_utrace(struct lwp *, void *, register_t *);

int	sys___sigprocmask14(struct lwp *, void *, register_t *);

int	sys___sigsuspend14(struct lwp *, void *, register_t *);

int	freebsd_sys_sigaction4(struct lwp *, void *, register_t *);

int	sys___sigpending14(struct lwp *, void *, register_t *);

int	sys_lchflags(struct lwp *, void *, register_t *);

int	sys_uuidgen(struct lwp *, void *, register_t *);

#if defined(P1003_1B_SEMAPHORE) || !defined(_KERNEL)
int	sys__ksem_close(struct lwp *, void *, register_t *);

int	sys__ksem_post(struct lwp *, void *, register_t *);

int	sys__ksem_wait(struct lwp *, void *, register_t *);

int	sys__ksem_trywait(struct lwp *, void *, register_t *);

int	sys__ksem_unlink(struct lwp *, void *, register_t *);

int	sys__ksem_getvalue(struct lwp *, void *, register_t *);

int	sys__ksem_destroy(struct lwp *, void *, register_t *);

#else
#endif
#endif /* _FREEBSD_SYS_SYSCALLARGS_H_ */
