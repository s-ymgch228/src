/* $NetBSD: linux_sysent.c,v 1.40 2001/11/13 02:08:41 lukem Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.60 2001/09/30 20:44:50 manu Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: linux_sysent.c,v 1.40 2001/11/13 02:08:41 lukem Exp $");

#if defined(_KERNEL_OPT)
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/poll.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent linux_sysent[] = {
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args), 0,
	    sys_exit },				/* 1 = exit */
	{ 0, 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct sys_read_args), 0,
	    sys_read },				/* 3 = read */
	{ 3, s(struct sys_write_args), 0,
	    sys_write },			/* 4 = write */
	{ 3, s(struct linux_sys_open_args), 0,
	    linux_sys_open },			/* 5 = open */
	{ 1, s(struct sys_close_args), 0,
	    sys_close },			/* 6 = close */
	{ 3, s(struct linux_sys_waitpid_args), 0,
	    linux_sys_waitpid },		/* 7 = waitpid */
	{ 2, s(struct linux_sys_creat_args), 0,
	    linux_sys_creat },			/* 8 = creat */
	{ 2, s(struct linux_sys_link_args), 0,
	    linux_sys_link },			/* 9 = link */
	{ 1, s(struct linux_sys_unlink_args), 0,
	    linux_sys_unlink },			/* 10 = unlink */
	{ 3, s(struct linux_sys_execve_args), 0,
	    linux_sys_execve },			/* 11 = execve */
	{ 1, s(struct linux_sys_chdir_args), 0,
	    linux_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct linux_sys_time_args), 0,
	    linux_sys_time },			/* 13 = time */
	{ 3, s(struct linux_sys_mknod_args), 0,
	    linux_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct linux_sys_chmod_args), 0,
	    linux_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct linux_sys_lchown16_args), 0,
	    linux_sys_lchown16 },		/* 16 = lchown16 */
	{ 1, s(struct linux_sys_break_args), 0,
	    linux_sys_break },			/* 17 = break */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 18 = obsolete ostat */
	{ 3, s(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0, SYCALL_MPSAFE | 0,
	    sys_getpid },			/* 20 = getpid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 21 = unimplemented mount */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 22 = unimplemented umount */
	{ 1, s(struct sys_setuid_args), 0,
	    sys_setuid },			/* 23 = linux_setuid16 */
	{ 0, 0, 0,
	    sys_getuid },			/* 24 = linux_getuid16 */
	{ 1, s(struct linux_sys_stime_args), 0,
	    linux_sys_stime },			/* 25 = stime */
	{ 4, s(struct linux_sys_ptrace_args), 0,
	    linux_sys_ptrace },			/* 26 = ptrace */
	{ 1, s(struct linux_sys_alarm_args), 0,
	    linux_sys_alarm },			/* 27 = alarm */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 28 = obsolete ofstat */
	{ 0, 0, 0,
	    linux_sys_pause },			/* 29 = pause */
	{ 2, s(struct linux_sys_utime_args), 0,
	    linux_sys_utime },			/* 30 = utime */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 31 = obsolete stty */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 32 = obsolete gtty */
	{ 2, s(struct linux_sys_access_args), 0,
	    linux_sys_access },			/* 33 = access */
	{ 1, s(struct linux_sys_nice_args), 0,
	    linux_sys_nice },			/* 34 = nice */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 35 = obsolete ftime */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct linux_sys_kill_args), 0,
	    linux_sys_kill },			/* 37 = kill */
	{ 2, s(struct linux_sys_rename_args), 0,
	    linux_sys_rename },			/* 38 = rename */
	{ 2, s(struct linux_sys_mkdir_args), 0,
	    linux_sys_mkdir },			/* 39 = mkdir */
	{ 1, s(struct linux_sys_rmdir_args), 0,
	    linux_sys_rmdir },			/* 40 = rmdir */
	{ 1, s(struct sys_dup_args), 0,
	    sys_dup },				/* 41 = dup */
	{ 1, s(struct linux_sys_pipe_args), 0,
	    linux_sys_pipe },			/* 42 = pipe */
	{ 1, s(struct linux_sys_times_args), 0,
	    linux_sys_times },			/* 43 = times */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 44 = obsolete prof */
	{ 1, s(struct linux_sys_brk_args), 0,
	    linux_sys_brk },			/* 45 = brk */
	{ 1, s(struct sys_setgid_args), 0,
	    sys_setgid },			/* 46 = linux_setgid16 */
	{ 0, 0, 0,
	    sys_getgid },			/* 47 = linux_getgid16 */
	{ 2, s(struct linux_sys_signal_args), 0,
	    linux_sys_signal },			/* 48 = signal */
	{ 0, 0, 0,
	    sys_geteuid },			/* 49 = linux_geteuid16 */
	{ 0, 0, 0,
	    sys_getegid },			/* 50 = linux_getegid16 */
	{ 1, s(struct sys_acct_args), 0,
	    sys_acct },				/* 51 = acct */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 52 = obsolete phys */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 53 = obsolete lock */
	{ 3, s(struct linux_sys_ioctl_args), 0,
	    linux_sys_ioctl },			/* 54 = ioctl */
	{ 3, s(struct linux_sys_fcntl_args), 0,
	    linux_sys_fcntl },			/* 55 = fcntl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 56 = obsolete mpx */
	{ 2, s(struct sys_setpgid_args), 0,
	    sys_setpgid },			/* 57 = setpgid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 58 = obsolete ulimit */
	{ 1, s(struct linux_sys_oldolduname_args), 0,
	    linux_sys_oldolduname },		/* 59 = oldolduname */
	{ 1, s(struct sys_umask_args), 0,
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args), 0,
	    sys_chroot },			/* 61 = chroot */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 62 = unimplemented ustat */
	{ 2, s(struct sys_dup2_args), 0,
	    sys_dup2 },				/* 63 = dup2 */
	{ 0, 0, 0,
	    sys_getppid },			/* 64 = getppid */
	{ 0, 0, 0,
	    sys_getpgrp },			/* 65 = getpgrp */
	{ 0, 0, 0,
	    sys_setsid },			/* 66 = setsid */
	{ 3, s(struct linux_sys_sigaction_args), 0,
	    linux_sys_sigaction },		/* 67 = sigaction */
	{ 0, 0, 0,
	    linux_sys_siggetmask },		/* 68 = siggetmask */
	{ 1, s(struct linux_sys_sigsetmask_args), 0,
	    linux_sys_sigsetmask },		/* 69 = sigsetmask */
	{ 2, s(struct linux_sys_setreuid16_args), 0,
	    linux_sys_setreuid16 },		/* 70 = setreuid16 */
	{ 2, s(struct linux_sys_setregid16_args), 0,
	    linux_sys_setregid16 },		/* 71 = setregid16 */
	{ 3, s(struct linux_sys_sigsuspend_args), 0,
	    linux_sys_sigsuspend },		/* 72 = sigsuspend */
	{ 1, s(struct linux_sys_sigpending_args), 0,
	    linux_sys_sigpending },		/* 73 = sigpending */
	{ 2, s(struct compat_43_sys_sethostname_args), 0,
	    compat_43_sys_sethostname },	/* 74 = sethostname */
	{ 2, s(struct compat_43_sys_setrlimit_args), 0,
	    compat_43_sys_setrlimit },		/* 75 = setrlimit */
	{ 2, s(struct compat_43_sys_getrlimit_args), 0,
	    compat_43_sys_getrlimit },		/* 76 = getrlimit */
	{ 2, s(struct sys_getrusage_args), 0,
	    sys_getrusage },			/* 77 = getrusage */
	{ 2, s(struct linux_sys_gettimeofday_args), 0,
	    linux_sys_gettimeofday },		/* 78 = gettimeofday */
	{ 2, s(struct linux_sys_settimeofday_args), 0,
	    linux_sys_settimeofday },		/* 79 = settimeofday */
	{ 2, s(struct linux_sys_getgroups16_args), 0,
	    linux_sys_getgroups16 },		/* 80 = getgroups16 */
	{ 2, s(struct linux_sys_setgroups16_args), 0,
	    linux_sys_setgroups16 },		/* 81 = setgroups16 */
	{ 1, s(struct linux_sys_oldselect_args), 0,
	    linux_sys_oldselect },		/* 82 = oldselect */
	{ 2, s(struct linux_sys_symlink_args), 0,
	    linux_sys_symlink },		/* 83 = symlink */
	{ 2, s(struct compat_43_sys_lstat_args), 0,
	    compat_43_sys_lstat },		/* 84 = oolstat */
	{ 3, s(struct linux_sys_readlink_args), 0,
	    linux_sys_readlink },		/* 85 = readlink */
	{ 1, s(struct linux_sys_uselib_args), 0,
	    linux_sys_uselib },			/* 86 = uselib */
	{ 1, s(struct linux_sys_swapon_args), 0,
	    linux_sys_swapon },			/* 87 = swapon */
	{ 4, s(struct linux_sys_reboot_args), 0,
	    linux_sys_reboot },			/* 88 = reboot */
	{ 3, s(struct linux_sys_readdir_args), 0,
	    linux_sys_readdir },		/* 89 = readdir */
	{ 1, s(struct linux_sys_old_mmap_args), 0,
	    linux_sys_old_mmap },		/* 90 = old_mmap */
	{ 2, s(struct sys_munmap_args), 0,
	    sys_munmap },			/* 91 = munmap */
	{ 2, s(struct linux_sys_truncate_args), 0,
	    linux_sys_truncate },		/* 92 = truncate */
	{ 2, s(struct compat_43_sys_ftruncate_args), 0,
	    compat_43_sys_ftruncate },		/* 93 = ftruncate */
	{ 2, s(struct sys_fchmod_args), 0,
	    sys_fchmod },			/* 94 = fchmod */
	{ 3, s(struct linux_sys_fchown16_args), 0,
	    linux_sys_fchown16 },		/* 95 = fchown16 */
	{ 2, s(struct sys_getpriority_args), 0,
	    sys_getpriority },			/* 96 = getpriority */
	{ 3, s(struct sys_setpriority_args), 0,
	    sys_setpriority },			/* 97 = setpriority */
	{ 4, s(struct sys_profil_args), 0,
	    sys_profil },			/* 98 = profil */
	{ 2, s(struct linux_sys_statfs_args), 0,
	    linux_sys_statfs },			/* 99 = statfs */
	{ 2, s(struct linux_sys_fstatfs_args), 0,
	    linux_sys_fstatfs },		/* 100 = fstatfs */
	{ 3, s(struct linux_sys_ioperm_args), 0,
	    linux_sys_ioperm },			/* 101 = ioperm */
	{ 2, s(struct linux_sys_socketcall_args), 0,
	    linux_sys_socketcall },		/* 102 = socketcall */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 103 = unimplemented syslog */
	{ 3, s(struct sys_setitimer_args), 0,
	    sys_setitimer },			/* 104 = setitimer */
	{ 2, s(struct sys_getitimer_args), 0,
	    sys_getitimer },			/* 105 = getitimer */
	{ 2, s(struct linux_sys_stat_args), 0,
	    linux_sys_stat },			/* 106 = stat */
	{ 2, s(struct linux_sys_lstat_args), 0,
	    linux_sys_lstat },			/* 107 = lstat */
	{ 2, s(struct linux_sys_fstat_args), 0,
	    linux_sys_fstat },			/* 108 = fstat */
	{ 1, s(struct linux_sys_olduname_args), 0,
	    linux_sys_olduname },		/* 109 = olduname */
	{ 1, s(struct linux_sys_iopl_args), 0,
	    linux_sys_iopl },			/* 110 = iopl */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 111 = unimplemented vhangup */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 112 = unimplemented idle */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 113 = unimplemented vm86old */
	{ 4, s(struct linux_sys_wait4_args), 0,
	    linux_sys_wait4 },			/* 114 = wait4 */
	{ 1, s(struct linux_sys_swapoff_args), 0,
	    linux_sys_swapoff },		/* 115 = swapoff */
	{ 1, s(struct linux_sys_sysinfo_args), 0,
	    linux_sys_sysinfo },		/* 116 = sysinfo */
	{ 5, s(struct linux_sys_ipc_args), 0,
	    linux_sys_ipc },			/* 117 = ipc */
	{ 1, s(struct sys_fsync_args), 0,
	    sys_fsync },			/* 118 = fsync */
	{ 1, s(struct linux_sys_sigreturn_args), 0,
	    linux_sys_sigreturn },		/* 119 = sigreturn */
	{ 2, s(struct linux_sys_clone_args), 0,
	    linux_sys_clone },			/* 120 = clone */
	{ 2, s(struct linux_sys_setdomainname_args), 0,
	    linux_sys_setdomainname },		/* 121 = setdomainname */
	{ 1, s(struct linux_sys_uname_args), 0,
	    linux_sys_uname },			/* 122 = uname */
	{ 3, s(struct linux_sys_modify_ldt_args), 0,
	    linux_sys_modify_ldt },		/* 123 = modify_ldt */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 124 = unimplemented adjtimex */
	{ 3, s(struct sys_mprotect_args), 0,
	    sys_mprotect },			/* 125 = mprotect */
	{ 3, s(struct linux_sys_sigprocmask_args), 0,
	    linux_sys_sigprocmask },		/* 126 = sigprocmask */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 127 = unimplemented create_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 128 = unimplemented init_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 129 = unimplemented delete_module */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 130 = unimplemented get_kernel_syms */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 131 = unimplemented quotactl */
	{ 1, s(struct linux_sys_getpgid_args), 0,
	    linux_sys_getpgid },		/* 132 = getpgid */
	{ 1, s(struct sys_fchdir_args), 0,
	    sys_fchdir },			/* 133 = fchdir */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 134 = unimplemented bdflush */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 135 = unimplemented sysfs */
	{ 1, s(struct linux_sys_personality_args), 0,
	    linux_sys_personality },		/* 136 = personality */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 137 = unimplemented afs_syscall */
	{ 1, s(struct linux_sys_setfsuid_args), 0,
	    linux_sys_setfsuid },		/* 138 = linux_setfsuid16 */
	{ 0, 0, 0,
	    linux_sys_getfsuid },		/* 139 = linux_getfsuid16 */
	{ 5, s(struct linux_sys_llseek_args), 0,
	    linux_sys_llseek },			/* 140 = llseek */
	{ 3, s(struct linux_sys_getdents_args), 0,
	    linux_sys_getdents },		/* 141 = getdents */
	{ 5, s(struct linux_sys_select_args), 0,
	    linux_sys_select },			/* 142 = select */
	{ 2, s(struct sys_flock_args), 0,
	    sys_flock },			/* 143 = flock */
	{ 3, s(struct linux_sys_msync_args), 0,
	    linux_sys_msync },			/* 144 = msync */
	{ 3, s(struct sys_readv_args), 0,
	    sys_readv },			/* 145 = readv */
	{ 3, s(struct sys_writev_args), 0,
	    sys_writev },			/* 146 = writev */
	{ 1, s(struct sys_getsid_args), 0,
	    sys_getsid },			/* 147 = getsid */
	{ 1, s(struct linux_sys_fdatasync_args), 0,
	    linux_sys_fdatasync },		/* 148 = fdatasync */
	{ 1, s(struct linux_sys___sysctl_args), 0,
	    linux_sys___sysctl },		/* 149 = __sysctl */
	{ 2, s(struct sys_mlock_args), 0,
	    sys_mlock },			/* 150 = mlock */
	{ 2, s(struct sys_munlock_args), 0,
	    sys_munlock },			/* 151 = munlock */
	{ 1, s(struct sys_mlockall_args), 0,
	    sys_mlockall },			/* 152 = mlockall */
	{ 0, 0, 0,
	    sys_munlockall },			/* 153 = munlockall */
	{ 2, s(struct linux_sys_sched_setparam_args), 0,
	    linux_sys_sched_setparam },		/* 154 = sched_setparam */
	{ 2, s(struct linux_sys_sched_getparam_args), 0,
	    linux_sys_sched_getparam },		/* 155 = sched_getparam */
	{ 3, s(struct linux_sys_sched_setscheduler_args), 0,
	    linux_sys_sched_setscheduler },	/* 156 = sched_setscheduler */
	{ 1, s(struct linux_sys_sched_getscheduler_args), 0,
	    linux_sys_sched_getscheduler },	/* 157 = sched_getscheduler */
	{ 0, 0, 0,
	    linux_sys_sched_yield },		/* 158 = sched_yield */
	{ 1, s(struct linux_sys_sched_get_priority_max_args), 0,
	    linux_sys_sched_get_priority_max },	/* 159 = sched_get_priority_max */
	{ 1, s(struct linux_sys_sched_get_priority_min_args), 0,
	    linux_sys_sched_get_priority_min },	/* 160 = sched_get_priority_min */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 161 = unimplemented sys_sched_rr_get_interval */
	{ 2, s(struct sys_nanosleep_args), 0,
	    sys_nanosleep },			/* 162 = nanosleep */
	{ 4, s(struct linux_sys_mremap_args), 0,
	    linux_sys_mremap },			/* 163 = mremap */
	{ 3, s(struct linux_sys_setresuid16_args), 0,
	    linux_sys_setresuid16 },		/* 164 = setresuid16 */
	{ 3, s(struct linux_sys_getresuid_args), 0,
	    linux_sys_getresuid },		/* 165 = linux_getresuid16 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 166 = unimplemented vm86 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 167 = unimplemented query_module */
	{ 3, s(struct sys_poll_args), 0,
	    sys_poll },				/* 168 = poll */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 169 = unimplemented nfsservctl */
	{ 3, s(struct linux_sys_setresgid16_args), 0,
	    linux_sys_setresgid16 },		/* 170 = setresgid16 */
	{ 3, s(struct linux_sys_getresgid_args), 0,
	    linux_sys_getresgid },		/* 171 = linux_getresgid16 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 172 = unimplemented prctl */
	{ 1, s(struct linux_sys_rt_sigreturn_args), 0,
	    linux_sys_rt_sigreturn },		/* 173 = rt_sigreturn */
	{ 4, s(struct linux_sys_rt_sigaction_args), 0,
	    linux_sys_rt_sigaction },		/* 174 = rt_sigaction */
	{ 4, s(struct linux_sys_rt_sigprocmask_args), 0,
	    linux_sys_rt_sigprocmask },		/* 175 = rt_sigprocmask */
	{ 2, s(struct linux_sys_rt_sigpending_args), 0,
	    linux_sys_rt_sigpending },		/* 176 = rt_sigpending */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 177 = unimplemented rt_sigtimedwait */
	{ 3, s(struct linux_sys_rt_queueinfo_args), 0,
	    linux_sys_rt_queueinfo },		/* 178 = rt_queueinfo */
	{ 2, s(struct linux_sys_rt_sigsuspend_args), 0,
	    linux_sys_rt_sigsuspend },		/* 179 = rt_sigsuspend */
	{ 4, s(struct linux_sys_pread_args), 0,
	    linux_sys_pread },			/* 180 = pread */
	{ 4, s(struct linux_sys_pwrite_args), 0,
	    linux_sys_pwrite },			/* 181 = pwrite */
	{ 3, s(struct linux_sys_chown16_args), 0,
	    linux_sys_chown16 },		/* 182 = chown16 */
	{ 2, s(struct sys___getcwd_args), 0,
	    sys___getcwd },			/* 183 = __getcwd */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 184 = unimplemented capget */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 185 = unimplemented capset */
	{ 2, s(struct linux_sys_sigaltstack_args), 0,
	    linux_sys_sigaltstack },		/* 186 = sigaltstack */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 187 = unimplemented sendfile */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 188 = unimplemented getpmsg */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 189 = unimplemented putpmsg */
	{ 0, 0, 0,
	    sys___vfork14 },			/* 190 = __vfork14 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 191 = unimplemented getrlimit */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 192 = unimplemented mmap2 */
	{ 2, s(struct linux_sys_truncate64_args), 0,
	    linux_sys_truncate64 },		/* 193 = truncate64 */
	{ 2, s(struct sys_ftruncate_args), 0,
	    sys_ftruncate },			/* 194 = linux_ftruncate64 */
	{ 2, s(struct linux_sys_stat64_args), 0,
	    linux_sys_stat64 },			/* 195 = stat64 */
	{ 2, s(struct linux_sys_lstat64_args), 0,
	    linux_sys_lstat64 },		/* 196 = lstat64 */
	{ 2, s(struct linux_sys_fstat64_args), 0,
	    linux_sys_fstat64 },		/* 197 = fstat64 */
	{ 3, s(struct linux_sys_lchown_args), 0,
	    linux_sys_lchown },			/* 198 = lchown */
	{ 0, 0, 0,
	    sys_getuid },			/* 199 = getuid */
	{ 0, 0, 0,
	    sys_getgid },			/* 200 = getgid */
	{ 0, 0, 0,
	    sys_geteuid },			/* 201 = geteuid */
	{ 0, 0, 0,
	    sys_getegid },			/* 202 = getegid */
	{ 2, s(struct sys_setreuid_args), 0,
	    sys_setreuid },			/* 203 = setreuid */
	{ 2, s(struct sys_setregid_args), 0,
	    sys_setregid },			/* 204 = setregid */
	{ 2, s(struct sys_getgroups_args), 0,
	    sys_getgroups },			/* 205 = getgroups */
	{ 2, s(struct sys_setgroups_args), 0,
	    sys_setgroups },			/* 206 = setgroups */
	{ 3, s(struct sys___posix_fchown_args), 0,
	    sys___posix_fchown },		/* 207 = __posix_fchown */
	{ 3, s(struct linux_sys_setresuid_args), 0,
	    linux_sys_setresuid },		/* 208 = setresuid */
	{ 3, s(struct linux_sys_getresuid_args), 0,
	    linux_sys_getresuid },		/* 209 = getresuid */
	{ 3, s(struct linux_sys_setresgid_args), 0,
	    linux_sys_setresgid },		/* 210 = setresgid */
	{ 3, s(struct linux_sys_getresgid_args), 0,
	    linux_sys_getresgid },		/* 211 = getresgid */
	{ 3, s(struct linux_sys_chown_args), 0,
	    linux_sys_chown },			/* 212 = chown */
	{ 1, s(struct sys_setuid_args), 0,
	    sys_setuid },			/* 213 = setuid */
	{ 1, s(struct sys_setgid_args), 0,
	    sys_setgid },			/* 214 = setgid */
	{ 1, s(struct linux_sys_setfsuid_args), 0,
	    linux_sys_setfsuid },		/* 215 = setfsuid */
	{ 0, 0, 0,
	    linux_sys_getfsuid },		/* 216 = getfsuid */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 217 = unimplemented pivot_root */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 218 = unimplemented mincore */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 219 = unimplemented madvise */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 220 = unimplemented getdents64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 221 = unimplemented fcntl64 */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 222 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 223 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 224 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 225 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 226 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 227 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 228 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 229 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 230 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 231 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 232 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 233 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 234 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 235 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 236 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 237 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 238 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 239 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 240 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 241 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 242 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 243 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 244 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 245 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 246 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 247 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 248 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 249 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 250 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 251 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 252 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 253 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 254 = filler */
	{ 0, 0, 0,
	    linux_sys_nosys },			/* 255 = filler */
};

