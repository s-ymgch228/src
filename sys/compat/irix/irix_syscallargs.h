/* $NetBSD: irix_syscallargs.h,v 1.20 2002/02/03 17:39:54 manu Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.18 2002/02/03 01:21:48 manu Exp 
 */

#ifndef _IRIX_SYS__SYSCALLARGS_H_
#define	_IRIX_SYS__SYSCALLARGS_H_

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

struct svr4_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};

struct svr4_sys_creat_args {
	syscallarg(const char *) path;
	syscallarg(int) mode;
};

struct svr4_sys_execv_args {
	syscallarg(const char *) path;
	syscallarg(char **) argp;
};

struct svr4_sys_time_args {
	syscallarg(svr4_time_t *) t;
};

struct svr4_sys_break_args {
	syscallarg(caddr_t) nsize;
};

struct svr4_sys_alarm_args {
	syscallarg(unsigned) sec;
};

struct svr4_sys_fstat_args {
	syscallarg(int) fd;
	syscallarg(struct svr4_stat *) sb;
};

struct svr4_sys_utime_args {
	syscallarg(const char *) path;
	syscallarg(struct svr4_utimbuf *) ubuf;
};

struct svr4_sys_access_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
};

struct svr4_sys_nice_args {
	syscallarg(int) prio;
};

struct svr4_sys_kill_args {
	syscallarg(int) pid;
	syscallarg(int) signum;
};

struct svr4_sys_pgrpsys_args {
	syscallarg(int) cmd;
	syscallarg(int) pid;
	syscallarg(int) pgid;
};

struct irix_sys_syssgi_args {
	syscallarg(int) request;
	syscallarg(void *) arg1;
	syscallarg(void *) arg2;
	syscallarg(void *) arg3;
	syscallarg(void *) arg4;
	syscallarg(void *) arg5;
};

struct svr4_sys_times_args {
	syscallarg(struct tms *) tp;
};

struct svr4_sys_msgsys_args {
	syscallarg(int) what;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
	syscallarg(int) a5;
};

struct svr4_sys_shmsys_args {
	syscallarg(int) what;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
};

struct svr4_sys_semsys_args {
	syscallarg(int) what;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
	syscallarg(int) a5;
};

struct svr4_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(caddr_t) data;
};

struct irix_sys_sysmp_args {
	syscallarg(int) cmd;
	syscallarg(void *) arg1;
	syscallarg(void *) arg2;
	syscallarg(void *) arg3;
	syscallarg(void *) arg4;
};

struct svr4_sys_utssys_args {
	syscallarg(void *) a1;
	syscallarg(void *) a2;
	syscallarg(int) sel;
	syscallarg(void *) a3;
};

struct svr4_sys_execve_args {
	syscallarg(const char *) path;
	syscallarg(char **) argp;
	syscallarg(char **) envp;
};

struct svr4_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(char *) arg;
};

struct svr4_sys_ulimit_args {
	syscallarg(int) cmd;
	syscallarg(long) newlimit;
};

struct irix_sys_lseek64_args {
	syscallarg(int) fd;
	syscallarg(int) pad1;
	syscallarg(irix_off64_t) offset;
	syscallarg(int) whence;
	syscallarg(int) pad2;
	syscallarg(int) pad3;
	syscallarg(int) pad4;
};

struct irix_sys_getdents_args {
	syscallarg(int) fildes;
	syscallarg(irix_dirent_t *) buf;
	syscallarg(int) nbytes;
};

struct irix_sys_sginap_args {
	syscallarg(long) ticks;
};

struct svr4_sys_getmsg_args {
	syscallarg(int) fd;
	syscallarg(struct svr4_strbuf *) ctl;
	syscallarg(struct svr4_strbuf *) dat;
	syscallarg(int *) flags;
};

struct svr4_sys_putmsg_args {
	syscallarg(int) fd;
	syscallarg(struct svr4_strbuf *) ctl;
	syscallarg(struct svr4_strbuf *) dat;
	syscallarg(int) flags;
};

struct irix_sys_sigreturn_args {
	syscallarg(struct irix_sigframe *) sf;
};

struct svr4_sys_socket_args {
	syscallarg(int) domain;
	syscallarg(int) type;
	syscallarg(int) protocol;
};

struct svr4_sys_gettimeofday_args {
	syscallarg(struct timeval *) tp;
};

struct irix_sys_prctl_args {
	syscallarg(unsigned int) option;
	syscallarg(void *) arg1;
};

struct svr4_sys_mmap_args {
	syscallarg(void *) addr;
	syscallarg(svr4_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(svr4_off_t) pos;
};

struct irix_sys_xstat_args {
	syscallarg(const int) version;
	syscallarg(const char *) path;
	syscallarg(struct stat *) buf;
};

struct irix_sys_lxstat_args {
	syscallarg(const int) version;
	syscallarg(const char *) path;
	syscallarg(struct stat *) buf;
};

struct irix_sys_fxstat_args {
	syscallarg(const int) version;
	syscallarg(const int) fd;
	syscallarg(struct stat *) buf;
};

struct svr4_sys_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const struct svr4_sigaction *) nsa;
	syscallarg(struct svr4_sigaction *) osa;
};

struct svr4_sys_sigpending_args {
	syscallarg(int) what;
	syscallarg(svr4_sigset_t *) set;
};

struct svr4_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const svr4_sigset_t *) set;
	syscallarg(svr4_sigset_t *) oset;
};

struct svr4_sys_sigsuspend_args {
	syscallarg(const svr4_sigset_t *) set;
};

struct irix_sys_getcontext_args {
	syscallarg(irix_ucontext_t *) ucp;
};

struct irix_sys_setcontext_args {
	syscallarg(const irix_ucontext_t *) ucp;
};

struct irix_sys_waitsys_args {
	syscallarg(int) type;
	syscallarg(int) pid;
	syscallarg(struct irix_irix5_siginfo *) info;
	syscallarg(int) options;
	syscallarg(struct rusage *) ru;
};

struct irix_sys_getmountid_args {
	syscallarg(const char *) path;
	syscallarg(irix_mountid_t *) buf;
};

struct irix_sys_ngetdents_args {
	syscallarg(int) fildes;
	syscallarg(irix_dirent_t *) buf;
	syscallarg(unsigned short) nbyte;
	syscallarg(int *) eof;
};

/*
 * System call prototypes.
 */

int	sys_nosys(struct proc *, void *, register_t *);
int	sys_exit(struct proc *, void *, register_t *);
int	sys_fork(struct proc *, void *, register_t *);
int	sys_read(struct proc *, void *, register_t *);
int	sys_write(struct proc *, void *, register_t *);
int	svr4_sys_open(struct proc *, void *, register_t *);
int	sys_close(struct proc *, void *, register_t *);
int	svr4_sys_creat(struct proc *, void *, register_t *);
int	sys_link(struct proc *, void *, register_t *);
int	sys_unlink(struct proc *, void *, register_t *);
int	svr4_sys_execv(struct proc *, void *, register_t *);
int	sys_chdir(struct proc *, void *, register_t *);
int	svr4_sys_time(struct proc *, void *, register_t *);
int	sys_chmod(struct proc *, void *, register_t *);
int	sys___posix_chown(struct proc *, void *, register_t *);
int	svr4_sys_break(struct proc *, void *, register_t *);
int	compat_43_sys_lseek(struct proc *, void *, register_t *);
int	sys_getpid(struct proc *, void *, register_t *);
int	sys_setuid(struct proc *, void *, register_t *);
int	sys_getuid_with_euid(struct proc *, void *, register_t *);
int	svr4_sys_alarm(struct proc *, void *, register_t *);
int	svr4_sys_fstat(struct proc *, void *, register_t *);
int	svr4_sys_pause(struct proc *, void *, register_t *);
int	svr4_sys_utime(struct proc *, void *, register_t *);
int	svr4_sys_access(struct proc *, void *, register_t *);
int	svr4_sys_nice(struct proc *, void *, register_t *);
int	sys_sync(struct proc *, void *, register_t *);
int	svr4_sys_kill(struct proc *, void *, register_t *);
int	svr4_sys_pgrpsys(struct proc *, void *, register_t *);
int	irix_sys_syssgi(struct proc *, void *, register_t *);
int	sys_dup(struct proc *, void *, register_t *);
int	sys_pipe(struct proc *, void *, register_t *);
int	svr4_sys_times(struct proc *, void *, register_t *);
int	sys_setgid(struct proc *, void *, register_t *);
int	sys_getgid_with_egid(struct proc *, void *, register_t *);
#ifdef SYSVMSG
int	svr4_sys_msgsys(struct proc *, void *, register_t *);
#else
#endif
#ifdef SYSVSHM
int	svr4_sys_shmsys(struct proc *, void *, register_t *);
#else
#endif
#ifdef SYSVSEM
int	svr4_sys_semsys(struct proc *, void *, register_t *);
#else
#endif
int	svr4_sys_ioctl(struct proc *, void *, register_t *);
int	irix_sys_sysmp(struct proc *, void *, register_t *);
int	svr4_sys_utssys(struct proc *, void *, register_t *);
int	svr4_sys_execve(struct proc *, void *, register_t *);
int	sys_umask(struct proc *, void *, register_t *);
int	sys_chroot(struct proc *, void *, register_t *);
int	svr4_sys_fcntl(struct proc *, void *, register_t *);
int	svr4_sys_ulimit(struct proc *, void *, register_t *);
int	irix_sys_lseek64(struct proc *, void *, register_t *);
int	sys_rmdir(struct proc *, void *, register_t *);
int	sys_mkdir(struct proc *, void *, register_t *);
int	irix_sys_getdents(struct proc *, void *, register_t *);
int	irix_sys_sginap(struct proc *, void *, register_t *);
int	svr4_sys_getmsg(struct proc *, void *, register_t *);
int	svr4_sys_putmsg(struct proc *, void *, register_t *);
int	sys_poll(struct proc *, void *, register_t *);
int	irix_sys_sigreturn(struct proc *, void *, register_t *);
int	compat_43_sys_accept(struct proc *, void *, register_t *);
int	sys_bind(struct proc *, void *, register_t *);
int	sys_connect(struct proc *, void *, register_t *);
int	compat_43_sys_getpeername(struct proc *, void *, register_t *);
int	compat_43_sys_getsockname(struct proc *, void *, register_t *);
int	sys_getsockopt(struct proc *, void *, register_t *);
int	sys_listen(struct proc *, void *, register_t *);
int	compat_43_sys_recv(struct proc *, void *, register_t *);
int	compat_43_sys_recvfrom(struct proc *, void *, register_t *);
int	compat_43_sys_recvmsg(struct proc *, void *, register_t *);
int	sys_select(struct proc *, void *, register_t *);
int	compat_43_sys_send(struct proc *, void *, register_t *);
int	compat_43_sys_sendmsg(struct proc *, void *, register_t *);
int	sys_sendto(struct proc *, void *, register_t *);
int	sys_setsockopt(struct proc *, void *, register_t *);
int	sys_shutdown(struct proc *, void *, register_t *);
int	svr4_sys_socket(struct proc *, void *, register_t *);
int	compat_43_sys_gethostname(struct proc *, void *, register_t *);
int	compat_43_sys_sethostname(struct proc *, void *, register_t *);
int	compat_09_sys_getdomainname(struct proc *, void *, register_t *);
int	compat_09_sys_setdomainname(struct proc *, void *, register_t *);
int	sys_rename(struct proc *, void *, register_t *);
int	sys_symlink(struct proc *, void *, register_t *);
int	sys_readlink(struct proc *, void *, register_t *);
int	sys_getitimer(struct proc *, void *, register_t *);
int	sys_setitimer(struct proc *, void *, register_t *);
int	sys_adjtime(struct proc *, void *, register_t *);
int	svr4_sys_gettimeofday(struct proc *, void *, register_t *);
int	irix_sys_prctl(struct proc *, void *, register_t *);
int	svr4_sys_mmap(struct proc *, void *, register_t *);
int	irix_sys_xstat(struct proc *, void *, register_t *);
int	irix_sys_lxstat(struct proc *, void *, register_t *);
int	irix_sys_fxstat(struct proc *, void *, register_t *);
int	svr4_sys_sigaction(struct proc *, void *, register_t *);
int	svr4_sys_sigpending(struct proc *, void *, register_t *);
int	svr4_sys_sigprocmask(struct proc *, void *, register_t *);
int	svr4_sys_sigsuspend(struct proc *, void *, register_t *);
int	irix_sys_getcontext(struct proc *, void *, register_t *);
int	irix_sys_setcontext(struct proc *, void *, register_t *);
int	irix_sys_waitsys(struct proc *, void *, register_t *);
int	sys_readv(struct proc *, void *, register_t *);
int	sys_writev(struct proc *, void *, register_t *);
int	irix_sys_getmountid(struct proc *, void *, register_t *);
int	irix_sys_ngetdents(struct proc *, void *, register_t *);
#endif /* _IRIX_SYS__SYSCALLARGS_H_ */
