/*	$NetBSD: postmap.c,v 1.1.1.10 2007/02/05 17:41:38 rpaulo Exp $	*/

/*++
/* NAME
/*	postmap 1
/* SUMMARY
/*	Postfix lookup table management
/* SYNOPSIS
/* .fi
/*	\fBpostmap\fR [\fB-Nfinoprsvw\fR] [\fB-c \fIconfig_dir\fR]
/*	[\fB-d \fIkey\fR] [\fB-q \fIkey\fR]
/*		[\fIfile_type\fR:]\fIfile_name\fR ...
/* DESCRIPTION
/*	The \fBpostmap\fR(1) command creates or queries one or more Postfix
/*	lookup tables, or updates an existing one. The input and output
/*	file formats are expected to be compatible with:
/*
/* .ti +4
/*	\fBmakemap \fIfile_type\fR \fIfile_name\fR < \fIfile_name\fR
/*
/*	If the result files do not exist they will be created with the
/*	same group and other read permissions as their source file.
/*
/*	While the table update is in progress, signal delivery is
/*	postponed, and an exclusive, advisory, lock is placed on the
/*	entire table, in order to avoid surprises in spectator
/*	processes.
/* INPUT FILE FORMAT
/* .ad
/* .fi
/*	The format of a lookup table input file is as follows:
/* .IP \(bu
/*	A table entry has the form
/* .sp
/* .ti +5
/*	\fIkey\fR whitespace \fIvalue\fR
/* .IP \(bu
/*	Empty lines and whitespace-only lines are ignored, as
/*	are lines whose first non-whitespace character is a `#'.
/* .IP \(bu
/*	A logical line starts with non-whitespace text. A line that
/*	starts with whitespace continues a logical line.
/* .PP
/*	The \fIkey\fR and \fIvalue\fR are processed as is, except that
/*	surrounding white space is stripped off. Unlike with Postfix alias
/*	databases, quotes cannot be used to protect lookup keys that contain
/*	special characters such as `#' or whitespace. 
/*
/*	By default the lookup key is mapped to lowercase to make
/*	the lookups case insensitive; as of Postfix 2.3 this case
/*	folding happens only with tables whose lookup keys are
/*	fixed-case strings such as btree:, dbm: or hash:. With
/*	earlier versions, the lookup key is folded even with tables
/*	where a lookup field can match both upper and lower case
/*	text, such as regexp: and pcre:. This resulted in loss of
/*	information with $\fInumber\fR substitutions.
/* COMMAND-LINE ARGUMENTS
/* .ad
/* .fi
/* .IP "\fB-c \fIconfig_dir\fR"
/*	Read the \fBmain.cf\fR configuration file in the named directory
/*	instead of the default configuration directory.
/* .IP "\fB-d \fIkey\fR"
/*	Search the specified maps for \fIkey\fR and remove one entry per map.
/*	The exit status is zero when the requested information was found.
/*
/*	If a key value of \fB-\fR is specified, the program reads key
/*	values from the standard input stream. The exit status is zero
/*	when at least one of the requested keys was found.
/* .IP \fB-f\fR
/*	Do not fold the lookup key to lower case while creating or querying
/*	a table.
/*
/*	With Postfix version 2.3 and later, this option has no
/*	effect for regular expression tables. There, case folding
/*	is controlled by appending a flag to a pattern.
/* .IP \fB-i\fR
/*	Incremental mode. Read entries from standard input and do not
/*	truncate an existing database. By default, \fBpostmap\fR(1) creates
/*	a new database from the entries in \fBfile_name\fR.
/* .IP \fB-N\fR
/*	Include the terminating null character that terminates lookup keys
/*	and values. By default, \fBpostmap\fR(1) does whatever is
/*	the default for
/*	the host operating system.
/* .IP \fB-n\fR
/*	Don't include the terminating null character that terminates lookup
/*	keys and values. By default, \fBpostmap\fR(1) does whatever
/*	is the default for
/*	the host operating system.
/* .IP \fB-o\fR
/*	Do not release root privileges when processing a non-root
/*	input file. By default, \fBpostmap\fR(1) drops root privileges
/*	and runs as the source file owner instead.
/* .IP \fB-p\fR
/*	Do not inherit the file access permissions from the input file
/*	when creating a new file.  Instead, create a new file with default
/*	access permissions (mode 0644).
/* .IP "\fB-q \fIkey\fR"
/*	Search the specified maps for \fIkey\fR and write the first value
/*	found to the standard output stream. The exit status is zero
/*	when the requested information was found.
/*
/*	If a key value of \fB-\fR is specified, the program reads key
/*	values from the standard input stream and writes one line of
/*	\fIkey value\fR output for each key that was found. The exit
/*	status is zero when at least one of the requested keys was found.
/* .IP \fB-r\fR
/*	When updating a table, do not complain about attempts to update
/*	existing entries, and make those updates anyway.
/* .IP \fB-s\fR
/*	Retrieve all database elements, and write one line of
/*	\fIkey value\fR output for each element. The elements are
/*	printed in database order, which is not necessarily the same
/*	as the original input order.
/*	This feature is available in Postfix version 2.2 and later,
/*	and is not available for all database types.
/* .IP \fB-v\fR
/*	Enable verbose logging for debugging purposes. Multiple \fB-v\fR
/*	options make the software increasingly verbose.
/* .IP \fB-w\fR
/*	When updating a table, do not complain about attempts to update
/*	existing entries, and ignore those attempts.
/* .PP
/*	Arguments:
/* .IP \fIfile_type\fR
/*	The database type. To find out what types are supported, use
/*	the "\fBpostconf -m\fR" command.
/*
/*	The \fBpostmap\fR(1) command can query any supported file type,
/*	but it can create only the following file types:
/* .RS
/* .IP \fBbtree\fR
/*	The output file is a btree file, named \fIfile_name\fB.db\fR.
/*	This is available on systems with support for \fBdb\fR databases.
/* .IP \fBcdb\fR
/*	The output consists of one file, named \fIfile_name\fB.cdb\fR.
/*	This is available on systems with support for \fBcdb\fR databases.
/* .IP \fBdbm\fR
/*	The output consists of two files, named \fIfile_name\fB.pag\fR and
/*	\fIfile_name\fB.dir\fR.
/*	This is available on systems with support for \fBdbm\fR databases.
/* .IP \fBhash\fR
/*	The output file is a hashed file, named \fIfile_name\fB.db\fR.
/*	This is available on systems with support for \fBdb\fR databases.
/* .IP \fBsdbm\fR
/*	The output consists of two files, named \fIfile_name\fB.pag\fR and
/*	\fIfile_name\fB.dir\fR.
/*	This is available on systems with support for \fBsdbm\fR databases.
/* .PP
/*	When no \fIfile_type\fR is specified, the software uses the database
/*	type specified via the \fBdefault_database_type\fR configuration
/*	parameter.
/* .RE
/* .IP \fIfile_name\fR
/*	The name of the lookup table source file when rebuilding a database.
/* DIAGNOSTICS
/*	Problems are logged to the standard error stream and to
/*	\fBsyslogd\fR(8).
/*	No output means that no problems were detected. Duplicate entries are
/*	skipped and are flagged with a warning.
/*
/*	\fBpostmap\fR(1) terminates with zero exit status in case of success
/*	(including successful "\fBpostmap -q\fR" lookup) and terminates
/*	with non-zero exit status in case of failure.
/* ENVIRONMENT
/* .ad
/* .fi
/* .IP \fBMAIL_CONFIG\fR
/*	Directory with Postfix configuration files.
/* .IP \fBMAIL_VERBOSE\fR
/*	Enable verbose logging for debugging purposes.
/* CONFIGURATION PARAMETERS
/* .ad
/* .fi
/*	The following \fBmain.cf\fR parameters are especially relevant to
/*	this program.
/*	The text below provides only a parameter summary. See
/*	\fBpostconf\fR(5) for more details including examples.
/* .IP "\fBberkeley_db_create_buffer_size (16777216)\fR"
/*	The per-table I/O buffer size for programs that create Berkeley DB
/*	hash or btree tables.
/* .IP "\fBberkeley_db_read_buffer_size (131072)\fR"
/*	The per-table I/O buffer size for programs that read Berkeley DB
/*	hash or btree tables.
/* .IP "\fBconfig_directory (see 'postconf -d' output)\fR"
/*	The default location of the Postfix main.cf and master.cf
/*	configuration files.
/* .IP "\fBdefault_database_type (see 'postconf -d' output)\fR"
/*	The default database type for use in \fBnewaliases\fR(1), \fBpostalias\fR(1)
/*	and \fBpostmap\fR(1) commands.
/* .IP "\fBsyslog_facility (mail)\fR"
/*	The syslog facility of Postfix logging.
/* .IP "\fBsyslog_name (postfix)\fR"
/*	The mail system name that is prepended to the process name in syslog
/*	records, so that "smtpd" becomes, for example, "postfix/smtpd".
/* SEE ALSO
/*	postalias(1), create/update/query alias database
/*	postconf(1), supported database types
/*	postconf(5), configuration parameters
/*	syslogd(8), system logging
/* README FILES
/* .ad
/* .fi
/*	Use "\fBpostconf readme_directory\fR" or
/*	"\fBpostconf html_directory\fR" to locate this information.
/* .na
/* .nf
/*	DATABASE_README, Postfix lookup table overview
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
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <vstring.h>
#include <vstream.h>
#include <msg_vstream.h>
#include <msg_syslog.h>
#include <readlline.h>
#include <stringops.h>
#include <split_at.h>
#include <vstring_vstream.h>
#include <set_eugid.h>

/* Global library. */

#include <mail_conf.h>
#include <mail_dict.h>
#include <mail_params.h>
#include <mkmap.h>
#include <mail_task.h>

/* Application-specific. */

#define STR	vstring_str

#define POSTMAP_FLAG_AS_OWNER	(1<<0)	/* open dest as owner of source */
#define POSTMAP_FLAG_SAVE_PERM	(1<<1)	/* copy access permission from source */

/* postmap - create or update mapping database */

static void postmap(char *map_type, char *path_name, int postmap_flags,
		            int open_flags, int dict_flags)
{
    VSTREAM *source_fp;
    VSTRING *line_buffer;
    MKMAP  *mkmap;
    int     lineno;
    char   *key;
    char   *value;
    struct stat st;
    mode_t  saved_mask;

    /*
     * Initialize.
     */
    line_buffer = vstring_alloc(100);
    if ((open_flags & O_TRUNC) == 0) {
	source_fp = VSTREAM_IN;
	vstream_control(source_fp, VSTREAM_CTL_PATH, "stdin", VSTREAM_CTL_END);
    } else if ((source_fp = vstream_fopen(path_name, O_RDONLY, 0)) == 0) {
	msg_fatal("open %s: %m", path_name);
    }
    if (fstat(vstream_fileno(source_fp), &st) < 0)
	msg_fatal("fstat %s: %m", path_name);

    /*
     * Turn off group/other read permissions as indicated in the source file.
     */
    if ((postmap_flags & POSTMAP_FLAG_SAVE_PERM) && S_ISREG(st.st_mode))
	saved_mask = umask(022 | (~st.st_mode & 077));

    /*
     * If running as root, run as the owner of the source file, so that the
     * result shows proper ownership, and so that a bug in postmap does not
     * allow privilege escalation.
     */
    if ((postmap_flags & POSTMAP_FLAG_AS_OWNER) && getuid() == 0
	&& (st.st_uid != geteuid() || st.st_gid != getegid()))
	set_eugid(st.st_uid, st.st_gid);

    /*
     * Open the database, optionally create it when it does not exist,
     * optionally truncate it when it does exist, and lock out any
     * spectators.
     */
    mkmap = mkmap_open(map_type, path_name, open_flags, dict_flags);

    /*
     * And restore the umask, in case it matters.
     */
    if ((postmap_flags & POSTMAP_FLAG_SAVE_PERM) && S_ISREG(st.st_mode))
	umask(saved_mask);

    /*
     * Add records to the database.
     */
    lineno = 0;
    while (readlline(line_buffer, source_fp, &lineno)) {

	/*
	 * Split on the first whitespace character, then trim leading and
	 * trailing whitespace from key and value.
	 */
	key = STR(line_buffer);
	value = key + strcspn(key, " \t\r\n");
	if (*value)
	    *value++ = 0;
	while (ISSPACE(*value))
	    value++;
	trimblanks(key, 0)[0] = 0;
	trimblanks(value, 0)[0] = 0;

	/*
	 * Enforce the "key whitespace value" format. Disallow missing keys
	 * or missing values.
	 */
	if (*key == 0 || *value == 0) {
	    msg_warn("%s, line %d: expected format: key whitespace value",
		     VSTREAM_PATH(source_fp), lineno);
	    continue;
	}
	if (key[strlen(key) - 1] == ':')
	    msg_warn("%s, line %d: record is in \"key: value\" format; is this an alias file?",
		     VSTREAM_PATH(source_fp), lineno);

	/*
	 * Store the value under a case-insensitive key.
	 */
	mkmap_append(mkmap, key, value);
    }

    /*
     * Close the mapping database, and release the lock.
     */
    mkmap_close(mkmap);

    /*
     * Cleanup. We're about to terminate, but it is a good sanity check.
     */
    vstring_free(line_buffer);
    if (source_fp != VSTREAM_IN)
	vstream_fclose(source_fp);
}

/* postmap_queries - apply multiple requests from stdin */

static int postmap_queries(VSTREAM *in, char **maps, const int map_count,
			           const int dict_flags)
{
    int     found = 0;
    VSTRING *keybuf = vstring_alloc(100);
    DICT  **dicts;
    const char *map_name;
    const char *value;
    int     n;

    /*
     * Sanity check.
     */
    if (map_count <= 0)
	msg_panic("postmap_queries: bad map count");

    /*
     * Prepare to open maps lazily.
     */
    dicts = (DICT **) mymalloc(sizeof(*dicts) * map_count);
    for (n = 0; n < map_count; n++)
	dicts[n] = 0;

    /*
     * Perform all queries. Open maps on the fly, to avoid opening unecessary
     * maps.
     */
    while (vstring_get_nonl(keybuf, in) != VSTREAM_EOF) {
	for (n = 0; n < map_count; n++) {
	    if (dicts[n] == 0)
		dicts[n] = ((map_name = split_at(maps[n], ':')) != 0 ?
		       dict_open3(maps[n], map_name, O_RDONLY, dict_flags) :
		    dict_open3(var_db_type, maps[n], O_RDONLY, dict_flags));
	    if ((value = dict_get(dicts[n], STR(keybuf))) != 0) {
		if (*value == 0) {
		    msg_warn("table %s:%s: key %s: empty string result is not allowed",
			     dicts[n]->type, dicts[n]->name, STR(keybuf));
		    msg_warn("table %s:%s should return NO RESULT in case of NOT FOUND",
			     dicts[n]->type, dicts[n]->name);
		}
		vstream_printf("%s	%s\n", STR(keybuf), value);
		found = 1;
		break;
	    }
	}
    }
    if (found)
	vstream_fflush(VSTREAM_OUT);

    /*
     * Cleanup.
     */
    for (n = 0; n < map_count; n++)
	if (dicts[n])
	    dict_close(dicts[n]);
    myfree((char *) dicts);
    vstring_free(keybuf);

    return (found);
}

/* postmap_query - query a map and print the result to stdout */

static int postmap_query(const char *map_type, const char *map_name,
			           const char *key, int dict_flags)
{
    DICT   *dict;
    const char *value;

    dict = dict_open3(map_type, map_name, O_RDONLY, dict_flags);
    if ((value = dict_get(dict, key)) != 0) {
	if (*value == 0) {
	    msg_warn("table %s:%s: key %s: empty string result is not allowed",
		     map_type, map_name, key);
	    msg_warn("table %s:%s should return NO RESULT in case of NOT FOUND",
		     map_type, map_name);
	}
	vstream_printf("%s\n", value);
    }
    vstream_fflush(VSTREAM_OUT);
    dict_close(dict);
    return (value != 0);
}

/* postmap_deletes - apply multiple requests from stdin */

static int postmap_deletes(VSTREAM *in, char **maps, const int map_count,
			             int dict_flags)
{
    int     found = 0;
    VSTRING *keybuf = vstring_alloc(100);
    DICT  **dicts;
    const char *map_name;
    int     n;

    /*
     * Sanity check.
     */
    if (map_count <= 0)
	msg_panic("postmap_deletes: bad map count");

    /*
     * Open maps ahead of time.
     */
    dicts = (DICT **) mymalloc(sizeof(*dicts) * map_count);
    for (n = 0; n < map_count; n++)
	dicts[n] = ((map_name = split_at(maps[n], ':')) != 0 ?
		    dict_open3(maps[n], map_name, O_RDWR, dict_flags) :
		    dict_open3(var_db_type, maps[n], O_RDWR, dict_flags));

    /*
     * Perform all requests.
     */
    while (vstring_get_nonl(keybuf, in) != VSTREAM_EOF)
	for (n = 0; n < map_count; n++)
	    found |= (dict_del(dicts[n], STR(keybuf)) == 0);

    /*
     * Cleanup.
     */
    for (n = 0; n < map_count; n++)
	if (dicts[n])
	    dict_close(dicts[n]);
    myfree((char *) dicts);
    vstring_free(keybuf);

    return (found);
}

/* postmap_delete - delete a (key, value) pair from a map */

static int postmap_delete(const char *map_type, const char *map_name,
			            const char *key, int dict_flags)
{
    DICT   *dict;
    int     status;

    dict = dict_open3(map_type, map_name, O_RDWR, dict_flags);
    status = dict_del(dict, key);
    dict_close(dict);
    return (status == 0);
}

/* postmap_seq - print all map entries to stdout */

static void postmap_seq(const char *map_type, const char *map_name,
			          int dict_flags)
{
    DICT   *dict;
    const char *key;
    const char *value;
    int     func;

    dict = dict_open3(map_type, map_name, O_RDONLY, dict_flags);
    for (func = DICT_SEQ_FUN_FIRST; /* void */ ; func = DICT_SEQ_FUN_NEXT) {
	if (dict_seq(dict, func, &key, &value) != 0)
	    break;
	if (*key == 0) {
	    msg_warn("table %s:%s: empty lookup key value is not allowed",
		     map_type, map_name);
	} else if (*value == 0) {
	    msg_warn("table %s:%s: key %s: empty string result is not allowed",
		     map_type, map_name, key);
	    msg_warn("table %s:%s should return NO RESULT in case of NOT FOUND",
		     map_type, map_name);
	}
	vstream_printf("%s	%s\n", key, value);
    }
    vstream_fflush(VSTREAM_OUT);
    dict_close(dict);
}

/* usage - explain */

static NORETURN usage(char *myname)
{
    msg_fatal("usage: %s [-Nfinoprsvw] [-c config_dir] [-d key] [-q key] [map_type:]file...",
	      myname);
}

int     main(int argc, char **argv)
{
    char   *path_name;
    int     ch;
    int     fd;
    char   *slash;
    struct stat st;
    int     postmap_flags = POSTMAP_FLAG_AS_OWNER | POSTMAP_FLAG_SAVE_PERM;
    int     open_flags = O_RDWR | O_CREAT | O_TRUNC;
    int     dict_flags = DICT_FLAG_DUP_WARN | DICT_FLAG_FOLD_FIX;
    char   *query = 0;
    char   *delkey = 0;
    int     sequence = 0;
    int     found;

    /*
     * Be consistent with file permissions.
     */
    umask(022);

    /*
     * To minimize confusion, make sure that the standard file descriptors
     * are open before opening anything else. XXX Work around for 44BSD where
     * fstat can return EBADF on an open file descriptor.
     */
    for (fd = 0; fd < 3; fd++)
	if (fstat(fd, &st) == -1
	    && (close(fd), open("/dev/null", O_RDWR, 0)) != fd)
	    msg_fatal("open /dev/null: %m");

    /*
     * Process environment options as early as we can. We are not set-uid,
     * and we are supposed to be running in a controlled environment.
     */
    if (getenv(CONF_ENV_VERB))
	msg_verbose = 1;

    /*
     * Initialize. Set up logging, read the global configuration file and
     * extract configuration information.
     */
    if ((slash = strrchr(argv[0], '/')) != 0 && slash[1])
	argv[0] = slash + 1;
    msg_vstream_init(argv[0], VSTREAM_ERR);
    msg_syslog_init(mail_task(argv[0]), LOG_PID, LOG_FACILITY);

    /*
     * Parse JCL.
     */
    while ((ch = GETOPT(argc, argv, "Nc:d:finopq:rsvw")) > 0) {
	switch (ch) {
	default:
	    usage(argv[0]);
	    break;
	case 'N':
	    dict_flags |= DICT_FLAG_TRY1NULL;
	    dict_flags &= ~DICT_FLAG_TRY0NULL;
	    break;
	case 'c':
	    if (setenv(CONF_ENV_PATH, optarg, 1) < 0)
		msg_fatal("out of memory");
	    break;
	case 'd':
	    if (sequence || query || delkey)
		msg_fatal("specify only one of -s -q or -d");
	    delkey = optarg;
	    break;
	case 'f':
	    dict_flags &= ~DICT_FLAG_FOLD_FIX;
	    break;
	case 'i':
	    open_flags &= ~O_TRUNC;
	    break;
	case 'n':
	    dict_flags |= DICT_FLAG_TRY0NULL;
	    dict_flags &= ~DICT_FLAG_TRY1NULL;
	    break;
	case 'o':
	    postmap_flags &= ~POSTMAP_FLAG_AS_OWNER;
	    break;
	case 'p':
	    postmap_flags &= ~POSTMAP_FLAG_SAVE_PERM;
	    break;
	case 'q':
	    if (sequence || query || delkey)
		msg_fatal("specify only one of -s -q or -d");
	    query = optarg;
	    break;
	case 'r':
	    dict_flags &= ~(DICT_FLAG_DUP_WARN | DICT_FLAG_DUP_IGNORE);
	    dict_flags |= DICT_FLAG_DUP_REPLACE;
	    break;
	case 's':
	    if (query || delkey)
		msg_fatal("specify only one of -s or -q or -d");
	    sequence = 1;
	    break;
	case 'v':
	    msg_verbose++;
	    break;
	case 'w':
	    dict_flags &= ~(DICT_FLAG_DUP_WARN | DICT_FLAG_DUP_REPLACE);
	    dict_flags |= DICT_FLAG_DUP_IGNORE;
	    break;
	}
    }
    mail_conf_read();
    mail_dict_init();

    /*
     * Use the map type specified by the user, or fall back to a default
     * database type.
     */
    if (delkey) {				/* remove entry */
	if (optind + 1 > argc)
	    usage(argv[0]);
	if (strcmp(delkey, "-") == 0)
	    exit(postmap_deletes(VSTREAM_IN, argv + optind, argc - optind,
				   dict_flags | DICT_FLAG_LOCK) == 0);
	found = 0;
	while (optind < argc) {
	    if ((path_name = split_at(argv[optind], ':')) != 0) {
		found |= postmap_delete(argv[optind], path_name, delkey,
					  dict_flags | DICT_FLAG_LOCK);
	    } else {
		found |= postmap_delete(var_db_type, argv[optind], delkey,
					  dict_flags | DICT_FLAG_LOCK);
	    }
	    optind++;
	}
	exit(found ? 0 : 1);
    } else if (query) {				/* query map(s) */
	if (optind + 1 > argc)
	    usage(argv[0]);
	if (strcmp(query, "-") == 0)
	    exit(postmap_queries(VSTREAM_IN, argv + optind, argc - optind,
				   dict_flags | DICT_FLAG_LOCK) == 0);
	while (optind < argc) {
	    if ((path_name = split_at(argv[optind], ':')) != 0) {
		found = postmap_query(argv[optind], path_name, query,
					dict_flags | DICT_FLAG_LOCK);
	    } else {
		found = postmap_query(var_db_type, argv[optind], query,
					dict_flags | DICT_FLAG_LOCK);
	    }
	    if (found)
		exit(0);
	    optind++;
	}
	exit(1);
    } else if (sequence) {
	while (optind < argc) {
	    if ((path_name = split_at(argv[optind], ':')) != 0) {
		postmap_seq(argv[optind], path_name,
			      dict_flags | DICT_FLAG_LOCK);
	    } else {
		postmap_seq(var_db_type, argv[optind],
			      dict_flags | DICT_FLAG_LOCK);
	    }
	    exit(0);
	}
	exit(1);
    } else {					/* create/update map(s) */
	if (optind + 1 > argc)
	    usage(argv[0]);
	while (optind < argc) {
	    if ((path_name = split_at(argv[optind], ':')) != 0) {
		postmap(argv[optind], path_name, postmap_flags,
			open_flags, dict_flags);
	    } else {
		postmap(var_db_type, argv[optind], postmap_flags,
			open_flags, dict_flags);
	    }
	    optind++;
	}
	exit(0);
    }
}
