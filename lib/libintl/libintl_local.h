/*	$NetBSD: libintl_local.h,v 1.9 2004/09/23 21:35:27 tshiozak Exp $	*/

/*-
 * Copyright (c) 2000, 2001 Citrus Project,
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
 * $Citrus: xpg4dl/FreeBSD/lib/libintl/libintl_local.h,v 1.13 2001/09/27 15:18:45 yamt Exp $
 */

#define MO_MAGIC		0x950412de
#define MO_MAGIC_SWAPPED	0xde120495
#define MO_GET_REV_MAJOR(r)	(((r) >> 16) & 0xFFFF)
#define MO_GET_REV_MINOR(r)	((r) & 0xFFFF)
#define MO_MAKE_REV(maj, min)	(((maj) << 16) | (min))

#define GETTEXT_MMAP_MAX	(1024 * 1024)	/*XXX*/

#define DEFAULT_DOMAINNAME	"messages"

/* *.mo file format */
struct mo {
	u_int32_t mo_magic;	/* determines endian */
	u_int32_t mo_revision;	/* file format revision: 0 */
	u_int32_t mo_nstring;	/* N: number of strings */
	u_int32_t mo_otable;	/* O: original text table offset */
	u_int32_t mo_ttable;	/* T: translated text table offset */
	u_int32_t mo_hsize;	/* S: size of hashing table */
	u_int32_t mo_hoffset;	/* H: offset of hashing table */
	/* rev 0.1 / 1.1 */
	/* system dependent string support */
	u_int32_t mo_sysdep_nsegs;	/* number of sysdep segments */
	u_int32_t mo_sysdep_segoff;	/* offset of sysdep segment table */
	u_int32_t mo_sysdep_nstring;	/* number of strings */
	u_int32_t mo_sysdep_otable;	/* offset of original text table */
	u_int32_t mo_sysdep_ttable;	/* offset of translated text table */
} __attribute__((__packed__));

struct moentry {
	u_int32_t len;		/* strlen(str), so region will be len + 1 */
	u_int32_t off;		/* offset of \0-terminated string */
} __attribute__((__packed__));

struct mosysdepsegentry {
	u_int32_t len;		/* length of this part */
	u_int32_t ref;		/* reference number of the sysdep string,
				 * concatenated just after this segment.
				 */
} __attribute__((__packed__));
#define MO_LASTSEG		(0xFFFFFFFF)

struct mosysdepstr {
	u_int32_t off;				/* offset of seed text */
	struct mosysdepsegentry segs[1];	/* text segments */
} __attribute__((__packed__));

/* libintl internal data format */
struct moentry_h {
	size_t len;		/* strlen(str), so region will be len + 1 */
	char *off;		/* offset of \0-terminated string */
};

struct mosysdepsegs_h {
	const char *str;
	size_t len;
};

struct mosysdepsegentry_h {
	u_int32_t len;
	u_int32_t ref;
};

struct mosysdepstr_h {
	const char *off;			/* offset of the base string */
	char *expanded;				/* expanded string */
	size_t expanded_len;			/* length of expanded string */
	struct mosysdepsegentry_h segs[1];	/* text segments */
};

struct mo_h {
	u_int32_t mo_magic;	/* determines endian */
	u_int32_t mo_revision;	/* file format revision: 0 */
	u_int32_t mo_nstring;	/* N: number of strings */
	struct moentry_h *mo_otable;	/* O: original text table offset */
	struct moentry_h *mo_ttable;	/* T: translated text table offset */
	const char *mo_header;
	char *mo_charset;
	u_int32_t mo_hsize;	/* S: size of hashing table */
	u_int32_t *mo_htable;	/* H: hashing table */
#define MO_HASH_SYSDEP_MASK	0x80000000	/* means sysdep entry */

	u_int32_t mo_flags;
#define MO_F_SYSDEP	0x00000001	/* enable sysdep string support */

	/* system dependent string support */
	u_int32_t mo_sysdep_nsegs;	/* number of sysdep segments */
	u_int32_t mo_sysdep_nstring;	/* number of sysdep strings */
	struct mosysdepsegs_h *mo_sysdep_segs;	/* sysdep segment table */
	struct mosysdepstr_h **mo_sysdep_otable;	/* original text */
	struct mosysdepstr_h **mo_sysdep_ttable;	/* translated text */
};


struct mohandle {
	void *addr;		/* mmap'ed region */
	size_t len;
	struct mo_h mo;		/* endian-flipped mo file header */
};

struct domainbinding {
	struct domainbinding *next;
	char domainname[PATH_MAX];
	char path[PATH_MAX];
	char *codeset;
	struct mohandle mohandle;
};

extern struct domainbinding *__bindings;
extern char __current_domainname[PATH_MAX];

__BEGIN_DECLS
const char *__gettext_iconv __P((const char *, struct domainbinding *));
u_int32_t __intl_string_hash __P((const char *));
const char *__intl_sysdep_get_string_by_tag __P((const char *, size_t *));
__END_DECLS
