/*	$NetBSD: msg.md.es,v 1.1 2008/01/02 11:30:32 mrg Exp $	*/

/*
 * Copyright 1997 Piermont Information Systems Inc.
 * All rights reserved.
 *
 * Written by Philip A. Nelson for Piermont Information Systems Inc.
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
 *      This product includes software developed for the NetBSD Project by
 *      Piermont Information Systems Inc.
 * 4. The name of Piermont Information Systems Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PIERMONT INFORMATION SYSTEMS INC. ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL PIERMONT INFORMATION SYSTEMS INC. BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* MD Message catalog -- spanish, ofppc version */

message md_hello
{
}

message dobad144
{Instalando la tabla de bloques malos ...
}

message dobootblks
{Instalando bloques de arranque en %s....
}

message pickdisk
{Escoja disco: }

message partabovechs
{La parte NetBSD del disco est� fuera del rango que la BIOS de su maquina
puede acceder.
Puede que no sea posible iniciar desde ahi.
�Est� seguro de que quiere hacer eso?

(Seleccionando 'no' le devolver� al menu de edicion de particiones.)}

message set_kernel_1
{N�cleo (GENERIC)}

message nobootpart
{There is no boot partition in the MBR partition table.}

message boottoosmall
{The boot partition is too small.  It needs to be at least 2MB in size,
however a size of at least 100MB is reccomended.}

message nobootpartdisklabel
{There is no boot partition in the disklabel.  The boot partition should 
match the boot partition you set up in the MBR partition table.}
