/*	$NetBSD: libntpq.h,v 1.1.1.1 2009/12/13 16:56:28 kardel Exp $	*/

/*****************************************************************************
 *
 *  libntpq.h
 *
 *  This is the wrapper library for ntpq, the NTP query utility. 
 *  This library reuses the sourcecode from ntpq and exports a number
 *  of useful functions in a library that can be linked against applications
 *  that need to query the status of a running ntpd. The whole 
 *  communcation is based on mode 6 packets.
 *
 * This header file can be used in applications that want to link against 
 * libntpq.
 *
 ****************************************************************************/

#include "ntp_net.h"

/* general purpose buffer size */
#define NTPQ_BUFLEN 2048

/* max. number of associations */
#ifndef MAXASSOC
#define MAXASSOC    1024
#endif

/* general purpose max array size definition */
#ifndef MAXLIST
#define MAXLIST 64
#endif

#ifndef LENHOSTNAME
#define LENHOSTNAME 256     /* host name is max. 256 characters long */
#endif

/* NTP Status codes */
#define NTP_STATUS_INVALID      0
#define NTP_STATUS_FALSETICKER  1
#define NTP_STATUS_EXCESS       2
#define NTP_STATUS_OUTLYER      3
#define NTP_STATUS_CANDIDATE    4
#define NTP_STATUS_SELECTED     5
#define NTP_STATUS_SYSPEER      6
#define NTP_STATUS_PPSPEER      7

/* NTP association type identifier */
#define NTP_CLOCKTYPE_UNKNOWN   '-'
#define NTP_CLOCKTYPE_BROADCAST 'b'
#define NTP_CLOCKTYPE_LOCAL     'l'
#define NTP_CLOCKTYPE_UNICAST   'u'
#define NTP_CLOCKTYPE_MULTICAST 'm'

/* Variable Sets */
#define PEERVARS CTL_OP_READVAR
#define CLOCKVARS CTL_OP_CLOCKVAR

/* Variable list struct */
struct ntpq_varlist {
	char *name;
	char *value;
};

/* global variables used for holding snapshots of data */
#ifndef _LIBNTPQC
extern char peervars[];
extern int peervarlen;
extern int peervar_assoc;
extern char clockvars[];
extern int clockvarlen;
extern int clockvar_assoc;
extern char sysvars[];
extern int sysvarlen;
extern char *ntpq_resultbuffer[];
extern struct ntpq_varlist ntpq_varlist[MAXLIST];
#endif



/* 
 * Prototypes of exported libary functions
 */

/* from libntpq.c */
#ifndef _LIBNTPQC
extern int ntpq_openhost(char *);
extern int ntpq_closehost(void);
extern int ntpq_queryhost(unsigned short VARSET, unsigned short association, char *resultbuf, int maxlen);
extern int ntpq_getvar( char *resultbuf, int datalen, const char *varname, char *varvalue, int maxlen);
extern int ntpq_stripquotes ( char *resultbuf, char *srcbuf, int datalen, int maxlen );
extern int ntpq_queryhost_peervars(unsigned short association, char *resultbuf, int maxlen);
extern int ntpq_get_peervar( const char *varname, char *varvalue, int maxlen);
extern int ntpq_read_sysvars( char *resultbuf, int maxsize );
extern int ntpq_get_sysvars( void );
extern int ntpq_read_associations ( unsigned short resultbuf[], int max_entries );
extern int ntpq_get_assocs ( void );
extern int ntpq_get_assoc_number ( int associd );
extern int ntpq_get_assoc_peervars( int associd );
extern int ntpq_get_assoc_clockvars( int associd );
extern int ntpq_get_assoc_allvars( int associd  );
extern int ntpq_get_assoc_clocktype ( int assoc_number );
extern int ntpq_read_assoc_peervars( int associd, char *resultbuf, int maxsize );
extern int ntpq_read_assoc_clockvars( int associd, char *resultbuf, int maxsize );
 #endif

/* in libntpq_subs.c */
#ifndef _LIBNTPQSUBSC
extern int ntpq_dogetassoc(void);
extern char ntpq_decodeaddrtype(sockaddr_u *sock);
extern int ntpq_doquerylist(struct ntpq_varlist *, int , int , int , u_short *, int *, char **datap );
#endif

