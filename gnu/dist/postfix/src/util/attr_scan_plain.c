/*	$NetBSD: attr_scan_plain.c,v 1.1.1.6 2008/06/22 14:03:49 christos Exp $	*/

/*++
/* NAME
/*	attr_scan_plain 3
/* SUMMARY
/*	recover attributes from byte stream
/* SYNOPSIS
/*	#include <attr.h>
/*
/*	int	attr_scan_plain(fp, flags, type, name, ..., ATTR_TYPE_END)
/*	VSTREAM	fp;
/*	int	flags;
/*	int	type;
/*	char	*name;
/*
/*	int	attr_vscan_plain(fp, flags, ap)
/*	VSTREAM	fp;
/*	int	flags;
/*	va_list	ap;
/* DESCRIPTION
/*	attr_scan_plain() takes zero or more (name, value) request attributes
/*	and recovers the attribute values from the byte stream that was
/*	possibly generated by attr_print_plain().
/*
/*	attr_vscan_plain() provides an alternative interface that is convenient
/*	for calling from within a variadic function.
/*
/*	The input stream is formatted as follows, where (item)* stands
/*	for zero or more instances of the specified item, and where
/*	(item1 | item2) stands for choice:
/*
/* .in +5
/*	attr-list :== simple-attr* newline
/* .br
/*	simple-attr :== attr-name "=" attr-value newline
/* .br
/*	attr-name :== any string without null or "=" or newline.
/* .br
/*	attr-value :== any string without null or newline.
/* .br
/*	newline :== the ASCII newline character
/* .in
/*
/*	All attribute names and attribute values are sent as plain
/*	strings. Each string must be no longer than 4*var_line_limit
/*	characters. The formatting rules aim to make implementations in PERL
/*	and other languages easy.
/*
/*	Normally, attributes must be received in the sequence as specified
/*	with the attr_scan_plain() argument list.  The input stream may
/*	contain additional attributes at any point in the input stream,
/*	including additional instances of requested attributes.
/*
/*	Additional input attributes or input attribute instances are silently
/*	skipped over, unless the ATTR_FLAG_EXTRA processing flag is specified
/*	(see below). This allows for some flexibility in the evolution of
/*	protocols while still providing the option of being strict where
/*	this is desirable.
/*
/*	Arguments:
/* .IP fp
/*	Stream to recover the input attributes from.
/* .IP flags
/*	The bit-wise OR of zero or more of the following.
/* .RS
/* .IP ATTR_FLAG_MISSING
/*	Log a warning when the input attribute list terminates before all
/*	requested attributes are recovered. It is always an error when the
/*	input stream ends without the newline attribute list terminator.
/* .IP ATTR_FLAG_EXTRA
/*	Log a warning and stop attribute recovery when the input stream
/*	contains an attribute that was not requested. This includes the
/*	case of additional instances of a requested attribute.
/* .IP ATTR_FLAG_MORE
/*	After recovering the requested attributes, leave the input stream
/*	in a state that is usable for more attr_scan_plain() operations
/*	from the same input attribute list.
/*	By default, attr_scan_plain() skips forward past the input attribute
/*	list terminator.
/* .IP ATTR_FLAG_STRICT
/*	For convenience, this value combines both ATTR_FLAG_MISSING and
/*	ATTR_FLAG_EXTRA.
/* .IP ATTR_FLAG_NONE
/*	For convenience, this value requests none of the above.
/* .RE
/* .IP type
/*	The type argument determines the arguments that follow.
/* .RS
/* .IP "ATTR_TYPE_INT (char *, int *)"
/*	This argument is followed by an attribute name and an integer pointer.
/* .IP "ATTR_TYPE_LONG (char *, long *)"
/*	This argument is followed by an attribute name and a long pointer.
/* .IP "ATTR_TYPE_STR (char *, VSTRING *)"
/*	This argument is followed by an attribute name and a VSTRING pointer.
/* .IP "ATTR_TYPE_DATA (char *, VSTRING *)"
/*	This argument is followed by an attribute name and a VSTRING pointer.
/* .IP "ATTR_TYPE_FUNC (ATTR_SCAN_SLAVE_FN, void *)"
/*	This argument is followed by a function pointer and a generic data
/*	pointer. The caller-specified function returns < 0 in case of
/*	error.
/* .IP "ATTR_TYPE_HASH (HTABLE *)"
/* .IP "ATTR_TYPE_NAMEVAL (NVTABLE *)"
/*	All further input attributes are processed as string attributes.
/*	No specific attribute sequence is enforced.
/*	All attributes up to the attribute list terminator are read,
/*	but only the first instance of each attribute is stored.
/*	There can be no more than 1024 attributes in a hash table.
/* .sp
/*	The attribute string values are stored in the hash table under
/*	keys equal to the attribute name (obtained from the input stream).
/*	Values from the input stream are added to the hash table. Existing
/*	hash table entries are not replaced.
/* .sp
/*	N.B. This construct must be followed by an ATTR_TYPE_END argument.
/* .IP ATTR_TYPE_END
/*	This argument terminates the requested attribute list.
/* .RE
/* BUGS
/*	ATTR_TYPE_HASH (ATTR_TYPE_NAMEVAL) accepts attributes with arbitrary
/*	names from possibly untrusted sources.
/*	This is unsafe, unless the resulting table is queried only with
/*	known to be good attribute names.
/* DIAGNOSTICS
/*	attr_scan_plain() and attr_vscan_plain() return -1 when malformed input
/*	is detected (string too long, incomplete line, missing end marker).
/*	Otherwise, the result value is the number of attributes that were
/*	successfully recovered from the input stream (a hash table counts
/*	as the number of entries stored into the table).
/*
/*	Panic: interface violation. All system call errors are fatal.
/* SEE ALSO
/*	attr_print_plain(3) send attributes over byte stream.
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <vstream.h>
#include <vstring.h>
#include <htable.h>
#include <base64_code.h>
#include <attr.h>

/* Application specific. */

#define STR(x)	vstring_str(x)
#define LEN(x)	VSTRING_LEN(x)

/* attr_scan_plain_string - pull a string from the input stream */

static int attr_scan_plain_string(VSTREAM *fp, VSTRING *plain_buf,
				        int terminator, const char *context)
{
#if 0
    extern int var_line_limit;		/* XXX */
    int     limit = var_line_limit * 4;

#endif
    int     ch;

    VSTRING_RESET(plain_buf);
    while ((ch = VSTREAM_GETC(fp)) != '\n'
	   && (terminator == 0 || ch != terminator)) {
	if (ch == VSTREAM_EOF) {
	    msg_warn("%s on %s while reading %s",
		vstream_ftimeout(fp) ? "timeout" : "premature end-of-input",
		     VSTREAM_PATH(fp), context);
	    return (-1);
	}
	VSTRING_ADDCH(plain_buf, ch);
#if 0
	if (LEN(plain_buf) > limit) {
	    msg_warn("string length > %d characters from %s while reading %s",
		     limit, VSTREAM_PATH(fp), context);
	    return (-1);
	}
#endif
    }
    VSTRING_TERMINATE(plain_buf);

    if (msg_verbose)
	msg_info("%s: %s", context, *STR(plain_buf) ? STR(plain_buf) : "(end)");
    return (ch);
}

/* attr_scan_plain_data - pull a data blob from the input stream */

static int attr_scan_plain_data(VSTREAM *fp, VSTRING *str_buf,
				        int terminator,
				        const char *context)
{
    static VSTRING *base64_buf = 0;
    int     ch;

    if (base64_buf == 0)
	base64_buf = vstring_alloc(10);
    if ((ch = attr_scan_plain_string(fp, base64_buf, terminator, context)) < 0)
	return (-1);
    if (base64_decode(str_buf, STR(base64_buf), LEN(base64_buf)) == 0) {
	msg_warn("malformed base64 data from %s while reading %s: %.100s",
		 VSTREAM_PATH(fp), context, STR(base64_buf));
	return (-1);
    }
    return (ch);
}

/* attr_scan_plain_number - pull a number from the input stream */

static int attr_scan_plain_number(VSTREAM *fp, unsigned *ptr, VSTRING *str_buf,
				        int terminator, const char *context)
{
    char    junk = 0;
    int     ch;

    if ((ch = attr_scan_plain_string(fp, str_buf, terminator, context)) < 0)
	return (-1);
    if (sscanf(STR(str_buf), "%u%c", ptr, &junk) != 1 || junk != 0) {
	msg_warn("malformed numerical data from %s while reading %s: %.100s",
		 VSTREAM_PATH(fp), context, STR(str_buf));
	return (-1);
    }
    return (ch);
}

/* attr_scan_plain_long_number - pull a number from the input stream */

static int attr_scan_plain_long_number(VSTREAM *fp, unsigned long *ptr,
				               VSTRING *str_buf,
				               int terminator,
				               const char *context)
{
    char    junk = 0;
    int     ch;

    if ((ch = attr_scan_plain_string(fp, str_buf, terminator, context)) < 0)
	return (-1);
    if (sscanf(STR(str_buf), "%lu%c", ptr, &junk) != 1 || junk != 0) {
	msg_warn("malformed numerical data from %s while reading %s: %.100s",
		 VSTREAM_PATH(fp), context, STR(str_buf));
	return (-1);
    }
    return (ch);
}

/* attr_vscan_plain - receive attribute list from stream */

int     attr_vscan_plain(VSTREAM *fp, int flags, va_list ap)
{
    const char *myname = "attr_scan_plain";
    static VSTRING *str_buf = 0;
    static VSTRING *name_buf = 0;
    int     wanted_type = -1;
    char   *wanted_name;
    unsigned int *number;
    unsigned long *long_number;
    VSTRING *string;
    HTABLE *hash_table;
    int     ch;
    int     conversions;
    ATTR_SCAN_SLAVE_FN scan_fn;
    void   *scan_arg;

    /*
     * Sanity check.
     */
    if (flags & ~ATTR_FLAG_ALL)
	msg_panic("%s: bad flags: 0x%x", myname, flags);

    /*
     * EOF check.
     */
    if ((ch = VSTREAM_GETC(fp)) == VSTREAM_EOF)
	return (0);
    vstream_ungetc(fp, ch);

    /*
     * Initialize.
     */
    if (str_buf == 0) {
	str_buf = vstring_alloc(10);
	name_buf = vstring_alloc(10);
    }

    /*
     * Iterate over all (type, name, value) triples.
     */
    for (conversions = 0; /* void */ ; conversions++) {

	/*
	 * Determine the next attribute type and attribute name on the
	 * caller's wish list.
	 * 
	 * If we're reading into a hash table, we already know that the
	 * attribute value is string-valued, and we get the attribute name
	 * from the input stream instead. This is secure only when the
	 * resulting table is queried with known to be good attribute names.
	 */
	if (wanted_type != ATTR_TYPE_HASH) {
	    wanted_type = va_arg(ap, int);
	    if (wanted_type == ATTR_TYPE_END) {
		if ((flags & ATTR_FLAG_MORE) != 0)
		    return (conversions);
		wanted_name = "(list terminator)";
	    } else if (wanted_type == ATTR_TYPE_HASH) {
		wanted_name = "(any attribute name or list terminator)";
		hash_table = va_arg(ap, HTABLE *);
		if (va_arg(ap, int) != ATTR_TYPE_END)
		    msg_panic("%s: ATTR_TYPE_HASH not followed by ATTR_TYPE_END",
			      myname);
	    } else if (wanted_type != ATTR_TYPE_FUNC) {
		wanted_name = va_arg(ap, char *);
	    }
	}

	/*
	 * Locate the next attribute of interest in the input stream.
	 */
	while (wanted_type != ATTR_TYPE_FUNC) {

	    /*
	     * Get the name of the next attribute. Hitting EOF is always bad.
	     * Hitting the end-of-input early is OK if the caller is prepared
	     * to deal with missing inputs.
	     */
	    if (msg_verbose)
		msg_info("%s: wanted attribute: %s",
			 VSTREAM_PATH(fp), wanted_name);
	    if ((ch = attr_scan_plain_string(fp, name_buf, '=',
				    "input attribute name")) == VSTREAM_EOF)
		return (-1);
	    if (ch == '\n' && LEN(name_buf) == 0) {
		if (wanted_type == ATTR_TYPE_END
		    || wanted_type == ATTR_TYPE_HASH)
		    return (conversions);
		if ((flags & ATTR_FLAG_MISSING) != 0)
		    msg_warn("missing attribute %s in input from %s",
			     wanted_name, VSTREAM_PATH(fp));
		return (conversions);
	    }

	    /*
	     * See if the caller asks for this attribute.
	     */
	    if (wanted_type == ATTR_TYPE_HASH
		|| (wanted_type != ATTR_TYPE_END
		    && strcmp(wanted_name, STR(name_buf)) == 0))
		break;
	    if ((flags & ATTR_FLAG_EXTRA) != 0) {
		msg_warn("unexpected attribute %s from %s (expecting: %s)",
			 STR(name_buf), VSTREAM_PATH(fp), wanted_name);
		return (conversions);
	    }

	    /*
	     * Skip over this attribute. The caller does not ask for it.
	     */
	    while (ch != '\n' && (ch = VSTREAM_GETC(fp)) != VSTREAM_EOF)
		 /* void */ ;
	}

	/*
	 * Do the requested conversion.
	 */
	switch (wanted_type) {
	case ATTR_TYPE_INT:
	    if (ch != '=') {
		msg_warn("missing value for number attribute %s from %s",
			 STR(name_buf), VSTREAM_PATH(fp));
		return (-1);
	    }
	    number = va_arg(ap, unsigned int *);
	    if ((ch = attr_scan_plain_number(fp, number, str_buf, 0,
					     "input attribute value")) < 0)
		return (-1);
	    break;
	case ATTR_TYPE_LONG:
	    if (ch != '=') {
		msg_warn("missing value for number attribute %s from %s",
			 STR(name_buf), VSTREAM_PATH(fp));
		return (-1);
	    }
	    long_number = va_arg(ap, unsigned long *);
	    if ((ch = attr_scan_plain_long_number(fp, long_number, str_buf,
					   0, "input attribute value")) < 0)
		return (-1);
	    break;
	case ATTR_TYPE_STR:
	    if (ch != '=') {
		msg_warn("missing value for string attribute %s from %s",
			 STR(name_buf), VSTREAM_PATH(fp));
		return (-1);
	    }
	    string = va_arg(ap, VSTRING *);
	    if ((ch = attr_scan_plain_string(fp, string, 0,
					     "input attribute value")) < 0)
		return (-1);
	    break;
	case ATTR_TYPE_DATA:
	    if (ch != '=') {
		msg_warn("missing value for data attribute %s from %s",
			 STR(name_buf), VSTREAM_PATH(fp));
		return (-1);
	    }
	    string = va_arg(ap, VSTRING *);
	    if ((ch = attr_scan_plain_data(fp, string, 0,
					   "input attribute value")) < 0)
		return (-1);
	    break;
	case ATTR_TYPE_FUNC:
	    scan_fn = va_arg(ap, ATTR_SCAN_SLAVE_FN);
	    scan_arg = va_arg(ap, void *);
	    if (scan_fn(attr_scan_plain, fp, flags | ATTR_FLAG_MORE, scan_arg) < 0)
		return (-1);
	    break;
	case ATTR_TYPE_HASH:
	    if (ch != '=') {
		msg_warn("missing value for string attribute %s from %s",
			 STR(name_buf), VSTREAM_PATH(fp));
		return (-1);
	    }
	    if ((ch = attr_scan_plain_string(fp, str_buf, 0,
					     "input attribute value")) < 0)
		return (-1);
	    if (htable_locate(hash_table, STR(name_buf)) != 0) {
		if ((flags & ATTR_FLAG_EXTRA) != 0) {
		    msg_warn("duplicate attribute %s in input from %s",
			     STR(name_buf), VSTREAM_PATH(fp));
		    return (conversions);
		}
	    } else if (hash_table->used >= ATTR_HASH_LIMIT) {
		msg_warn("attribute count exceeds limit %d in input from %s",
			 ATTR_HASH_LIMIT, VSTREAM_PATH(fp));
		return (conversions);
	    } else {
		htable_enter(hash_table, STR(name_buf),
			     mystrdup(STR(str_buf)));
	    }
	    break;
	default:
	    msg_panic("%s: unknown type code: %d", myname, wanted_type);
	}
    }
}

/* attr_scan_plain - read attribute list from stream */

int     attr_scan_plain(VSTREAM *fp, int flags,...)
{
    va_list ap;
    int     ret;

    va_start(ap, flags);
    ret = attr_vscan_plain(fp, flags, ap);
    va_end(ap);
    return (ret);
}

#ifdef TEST

 /*
  * Proof of concept test program.  Mirror image of the attr_scan_plain test
  * program.
  */
#include <msg_vstream.h>

int     var_line_limit = 2048;

int     main(int unused_argc, char **used_argv)
{
    VSTRING *data_val = vstring_alloc(1);
    VSTRING *str_val = vstring_alloc(1);
    HTABLE *table = htable_create(1);
    HTABLE_INFO **ht_info_list;
    HTABLE_INFO **ht;
    int     int_val;
    long    long_val;
    int     ret;

    msg_verbose = 1;
    msg_vstream_init(used_argv[0], VSTREAM_ERR);
    if ((ret = attr_scan_plain(VSTREAM_IN,
			       ATTR_FLAG_STRICT,
			       ATTR_TYPE_INT, ATTR_NAME_INT, &int_val,
			       ATTR_TYPE_LONG, ATTR_NAME_LONG, &long_val,
			       ATTR_TYPE_STR, ATTR_NAME_STR, str_val,
			       ATTR_TYPE_DATA, ATTR_NAME_DATA, data_val,
			       ATTR_TYPE_HASH, table,
			       ATTR_TYPE_END)) > 4) {
	vstream_printf("%s %d\n", ATTR_NAME_INT, int_val);
	vstream_printf("%s %ld\n", ATTR_NAME_LONG, long_val);
	vstream_printf("%s %s\n", ATTR_NAME_STR, STR(str_val));
	vstream_printf("%s %s\n", ATTR_NAME_DATA, STR(data_val));
	ht_info_list = htable_list(table);
	for (ht = ht_info_list; *ht; ht++)
	    vstream_printf("(hash) %s %s\n", ht[0]->key, ht[0]->value);
	myfree((char *) ht_info_list);
    } else {
	vstream_printf("return: %d\n", ret);
    }
    if ((ret = attr_scan_plain(VSTREAM_IN,
			       ATTR_FLAG_STRICT,
			       ATTR_TYPE_INT, ATTR_NAME_INT, &int_val,
			       ATTR_TYPE_LONG, ATTR_NAME_LONG, &long_val,
			       ATTR_TYPE_STR, ATTR_NAME_STR, str_val,
			       ATTR_TYPE_DATA, ATTR_NAME_DATA, data_val,
			       ATTR_TYPE_END)) == 4) {
	vstream_printf("%s %d\n", ATTR_NAME_INT, int_val);
	vstream_printf("%s %ld\n", ATTR_NAME_LONG, long_val);
	vstream_printf("%s %s\n", ATTR_NAME_STR, STR(str_val));
	vstream_printf("%s %s\n", ATTR_NAME_DATA, STR(data_val));
	ht_info_list = htable_list(table);
	for (ht = ht_info_list; *ht; ht++)
	    vstream_printf("(hash) %s %s\n", ht[0]->key, ht[0]->value);
	myfree((char *) ht_info_list);
    } else {
	vstream_printf("return: %d\n", ret);
    }
    if (vstream_fflush(VSTREAM_OUT) != 0)
	msg_fatal("write error: %m");

    vstring_free(data_val);
    vstring_free(str_val);
    htable_free(table, myfree);

    return (0);
}

#endif
