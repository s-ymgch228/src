/* $NetBSD: aoutm68k_syscalls.c,v 1.4 2001/01/27 07:24:40 thorpej Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.3 2000/12/09 05:27:31 mycroft Exp 
 */

#if defined(_KERNEL) && !defined(_LKM)
#if defined(_KERNEL) && !defined(_LKM)
#include "opt_ktrace.h"
#include "opt_nfsserver.h"
#include "opt_ntp.h"
#include "opt_compat_netbsd.h"
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
#include <compat/aoutm68k/aoutm68k_syscallargs.h>
#endif /* _KERNEL && ! _LKM */

const char *const aoutm68k_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"open",			/* 5 = open */
	"close",			/* 6 = close */
	"wait4",			/* 7 = wait4 */
#ifdef COMPAT_43
	"ocreat",			/* 8 = ocreat */
#else
	"#8 (excluded compat_43_sys_creat)",		/* 8 = excluded compat_43_sys_creat */
#endif
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"#11 (obsolete execv)",		/* 11 = obsolete execv */
	"chdir",			/* 12 = chdir */
	"fchdir",			/* 13 = fchdir */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"chown",			/* 16 = chown */
	"break",			/* 17 = break */
	"getfsstat",			/* 18 = getfsstat */
#ifdef COMPAT_43
	"olseek",			/* 19 = olseek */
#else
	"#19 (excluded compat_43_sys_lseek)",		/* 19 = excluded compat_43_sys_lseek */
#endif
	"getpid",			/* 20 = getpid */
	"mount",			/* 21 = mount */
	"unmount",			/* 22 = unmount */
	"setuid",			/* 23 = setuid */
	"getuid",			/* 24 = getuid */
	"geteuid",			/* 25 = geteuid */
	"ptrace",			/* 26 = ptrace */
	"recvmsg",			/* 27 = recvmsg */
	"sendmsg",			/* 28 = sendmsg */
	"recvfrom",			/* 29 = recvfrom */
	"accept",			/* 30 = accept */
	"getpeername",			/* 31 = getpeername */
	"getsockname",			/* 32 = getsockname */
	"access",			/* 33 = access */
	"chflags",			/* 34 = chflags */
	"fchflags",			/* 35 = fchflags */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
#ifdef COMPAT_43
	"stat43",			/* 38 = stat43 */
#else
	"#38 (excluded aoutm68k_compat_43_sys_stat)",		/* 38 = excluded aoutm68k_compat_43_sys_stat */
#endif
	"getppid",			/* 39 = getppid */
#ifdef COMPAT_43
	"lstat43",			/* 40 = lstat43 */
#else
	"#40 (excluded aoutm68k_compat_43_sys_lstat)",		/* 40 = excluded aoutm68k_compat_43_sys_lstat */
#endif
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"getegid",			/* 43 = getegid */
	"profil",			/* 44 = profil */
#if defined(KTRACE) || !defined(_KERNEL)
	"ktrace",			/* 45 = ktrace */
#else
	"#45 (excluded ktrace)",		/* 45 = excluded ktrace */
#endif
#ifdef COMPAT_13
	"sigaction13",			/* 46 = sigaction13 */
#else
	"#46 (excluded compat_13_sys_sigaction)",		/* 46 = excluded compat_13_sys_sigaction */
#endif
	"getgid",			/* 47 = getgid */
#ifdef COMPAT_13
	"sigprocmask13",			/* 48 = sigprocmask13 */
#else
	"#48 (excluded compat_13_sys_sigprocmask)",		/* 48 = excluded compat_13_sys_sigprocmask */
#endif
	"__getlogin",			/* 49 = __getlogin */
	"setlogin",			/* 50 = setlogin */
	"acct",			/* 51 = acct */
#ifdef COMPAT_13
	"sigpending13",			/* 52 = sigpending13 */
	"sigaltstack13",			/* 53 = sigaltstack13 */
#else
	"#52 (excluded compat_13_sys_sigpending)",		/* 52 = excluded compat_13_sys_sigpending */
	"#53 (excluded compat_13_sys_sigaltstack)",		/* 53 = excluded compat_13_sys_sigaltstack */
#endif
	"ioctl",			/* 54 = ioctl */
#ifdef COMPAT_12
	"oreboot",			/* 55 = oreboot */
#else
	"#55 (excluded compat_12_sys_reboot)",		/* 55 = excluded compat_12_sys_reboot */
#endif
	"revoke",			/* 56 = revoke */
	"symlink",			/* 57 = symlink */
	"readlink",			/* 58 = readlink */
	"execve",			/* 59 = execve */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
#ifdef COMPAT_43
	"fstat43",			/* 62 = fstat43 */
	"ogetkerninfo",			/* 63 = ogetkerninfo */
	"ogetpagesize",			/* 64 = ogetpagesize */
#else
	"#62 (excluded aoutm68k_compat_43_sys_fstat)",		/* 62 = excluded aoutm68k_compat_43_sys_fstat */
	"#63 (excluded compat_43_sys_getkerninfo)",		/* 63 = excluded compat_43_sys_getkerninfo */
	"#64 (excluded compat_43_sys_getpagesize)",		/* 64 = excluded compat_43_sys_getpagesize */
#endif
#ifdef COMPAT_12
	"msync",			/* 65 = msync */
#else
	"#65 (excluded compat_12_sys_msync)",		/* 65 = excluded compat_12_sys_msync */
#endif
	"vfork",			/* 66 = vfork */
	"#67 (obsolete vread)",		/* 67 = obsolete vread */
	"#68 (obsolete vwrite)",		/* 68 = obsolete vwrite */
	"sbrk",			/* 69 = sbrk */
	"sstk",			/* 70 = sstk */
#ifdef COMPAT_43
	"ommap",			/* 71 = ommap */
#else
	"#71 (excluded compat_43_sys_mmap)",		/* 71 = excluded compat_43_sys_mmap */
#endif
	"vadvise",			/* 72 = vadvise */
	"munmap",			/* 73 = munmap */
	"mprotect",			/* 74 = mprotect */
	"madvise",			/* 75 = madvise */
	"#76 (obsolete vhangup)",		/* 76 = obsolete vhangup */
	"#77 (obsolete vlimit)",		/* 77 = obsolete vlimit */
	"mincore",			/* 78 = mincore */
	"getgroups",			/* 79 = getgroups */
	"setgroups",			/* 80 = setgroups */
	"getpgrp",			/* 81 = getpgrp */
	"setpgid",			/* 82 = setpgid */
	"setitimer",			/* 83 = setitimer */
#ifdef COMPAT_43
	"owait",			/* 84 = owait */
#else
	"#84 (excluded compat_43_sys_wait)",		/* 84 = excluded compat_43_sys_wait */
#endif
#ifdef COMPAT_12
	"oswapon",			/* 85 = oswapon */
#else
	"#85 (excluded compat_12_sys_swapon)",		/* 85 = excluded compat_12_sys_swapon */
#endif
	"getitimer",			/* 86 = getitimer */
#ifdef COMPAT_43
	"ogethostname",			/* 87 = ogethostname */
	"osethostname",			/* 88 = osethostname */
	"ogetdtablesize",			/* 89 = ogetdtablesize */
#else
	"#87 (excluded compat_43_sys_gethostname)",		/* 87 = excluded compat_43_sys_gethostname */
	"#88 (excluded compat_43_sys_sethostname)",		/* 88 = excluded compat_43_sys_sethostname */
	"#89 (excluded compat_43_sys_getdtablesize)",		/* 89 = excluded compat_43_sys_getdtablesize */
#endif
	"dup2",			/* 90 = dup2 */
	"#91 (unimplemented getdopt)",		/* 91 = unimplemented getdopt */
	"fcntl",			/* 92 = fcntl */
	"select",			/* 93 = select */
	"#94 (unimplemented setdopt)",		/* 94 = unimplemented setdopt */
	"fsync",			/* 95 = fsync */
	"setpriority",			/* 96 = setpriority */
	"socket",			/* 97 = socket */
	"connect",			/* 98 = connect */
#ifdef COMPAT_43
	"oaccept",			/* 99 = oaccept */
#else
	"#99 (excluded compat_43_sys_accept)",		/* 99 = excluded compat_43_sys_accept */
#endif
	"getpriority",			/* 100 = getpriority */
#ifdef COMPAT_43
	"osend",			/* 101 = osend */
	"orecv",			/* 102 = orecv */
#else
	"#101 (excluded compat_43_sys_send)",		/* 101 = excluded compat_43_sys_send */
	"#102 (excluded compat_43_sys_recv)",		/* 102 = excluded compat_43_sys_recv */
#endif
#ifdef COMPAT_13
	"sigreturn13",			/* 103 = sigreturn13 */
#else
	"#103 (excluded compat_13_sys_sigreturn)",		/* 103 = excluded compat_13_sys_sigreturn */
#endif
	"bind",			/* 104 = bind */
	"setsockopt",			/* 105 = setsockopt */
	"listen",			/* 106 = listen */
	"#107 (obsolete vtimes)",		/* 107 = obsolete vtimes */
#ifdef COMPAT_43
	"osigvec",			/* 108 = osigvec */
	"osigblock",			/* 109 = osigblock */
	"osigsetmask",			/* 110 = osigsetmask */
#else
	"#108 (excluded compat_43_sys_sigvec)",		/* 108 = excluded compat_43_sys_sigvec */
	"#109 (excluded compat_43_sys_sigblock)",		/* 109 = excluded compat_43_sys_sigblock */
	"#110 (excluded compat_43_sys_sigsetmask)",		/* 110 = excluded compat_43_sys_sigsetmask */
#endif
#ifdef COMPAT_13
	"sigsuspend13",			/* 111 = sigsuspend13 */
#else
	"#111 (excluded compat_13_sys_sigsuspend)",		/* 111 = excluded compat_13_sys_sigsuspend */
#endif
#ifdef COMPAT_43
	"osigstack",			/* 112 = osigstack */
	"orecvmsg",			/* 113 = orecvmsg */
	"osendmsg",			/* 114 = osendmsg */
#else
	"#112 (excluded compat_43_sys_sigstack)",		/* 112 = excluded compat_43_sys_sigstack */
	"#113 (excluded compat_43_sys_recvmesg)",		/* 113 = excluded compat_43_sys_recvmesg */
	"#114 (excluded compat_43_sys_sendmesg)",		/* 114 = excluded compat_43_sys_sendmesg */
#endif
	"#115 (obsolete vtrace)",		/* 115 = obsolete vtrace */
	"gettimeofday",			/* 116 = gettimeofday */
	"getrusage",			/* 117 = getrusage */
	"getsockopt",			/* 118 = getsockopt */
	"#119 (obsolete resuba)",		/* 119 = obsolete resuba */
	"readv",			/* 120 = readv */
	"writev",			/* 121 = writev */
	"settimeofday",			/* 122 = settimeofday */
	"fchown",			/* 123 = fchown */
	"fchmod",			/* 124 = fchmod */
#ifdef COMPAT_43
	"orecvfrom",			/* 125 = orecvfrom */
#else
	"#125 (excluded compat_43_sys_recvfrom)",		/* 125 = excluded compat_43_sys_recvfrom */
#endif
	"setreuid",			/* 126 = setreuid */
	"setregid",			/* 127 = setregid */
	"rename",			/* 128 = rename */
#ifdef COMPAT_43
	"otruncate",			/* 129 = otruncate */
	"oftruncate",			/* 130 = oftruncate */
#else
	"#129 (excluded aoutm68k_compat_43_sys_truncate)",		/* 129 = excluded aoutm68k_compat_43_sys_truncate */
	"#130 (excluded compat_43_sys_ftruncate)",		/* 130 = excluded compat_43_sys_ftruncate */
#endif
	"flock",			/* 131 = flock */
	"mkfifo",			/* 132 = mkfifo */
	"sendto",			/* 133 = sendto */
	"shutdown",			/* 134 = shutdown */
	"socketpair",			/* 135 = socketpair */
	"mkdir",			/* 136 = mkdir */
	"rmdir",			/* 137 = rmdir */
	"utimes",			/* 138 = utimes */
	"#139 (obsolete 4.2 sigreturn)",		/* 139 = obsolete 4.2 sigreturn */
	"adjtime",			/* 140 = adjtime */
#ifdef COMPAT_43
	"ogetpeername",			/* 141 = ogetpeername */
	"ogethostid",			/* 142 = ogethostid */
	"osethostid",			/* 143 = osethostid */
	"ogetrlimit",			/* 144 = ogetrlimit */
	"osetrlimit",			/* 145 = osetrlimit */
	"okillpg",			/* 146 = okillpg */
#else
	"#141 (excluded compat_43_sys_getpeername)",		/* 141 = excluded compat_43_sys_getpeername */
	"#142 (excluded compat_43_sys_gethostid)",		/* 142 = excluded compat_43_sys_gethostid */
	"#143 (excluded compat_43_sys_sethostid)",		/* 143 = excluded compat_43_sys_sethostid */
	"#144 (excluded compat_43_sys_getrlimit)",		/* 144 = excluded compat_43_sys_getrlimit */
	"#145 (excluded compat_43_sys_setrlimit)",		/* 145 = excluded compat_43_sys_setrlimit */
	"#146 (excluded compat_43_sys_killpg)",		/* 146 = excluded compat_43_sys_killpg */
#endif
	"setsid",			/* 147 = setsid */
	"quotactl",			/* 148 = quotactl */
#ifdef COMPAT_43
	"oquota",			/* 149 = oquota */
	"ogetsockname",			/* 150 = ogetsockname */
#else
	"#149 (excluded compat_43_sys_quota)",		/* 149 = excluded compat_43_sys_quota */
	"#150 (excluded compat_43_sys_getsockname)",		/* 150 = excluded compat_43_sys_getsockname */
#endif
	"#151 (unimplemented)",		/* 151 = unimplemented */
	"#152 (unimplemented)",		/* 152 = unimplemented */
	"#153 (unimplemented)",		/* 153 = unimplemented */
	"#154 (unimplemented)",		/* 154 = unimplemented */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	"nfssvc",			/* 155 = nfssvc */
#else
	"#155 (excluded nfssvc)",		/* 155 = excluded nfssvc */
#endif
#ifdef COMPAT_43
	"ogetdirentries",			/* 156 = ogetdirentries */
#else
	"#156 (excluded compat_43_sys_getdirentries)",		/* 156 = excluded compat_43_sys_getdirentries */
#endif
	"statfs",			/* 157 = statfs */
	"fstatfs",			/* 158 = fstatfs */
	"#159 (unimplemented)",		/* 159 = unimplemented */
	"#160 (unimplemented)",		/* 160 = unimplemented */
#if defined(NFS) || defined(NFSSERVER) || !defined(_KERNEL)
	"getfh",			/* 161 = getfh */
#else
	"#161 (excluded getfh)",		/* 161 = excluded getfh */
#endif
#ifdef COMPAT_09
	"ogetdomainname",			/* 162 = ogetdomainname */
	"osetdomainname",			/* 163 = osetdomainname */
	"ouname",			/* 164 = ouname */
#else
	"#162 (excluded compat_09_sys_getdomainname)",		/* 162 = excluded compat_09_sys_getdomainname */
	"#163 (excluded compat_09_sys_setdomainname)",		/* 163 = excluded compat_09_sys_setdomainname */
	"#164 (excluded compat_09_sys_uname)",		/* 164 = excluded compat_09_sys_uname */
#endif
	"sysarch",			/* 165 = sysarch */
	"#166 (unimplemented)",		/* 166 = unimplemented */
	"#167 (unimplemented)",		/* 167 = unimplemented */
	"#168 (unimplemented)",		/* 168 = unimplemented */
#if (defined(SYSVSEM) || !defined(_KERNEL)) && !defined(alpha) && defined(COMPAT_10)
	"osemsys",			/* 169 = osemsys */
#else
	"#169 (excluded 1.0 semsys)",		/* 169 = excluded 1.0 semsys */
#endif
#if (defined(SYSVMSG) || !defined(_KERNEL)) && !defined(alpha) && defined(COMPAT_10)
	"omsgsys",			/* 170 = omsgsys */
#else
	"#170 (excluded 1.0 msgsys)",		/* 170 = excluded 1.0 msgsys */
#endif
#if (defined(SYSVSHM) || !defined(_KERNEL)) && !defined(alpha) && defined(COMPAT_10)
	"oshmsys",			/* 171 = oshmsys */
#else
	"#171 (excluded 1.0 shmsys)",		/* 171 = excluded 1.0 shmsys */
#endif
	"#172 (unimplemented)",		/* 172 = unimplemented */
	"pread",			/* 173 = pread */
	"pwrite",			/* 174 = pwrite */
	"ntp_gettime",			/* 175 = ntp_gettime */
#if defined(NTP) || !defined(_KERNEL)
	"ntp_adjtime",			/* 176 = ntp_adjtime */
#else
	"#176 (excluded ntp_adjtime)",		/* 176 = excluded ntp_adjtime */
#endif
	"#177 (unimplemented)",		/* 177 = unimplemented */
	"#178 (unimplemented)",		/* 178 = unimplemented */
	"#179 (unimplemented)",		/* 179 = unimplemented */
	"#180 (unimplemented)",		/* 180 = unimplemented */
	"setgid",			/* 181 = setgid */
	"setegid",			/* 182 = setegid */
	"seteuid",			/* 183 = seteuid */
#if defined(LFS) || !defined(_KERNEL)
	"lfs_bmapv",			/* 184 = lfs_bmapv */
	"lfs_markv",			/* 185 = lfs_markv */
	"lfs_segclean",			/* 186 = lfs_segclean */
	"lfs_segwait",			/* 187 = lfs_segwait */
#else
	"#184 (excluded lfs_bmapv)",		/* 184 = excluded lfs_bmapv */
	"#185 (excluded lfs_markv)",		/* 185 = excluded lfs_markv */
	"#186 (excluded lfs_segclean)",		/* 186 = excluded lfs_segclean */
	"#187 (excluded lfs_segwait)",		/* 187 = excluded lfs_segwait */
#endif
#ifdef COMPAT_12
	"stat12",			/* 188 = stat12 */
	"fstat12",			/* 189 = fstat12 */
	"lstat12",			/* 190 = lstat12 */
#else
	"#188 (excluded aoutm68k_compat_12_sys_stat)",		/* 188 = excluded aoutm68k_compat_12_sys_stat */
	"#189 (excluded aoutm68k_compat_12_sys_fstat)",		/* 189 = excluded aoutm68k_compat_12_sys_fstat */
	"#190 (excluded aoutm68k_compat_12_sys_lstat)",		/* 190 = excluded aoutm68k_compat_12_sys_lstat */
#endif
	"pathconf",			/* 191 = pathconf */
	"fpathconf",			/* 192 = fpathconf */
	"#193 (unimplemented)",		/* 193 = unimplemented */
	"getrlimit",			/* 194 = getrlimit */
	"setrlimit",			/* 195 = setrlimit */
#ifdef COMPAT_12
	"getdirentries",			/* 196 = getdirentries */
#else
	"#196 (excluded compat_12_sys_getdirentries)",		/* 196 = excluded compat_12_sys_getdirentries */
#endif
	"mmap",			/* 197 = mmap */
	"__syscall",			/* 198 = __syscall */
	"lseek",			/* 199 = lseek */
	"truncate",			/* 200 = truncate */
	"ftruncate",			/* 201 = ftruncate */
	"__sysctl",			/* 202 = __sysctl */
	"mlock",			/* 203 = mlock */
	"munlock",			/* 204 = munlock */
	"undelete",			/* 205 = undelete */
	"futimes",			/* 206 = futimes */
	"getpgid",			/* 207 = getpgid */
	"reboot",			/* 208 = reboot */
	"poll",			/* 209 = poll */
#if defined(LKM) || !defined(_KERNEL)
	"lkmnosys",			/* 210 = lkmnosys */
	"lkmnosys",			/* 211 = lkmnosys */
	"lkmnosys",			/* 212 = lkmnosys */
	"lkmnosys",			/* 213 = lkmnosys */
	"lkmnosys",			/* 214 = lkmnosys */
	"lkmnosys",			/* 215 = lkmnosys */
	"lkmnosys",			/* 216 = lkmnosys */
	"lkmnosys",			/* 217 = lkmnosys */
	"lkmnosys",			/* 218 = lkmnosys */
	"lkmnosys",			/* 219 = lkmnosys */
#else	/* !LKM */
	"#210 (excluded lkmnosys)",		/* 210 = excluded lkmnosys */
	"#211 (excluded lkmnosys)",		/* 211 = excluded lkmnosys */
	"#212 (excluded lkmnosys)",		/* 212 = excluded lkmnosys */
	"#213 (excluded lkmnosys)",		/* 213 = excluded lkmnosys */
	"#214 (excluded lkmnosys)",		/* 214 = excluded lkmnosys */
	"#215 (excluded lkmnosys)",		/* 215 = excluded lkmnosys */
	"#216 (excluded lkmnosys)",		/* 216 = excluded lkmnosys */
	"#217 (excluded lkmnosys)",		/* 217 = excluded lkmnosys */
	"#218 (excluded lkmnosys)",		/* 218 = excluded lkmnosys */
	"#219 (excluded lkmnosys)",		/* 219 = excluded lkmnosys */
#endif	/* !LKM */
#if defined(SYSVSEM) || !defined(_KERNEL)
#ifdef COMPAT_14
	"__semctl",			/* 220 = __semctl */
#else
	"#220 (excluded compat_14_semctl)",		/* 220 = excluded compat_14_semctl */
#endif
	"semget",			/* 221 = semget */
	"semop",			/* 222 = semop */
	"semconfig",			/* 223 = semconfig */
#else
	"#220 (excluded compat_14_semctl)",		/* 220 = excluded compat_14_semctl */
	"#221 (excluded semget)",		/* 221 = excluded semget */
	"#222 (excluded semop)",		/* 222 = excluded semop */
	"#223 (excluded semconfig)",		/* 223 = excluded semconfig */
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
#ifdef COMPAT_14
	"msgctl",			/* 224 = msgctl */
#else
	"#224 (excluded compat_14_sys_msgctl)",		/* 224 = excluded compat_14_sys_msgctl */
#endif
	"msgget",			/* 225 = msgget */
	"msgsnd",			/* 226 = msgsnd */
	"msgrcv",			/* 227 = msgrcv */
#else
	"#224 (excluded compat_14_msgctl)",		/* 224 = excluded compat_14_msgctl */
	"#225 (excluded msgget)",		/* 225 = excluded msgget */
	"#226 (excluded msgsnd)",		/* 226 = excluded msgsnd */
	"#227 (excluded msgrcv)",		/* 227 = excluded msgrcv */
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	"shmat",			/* 228 = shmat */
#ifdef COMPAT_14
	"shmctl",			/* 229 = shmctl */
#else
	"#229 (excluded compat_14_sys_shmctl)",		/* 229 = excluded compat_14_sys_shmctl */
#endif
	"shmdt",			/* 230 = shmdt */
	"shmget",			/* 231 = shmget */
#else
	"#228 (excluded shmat)",		/* 228 = excluded shmat */
	"#229 (excluded compat_14_shmctl)",		/* 229 = excluded compat_14_shmctl */
	"#230 (excluded shmdt)",		/* 230 = excluded shmdt */
	"#231 (excluded shmget)",		/* 231 = excluded shmget */
#endif
	"clock_gettime",			/* 232 = clock_gettime */
	"clock_settime",			/* 233 = clock_settime */
	"clock_getres",			/* 234 = clock_getres */
	"#235 (unimplemented timer_create)",		/* 235 = unimplemented timer_create */
	"#236 (unimplemented timer_delete)",		/* 236 = unimplemented timer_delete */
	"#237 (unimplemented timer_settime)",		/* 237 = unimplemented timer_settime */
	"#238 (unimplemented timer_gettime)",		/* 238 = unimplemented timer_gettime */
	"#239 (unimplemented timer_getoverrun)",		/* 239 = unimplemented timer_getoverrun */
	"nanosleep",			/* 240 = nanosleep */
	"fdatasync",			/* 241 = fdatasync */
	"mlockall",			/* 242 = mlockall */
	"munlockall",			/* 243 = munlockall */
	"#244 (unimplemented)",		/* 244 = unimplemented */
	"#245 (unimplemented)",		/* 245 = unimplemented */
	"#246 (unimplemented)",		/* 246 = unimplemented */
	"#247 (unimplemented)",		/* 247 = unimplemented */
	"#248 (unimplemented)",		/* 248 = unimplemented */
	"#249 (unimplemented)",		/* 249 = unimplemented */
	"#250 (unimplemented)",		/* 250 = unimplemented */
	"#251 (unimplemented)",		/* 251 = unimplemented */
	"#252 (unimplemented)",		/* 252 = unimplemented */
	"#253 (unimplemented)",		/* 253 = unimplemented */
	"#254 (unimplemented)",		/* 254 = unimplemented */
	"#255 (unimplemented)",		/* 255 = unimplemented */
	"#256 (unimplemented)",		/* 256 = unimplemented */
	"#257 (unimplemented)",		/* 257 = unimplemented */
	"#258 (unimplemented)",		/* 258 = unimplemented */
	"#259 (unimplemented)",		/* 259 = unimplemented */
	"#260 (unimplemented)",		/* 260 = unimplemented */
	"#261 (unimplemented)",		/* 261 = unimplemented */
	"#262 (unimplemented)",		/* 262 = unimplemented */
	"#263 (unimplemented)",		/* 263 = unimplemented */
	"#264 (unimplemented)",		/* 264 = unimplemented */
	"#265 (unimplemented)",		/* 265 = unimplemented */
	"#266 (unimplemented)",		/* 266 = unimplemented */
	"#267 (unimplemented)",		/* 267 = unimplemented */
	"#268 (unimplemented)",		/* 268 = unimplemented */
	"#269 (unimplemented)",		/* 269 = unimplemented */
	"__posix_rename",			/* 270 = __posix_rename */
	"swapctl",			/* 271 = swapctl */
	"getdents",			/* 272 = getdents */
	"minherit",			/* 273 = minherit */
	"lchmod",			/* 274 = lchmod */
	"lchown",			/* 275 = lchown */
	"lutimes",			/* 276 = lutimes */
	"__msync13",			/* 277 = __msync13 */
	"__stat13",			/* 278 = __stat13 */
	"__fstat13",			/* 279 = __fstat13 */
	"__lstat13",			/* 280 = __lstat13 */
	"__sigaltstack14",			/* 281 = __sigaltstack14 */
	"__vfork14",			/* 282 = __vfork14 */
	"__posix_chown",			/* 283 = __posix_chown */
	"__posix_fchown",			/* 284 = __posix_fchown */
	"__posix_lchown",			/* 285 = __posix_lchown */
	"getsid",			/* 286 = getsid */
	"#287 (unimplemented)",		/* 287 = unimplemented */
#if defined(KTRACE) || !defined(_KERNEL)
	"fktrace",			/* 288 = fktrace */
#else
	"#288 (excluded ktrace)",		/* 288 = excluded ktrace */
#endif
	"preadv",			/* 289 = preadv */
	"pwritev",			/* 290 = pwritev */
	"__sigaction14",			/* 291 = __sigaction14 */
	"__sigpending14",			/* 292 = __sigpending14 */
	"__sigprocmask14",			/* 293 = __sigprocmask14 */
	"__sigsuspend14",			/* 294 = __sigsuspend14 */
	"__sigreturn14",			/* 295 = __sigreturn14 */
	"__getcwd",			/* 296 = __getcwd */
	"fchroot",			/* 297 = fchroot */
	"fhopen",			/* 298 = fhopen */
	"fhstat",			/* 299 = fhstat */
	"fhstatfs",			/* 300 = fhstatfs */
#if defined(SYSVSEM) || !defined(_KERNEL)
	"____semctl13",			/* 301 = ____semctl13 */
#else
	"#301 (excluded ____semctl13)",		/* 301 = excluded ____semctl13 */
#endif
#if defined(SYSVMSG) || !defined(_KERNEL)
	"__msgctl13",			/* 302 = __msgctl13 */
#else
	"#302 (excluded __msgctl13)",		/* 302 = excluded __msgctl13 */
#endif
#if defined(SYSVSHM) || !defined(_KERNEL)
	"__shmctl13",			/* 303 = __shmctl13 */
#else
	"#303 (excluded __shmctl13)",		/* 303 = excluded __shmctl13 */
#endif
	"lchflags",			/* 304 = lchflags */
	"issetugid",			/* 305 = issetugid */
};
