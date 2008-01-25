/*	$NetBSD: pmap_getport.c,v 1.17 2008/01/25 19:43:53 christos Exp $	*/

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char *sccsid = "@(#)pmap_getport.c 1.9 87/08/11 Copyr 1984 Sun Micro";
static char *sccsid = "@(#)pmap_getport.c	2.2 88/08/01 4.0 RPCSRC";
#else
__RCSID("$NetBSD: pmap_getport.c,v 1.17 2008/01/25 19:43:53 christos Exp $");
#endif
#endif

/*
 * pmap_getport.c
 * Client interface to pmap rpc service.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 */

#include "namespace.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <net/if.h>

#include <assert.h>
#include <unistd.h>

#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>

#ifdef __weak_alias
__weak_alias(pmap_getport,_pmap_getport)
#endif

static const struct timeval timeout = { 5, 0 };
static const struct timeval tottimeout = { 60, 0 };

/*
 * Find the mapped port for program,version.
 * Calls the pmap service remotely to do the lookup.
 * Returns 0 if no map exists.
 */

static void
remote_pmap_getport(CLIENT *client, struct pmap *parms, u_short *port)
{
	if (CLNT_CALL(client, (rpcproc_t)PMAPPROC_GETPORT, (xdrproc_t)xdr_pmap,
	    parms, (xdrproc_t)xdr_u_short, port, tottimeout) != RPC_SUCCESS) {
		rpc_createerr.cf_stat = RPC_PMAPFAILURE;
		clnt_geterr(client, &rpc_createerr.cf_error);
	} else if (*port == 0) {
		rpc_createerr.cf_stat = RPC_PROGNOTREGISTERED;
		clnt_geterr(client, &rpc_createerr.cf_error);
	}
	CLNT_DESTROY(client);
}

u_short
pmap_getport(address, program, version, protocol)
	struct sockaddr_in *address;
	u_long program;
	u_long version;
	u_int protocol;
{
	u_short port = 0;
	int sock = -1;
	CLIENT *client;
	struct pmap parms;

	_DIAGASSERT(address != NULL);

	parms.pm_prog = program;
	parms.pm_vers = version;
	parms.pm_prot = protocol;
	parms.pm_port = 0;  /* not needed or used */

	address->sin_port = htons(PMAPPORT);
	if (protocol == IPPROTO_TCP) {
	    client = clnttcp_create(address, PMAPPROG, PMAPVERS, &sock, 0, 0);
	    if (client != NULL)
		    remote_pmap_getport(client, &parms, &port);
	}
	if (port == 0) {
	    client = clntudp_bufcreate(address, PMAPPROG, PMAPVERS, timeout,
		&sock, RPCSMALLMSGSIZE, RPCSMALLMSGSIZE);
	    if (client != NULL)
		    remote_pmap_getport(client, &parms, &port);
	}
	address->sin_port = 0;
	return (port);
}
