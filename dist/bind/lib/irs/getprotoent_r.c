/*	$NetBSD: getprotoent_r.c,v 1.1.1.1 1999/11/20 18:54:09 veego Exp $	*/

/*
 * Copyright (c) 1998-1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#if defined(LIBC_SCCS) && !defined(lint)
static const char rcsid[] = "Id: getprotoent_r.c,v 8.4 1999/01/18 07:46:52 vixie Exp";
#endif /* LIBC_SCCS and not lint */

#include <port_before.h>
#if !defined(_REENTRANT) || !defined(DO_PTHREADS)
	static int getprotoent_r_not_required = 0;
#else
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <port_after.h>

#ifdef PROTO_R_RETURN

static PROTO_R_RETURN 
copy_protoent(struct protoent *, struct protoent *, PROTO_R_COPY_ARGS);

PROTO_R_RETURN
getprotobyname_r(const char *name, struct protoent *pptr, PROTO_R_ARGS) {
	struct protoent *pe = getprotobyname(name);

	if (pe == NULL)
		return (PROTO_R_BAD);

	return (copy_protoent(pe, pptr, PROTO_R_COPY));
}

PROTO_R_RETURN
getprotobynumber_r(int proto, struct protoent *pptr, PROTO_R_ARGS) {
	struct protoent *pe = getprotobynumber(proto);

	if (pe == NULL)
		return (PROTO_R_BAD);

	return (copy_protoent(pe, pptr, PROTO_R_COPY));
}

/*
 *	These assume a single context is in operation per thread.
 *	If this is not the case we will need to call irs directly
 *	rather than through the base functions.
 */

PROTO_R_RETURN
getprotoent_r(struct protoent *pptr, PROTO_R_ARGS) {
	struct protoent *pe = getprotoent();

	if (pe == NULL)
		return (PROTO_R_BAD);

	return (copy_protoent(pe, pptr, PROTO_R_COPY));
}

PROTO_R_SET_RETURN
#ifdef PROTO_R_ENT_ARGS
setprotoent_r(int stay_open, PROTO_R_ENT_ARGS)
#else
setprotoent_r(int stay_open)
#endif
{
	setprotoent(stay_open);
#ifdef PROTO_R_SET_RESULT
	return (PROTO_R_SET_RESULT);
#endif
}

PROTO_R_END_RETURN
#ifdef PROTO_R_ENT_ARGS
endprotoent_r(PROTO_R_ENT_ARGS)
#else
endprotoent_r()
#endif
{
	endprotoent();
	PROTO_R_END_RESULT(PROTO_R_OK);
}

/* Private */

#ifndef PROTOENT_DATA
static PROTO_R_RETURN
copy_protoent(struct protoent *pe, struct protoent *pptr, PROTO_R_COPY_ARGS) {
	char *cp;
	int i, n;
	int numptr, len;

	/* Find out the amount of space required to store the answer. */
	numptr = 1; /* NULL ptr */
	len = (char *)ALIGN(buf) - buf;
	for (i = 0; pe->p_aliases[i]; i++, numptr++) {
		len += strlen(pe->p_aliases[i]) + 1;
	}
	len += strlen(pe->p_name) + 1;
	len += numptr * sizeof(char*);
	
	if (len > buflen) {
		errno = ERANGE;
		return (PROTO_R_BAD);
	}

	/* copy protocol value*/
	pptr->p_proto = pe->p_proto;

	cp = (char *)ALIGN(buf) + numptr * sizeof(char *);

	/* copy official name */
	n = strlen(pe->p_name) + 1;
	strcpy(cp, pe->p_name);
	pptr->p_name = cp;
	cp += n;

	/* copy aliases */
	pptr->p_aliases = (char **)ALIGN(buf);
	for (i = 0 ; pe->p_aliases[i]; i++) {
		n = strlen(pe->p_aliases[i]) + 1;
		strcpy(cp, pe->p_aliases[i]);
		pptr->p_aliases[i] = cp;
		cp += n;
	}
	pptr->p_aliases[i] = NULL;

	return (PROTO_R_OK);
}
#else /* !PROTOENT_DATA */
static int
copy_protoent(struct protoent *pe, struct protoent *pptr, PROTO_R_COPY_ARGS) {
	char *cp, *eob;
	int i, n;

	/* copy protocol value */
	pptr->p_proto = pe->p_proto;

	/* copy official name */
	cp = pdptr->line;
	eob = pdptr->line + sizeof(pdptr->line);
	if ((n = strlen(pe->p_name) + 1) < (eob - cp)) {
		strcpy(cp, pe->p_name);
		pptr->p_name = cp;
		cp += n;
	} else {
		return (-1);
	}

	/* copy aliases */
	i = 0;
	pptr->p_aliases = pdptr->proto_aliases;
	while (pe->p_aliases[i] && i < (_MAXALIASES-1)) {
		if ((n = strlen(pe->p_aliases[i]) + 1) < (eob - cp)) {
			strcpy(cp, pe->p_aliases[i]);
			pptr->p_aliases[i] = cp;
			cp += n;
		} else {
			break;
		}
		i++;
	}
	pptr->p_aliases[i] = NULL;

	return (PROTO_R_OK);
}
#endif /* PROTOENT_DATA */
#else /* PROTO_R_RETURN */
	static int getprotoent_r_unknown_systemm = 0;
#endif /* PROTO_R_RETURN */
#endif /* !defined(_REENTRANT) || !defined(DO_PTHREADS) */
