/*
 * Copyright (c) 2000, Boris Popov
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by Boris Popov.
 * 4. Neither the name of the author nor the names of any co-contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
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
 * from: Id: nb_net.c,v 1.4 2001/02/16 02:46:12 bp Exp 
 */

#include <sys/cdefs.h>
__RCSID("$NetBSD: nb_net.c,v 1.5 2004/02/21 10:09:01 jdolecek Exp $");

#include <sys/param.h>
#include <sys/socket.h>

#include <net/if.h>

#include <ctype.h>
#include <netdb.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>

#include <netsmb/netbios.h>
#include <netsmb/smb_lib.h>
#include <netsmb/nb_lib.h>

int
nb_getlocalname(char *name)
{
	char buf[1024], *cp;

	if (gethostname(buf, sizeof(buf)) != 0)
		return errno;
	cp = strchr(buf, '.');
	if (cp)
		*cp = 0;
	strcpy(name, buf);
	return 0;
}

int
nb_resolvehost_in(const char *name, struct sockaddr **dest)
{
	struct addrinfo *res, hints;
	struct sockaddr_in *sinp;
	int error;
	char port[6];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	snprintf(port, sizeof(port), "%d", SMB_TCP_PORT);

	error = getaddrinfo(name, port, &hints, &res);
	if (error) {
		warnx("server address `%s': %s", name, gai_strerror(error));
		return ENETDOWN;
	}

	/* Use first address as the address to connect to */
	sinp = malloc(res[0].ai_addrlen);
	if (sinp == NULL)
		return ENOMEM;
	memcpy(sinp, res[0].ai_addr, res[0].ai_addrlen);

	freeaddrinfo(res);

	*dest = (struct sockaddr*)sinp;
	return 0;
}

int
nb_enum_if(struct nb_ifdesc **iflist, int maxif)
{  
	struct nb_ifdesc *ifd;
	struct ifaddrs *ifp, *p;
	int i;

	if (getifaddrs(&ifp) < 0)
		return errno;

	*iflist = NULL;
	i = 0;
	for (p = ifp; p; p = p->ifa_next) {

		if (i >= maxif)
			break;

		if ((p->ifa_addr->sa_family != AF_INET) ||
		    ((p->ifa_flags & (IFF_UP|IFF_BROADCAST))
		     != (IFF_UP|IFF_BROADCAST)))
			continue;
		if (strlen(p->ifa_name) >= sizeof(ifd->id_name))
			continue;

		ifd = malloc(sizeof(struct nb_ifdesc));
		if (ifd == NULL) {
			freeifaddrs(ifp);
			/* XXX should free stuff already in *iflist */
			return ENOMEM;
		}
		bzero(ifd, sizeof(struct nb_ifdesc));
		strcpy(ifd->id_name, p->ifa_name);
		ifd->id_flags = p->ifa_flags;
		ifd->id_addr = ((struct sockaddr_in *)p->ifa_addr)->sin_addr;
		ifd->id_mask = ((struct sockaddr_in *)p->ifa_netmask)->sin_addr;
		ifd->id_next = *iflist;
		*iflist = ifd;
		i++;
	}

	freeifaddrs(ifp);
	return 0;
}
