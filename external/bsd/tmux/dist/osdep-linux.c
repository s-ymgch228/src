/* $Id: osdep-linux.c,v 1.1.1.1 2011/03/10 09:15:38 jmmv Exp $ */

/*
 * Copyright (c) 2009 Nicholas Marriott <nicm@users.sourceforge.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <unistd.h>

#include "tmux.h"

char *
osdep_get_name(int fd, unused char *tty)
{
	FILE	*f;
	char	*path, *buf;
	size_t	 len;
	int	 ch;
	pid_t	 pgrp;

	if ((pgrp = tcgetpgrp(fd)) == -1)
		return (NULL);

	xasprintf(&path, "/proc/%lld/cmdline", (long long) pgrp);
	if ((f = fopen(path, "r")) == NULL) {
		xfree(path);
		return (NULL);
	}
	xfree(path);

	len = 0;
	buf = NULL;
	while ((ch = fgetc(f)) != EOF) {
		if (ch == '\0')
			break;
		buf = xrealloc(buf, 1, len + 2);
		buf[len++] = ch;
	}
	if (buf != NULL)
		buf[len] = '\0';

	fclose(f);
	return (buf);
}
