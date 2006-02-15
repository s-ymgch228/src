/* $NetBSD: linux32_syscall.h,v 1.2 2006/02/15 09:31:17 manu Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD
 */

#ifndef _LINUX32_SYS_SYSCALL_H_
#define	_LINUX32_SYS_SYSCALL_H_

/* syscall: "syscall" ret: "int" args: */
#define	LINUX32_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	LINUX32_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	LINUX32_SYS_fork	2

/* syscall: "netbsd32_read" ret: "netbsd32_ssize_t" args: "int" "netbsd32_voidp" "netbsd32_size_t" */
#define	LINUX32_SYS_netbsd32_read	3

/* syscall: "netbsd32_write" ret: "netbsd32_ssize_t" args: "int" "const netbsd32_voidp" "netbsd32_size_t" */
#define	LINUX32_SYS_netbsd32_write	4

/* syscall: "open" ret: "int" args: "const netbsd32_charp" "int" "int" */
#define	LINUX32_SYS_open	5

/* syscall: "close" ret: "int" args: "int" */
#define	LINUX32_SYS_close	6

/* syscall: "waitpid" ret: "int" args: "int" "netbsd32_intp" "int" */
#define	LINUX32_SYS_waitpid	7

/* syscall: "creat" ret: "int" args: "const netbsd32_charp" "int" */
#define	LINUX32_SYS_creat	8

/* syscall: "link" ret: "int" args: "const netbsd32_charp" "const netbsd32_charp" */
#define	LINUX32_SYS_link	9

/* syscall: "unlink" ret: "int" args: "const netbsd32_charp" */
#define	LINUX32_SYS_unlink	10

/* syscall: "execve" ret: "int" args: "const netbsd32_charp" "netbsd32_charpp" "netbsd32_charpp" */
#define	LINUX32_SYS_execve	11

/* syscall: "chdir" ret: "int" args: "const netbsd32_charp" */
#define	LINUX32_SYS_chdir	12

/* syscall: "time" ret: "int" args: "linux32_timep_t" */
#define	LINUX32_SYS_time	13

/* syscall: "mknod" ret: "int" args: "const netbsd32_charp" "int" "int" */
#define	LINUX32_SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "const netbsd32_charp" "int" */
#define	LINUX32_SYS_chmod	15

/* syscall: "lchown16" ret: "int" args: "const netbsd32_charp" "int" "int" */
#define	LINUX32_SYS_lchown16	16

/* syscall: "break" ret: "int" args: "netbsd32_charp" */
#define	LINUX32_SYS_break	17

				/* 18 is obsolete ostat */
/* syscall: "compat_43_netbsd32_olseek" ret: "netbsd32_long" args: "int" "netbsd32_long" "int" */
#define	LINUX32_SYS_compat_43_netbsd32_olseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	LINUX32_SYS_getpid	20

/* syscall: "linux_setuid16" ret: "int" args: "uid_t" */
#define	LINUX32_SYS_linux_setuid16	23

/* syscall: "linux_getuid16" ret: "uid_t" args: */
#define	LINUX32_SYS_linux_getuid16	24

/* syscall: "stime" ret: "int" args: "linux32_timep_t" */
#define	LINUX32_SYS_stime	25

/* syscall: "alarm" ret: "int" args: "unsigned int" */
#define	LINUX32_SYS_alarm	27

				/* 28 is obsolete ofstat */
/* syscall: "pause" ret: "int" args: */
#define	LINUX32_SYS_pause	29

/* syscall: "utime" ret: "int" args: "const netbsd32_charp" "linux32_utimbufp_t" */
#define	LINUX32_SYS_utime	30

				/* 31 is obsolete stty */
				/* 32 is obsolete gtty */
/* syscall: "access" ret: "int" args: "const netbsd32_charp" "int" */
#define	LINUX32_SYS_access	33

/* syscall: "nice" ret: "int" args: "int" */
#define	LINUX32_SYS_nice	34

				/* 35 is obsolete ftime */
/* syscall: "sync" ret: "int" args: */
#define	LINUX32_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_kill	37

/* syscall: "rename" ret: "int" args: "const netbsd32_charp" "const netbsd32_charp" */
#define	LINUX32_SYS_rename	38

/* syscall: "mkdir" ret: "int" args: "const netbsd32_charp" "int" */
#define	LINUX32_SYS_mkdir	39

/* syscall: "rmdir" ret: "int" args: "const netbsd32_charp" */
#define	LINUX32_SYS_rmdir	40

/* syscall: "dup" ret: "int" args: "u_int" */
#define	LINUX32_SYS_dup	41

/* syscall: "pipe" ret: "int" args: "netbsd32_intp" */
#define	LINUX32_SYS_pipe	42

/* syscall: "times" ret: "int" args: "linux32_tmsp_t" */
#define	LINUX32_SYS_times	43

				/* 44 is obsolete prof */
/* syscall: "brk" ret: "int" args: "netbsd32_charp" */
#define	LINUX32_SYS_brk	45

/* syscall: "linux_setgid16" ret: "int" args: "gid_t" */
#define	LINUX32_SYS_linux_setgid16	46

/* syscall: "linux_getgid16" ret: "gid_t" args: */
#define	LINUX32_SYS_linux_getgid16	47

/* syscall: "signal" ret: "int" args: "int" "linux32_handler_t" */
#define	LINUX32_SYS_signal	48

/* syscall: "linux_geteuid16" ret: "uid_t" args: */
#define	LINUX32_SYS_linux_geteuid16	49

/* syscall: "linux_getegid16" ret: "gid_t" args: */
#define	LINUX32_SYS_linux_getegid16	50

/* syscall: "acct" ret: "int" args: "netbsd32_charp" */
#define	LINUX32_SYS_acct	51

				/* 52 is obsolete phys */
				/* 53 is obsolete lock */
/* syscall: "ioctl" ret: "int" args: "int" "netbsd32_u_long" "netbsd32_charp" */
#define	LINUX32_SYS_ioctl	54

/* syscall: "fcntl" ret: "int" args: "int" "int" "netbsd32_voidp" */
#define	LINUX32_SYS_fcntl	55

				/* 56 is obsolete mpx */
/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_setpgid	57

				/* 58 is obsolete ulimit */
/* syscall: "oldolduname" ret: "int" args: "linux32_oldold_utsnamep_t" */
#define	LINUX32_SYS_oldolduname	59

/* syscall: "umask" ret: "int" args: "int" */
#define	LINUX32_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "netbsd32_charp" */
#define	LINUX32_SYS_chroot	61

/* syscall: "dup2" ret: "int" args: "u_int" "u_int" */
#define	LINUX32_SYS_dup2	63

/* syscall: "getppid" ret: "pid_t" args: */
#define	LINUX32_SYS_getppid	64

/* syscall: "getpgrp" ret: "int" args: */
#define	LINUX32_SYS_getpgrp	65

/* syscall: "setsid" ret: "int" args: */
#define	LINUX32_SYS_setsid	66

/* syscall: "setreuid16" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_setreuid16	70

/* syscall: "setregid16" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_setregid16	71

/* syscall: "compat_43_netbsd32_osethostname" ret: "int" args: "netbsd32_charp" "u_int" */
#define	LINUX32_SYS_compat_43_netbsd32_osethostname	74

/* syscall: "setrlimit" ret: "int" args: "u_int" "netbsd32_orlimitp_t" */
#define	LINUX32_SYS_setrlimit	75

/* syscall: "getrlimit" ret: "int" args: "u_int" "netbsd32_orlimitp_t" */
#define	LINUX32_SYS_getrlimit	76

/* syscall: "getrusage" ret: "int" args: "int" "netbsd32_rusagep_t" */
#define	LINUX32_SYS_getrusage	77

/* syscall: "gettimeofday" ret: "int" args: "netbsd32_timevalp_t" "netbsd32_timezonep_t" */
#define	LINUX32_SYS_gettimeofday	78

/* syscall: "settimeofday" ret: "int" args: "netbsd32_timevalp_t" "netbsd32_timezonep_t" */
#define	LINUX32_SYS_settimeofday	79

/* syscall: "getgroups16" ret: "int" args: "int" "linux32_gidp_t" */
#define	LINUX32_SYS_getgroups16	80

/* syscall: "setgroups16" ret: "int" args: "int" "linux32_gidp_t" */
#define	LINUX32_SYS_setgroups16	81

/* syscall: "oldselect" ret: "int" args: "linux32_oldselectp_t" */
#define	LINUX32_SYS_oldselect	82

/* syscall: "symlink" ret: "int" args: "netbsd32_charp" "const netbsd32_charp" */
#define	LINUX32_SYS_symlink	83

/* syscall: "lstat" ret: "int" args: "const netbsd32_charp" "netbsd32_stat43p_t" */
#define	LINUX32_SYS_lstat	84

/* syscall: "readlink" ret: "int" args: "const netbsd32_charp" "netbsd32_charp" "int" */
#define	LINUX32_SYS_readlink	85

/* syscall: "swapon" ret: "int" args: "netbsd32_charp" */
#define	LINUX32_SYS_swapon	87

/* syscall: "reboot" ret: "int" args: "int" "int" "int" "netbsd32_voidp" */
#define	LINUX32_SYS_reboot	88

/* syscall: "old_mmap" ret: "int" args: "linux32_oldmmapp" */
#define	LINUX32_SYS_old_mmap	90

/* syscall: "munmap" ret: "int" args: "netbsd32_caddr_t" "int" */
#define	LINUX32_SYS_munmap	91

/* syscall: "truncate" ret: "int" args: "const netbsd32_charp" "netbsd32_long" */
#define	LINUX32_SYS_truncate	92

/* syscall: "ftruncate" ret: "int" args: "int" "long" */
#define	LINUX32_SYS_ftruncate	93

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_fchmod	94

/* syscall: "fchown16" ret: "int" args: "int" "int" "int" */
#define	LINUX32_SYS_fchown16	95

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_getpriority	96

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	LINUX32_SYS_setpriority	97

/* syscall: "socketcall" ret: "int" args: "int" "netbsd32_voidp" */
#define	LINUX32_SYS_socketcall	102

/* syscall: "setitimer" ret: "int" args: "u_int" "netbsd32_itimervalp_t" "netbsd32_itimervalp_t" */
#define	LINUX32_SYS_setitimer	104

/* syscall: "getitimer" ret: "int" args: "u_int" "netbsd32_itimervalp_t" */
#define	LINUX32_SYS_getitimer	105

/* syscall: "wait4" ret: "int" args: "int" "netbsd32_intp" "int" "netbsd32_rusagep_t" */
#define	LINUX32_SYS_wait4	114

/* syscall: "swapoff" ret: "int" args: "const netbsd32_charp" */
#define	LINUX32_SYS_swapoff	115

/* syscall: "fsync" ret: "int" args: "int" */
#define	LINUX32_SYS_fsync	118

/* syscall: "sigreturn" ret: "int" args: "linux32_sigcontextp_t" */
#define	LINUX32_SYS_sigreturn	119

/* syscall: "clone" ret: "int" args: "int" "netbsd32_voidp" */
#define	LINUX32_SYS_clone	120

/* syscall: "uname" ret: "int" args: "linux32_utsnamep" */
#define	LINUX32_SYS_uname	122

/* syscall: "mprotect" ret: "int" args: "netbsd32_voidp" "netbsd32_size_t" "int" */
#define	LINUX32_SYS_mprotect	125

/* syscall: "fchdir" ret: "int" args: "int" */
#define	LINUX32_SYS_fchdir	133

/* syscall: "llseek" ret: "int" args: "int" "u_int32_t" "u_int32_t" "netbsd32_caddr_t" "int" */
#define	LINUX32_SYS_llseek	140

/* syscall: "getdents" ret: "int" args: "int" "linux32_direntp_t" "unsigned int" */
#define	LINUX32_SYS_getdents	141

/* syscall: "select" ret: "int" args: "int" "netbsd32_fd_setp_t" "netbsd32_fd_setp_t" "netbsd32_fd_setp_t" "netbsd32_timevalp_t" */
#define	LINUX32_SYS_select	142

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	LINUX32_SYS_flock	143

/* syscall: "readv" ret: "int" args: "int" "const netbsd32_iovecp_t" "u_int" */
#define	LINUX32_SYS_readv	145

/* syscall: "writev" ret: "netbsd32_ssize_t" args: "int" "const netbsd32_iovecp_t" "int" */
#define	LINUX32_SYS_writev	146

/* syscall: "getsid" ret: "pid_t" args: "pid_t" */
#define	LINUX32_SYS_getsid	147

/* syscall: "__sysctl" ret: "int" args: "linux32___sysctlp_t" */
#define	LINUX32_SYS___sysctl	149

/* syscall: "mlockall" ret: "int" args: "int" */
#define	LINUX32_SYS_mlockall	152

/* syscall: "munlockall" ret: "int" args: */
#define	LINUX32_SYS_munlockall	153

/* syscall: "sched_setscheduler" ret: "int" args: "pid_t" "int" "const linux32_sched_paramp_t" */
#define	LINUX32_SYS_sched_setscheduler	156

/* syscall: "nanosleep" ret: "int" args: "const netbsd32_timespecp_t rqtp netbsd32_timespecp_t" */
#define	LINUX32_SYS_nanosleep	162

/* syscall: "setresuid16" ret: "int" args: "uid_t" "uid_t" "uid_t" */
#define	LINUX32_SYS_setresuid16	164

/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	LINUX32_SYS_poll	168

/* syscall: "setresgid16" ret: "int" args: "gid_t" "gid_t" "gid_t" */
#define	LINUX32_SYS_setresgid16	170

/* syscall: "rt_sigreturn" ret: "int" args: "linux32_ucontextp_t" */
#define	LINUX32_SYS_rt_sigreturn	173

/* syscall: "rt_sigaction" ret: "int" args: "int" "const linux32_sigactionp_t" "linux32_sigactionp_t" "netbsd32_size_t" */
#define	LINUX32_SYS_rt_sigaction	174

/* syscall: "rt_sigprocmask" ret: "int" args: "int" "const linux32_sigsetp_t" "linux32_sigsetp_t" "netbsd32_size_t" */
#define	LINUX32_SYS_rt_sigprocmask	175

/* syscall: "rt_sigsuspend" ret: "int" args: "linux32_sigsetp_t" "netbsd32_size_t" */
#define	LINUX32_SYS_rt_sigsuspend	179

/* syscall: "netbsd32___getcwd" ret: "int" args: "netbsd32_charp" "netbsd32_size_t" */
#define	LINUX32_SYS_netbsd32___getcwd	183

/* syscall: "__vfork14" ret: "int" args: */
#define	LINUX32_SYS___vfork14	190

/* syscall: "ugetrlimit" ret: "int" args: "int" "netbsd32_orlimitp_t" */
#define	LINUX32_SYS_ugetrlimit	191

/* syscall: "stat64" ret: "int" args: "const netbsd32_charp" "linux32_stat64p" */
#define	LINUX32_SYS_stat64	195

/* syscall: "lstat64" ret: "int" args: "const netbsd32_charp" "linux32_stat64p" */
#define	LINUX32_SYS_lstat64	196

/* syscall: "fstat64" ret: "int" args: "int" "linux32_stat64p" */
#define	LINUX32_SYS_fstat64	197

/* syscall: "getuid" ret: "uid_t" args: */
#define	LINUX32_SYS_getuid	199

/* syscall: "getgid" ret: "gid_t" args: */
#define	LINUX32_SYS_getgid	200

/* syscall: "geteuid" ret: "uid_t" args: */
#define	LINUX32_SYS_geteuid	201

/* syscall: "getegid" ret: "gid_t" args: */
#define	LINUX32_SYS_getegid	202

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	LINUX32_SYS_setreuid	203

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	LINUX32_SYS_setregid	204

/* syscall: "__posix_fchown" ret: "int" args: "int" "uid_t" "gid_t" */
#define	LINUX32_SYS___posix_fchown	207

/* syscall: "setresuid" ret: "int" args: "uid_t" "uid_t" "uid_t" */
#define	LINUX32_SYS_setresuid	208

/* syscall: "setresgid" ret: "int" args: "gid_t" "gid_t" "gid_t" */
#define	LINUX32_SYS_setresgid	210

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	LINUX32_SYS_setuid	213

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	LINUX32_SYS_setgid	214

/* syscall: "getdents64" ret: "int" args: "int" "linux32_dirent64p_t" "unsigned int" */
#define	LINUX32_SYS_getdents64	220

/* syscall: "fcntl64" ret: "int" args: "int" "int" "netbsd32_voidp" */
#define	LINUX32_SYS_fcntl64	221

/* syscall: "exit_group" ret: "int" args: "int" */
#define	LINUX32_SYS_exit_group	252

#define	LINUX32_SYS_MAXSYSCALL	284
#define	LINUX32_SYS_NSYSENT	512
#endif /* _LINUX32_SYS_SYSCALL_H_ */
