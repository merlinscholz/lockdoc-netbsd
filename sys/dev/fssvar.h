/*	$NetBSD: fssvar.h,v 1.1 2003/12/10 11:40:11 hannken Exp $	*/

/*-
 * Copyright (c) 2003 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Juergen Hannken-Illjes.
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

#ifndef _SYS_DEV_FSSVAR_H
#define _SYS_DEV_FSSVAR_H

struct fss_set {
	char		*fss_mount;	/* Mount point of file system */
	char		*fss_bstore;	/* Path of backing store */
	blksize_t	fss_csize;	/* Preferred cluster size */
};

struct fss_get {
	char		fsg_mount[MNAMELEN]; /* Mount point of file system */
	struct timeval	fsg_time;	/* Time this snapshot was taken */
	blksize_t	fsg_csize;	/* Current cluster size */
	blkcnt_t	fsg_mount_size;	/* # clusters on file system */
	blkcnt_t	fsg_bs_size;	/* # clusters on backing store */
};

#define FSSIOCSET	_IOW('F', 0, struct fss_set)	/* Configure */
#define FSSIOCGET	_IOR('F', 1, struct fss_get)	/* Status */
#define FSSIOCCLR	_IO('F', 2)			/* Unconfigure */

#ifdef _KERNEL

#define FSS_CLUSTER_MAX	(1<<24)		/* Upper bound of clusters. The
					   sc_copied map uses
					   FSS_CLUSTER_MAX/NBBY bytes */
typedef enum {
	FSS_CACHE_FREE	= 0,		/* Cache entry is free */
	FSS_CACHE_BUSY	= 1,		/* Cache entry is read from device */
	FSS_CACHE_VALID	= 2		/* Cache entry contains valid data */
} fss_cache_type;

struct fss_cache {
	fss_cache_type	fc_type;	/* Current state */
	struct fss_softc *fc_softc;	/* Backlink to our softc */
	volatile int	fc_xfercount;	/* Number of outstanding transfers */
	u_int32_t	fc_cluster;	/* Cluster number of this entry */
	caddr_t		fc_data;	/* Data */
};

struct fss_softc {
	int		sc_unit;	/* Logical unit number */
	struct lock	sc_lock;	/* Get exclusive access to device */
	volatile int	sc_flags;	/* Flags */
#define FSS_ACTIVE	0x01		/* Snapshot is active */
#define FSS_ERROR	0x02		/* I/o error occured */
#define FSS_BS_THREAD	0x04		/* Kernel thread is running */
	struct mount	*sc_mount;	/* Mount point */
	char		sc_mntname[MNAMELEN]; /* Mount point */
	struct timeval	sc_time;	/* Time this snapshot was taken */
	dev_t		sc_bdev;	/* Underlying block device */
	void		(*sc_strategy)(struct buf *); /* And its strategy */
	struct vnode	*sc_bs_vp;	/* Our backing store */
	int		sc_bs_bsize;	/* Its bsize */
	struct proc	*sc_bs_proc;	/* Our kernel thread */
	u_int32_t	sc_clsize;	/* Size of cluster */
	u_int32_t	sc_clcount;	/* # clusters in file system */
	u_int8_t	*sc_copied;	/* Map of clusters already copied */
	long		sc_cllast;	/* Bytes in last cluster */
	int		sc_cowcount;	/* Number of cow in progress */
	int		sc_cache_size;	/* Number of entries in sc_cache */
	struct fss_cache *sc_cache;	/* Cluster cache */
	struct bufq_state sc_bufq;	/* Transfer queue */
	u_int32_t	sc_clnext;	/* Next free cluster on backing store */
	int		sc_indir_size;	/* # clusters for indirect mapping */
	u_int8_t	*sc_indir_valid; /* Map of valid indirect clusters */
	u_int32_t	sc_indir_cur;	/* Current indir cluster number */
	int		sc_indir_dirty;	/* Current indir cluster modified */
	u_int32_t	*sc_indir_data;	/* Current indir cluster data */
};

struct fss_softc fss_softc[NFSS];

void fss_copy_on_write(struct fss_softc *, struct buf *);
int fss_umount_hook(struct mount *, int);

/*
 * Get this dev's softc if it is a valid device.
 * Return a pointer to its softc.
 */
static inline int
fss_dev_to_softc(dev_t dev, struct fss_softc **scp)
{
	int unit;
	struct fss_softc *sc;

	unit = minor(dev);
	if (unit < 0 || unit >= NFSS)
		return ENODEV;
	sc = &fss_softc[unit];
	*scp = sc;

	return 0;
}

/*
 * Check if this buf needs to be copied on write.
 * Unroll the loop for small NFSS.
 */
static inline void
fss_cow_hook(struct buf *bp)
{
	int i;

	if (bp->b_flags & B_READ)
		return;

	if (NFSS <= 4) {
		if (__predict_false(fss_softc[0].sc_bdev == bp->b_dev ||
		    (NFSS > 1 && fss_softc[1].sc_bdev == bp->b_dev) ||
		    (NFSS > 2 && fss_softc[2].sc_bdev == bp->b_dev) ||
		    (NFSS > 3 && fss_softc[3].sc_bdev == bp->b_dev)))
			for (i = 0; i < NFSS; i++)
				if (fss_softc[i].sc_bdev == bp->b_dev)
					fss_copy_on_write(&fss_softc[i], bp);
	} else {
		for (i = 0; i < NFSS; i++)
			if (__predict_false(fss_softc[i].sc_bdev == bp->b_dev))
				fss_copy_on_write(&fss_softc[i], bp);
	}
}

#endif /* _KERNEL */

#endif /* !_SYS_DEV_FSSVAR_H */
