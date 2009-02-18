/*	$NetBSD: logging.c,v 1.1.1.1 2009/02/18 11:16:33 haad Exp $	*/

#include <syslog.h>

int log_tabbing = 0;
int log_is_open = 0;

/*
 * Variables for various conditional logging
 */
#ifdef MEMB
int log_membership_change = 1;
#else
int log_membership_change = 0;
#endif

#ifdef CKPT
int log_checkpoint = 1;
#else
int log_checkpoint = 0;
#endif

#ifdef RESEND
int log_resend_requests = 1;
#else
int log_resend_requests = 0;
#endif
