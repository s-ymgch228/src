/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from: NetBSD syscalls.master,v 1.4 1994/06/29 06:30:40
 */

#include <sys/param.h>
#include <sys/systm.h>

int	nosys();

int	nosys();
int	exit();
int	fork();
int	read();
int	write();
int	sun_open();
int	close();
int	sun_creat();
int	link();
int	unlink();
int	sun_execv();
int	chdir();
int	sun_mknod();
int	chmod();
int	chown();
int	obreak();
int	olseek();
int	getpid();
int	sun_mount();
int	sun_unmount();
int	getuid();
int	access();
int	sync();
int	kill();
int	ostat();
int	olstat();
int	dup();
int	pipe();
int	profil();
int	getgid();
int	acct();
int	ioctl();
int	reboot();
int	symlink();
int	readlink();
int	execve();
int	umask();
int	chroot();
int	ofstat();
int	ogetpagesize();
int	vfork();
int	sbrk();
int	sstk();
int	sun_mmap();
int	ovadvise();
int	munmap();
int	mprotect();
int	madvise();
int	sun_vhangup();
int	mincore();
int	getgroups();
int	setgroups();
int	getpgrp();
int	sun_setpgid();
int	setitimer();
int	sun_wait3();
int	swapon();
int	getitimer();
int	ogethostname();
int	osethostname();
int	ogetdtablesize();
int	dup2();
int	fcntl();
int	select();
int	fsync();
int	setpriority();
int	socket();
int	connect();
int	oaccept();
int	getpriority();
int	osend();
int	orecv();
int	sigreturn();
int	bind();
int	sun_setsockopt();
int	listen();
int	osigvec();
int	osigblock();
int	osigsetmask();
int	sigsuspend();
int	osigstack();
int	orecvmsg();
int	osendmsg();
int	gettimeofday();
int	getrusage();
int	getsockopt();
int	readv();
int	writev();
int	settimeofday();
int	fchown();
int	fchmod();
int	orecvfrom();
int	osetreuid();
int	osetregid();
int	rename();
int	otruncate();
int	oftruncate();
int	flock();
int	sendto();
int	shutdown();
int	socketpair();
int	mkdir();
int	rmdir();
int	utimes();
int	sigreturn();
int	adjtime();
int	ogetpeername();
int	ogethostid();
int	sun_getrlimit();
int	sun_setrlimit();
int	okillpg();
int	ogetsockname();
int	nfssvc();
int	getdirentries();
int	sun_statfs();
int	sun_fstatfs();
int	sun_unmount();
int	async_daemon();
int	getfh();
int	sun_quotactl();
int	sun_exportfs();
int	sun_mount();
#ifdef SYSVMSG
#else
#endif
#ifdef SYSVSEM
#else
#endif
int	sun_uname();
#ifdef SYSVSHM
int	shmsys();
#else
#endif
int	sun_ustat();
int	sun_sigpending();
int	setsid();
int	ultrix_waitpid();

#ifdef XXX_UNUSED
#define compat(n, name) n, __CONCAT(o,name)

int	osun_time();
int	osun_stat();
int	osetuid();
int	osun_stime();
int	osun_alarm();
int	osun_fstat();
int	osun_pause();
int	osun_utime();
int	osun_nice();
int	osun_ftime();
int	osun_setpgrp();
int	osun_times();
int	osun_setgid();
int	osun_ssig();
int	ovlimit();
int	ovtimes();
#ifdef SYSVMSG
#else
#endif
#ifdef SYSVSEM
#else
#endif
#ifdef SYSVSHM
#else
#endif

#else /* XXX_UNUSED */
#define compat(n, name) 0, nosys
#endif /* XXX_UNUSED */

struct sysent ultrix_sysent[] = {
	{ 0, nosys },			/* 0 = syscall */
	{ 1, exit },			/* 1 = exit */
	{ 0, fork },			/* 2 = fork */
	{ 3, read },			/* 3 = read */
	{ 3, write },			/* 4 = write */
	{ 3, sun_open },			/* 5 = sun_open */
	{ 1, close },			/* 6 = close */
	{ 0, nosys },			/* 7 = old_wait */
	{ 2, sun_creat },			/* 8 = sun_creat */
	{ 2, link },			/* 9 = link */
	{ 1, unlink },			/* 10 = unlink */
	{ 2, sun_execv },			/* 11 = sun_execv */
	{ 1, chdir },			/* 12 = chdir */
	{ compat(0,sun_time) },		/* 13 = old sun_time */
	{ 3, sun_mknod },			/* 14 = sun_mknod */
	{ 2, chmod },			/* 15 = chmod */
	{ 3, chown },			/* 16 = chown */
	{ 1, obreak },			/* 17 = break */
	{ compat(2,sun_stat) },		/* 18 = old sun_stat */
	{ 3, olseek },			/* 19 = lseek */
	{ 0, getpid },			/* 20 = getpid */
	{ 4, sun_mount },			/* 21 = sun_mount */
	{ 2, sun_unmount },			/* 22 = sun_unmount */
	{ compat(1,setuid) },		/* 23 = old setuid */
	{ 0, getuid },			/* 24 = getuid */
	{ compat(1,sun_stime) },		/* 25 = old sun_stime */
	{ 0, nosys },			/* 26 = sun_ptrace */
	{ compat(1,sun_alarm) },		/* 27 = old sun_alarm */
	{ compat(1,sun_fstat) },		/* 28 = old sun_fstat */
	{ compat(0,sun_pause) },		/* 29 = old sun_pause */
	{ compat(2,sun_utime) },		/* 30 = old sun_utime */
	{ 0, nosys },			/* 31 = was stty */
	{ 0, nosys },			/* 32 = was gtty */
	{ 2, access },			/* 33 = access */
	{ compat(1,sun_nice) },		/* 34 = old sun_nice */
	{ compat(1,sun_ftime) },		/* 35 = old sun_ftime */
	{ 0, sync },			/* 36 = sync */
	{ 2, kill },			/* 37 = kill */
	{ 2, ostat },			/* 38 = stat */
	{ compat(2,sun_setpgrp) },		/* 39 = old sun_setpgrp */
	{ 2, olstat },			/* 40 = lstat */
	{ 2, dup },			/* 41 = dup */
	{ 0, pipe },			/* 42 = pipe */
	{ compat(1,sun_times) },		/* 43 = old sun_times */
	{ 4, profil },			/* 44 = profil */
	{ 0, nosys },			/* 45 = nosys */
	{ compat(1,sun_setgid) },		/* 46 = old sun_setgid */
	{ 0, getgid },			/* 47 = getgid */
	{ compat(2,sun_ssig) },		/* 48 = old sun_ssig */
	{ 0, nosys },			/* 49 = reserved for USG */
	{ 0, nosys },			/* 50 = reserved for USG */
	{ 1, acct },			/* 51 = acct */
	{ 0, nosys },			/* 52 = nosys */
	{ 0, nosys },			/* 53 = syslock */
	{ 3, ioctl },			/* 54 = ioctl */
	{ 2, reboot },			/* 55 = reboot */
	{ 0, nosys },			/* 56 = mpxchan */
	{ 2, symlink },			/* 57 = symlink */
	{ 3, readlink },			/* 58 = readlink */
	{ 3, execve },			/* 59 = execve */
	{ 1, umask },			/* 60 = umask */
	{ 1, chroot },			/* 61 = chroot */
	{ 2, ofstat },			/* 62 = ofstat */
	{ 0, nosys },			/* 63 = nosys */
	{ 0, ogetpagesize },			/* 64 = ogetpagesize */
	{ 0, nosys },			/* 65 = mremap */
	{ 0, vfork },			/* 66 = vfork */
	{ 0, nosys },			/* 67 = obsolete vread */
	{ 0, nosys },			/* 68 = obsolete vwrite */
	{ 1, sbrk },			/* 69 = sbrk */
	{ 1, sstk },			/* 70 = sstk */
	{ 6, sun_mmap },			/* 71 = mmap */
	{ 1, ovadvise },			/* 72 = vadvise */
	{ 2, munmap },			/* 73 = munmap */
	{ 3, mprotect },			/* 74 = mprotect */
	{ 3, madvise },			/* 75 = madvise */
	{ 0, sun_vhangup },			/* 76 = sun_vhangup */
	{ compat(0,vlimit) },		/* 77 = old vlimit */
	{ 3, mincore },			/* 78 = mincore */
	{ 2, getgroups },			/* 79 = getgroups */
	{ 2, setgroups },			/* 80 = setgroups */
	{ 1, getpgrp },			/* 81 = getpgrp */
	{ 2, sun_setpgid },			/* 82 = sun_setpgid */
	{ 3, setitimer },			/* 83 = setitimer */
	{ 3, sun_wait3 },			/* 84 = sun_wait3 */
	{ 1, swapon },			/* 85 = swapon */
	{ 2, getitimer },			/* 86 = getitimer */
	{ 2, ogethostname },			/* 87 = ogethostname */
	{ 2, osethostname },			/* 88 = osethostname */
	{ 0, ogetdtablesize },			/* 89 = ogetdtablesize */
	{ 2, dup2 },			/* 90 = dup2 */
	{ 0, nosys },			/* 91 = getdopt */
	{ 3, fcntl },			/* 92 = fcntl */
	{ 5, select },			/* 93 = select */
	{ 0, nosys },			/* 94 = setdopt */
	{ 1, fsync },			/* 95 = fsync */
	{ 3, setpriority },			/* 96 = setpriority */
	{ 3, socket },			/* 97 = socket */
	{ 3, connect },			/* 98 = connect */
	{ 3, oaccept },			/* 99 = oaccept */
	{ 2, getpriority },			/* 100 = getpriority */
	{ 4, osend },			/* 101 = osend */
	{ 4, orecv },			/* 102 = orecv */
	{ 1, sigreturn },			/* 103 = sigreturn */
	{ 3, bind },			/* 104 = bind */
	{ 5, sun_setsockopt },			/* 105 = sun_setsockopt */
	{ 2, listen },			/* 106 = listen */
	{ compat(0,vtimes) },		/* 107 = old vtimes */
	{ 3, osigvec },			/* 108 = osigvec */
	{ 1, osigblock },			/* 109 = osigblock */
	{ 1, osigsetmask },			/* 110 = osigsetmask */
	{ 1, sigsuspend },			/* 111 = sigsuspend */
	{ 2, osigstack },			/* 112 = osigstack */
	{ 3, orecvmsg },			/* 113 = orecvmsg */
	{ 3, osendmsg },			/* 114 = osendmsg */
	{ 0, nosys },			/* 115 = obsolete vtrace */
	{ 2, gettimeofday },			/* 116 = gettimeofday */
	{ 2, getrusage },			/* 117 = getrusage */
	{ 5, getsockopt },			/* 118 = getsockopt */
	{ 0, nosys },			/* 119 = resuba */
	{ 3, readv },			/* 120 = readv */
	{ 3, writev },			/* 121 = writev */
	{ 2, settimeofday },			/* 122 = settimeofday */
	{ 3, fchown },			/* 123 = fchown */
	{ 2, fchmod },			/* 124 = fchmod */
	{ 6, orecvfrom },			/* 125 = orecvfrom */
	{ 2, osetreuid },			/* 126 = osetreuid */
	{ 2, osetregid },			/* 127 = osetregid */
	{ 2, rename },			/* 128 = rename */
	{ 2, otruncate },			/* 129 = truncate */
	{ 2, oftruncate },			/* 130 = ftruncate */
	{ 2, flock },			/* 131 = flock */
	{ 0, nosys },			/* 132 = nosys */
	{ 6, sendto },			/* 133 = sendto */
	{ 2, shutdown },			/* 134 = shutdown */
	{ 5, socketpair },			/* 135 = socketpair */
	{ 2, mkdir },			/* 136 = mkdir */
	{ 1, rmdir },			/* 137 = rmdir */
	{ 2, utimes },			/* 138 = utimes */
	{ 1, sigreturn },			/* 139 = sigreturn */
	{ 2, adjtime },			/* 140 = adjtime */
	{ 3, ogetpeername },			/* 141 = ogetpeername */
	{ 0, ogethostid },			/* 142 = ogethostid */
	{ 0, nosys },			/* 143 = old sethostid */
	{ 2, sun_getrlimit },			/* 144 = sun_getrlimit */
	{ 2, sun_setrlimit },			/* 145 = sun_setrlimit */
	{ 2, okillpg },			/* 146 = okillpg */
	{ 0, nosys },			/* 147 = nosys */
	{ 0, nosys },			/* 148 = setquota */
	{ 0, nosys },			/* 149 = quota */
	{ 3, ogetsockname },			/* 150 = ogetsockname */
	{ 0, nosys },			/* 151 = sysmips */
	{ 0, nosys },			/* 152 = cacheflush */
	{ 0, nosys },			/* 153 = cachectl */
	{ 0, nosys },			/* 154 = nosys */
	{ 0, nosys },			/* 155 = atomic_op */
	{ 0, nosys },			/* 156 = nosys */
	{ 0, nosys },			/* 157 = nosys */
	{ 5, nfssvc },			/* 158 = nfssvc */
	{ 4, getdirentries },			/* 159 = getdirentries */
	{ 2, sun_statfs },			/* 160 = sun_statfs */
	{ 2, sun_fstatfs },			/* 161 = sun_fstatfs */
	{ 1, sun_unmount },			/* 162 = sun_unmount */
	{ 0, async_daemon },			/* 163 = async_daemon */
	{ 2, getfh },			/* 164 = getfh */
	{ 0, nosys },			/* 165 = ogetdomainname */
	{ 0, nosys },			/* 166 = osetdomainname */
	{ 0, nosys },			/* 167 = nosys */
	{ 4, sun_quotactl },			/* 168 = sun_quotactl */
	{ 2, sun_exportfs },			/* 169 = sun_exportfs */
	{ 4, sun_mount },			/* 170 = sun_mount */
	{ 0, nosys },			/* 171 = hdwconf */
#ifdef SYSVMSG
	{ 0, nosys },			/* 172 = msgctl */
	{ 0, nosys },			/* 173 = msgget */
	{ 0, nosys },			/* 174 = msgrcv */
	{ 0, nosys },			/* 175 = msgsnd */
#else
	{ 0, nosys },			/* 172 = msgctl */
	{ 0, nosys },			/* 173 = msgget */
	{ 0, nosys },			/* 174 = msgrcv */
	{ 0, nosys },			/* 175 = msgsnd */
#endif
#ifdef SYSVSEM
	{ 0, nosys },			/* 176 = semctl */
	{ 0, nosys },			/* 177 = semget */
	{ 0, nosys },			/* 178 = semop */
#else
	{ 0, nosys },			/* 176 = semctl */
	{ 0, nosys },			/* 177 = semget */
	{ 0, nosys },			/* 178 = semop */
#endif
	{ 1, sun_uname },			/* 179 = sun_uname */
#ifdef SYSVSHM
	{ 4, shmsys },			/* 180 = shmsys */
#else
	{ 0, nosys },			/* 180 = nosys */
#endif
	{ 0, nosys },			/* 181 = plock */
	{ 0, nosys },			/* 182 = lockf */
	{ 2, sun_ustat },			/* 183 = sun_ustat */
	{ 0, nosys },			/* 184 = getmnt */
	{ 0, nosys },			/* 185 = notdef */
	{ 0, nosys },			/* 186 = notdef */
	{ 1, sun_sigpending },			/* 187 = sun_sigpending */
	{ 1, setsid },			/* 188 = setsid */
	{ 1, ultrix_waitpid },			/* 189 = ultrix_waitpid */
	{ 0, nosys },			/* 190 = nosys */
	{ 0, nosys },			/* 191 = nosys */
	{ 0, nosys },			/* 192 = nosys */
	{ 0, nosys },			/* 193 = nosys */
	{ 0, nosys },			/* 194 = nosys */
	{ 0, nosys },			/* 195 = nosys */
	{ 0, nosys },			/* 196 = nosys */
	{ 0, nosys },			/* 197 = nosys */
	{ 0, nosys },			/* 198 = nosys */
	{ 0, nosys },			/* 199 = nosys */
	{ 0, nosys },			/* 200 = nosys */
	{ 0, nosys },			/* 201 = nosys */
	{ 0, nosys },			/* 202 = nosys */
	{ 0, nosys },			/* 203 = nosys */
	{ 0, nosys },			/* 204 = nosys */
	{ 0, nosys },			/* 205 = nosys */
	{ 0, nosys },			/* 206 = nosys */
	{ 0, nosys },			/* 207 = nosys */
	{ 0, nosys },			/* 208 = nosys */
	{ 0, nosys },			/* 209 = nosys */
	{ 0, nosys },			/* 210 = nosys */
	{ 0, nosys },			/* 211 = nosys */
	{ 0, nosys },			/* 212 = nosys */
	{ 0, nosys },			/* 213 = nosys */
	{ 0, nosys },			/* 214 = nosys */
	{ 0, nosys },			/* 215 = nosys */
	{ 0, nosys },			/* 216 = nosys */
	{ 0, nosys },			/* 217 = nosys */
	{ 0, nosys },			/* 218 = nosys */
	{ 0, nosys },			/* 219 = nosys */
	{ 0, nosys },			/* 220 = nosys */
	{ 0, nosys },			/* 221 = nosys */
	{ 0, nosys },			/* 222 = nosys */
	{ 0, nosys },			/* 223 = nosys */
	{ 0, nosys },			/* 224 = nosys */
	{ 0, nosys },			/* 225 = nosys */
	{ 0, nosys },			/* 226 = nosys */
	{ 0, nosys },			/* 227 = nosys */
	{ 0, nosys },			/* 228 = nosys */
	{ 0, nosys },			/* 229 = nosys */
	{ 0, nosys },			/* 230 = nosys */
	{ 0, nosys },			/* 231 = nosys */
	{ 0, nosys },			/* 232 = nosys */
	{ 0, nosys },			/* 233 = nosys */
	{ 0, nosys },			/* 234 = nosys */
	{ 0, nosys },			/* 235 = nosys */
	{ 0, nosys },			/* 236 = nosys */
	{ 0, nosys },			/* 237 = nosys */
	{ 0, nosys },			/* 238 = nosys */
	{ 0, nosys },			/* 239 = nosys */
	{ 0, nosys },			/* 240 = nosys */
	{ 0, nosys },			/* 241 = nosys */
	{ 0, nosys },			/* 242 = nosys */
	{ 0, nosys },			/* 243 = nosys */
	{ 0, nosys },			/* 244 = nosys */
	{ 0, nosys },			/* 245 = nosys */
	{ 0, nosys },			/* 246 = nosys */
	{ 0, nosys },			/* 247 = nosys */
	{ 0, nosys },			/* 248 = nosys */
	{ 0, nosys },			/* 249 = nosys */
	{ 0, nosys },			/* 250 = nosys */
	{ 0, nosys },			/* 251 = nosys */
	{ 0, nosys },			/* 252 = audctl */
	{ 0, nosys },			/* 253 = audgen */
	{ 0, nosys },			/* 254 = startcpu */
	{ 0, nosys },			/* 255 = stopcpu */
	{ 0, nosys },			/* 256 = ultrix_getsysinfo */
	{ 0, nosys },			/* 257 = ultrix_setsysinfo */
};

int	nultrix_sysent = sizeof(ultrix_sysent) / sizeof(ultrix_sysent[0]);
