/*	$NetBSD: acpi_quirks.c,v 1.7 2006/07/04 00:30:23 christos Exp $	*/

/*
 * Copyright 2002 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Frank van der Linden for Wasabi Systems, Inc.
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
 *      Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>

__KERNEL_RCSID(0, "$NetBSD: acpi_quirks.c,v 1.7 2006/07/04 00:30:23 christos Exp $");

#include "opt_acpi.h"

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/kernel.h>
#include <sys/proc.h>

#include <dev/acpi/acpica.h>
#include <dev/acpi/acpireg.h>
#include <dev/acpi/acpivar.h>

static int acpi_rev_cmp(uint32_t, uint32_t, int);

/*
 * XXX add more
 */
static struct acpi_quirk acpi_quirks[] = {
	{ ACPI_TABLE_FADT, "PTLTD ", 0x06040000, AQ_LTE, "  FACP  ",
	  ACPI_QUIRK_BROKEN },
};

static int
acpi_rev_cmp(uint32_t tabval, uint32_t wanted, int op)
{
	switch (op) {
	case AQ_GT:
		if (tabval > wanted)
			return 0;
		else
			return 1;
	case AQ_LT:
		if (tabval < wanted)
			return 0;
		else
			return 1;
	case AQ_LTE:
		if (tabval <= wanted)
			return 0;
		else
			return 1;
	case AQ_GTE:
		if (tabval >= wanted)
			return 0;
		else
			return 1;
	case AQ_EQ:
		if (tabval == wanted)
			return 0;
		else
			return 1;
	}
	return 1;
}

/*
 * Simple function to search the quirk table. Only to be used after
 * AcpiLoadTables has been successfully called.
 */
int
acpi_find_quirks(void)
{
	int i, nquirks;
	struct acpi_quirk *aqp;
	ACPI_TABLE_HEADER *hdr;

	nquirks = sizeof(acpi_quirks) / sizeof(struct acpi_quirk);

	for (i = 0; i < nquirks; i++) {
		aqp = &acpi_quirks[i];
		/* XXX AcpiGetTableHeader doesn't work for some reason */
		switch (aqp->aq_tabletype) {
		case ACPI_TABLE_DSDT:
			hdr = (ACPI_TABLE_HEADER *)AcpiGbl_DSDT;
			break;
		case ACPI_TABLE_XSDT:
			hdr = (ACPI_TABLE_HEADER *)AcpiGbl_XSDT;
			break;
		case ACPI_TABLE_FADT:
			hdr = (ACPI_TABLE_HEADER *)AcpiGbl_FADT;
			break;
		default:
			continue;
		}
		if (strncmp(aqp->aq_oemid, hdr->OemId, strlen(aqp->aq_oemid)))
			continue;
		if (acpi_rev_cmp(aqp->aq_oemrev, hdr->OemRevision,
		    aqp->aq_cmpop))
			continue;
		if (strncmp(aqp->aq_tabid, hdr->OemTableId,
		    strlen(aqp->aq_tabid)))
			continue;
		return aqp->aq_quirks;
	}
	return 0;
}
