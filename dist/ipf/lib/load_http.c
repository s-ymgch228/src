/*	$NetBSD: load_http.c,v 1.3 2009/05/08 12:06:11 christos Exp $	*/

/*
 * Copyright (C) 2006 by Darren Reed.
 *
 * See the IPFILTER.LICENCE file for details on licencing.
 *
 * Id: load_http.c,v 1.1.2.1 2006/08/25 21:13:04 darrenr Exp
 */

#include "ipf.h"

/*
 * Format expected is one addres per line, at the start of each line.
 */
alist_t *
load_http(char *url)
{
	int fd, len, left, port, endhdr, removed;
	size_t rem;
	char *s, *t, *u, buffer[2048], *myurl;
	alist_t *a, *rtop, *rbot;
	struct sockaddr_in sin;
	struct hostent *host;

	/*
	 * More than this would just be absurd.
	 */
	if (strlen(url) > 512) {
		fprintf(stderr, "load_http has a URL > 512 bytes?!\n");
		return NULL;
	}

	fd = -1;
	rtop = NULL;
	rbot = NULL;

	myurl = strdup(url);
	if (myurl == NULL)
		goto done;

	rem = sizeof(buffer);
	left = snprintf(buffer, rem, "GET %s HTTP/1.0\r\n", url);
	if (left < 0 || left > rem)
		goto done;
	rem -= left;

	s = myurl + 7;			/* http:// */
	t = strchr(s, '/');
	if (t == NULL) {
		fprintf(stderr, "load_http has a malformed URL '%s'\n", url);
		goto done;
	}
	*t++ = '\0';

	u = strchr(s, '@');
	if (u != NULL)
		s = u + 1;		/* AUTH */

	left = snprintf(buffer + left, rem, "Host: %s\r\n\r\n", s);
	if (left < 0 || left > rem)
		goto done;
	rem -= left;

	u = strchr(s, ':');
	if (u != NULL) {
		*u++ = '\0';
		port = atoi(u);
		if (port < 0 || port > 65535)
			goto done;
	} else {
		port = 80;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	if (isdigit((unsigned char)*s)) {
		if (inet_aton(s, &sin.sin_addr) == -1) {
			goto done;
		}
	} else {
		host = gethostbyname(s);
		if (host == NULL)
			goto done;
		memcpy(&sin.sin_addr, host->h_addr_list[0],
		       sizeof(sin.sin_addr));
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		goto done;

	if (connect(fd, (struct sockaddr *)&sin, sizeof(sin)) == -1)
		goto done;

	len = strlen(buffer);
	if (write(fd, buffer, len) != len)
		goto done;

	s = buffer;
	endhdr = 0;
	left = sizeof(buffer) - 1;

	while ((len = read(fd, s, left)) > 0) {
		s[len] = '\0';
		left -= len;
		s += len;

		if (endhdr >= 0) {
			if (endhdr == 0) {
				t = strchr(buffer, ' ');
				if (t == NULL)
					continue;
				t++;
				if (*t != '2')
					break;
			}

			u = buffer;
			while ((t = strchr(u, '\r')) != NULL) {
				if (t == u) {
					if (*(t + 1) == '\n') {
						u = t + 2;
						endhdr = -1;
						break;
					} else
						t++;
				} else if (*(t + 1) == '\n') {
					endhdr++;
					u = t + 2;
				} else
					u = t + 1;
			}
			if (endhdr >= 0)
				continue;
			removed = (u - buffer) + 1;
			memmove(buffer, u, (sizeof(buffer) - left) - removed);
			s -= removed;
			left += removed;
		}

		do {
			t = strchr(buffer, '\n');
			if (t == NULL)
				break;

			*t++ = '\0';
			for (u = buffer; isdigit((unsigned char)*u) ||
			    (*u == '.'); u++)
				continue;
			if (*u == '/') {
				char *slash;

				slash = u;
				u++;
				while (isdigit((unsigned char)*u))
					u++;
				if (!isspace((unsigned char)*u) && *u)
					u = slash;
			}
			*u = '\0';

			a = alist_new(4, buffer);
			if (a != NULL) {
				if (rbot != NULL)
					rbot->al_next = a;
				else
					rtop = a;
				rbot = a;
			}

			removed = t - buffer;
			memmove(buffer, t, sizeof(buffer) - left - removed);
			s -= removed;
			left += removed;

		} while (1);
	}

done:
	if (myurl != NULL)
		free(myurl);
	if (fd != -1)
		close(fd);
	return rtop;
}
