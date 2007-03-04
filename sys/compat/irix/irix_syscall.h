/* $NetBSD: irix_syscall.h,v 1.56 2007/03/04 06:01:18 christos Exp $ */

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.48 2005/02/26 23:10:19 perry Exp
 */

/* syscall: "syscall" ret: "int" args: */
#define	IRIX_SYS_syscall	0

/* syscall: "exit" ret: "int" args: "int" */
#define	IRIX_SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	IRIX_SYS_fork	2

/* syscall: "read" ret: "int" args: "int" "char *" "u_int" */
#define	IRIX_SYS_read	3

/* syscall: "write" ret: "int" args: "int" "char *" "u_int" */
#define	IRIX_SYS_write	4

/* syscall: "open" ret: "int" args: "const char *" "int" "int" */
#define	IRIX_SYS_open	5

/* syscall: "close" ret: "int" args: "int" */
#define	IRIX_SYS_close	6

				/* 7 is obsolete wait */
/* syscall: "creat" ret: "int" args: "const char *" "int" */
#define	IRIX_SYS_creat	8

/* syscall: "link" ret: "int" args: "char *" "char *" */
#define	IRIX_SYS_link	9

/* syscall: "unlink" ret: "int" args: "char *" */
#define	IRIX_SYS_unlink	10

/* syscall: "execv" ret: "int" args: "const char *" "char **" */
#define	IRIX_SYS_execv	11

/* syscall: "chdir" ret: "int" args: "char *" */
#define	IRIX_SYS_chdir	12

/* syscall: "time" ret: "int" args: "svr4_time_t *" */
#define	IRIX_SYS_time	13

				/* 14 is obsolete mknod */
/* syscall: "chmod" ret: "int" args: "char *" "int" */
#define	IRIX_SYS_chmod	15

/* syscall: "chown" ret: "int" args: "char *" "int" "int" */
#define	IRIX_SYS_chown	16

/* syscall: "break" ret: "int" args: "void *" */
#define	IRIX_SYS_break	17

				/* 18 is obsolete stat */
/* syscall: "lseek" ret: "long" args: "int" "long" "int" */
#define	IRIX_SYS_lseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	IRIX_SYS_getpid	20

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	IRIX_SYS_setuid	23

/* syscall: "getuid_with_euid" ret: "uid_t" args: */
#define	IRIX_SYS_getuid_with_euid	24

/* syscall: "alarm" ret: "int" args: "unsigned" */
#define	IRIX_SYS_alarm	27

/* syscall: "fstat" ret: "int" args: "int" "struct svr4_stat *" */
#define	IRIX_SYS_fstat	28

/* syscall: "pause" ret: "int" args: */
#define	IRIX_SYS_pause	29

/* syscall: "utime" ret: "int" args: "const char *" "struct svr4_utimbuf *" */
#define	IRIX_SYS_utime	30

/* syscall: "access" ret: "int" args: "const char *" "int" */
#define	IRIX_SYS_access	33

/* syscall: "nice" ret: "int" args: "int" */
#define	IRIX_SYS_nice	34

/* syscall: "sync" ret: "int" args: */
#define	IRIX_SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	IRIX_SYS_kill	37

/* syscall: "pgrpsys" ret: "int" args: "int" "int" "int" */
#define	IRIX_SYS_pgrpsys	39

/* syscall: "syssgi" ret: "ptrdiff_t" args: "int" "void *" "void *" "void *" "void *" "void *" */
#define	IRIX_SYS_syssgi	40

/* syscall: "dup" ret: "int" args: "u_int" */
#define	IRIX_SYS_dup	41

/* syscall: "pipe" ret: "int" args: */
#define	IRIX_SYS_pipe	42

/* syscall: "times" ret: "int" args: "struct tms *" */
#define	IRIX_SYS_times	43

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	IRIX_SYS_setgid	46

/* syscall: "getgid_with_egid" ret: "gid_t" args: */
#define	IRIX_SYS_getgid_with_egid	47

				/* 48 is obsolete ssig */
#ifdef SYSVMSG
/* syscall: "msgsys" ret: "int" args: "int" "int" "int" "int" "int" */
#define	IRIX_SYS_msgsys	49

#else
#endif
#ifdef SYSVSHM
/* syscall: "shmsys" ret: "int" args: "int" "int" "int" "int" */
#define	IRIX_SYS_shmsys	52

#else
#endif
#ifdef SYSVSEM
/* syscall: "semsys" ret: "int" args: "int" "int" "int" "int" "int" */
#define	IRIX_SYS_semsys	53

#else
#endif
/* syscall: "ioctl" ret: "int" args: "int" "u_long" "void *" */
#define	IRIX_SYS_ioctl	54

/* syscall: "sysmp" ret: "int" args: "int" "void *" "void *" "void *" "void *" */
#define	IRIX_SYS_sysmp	56

/* syscall: "utssys" ret: "int" args: "void *" "void *" "int" "void *" */
#define	IRIX_SYS_utssys	57

/* syscall: "execve" ret: "int" args: "const char *" "char **" "char **" */
#define	IRIX_SYS_execve	59

/* syscall: "umask" ret: "int" args: "int" */
#define	IRIX_SYS_umask	60

/* syscall: "chroot" ret: "int" args: "char *" */
#define	IRIX_SYS_chroot	61

/* syscall: "fcntl" ret: "int" args: "int" "int" "char *" */
#define	IRIX_SYS_fcntl	62

/* syscall: "ulimit" ret: "long" args: "int" "long" */
#define	IRIX_SYS_ulimit	63

				/* 70 is obsolete advfs */
				/* 71 is obsolete unadvfs */
				/* 72 is obsolete rmount */
				/* 73 is obsolete rumount */
				/* 74 is obsolete rfstart */
/* syscall: "getrlimit64" ret: "int" args: "int" "struct irix_rlimit64 *" */
#define	IRIX_SYS_getrlimit64	75

/* syscall: "setrlimit64" ret: "int" args: "int" "const struct irix_rlimit64 *" */
#define	IRIX_SYS_setrlimit64	76

/* syscall: "nanosleep" ret: "int" args: "const struct timespec *" "struct timespec *" */
#define	IRIX_SYS_nanosleep	77

/* syscall: "lseek64" ret: "irix_off64_t" args: "int" "int" "irix_off64_t" "int" "int" "int" "int" */
#define	IRIX_SYS_lseek64	78

/* syscall: "rmdir" ret: "int" args: "char *" */
#define	IRIX_SYS_rmdir	79

/* syscall: "mkdir" ret: "int" args: "char *" "int" */
#define	IRIX_SYS_mkdir	80

/* syscall: "getdents" ret: "int" args: "int" "irix_dirent_t *" "int" */
#define	IRIX_SYS_getdents	81

/* syscall: "sginap" ret: "long" args: "long" */
#define	IRIX_SYS_sginap	82

/* syscall: "getmsg" ret: "int" args: "int" "struct svr4_strbuf *" "struct svr4_strbuf *" "int *" */
#define	IRIX_SYS_getmsg	85

/* syscall: "putmsg" ret: "int" args: "int" "struct svr4_strbuf *" "struct svr4_strbuf *" "int" */
#define	IRIX_SYS_putmsg	86

/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	IRIX_SYS_poll	87

/* syscall: "sigreturn" ret: "int" args: "struct irix_sigcontext *" "struct irix_ucontext *" "int" */
#define	IRIX_SYS_sigreturn	88

/* syscall: "accept" ret: "int" args: "int" "struct sockaddr *" "int *" */
#define	IRIX_SYS_accept	89

/* syscall: "bind" ret: "int" args: "int" "const struct sockaddr *" "int" */
#define	IRIX_SYS_bind	90

/* syscall: "connect" ret: "int" args: "int" "const struct sockaddr *" "int" */
#define	IRIX_SYS_connect	91

/* syscall: "gethostid" ret: "int32_t" args: */
#define	IRIX_SYS_gethostid	92

/* syscall: "getpeername" ret: "int" args: "int" "struct sockaddr *" "int *" */
#define	IRIX_SYS_getpeername	93

/* syscall: "getsockname" ret: "int" args: "int" "struct sockaddr *" "int *" */
#define	IRIX_SYS_getsockname	94

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "void *" "int *" */
#define	IRIX_SYS_getsockopt	95

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	IRIX_SYS_listen	96

/* syscall: "recv" ret: "int" args: "int" "void *" "int" "int" */
#define	IRIX_SYS_recv	97

/* syscall: "recvfrom" ret: "ssize_t" args: "int" "void *" "size_t" "int" "struct sockaddr *" "int *" */
#define	IRIX_SYS_recvfrom	98

/* syscall: "recvmsg" ret: "ssize_t" args: "int" "struct msghdr *" "int" */
#define	IRIX_SYS_recvmsg	99

/* syscall: "select" ret: "int" args: "int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	IRIX_SYS_select	100

/* syscall: "send" ret: "int" args: "int" "void *" "int" "int" */
#define	IRIX_SYS_send	101

/* syscall: "sendmsg" ret: "ssize_t" args: "int" "const struct msghdr *" "int" */
#define	IRIX_SYS_sendmsg	102

/* syscall: "sendto" ret: "ssize_t" args: "int" "const void *" "size_t" "int" "const struct sockaddr *" "int" */
#define	IRIX_SYS_sendto	103

/* syscall: "sethostid" ret: "int" args: "int32_t" */
#define	IRIX_SYS_sethostid	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "const void *" "int" */
#define	IRIX_SYS_setsockopt	105

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	IRIX_SYS_shutdown	106

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	IRIX_SYS_socket	107

/* syscall: "gethostname" ret: "int" args: "char *" "u_int" */
#define	IRIX_SYS_gethostname	108

/* syscall: "sethostname" ret: "int" args: "const char *" "u_int" */
#define	IRIX_SYS_sethostname	109

/* syscall: "getdomainname" ret: "int" args: "char *" "int" */
#define	IRIX_SYS_getdomainname	110

/* syscall: "setdomainname" ret: "int" args: "char *" "int" */
#define	IRIX_SYS_setdomainname	111

/* syscall: "truncate" ret: "int" args: "const char *" "long" */
#define	IRIX_SYS_truncate	112

/* syscall: "ftruncate" ret: "int" args: "int" "long" */
#define	IRIX_SYS_ftruncate	113

/* syscall: "rename" ret: "int" args: "const char *" "const char *" */
#define	IRIX_SYS_rename	114

/* syscall: "symlink" ret: "int" args: "const char *" "const char *" */
#define	IRIX_SYS_symlink	115

/* syscall: "readlink" ret: "int" args: "const char *" "char *" "size_t" */
#define	IRIX_SYS_readlink	116

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	IRIX_SYS_setregid	123

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	IRIX_SYS_setreuid	124

/* syscall: "getitimer" ret: "int" args: "u_int" "struct itimerval *" */
#define	IRIX_SYS_getitimer	125

/* syscall: "setitimer" ret: "int" args: "u_int" "struct itimerval *" "struct itimerval *" */
#define	IRIX_SYS_setitimer	126

/* syscall: "adjtime" ret: "int" args: "struct timeval *" "struct timeval *" */
#define	IRIX_SYS_adjtime	127

/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" */
#define	IRIX_SYS_gettimeofday	128

/* syscall: "sproc" ret: "irix_pid_t" args: "void *" "unsigned int" "void *" */
#define	IRIX_SYS_sproc	129

/* syscall: "prctl" ret: "ptrdiff_t" args: "unsigned int" "void *" */
#define	IRIX_SYS_prctl	130

/* syscall: "procblk" ret: "int" args: "int" "pid_t" "int" */
#define	IRIX_SYS_procblk	131

/* syscall: "sprocsp" ret: "irix_pid_t" args: "void *" "unsigned int" "void *" "void *" "irix_size_t" */
#define	IRIX_SYS_sprocsp	132

/* syscall: "mmap" ret: "void *" args: "void *" "irix_size_t" "int" "int" "int" "irix_off_t" */
#define	IRIX_SYS_mmap	134

/* syscall: "munmap" ret: "int" args: "void *" "int" */
#define	IRIX_SYS_munmap	135

/* syscall: "mprotect" ret: "int" args: "void *" "int" "int" */
#define	IRIX_SYS_mprotect	136

/* syscall: "__msync13" ret: "int" args: "void *" "size_t" "int" */
#define	IRIX_SYS___msync13	137

/* syscall: "getpgrp" ret: "int" args: */
#define	IRIX_SYS_getpgrp	143

/* syscall: "setpgrp" ret: "int" args: "int" "int" */
#define	IRIX_SYS_setpgrp	144

/* syscall: "fsync" ret: "int" args: "int" */
#define	IRIX_SYS_fsync	146

/* syscall: "fchdir" ret: "int" args: "int" */
#define	IRIX_SYS_fchdir	147

/* syscall: "getrlimit" ret: "int" args: "int" "struct irix_rlimit *" */
#define	IRIX_SYS_getrlimit	148

/* syscall: "setrlimit" ret: "int" args: "int" "const struct irix_rlimit *" */
#define	IRIX_SYS_setrlimit	149

/* syscall: "fchown" ret: "int" args: "int" "int" "int" */
#define	IRIX_SYS_fchown	152

/* syscall: "fchmod" ret: "int" args: "int" "int" */
#define	IRIX_SYS_fchmod	153

/* syscall: "systeminfo" ret: "long" args: "int" "char *" "long" */
#define	IRIX_SYS_systeminfo	156

/* syscall: "uname" ret: "int" args: "struct irix_utsname *" */
#define	IRIX_SYS_uname	157

/* syscall: "xstat" ret: "int" args: "const int" "const char *" "struct stat *" */
#define	IRIX_SYS_xstat	158

/* syscall: "lxstat" ret: "int" args: "const int" "const char *" "struct stat *" */
#define	IRIX_SYS_lxstat	159

/* syscall: "fxstat" ret: "int" args: "const int" "const int" "struct stat *" */
#define	IRIX_SYS_fxstat	160

/* syscall: "sigaction" ret: "int" args: "int" "const struct svr4_sigaction *" "struct svr4_sigaction *" "void *" */
#define	IRIX_SYS_sigaction	162

/* syscall: "sigpending" ret: "int" args: "int" "svr4_sigset_t *" */
#define	IRIX_SYS_sigpending	163

/* syscall: "sigprocmask" ret: "int" args: "int" "const irix_sigset_t *" "irix_sigset_t *" */
#define	IRIX_SYS_sigprocmask	164

/* syscall: "sigsuspend" ret: "int" args: "const svr4_sigset_t *" */
#define	IRIX_SYS_sigsuspend	165

/* syscall: "swapctl" ret: "int" args: "int" "void *" */
#define	IRIX_SYS_swapctl	167

/* syscall: "getcontext" ret: "int" args: "irix_ucontext_t *" */
#define	IRIX_SYS_getcontext	168

/* syscall: "setcontext" ret: "int" args: "const irix_ucontext_t *" */
#define	IRIX_SYS_setcontext	169

/* syscall: "waitsys" ret: "int" args: "int" "int" "struct irix_irix5_siginfo *" "int" "struct rusage *" */
#define	IRIX_SYS_waitsys	170

/* syscall: "statvfs" ret: "int" args: "const char *" "struct svr4_statvfs *" */
#define	IRIX_SYS_statvfs	174

/* syscall: "fstatvfs" ret: "int" args: "int" "struct svr4_statvfs *" */
#define	IRIX_SYS_fstatvfs	175

/* syscall: "readv" ret: "ssize_t" args: "int" "const struct iovec *" "int" */
#define	IRIX_SYS_readv	181

/* syscall: "writev" ret: "ssize_t" args: "int" "const struct iovec *" "int" */
#define	IRIX_SYS_writev	182

/* syscall: "truncate64" ret: "int" args: "const char *" "int" "off_t" */
#define	IRIX_SYS_truncate64	183

/* syscall: "ftruncate64" ret: "int" args: "int" "int" "off_t" */
#define	IRIX_SYS_ftruncate64	184

/* syscall: "mmap64" ret: "void *" args: "void *" "irix_size_t" "int" "int" "int" "int" "irix_off_t" */
#define	IRIX_SYS_mmap64	185

/* syscall: "pread" ret: "ssize_t" args: "int" "void *" "size_t" "svr4_off_t" */
#define	IRIX_SYS_pread	187

/* syscall: "pwrite" ret: "ssize_t" args: "int" "const void *" "size_t" "svr4_off_t" */
#define	IRIX_SYS_pwrite	188

/* syscall: "getmountid" ret: "int" args: "const char *" "irix_mountid_t *" */
#define	IRIX_SYS_getmountid	203

/* syscall: "getdents64" ret: "int" args: "int" "irix_dirent64_t *" "int" */
#define	IRIX_SYS_getdents64	205

/* syscall: "ngetdents" ret: "int" args: "int" "irix_dirent_t *" "unsigned short" "int *" */
#define	IRIX_SYS_ngetdents	207

/* syscall: "ngetdents64" ret: "int" args: "int" "irix_dirent64_t *" "unsigned short" "int *" */
#define	IRIX_SYS_ngetdents64	208

/* syscall: "pidsprocsp" ret: "irix_pid_t" args: "void *" "unsigned int" "void *" "void *" "irix_size_t" "irix_pid_t" */
#define	IRIX_SYS_pidsprocsp	210

/* syscall: "usync_cntl" ret: "int" args: "int" "void *" */
#define	IRIX_SYS_usync_cntl	223

#define	IRIX_SYS_MAXSYSCALL	236
#define	IRIX_SYS_NSYSENT	256
