/*-
 * Copyright (c) 1990 The Regents of the University of California.
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
 *	@(#)db.h	5.21 (Berkeley) 2/14/93
 */

#ifndef _DB_H_
#define	_DB_H_

#include <sys/types.h>
#include <sys/cdefs.h>
#include <machine/endian.h>

#define	RET_ERROR	-1		/* Return values. */
#define	RET_SUCCESS	 0
#define	RET_SPECIAL	 1

#define	MAX_PAGE_NUMBER	ULONG_MAX	/* >= # of pages in a file */
typedef u_long	pgno_t;
#define	MAX_PAGE_OFFSET	USHRT_MAX	/* >= # of bytes in a page */
typedef u_short	indx_t;
#define	MAX_REC_NUMBER	ULONG_MAX	/* >= # of records in a tree */
typedef u_long	recno_t;

/* Key/data structure -- a Data-Base Thang. */
typedef struct {
	void	*data;			/* data */
	size_t	 size;			/* data length */
} DBT;

/* Routine flags. */
#define	R_CURSOR	1		/* del, put, seq */
#define	R_CURSORLOG	2		/* put (RECNO) */
#define	R_FIRST		3		/* seq */
#define	R_IAFTER	4		/* put (RECNO) */
#define	R_IBEFORE	5		/* put (RECNO) */
#define	R_LAST		6		/* seq (BTREE, RECNO) */
#define	R_NEXT		7		/* seq */
#define	R_NOOVERWRITE	8		/* put */
#define	R_PREV		9		/* seq (BTREE, RECNO) */
#define	R_SETCURSOR	10		/* put (RECNO) */

typedef enum { DB_BTREE, DB_HASH, DB_RECNO } DBTYPE;

#define	__USE_OPEN_FLAGS \
	(O_CREAT|O_EXCL|O_EXLOCK|O_RDONLY|O_RDWR|O_SHLOCK|O_TRUNC)

/* Access method description structure. */
typedef struct __db {
	DBTYPE type;			/* underlying db type */
	int (*close)	__P((struct __db *));
	int (*del)	__P((const struct __db *, const DBT *, u_int));
	int (*get)	__P((const struct __db *, const DBT *, DBT *, u_int));
	int (*put)	__P((const struct __db *, DBT *, const DBT *, u_int));
	int (*seq)	__P((const struct __db *, DBT *, DBT *, u_int));
	int (*sync)	__P((const struct __db *));
	void *internal;			/* access method private */
} DB;

#define	BTREEMAGIC	0x053162
#define	BTREEVERSION	3

/* Structure used to pass parameters to the btree routines. */
typedef struct {
#define	R_DUP		0x01	/* duplicate keys */
	u_long flags;
	int cachesize;		/* bytes to cache */
	int maxkeypage;		/* maximum keys per page */
	int minkeypage;		/* minimum keys per page */
	int psize;		/* page size */
				/* comparison, prefix functions */
	int (*compare)	__P((const DBT *, const DBT *));
	int (*prefix)	__P((const DBT *, const DBT *));
	int lorder;		/* byte order */
} BTREEINFO;

#define	HASHMAGIC	0x061561
#define	HASHVERSION	2

/* Structure used to pass parameters to the hashing routines. */
typedef struct {
	int bsize;		/* bucket size */
	int ffactor;		/* fill factor */
	int nelem;		/* number of elements */
	int cachesize;		/* bytes to cache */
	int (*hash)();		/* hash function */
	int lorder;		/* byte order */
} HASHINFO;

/* Structure used to pass parameters to the record routines. */
typedef struct {
#define	R_FIXEDLEN	0x01	/* fixed-length records */
#define	R_NOKEY		0x02	/* key not required */
#define	R_SNAPSHOT	0x04	/* snapshot the input */
	u_long flags;
	int cachesize;		/* bytes to cache */
	int lorder;		/* byte order */
	size_t reclen;		/* record length (fixed-length records) */
	u_char bval;		/* delimiting byte (variable-length records */
} RECNOINFO;

/* Key structure for the record routines. */
typedef struct {
	u_long number;
	u_long offset;
	u_long length;
#define	R_LENGTH	0x01	/* length is valid */
#define	R_NUMBER	0x02	/* record number is valid */
#define	R_OFFSET	0x04	/* offset is valid */
	u_char valid;
} RECNOKEY;

/*
 * Little endian <==> big endian long swap macros.
 *	BLSWAP		swap a memory location
 *	BLPSWAP		swap a referenced memory location
 *	BLSWAP_COPY	swap from one location to another
 */
#define BLSWAP(a) { a = __byte_swap_long(a); }
#define	BLPSWAP(a) BLSWAP((*a))
#define	BLSWAP_COPY(a, b) { b = __byte_swap_long(a); }

/*
 * Little endian <==> big endian short swap macros.
 *	BSSWAP		swap a memory location
 *	BSPSWAP		swap a referenced memory location
 *	BSSWAP_COPY	swap from one location to another
 */
#define BSSWAP(a) { a = __byte_swap_word(a); }
#define BSPSWAP(a)  BSSWAP((*a))
#define BSSWAP_COPY(a, b) { b = __byte_swap_word(a); }

__BEGIN_DECLS
DB *dbopen __P((const char *, int, int, DBTYPE, const void *));

#ifdef __DBINTERFACE_PRIVATE
DB	*__bt_open __P((const char *, int, int, const BTREEINFO *));
DB	*__hash_open __P((const char *, int, int, const HASHINFO *));
DB	*__rec_open __P((const char *, int, int, const RECNOINFO *));
void	 __dbpanic __P((DB *dbp));
#endif
__END_DECLS
#endif /* !_DB_H_ */
