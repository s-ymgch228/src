/*	$NetBSD: mpacpi.h,v 1.4 2006/07/04 00:30:22 christos Exp $	*/

#ifndef _X86_MPACPI_H_
#define _X86_MPACPI_H_

struct pcibus_attach_args;

int mpacpi_scan_apics(struct device *, int *, int *);
int mpacpi_find_interrupts(void *);
int mpacpi_pci_attach_hook(struct device *, struct device *,
			   struct pcibus_attach_args *);
int mpacpi_scan_pci(struct device *, struct pcibus_attach_args *, cfprint_t);

struct mp_intr_map;
int mpacpi_findintr_linkdev(struct mp_intr_map *);

#endif /* _X86_MPACPI_H_ */
