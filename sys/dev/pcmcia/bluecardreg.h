/*	$NetBSD: bluecardreg.h,v 1.1 2007/08/20 00:29:43 kiyohara Exp $	*/
/*
 * Copyright (c) 2007 KIYOHARA Takashi
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define BLUECARD_BUF1		0x00
#define BLUECARD_BUF2		0x10
#define BLUECARD_COMMAND	0x20
#define BLUECARD_INTERRUPT	0x21
#define BLUECARD_CONTROL	0x22
#define BLUECARD_RXCONTROL	0x24
#define BLUECARD_CARDRESET	0x30
#define BLUECARD_LEDCONTROL	0x30

#define BLUECARD_COMMAND_TXBUF1		0x01
#define BLUECARD_COMMAND_TXBUF2		0x02
#define BLUECARD_COMMAND_RXBUF1		0x04
#define BLUECARD_COMMAND_RXBUF2		0x08
#define BLUECARD_COMMAND_RXWIN1		0x00
#define BLUECARD_COMMAND_RXWIN2		0x10

#define BLUECARD_INTERRUPT_TXBUF1	0x01
#define BLUECARD_INTERRUPT_TXBUF2	0x02
#define BLUECARD_INTERRUPT_RXBUF1	0x04
#define BLUECARD_INTERRUPT_RXBUF2	0x08

#define BLUECARD_CONTROL_BAUDRATE_MASK	0x03
#define BLUECARD_CONTROL_BAUDRATE_57600	0x00
#define BLUECARD_CONTROL_BAUDRATE_115200 0x01
#define BLUECARD_CONTROL_BAUDRATE_230400 0x02
#define BLUECARD_CONTROL_BAUDRATE_460800 0x03
#define BLUECARD_CONTROL_RTS		0x04
#define BLUECARD_CONTROL_ON             0x08
#define BLUECARD_CONTROL_RESET		0x10
#define BLUECARD_CONTROL_RESPU		0x20
#define BLUECARD_CONTROL_INTERRUPT	0x40
#define BLUECARD_CONTROL_CARDRESET	0x80

#define BLUECARD_RXCONTROL_RTSLEVEL(x)	((x) << 2)	
