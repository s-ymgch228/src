/* $NetBSD: linux_syscalls.c,v 1.68 2008/11/12 18:09:22 njoly Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.63 2008/11/12 18:07:40 njoly Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_syscalls.c,v 1.68 2008/11/12 18:09:22 njoly Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "opt_compat_netbsd.h"
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sched.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/common/linux_mmap.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const linux_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"waitpid",
	/*   8 */	"creat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"execve",
	/*  12 */	"chdir",
	/*  13 */	"time",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"chown16",
	/*  17 */	"#17 (obsolete break)",
	/*  18 */	"#18 (obsolete ostat)",
#if !defined(_KERNEL) || defined(COMPAT_43)
	/*  19 */	"lseek",
#else
	/*  19 */	"#19 (unimplemented compat_43_sys_lseek)",
#endif
	/*  20 */	"getpid",
	/*  21 */	"#21 (unimplemented mount)",
	/*  22 */	"#22 (obsolete umount)",
	/*  23 */	"linux_setuid16",
	/*  24 */	"linux_getuid16",
	/*  25 */	"stime",
	/*  26 */	"ptrace",
	/*  27 */	"alarm",
	/*  28 */	"#28 (obsolete ofstat)",
	/*  29 */	"pause",
	/*  30 */	"utime",
	/*  31 */	"#31 (obsolete stty)",
	/*  32 */	"#32 (obsolete gtty)",
	/*  33 */	"access",
	/*  34 */	"nice",
	/*  35 */	"#35 (obsolete ftime)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"__posix_rename",
	/*  39 */	"mkdir",
	/*  40 */	"rmdir",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"times",
	/*  44 */	"#44 (obsolete prof)",
	/*  45 */	"brk",
	/*  46 */	"linux_setgid16",
	/*  47 */	"linux_getgid16",
	/*  48 */	"signal",
	/*  49 */	"linux_geteuid16",
	/*  50 */	"linux_getegid16",
	/*  51 */	"acct",
	/*  52 */	"#52 (unimplemented umount)",
	/*  53 */	"#53 (obsolete lock)",
	/*  54 */	"ioctl",
	/*  55 */	"fcntl",
	/*  56 */	"#56 (obsolete mpx)",
	/*  57 */	"setpgid",
	/*  58 */	"#58 (obsolete ulimit)",
	/*  59 */	"#59 (unimplemented oldolduname)",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"#62 (unimplemented ustat)",
	/*  63 */	"dup2",
	/*  64 */	"getppid",
	/*  65 */	"getpgrp",
	/*  66 */	"setsid",
	/*  67 */	"sigaction",
	/*  68 */	"siggetmask",
	/*  69 */	"sigsetmask",
	/*  70 */	"setreuid16",
	/*  71 */	"setregid16",
	/*  72 */	"sigsuspend",
	/*  73 */	"sigpending",
#if !defined(_KERNEL) || defined(COMPAT_43)
	/*  74 */	"sethostname",
#else
	/*  74 */	"#74 (unimplemented compat_43_sys_sethostname)",
#endif
	/*  75 */	"setrlimit",
	/*  76 */	"getrlimit",
	/*  77 */	"getrusage",
	/*  78 */	"gettimeofday",
	/*  79 */	"settimeofday",
	/*  80 */	"getgroups16",
	/*  81 */	"setgroups16",
	/*  82 */	"oldselect",
	/*  83 */	"symlink",
#if !defined(_KERNEL) || defined(COMPAT_43)
	/*  84 */	"oolstat",
#else
	/*  84 */	"#84 (unimplemented compat_43_sys_lstat)",
#endif
	/*  85 */	"readlink",
#ifdef EXEC_AOUT
	/*  86 */	"uselib",
#else
	/*  86 */	"#86 (unimplemented sys_uselib)",
#endif
	/*  87 */	"swapon",
	/*  88 */	"reboot",
	/*  89 */	"readdir",
	/*  90 */	"old_mmap",
	/*  91 */	"munmap",
	/*  92 */	"truncate",
#if !defined(_KERNEL) || defined(COMPAT_43)
	/*  93 */	"ftruncate",
#else
	/*  93 */	"#93 (unimplemented compat_43_sys_ftruncate)",
#endif
	/*  94 */	"fchmod",
	/*  95 */	"fchown16",
	/*  96 */	"getpriority",
	/*  97 */	"setpriority",
	/*  98 */	"profil",
	/*  99 */	"statfs",
	/* 100 */	"fstatfs",
	/* 101 */	"#101 (unimplemented ioperm)",
	/* 102 */	"socketcall",
	/* 103 */	"#103 (unimplemented syslog)",
	/* 104 */	"setitimer",
	/* 105 */	"getitimer",
	/* 106 */	"stat",
	/* 107 */	"lstat",
	/* 108 */	"fstat",
	/* 109 */	"#109 (unimplemented olduname)",
	/* 110 */	"#110 (unimplemented iopl)",
	/* 111 */	"#111 (unimplemented vhangup)",
	/* 112 */	"#112 (unimplemented idle)",
	/* 113 */	"#113 (unimplemented vm86old)",
	/* 114 */	"wait4",
	/* 115 */	"swapoff",
	/* 116 */	"sysinfo",
	/* 117 */	"ipc",
	/* 118 */	"fsync",
	/* 119 */	"sigreturn",
	/* 120 */	"clone",
	/* 121 */	"setdomainname",
	/* 122 */	"uname",
	/* 123 */	"cacheflush",
	/* 124 */	"#124 (unimplemented adjtimex)",
	/* 125 */	"mprotect",
	/* 126 */	"sigprocmask",
	/* 127 */	"#127 (unimplemented create_module)",
	/* 128 */	"#128 (unimplemented init_module)",
	/* 129 */	"#129 (unimplemented delete_module)",
	/* 130 */	"#130 (unimplemented get_kernel_syms)",
	/* 131 */	"#131 (unimplemented quotactl)",
	/* 132 */	"getpgid",
	/* 133 */	"fchdir",
	/* 134 */	"#134 (unimplemented bdflush)",
	/* 135 */	"#135 (unimplemented sysfs)",
	/* 136 */	"personality",
	/* 137 */	"#137 (unimplemented afs_syscall)",
	/* 138 */	"linux_setfsuid16",
	/* 139 */	"linux_setfsgid16",
	/* 140 */	"llseek",
	/* 141 */	"getdents",
	/* 142 */	"select",
	/* 143 */	"flock",
	/* 144 */	"__msync13",
	/* 145 */	"readv",
	/* 146 */	"writev",
	/* 147 */	"getsid",
	/* 148 */	"fdatasync",
	/* 149 */	"__sysctl",
	/* 150 */	"mlock",
	/* 151 */	"munlock",
	/* 152 */	"mlockall",
	/* 153 */	"munlockall",
	/* 154 */	"sched_setparam",
	/* 155 */	"sched_getparam",
	/* 156 */	"sched_setscheduler",
	/* 157 */	"sched_getscheduler",
	/* 158 */	"sched_yield",
	/* 159 */	"sched_get_priority_max",
	/* 160 */	"sched_get_priority_min",
	/* 161 */	"#161 (unimplemented sched_rr_get_interval)",
	/* 162 */	"nanosleep",
	/* 163 */	"mremap",
	/* 164 */	"setresuid16",
	/* 165 */	"getresuid16",
	/* 166 */	"#166 (unimplemented vm86)",
	/* 167 */	"#167 (unimplemented query_module)",
	/* 168 */	"poll",
	/* 169 */	"#169 (unimplemented nfsservctl)",
	/* 170 */	"setresgid16",
	/* 171 */	"getresgid16",
	/* 172 */	"#172 (unimplemented prctl)",
	/* 173 */	"rt_sigreturn",
	/* 174 */	"rt_sigaction",
	/* 175 */	"rt_sigprocmask",
	/* 176 */	"rt_sigpending",
	/* 177 */	"#177 (unimplemented rt_sigtimedwait)",
	/* 178 */	"rt_queueinfo",
	/* 179 */	"rt_sigsuspend",
	/* 180 */	"pread",
	/* 181 */	"pwrite",
	/* 182 */	"lchown16",
	/* 183 */	"__getcwd",
	/* 184 */	"#184 (unimplemented capget)",
	/* 185 */	"#185 (unimplemented capset)",
	/* 186 */	"sigaltstack",
	/* 187 */	"#187 (unimplemented sendfile)",
	/* 188 */	"#188 (unimplemented getpmsg)",
	/* 189 */	"#189 (unimplemented putpmsg)",
	/* 190 */	"__vfork14",
	/* 191 */	"ugetrlimit",
#define linux_sys_mmap2_args linux_sys_mmap_args
	/* 192 */	"mmap2",
	/* 193 */	"truncate64",
	/* 194 */	"ftruncate64",
	/* 195 */	"stat64",
	/* 196 */	"lstat64",
	/* 197 */	"fstat64",
	/* 198 */	"__posix_chown",
	/* 199 */	"getuid",
	/* 200 */	"getgid",
	/* 201 */	"geteuid",
	/* 202 */	"getegid",
	/* 203 */	"setreuid",
	/* 204 */	"setregid",
	/* 205 */	"getgroups",
	/* 206 */	"setgroups",
	/* 207 */	"__posix_fchown",
	/* 208 */	"setresuid",
	/* 209 */	"getresuid",
	/* 210 */	"setresgid",
	/* 211 */	"getresgid",
	/* 212 */	"__posix_lchown",
	/* 213 */	"setuid",
	/* 214 */	"setgid",
	/* 215 */	"setfsuid",
	/* 216 */	"setfsgid",
	/* 217 */	"#217 (unimplemented / * unused * /)",
	/* 218 */	"#218 (unimplemented / * unused * /)",
	/* 219 */	"#219 (unimplemented / * unused * /)",
	/* 220 */	"getdents64",
	/* 221 */	"#221 (unimplemented gettid)",
	/* 222 */	"#222 (unimplemented tkill)",
	/* 223 */	"setxattr",
	/* 224 */	"lsetxattr",
	/* 225 */	"fsetxattr",
	/* 226 */	"getxattr",
	/* 227 */	"lgetxattr",
	/* 228 */	"fgetxattr",
	/* 229 */	"listxattr",
	/* 230 */	"llistxattr",
	/* 231 */	"flistxattr",
	/* 232 */	"removexattr",
	/* 233 */	"lremovexattr",
	/* 234 */	"fremovexattr",
	/* 235 */	"#235 (unimplemented futex)",
	/* 236 */	"#236 (unimplemented sendfile64)",
	/* 237 */	"#237 (unimplemented mincore)",
	/* 238 */	"#238 (unimplemented madvise)",
	/* 239 */	"#239 (unimplemented fcntl64)",
	/* 240 */	"#240 (unimplemented readahead)",
	/* 241 */	"#241 (unimplemented io_setup)",
	/* 242 */	"#242 (unimplemented io_destroy)",
	/* 243 */	"#243 (unimplemented io_getevents)",
	/* 244 */	"#244 (unimplemented io_submit)",
	/* 245 */	"#245 (unimplemented io_cancel)",
	/* 246 */	"#246 (unimplemented fadvise64)",
	/* 247 */	"#247 (unimplemented exit_group)",
	/* 248 */	"#248 (unimplemented lookup_dcookie)",
	/* 249 */	"#249 (unimplemented epoll_create)",
	/* 250 */	"#250 (unimplemented epoll_ctl)",
	/* 251 */	"#251 (unimplemented epoll_wait)",
	/* 252 */	"#252 (unimplemented remap_file_pages)",
	/* 253 */	"#253 (unimplemented set_tid_address)",
	/* 254 */	"#254 (unimplemented timer_create)",
	/* 255 */	"#255 (unimplemented timer_settime)",
	/* 256 */	"#256 (unimplemented timer_gettime)",
	/* 257 */	"#257 (unimplemented timer_getoverrun)",
	/* 258 */	"#258 (unimplemented timer_delete)",
	/* 259 */	"clock_settime",
	/* 260 */	"clock_gettime",
	/* 261 */	"clock_getres",
	/* 262 */	"clock_nanosleep",
};
