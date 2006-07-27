/*	$NetBSD: util.c,v 1.7 2006/07/27 15:29:07 christos Exp $	*/

/*
 * Copyright (c) 1994 Christos Zoulas
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
 *	This product includes software developed by Christos Zoulas.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD: util.c,v 1.7 2006/07/27 15:29:07 christos Exp $");
#endif

#include <err.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/* emalloc():
 *	Error checked malloc
 */
void *
emalloc(s)
    size_t s;
{
    void *ptr = malloc(s);
    if (ptr == NULL)
	err(1, NULL);
    return ptr;
}


/* erealloc():
 *	Error checked realloc
 */
void *
erealloc(p, s)
    void *p;
    size_t s;
{
    void *ptr = realloc(p, s);
    if (ptr == NULL)
	err(1, NULL);
    return ptr;
}


/* getline():
 *	Read a line from a file parsing continuations ending in \
 *	and eliminating trailing newlines.
 */
char *
getline(fp, size)
    FILE *fp;
    size_t *size;
{
    size_t s, len = 0;
    char *buf = NULL;
    char *ptr;
    char line[10240];
    int cnt = 1;

    while (cnt) {
	if ((ptr = fgets(line, sizeof(line), fp)) == NULL) {
	    *size = len;
	    return buf;
	}
	s = strlen(line);
	if (ptr[s - 1] == '\n')		/* the newline may be missing at EOF */
	    s--;			/* forget newline */
	if (!s)
	    cnt = 0;
	else {
	    if ((cnt = (ptr[s - 1] == '\\')) != 0)	/* check for \\ */
	    	s--;					/* forget \\ */
	}

	buf = erealloc(buf, len + s + 1);
	memcpy(buf + len, ptr, s);
	len += s;
	buf[len] = '\0';
    }
    *size = len;
    return buf;
}
