/*	$NetBSD: ipaq_atmel.h,v 1.1 2001/07/31 21:33:03 ichiro Exp $	*/

/*-
 * Copyright (c) 2001 The NetBSD Foundation, Inc.  All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Ichiro FUKUHARA (ichiro@ichiro.org).
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
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Atmel microcontroller (Proxy Controller for StrongARM)
 */

/* Frame character */
#define FRAME_SOF		0x02	/* Start of Frame character */
#define FRAME_EOF		0x04	/* End of Frame character */

/* ID */
#define ID_VERSION		0x00	/* Get version ID */
#define EVENT_BUTTON		0x02	/* Get event of BUTTON */
#define EVENT_TS		0x03	/* Get event of Touch Screen */
#define READ_EEPROM		0x04	/* Read EEPROM */
#define WRITE_EEPROM		0x05	/* Write EEPROM */
#define SW_LED			0x08	/* Switch of LED */
#define STATUS_BATTERY		0x09	/* Get status of battery */
#define READ_IIC		0x0b	/* Read data via IIC protocol */
#define WRITE_IIC		0x0c	/* Write data via IIC protocol */
#define SET_BACKLIGHT		0x0d	/* Backlight Control command */
#define STATUS_EXTP		0xa1	/* Get status of Extension Pack */

/*
 * Battery status
 */
#define BATT_STATUS_HIGH		0x00
#define BATT_STATUS_LOW			0x01
#define BATT_STATUS_CRITICAL 		0x02
#define BATT_STATUS_CHARGING		0x04
#define BATT_STATUS_CHARGING_MAIN	0x10
#define BATT_STATUS_NOBATT		0x80
#define BATT_STATUS_UNKNOWN		0xff

/*
 * AC status   
 */
#define AC_STATUS_OFFLINE		0x00
#define AC_STATUS_ONLINE		0x01
#define AC_STATUS_BACKUP		0x02
#define AC_STATUS_UNKNOWN		0xff

/*
 * kind of battery
 */
#define BATT_ALKALINE			0x01
#define BATT_NICD			0x02
#define BATT_NIMH			0x03
#define BATT_LION			0x04
#define BATT_LIPOLY			0x05
#define BATT_UNKNOWN			0xff
