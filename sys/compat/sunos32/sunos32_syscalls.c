/* $NetBSD: sunos32_syscalls.c,v 1.8 2002/01/03 15:30:08 mrg Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from;	NetBSD: syscalls.master,v 1.7 2002/01/03 15:28:43 mrg Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: sunos32_syscalls.c,v 1.8 2002/01/03 15:30:08 mrg Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_nfsserver.h"
#include "opt_sysv.h"
#include "fs_nfs.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/syscallargs.h>
#include <compat/sunos/sunos.h>
#include <compat/sunos/sunos_syscallargs.h>
#include <compat/netbsd32/netbsd32.h>
#include <compat/netbsd32/netbsd32_syscallargs.h>
#include <compat/sunos32/sunos32.h>
#include <compat/sunos32/sunos32_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const sunos32_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"netbsd32_exit",			/* 1 = netbsd32_exit */
	"fork",			/* 2 = fork */
	"netbsd32_read",			/* 3 = netbsd32_read */
	"netbsd32_write",			/* 4 = netbsd32_write */
	"open",			/* 5 = open */
	"netbsd32_close",			/* 6 = netbsd32_close */
	"wait4",			/* 7 = wait4 */
	"creat",			/* 8 = creat */
	"netbsd32_link",			/* 9 = netbsd32_link */
	"netbsd32_unlink",			/* 10 = netbsd32_unlink */
	"execv",			/* 11 = execv */
	"netbsd32_chdir",			/* 12 = netbsd32_chdir */
	"#13 (obsolete old_time)",		/* 13 = obsolete old_time */
	"mknod",			/* 14 = mknod */
	"netbsd32_chmod",			/* 15 = netbsd32_chmod */
	"netbsd32_chown",			/* 16 = netbsd32_chown */
	"netbsd32_break",			/* 17 = netbsd32_break */
	"#18 (obsolete old_stat)",		/* 18 = obsolete old_stat */
	"compat_43_netbsd32_olseek",			/* 19 = compat_43_netbsd32_olseek */
	"getpid_with_ppid",			/* 20 = getpid_with_ppid */
	"#21 (obsolete sunos_old_mount)",		/* 21 = obsolete sunos_old_mount */
	"#22 (unimplemented System V umount)",		/* 22 = unimplemented System V umount */
	"netbsd32_setuid",			/* 23 = netbsd32_setuid */
	"getuid_with_euid",			/* 24 = getuid_with_euid */
	"stime",			/* 25 = stime */
	"ptrace",			/* 26 = ptrace */
	"#27 (unimplemented old_sunos_alarm)",		/* 27 = unimplemented old_sunos_alarm */
	"#28 (unimplemented old_sunos_fstat)",		/* 28 = unimplemented old_sunos_fstat */
	"#29 (unimplemented old_sunos_pause)",		/* 29 = unimplemented old_sunos_pause */
	"#30 (unimplemented old_sunos_utime)",		/* 30 = unimplemented old_sunos_utime */
	"#31 (unimplemented old_sunos_stty)",		/* 31 = unimplemented old_sunos_stty */
	"#32 (unimplemented old_sunos_gtty)",		/* 32 = unimplemented old_sunos_gtty */
	"access",			/* 33 = access */
	"#34 (unimplemented old_sunos_nice)",		/* 34 = unimplemented old_sunos_nice */
	"#35 (unimplemented old_sunos_ftime)",		/* 35 = unimplemented old_sunos_ftime */
	"sync",			/* 36 = sync */
	"netbsd32_kill",			/* 37 = netbsd32_kill */
	"stat",			/* 38 = stat */
	"#39 (unimplemented sunos_setpgrp)",		/* 39 = unimplemented sunos_setpgrp */
	"lstat",			/* 40 = lstat */
	"netbsd32_dup",			/* 41 = netbsd32_dup */
	"pipe",			/* 42 = pipe */
	"#43 (unimplemented sunos_times)",		/* 43 = unimplemented sunos_times */
	"netbsd32_profil",			/* 44 = netbsd32_profil */
	"#45 (unimplemented)",		/* 45 = unimplemented */
	"netbsd32_setgid",			/* 46 = netbsd32_setgid */
	"getgid_with_egid",			/* 47 = getgid_with_egid */
	"#48 (unimplemented sunos_ssig)",		/* 48 = unimplemented sunos_ssig */
	"#49 (unimplemented reserved for USG)",		/* 49 = unimplemented reserved for USG */
	"#50 (unimplemented reserved for USG)",		/* 50 = unimplemented reserved for USG */
	"netbsd32_acct",			/* 51 = netbsd32_acct */
	"#52 (unimplemented)",		/* 52 = unimplemented */
	"mctl",			/* 53 = mctl */
	"ioctl",			/* 54 = ioctl */
	"reboot",			/* 55 = reboot */
	"#56 (obsolete sunos_owait3)",		/* 56 = obsolete sunos_owait3 */
	"netbsd32_symlink",			/* 57 = netbsd32_symlink */
	"netbsd32_readlink",			/* 58 = netbsd32_readlink */
	"execve",			/* 59 = execve */
	"netbsd32_umask",			/* 60 = netbsd32_umask */
	"netbsd32_chroot",			/* 61 = netbsd32_chroot */
	"compat_43_netbsd32_fstat43",			/* 62 = compat_43_netbsd32_fstat43 */
	"#63 (unimplemented)",		/* 63 = unimplemented */
	"ogetpagesize",			/* 64 = ogetpagesize */
	"omsync",			/* 65 = omsync */
	"vfork",			/* 66 = vfork */
	"#67 (obsolete vread)",		/* 67 = obsolete vread */
	"#68 (obsolete vwrite)",		/* 68 = obsolete vwrite */
	"netbsd32_sbrk",			/* 69 = netbsd32_sbrk */
	"netbsd32_sstk",			/* 70 = netbsd32_sstk */
	"mmap",			/* 71 = mmap */
	"vadvise",			/* 72 = vadvise */
	"netbsd32_munmap",			/* 73 = netbsd32_munmap */
	"netbsd32_mprotect",			/* 74 = netbsd32_mprotect */
	"netbsd32_madvise",			/* 75 = netbsd32_madvise */
	"vhangup",			/* 76 = vhangup */
	"#77 (unimplemented vlimit)",		/* 77 = unimplemented vlimit */
	"netbsd32_mincore",			/* 78 = netbsd32_mincore */
	"netbsd32_getgroups",			/* 79 = netbsd32_getgroups */
	"netbsd32_setgroups",			/* 80 = netbsd32_setgroups */
	"getpgrp",			/* 81 = getpgrp */
	"setpgrp",			/* 82 = setpgrp */
	"netbsd32_setitimer",			/* 83 = netbsd32_setitimer */
	"#84 (unimplemented { int sunos32_sys_wait ( void ) ; })",		/* 84 = unimplemented { int sunos32_sys_wait ( void ) ; } */
	"compat_12_netbsd32_oswapon",			/* 85 = compat_12_netbsd32_oswapon */
	"netbsd32_getitimer",			/* 86 = netbsd32_getitimer */
	"compat_43_netbsd32_ogethostname",			/* 87 = compat_43_netbsd32_ogethostname */
	"compat_43_netbsd32_osethostname",			/* 88 = compat_43_netbsd32_osethostname */
	"ogetdtablesize",			/* 89 = ogetdtablesize */
	"netbsd32_dup2",			/* 90 = netbsd32_dup2 */
	"#91 (unimplemented getdopt)",		/* 91 = unimplemented getdopt */
	"fcntl",			/* 92 = fcntl */
	"netbsd32_select",			/* 93 = netbsd32_select */
	"#94 (unimplemented setdopt)",		/* 94 = unimplemented setdopt */
	"netbsd32_fsync",			/* 95 = netbsd32_fsync */
	"netbsd32_setpriority",			/* 96 = netbsd32_setpriority */
	"socket",			/* 97 = socket */
	"netbsd32_connect",			/* 98 = netbsd32_connect */
	"compat_43_netbsd32_oaccept",			/* 99 = compat_43_netbsd32_oaccept */
	"netbsd32_getpriority",			/* 100 = netbsd32_getpriority */
	"compat_43_netbsd32_osend",			/* 101 = compat_43_netbsd32_osend */
	"compat_43_netbsd32_orecv",			/* 102 = compat_43_netbsd32_orecv */
	"#103 (unimplemented old socketaddr)",		/* 103 = unimplemented old socketaddr */
	"netbsd32_bind",			/* 104 = netbsd32_bind */
	"setsockopt",			/* 105 = setsockopt */
	"netbsd32_listen",			/* 106 = netbsd32_listen */
	"#107 (unimplemented vtimes)",		/* 107 = unimplemented vtimes */
	"sigvec",			/* 108 = sigvec */
	"compat_43_netbsd32_sigblock",			/* 109 = compat_43_netbsd32_sigblock */
	"compat_43_netbsd32_sigsetmask",			/* 110 = compat_43_netbsd32_sigsetmask */
	"sigsuspend",			/* 111 = sigsuspend */
	"compat_43_netbsd32_osigstack",			/* 112 = compat_43_netbsd32_osigstack */
	"compat_43_netbsd32_orecvmsg",			/* 113 = compat_43_netbsd32_orecvmsg */
	"compat_43_netbsd32_osendmsg",			/* 114 = compat_43_netbsd32_osendmsg */
	"#115 (obsolete vtrace)",		/* 115 = obsolete vtrace */
	"netbsd32_gettimeofday",			/* 116 = netbsd32_gettimeofday */
	"netbsd32_getrusage",			/* 117 = netbsd32_getrusage */
	"netbsd32_getsockopt",			/* 118 = netbsd32_getsockopt */
	"#119 (unimplemented)",		/* 119 = unimplemented */
	"netbsd32_readv",			/* 120 = netbsd32_readv */
	"netbsd32_writev",			/* 121 = netbsd32_writev */
	"netbsd32_settimeofday",			/* 122 = netbsd32_settimeofday */
	"netbsd32_fchown",			/* 123 = netbsd32_fchown */
	"netbsd32_fchmod",			/* 124 = netbsd32_fchmod */
	"compat_43_netbsd32_orecvfrom",			/* 125 = compat_43_netbsd32_orecvfrom */
	"netbsd32_setreuid",			/* 126 = netbsd32_setreuid */
	"netbsd32_setregid",			/* 127 = netbsd32_setregid */
	"netbsd32_rename",			/* 128 = netbsd32_rename */
	"compat_43_netbsd32_otruncate",			/* 129 = compat_43_netbsd32_otruncate */
	"compat_43_netbsd32_oftruncate",			/* 130 = compat_43_netbsd32_oftruncate */
	"netbsd32_flock",			/* 131 = netbsd32_flock */
	"#132 (unimplemented)",		/* 132 = unimplemented */
	"netbsd32_sendto",			/* 133 = netbsd32_sendto */
	"netbsd32_shutdown",			/* 134 = netbsd32_shutdown */
	"socketpair",			/* 135 = socketpair */
	"netbsd32_mkdir",			/* 136 = netbsd32_mkdir */
	"netbsd32_rmdir",			/* 137 = netbsd32_rmdir */
	"netbsd32_utimes",			/* 138 = netbsd32_utimes */
	"sigreturn",			/* 139 = sigreturn */
	"netbsd32_adjtime",			/* 140 = netbsd32_adjtime */
	"compat_43_netbsd32_ogetpeername",			/* 141 = compat_43_netbsd32_ogetpeername */
	"gethostid",			/* 142 = gethostid */
	"#143 (unimplemented old sethostid)",		/* 143 = unimplemented old sethostid */
	"getrlimit",			/* 144 = getrlimit */
	"setrlimit",			/* 145 = setrlimit */
	"compat_43_netbsd32_killpg",			/* 146 = compat_43_netbsd32_killpg */
	"#147 (unimplemented)",		/* 147 = unimplemented */
	"#148 (unimplemented)",		/* 148 = unimplemented */
	"#149 (unimplemented)",		/* 149 = unimplemented */
	"compat_43_netbsd32_ogetsockname",			/* 150 = compat_43_netbsd32_ogetsockname */
	"#151 (unimplemented getmsg)",		/* 151 = unimplemented getmsg */
	"#152 (unimplemented putmsg)",		/* 152 = unimplemented putmsg */
	"netbsd32_poll",			/* 153 = netbsd32_poll */
	"#154 (unimplemented)",		/* 154 = unimplemented */
#ifdef NFSSERVER
	"nfssvc",			/* 155 = nfssvc */
#else
	"#155 (unimplemented)",		/* 155 = unimplemented */
#endif
	"compat_43_netbsd32_ogetdirentries",			/* 156 = compat_43_netbsd32_ogetdirentries */
	"statfs",			/* 157 = statfs */
	"fstatfs",			/* 158 = fstatfs */
	"unmount",			/* 159 = unmount */
#ifdef NFS
	"async_daemon",			/* 160 = async_daemon */
	"getfh",			/* 161 = getfh */
#else
	"#160 (unimplemented)",		/* 160 = unimplemented */
	"#161 (unimplemented)",		/* 161 = unimplemented */
#endif
	"compat_09_netbsd32_ogetdomainname",			/* 162 = compat_09_netbsd32_ogetdomainname */
	"compat_09_netbsd32_osetdomainname",			/* 163 = compat_09_netbsd32_osetdomainname */
	"#164 (unimplemented rtschedule)",		/* 164 = unimplemented rtschedule */
	"quotactl",			/* 165 = quotactl */
	"exportfs",			/* 166 = exportfs */
	"mount",			/* 167 = mount */
	"ustat",			/* 168 = ustat */
#ifdef SYSVSEM
	"osemsys",			/* 169 = osemsys */
#else
	"#169 (unimplemented semsys)",		/* 169 = unimplemented semsys */
#endif
#ifdef SYSVMSG
	"omsgsys",			/* 170 = omsgsys */
#else
	"#170 (unimplemented msgsys)",		/* 170 = unimplemented msgsys */
#endif
#ifdef SYSVSHM
	"oshmsys",			/* 171 = oshmsys */
#else
	"#171 (unimplemented shmsys)",		/* 171 = unimplemented shmsys */
#endif
	"auditsys",			/* 172 = auditsys */
	"#173 (unimplemented rfssys)",		/* 173 = unimplemented rfssys */
	"getdents",			/* 174 = getdents */
	"setsid",			/* 175 = setsid */
	"netbsd32_fchdir",			/* 176 = netbsd32_fchdir */
	"netbsd32_fchroot",			/* 177 = netbsd32_fchroot */
	"#178 (unimplemented vpixsys)",		/* 178 = unimplemented vpixsys */
	"#179 (unimplemented aioread)",		/* 179 = unimplemented aioread */
	"#180 (unimplemented aiowrite)",		/* 180 = unimplemented aiowrite */
	"#181 (unimplemented aiowait)",		/* 181 = unimplemented aiowait */
	"#182 (unimplemented aiocancel)",		/* 182 = unimplemented aiocancel */
	"sigpending",			/* 183 = sigpending */
	"#184 (unimplemented)",		/* 184 = unimplemented */
	"netbsd32_setpgid",			/* 185 = netbsd32_setpgid */
	"netbsd32_pathconf",			/* 186 = netbsd32_pathconf */
	"netbsd32_fpathconf",			/* 187 = netbsd32_fpathconf */
	"sysconf",			/* 188 = sysconf */
	"uname",			/* 189 = uname */
};
