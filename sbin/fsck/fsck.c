/*	$NetBSD: fsck.c,v 1.13 1997/10/13 09:44:18 bouyer Exp $	*/

/*
 * Copyright (c) 1996 Christos Zoulas. All rights reserved.
 * Copyright (c) 1980, 1989, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * From: @(#)mount.c	8.19 (Berkeley) 4/19/94
 * From: NetBSD: mount.c,v 1.24 1995/11/18 03:34:29 cgd Exp 
 *
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: fsck.c,v 1.13 1997/10/13 09:44:18 bouyer Exp $");
#endif /* not lint */

#include <sys/param.h>
#include <sys/mount.h>
#include <sys/queue.h>
#include <sys/wait.h>
#define DKTYPENAMES
#include <sys/disklabel.h>
#include <sys/ioctl.h>

#include <err.h>
#include <errno.h>
#include <fstab.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pathnames.h"
#include "fsutil.h"

static enum { IN_LIST, NOT_IN_LIST } which = NOT_IN_LIST;

TAILQ_HEAD(fstypelist, entry) opthead, selhead;

struct entry {
	char *type;
	char *options;
	TAILQ_ENTRY(entry) entries;
};

static int maxrun = 0;
static char *options = NULL;
static int flags = 0;

int main __P((int, char *[]));

static int checkfs __P((const char *, const char *, const char *, void *,
    pid_t *));
static int selected __P((const char *));
static void addoption __P((char *));
static const char *getoptions __P((const char *));
static void addentry __P((struct fstypelist *, const char *, const char *));
static void maketypelist __P((char *));
static char *catopt __P((char *, const char *, int));
static void mangle __P((char *, int *, const char ***, int *));
static char *getfslab __P((const char *));
static void usage __P((void));
static void *isok __P((struct fstab *));

int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct fstab *fs;
	int i, rval = 0;
	char *vfstype = NULL;
	char globopt[3];

	globopt[0] = '-';
	globopt[2] = '\0';

	TAILQ_INIT(&selhead);
	TAILQ_INIT(&opthead);

	while ((i = getopt(argc, argv, "dvpfnyl:t:T:")) != -1)
		switch (i) {
		case 'd':
			flags |= CHECK_DEBUG;
			break;

		case 'v':
			flags |= CHECK_VERBOSE;
			break;

		case 'p':
			flags |= CHECK_PREEN;
			/*FALLTHROUGH*/
		case 'n':
		case 'f':
		case 'y':
			globopt[1] = i;
			options = catopt(options, globopt, 1);
			break;

		case 'l':
			maxrun = atoi(optarg);
			break;

		case 'T':
			if (*optarg)
				addoption(optarg);
			break;

		case 't':
			if (selhead.tqh_first != NULL)
				errx(1, "only one -t option may be specified.");

			maketypelist(optarg);
			vfstype = optarg;
			break;

		case '?':
		default:
			usage();
			/* NOTREACHED */
		}

	argc -= optind;
	argv += optind;

	if (argc == 0)
		return checkfstab(flags, maxrun, isok, checkfs);

#define	BADTYPE(type)							\
	(strcmp(type, FSTAB_RO) &&					\
	    strcmp(type, FSTAB_RW) && strcmp(type, FSTAB_RQ))


	for (; argc--; argv++) {
		char *spec, *type;

		if ((fs = getfsfile(*argv)) == NULL &&
		    (fs = getfsspec(*argv)) == NULL) {
			if (vfstype == NULL)
				vfstype = getfslab(*argv);
			spec = *argv;
			type = vfstype;
		}
		else {
			spec = fs->fs_spec;
			type = fs->fs_vfstype;
			if (BADTYPE(fs->fs_type))
				errx(1, "%s has unknown file system type.",
				    *argv);
		}

		rval |= checkfs(type, blockcheck(spec), *argv, NULL, NULL);
	}

	return rval;
}


static void *
isok(fs)
	struct fstab *fs;
{
	if (fs->fs_passno == 0)
		return NULL;

	if (BADTYPE(fs->fs_type))
		return NULL;

	if (!selected(fs->fs_vfstype))
		return NULL;

	return fs;
}


static int
checkfs(vfstype, spec, mntpt, auxarg, pidp)
	const char *vfstype, *spec, *mntpt;
	void *auxarg;
	pid_t *pidp;
{
	/* List of directories containing fsck_xxx subcommands. */
	static const char *edirs[] = {
		_PATH_SBIN,
		_PATH_USRSBIN,
		NULL
	};
	char execbase[MAXPATHLEN];
	const char **argv, **edir;
	pid_t pid;
	int argc = 1, i, status, maxargc;
	char *optbuf = NULL, execname[MAXPATHLEN + 1];
	const char *extra = getoptions(vfstype);


#ifdef __GNUC__
	/* Avoid vfork clobbering */
	(void) &optbuf;
	(void) &vfstype;
#endif

	if (strcmp(vfstype, "ufs") == 0)
		vfstype = MOUNT_UFS;

	maxargc = 100;
	argv = emalloc(sizeof(char *) * maxargc);

	/* construct basename of executable and argv[0] simultaneously */
	(void) snprintf(execbase, sizeof(execbase), "fsck_%s", vfstype);
	argv[0] = vfstype;

	if (options) {
		if (extra != NULL)
			optbuf = catopt(options, extra, 0);
		else
			optbuf = estrdup(options);
	}
	else if (extra)
		optbuf = estrdup(extra);

	if (optbuf)
		mangle(optbuf, &argc, &argv, &maxargc);

	argv[argc++] = spec;
	argv[argc] = NULL;

	if (flags & (CHECK_DEBUG|CHECK_VERBOSE)) {
		(void)printf("start %s %swait", mntpt, 
			pidp ? "no" : "");
		for (i = 0; i < argc; i++)
			(void)printf(" %s", argv[i]);
		(void)printf("\n");
	}

	switch (pid = vfork()) {
	case -1:				/* Error. */
		warn("vfork");
		if (optbuf)
			free(optbuf);
		return (1);

	case 0:					/* Child. */
		if (flags & CHECK_DEBUG)
			_exit(0);

		/* Go find an executable. */
		edir = edirs;
		do {
			(void)snprintf(execname,
			    sizeof(execname), "%s/%s", *edir, execbase);
			execv(execname, (char * const *)argv);
			if (errno != ENOENT)
				if (spec)
					warn("exec %s for %s", execname, spec);
				else
					warn("exec %s", execname);
		} while (*++edir != NULL);

		if (errno == ENOENT)
			if (spec)
				warn("exec %s for %s", execname, spec);
			else
				warn("exec %s", execname);
		_exit(1);
		/* NOTREACHED */

	default:				/* Parent. */
		if (optbuf)
			free(optbuf);

		if (pidp) {
			*pidp = pid;
			return 0;
		}

		if (waitpid(pid, &status, 0) < 0) {
			warn("waitpid");
			return (1);
		}

		if (WIFEXITED(status)) {
			if (WEXITSTATUS(status) != 0)
				return (WEXITSTATUS(status));
		}
		else if (WIFSIGNALED(status)) {
			warnx("%s: %s", spec, strsignal(WTERMSIG(status)));
			return (1);
		}
		break;
	}

	return (0);
}


static int
selected(type)
	const char *type;
{
	struct entry *e;

	/* If no type specified, it's always selected. */
	for (e = selhead.tqh_first; e != NULL; e = e->entries.tqe_next)
		if (!strncmp(e->type, type, MFSNAMELEN))
			return which == IN_LIST ? 1 : 0;

	return which == IN_LIST ? 0 : 1;
}


static const char *
getoptions(type)
	const char *type;
{
	struct entry *e;

	for (e = opthead.tqh_first; e != NULL; e = e->entries.tqe_next)
		if (!strncmp(e->type, type, MFSNAMELEN))
			return e->options;
	return "";
}


static void
addoption(optstr)
	char *optstr;
{
	char *newoptions;
	struct entry *e;

	if ((newoptions = strchr(optstr, ':')) == NULL)
		errx(1, "Invalid option string");

	*newoptions++ = '\0';

	for (e = opthead.tqh_first; e != NULL; e = e->entries.tqe_next)
		if (!strncmp(e->type, optstr, MFSNAMELEN)) {
			e->options = catopt(e->options, newoptions, 1);
			return;
		}
	addentry(&opthead, optstr, newoptions);
}


static void
addentry(list, type, opts)
	struct fstypelist *list;
	const char *type;
	const char *opts;
{
	struct entry *e;

	e = emalloc(sizeof(struct entry));
	e->type = estrdup(type);
	e->options = estrdup(opts);
	TAILQ_INSERT_TAIL(list, e, entries);
}


static void
maketypelist(fslist)
	char *fslist;
{
	char *ptr;

	if ((fslist == NULL) || (fslist[0] == '\0'))
		errx(1, "empty type list");

	if (fslist[0] == 'n' && fslist[1] == 'o') {
		fslist += 2;
		which = NOT_IN_LIST;
	}
	else
		which = IN_LIST;

	while ((ptr = strsep(&fslist, ",")) != NULL)
		addentry(&selhead, ptr, "");

}


static char *
catopt(s0, s1, fr)
	char *s0;
	const char *s1;
	int fr;
{
	size_t i;
	char *cp;

	if (s0 && *s0) {
		i = strlen(s0) + strlen(s1) + 1 + 1;
		cp = emalloc(i);
		(void)snprintf(cp, i, "%s,%s", s0, s1);
	}
	else
		cp = estrdup(s1);

	if (s0 && fr)
		free(s0);
	return (cp);
}


static void
mangle(opts, argcp, argvp, maxargcp)
	char *opts;
	int *argcp;
	const char ***argvp;
	int *maxargcp;
{
	char *p, *s;
	int argc = *argcp, maxargc = *maxargcp;
	const char **argv = *argvp;

	argc = *argcp;
	maxargc = *maxargcp;

	for (s = opts; (p = strsep(&s, ",")) != NULL;) {
		/* always leave space for one more argument and the NULL */
		if (argc >= maxargc - 3) {
			maxargc += 50;
			argv = erealloc(argv, maxargc * sizeof(char *));
		}
		if (*p != '\0')
			if (*p == '-') {
				argv[argc++] = p;
				p = strchr(p, '=');
				if (p) {
					*p = '\0';
					argv[argc++] = p+1;
				}
			}
			else {
				argv[argc++] = "-o";
				argv[argc++] = p;
			}
	}

	*argcp = argc;
	*argvp = argv;
	*maxargcp = maxargc;
}


static char *
getfslab(str)
	const char *str;
{
	struct disklabel dl;
	int fd;
	char p, *vfstype;
	u_char t;

	/* deduce the filesystem type from the disk label */
	if ((fd = open(str, O_RDONLY)) == -1)
		err(1, "cannot open `%s'", str);

	if (ioctl(fd, DIOCGDINFO, &dl) == -1)
		err(1, "cannot get disklabel for `%s'", str);

	(void) close(fd);

	p = str[strlen(str) - 1];

	if ((p - 'a') >= dl.d_npartitions)
		errx(1, "partition `%s' is not defined on disk", str);

	if ((t = dl.d_partitions[p - 'a'].p_fstype) >= FSMAXTYPES) 
		errx(1, "partition `%s' is not of a legal vfstype",
		    str);

	if ((vfstype = fscknames[t]) == NULL)
		errx(1, "vfstype `%s' on partition `%s' is not supported",
		    fstypenames[t], str);

	return vfstype;
}


static void
usage()
{
	extern char *__progname;
	static const char common[] =
	    "[-dpvlyn] [-T fstype:fsoptions] [-t fstype]";

	(void)fprintf(stderr, "Usage: %s %s [special|node]...\n",
	    __progname, common);
	exit(1);
}
