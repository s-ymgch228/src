/*	$NetBSD: su_pam.c,v 1.1 2005/01/10 03:11:50 christos Exp $	*/

/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
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
 */

#include <sys/cdefs.h>
#ifndef lint
__COPYRIGHT(
    "@(#) Copyright (c) 1988 The Regents of the University of California.\n\
 All rights reserved.\n");
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)su.c	8.3 (Berkeley) 4/2/94";*/
#else
__RCSID("$NetBSD: su_pam.c,v 1.1 2005/01/10 03:11:50 christos Exp $");
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <err.h>
#include <errno.h>
#include <grp.h>
#include <paths.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <tzfile.h>
#include <unistd.h>

#ifdef LOGIN_CAP
#include <login_cap.h>
#endif

#include <security/pam_appl.h>
#include <security/openpam.h>   /* for openpam_ttyconv() */
 

static pam_handle_t *pamh = NULL;
static const struct pam_conv pamc = { &openpam_ttyconv, NULL };
static int chshell(const char *);
static char *ontty(void);

int main(int, char **);

#define	ARGSTRX	"-dflm"

#ifdef LOGIN_CAP
#define ARGSTR	ARGSTRX "c:"
#else
#define ARGSTR ARGSTRX
#endif

int
main(int argc, char **argv)
{
	extern char **environ;
	struct passwd *pwd;
	char *p;
	uid_t ruid;
	int asme, ch, asthem, fastlogin, prio, gohome;
	enum { UNSET, YES, NO } iscsh = UNSET;
	char *user, *shell, *avshell, *username, **np;
	char *class;
	char shellbuf[MAXPATHLEN], avshellbuf[MAXPATHLEN];
	int pam_err;
	char hostname[MAXHOSTNAMELEN];
	char *tty;
	const char *func;
	const void *newuser;
#ifdef LOGIN_CAP
	login_cap_t *lc;
#endif
#ifdef PAM_DEBUG
	extern int _openpam_debug;
	_openpam_debug = 1;
#endif

	asme = asthem = fastlogin = 0;
	gohome = 1;
	shell = class = NULL;
	while ((ch = getopt(argc, argv, ARGSTR)) != -1)
		switch((char)ch) {
#ifdef LOGIN_CAP
		case 'c':
			class = optarg;
			break;
#endif
		case 'd':
			asme = 0;
			asthem = 1;
			gohome = 0;
			break;
		case 'f':
			fastlogin = 1;
			break;
		case '-':
		case 'l':
			asme = 0;
			asthem = 1;
			break;
		case 'm':
			asme = 1;
			asthem = 0;
			break;
		case '?':
		default:
			(void)fprintf(stderr,
			    "Usage: %s [%s] [login [shell arguments]]\n",
			    getprogname(), ARGSTR);
			exit(1);
		}
	argv += optind;

	/* Lower the priority so su runs faster */
	errno = 0;
	prio = getpriority(PRIO_PROCESS, 0);
	if (errno)
		prio = 0;
	if (prio > -2)
		(void)setpriority(PRIO_PROCESS, 0, -2);
	openlog("su", 0, LOG_AUTH);

	/* get current login name and shell */
	ruid = getuid();
	username = getlogin();
	if (username == NULL || (pwd = getpwnam(username)) == NULL ||
	    pwd->pw_uid != ruid)
		pwd = getpwuid(ruid);
	if (pwd == NULL)
		errx(1, "who are you?");
	if ((username = strdup(pwd->pw_name)) == NULL)
		err(1, "strdup");


	if (asme) {
		if (pwd->pw_shell && *pwd->pw_shell) {
			strlcpy(shellbuf, pwd->pw_shell, sizeof(shellbuf));
			shell = shellbuf;
		} else {
			shell = _PATH_BSHELL;
			iscsh = NO;
		}
	}
	/* get target login information, default to root */
	user = *argv ? *argv : "root";
	np = *argv ? argv : argv - 1;

	if ((pwd = getpwnam(user)) == NULL)
		errx(1, "unknown login %s", user);

	/*
	 * PAM initialization
	 */
#define PAM_END(msg) do { func = msg; goto done; } while (/*CONSTCOND*/0)

	if ((pam_err = pam_start("su", user, &pamc, &pamh)) != PAM_SUCCESS) {
		if (pamh != NULL)
			PAM_END("pam_start");
		/* Things went really bad... */
		syslog(LOG_ERR, "pam_start failed");
		errx(1, "pam_start failed");
	}

#define PAM_END_ITEM(item)	PAM_END("pam_set_item(" # item ")")
#define PAM_SET_ITEM(item, var) \
	if ((pam_err = pam_set_item(pamh, (item), (var))) != PAM_SUCCESS) \
		PAM_END_ITEM(item)

	/*
	 * Fill hostname, username and tty
	 */
	PAM_SET_ITEM(PAM_RUSER, username);
	if (gethostname(hostname, sizeof(hostname)) != -1)
		PAM_SET_ITEM(PAM_RHOST, hostname);

	if ((tty = ttyname(STDERR_FILENO)) != NULL)
		PAM_SET_ITEM(PAM_TTY, tty);

	/* 
	 * Authentication 
	 */
	if ((pam_err = pam_authenticate(pamh, 0)) != PAM_SUCCESS) {
		syslog(LOG_WARNING, "BAD SU %s to %s%s",
		    username, user, ontty());
		pam_end(pamh, pam_err);
		errx(1, "Sorry");
	}

	/*
	 * Authorization
	 */
	switch(pam_err = pam_acct_mgmt(pamh, 0)) {
	case PAM_NEW_AUTHTOK_REQD:
		pam_err = pam_chauthtok(pamh, PAM_CHANGE_EXPIRED_AUTHTOK);
		if (pam_err != PAM_SUCCESS)
			PAM_END("pam_chauthok");
		break;
	case PAM_SUCCESS:
		break;
	default:
		PAM_END("pam_acct_mgmt");
		break;
	}

	/*
	 * pam_authenticate might have changed the target user. 
	 * refresh pwd and user
	 */
	pam_err = pam_get_item(pamh, PAM_USER, &newuser);
	if (pam_err != PAM_SUCCESS) {
		syslog(LOG_WARNING,
		    "pam_get_item(PAM_USER): %s", pam_strerror(pamh, pam_err));
	} else {
		user = (char *)newuser;
		if ((pwd = getpwnam(user)) == NULL) {	
			pam_end(pamh, pam_err);
			syslog(LOG_ERR, "unknown login: %s", username);
			errx(1, "unknown login: %s", username);
		}
	}

#define ERRX_PAM_END(args) do {			\
	pam_end(pamh, pam_err);			\
	errx args;				\
} while (/* CONSTOCOND */0)

#define ERR_PAM_END(args) do {			\
	pam_end(pamh, pam_err);			\
	err args;				\
} while (/* CONSTOCOND */0)
	
#ifdef LOGIN_CAP
	/* force the usage of specified class */
	if (class) {
		if (ruid) 
			ERRX_PAM_END((1, "Only root may use -c"));

		pwd->pw_class = class;
	}
	if ((lc = login_getclass(pwd->pw_class)) == NULL)
		ERRX_PAM_END((1, "Unknown class %s\n", pwd->pw_class));
#endif

	if (asme) {
		/* if asme and non-standard target shell, must be root */
		if (!chshell(pwd->pw_shell) && ruid)
			ERRX_PAM_END((1,"permission denied (shell)."));
	} else if (pwd->pw_shell && *pwd->pw_shell) {
		shell = pwd->pw_shell;
		iscsh = UNSET;
	} else {
		shell = _PATH_BSHELL;
		iscsh = NO;
	}

	if ((p = strrchr(shell, '/')) != NULL)
		avshell = p + 1;
	else
		avshell = shell;

	/* if we're forking a csh, we want to slightly muck the args */
	if (iscsh == UNSET)
		iscsh = strstr(avshell, "csh") ? YES : NO;

	/* 
	 * Set permissions. We change the user credentials (UID) here 
	 * XXX PAM should come before LOGIN_CAP so that the class
	 * specified through -c can override PAM. But as we might drop
	 * root UID on both operations, it is not possible to do that.
	 * If a login class was specified, skip PAM. 
	 */
#ifdef LOGIN_CAP
	if (class) {
		if (setusercontext(lc, pwd, pwd->pw_uid,
		    (asthem ? (LOGIN_SETPRIORITY | LOGIN_SETUMASK) : 0) |
		    LOGIN_SETRESOURCES | LOGIN_SETGROUP | LOGIN_SETUSER))
			ERR_PAM_END((1, "setting user context"));
		printf("%d %d\n", asthem, pwd->pw_uid);
	} else 
#endif
	{
		pam_err = pam_setcred(pamh, PAM_ESTABLISH_CRED);
		if (pam_err != PAM_SUCCESS)
			PAM_END("pam_setcred");
	}

	/*
	 * Manage session. 
	 */
	if (asthem) {
		pid_t pid, xpid;
		void *oint;
		void *oabrt;
		int status;

 		if ((pam_err = pam_open_session(pamh, 0)) != PAM_SUCCESS)
			PAM_END("pam_open_session");

		/*
		 * In order to call pam_close_session after the
		 * command terminates, we need to fork. 
		 * Make sure signals cannot kill the parent.
		 * This is copied from crontab(8), which has to
		 * cope with a similar situation. XXX FreeBSD 
		 * has a much more complicated code (CVS logs 
		 * tell about workaround in libpthread, but we 
		 * might miss useful stuff)
		 */
		oint = signal(SIGINT, SIG_IGN);
		oabrt = signal(SIGABRT, SIG_IGN);
		
		switch (pid = fork()) {
		case -1:
			pam_err = pam_close_session(pamh, 0);
			if (pam_err != PAM_SUCCESS) {
				syslog(LOG_ERR, "pam_close_session: %s", 
				    pam_strerror(pamh, pam_err));
				warnx("pam_close_session: %s", 
				    pam_strerror(pamh, pam_err));
			}
			ERR_PAM_END((1, "fork"));
			break;

		case 0:	/* Child */
			break;

		default:
			/*
			 * Parent: wait for the child to terminate
			 * and call pam_close_session.
			 */
			if ((xpid = wait(&status)) != pid) {
				pam_err = pam_close_session(pamh, 0);
				if (pam_err != PAM_SUCCESS) {
					syslog(LOG_ERR, 
					    "pam_close_session: %s", 
					    pam_strerror(pamh, pam_err));
					warnx("pam_close_session: %s", 
					    pam_strerror(pamh, pam_err));
				}
				ERRX_PAM_END((1, 
				    "wrong PID: %d != %d", pid, xpid));
			}
		
			(void)signal(SIGINT, oint);
			(void)signal(SIGABRT, oabrt);

 			pam_err = pam_close_session(pamh, 0);
			if (pam_err != PAM_SUCCESS)
				PAM_END("pam_open_session");

			pam_end(pamh, PAM_SUCCESS);
			exit(0);
			break;	
		}
	}

	/* 
	 * The child: starting here, we don't have to care about
	 * handling PAM issues if we exit, the parent will do the 
	 * job when we exit.
	 */
#undef PAM_END
#undef ERR_PAM_END
#undef ERRX_PAM_END

	if (!asme) {
		if (asthem) {
			char **pamenv;

			p = getenv("TERM");
			/* 
			 * Create an empty environment 
			 */
			if ((environ = malloc(sizeof(char *))) == NULL)
				err(1, NULL);
			environ[0] = NULL;

			/* 
			 * Add PAM environement, before the LOGIN_CAP stuff:
			 * if the login class is unspecified, we'll get the
			 * same data from PAM, if -c was used, the specified
			 * class must override PAM.
	 		 */
			if ((pamenv = pam_getenvlist(pamh)) != NULL) {
				char **envitem;

				/* 
				 * XXX Here FreeBSD filters out 
				 * SHELL, LOGNAME, MAIL, CDPATH, IFS, PATH
				 * how could we get untrusted data here?
				 */
				for (envitem = pamenv; *envitem; envitem++) {
					putenv(*envitem);
					free(*envitem); 
				}

				free(pamenv);
			}

#ifdef LOGIN_CAP
			if (setusercontext(lc, pwd, pwd->pw_uid, LOGIN_SETPATH))
				err(1, "setting user context");
#else
			(void)setenv("PATH", _PATH_DEFPATH, 1);
#endif
			if (p)
				(void)setenv("TERM", p, 1);
			if (gohome && chdir(pwd->pw_dir) < 0)
				errx(1, "no directory");
		} 

		if (asthem || pwd->pw_uid)
			(void)setenv("USER", pwd->pw_name, 1);
		(void)setenv("HOME", pwd->pw_dir, 1);
		(void)setenv("SHELL", shell, 1);
	}
	(void)setenv("SU_FROM", username, 1);

	if (iscsh == YES) {
		if (fastlogin)
			*np-- = "-f";
		if (asme)
			*np-- = "-m";
	} else {
		if (fastlogin)
			unsetenv("ENV");
	}

	if (asthem) {
		avshellbuf[0] = '-';
		(void)strlcpy(avshellbuf+1, avshell, sizeof(avshellbuf) - 1);
		avshell = avshellbuf;
	} else if (iscsh == YES) {
		/* csh strips the first character... */
		avshellbuf[0] = '_';
		(void)strlcpy(avshellbuf+1, avshell, sizeof(avshellbuf) - 1);
		avshell = avshellbuf;
	}
	*np = avshell;

	if (ruid != 0)
		syslog(LOG_NOTICE, "%s to %s%s",
		    username, pwd->pw_name, ontty());

	/* Raise our priority back to what we had before */
	(void)setpriority(PRIO_PROCESS, 0, prio);

	printf("%d %d\n", geteuid(), getuid());
	(void)execv(shell, np);
	err(1, "%s", shell);
done:
	syslog(LOG_ERR, "%s: %s", func, pam_strerror(pamh, pam_err));   \
	warnx("%s: %s", func, pam_strerror(pamh, pam_err));             \
	pam_end(pamh, pam_err);                                         \
	return 1;
}

static int
chshell(const char *sh)
{
	const char *cp;

	setusershell();
	while ((cp = getusershell()) != NULL)
		if (!strcmp(cp, sh))
			return 1;
	return 0;
}

static char *
ontty(void)
{
	char *p;
	static char buf[MAXPATHLEN + 4];

	buf[0] = 0;
	if ((p = ttyname(STDERR_FILENO)) != NULL)
		(void)snprintf(buf, sizeof buf, " on %s", p);
	return buf;
}
