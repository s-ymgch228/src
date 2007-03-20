/* drm_drv.h -- Generic driver template -*- linux-c -*-
 * Created: Thu Nov 23 03:10:50 2000 by gareth@valinux.com
 */
/*-
 * Copyright 1999, 2000 Precision Insight, Inc., Cedar Park, Texas.
 * Copyright 2000 VA Linux Systems, Inc., Sunnyvale, California.
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * VA LINUX SYSTEMS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Rickard E. (Rik) Faith <faith@valinux.com>
 *    Gareth Hughes <gareth@valinux.com>
 *
 */

#include <sys/cdefs.h>
/*
__FBSDID("$FreeBSD: src/sys/dev/drm/drm_drv.c,v 1.6 2006/09/07 23:04:47 anholt Exp $");
*/

#include "drmP.h"
#include "drm.h"
#include "drm_sarea.h"

#ifdef DRM_DEBUG_DEFAULT_ON
int drm_debug_flag = 1;
#else
int drm_debug_flag = 0;
#endif

static int drm_load(drm_device_t *dev);
static void drm_unload(drm_device_t *dev);
static drm_pci_id_list_t *drm_find_description(int vendor, int device,
    drm_pci_id_list_t *idlist);

#ifdef __FreeBSD__
#define DRIVER_SOFTC(unit) \
	((drm_device_t *)devclass_get_softc(drm_devclass, unit))

MODULE_VERSION(drm, 1);
MODULE_DEPEND(drm, agp, 1, 1, 1);
MODULE_DEPEND(drm, pci, 1, 1, 1);
#if __FreeBSD_version > 502127
MODULE_DEPEND(drm, mem, 1, 1, 1);
#endif
#endif /* __FreeBSD__ */

#if defined(__NetBSD__) || defined(__OpenBSD__)
#define DRIVER_SOFTC(unit) \
	(((unit)<DRM_MAXUNITS) ? drm_units[(unit)] : NULL)
#endif /* __NetBSD__ || __OpenBSD__ */

static drm_ioctl_desc_t		  drm_ioctls[256] = {
	[DRM_IOCTL_NR(DRM_IOCTL_VERSION)]       = { drm_version,     0 },
	[DRM_IOCTL_NR(DRM_IOCTL_GET_UNIQUE)]    = { drm_getunique,   0 },
	[DRM_IOCTL_NR(DRM_IOCTL_GET_MAGIC)]     = { drm_getmagic,    0 },
	[DRM_IOCTL_NR(DRM_IOCTL_IRQ_BUSID)]     = { drm_irq_by_busid, DRM_MASTER|DRM_ROOT_ONLY},
	[DRM_IOCTL_NR(DRM_IOCTL_GET_MAP)]       = { drm_getmap,      0 },
	[DRM_IOCTL_NR(DRM_IOCTL_GET_CLIENT)]    = { drm_getclient,   0 },
	[DRM_IOCTL_NR(DRM_IOCTL_GET_STATS)]     = { drm_getstats,    0 },
	[DRM_IOCTL_NR(DRM_IOCTL_SET_VERSION)]   = { drm_setversion,  DRM_MASTER|DRM_ROOT_ONLY },

	[DRM_IOCTL_NR(DRM_IOCTL_SET_UNIQUE)]    = { drm_setunique,   DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_BLOCK)]         = { drm_noop,        DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_UNBLOCK)]       = { drm_noop,        DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AUTH_MAGIC)]    = { drm_authmagic,   DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },

	[DRM_IOCTL_NR(DRM_IOCTL_ADD_MAP)]       = { drm_addmap_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_RM_MAP)]        = { drm_rmmap_ioctl, DRM_AUTH },

	[DRM_IOCTL_NR(DRM_IOCTL_SET_SAREA_CTX)] = { drm_setsareactx, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_GET_SAREA_CTX)] = { drm_getsareactx, DRM_AUTH },

	[DRM_IOCTL_NR(DRM_IOCTL_ADD_CTX)]       = { drm_addctx,      DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_RM_CTX)]        = { drm_rmctx,       DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_MOD_CTX)]       = { drm_modctx,      DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_GET_CTX)]       = { drm_getctx,      DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_SWITCH_CTX)]    = { drm_switchctx,   DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_NEW_CTX)]       = { drm_newctx,      DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_RES_CTX)]       = { drm_resctx,      DRM_AUTH },

	[DRM_IOCTL_NR(DRM_IOCTL_ADD_DRAW)]      = { drm_adddraw,     DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_RM_DRAW)]       = { drm_rmdraw,      DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },

	[DRM_IOCTL_NR(DRM_IOCTL_LOCK)]	        = { drm_lock,        DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_UNLOCK)]        = { drm_unlock,      DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_FINISH)]        = { drm_noop,        DRM_AUTH },

	[DRM_IOCTL_NR(DRM_IOCTL_ADD_BUFS)]      = { drm_addbufs_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_MARK_BUFS)]     = { drm_markbufs,    DRM_AUTH|DRM_MASTER },
	[DRM_IOCTL_NR(DRM_IOCTL_INFO_BUFS)]     = { drm_infobufs,    DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_MAP_BUFS)]      = { drm_mapbufs,     DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_FREE_BUFS)]     = { drm_freebufs,    DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_DMA)]           = { drm_dma,         DRM_AUTH },

	[DRM_IOCTL_NR(DRM_IOCTL_CONTROL)]       = { drm_control,     DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },

	[DRM_IOCTL_NR(DRM_IOCTL_AGP_ACQUIRE)]   = { drm_agp_acquire_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_RELEASE)]   = { drm_agp_release_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_ENABLE)]    = { drm_agp_enable_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_INFO)]      = { drm_agp_info_ioctl, DRM_AUTH },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_ALLOC)]     = { drm_agp_alloc_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_FREE)]      = { drm_agp_free_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_BIND)]      = { drm_agp_bind_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_AGP_UNBIND)]    = { drm_agp_unbind_ioctl, DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },

	[DRM_IOCTL_NR(DRM_IOCTL_SG_ALLOC)]      = { drm_sg_alloc,    DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },
	[DRM_IOCTL_NR(DRM_IOCTL_SG_FREE)]       = { drm_sg_free,     DRM_AUTH|DRM_MASTER|DRM_ROOT_ONLY },

	[DRM_IOCTL_NR(DRM_IOCTL_WAIT_VBLANK)]   = { drm_wait_vblank, 0 },
};

#ifdef __FreeBSD__
static struct cdevsw drm_cdevsw = {
#if __FreeBSD_version >= 502103
	.d_version =	D_VERSION,
#endif
	.d_open =	drm_open,
	.d_close =	drm_close,
	.d_read =	drm_read,
	.d_ioctl =	drm_ioctl,
	.d_poll =	drm_poll,
	.d_mmap =	drm_mmap,
	.d_name =	"drm",
#if __FreeBSD_version >= 502103
	.d_flags =	D_TRACKCLOSE | D_NEEDGIANT,
#else
	.d_maj =	145,
	.d_flags =	D_TRACKCLOSE,
#endif
#if __FreeBSD_version < 500000
	.d_bmaj =	-1
#endif
};

int drm_probe(device_t dev, drm_pci_id_list_t *idlist)
{
	drm_pci_id_list_t *id_entry;
	int vendor, device;
#if __FreeBSD_version < 700010
	device_t realdev;

	if (!strcmp(device_get_name(dev), "drmsub"))
		realdev = device_get_parent(dev);
	else
		realdev = dev;
	vendor = pci_get_vendor(realdev);
	device = pci_get_device(realdev);
#else
	vendor = pci_get_vendor(dev);
	device = pci_get_device(dev);
#endif

	id_entry = drm_find_description(vendor, device, idlist);
	if (id_entry != NULL) {
		device_set_desc(dev, id_entry->name);
		return 0;
	}

	return ENXIO;
}

int drm_attach(device_t nbdev, drm_pci_id_list_t *idlist)
{
	drm_device_t *dev;
	drm_pci_id_list_t *id_entry;
	int unit;

	unit = device_get_unit(nbdev);
	dev = device_get_softc(nbdev);

#if __FreeBSD_version < 700010
	if (!strcmp(device_get_name(nbdev), "drmsub"))
		dev->device = device_get_parent(nbdev);
	else
		dev->device = nbdev;
#else
	dev->device = nbdev;
#endif
	dev->devnode = make_dev(&drm_cdevsw,
			unit,
			DRM_DEV_UID,
			DRM_DEV_GID,
			DRM_DEV_MODE,
			"dri/card%d", unit);
#if __FreeBSD_version >= 500000
	mtx_init(&dev->dev_lock, "drm device", NULL, MTX_DEF);
#endif

	id_entry = drm_find_description(pci_get_vendor(dev->device),
	    pci_get_device(dev->device), idlist);
	dev->id_entry = id_entry;

	return drm_load(dev);
}

int drm_detach(device_t dev)
{
	drm_unload(device_get_softc(dev));
	return 0;
}

#ifndef DRM_DEV_NAME
#define DRM_DEV_NAME "drm"
#endif

devclass_t drm_devclass;

#elif defined(__NetBSD__) || defined(__OpenBSD__)

const struct cdevsw drm_cdevsw = {
	drm_open,
	drm_close,
	drm_read,
	nowrite,
	drm_ioctl,
	nostop,
	notty,
	drm_poll,
	drm_mmap,
	nokqfilter,
	D_TTY
};

int drm_refcnt = 0;

drm_device_t *drm_units[DRM_MAXUNITS];

static int init_units = 1;

int drm_probe(struct pci_attach_args *pa, drm_pci_id_list_t *idlist);
void drm_attach(struct device *kdev, struct pci_attach_args *pa,
                drm_pci_id_list_t *idlist);

int drm_probe(struct pci_attach_args *pa, drm_pci_id_list_t *idlist)
{
	int unit;
	drm_pci_id_list_t *id_entry;

	/* first make sure there is place for the device */
        for(unit=0; unit<DRM_MAXUNITS; unit++)
		if(drm_units[unit] == NULL) break;
	if(unit == DRM_MAXUNITS) return 0;

	id_entry = drm_find_description(PCI_VENDOR(pa->pa_id),
	    PCI_PRODUCT(pa->pa_id), idlist);
	if (id_entry != NULL) {
		return 1;
	}

	return 0;
}

void drm_attach(struct device *kdev, struct pci_attach_args *pa,
                drm_pci_id_list_t *idlist)
{
	int unit;
	drm_device_t *dev;
	drm_pci_id_list_t *id_entry;

        if(init_units)
	{
		for(unit=0; unit<DRM_MAXUNITS;unit++)
			drm_units[unit] = NULL;
		init_units = 0;
	}

        for(unit=0; unit<DRM_MAXUNITS; unit++)
		if(drm_units[unit] == NULL) break;
	/* This should not happen, since drm_probe made sure there was room */
	if(unit == DRM_MAXUNITS) return;

	dev = drm_units[unit] = (drm_device_t*)kdev;
	dev->unit = unit;

	id_entry = drm_find_description(PCI_VENDOR(pa->pa_id),
	    PCI_PRODUCT(pa->pa_id), idlist);

	dev->id_entry = id_entry;
	dev->driver.id_entry = id_entry;

	dev->unique = NULL;
	dev->unique_len = 0;
	dev->if_version = 0;
	dev->flags = 0;
	dev->open_count = 0;
	dev->buf_use = 0;
	dev->counters = 0;
        /* dev->files : drm_load */
        /* dev->magiclist : drm_firstopen */
        /* dev->maplist : drm_load */
        dev->context_sareas = NULL;
        dev->max_context = 0;
	DRM_SPININIT(dev->dev_lock, "drm device");
        dev->dma = NULL;
	/* dev->irq : drm_load */
	dev->irq_enabled = 0;
	dev->pa = *pa;
	dev->irqh = NULL;
	for(unit=0; unit<DRM_MAX_PCI_RESOURCE; unit++)
	{
		dev->pci_map_data[unit].mapped = 0;
		dev->pci_map_data[unit].maptype =
			pci_mapreg_type(dev->pa.pa_pc, dev->pa.pa_tag,
				PCI_MAPREG_START + unit*4);
		if(pci_mapreg_info(dev->pa.pa_pc, dev->pa.pa_tag,
				PCI_MAPREG_START + unit*4,
				dev->pci_map_data[unit].maptype,
				&(dev->pci_map_data[unit].base),
				&(dev->pci_map_data[unit].size),
				&(dev->pci_map_data[unit].flags)))
		{
			dev->pci_map_data[unit].base = 0;
			dev->pci_map_data[unit].size = 0;
		}
		if(dev->pci_map_data[unit].maptype == PCI_MAPREG_TYPE_MEM)
			dev->pci_map_data[unit].flags |= BUS_SPACE_MAP_LINEAR;
	}
	dev->context_flag = 0;
	dev->last_context = 0;
	dev->vbl_queue = 0;
	dev->vbl_received = 0;
	dev->buf_pgid = 0;
	dev->sysctl = NULL;
	dev->agp = NULL;
	dev->sg = NULL;
	dev->ctx_bitmap = NULL;
	dev->dev_private = NULL;
	dev->agp_buffer_token = 0;
	dev->agp_buffer_map = 0;
	/* dev->unit - already done */

	DRM_INFO("%s (unit %d)\n", id_entry->name, dev->unit);
	drm_load(dev);
}

int drm_detach(struct device *self, int flags)
{
	drm_device_t *dev = (drm_device_t*)self;
	drm_unload(dev);
	drm_units[dev->unit] = NULL;
	return 0;
}

int drm_activate(struct device *self, enum devact act)
{
	switch (act) {
	case DVACT_ACTIVATE:
		return (EOPNOTSUPP);
		break;

	case DVACT_DEACTIVATE:
		/* FIXME */
		break;
	}
	return (0);
}

#endif /* __NetBSD__ || __OpenBSD__ */

drm_pci_id_list_t *drm_find_description(int vendor, int device,
    drm_pci_id_list_t *idlist)
{
	int i = 0;
	
	for (i = 0; idlist[i].vendor != 0; i++) {
		if ((idlist[i].vendor == vendor) &&
		    (idlist[i].device == device)) {
			return &idlist[i];
		}
	}
	return NULL;
}

static int drm_firstopen(drm_device_t *dev)
{
	drm_local_map_t *map;
	int i;

	DRM_SPINLOCK_ASSERT(&dev->dev_lock);

	/* prebuild the SAREA */
	i = drm_addmap(dev, 0, SAREA_MAX, _DRM_SHM,
		       _DRM_CONTAINS_LOCK, &map);
	if (i != 0)
		return i;

	if (dev->driver.firstopen)
		dev->driver.firstopen(dev);

	dev->buf_use = 0;

	if (dev->driver.use_dma) {
		i = drm_dma_setup(dev);
		if (i != 0)
			return i;
	}

	dev->counters  = 6;
	dev->types[0]  = _DRM_STAT_LOCK;
	dev->types[1]  = _DRM_STAT_OPENS;
	dev->types[2]  = _DRM_STAT_CLOSES;
	dev->types[3]  = _DRM_STAT_IOCTLS;
	dev->types[4]  = _DRM_STAT_LOCKS;
	dev->types[5]  = _DRM_STAT_UNLOCKS;

	for ( i = 0 ; i < DRM_ARRAY_SIZE(dev->counts) ; i++ )
		atomic_set( &dev->counts[i], 0 );

	for ( i = 0 ; i < DRM_HASH_SIZE ; i++ ) {
		dev->magiclist[i].head = NULL;
		dev->magiclist[i].tail = NULL;
	}

	dev->lock.lock_queue = 0;
	dev->irq_enabled = 0;
	dev->context_flag = 0;
	dev->last_context = 0;
	dev->if_version = 0;

#ifdef __FreeBSD__
	dev->buf_sigio = NULL;
#elif defined(__NetBSD__) || defined(__OpenBSD__)
	dev->buf_pgid = 0;
#endif

	DRM_DEBUG( "\n" );

	return 0;
}

static int drm_lastclose(drm_device_t *dev)
{
	drm_magic_entry_t *pt, *next;
	drm_local_map_t *map;
#ifdef __FreeBSD__
	drm_local_map_t *mapsave;
#endif
#ifdef __NetBSD__
	struct drm_file *filep;
#endif
	int i;

	DRM_SPINLOCK_ASSERT(&dev->dev_lock);

	DRM_DEBUG( "\n" );

	if (dev->driver.lastclose != NULL)
		dev->driver.lastclose(dev);

	if (dev->irq_enabled)
		drm_irq_uninstall(dev);

	if ( dev->unique ) {
		free(dev->unique, M_DRM);
		dev->unique = NULL;
		dev->unique_len = 0;
	}
				/* Clear pid list */
	for ( i = 0 ; i < DRM_HASH_SIZE ; i++ ) {
		for ( pt = dev->magiclist[i].head ; pt ; pt = next ) {
			next = pt->next;
			free(pt, M_DRM);
		}
		dev->magiclist[i].head = dev->magiclist[i].tail = NULL;
	}

				/* Clear AGP information */
	if ( dev->agp ) {
		drm_agp_mem_t *entry;
		drm_agp_mem_t *nexte;

		/* Remove AGP resources, but leave dev->agp intact until
		 * drm_unload is called.
		 */
		for ( entry = dev->agp->memory ; entry ; entry = nexte ) {
			nexte = entry->next;
			if ( entry->bound )
				drm_agp_unbind_memory(entry->handle);
			drm_agp_free_memory(entry->handle);
			free(entry, M_DRM);
		}
		dev->agp->memory = NULL;

		if (dev->agp->acquired)
			drm_agp_release(dev);

		dev->agp->acquired = 0;
		dev->agp->enabled  = 0;
	}
	if (dev->sg != NULL) {
		drm_sg_cleanup(dev->sg);
		dev->sg = NULL;
	}

#ifdef __FreeBSD__
	TAILQ_FOREACH_SAFE(map, &dev->maplist, link, mapsave) {
		drm_rmmap(dev, map);
	}
#elif defined(__NetBSD__)
	while ((map = TAILQ_FIRST(&dev->maplist)) != NULL) {
		drm_rmmap(dev, map);
	}

	for(i = 0; i<DRM_MAX_PCI_RESOURCE; i++) {
		if (dev->pci_map_data[i].mapped > 1) {
			bus_space_unmap(dev->pci_map_data[i].maptype,
					dev->pci_map_data[i].bsh,
					dev->pci_map_data[i].size);
			dev->pci_map_data[i].mapped = 0;
			DRM_DEBUG("WARNING: had to unmap resource %d\n", i);
		}
	}
#endif

	drm_dma_takedown(dev);
	DRM_DEBUG( "\n" );
	if ( dev->lock.hw_lock ) {
		dev->lock.hw_lock = NULL; /* SHM removed */
		dev->lock.filp = NULL;
		DRM_WAKEUP_INT((void *)&dev->lock.lock_queue);
	}

#ifdef __NetBSD__
	while ((filep = TAILQ_FIRST(&dev->files)) != NULL) {
		DRM_INFO("had to remove pid %d still in file list\n",
		         (int) filep->pid);
		TAILQ_REMOVE(&dev->files, filep, link);
		free(filep, M_DRM);
	}
#endif

	return 0;
}

static int drm_load(drm_device_t *dev)
{
	int retcode;

	DRM_DEBUG( "\n" );

#ifdef __FreeBSD__
	dev->irq = pci_get_irq(dev->device);
	/* XXX Fix domain number (alpha hoses) */
	dev->pci_domain = 0;
	dev->pci_bus = pci_get_bus(dev->device);
	dev->pci_slot = pci_get_slot(dev->device);
	dev->pci_func = pci_get_function(dev->device);

	dev->pci_vendor = pci_get_vendor(dev->device);
	dev->pci_device = pci_get_device(dev->device);
#elif defined(__NetBSD__)
	dev->irq = dev->pa.pa_intrline;
	dev->pci_domain = 0;
	dev->pci_bus = dev->pa.pa_bus;
	dev->pci_slot = dev->pa.pa_device;
	dev->pci_func = dev->pa.pa_function;

	dev->pci_vendor = PCI_VENDOR(dev->pa.pa_id);
	dev->pci_device = PCI_PRODUCT(dev->pa.pa_id);
#endif

	TAILQ_INIT(&dev->maplist);

	drm_mem_init();
	drm_sysctl_init(dev);
	TAILQ_INIT(&dev->files);

	if (dev->driver.load != NULL) {
		DRM_LOCK();
		retcode = dev->driver.load(dev, dev->id_entry->driver_private);
		DRM_UNLOCK();
		if (retcode != 0)
			goto error;
	}

	if (dev->driver.use_agp) {
		if (drm_device_is_agp(dev))
			dev->agp = drm_agp_init();
		if (dev->driver.require_agp && dev->agp == NULL) {
			DRM_ERROR("Card isn't AGP, or couldn't initialize "
			    "AGP.\n");
			retcode = DRM_ERR(ENOMEM);
			goto error;
		}
		if (dev->agp != NULL) {
			if (drm_mtrr_add(dev->agp->info.ai_aperture_base,
			    dev->agp->info.ai_aperture_size, DRM_MTRR_WC) == 0)
				dev->agp->mtrr = 1;
		}
	}

	retcode = drm_ctxbitmap_init(dev);
	if (retcode != 0) {
		DRM_ERROR("Cannot allocate memory for context bitmap.\n");
		goto error;
	}
	
	DRM_INFO("Initialized %s %d.%d.%d %s\n",
	  	dev->driver.name,
	  	dev->driver.major,
	  	dev->driver.minor,
	  	dev->driver.patchlevel,
	  	dev->driver.date);

	return 0;

error:
	drm_sysctl_cleanup(dev);
	DRM_LOCK();
	drm_lastclose(dev);
	DRM_UNLOCK();
#ifdef __FreeBSD__
	destroy_dev(dev->devnode);
#if __FreeBSD_version >= 500000
	mtx_destroy(&dev->dev_lock);
#endif
#endif
#ifdef __NetBSD__
	DRM_SPINUNINIT(&dev->dev_lock);
#endif
	return retcode;
}

static void drm_unload(drm_device_t *dev)
{
	int i;

	DRM_DEBUG( "\n" );

	drm_sysctl_cleanup(dev);
#ifdef __FreeBSD__
	destroy_dev(dev->devnode);
#endif

	drm_ctxbitmap_cleanup(dev);

	if (dev->agp && dev->agp->mtrr) {
		int __unused retcode;

		retcode = drm_mtrr_del(0, dev->agp->info.ai_aperture_base,
		    dev->agp->info.ai_aperture_size, DRM_MTRR_WC);
		DRM_DEBUG("mtrr_del = %d", retcode);
	}

	DRM_LOCK();
	drm_lastclose(dev);
	DRM_UNLOCK();

	/* Clean up PCI resources allocated by drm_bufs.c.  We're not really
	 * worried about resource consumption while the DRM is inactive (between
	 * lastclose and firstopen or unload) because these aren't actually
	 * taking up KVA, just keeping the PCI resource allocated.
	 */
#ifdef __FreeBSD__
	for (i = 0; i < DRM_MAX_PCI_RESOURCE; i++) {
		if (dev->pcir[i] == NULL)
			continue;
		bus_release_resource(dev->device, SYS_RES_MEMORY,
		    dev->pcirid[i], dev->pcir[i]);
		dev->pcir[i] = NULL;
	}
#elif defined(__NetBSD__)
	i = 0;

	for (i = 0; i < DRM_MAX_PCI_RESOURCE; i++)
		if (dev->pci_map_data[i].mapped != 0)
		{
			bus_space_unmap(dev->pa.pa_memt,
					dev->pci_map_data[i].bsh,
					dev->pci_map_data[i].size);
			dev->pci_map_data[i].mapped = 0;
		}
#endif

	if ( dev->agp ) {
		free(dev->agp, M_DRM);
		dev->agp = NULL;
	}

	if (dev->driver.unload != NULL)
		dev->driver.unload(dev);

	drm_mem_uninit();
#if defined(__FreeBSD__) &&  __FreeBSD_version >= 500000
	mtx_destroy(&dev->dev_lock);
#endif
#ifdef __NetBSD__
	DRM_SPINUNINIT(&dev->dev_lock);
#endif
}


int drm_version(DRM_IOCTL_ARGS)
{
	DRM_DEVICE;
	drm_version_t drmversion;
	int len;

	DRM_COPY_FROM_USER_IOCTL( drmversion, (drm_version_t *)data, sizeof(drmversion) );

#define DRM_COPY( name, value )						\
	len = strlen( value );						\
	if ( len > name##_len ) len = name##_len;			\
	name##_len = strlen( value );					\
	if ( len && name ) {						\
		if ( DRM_COPY_TO_USER( name, value, len ) )		\
			return DRM_ERR(EFAULT);				\
	}

	drmversion.version_major	= dev->driver.major;
	drmversion.version_minor	= dev->driver.minor;
	drmversion.version_patchlevel	= dev->driver.patchlevel;

	DRM_COPY(drmversion.name, dev->driver.name);
	DRM_COPY(drmversion.date, dev->driver.date);
	DRM_COPY(drmversion.desc, dev->driver.desc);

	DRM_COPY_TO_USER_IOCTL( (drm_version_t *)data, drmversion, sizeof(drmversion) );

	return 0;
}

int drm_open(DRM_CDEV kdev, int flags, int fmt, DRM_STRUCTCDEVPROC *p)
{
	drm_device_t *dev = NULL;
	int retcode = 0;

	dev = DRIVER_SOFTC(minor(kdev));

	DRM_DEBUG( "open_count = %d\n", dev->open_count );

#ifdef __FreeBSD__
	retcode = drm_open_helper(kdev, flags, fmt, p, dev);
#elif defined(__NetBSD__)
	retcode = drm_open_helper(kdev, flags, fmt, p->l_proc, dev);
#endif

	if ( !retcode ) {
		atomic_inc( &dev->counts[_DRM_STAT_OPENS] );
		DRM_LOCK();
#ifdef __FreeBSD__
		device_busy(dev->device);
#endif
		if ( !dev->open_count++ )
			retcode = drm_firstopen(dev);
		DRM_UNLOCK();
	}

	return retcode;
}

int drm_close(DRM_CDEV kdev, int flags, int fmt, DRM_STRUCTCDEVPROC *p)
{
	drm_file_t *priv;
	DRM_DEVICE;
	int retcode = 0;
	DRMFILE filp = (void *)(uintptr_t)(DRM_CURRENTPID);
	
	DRM_DEBUG( "open_count = %d\n", dev->open_count );

	DRM_LOCK();

#ifdef __FreeBSD__
	priv = drm_find_file_by_proc(dev, p);
#elif defined(__NetBSD__)
	priv = drm_find_file_by_proc(dev, p->l_proc);
#endif
	if (!priv) {
		DRM_UNLOCK();
		DRM_ERROR("can't find authenticator\n");
		return EINVAL;
	}

	if (dev->driver.preclose != NULL)
		dev->driver.preclose(dev, filp);

	/* ========================================================
	 * Begin inline drm_release
	 */

#ifdef __FreeBSD__
	DRM_DEBUG( "pid = %d, device = 0x%lx, open_count = %d\n",
		   DRM_CURRENTPID, (long)dev->device, dev->open_count );
#elif defined(__NetBSD__) || defined(__OpenBSD__)
	DRM_DEBUG( "pid = %d, device = 0x%lx, open_count = %d\n",
		   DRM_CURRENTPID, (long)&dev->device, dev->open_count);
#endif

	if (dev->lock.hw_lock && _DRM_LOCK_IS_HELD(dev->lock.hw_lock->lock)
	    && dev->lock.filp == filp) {
		DRM_DEBUG("Process %d dead, freeing lock for context %d\n",
			  DRM_CURRENTPID,
			  _DRM_LOCKING_CONTEXT(dev->lock.hw_lock->lock));
		if (dev->driver.reclaim_buffers_locked != NULL)
			dev->driver.reclaim_buffers_locked(dev, filp);

		drm_lock_free(dev, &dev->lock.hw_lock->lock,
		    _DRM_LOCKING_CONTEXT(dev->lock.hw_lock->lock));
		
				/* FIXME: may require heavy-handed reset of
                                   hardware at this point, possibly
                                   processed via a callback to the X
                                   server. */
	} else if (dev->driver.reclaim_buffers_locked != NULL &&
	    dev->lock.hw_lock != NULL) {
		/* The lock is required to reclaim buffers */
		for (;;) {
			if ( !dev->lock.hw_lock ) {
				/* Device has been unregistered */
				retcode = DRM_ERR(EINTR);
				break;
			}
			if (drm_lock_take(&dev->lock.hw_lock->lock,
			    DRM_KERNEL_CONTEXT)) {
				dev->lock.filp = filp;
				dev->lock.lock_time = jiffies;
                                atomic_inc( &dev->counts[_DRM_STAT_LOCKS] );
				break;	/* Got lock */
			}
				/* Contention */
#if defined(__FreeBSD__) && __FreeBSD_version > 500000
			retcode = msleep((void *)&dev->lock.lock_queue,
			    &dev->dev_lock, PZERO | PCATCH, "drmlk2", 0);
#else
			retcode = ltsleep((void *)&dev->lock.lock_queue,
			    PZERO | PCATCH, "drmlk2", 0, &dev->dev_lock);
#endif
			if (retcode)
				break;
		}
		if (retcode == 0) {
			dev->driver.reclaim_buffers_locked(dev, filp);
			drm_lock_free(dev, &dev->lock.hw_lock->lock,
			    DRM_KERNEL_CONTEXT);
		}
	}

	if (dev->driver.use_dma && !dev->driver.reclaim_buffers_locked)
		drm_reclaim_buffers(dev, filp);

#if defined (__FreeBSD__) && (__FreeBSD_version >= 500000)
	funsetown(&dev->buf_sigio);
#elif defined(__FreeBSD__)
	funsetown(dev->buf_sigio);
#elif defined(__NetBSD__) || defined(__OpenBSD__)
	dev->buf_pgid = 0;
#endif /* __NetBSD__  || __OpenBSD__ */

#ifdef __NetBSD__
	/* On NetBSD, close will only be called once */
	DRM_DEBUG("setting priv->refs %d to 1\n", (int)priv->refs);
	priv->refs = 1;
#endif
	if (--priv->refs == 0) {
		if (dev->driver.postclose != NULL)
			dev->driver.postclose(dev, priv);
		TAILQ_REMOVE(&dev->files, priv, link);
		free(priv, M_DRM);
	}

	/* ========================================================
	 * End inline drm_release
	 */

	atomic_inc( &dev->counts[_DRM_STAT_CLOSES] );
#ifdef __FreeBSD__
	device_unbusy(dev->device);
#endif
#ifdef __NetBSD__
	/* On NetBSD, close will only be called once */
	DRM_DEBUG("setting open_count %d to 1\n", (int)dev->open_count);
	dev->open_count = 1;
#endif
	if (--dev->open_count == 0) {
		retcode = drm_lastclose(dev);
	}

	DRM_UNLOCK();
	
	return retcode;
}

/* drm_ioctl is called whenever a process performs an ioctl on /dev/drm.
 */
int drm_ioctl(DRM_CDEV kdev, u_long cmd, void *data, int flags, 
    DRM_STRUCTCDEVPROC *p)
{
	DRM_DEVICE;
	int retcode = 0;
	drm_ioctl_desc_t *ioctl;
	int (*func)(DRM_IOCTL_ARGS);
	int nr = DRM_IOCTL_NR(cmd);
	int is_driver_ioctl = 0;
	drm_file_t *priv;
	DRMFILE filp = (DRMFILE)(uintptr_t)DRM_CURRENTPID;

	DRM_LOCK();
#ifdef __FreeBSD__
	priv = drm_find_file_by_proc(dev, p);
#elif defined(__NetBSD__)
	priv = drm_find_file_by_proc(dev, p->l_proc);
#endif
	DRM_UNLOCK();
	if (priv == NULL) {
		DRM_ERROR("can't find authenticator\n");
		return EINVAL;
	}

	atomic_inc( &dev->counts[_DRM_STAT_IOCTLS] );
	++priv->ioctl_count;

#ifdef __FreeBSD__
	DRM_DEBUG( "pid=%d, cmd=0x%02lx, nr=0x%02x, dev 0x%lx, auth=%d\n",
		 DRM_CURRENTPID, cmd, nr, (long)dev->device, priv->authenticated );
#elif defined(__NetBSD__) || defined(__OpenBSD__)
	DRM_DEBUG( "pid=%d, cmd=0x%02lx, nr=0x%02x, dev 0x%lx, auth=%d\n",
		 DRM_CURRENTPID, cmd, nr, (long)&dev->device, priv->authenticated );
#endif

	switch (cmd) {
	case FIONBIO:
	case FIOASYNC:
		return 0;

#ifdef __FreeBSD__
	case FIOSETOWN:
		return fsetown(*(int *)data, &dev->buf_sigio);

	case FIOGETOWN:
#if (__FreeBSD_version >= 500000)
		*(int *) data = fgetown(&dev->buf_sigio);
#else
		*(int *) data = fgetown(dev->buf_sigio);
#endif
		return 0;
#endif /* __FreeBSD__ */
#if defined(__NetBSD__) || defined(__OpenBSD__)
	case SIOCSPGRP:
	case TIOCSPGRP:
	case FIOSETOWN:
		return fsetown(p->l_proc, &dev->buf_pgid, cmd, data);

	case SIOCGPGRP:
	case TIOCGPGRP:
	case FIOGETOWN:
		return fgetown(p->l_proc, dev->buf_pgid, cmd, data);
#endif /* __NetBSD__ */
	}

	if (IOCGROUP(cmd) != DRM_IOCTL_BASE) {
		DRM_DEBUG("Bad ioctl group 0x%x\n", (int)IOCGROUP(cmd));
		return EINVAL;
	}

	ioctl = &drm_ioctls[nr];
	/* It's not a core DRM ioctl, try driver-specific. */
	if (ioctl->func == NULL && nr >= DRM_COMMAND_BASE) {
		/* The array entries begin at DRM_COMMAND_BASE ioctl nr */
		nr -= DRM_COMMAND_BASE;
		if (nr > dev->driver.max_ioctl) {
			DRM_DEBUG("Bad driver ioctl number, 0x%x (of 0x%x)\n",
			    nr, dev->driver.max_ioctl);
			return EINVAL;
		}
		ioctl = &dev->driver.ioctls[nr];
		is_driver_ioctl = 1;
	}
	func = ioctl->func;

	if (func == NULL) {
		DRM_DEBUG( "no function\n" );
		return EINVAL;
	}
	/* ioctl->master check should be against something in the filp set up
	 * for the first opener, but it doesn't matter yet.
	 */
#ifdef __FreeBSD__
	if (((ioctl->flags & DRM_ROOT_ONLY) && !DRM_SUSER(p)) ||
#elif defined(__NetBSD__)
	if (((ioctl->flags & DRM_ROOT_ONLY) && !DRM_SUSER(p->l_proc)) ||
#endif
	    ((ioctl->flags & DRM_AUTH) && !priv->authenticated) ||
	    ((ioctl->flags & DRM_MASTER) && !priv->master))
		return EACCES;

	if (is_driver_ioctl)
		DRM_LOCK();
	retcode = func(kdev, cmd, data, flags, p, filp);
	if (is_driver_ioctl)
		DRM_UNLOCK();

	if (retcode != 0)
		DRM_DEBUG("    returning %d\n", retcode);

	return DRM_ERR(retcode);
}


#if DRM_LINUX

#include <sys/sysproto.h>

MODULE_DEPEND(DRIVER_NAME, linux, 1, 1, 1);

#define LINUX_IOCTL_DRM_MIN		0x6400
#define LINUX_IOCTL_DRM_MAX		0x64ff

static linux_ioctl_function_t drm_linux_ioctl;
static struct linux_ioctl_handler drm_handler = {drm_linux_ioctl, 
    LINUX_IOCTL_DRM_MIN, LINUX_IOCTL_DRM_MAX};

SYSINIT(drm_register, SI_SUB_KLD, SI_ORDER_MIDDLE, 
    linux_ioctl_register_handler, &drm_handler);
SYSUNINIT(drm_unregister, SI_SUB_KLD, SI_ORDER_MIDDLE, 
    linux_ioctl_unregister_handler, &drm_handler);

/* The bits for in/out are switched on Linux */
#define LINUX_IOC_IN	IOC_OUT
#define LINUX_IOC_OUT	IOC_IN

static int
drm_linux_ioctl(DRM_STRUCTPROC *p, struct linux_ioctl_args* args)
{
	int error;
	int cmd = args->cmd;

	args->cmd &= ~(LINUX_IOC_IN | LINUX_IOC_OUT);
	if (cmd & LINUX_IOC_IN)
		args->cmd |= IOC_IN;
	if (cmd & LINUX_IOC_OUT)
		args->cmd |= IOC_OUT;
	
	error = ioctl(p, (struct ioctl_args *)args);

	return error;
}
#endif /* DRM_LINUX */
