/*	$NetBSD: statd.c,v 1.1 1997/03/10 06:28:31 scottr Exp $	*/

/*
 * Copyright (c) 1995
 *	A.R. Gordon (andrew.gordon@net-tel.co.uk).  All rights reserved.
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
 *	This product includes software developed for the FreeBSD project
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANDREW GORDON AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


/* main() function for status monitor daemon.  Some of the code in this	*/
/* file was generated by running rpcgen /usr/include/rpcsvc/sm_inter.x	*/
/* The actual program logic is in the file procs.c			*/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include <rpc/rpc.h>

#include "statd.h"

#ifndef lint
static char rcsid[] = "$NetBSD: statd.c,v 1.1 1997/03/10 06:28:31 scottr Exp $";
#endif				/* not lint */

int     	debug = 0;		/* Controls syslog() for debug msgs */
int     	_rpcsvcdirty = 0;	/* XXX ??? */
FileLayout	*status_info;		/* Pointer to mmap()ed status file */
static int	status_fd;		/* File descriptor for the open file */
static off_t	status_file_len;	/* Current on-disc length of file */

extern void sm_prog_1(struct svc_req * rqstp, SVCXPRT * transp);
static void handle_sigchld();

main(argc, argv)
	int argc;
	char **argv;
{
	SVCXPRT *transp;
	struct sigaction sa;
	int ch;

	while ((ch = getopt(argc, argv, "d")) != (-1)) {
		switch (ch) {
		case 'd':
			debug = 1;
			break;
		default:
		case '?':
			errx(1, "usage: rpc.statd [-d]");
			/* NOTREACHED */
		}
	}
	(void)pmap_unset(SM_PROG, SM_VERS);

	transp = svcudp_create(RPC_ANYSOCK);
	if (transp == NULL) {
		errx(1, "cannot create udp service.");
		/* NOTREACHED */
	}
	if (!svc_register(transp, SM_PROG, SM_VERS, sm_prog_1, IPPROTO_UDP)) {
		errx(1, "unable to register (SM_PROG, SM_VERS, udp).");
		/* NOTREACHED */
	}
	transp = svctcp_create(RPC_ANYSOCK, 0, 0);
	if (transp == NULL) {
		errx(1, "cannot create tcp service.");
		/* NOTREACHED */
	}
	if (!svc_register(transp, SM_PROG, SM_VERS, sm_prog_1, IPPROTO_TCP)) {
		errx(1, "unable to register (SM_PROG, SM_VERS, tcp).");
		/* NOTREACHED */
	}
	init_file("/var/db/statd.status");

	/*
	 * Note that it is NOT sensible to run this program from inetd - the
	 * protocol assumes that it will run immediately at boot time.
	 */
	daemon(0, 0);
	openlog("rpc.statd", 0, LOG_DAEMON);
	if (debug)
		syslog(LOG_INFO, "Starting - debug enabled");
	else
		syslog(LOG_INFO, "Starting");

	/* Install signal handler to collect exit status of child processes */
	sa.sa_handler = handle_sigchld;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGCHLD);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGCHLD, &sa, NULL);

	/* Initialisation now complete - start operating */

	/*
	 * notify_hosts() forks a process (if necessary) to do the
	 * SM_NOTIFY calls, which may be slow.
	 */
	notify_hosts();

	svc_run();		/* Should never return */
	exit(1);
}

/* handle_sigchld ---------------------------------------------------------- */
/*
 * Purpose:	Catch SIGCHLD and collect process status
 * Returns:	Nothing.
 * Notes:	No special action required, other than to collect the
 *		process status and hence allow the child to die:
 *		we only use child processes for asynchronous transmission
 *		of SM_NOTIFY to other systems, so it is normal for the
 *		children to exit when they have done their work.
 */
static void 
handle_sigchld(sig, code, scp)
	int sig, code;
	struct sigcontext *scp;
{
	int     pid, status;
	pid = wait4(-1, &status, WNOHANG, (struct rusage *) 0);
	if (!pid)
		syslog(LOG_ERR, "Phantom SIGCHLD??");
	else if (status)
		syslog(LOG_ERR, "Child %d failed with status %d", pid,
		    WEXITSTATUS(status));
	else if (debug)
		syslog(LOG_DEBUG, "Child %d exited OK", pid);
}

/* sync_file --------------------------------------------------------------- */
/*
 * Purpose:	Packaged call of msync() to flush changes to mmap()ed file
 * Returns:	Nothing.  Errors to syslog.
 */
void 
sync_file()
{
	if (msync((void *)status_info, 0) < 0)
		syslog(LOG_ERR, "msync() failed: %s", strerror(errno));
}

/* find_host -------------------------------------------------------------- */
/*
 * Purpose:	Find the entry in the status file for a given host
 * Returns:	Pointer to that entry in the mmap() region, or NULL.
 * Notes:	Also creates entries if requested.
 *		Failure to create also returns NULL.
 */
HostInfo *
find_host(hostname, create)
	char *hostname;
	int create;
{
	HostInfo *hp;
	HostInfo *spare_slot = NULL;
	HostInfo *result = NULL;
	int i;

	for (i = 0, hp = status_info->hosts; i < status_info->noOfHosts;
	    i++, hp++) {
		if (!strncasecmp(hostname, hp->hostname, SM_MAXSTRLEN)) {
			result = hp;
			break;
		}
		if (!spare_slot && !hp->monList && !hp->notifyReqd)
			spare_slot = hp;
	}

	/* Return if entry found, or if not asked to create one. */
	if (result || !create)
		return (result);

	/*
	 * Now create an entry, using the spare slot if one was found or
	 * adding to the end of the list otherwise, extending file if req'd
	 */
	if (!spare_slot) {
		off_t desired_size;
		spare_slot = &status_info->hosts[status_info->noOfHosts];
		desired_size = ((char *)spare_slot - (char *)status_info) +
		    sizeof(HostInfo);

		if (desired_size > status_file_len) {
			/* Extend file by writing 1 byte of junk at the
			 * desired end pos	 */
			lseek(status_fd, desired_size - 1, SEEK_SET);
			i = write(status_fd, &i, 1);
			if (i < 1) {
				syslog(LOG_ERR, "Unable to extend status file");
				return (NULL);
			}
			status_file_len = desired_size;
		}
		status_info->noOfHosts++;
	}
	/*
	 * Initialise the spare slot that has been found/created
	 * Note that we do not msync(), since the caller is presumed to be
	 * about to modify the entry further
	 */
	memset(spare_slot, 0, sizeof(HostInfo));
	strncpy(spare_slot->hostname, hostname, SM_MAXSTRLEN);
	return (spare_slot);
}

/* init_file -------------------------------------------------------------- */
/*
 * Purpose:	Open file, create if necessary, initialise it.
 * Returns:	Nothing - exits on error
 * Notes:	Called before process becomes daemon, hence logs to
 *		stderr rather than syslog.
 *		Opens the file, then mmap()s it for ease of access.
 *		Also performs initial clean-up of the file, zeroing
 *		monitor list pointers, setting the notifyReqd flag in
 *		all hosts that had a monitor list, and incrementing
 *		the state number to the next even value.
 */
void 
init_file(filename)
	char *filename;
{
	char buf[HEADER_LEN];
	int new_file = FALSE;
	int i;

	/* try to open existing file - if not present, create one */
	status_fd = open(filename, O_RDWR);
	if ((status_fd < 0) && (errno == ENOENT)) {
		status_fd = open(filename, O_RDWR | O_CREAT, 0644);
		new_file = TRUE;
	}
	if (status_fd < 0) {
		err(1, "unable to open status file %s", filename);
		/* NOTREACHED */
	}

	/*
	 * File now open.  mmap() it, with a generous size to allow for
	 * later growth, where we will extend the file but not re-map it.
	 */
	status_info = (FileLayout *)mmap(NULL, 0x10000000,
	    PROT_READ | PROT_WRITE, MAP_SHARED, status_fd, 0);

	if (status_info == (FileLayout *)(-1)) {
		perror("rpc.statd");
		fprintf(stderr, "Unable to mmap() status file\n");
	}
	status_file_len = lseek(status_fd, 0L, SEEK_END);

	/*
	 * If the file was not newly created, validate the contents, and if
	 * defective, re-create from scratch.
	 */
	if (!new_file) {
		if ((status_file_len < HEADER_LEN) || (status_file_len <
		    (HEADER_LEN + sizeof(HostInfo) * status_info->noOfHosts))) {
			fprintf(stderr, "rpc.statd: status file is corrupt\n");
			new_file = TRUE;
		}
	}
	/* Initialisation of a new, empty file. */
	if (new_file) {
		memset(buf, 0, sizeof(buf));
		lseek(status_fd, 0L, SEEK_SET);
		write(status_fd, buf, HEADER_LEN);
		status_file_len = HEADER_LEN;
	} else {
		/*
		 * Clean-up of existing file - monitored hosts will have a
		 * pointer to a list of clients, which refers to memory in
		 * the previous incarnation of the program and so are
		 * meaningless now.  These pointers are zeroed and the fact
		 * that the host was previously monitored is recorded by
		 * setting the notifyReqd flag, which will in due course
		 * cause a SM_NOTIFY to be sent.
		 *
		 * Note that if we crash twice in quick succession, some hosts
		 * may already have notifyReqd set, where we didn't manage to
		 * notify them before the second crash occurred.
		 */
		for (i = 0; i < status_info->noOfHosts; i++) {
			HostInfo *this_host = &status_info->hosts[i];

			if (this_host->monList) {
				this_host->notifyReqd = TRUE;
				this_host->monList = NULL;
			}
		}
		/* Select the next higher even number for the state counter */
		status_info->ourState =
		    (status_info->ourState + 2) & 0xfffffffe;
		status_info->ourState++;	/* XXX - ??? */
	}
}

/* notify_one_host --------------------------------------------------------- */
/*
 * Purpose:	Perform SM_NOTIFY procedure at specified host
 * Returns:	TRUE if success, FALSE if failed.
 */
static int 
notify_one_host(hostname)
	char *hostname;
{
	struct timeval timeout = {20, 0};	/* 20 secs timeout */
	CLIENT *cli;
	char dummy;
	stat_chge arg;
	char our_hostname[SM_MAXSTRLEN + 1];

	gethostname(our_hostname, sizeof(our_hostname));
	our_hostname[SM_MAXSTRLEN] = '\0';
	arg.mon_name = our_hostname;
	arg.state = status_info->ourState;

	if (debug)
		syslog(LOG_DEBUG, "Sending SM_NOTIFY to host %s from %s",
		    hostname, our_hostname);

	cli = clnt_create(hostname, SM_PROG, SM_VERS, "udp");
	if (!cli) {
		syslog(LOG_ERR, "Failed to contact host %s%s", hostname,
		    clnt_spcreateerror(""));
		return (FALSE);
	}
	if (clnt_call(cli, SM_NOTIFY, xdr_stat_chge, &arg, xdr_void,
	    &dummy, timeout) != RPC_SUCCESS) {
		syslog(LOG_ERR, "Failed to contact rpc.statd at host %s",
		    hostname);
		clnt_destroy(cli);
		return (FALSE);
	}
	clnt_destroy(cli);
	return (TRUE);
}

/* notify_hosts ------------------------------------------------------------ */
/*
 * Purpose:	Send SM_NOTIFY to all hosts marked as requiring it
 * Returns:	Nothing, immediately - forks a process to do the work.
 * Notes:	Does nothing if there are no monitored hosts.
 *		Called after all the initialisation has been done -
 *		logs to syslog.
 */
void 
notify_hosts(void)
{
	HostInfo *hp;
	int i, attempts;
	int work_to_do = FALSE;
	pid_t pid;

	/* First check if there is in fact any work to do. */
	for (i = status_info->noOfHosts, hp = status_info->hosts; i;
	    i--, hp++) {
		if (hp->notifyReqd) {
			work_to_do = TRUE;
			break;
		}
	}

	if (!work_to_do)
		return;		/* No work found */

	pid = fork();
	if (pid == -1) {
		syslog(LOG_ERR, "Unable to fork notify process - %s",
		    strerror(errno));
		return;
	}
	if (pid)
		return;

	/*
	 * Here in the child process.  We continue until all the hosts marked
	 * as requiring notification have been duly notified.
	 * If one of the initial attempts fails, we sleep for a while and
	 * have another go.  This is necessary because when we have crashed,
	 * (eg. a power outage) it is quite possible that we won't be able to
	 * contact all monitored hosts immediately on restart, either because
	 * they crashed too and take longer to come up (in which case the
	 * notification isn't really required), or more importantly if some
	 * router etc. needed to reach the monitored host has not come back
	 * up yet.  In this case, we will be a bit late in re-establishing
	 * locks (after the grace period) but that is the best we can do.
	 * We try 10 times at 5 sec intervals, 10 more times at 1 minute
	 * intervals, then 24 more times at hourly intervals, finally
	 * giving up altogether if the host hasn't come back to life after
	 * 24 hours.
	 */
	for (attempts = 0; attempts < 44; attempts++) {
		work_to_do = FALSE;	/* Unless anything fails */
		for (i = status_info->noOfHosts, hp = status_info->hosts; i > 0;
		    i--, hp++) {
			if (hp->notifyReqd) {
				if (notify_one_host(hp->hostname)) {
					hp->notifyReqd = FALSE;
					sync_file();
				} else
					work_to_do = TRUE;
			}
		}
		if (!work_to_do)
			break;
		if (attempts < 10)
			sleep(5);
		else
			if (attempts < 20)
				sleep(60);
			else
				sleep(60 * 60);
	}
	exit(0);
}
