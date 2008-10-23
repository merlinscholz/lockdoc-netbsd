/*	$NetBSD: ffs_snapshot.c,v 1.81 2008/10/23 14:25:21 hannken Exp $	*/

/*
 * Copyright 2000 Marshall Kirk McKusick. All Rights Reserved.
 *
 * Further information about snapshots can be obtained from:
 *
 *	Marshall Kirk McKusick		http://www.mckusick.com/softdep/
 *	1614 Oxford Street		mckusick@mckusick.com
 *	Berkeley, CA 94709-1608		+1-510-843-9542
 *	USA
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY MARSHALL KIRK MCKUSICK ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL MARSHALL KIRK MCKUSICK BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ffs_snapshot.c	8.11 (McKusick) 7/23/00
 *
 *	from FreeBSD: ffs_snapshot.c,v 1.79 2004/02/13 02:02:06 kuriyama Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ffs_snapshot.c,v 1.81 2008/10/23 14:25:21 hannken Exp $");

#if defined(_KERNEL_OPT)
#include "opt_ffs.h"
#endif

#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/systm.h>
#include <sys/conf.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/namei.h>
#include <sys/sched.h>
#include <sys/stat.h>
#include <sys/malloc.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/resourcevar.h>
#include <sys/vnode.h>
#include <sys/kauth.h>
#include <sys/fstrans.h>
#include <sys/wapbl.h>

#include <miscfs/specfs/specdev.h>

#include <ufs/ufs/quota.h>
#include <ufs/ufs/ufsmount.h>
#include <ufs/ufs/inode.h>
#include <ufs/ufs/ufs_extern.h>
#include <ufs/ufs/ufs_bswap.h>
#include <ufs/ufs/ufs_wapbl.h>

#include <ufs/ffs/fs.h>
#include <ufs/ffs/ffs_extern.h>

#include <uvm/uvm.h>

#if !defined(FFS_NO_SNAPSHOT)
typedef int (*acctfunc_t)
    (struct vnode *, void *, int, int, struct fs *, daddr_t, int);

static int snapshot_setup(struct mount *, struct vnode *);
static int snapshot_copyfs(struct mount *, struct vnode *, void **);
static int snapshot_expunge(struct mount *, struct vnode *,
    struct fs *, daddr_t *, daddr_t **);
static int snapshot_expunge_snap(struct mount *, struct vnode *,
    struct fs *, daddr_t);
static int snapshot_writefs(struct mount *, struct vnode *, void *);
static int cgaccount(struct vnode *, int, int *);
static int cgaccount1(int, struct vnode *, void *, int);
static int expunge(struct vnode *, struct inode *, struct fs *,
    acctfunc_t, int);
static int indiracct(struct vnode *, struct vnode *, int, daddr_t,
    daddr_t, daddr_t, daddr_t, daddr_t, struct fs *, acctfunc_t, int);
static int fullacct(struct vnode *, void *, int, int, struct fs *,
    daddr_t, int);
static int snapacct(struct vnode *, void *, int, int, struct fs *,
    daddr_t, int);
static int mapacct(struct vnode *, void *, int, int, struct fs *,
    daddr_t, int);
#endif /* !defined(FFS_NO_SNAPSHOT) */

static int ffs_copyonwrite(void *, struct buf *, bool);
static int snapblkaddr(struct vnode *, daddr_t, daddr_t *);
static int rwfsblk(struct vnode *, int, void *, daddr_t);
static int syncsnap(struct vnode *);
static int wrsnapblk(struct vnode *, void *, daddr_t);

static inline daddr_t db_get(struct inode *, int);
static inline void db_assign(struct inode *, int, daddr_t);
static inline daddr_t ib_get(struct inode *, int);
static inline void ib_assign(struct inode *, int, daddr_t);
static inline daddr_t idb_get(struct inode *, void *, int);
static inline void idb_assign(struct inode *, void *, int, daddr_t);

struct snap_info {
	kmutex_t si_lock;			/* Lock this snapinfo */
	kmutex_t si_snaplock;			/* Snapshot vnode common lock */
	TAILQ_HEAD(inodelst, inode) si_snapshots; /* List of active snapshots */
	daddr_t *si_snapblklist;		/* Snapshot block hints list */
	uint32_t si_gen;			/* Incremented on change */
};

#ifdef DEBUG
static int snapdebug = 0;
#endif

int
ffs_snapshot_init(struct ufsmount *ump)
{
	struct snap_info *si;

	si = ump->um_snapinfo = kmem_alloc(sizeof(*si), KM_SLEEP);
	if (si == NULL)
		return ENOMEM;

	TAILQ_INIT(&si->si_snapshots);
	mutex_init(&si->si_lock, MUTEX_DEFAULT, IPL_NONE);
	mutex_init(&si->si_snaplock, MUTEX_DEFAULT, IPL_NONE);
	si->si_gen = 0;
	si->si_snapblklist = NULL;

	return 0;
}

void
ffs_snapshot_fini(struct ufsmount *ump)
{
	struct snap_info *si;

	si = ump->um_snapinfo;
	ump->um_snapinfo = NULL;

	KASSERT(TAILQ_EMPTY(&si->si_snapshots));
	mutex_destroy(&si->si_lock);
	mutex_destroy(&si->si_snaplock);
	KASSERT(si->si_snapblklist == NULL);
	kmem_free(si, sizeof(*si));
}

/*
 * Create a snapshot file and initialize it for the filesystem.
 * Vnode is locked on entry and return.
 */
int
ffs_snapshot(struct mount *mp, struct vnode *vp, struct timespec *ctime)
{
#if defined(FFS_NO_SNAPSHOT)
	return EOPNOTSUPP;
}
#else /* defined(FFS_NO_SNAPSHOT) */
	bool suspended = false;
	bool snapshot_locked = false;
	int error, redo = 0, snaploc;
	void *sbbuf = NULL;
	daddr_t *snaplist = NULL, snaplistsize = 0;
	struct buf *bp, *nbp;
	struct fs *copy_fs, *fs = VFSTOUFS(mp)->um_fs;
	struct inode *ip = VTOI(vp);
	struct lwp *l = curlwp;
	struct snap_info *si = VFSTOUFS(mp)->um_snapinfo;
	struct timespec ts;
	struct timeval starttime;
#ifdef DEBUG
	struct timeval endtime;
#endif
	struct vnode *devvp = ip->i_devvp;

	/*
	 * If the vnode already is a snapshot, return.
	 */
	if (VTOI(vp)->i_flags & SF_SNAPSHOT) {
		if (ctime) {
			ctime->tv_sec = DIP(VTOI(vp), mtime);
			ctime->tv_nsec = DIP(VTOI(vp), mtimensec);
		}
		return 0;
	}
	/*
	 * Check for free snapshot slot in the superblock.
	 */
	for (snaploc = 0; snaploc < FSMAXSNAP; snaploc++)
		if (fs->fs_snapinum[snaploc] == 0)
			break;
	if (snaploc == FSMAXSNAP)
		return (ENOSPC);
	/*
	 * Prepare the vnode to become a snapshot.
	 */
	error = snapshot_setup(mp, vp);
	if (error)
		goto out;
	/*
	 * Change inode to snapshot type file.
	 */
	ip->i_flags |= SF_SNAPSHOT;
	DIP_ASSIGN(ip, flags, ip->i_flags);
	ip->i_flag |= IN_CHANGE | IN_UPDATE;
	/*
	 * Copy all the cylinder group maps. Although the
	 * filesystem is still active, we hope that only a few
	 * cylinder groups will change between now and when we
	 * suspend operations. Thus, we will be able to quickly
	 * touch up the few cylinder groups that changed during
	 * the suspension period.
	 */
	error = cgaccount(vp, 1, NULL);
	if (error)
		goto out;
	/*
	 * Ensure that the snapshot is completely on disk.
	 * Since we have marked it as a snapshot it is safe to
	 * unlock it as no process will be allowed to write to it.
	 */
	error = VOP_FSYNC(vp, l->l_cred, FSYNC_WAIT, 0, 0);
	if (error)
		goto out;
	VOP_UNLOCK(vp, 0);
	/*
	 * All allocations are done, so we can now suspend the filesystem.
	 */
	error = vfs_suspend(vp->v_mount, 0);
	vn_lock(vp, LK_EXCLUSIVE | LK_RETRY);
	if (error)
		goto out;
	suspended = true;
	getmicrotime(&starttime);
	/*
	 * First, copy all the cylinder group maps that have changed.
	 */
	error = cgaccount(vp, 2, &redo);
	if (error)
		goto out;
	/*
	 * Create a copy of the superblock and its summary information.
	 */
	error = snapshot_copyfs(mp, vp, &sbbuf);
	copy_fs = (struct fs *)((char *)sbbuf + blkoff(fs, fs->fs_sblockloc));
	if (error)
		goto out;
	/*
	 * Expunge unlinked files from our view.
	 */
	error = snapshot_expunge(mp, vp, copy_fs, &snaplistsize, &snaplist);
	if (error)
		goto out;
	/*
	 * Acquire the snapshot lock.
	 */
	mutex_enter(&si->si_snaplock);
	snapshot_locked = true;
	/*
	 * Record snapshot inode. Since this is the newest snapshot,
	 * it must be placed at the end of the list.
	 */
	fs->fs_snapinum[snaploc] = ip->i_number;

	mutex_enter(&si->si_lock);
	if (ip->i_nextsnap.tqe_prev != 0)
		panic("ffs_snapshot: %"PRIu64" already on list", ip->i_number);
	TAILQ_INSERT_TAIL(&si->si_snapshots, ip, i_nextsnap);
	if (TAILQ_FIRST(&si->si_snapshots) == ip) {
		/*
		 * If this is the first snapshot on this filesystem, put the
		 * preliminary list in place and establish the cow handler.
		 */
		si->si_snapblklist = snaplist;
		fscow_establish(mp, ffs_copyonwrite, devvp);
	}
	si->si_gen++;
	mutex_exit(&si->si_lock);

	vp->v_vflag |= VV_SYSTEM;
	/*
	 * Set the mtime to the time the snapshot has been taken.
	 */
	TIMEVAL_TO_TIMESPEC(&starttime, &ts);
	if (ctime)
		*ctime = ts;
	DIP_ASSIGN(ip, mtime, ts.tv_sec);
	DIP_ASSIGN(ip, mtimensec, ts.tv_nsec);
	ip->i_flag |= IN_CHANGE | IN_UPDATE;
	/*
	 * Copy allocation information from all snapshots and then
	 * expunge them from our view.
	 */
	error = snapshot_expunge_snap(mp, vp, copy_fs, snaplistsize);
	if (error)
		goto out;
	/*
	 * Write the superblock and its summary information to the snapshot.
	 */
	error = snapshot_writefs(mp, vp, sbbuf);
	if (error)
		goto out;
	/*
	 * We're nearly done, ensure that the snapshot is completely on disk.
	 */
	error = VOP_FSYNC(vp, l->l_cred, FSYNC_WAIT, 0, 0);
	if (error)
		goto out;
	/*
	 * Invalidate and free all pages on the snapshot vnode.
	 * We will read and write through the buffercache.
	 */
	mutex_enter(&vp->v_interlock);
	error = VOP_PUTPAGES(vp, 0, 0,
		    PGO_ALLPAGES | PGO_CLEANIT | PGO_SYNCIO | PGO_FREE);
	if (error)
		goto out;
	/*
	 * Invalidate short ( < fs_bsize ) buffers.  We will always read
	 * full size buffers later.
	 */
	mutex_enter(&bufcache_lock);
	KASSERT(LIST_FIRST(&vp->v_dirtyblkhd) == NULL);
	for (bp = LIST_FIRST(&vp->v_cleanblkhd); bp; bp = nbp) {
		nbp = LIST_NEXT(bp, b_vnbufs);
		KASSERT((bp->b_cflags & BC_BUSY) == 0);
		if (bp->b_bcount < fs->fs_bsize) {
			bp->b_cflags |= BC_BUSY;
			brelsel(bp, BC_INVAL | BC_VFLUSH);
		}
	}
	mutex_exit(&bufcache_lock);

out:
	if (sbbuf != NULL) {
		free(copy_fs->fs_csp, M_UFSMNT);
		free(sbbuf, M_UFSMNT);
	}
	if (fs->fs_active != NULL) {
		free(fs->fs_active, M_DEVBUF);
		fs->fs_active = NULL;
	}

	mutex_enter(&si->si_lock);
	if (snaplist != NULL) {
		if (si->si_snapblklist == snaplist)
			si->si_snapblklist = NULL;
		free(snaplist, M_UFSMNT);
	}
	if (error) {
		fs->fs_snapinum[snaploc] = 0;
	} else {
		/*
		 * As this is the newest list, it is the most inclusive, so
		 * should replace the previous list.
		 */
		si->si_snapblklist = ip->i_snapblklist;
	}
	si->si_gen++;
	mutex_exit(&si->si_lock);

	if (snapshot_locked)
		mutex_exit(&si->si_snaplock);
	if (suspended) {
		vfs_resume(vp->v_mount);
#ifdef DEBUG
		getmicrotime(&endtime);
		timersub(&endtime, &starttime, &endtime);
		printf("%s: suspended %ld.%03ld sec, redo %d of %d\n",
		    mp->mnt_stat.f_mntonname, (long)endtime.tv_sec,
		    endtime.tv_usec / 1000, redo, fs->fs_ncg);
#endif
	}
	if (error) {
		if (!UFS_WAPBL_BEGIN(mp)) {
			(void) ffs_truncate(vp, (off_t)0, 0, NOCRED);
			UFS_WAPBL_END(mp);
		}
	} else
		vref(vp);
	return (error);
}

/*
 * Prepare vnode to become a snapshot.
 */
static int
snapshot_setup(struct mount *mp, struct vnode *vp)
{
	int error, i, len, loc;
	daddr_t blkno, numblks;
	struct buf *ibp, *nbp;
	struct fs *fs = VFSTOUFS(mp)->um_fs;
	struct lwp *l = curlwp;

	/*
	 * Check mount, exclusive reference and owner.
	 */
	if (vp->v_mount != mp)
		return EXDEV;
	if (vp->v_usecount != 1 || vp->v_writecount != 0)
		return EBUSY;
	if (kauth_authorize_generic(l->l_cred, KAUTH_GENERIC_ISSUSER,
	    NULL) != 0 &&
	    VTOI(vp)->i_uid != kauth_cred_geteuid(l->l_cred))
		return EACCES;

	if (vp->v_size != 0) {
		error = ffs_truncate(vp, 0, 0, NOCRED);
		if (error)
			return error;
	}
	/*
	 * Write an empty list of preallocated blocks to the end of
	 * the snapshot to set size to at least that of the filesystem.
	 */
	numblks = howmany(fs->fs_size, fs->fs_frag);
	blkno = 1;
	blkno = ufs_rw64(blkno, UFS_FSNEEDSWAP(fs));
	error = vn_rdwr(UIO_WRITE, vp,
	    (void *)&blkno, sizeof(blkno), lblktosize(fs, (off_t)numblks),
	    UIO_SYSSPACE, IO_NODELOCKED|IO_UNIT, l->l_cred, NULL, NULL);
	if (error)
		return error;
	/*
	 * Preallocate critical data structures so that we can copy
	 * them in without further allocation after we suspend all
	 * operations on the filesystem. We would like to just release
	 * the allocated buffers without writing them since they will
	 * be filled in below once we are ready to go, but this upsets
	 * the soft update code, so we go ahead and write the new buffers.
	 *
	 * Allocate all indirect blocks and mark all of them as not
	 * needing to be copied.
	 */
	error = UFS_WAPBL_BEGIN(mp);
	if (error)
		return error;
	for (blkno = NDADDR, i = 0; blkno < numblks; blkno += NINDIR(fs)) {
		error = ffs_balloc(vp, lblktosize(fs, (off_t)blkno),
		    fs->fs_bsize, l->l_cred, B_METAONLY, &ibp);
		if (error)
			goto out;
		if (DOINGSOFTDEP(vp))
			bawrite(ibp);
		else
			brelse(ibp, 0);
		if ((++i % 16) == 0) {
			UFS_WAPBL_END(mp);
			error = UFS_WAPBL_BEGIN(mp);
			if (error)
				return error;
		}
	}
	/*
	 * Allocate copies for the superblock and its summary information.
	 */
	error = ffs_balloc(vp, fs->fs_sblockloc, fs->fs_sbsize, l->l_cred,
	    0, &nbp);
	if (error)
		goto out;
	bawrite(nbp);
	blkno = fragstoblks(fs, fs->fs_csaddr);
	len = howmany(fs->fs_cssize, fs->fs_bsize);
	for (loc = 0; loc < len; loc++) {
		error = ffs_balloc(vp, lblktosize(fs, (off_t)(blkno + loc)),
		    fs->fs_bsize, l->l_cred, 0, &nbp);
		if (error)
			goto out;
		bawrite(nbp);
	}

out:
	UFS_WAPBL_END(mp);
	return error;
}

/*
 * Create a copy of the superblock and its summary information.
 * It is up to the caller to free copyfs and copy_fs->fs_csp.
 */
static int
snapshot_copyfs(struct mount *mp, struct vnode *vp, void **sbbuf)
{
	int error, i, len, loc, size;
	void *space;
	int32_t *lp;
	struct buf *bp;
	struct fs *copyfs, *fs = VFSTOUFS(mp)->um_fs;
	struct lwp *l = curlwp;
	struct vnode *devvp = VTOI(vp)->i_devvp;

	/*
	 * Grab a copy of the superblock and its summary information.
	 * We delay writing it until the suspension is released below.
	 */
	*sbbuf = malloc(fs->fs_bsize, M_UFSMNT, M_WAITOK);
	loc = blkoff(fs, fs->fs_sblockloc);
	if (loc > 0)
		memset(*sbbuf, 0, loc);
	copyfs = (struct fs *)((char *)(*sbbuf) + loc);
	bcopy(fs, copyfs, fs->fs_sbsize);
	size = fs->fs_bsize < SBLOCKSIZE ? fs->fs_bsize : SBLOCKSIZE;
	if (fs->fs_sbsize < size)
		memset((char *)(*sbbuf) + loc + fs->fs_sbsize, 0, 
		    size - fs->fs_sbsize);
	size = blkroundup(fs, fs->fs_cssize);
	if (fs->fs_contigsumsize > 0)
		size += fs->fs_ncg * sizeof(int32_t);
	space = malloc(size, M_UFSMNT, M_WAITOK);
	copyfs->fs_csp = space;
	bcopy(fs->fs_csp, copyfs->fs_csp, fs->fs_cssize);
	space = (char *)space + fs->fs_cssize;
	loc = howmany(fs->fs_cssize, fs->fs_fsize);
	i = fs->fs_frag - loc % fs->fs_frag;
	len = (i == fs->fs_frag) ? 0 : i * fs->fs_fsize;
	if (len > 0) {
		if ((error = bread(devvp, fsbtodb(fs, fs->fs_csaddr + loc),
		    len, l->l_cred, 0, &bp)) != 0) {
			brelse(bp, 0);
			free(copyfs->fs_csp, M_UFSMNT);
			free(*sbbuf, M_UFSMNT);
			*sbbuf = NULL;
			return error;
		}
		bcopy(bp->b_data, space, (u_int)len);
		space = (char *)space + len;
		brelse(bp, BC_INVAL | BC_NOCACHE);
	}
	if (fs->fs_contigsumsize > 0) {
		copyfs->fs_maxcluster = lp = space;
		for (i = 0; i < fs->fs_ncg; i++)
			*lp++ = fs->fs_contigsumsize;
	}
	if (mp->mnt_wapbl)
		copyfs->fs_flags &= ~FS_DOWAPBL;
	return 0;
}

/*
 * We must check for active files that have been unlinked (e.g., with a zero
 * link count). We have to expunge all trace of these files from the snapshot
 * so that they are not reclaimed prematurely by fsck or unnecessarily dumped.
 * Note that we skip unlinked snapshot files as they will be handled separately.
 * Calculate the snapshot list size and create a preliminary list.
 */
static int
snapshot_expunge(struct mount *mp, struct vnode *vp, struct fs *copy_fs,
    daddr_t *snaplistsize, daddr_t **snaplist)
{
	bool has_wapbl = false;
	int cg, error, len, loc;
	daddr_t blkno, *blkp;
	struct fs *fs = VFSTOUFS(mp)->um_fs;
	struct inode *xp;
	struct lwp *l = curlwp;
	struct vattr vat;
	struct vnode *logvp = NULL, *mvp = NULL, *xvp;

	*snaplist = NULL;
	/*
	 * Get the log inode if any.
	 */
	if ((fs->fs_flags & FS_DOWAPBL) &&
	    fs->fs_journal_location == UFS_WAPBL_JOURNALLOC_IN_FILESYSTEM) {
		error = VFS_VGET(mp,
		    fs->fs_journallocs[UFS_WAPBL_INFS_INO], &logvp);
		if (error)
			goto out;
	}
	/*
	 * Allocate a marker vnode.
	 */
	if ((mvp = vnalloc(mp)) == NULL) {
		error = ENOMEM;
		goto out;
	}
	/*
	 * We also calculate the needed size for the snapshot list.
	 */
	*snaplistsize = fs->fs_ncg + howmany(fs->fs_cssize, fs->fs_bsize) +
	    FSMAXSNAP + 1 /* superblock */ + 1 /* last block */ + 1 /* size */;
	error = UFS_WAPBL_BEGIN(mp);
	if (error)
		goto out;
	has_wapbl = true;
	mutex_enter(&mntvnode_lock);
	/*
	 * NOTE: not using the TAILQ_FOREACH here since in this loop vgone()
	 * and vclean() can be called indirectly
	 */
	for (xvp = TAILQ_FIRST(&mp->mnt_vnodelist); xvp; xvp = vunmark(mvp)) {
		vmark(mvp, xvp);
		/*
		 * Make sure this vnode wasn't reclaimed in getnewvnode().
		 * Start over if it has (it won't be on the list anymore).
		 */
		if (xvp->v_mount != mp || vismarker(xvp))
			continue;
		mutex_enter(&xvp->v_interlock);
		if ((xvp->v_iflag & VI_XLOCK) ||
		    xvp->v_usecount == 0 || xvp->v_type == VNON ||
		    VTOI(xvp) == NULL ||
		    (VTOI(xvp)->i_flags & SF_SNAPSHOT)) {
			mutex_exit(&xvp->v_interlock);
			continue;
		}
		mutex_exit(&mntvnode_lock);
		/*
		 * XXXAD should increase vnode ref count to prevent it
		 * disappearing or being recycled.
		 */
		mutex_exit(&xvp->v_interlock);
#ifdef DEBUG
		if (snapdebug)
			vprint("ffs_snapshot: busy vnode", xvp);
#endif
		xp = VTOI(xvp);
		if (xvp != logvp) {
			if (VOP_GETATTR(xvp, &vat, l->l_cred) == 0 &&
			    vat.va_nlink > 0) {
				mutex_enter(&mntvnode_lock);
				continue;
			}
			if (ffs_checkfreefile(copy_fs, vp, xp->i_number)) {
				mutex_enter(&mntvnode_lock);
				continue;
			}
		}
		/*
		 * If there is a fragment, clear it here.
		 */
		blkno = 0;
		loc = howmany(xp->i_size, fs->fs_bsize) - 1;
		if (loc < NDADDR) {
			len = fragroundup(fs, blkoff(fs, xp->i_size));
			if (len > 0 && len < fs->fs_bsize) {
				ffs_blkfree(copy_fs, vp, db_get(xp, loc),
				    len, xp->i_number);
				blkno = db_get(xp, loc);
				db_assign(xp, loc, 0);
			}
		}
		*snaplistsize += 1;
		error = expunge(vp, xp, copy_fs, fullacct, BLK_NOCOPY);
		if (blkno)
			db_assign(xp, loc, blkno);
		if (!error)
			error = ffs_freefile(copy_fs, vp, xp->i_number,
			    xp->i_mode);
		if (error) {
			(void)vunmark(mvp);
			goto out;
		}
		mutex_enter(&mntvnode_lock);
	}
	mutex_exit(&mntvnode_lock);
	/*
	 * Create a preliminary list of preallocated snapshot blocks.
	 */
	*snaplist = malloc(*snaplistsize * sizeof(daddr_t), M_UFSMNT, M_WAITOK);
	blkp = &(*snaplist)[1];
	*blkp++ = lblkno(fs, fs->fs_sblockloc);
	blkno = fragstoblks(fs, fs->fs_csaddr);
	for (cg = 0; cg < fs->fs_ncg; cg++) {
		if (fragstoblks(fs, cgtod(fs, cg)) > blkno)
			break;
		*blkp++ = fragstoblks(fs, cgtod(fs, cg));
	}
	len = howmany(fs->fs_cssize, fs->fs_bsize);
	for (loc = 0; loc < len; loc++)
		*blkp++ = blkno + loc;
	for (; cg < fs->fs_ncg; cg++)
		*blkp++ = fragstoblks(fs, cgtod(fs, cg));

out:
	if (has_wapbl)
		UFS_WAPBL_END(mp);
	if (mvp != NULL)
		vnfree(mvp);
	if (logvp != NULL)
		vput(logvp);
	if (error && *snaplist != NULL) {
		free(*snaplist, M_UFSMNT);
		*snaplist = NULL;
	}

	return error;
}

/*
 * Copy allocation information from all the snapshots in this snapshot and
 * then expunge them from its view. Also, collect the list of allocated
 * blocks in i_snapblklist.
 */
static int
snapshot_expunge_snap(struct mount *mp, struct vnode *vp,
    struct fs *copy_fs, daddr_t snaplistsize)
{
	int error, i;
	daddr_t numblks, *snaplist = NULL;
	struct fs *fs = VFSTOUFS(mp)->um_fs;
	struct inode *ip = VTOI(vp), *xp;
	struct lwp *l = curlwp;
	struct snap_info *si = VFSTOUFS(mp)->um_snapinfo;

	error = UFS_WAPBL_BEGIN(mp);
	if (error)
		return error;
	TAILQ_FOREACH(xp, &si->si_snapshots, i_nextsnap) {
		if (xp == ip)
			break;
		error = expunge(vp, xp, fs, snapacct, BLK_SNAP);
		if (error)
			break;
		if (xp->i_ffs_effnlink != 0)
			continue;
		error = ffs_freefile(copy_fs, vp, xp->i_number, xp->i_mode);
		if (error)
			break;
	}
	if (error)
		goto out;
	/*
	 * Allocate space for the full list of preallocated snapshot blocks.
	 */
	snaplist = malloc(snaplistsize * sizeof(daddr_t), M_UFSMNT, M_WAITOK);
	ip->i_snapblklist = &snaplist[1];
	/*
	 * Expunge the blocks used by the snapshots from the set of
	 * blocks marked as used in the snapshot bitmaps. Also, collect
	 * the list of allocated blocks in i_snapblklist.
	 */
	error = expunge(vp, ip, copy_fs, mapacct, BLK_SNAP);
	if (error)
		goto out;
	if (snaplistsize < ip->i_snapblklist - snaplist)
		panic("ffs_snapshot: list too small");
	snaplistsize = ip->i_snapblklist - snaplist;
	snaplist[0] = snaplistsize;
	ip->i_snapblklist = &snaplist[0];
	/*
	 * Write out the list of allocated blocks to the end of the snapshot.
	 */
	numblks = howmany(fs->fs_size, fs->fs_frag);
	for (i = 0; i < snaplistsize; i++)
		snaplist[i] = ufs_rw64(snaplist[i], UFS_FSNEEDSWAP(fs));
	error = vn_rdwr(UIO_WRITE, vp, (void *)snaplist,
	    snaplistsize * sizeof(daddr_t), lblktosize(fs, (off_t)numblks),
	    UIO_SYSSPACE, IO_NODELOCKED | IO_JOURNALLOCKED | IO_UNIT,
	    l->l_cred, NULL, NULL);
	for (i = 0; i < snaplistsize; i++)
		snaplist[i] = ufs_rw64(snaplist[i], UFS_FSNEEDSWAP(fs));
out:
	UFS_WAPBL_END(mp);
	if (error && snaplist != NULL) {
		free(snaplist, M_UFSMNT);
		ip->i_snapblklist = NULL;
	}
	return error;
}

/*
 * Write the superblock and its summary information to the snapshot.
 * Make sure, the first NDADDR blocks get copied to the snapshot.
 */
static int
snapshot_writefs(struct mount *mp, struct vnode *vp, void *sbbuf)
{
	int error, len, loc;
	void *space;
	daddr_t blkno;
	struct buf *bp;
	struct fs *copyfs, *fs = VFSTOUFS(mp)->um_fs;
	struct inode *ip = VTOI(vp);
	struct lwp *l = curlwp;

	copyfs = (struct fs *)((char *)sbbuf + blkoff(fs, fs->fs_sblockloc));

	/*
	 * Write the superblock and its summary information
	 * to the snapshot.
	 */
	blkno = fragstoblks(fs, fs->fs_csaddr);
	len = howmany(fs->fs_cssize, fs->fs_bsize);
	space = copyfs->fs_csp;
#ifdef FFS_EI
	if (UFS_FSNEEDSWAP(fs)) {
		ffs_sb_swap(copyfs, copyfs);
		ffs_csum_swap(space, space, fs->fs_cssize);
	}
#endif
	error = UFS_WAPBL_BEGIN(mp);
	if (error)
		return error;
	for (loc = 0; loc < len; loc++) {
		error = bread(vp, blkno + loc, fs->fs_bsize, l->l_cred,
		    B_MODIFY, &bp);
		if (error) {
			brelse(bp, 0);
			break;
		}
		bcopy(space, bp->b_data, fs->fs_bsize);
		space = (char *)space + fs->fs_bsize;
		bawrite(bp);
	}
	if (error)
		goto out;
	error = bread(vp, lblkno(fs, fs->fs_sblockloc),
	    fs->fs_bsize, l->l_cred, B_MODIFY, &bp);
	if (error) {
		brelse(bp, 0);
		goto out;
	} else {
		bcopy(sbbuf, bp->b_data, fs->fs_bsize);
		bawrite(bp);
	}
	/*
	 * Copy the first NDADDR blocks to the snapshot so ffs_copyonwrite()
	 * and ffs_snapblkfree() will always work on indirect blocks.
	 */
	for (loc = 0; loc < NDADDR; loc++) {
		if (db_get(ip, loc) != 0)
			continue;
		error = ffs_balloc(vp, lblktosize(fs, (off_t)loc),
		    fs->fs_bsize, l->l_cred, 0, &bp);
		if (error)
			break;
		error = rwfsblk(vp, B_READ, bp->b_data, loc);
		if (error) {
			brelse(bp, 0);
			break;
		}
		bawrite(bp);
	}

out:
	UFS_WAPBL_END(mp);
	return error;
}

/*
 * Copy all cylinder group maps.
 */
static int
cgaccount(struct vnode *vp, int passno, int *redo)
{
	int cg, error;
	struct buf *nbp;
	struct fs *fs = VTOI(vp)->i_fs;

	error = UFS_WAPBL_BEGIN(vp->v_mount);
	if (error)
		return error;
	if (redo != NULL)
		*redo = 0;
	if (passno == 1)
		fs->fs_active = malloc(howmany(fs->fs_ncg, NBBY),
		    M_DEVBUF, M_WAITOK | M_ZERO);
	for (cg = 0; cg < fs->fs_ncg; cg++) {
		if (passno == 2 && ACTIVECG_ISSET(fs, cg))
			continue;
		if (redo != NULL)
			*redo += 1;
		error = ffs_balloc(vp, lfragtosize(fs, cgtod(fs, cg)),
		    fs->fs_bsize, curlwp->l_cred, 0, &nbp);
		if (error)
			break;
		error = cgaccount1(cg, vp, nbp->b_data, passno);
		bawrite(nbp);
		if (error)
			break;
	}
	UFS_WAPBL_END(vp->v_mount);
	return error;
}

/*
 * Copy a cylinder group map. All the unallocated blocks are marked
 * BLK_NOCOPY so that the snapshot knows that it need not copy them
 * if they are later written. If passno is one, then this is a first
 * pass, so only setting needs to be done. If passno is 2, then this
 * is a revision to a previous pass which must be undone as the
 * replacement pass is done.
 */
static int
cgaccount1(int cg, struct vnode *vp, void *data, int passno)
{
	struct buf *bp, *ibp;
	struct inode *ip;
	struct cg *cgp;
	struct fs *fs;
	struct lwp *l = curlwp;
	daddr_t base, numblks;
	int error, len, loc, ns, indiroff;

	ip = VTOI(vp);
	fs = ip->i_fs;
	ns = UFS_FSNEEDSWAP(fs);
	error = bread(ip->i_devvp, fsbtodb(fs, cgtod(fs, cg)),
		(int)fs->fs_cgsize, l->l_cred, 0, &bp);
	if (error) {
		brelse(bp, 0);
		return (error);
	}
	cgp = (struct cg *)bp->b_data;
	if (!cg_chkmagic(cgp, ns)) {
		brelse(bp, 0);
		return (EIO);
	}
	ACTIVECG_SET(fs, cg);

	bcopy(bp->b_data, data, fs->fs_cgsize);
	brelse(bp, 0);
	if (fs->fs_cgsize < fs->fs_bsize)
		memset((char *)data + fs->fs_cgsize, 0,
		    fs->fs_bsize - fs->fs_cgsize);
	numblks = howmany(fs->fs_size, fs->fs_frag);
	len = howmany(fs->fs_fpg, fs->fs_frag);
	base = cg * fs->fs_fpg / fs->fs_frag;
	if (base + len >= numblks)
		len = numblks - base - 1;
	loc = 0;
	if (base < NDADDR) {
		for ( ; loc < NDADDR; loc++) {
			if (ffs_isblock(fs, cg_blksfree(cgp, ns), loc))
				db_assign(ip, loc, BLK_NOCOPY);
			else if (db_get(ip, loc) == BLK_NOCOPY) {
				if (passno == 2)
					db_assign(ip, loc, 0);
				else if (passno == 1)
					panic("ffs_snapshot: lost direct block");
			}
		}
	}
	if ((error = ffs_balloc(vp, lblktosize(fs, (off_t)(base + loc)),
	    fs->fs_bsize, l->l_cred, B_METAONLY, &ibp)) != 0)
		return (error);
	indiroff = (base + loc - NDADDR) % NINDIR(fs);
	for ( ; loc < len; loc++, indiroff++) {
		if (indiroff >= NINDIR(fs)) {
			bawrite(ibp);
			if ((error = ffs_balloc(vp,
			    lblktosize(fs, (off_t)(base + loc)),
			    fs->fs_bsize, l->l_cred, B_METAONLY, &ibp)) != 0)
				return (error);
			indiroff = 0;
		}
		if (ffs_isblock(fs, cg_blksfree(cgp, ns), loc))
			idb_assign(ip, ibp->b_data, indiroff, BLK_NOCOPY);
		else if (idb_get(ip, ibp->b_data, indiroff) == BLK_NOCOPY) {
			if (passno == 2)
				idb_assign(ip, ibp->b_data, indiroff, 0);
			else if (passno == 1)
				panic("ffs_snapshot: lost indirect block");
		}
	}
	bdwrite(ibp);
	return (0);
}

/*
 * Before expunging a snapshot inode, note all the
 * blocks that it claims with BLK_SNAP so that fsck will
 * be able to account for those blocks properly and so
 * that this snapshot knows that it need not copy them
 * if the other snapshot holding them is freed.
 */
static int
expunge(struct vnode *snapvp, struct inode *cancelip, struct fs *fs,
    acctfunc_t acctfunc, int expungetype)
{
	int i, error, ns;
	daddr_t lbn, rlbn;
	daddr_t len, blkno, numblks, blksperindir;
	struct ufs1_dinode *dip1;
	struct ufs2_dinode *dip2;
	struct lwp *l = curlwp;
	void *bap;
	struct buf *bp;

	ns = UFS_FSNEEDSWAP(fs);
	/*
	 * Prepare to expunge the inode. If its inode block has not
	 * yet been copied, then allocate and fill the copy.
	 */
	lbn = fragstoblks(fs, ino_to_fsba(fs, cancelip->i_number));
	error = snapblkaddr(snapvp, lbn, &blkno);
	if (error)
		return error;
	if (blkno != 0) {
		error = bread(snapvp, lbn, fs->fs_bsize, l->l_cred,
		    B_MODIFY, &bp);
	} else {
		error = ffs_balloc(snapvp, lblktosize(fs, (off_t)lbn),
		    fs->fs_bsize, l->l_cred, 0, &bp);
		if (! error)
			error = rwfsblk(snapvp, B_READ, bp->b_data, lbn);
	}
	if (error)
		return error;
	/*
	 * Set a snapshot inode to be a zero length file, regular files
	 * or unlinked snapshots to be completely unallocated.
	 */
	if (fs->fs_magic == FS_UFS1_MAGIC) {
		dip1 = (struct ufs1_dinode *)bp->b_data +
		    ino_to_fsbo(fs, cancelip->i_number);
		if (expungetype == BLK_NOCOPY || cancelip->i_ffs_effnlink == 0)
			dip1->di_mode = 0;
		dip1->di_size = 0;
		dip1->di_blocks = 0;
		dip1->di_flags =
		    ufs_rw32(ufs_rw32(dip1->di_flags, ns) & ~SF_SNAPSHOT, ns);
		bzero(&dip1->di_db[0], (NDADDR + NIADDR) * sizeof(int32_t));
	} else {
		dip2 = (struct ufs2_dinode *)bp->b_data +
		    ino_to_fsbo(fs, cancelip->i_number);
		if (expungetype == BLK_NOCOPY || cancelip->i_ffs_effnlink == 0)
			dip2->di_mode = 0;
		dip2->di_size = 0;
		dip2->di_blocks = 0;
		dip2->di_flags =
		    ufs_rw32(ufs_rw32(dip2->di_flags, ns) & ~SF_SNAPSHOT, ns);
		bzero(&dip2->di_db[0], (NDADDR + NIADDR) * sizeof(int64_t));
	}
	bdwrite(bp);
	/*
	 * Now go through and expunge all the blocks in the file
	 * using the function requested.
	 */
	numblks = howmany(cancelip->i_size, fs->fs_bsize);
	if (fs->fs_magic == FS_UFS1_MAGIC)
		bap = &cancelip->i_ffs1_db[0];
	else
		bap = &cancelip->i_ffs2_db[0];
	if ((error = (*acctfunc)(snapvp, bap, 0, NDADDR, fs, 0, expungetype)))
		return (error);
	if (fs->fs_magic == FS_UFS1_MAGIC)
		bap = &cancelip->i_ffs1_ib[0];
	else
		bap = &cancelip->i_ffs2_ib[0];
	if ((error = (*acctfunc)(snapvp, bap, 0, NIADDR, fs, -1, expungetype)))
		return (error);
	blksperindir = 1;
	lbn = -NDADDR;
	len = numblks - NDADDR;
	rlbn = NDADDR;
	for (i = 0; len > 0 && i < NIADDR; i++) {
		error = indiracct(snapvp, ITOV(cancelip), i,
		    ib_get(cancelip, i), lbn, rlbn, len,
		    blksperindir, fs, acctfunc, expungetype);
		if (error)
			return (error);
		blksperindir *= NINDIR(fs);
		lbn -= blksperindir + 1;
		len -= blksperindir;
		rlbn += blksperindir;
	}
	return (0);
}

/*
 * Descend an indirect block chain for vnode cancelvp accounting for all
 * its indirect blocks in snapvp.
 */
static int
indiracct(struct vnode *snapvp, struct vnode *cancelvp, int level,
    daddr_t blkno, daddr_t lbn, daddr_t rlbn, daddr_t remblks,
    daddr_t blksperindir, struct fs *fs, acctfunc_t acctfunc, int expungetype)
{
	int error, num, i;
	daddr_t subblksperindir;
	struct indir indirs[NIADDR + 2];
	daddr_t last;
	void *bap;
	struct buf *bp;

	if (blkno == 0) {
		if (expungetype == BLK_NOCOPY)
			return (0);
		panic("indiracct: missing indir");
	}
	if ((error = ufs_getlbns(cancelvp, rlbn, indirs, &num)) != 0)
		return (error);
	if (lbn != indirs[num - 1 - level].in_lbn || num < 2)
		panic("indiracct: botched params");
	/*
	 * We have to expand bread here since it will deadlock looking
	 * up the block number for any blocks that are not in the cache.
	 */
	error = ffs_getblk(cancelvp, lbn, fsbtodb(fs, blkno), fs->fs_bsize,
	    false, &bp);
	if (error)
		return error;
	if ((bp->b_oflags & (BO_DONE | BO_DELWRI)) == 0 && (error =
	    rwfsblk(bp->b_vp, B_READ, bp->b_data, fragstoblks(fs, blkno)))) {
		brelse(bp, 0);
		return (error);
	}
	/*
	 * Account for the block pointers in this indirect block.
	 */
	last = howmany(remblks, blksperindir);
	if (last > NINDIR(fs))
		last = NINDIR(fs);
	bap = malloc(fs->fs_bsize, M_DEVBUF, M_WAITOK);
	bcopy(bp->b_data, (void *)bap, fs->fs_bsize);
	brelse(bp, 0);
	error = (*acctfunc)(snapvp, bap, 0, last,
	    fs, level == 0 ? rlbn : -1, expungetype);
	if (error || level == 0)
		goto out;
	/*
	 * Account for the block pointers in each of the indirect blocks
	 * in the levels below us.
	 */
	subblksperindir = blksperindir / NINDIR(fs);
	for (lbn++, level--, i = 0; i < last; i++) {
		error = indiracct(snapvp, cancelvp, level,
		    idb_get(VTOI(snapvp), bap, i), lbn, rlbn, remblks,
		    subblksperindir, fs, acctfunc, expungetype);
		if (error)
			goto out;
		rlbn += blksperindir;
		lbn -= blksperindir;
		remblks -= blksperindir;
	}
out:
	FREE(bap, M_DEVBUF);
	return (error);
}

/*
 * Do both snap accounting and map accounting.
 */
static int
fullacct(struct vnode *vp, void *bap, int oldblkp, int lastblkp,
    struct fs *fs, daddr_t lblkno,
    int exptype /* BLK_SNAP or BLK_NOCOPY */)
{
	int error;

	if ((error = snapacct(vp, bap, oldblkp, lastblkp, fs, lblkno, exptype)))
		return (error);
	return (mapacct(vp, bap, oldblkp, lastblkp, fs, lblkno, exptype));
}

/*
 * Identify a set of blocks allocated in a snapshot inode.
 */
static int
snapacct(struct vnode *vp, void *bap, int oldblkp, int lastblkp,
    struct fs *fs, daddr_t lblkno,
    int expungetype /* BLK_SNAP or BLK_NOCOPY */)
{
	struct inode *ip = VTOI(vp);
	struct lwp *l = curlwp;
	daddr_t blkno;
	daddr_t lbn;
	struct buf *ibp;
	int error;

	for ( ; oldblkp < lastblkp; oldblkp++) {
		blkno = idb_get(ip, bap, oldblkp);
		if (blkno == 0 || blkno == BLK_NOCOPY || blkno == BLK_SNAP)
			continue;
		lbn = fragstoblks(fs, blkno);
		if (lbn < NDADDR) {
			blkno = db_get(ip, lbn);
			ip->i_flag |= IN_CHANGE | IN_UPDATE;
		} else {
			error = ffs_balloc(vp, lblktosize(fs, (off_t)lbn),
			    fs->fs_bsize, l->l_cred, B_METAONLY, &ibp);
			if (error)
				return (error);
			blkno = idb_get(ip, ibp->b_data,
			    (lbn - NDADDR) % NINDIR(fs));
		}
		/*
		 * If we are expunging a snapshot vnode and we
		 * find a block marked BLK_NOCOPY, then it is
		 * one that has been allocated to this snapshot after
		 * we took our current snapshot and can be ignored.
		 */
		if (expungetype == BLK_SNAP && blkno == BLK_NOCOPY) {
			if (lbn >= NDADDR)
				brelse(ibp, 0);
		} else {
			if (blkno != 0)
				panic("snapacct: bad block");
			if (lbn < NDADDR)
				db_assign(ip, lbn, expungetype);
			else {
				idb_assign(ip, ibp->b_data,
				    (lbn - NDADDR) % NINDIR(fs), expungetype);
				bdwrite(ibp);
			}
		}
	}
	return (0);
}

/*
 * Account for a set of blocks allocated in a snapshot inode.
 */
static int
mapacct(struct vnode *vp, void *bap, int oldblkp, int lastblkp,
    struct fs *fs, daddr_t lblkno, int expungetype)
{
	daddr_t blkno;
	struct inode *ip;
	ino_t inum;
	int acctit;

	ip = VTOI(vp);
	inum = ip->i_number;
	if (lblkno == -1)
		acctit = 0;
	else
		acctit = 1;
	for ( ; oldblkp < lastblkp; oldblkp++, lblkno++) {
		blkno = idb_get(ip, bap, oldblkp);
		if (blkno == 0 || blkno == BLK_NOCOPY)
			continue;
		if (acctit && expungetype == BLK_SNAP && blkno != BLK_SNAP)
			*ip->i_snapblklist++ = lblkno;
		if (blkno == BLK_SNAP)
			blkno = blkstofrags(fs, lblkno);
		ffs_blkfree(fs, vp, blkno, fs->fs_bsize, inum);
	}
	return (0);
}
#endif /* defined(FFS_NO_SNAPSHOT) */

/*
 * Decrement extra reference on snapshot when last name is removed.
 * It will not be freed until the last open reference goes away.
 */
void
ffs_snapgone(struct inode *ip)
{
	struct mount *mp = ip->i_devvp->v_specmountpoint;
	struct inode *xp;
	struct fs *fs;
	struct snap_info *si;
	int snaploc;

	si = VFSTOUFS(mp)->um_snapinfo;

	/*
	 * Find snapshot in incore list.
	 */
	mutex_enter(&si->si_lock);
	TAILQ_FOREACH(xp, &si->si_snapshots, i_nextsnap)
		if (xp == ip)
			break;
	mutex_exit(&si->si_lock);
	if (xp != NULL)
		vrele(ITOV(ip));
#ifdef DEBUG
	else if (snapdebug)
		printf("ffs_snapgone: lost snapshot vnode %llu\n",
		    (unsigned long long)ip->i_number);
#endif
	/*
	 * Delete snapshot inode from superblock. Keep list dense.
	 */
	mutex_enter(&si->si_lock);
	fs = ip->i_fs;
	for (snaploc = 0; snaploc < FSMAXSNAP; snaploc++)
		if (fs->fs_snapinum[snaploc] == ip->i_number)
			break;
	if (snaploc < FSMAXSNAP) {
		for (snaploc++; snaploc < FSMAXSNAP; snaploc++) {
			if (fs->fs_snapinum[snaploc] == 0)
				break;
			fs->fs_snapinum[snaploc - 1] = fs->fs_snapinum[snaploc];
		}
		fs->fs_snapinum[snaploc - 1] = 0;
	}
	si->si_gen++;
	mutex_exit(&si->si_lock);
}

/*
 * Prepare a snapshot file for being removed.
 */
void
ffs_snapremove(struct vnode *vp)
{
	struct inode *ip = VTOI(vp), *xp;
	struct vnode *devvp = ip->i_devvp;
	struct fs *fs = ip->i_fs;
	struct mount *mp = devvp->v_specmountpoint;
	struct buf *ibp;
	struct snap_info *si;
	struct lwp *l = curlwp;
	daddr_t numblks, blkno, dblk;
	int error, loc, last;

	si = VFSTOUFS(mp)->um_snapinfo;
	mutex_enter(&si->si_snaplock);
	/*
	 * If active, delete from incore list (this snapshot may
	 * already have been in the process of being deleted, so
	 * would not have been active).
	 *
	 * Clear copy-on-write flag if last snapshot.
	 */
	if (ip->i_nextsnap.tqe_prev != 0) {
		mutex_enter(&si->si_lock);
		TAILQ_REMOVE(&si->si_snapshots, ip, i_nextsnap);
		ip->i_nextsnap.tqe_prev = 0;
		if (TAILQ_FIRST(&si->si_snapshots) != 0) {
			/* Roll back the list of preallocated blocks. */
			xp = TAILQ_LAST(&si->si_snapshots, inodelst);
			si->si_snapblklist = xp->i_snapblklist;
		} else {
			si->si_snapblklist = 0;
			si->si_gen++;
			mutex_exit(&si->si_lock);
			fscow_disestablish(mp, ffs_copyonwrite, devvp);
			mutex_enter(&si->si_lock);
		}
		si->si_gen++;
		mutex_exit(&si->si_lock);
		if (ip->i_snapblklist != NULL) {
			free(ip->i_snapblklist, M_UFSMNT);
			ip->i_snapblklist = NULL;
		}
	}
	mutex_exit(&si->si_snaplock);
	/*
	 * Clear all BLK_NOCOPY fields. Pass any block claims to other
	 * snapshots that want them (see ffs_snapblkfree below).
	 */
	for (blkno = 1; blkno < NDADDR; blkno++) {
		dblk = db_get(ip, blkno);
		if (dblk == BLK_NOCOPY || dblk == BLK_SNAP)
			db_assign(ip, blkno, 0);
		else if ((dblk == blkstofrags(fs, blkno) &&
		     ffs_snapblkfree(fs, ip->i_devvp, dblk, fs->fs_bsize,
		     ip->i_number))) {
			DIP_ADD(ip, blocks, -btodb(fs->fs_bsize));
			db_assign(ip, blkno, 0);
		}
	}
	numblks = howmany(ip->i_size, fs->fs_bsize);
	for (blkno = NDADDR; blkno < numblks; blkno += NINDIR(fs)) {
		error = ffs_balloc(vp, lblktosize(fs, (off_t)blkno),
		    fs->fs_bsize, l->l_cred, B_METAONLY, &ibp);
		if (error)
			continue;
		if (fs->fs_size - blkno > NINDIR(fs))
			last = NINDIR(fs);
		else
			last = fs->fs_size - blkno;
		for (loc = 0; loc < last; loc++) {
			dblk = idb_get(ip, ibp->b_data, loc);
			if (dblk == BLK_NOCOPY || dblk == BLK_SNAP)
				idb_assign(ip, ibp->b_data, loc, 0);
			else if (dblk == blkstofrags(fs, blkno) &&
			    ffs_snapblkfree(fs, ip->i_devvp, dblk,
			    fs->fs_bsize, ip->i_number)) {
				DIP_ADD(ip, blocks, -btodb(fs->fs_bsize));
				idb_assign(ip, ibp->b_data, loc, 0);
			}
		}
		bawrite(ibp);
	}
	/*
	 * Clear snapshot flag and drop reference.
	 */
	ip->i_flags &= ~SF_SNAPSHOT;
	DIP_ASSIGN(ip, flags, ip->i_flags);
	ip->i_flag |= IN_CHANGE | IN_UPDATE;
}

/*
 * Notification that a block is being freed. Return zero if the free
 * should be allowed to proceed. Return non-zero if the snapshot file
 * wants to claim the block. The block will be claimed if it is an
 * uncopied part of one of the snapshots. It will be freed if it is
 * either a BLK_NOCOPY or has already been copied in all of the snapshots.
 * If a fragment is being freed, then all snapshots that care about
 * it must make a copy since a snapshot file can only claim full sized
 * blocks. Note that if more than one snapshot file maps the block,
 * we can pick one at random to claim it. Since none of the snapshots
 * can change, we are assurred that they will all see the same unmodified
 * image. When deleting a snapshot file (see ffs_snapremove above), we
 * must push any of these claimed blocks to one of the other snapshots
 * that maps it. These claimed blocks are easily identified as they will
 * have a block number equal to their logical block number within the
 * snapshot. A copied block can never have this property because they
 * must always have been allocated from a BLK_NOCOPY location.
 */
int
ffs_snapblkfree(struct fs *fs, struct vnode *devvp, daddr_t bno,
    long size, ino_t inum)
{
	struct mount *mp = devvp->v_specmountpoint;
	struct buf *ibp;
	struct inode *ip;
	struct vnode *vp = NULL;
	struct snap_info *si;
	void *saved_data = NULL;
	daddr_t lbn;
	daddr_t blkno;
	uint32_t gen;
	int indiroff = 0, snapshot_locked = 0, error = 0, claimedblk = 0;

	si = VFSTOUFS(mp)->um_snapinfo;
	lbn = fragstoblks(fs, bno);
	mutex_enter(&si->si_lock);
retry:
	gen = si->si_gen;
	TAILQ_FOREACH(ip, &si->si_snapshots, i_nextsnap) {
		vp = ITOV(ip);
		if (snapshot_locked == 0) {
			if (!mutex_tryenter(&si->si_snaplock)) {
				mutex_exit(&si->si_lock);
				mutex_enter(&si->si_snaplock);
				mutex_enter(&si->si_lock);
			}
			snapshot_locked = 1;
			if (gen != si->si_gen)
				goto retry;
		}
		/*
		 * Lookup block being written.
		 */
		if (lbn < NDADDR) {
			blkno = db_get(ip, lbn);
		} else {
			mutex_exit(&si->si_lock);
			error = ffs_balloc(vp, lblktosize(fs, (off_t)lbn),
			    fs->fs_bsize, FSCRED, B_METAONLY, &ibp);
			if (error) {
				mutex_enter(&si->si_lock);
				break;
			}
			indiroff = (lbn - NDADDR) % NINDIR(fs);
			blkno = idb_get(ip, ibp->b_data, indiroff);
			mutex_enter(&si->si_lock);
			if (gen != si->si_gen) {
				brelse(ibp, 0);
				goto retry;
			}
		}
		/*
		 * Check to see if block needs to be copied.
		 */
		if (blkno == 0) {
			/*
			 * A block that we map is being freed. If it has not
			 * been claimed yet, we will claim or copy it (below).
			 */
			claimedblk = 1;
		} else if (blkno == BLK_SNAP) {
			/*
			 * No previous snapshot claimed the block,
			 * so it will be freed and become a BLK_NOCOPY
			 * (don't care) for us.
			 */
			if (claimedblk)
				panic("snapblkfree: inconsistent block type");
			if (lbn < NDADDR) {
				db_assign(ip, lbn, BLK_NOCOPY);
				ip->i_flag |= IN_CHANGE | IN_UPDATE;
			} else {
				idb_assign(ip, ibp->b_data, indiroff,
				    BLK_NOCOPY);
				mutex_exit(&si->si_lock);
				if (ip->i_ffs_effnlink > 0)
					bwrite(ibp);
				else
					bdwrite(ibp);
				mutex_enter(&si->si_lock);
				if (gen != si->si_gen)
					goto retry;
			}
			continue;
		} else /* BLK_NOCOPY or default */ {
			/*
			 * If the snapshot has already copied the block
			 * (default), or does not care about the block,
			 * it is not needed.
			 */
			if (lbn >= NDADDR)
				brelse(ibp, 0);
			continue;
		}
		/*
		 * If this is a full size block, we will just grab it
		 * and assign it to the snapshot inode. Otherwise we
		 * will proceed to copy it. See explanation for this
		 * routine as to why only a single snapshot needs to
		 * claim this block.
		 */
		if (size == fs->fs_bsize) {
#ifdef DEBUG
			if (snapdebug)
				printf("%s %llu lbn %" PRId64
				    "from inum %llu\n",
				    "Grabonremove: snapino",
				    (unsigned long long)ip->i_number,
				    lbn, (unsigned long long)inum);
#endif
			mutex_exit(&si->si_lock);
			if (lbn < NDADDR) {
				db_assign(ip, lbn, bno);
			} else {
				idb_assign(ip, ibp->b_data, indiroff, bno);
				if (ip->i_ffs_effnlink > 0)
					bwrite(ibp);
				else
					bdwrite(ibp);
			}
			DIP_ADD(ip, blocks, btodb(size));
			ip->i_flag |= IN_CHANGE | IN_UPDATE;
			if (ip->i_ffs_effnlink > 0 && mp->mnt_wapbl)
				error = syncsnap(vp);
			else
				error = 0;
			mutex_exit(&si->si_snaplock);
			return (error == 0);
		}
		if (lbn >= NDADDR)
			brelse(ibp, 0);
#ifdef DEBUG
		if (snapdebug)
			printf("%s%llu lbn %" PRId64 " %s %llu size %ld\n",
			    "Copyonremove: snapino ",
			    (unsigned long long)ip->i_number,
			    lbn, "for inum", (unsigned long long)inum, size);
#endif
		/*
		 * If we have already read the old block contents, then
		 * simply copy them to the new block. Note that we need
		 * to synchronously write snapshots that have not been
		 * unlinked, and hence will be visible after a crash,
		 * to ensure their integrity.
		 */
		mutex_exit(&si->si_lock);
		if (saved_data == NULL) {
			saved_data = malloc(fs->fs_bsize, M_UFSMNT, M_WAITOK);
			error = rwfsblk(vp, B_READ, saved_data, lbn);
			if (error) {
				free(saved_data, M_UFSMNT);
				saved_data = NULL;
				mutex_enter(&si->si_lock);
				break;
			}
		}
		error = wrsnapblk(vp, saved_data, lbn);
		if (error == 0 && ip->i_ffs_effnlink > 0 && mp->mnt_wapbl)
			error = syncsnap(vp);
		mutex_enter(&si->si_lock);
		if (error)
			break;
		if (gen != si->si_gen)
			goto retry;
	}
	mutex_exit(&si->si_lock);
	if (saved_data)
		free(saved_data, M_UFSMNT);
	/*
	 * If we have been unable to allocate a block in which to do
	 * the copy, then return non-zero so that the fragment will
	 * not be freed. Although space will be lost, the snapshot
	 * will stay consistent.
	 */
	if (snapshot_locked)
		mutex_exit(&si->si_snaplock);
	return (error);
}

/*
 * Associate snapshot files when mounting.
 */
void
ffs_snapshot_mount(struct mount *mp)
{
	struct vnode *devvp = VFSTOUFS(mp)->um_devvp;
	struct fs *fs = VFSTOUFS(mp)->um_fs;
	struct lwp *l = curlwp;
	struct vnode *vp;
	struct inode *ip, *xp;
	struct snap_info *si;
	daddr_t snaplistsize, *snapblklist;
	int i, error, ns, snaploc, loc;

	/*
	 * No persistent snapshots on apple ufs file systems.
	 */
	if (UFS_MPISAPPLEUFS(VFSTOUFS(mp)))
		return;

	si = VFSTOUFS(mp)->um_snapinfo;
	ns = UFS_FSNEEDSWAP(fs);
	/*
	 * XXX The following needs to be set before ffs_truncate or
	 * VOP_READ can be called.
	 */
	mp->mnt_stat.f_iosize = fs->fs_bsize;
	/*
	 * Process each snapshot listed in the superblock.
	 */
	vp = NULL;
	mutex_enter(&si->si_lock);
	for (snaploc = 0; snaploc < FSMAXSNAP; snaploc++) {
		if (fs->fs_snapinum[snaploc] == 0)
			break;
		if ((error = VFS_VGET(mp, fs->fs_snapinum[snaploc],
		    &vp)) != 0) {
			printf("ffs_snapshot_mount: vget failed %d\n", error);
			continue;
		}
		ip = VTOI(vp);
		if ((ip->i_flags & SF_SNAPSHOT) == 0) {
			printf("ffs_snapshot_mount: non-snapshot inode %d\n",
			    fs->fs_snapinum[snaploc]);
			vput(vp);
			vp = NULL;
			for (loc = snaploc + 1; loc < FSMAXSNAP; loc++) {
				if (fs->fs_snapinum[loc] == 0)
					break;
				fs->fs_snapinum[loc - 1] = fs->fs_snapinum[loc];
			}
			fs->fs_snapinum[loc - 1] = 0;
			snaploc--;
			continue;
		}

		/*
		 * Read the block hints list. Use an empty list on
		 * read errors.
		 */
		error = vn_rdwr(UIO_READ, vp,
		    (void *)&snaplistsize, sizeof(snaplistsize),
		    lblktosize(fs, howmany(fs->fs_size, fs->fs_frag)),
		    UIO_SYSSPACE, IO_NODELOCKED|IO_UNIT,
		    l->l_cred, NULL, NULL);
		if (error) {
			printf("ffs_snapshot_mount: read_1 failed %d\n", error);
			snaplistsize = 1;
		} else
			snaplistsize = ufs_rw64(snaplistsize, ns);
		snapblklist = malloc(
		    snaplistsize * sizeof(daddr_t), M_UFSMNT, M_WAITOK);
		if (error)
			snapblklist[0] = 1;
		else {
			error = vn_rdwr(UIO_READ, vp, (void *)snapblklist,
			    snaplistsize * sizeof(daddr_t),
			    lblktosize(fs, howmany(fs->fs_size, fs->fs_frag)),
			    UIO_SYSSPACE, IO_NODELOCKED|IO_UNIT,
			    l->l_cred, NULL, NULL);
			for (i = 0; i < snaplistsize; i++)
				snapblklist[i] = ufs_rw64(snapblklist[i], ns);
			if (error) {
				printf("ffs_snapshot_mount: read_2 failed %d\n",
				    error);
				snapblklist[0] = 1;
			}
		}
		ip->i_snapblklist = &snapblklist[0];

		/*
		 * Link it onto the active snapshot list.
		 */
		if (ip->i_nextsnap.tqe_prev != 0)
			panic("ffs_snapshot_mount: %llu already on list",
			    (unsigned long long)ip->i_number);
		else
			TAILQ_INSERT_TAIL(&si->si_snapshots, ip, i_nextsnap);
		vp->v_vflag |= VV_SYSTEM;
		VOP_UNLOCK(vp, 0);
	}
	/*
	 * No usable snapshots found.
	 */
	if (vp == NULL) {
		mutex_exit(&si->si_lock);
		return;
	}
	/*
	 * Attach the block hints list. We always want to
	 * use the list from the newest snapshot.
	*/
	xp = TAILQ_LAST(&si->si_snapshots, inodelst);
	si->si_snapblklist = xp->i_snapblklist;
	fscow_establish(mp, ffs_copyonwrite, devvp);
	si->si_gen++;
	mutex_exit(&si->si_lock);
}

/*
 * Disassociate snapshot files when unmounting.
 */
void
ffs_snapshot_unmount(struct mount *mp)
{
	struct vnode *devvp = VFSTOUFS(mp)->um_devvp;
	struct inode *xp;
	struct vnode *vp = NULL;
	struct snap_info *si;

	si = VFSTOUFS(mp)->um_snapinfo;
	mutex_enter(&si->si_lock);
	while ((xp = TAILQ_FIRST(&si->si_snapshots)) != 0) {
		vp = ITOV(xp);
		vp->v_vnlock = &vp->v_lock;
		TAILQ_REMOVE(&si->si_snapshots, xp, i_nextsnap);
		xp->i_nextsnap.tqe_prev = 0;
		if (xp->i_snapblklist == si->si_snapblklist)
			si->si_snapblklist = NULL;
		FREE(xp->i_snapblklist, M_UFSMNT);
		if (xp->i_ffs_effnlink > 0) {
			si->si_gen++;
			mutex_exit(&si->si_lock);
			vrele(vp);
			mutex_enter(&si->si_lock);
		}
	}
	if (vp)
		fscow_disestablish(mp, ffs_copyonwrite, devvp);
	si->si_gen++;
	mutex_exit(&si->si_lock);
}

/*
 * Check for need to copy block that is about to be written,
 * copying the block if necessary.
 */
static int
ffs_copyonwrite(void *v, struct buf *bp, bool data_valid)
{
	struct fs *fs;
	struct inode *ip;
	struct vnode *devvp = v, *vp = NULL;
	struct mount *mp = devvp->v_specmountpoint;
	struct snap_info *si;
	void *saved_data = NULL;
	daddr_t lbn, blkno, *snapblklist;
	uint32_t gen;
	int lower, upper, mid, snapshot_locked = 0, error = 0;

	/*
	 * Check for valid snapshots.
	 */
	si = VFSTOUFS(mp)->um_snapinfo;
	mutex_enter(&si->si_lock);
	ip = TAILQ_FIRST(&si->si_snapshots);
	if (ip == NULL) {
		mutex_exit(&si->si_lock);
		return 0;
	}
	/*
	 * First check to see if it is after the file system or
	 * in the preallocated list.
	 * By doing this check we avoid several potential deadlocks.
	 */
	fs = ip->i_fs;
	lbn = fragstoblks(fs, dbtofsb(fs, bp->b_blkno));
	if (bp->b_blkno >= fsbtodb(fs, fs->fs_size)) {
		mutex_exit(&si->si_lock);
		return 0;
	}
	snapblklist = si->si_snapblklist;
	upper = si->si_snapblklist[0] - 1;
	lower = 1;
	while (lower <= upper) {
		mid = (lower + upper) / 2;
		if (snapblklist[mid] == lbn)
			break;
		if (snapblklist[mid] < lbn)
			lower = mid + 1;
		else
			upper = mid - 1;
	}
	if (lower <= upper) {
		mutex_exit(&si->si_lock);
		return 0;
	}
	/*
	 * Not in the precomputed list, so check the snapshots.
	 */
	 if (data_valid && bp->b_bcount == fs->fs_bsize)
		saved_data = bp->b_data;
retry:
	gen = si->si_gen;
	TAILQ_FOREACH(ip, &si->si_snapshots, i_nextsnap) {
		vp = ITOV(ip);
		/*
		 * We ensure that everything of our own that needs to be
		 * copied will be done at the time that ffs_snapshot is
		 * called. Thus we can skip the check here which can
		 * deadlock in doing the lookup in ffs_balloc.
		 */
		if (bp->b_vp == vp)
			continue;
		/*
		 * Check to see if block needs to be copied.
		 */
		if (lbn < NDADDR) {
			blkno = db_get(ip, lbn);
		} else {
			mutex_exit(&si->si_lock);
			if ((error = snapblkaddr(vp, lbn, &blkno)) != 0) {
				mutex_enter(&si->si_lock);
				break;
			}
			mutex_enter(&si->si_lock);
			if (gen != si->si_gen)
				goto retry;
		}
#ifdef DIAGNOSTIC
		if (blkno == BLK_SNAP && bp->b_lblkno >= 0)
			panic("ffs_copyonwrite: bad copy block");
#endif
		if (blkno != 0)
			continue;

		if (curlwp == uvm.pagedaemon_lwp) {
			error = ENOMEM;
			break;
		}

		if (snapshot_locked == 0) {
			if (!mutex_tryenter(&si->si_snaplock)) {
				mutex_exit(&si->si_lock);
				mutex_enter(&si->si_snaplock);
				mutex_enter(&si->si_lock);
			}
			snapshot_locked = 1;
			if (gen != si->si_gen)
				goto retry;

			/* Check again if block still needs to be copied */
			if (lbn < NDADDR) {
				blkno = db_get(ip, lbn);
			} else {
				mutex_exit(&si->si_lock);
				if ((error = snapblkaddr(vp, lbn, &blkno)) != 0) {
					mutex_enter(&si->si_lock);
					break;
				}
				mutex_enter(&si->si_lock);
				if (gen != si->si_gen)
					goto retry;
			}

			if (blkno != 0)
				continue;
		}
		/*
		 * Allocate the block into which to do the copy. Since
		 * multiple processes may all try to copy the same block,
		 * we have to recheck our need to do a copy if we sleep
		 * waiting for the lock.
		 *
		 * Because all snapshots on a filesystem share a single
		 * lock, we ensure that we will never be in competition
		 * with another process to allocate a block.
		 */
#ifdef DEBUG
		if (snapdebug) {
			printf("Copyonwrite: snapino %llu lbn %" PRId64 " for ",
			    (unsigned long long)ip->i_number, lbn);
			if (bp->b_vp == devvp)
				printf("fs metadata");
			else
				printf("inum %llu", (unsigned long long)
				    VTOI(bp->b_vp)->i_number);
			printf(" lblkno %" PRId64 "\n", bp->b_lblkno);
		}
#endif
		/*
		 * If we have already read the old block contents, then
		 * simply copy them to the new block. Note that we need
		 * to synchronously write snapshots that have not been
		 * unlinked, and hence will be visible after a crash,
		 * to ensure their integrity.
		 */
		mutex_exit(&si->si_lock);
		if (saved_data == NULL) {
			saved_data = malloc(fs->fs_bsize, M_UFSMNT, M_WAITOK);
			error = rwfsblk(vp, B_READ, saved_data, lbn);
			if (error) {
				free(saved_data, M_UFSMNT);
				saved_data = NULL;
				mutex_enter(&si->si_lock);
				break;
			}
		}
		error = wrsnapblk(vp, saved_data, lbn);
		if (error == 0 && ip->i_ffs_effnlink > 0 && mp->mnt_wapbl)
			error = syncsnap(vp);
		mutex_enter(&si->si_lock);
		if (error)
			break;
		if (gen != si->si_gen)
			goto retry;
	}
	/*
	 * Note that we need to synchronously write snapshots that
	 * have not been unlinked, and hence will be visible after
	 * a crash, to ensure their integrity.
	 */
	mutex_exit(&si->si_lock);
	if (saved_data && saved_data != bp->b_data)
		free(saved_data, M_UFSMNT);
	if (snapshot_locked)
		mutex_exit(&si->si_snaplock);
	return error;
}

/*
 * Read from a snapshot.
 */
int
ffs_snapshot_read(struct vnode *vp, struct uio *uio, int ioflag)
{
	struct inode *ip = VTOI(vp);
	struct fs *fs = ip->i_fs;
	struct snap_info *si = VFSTOUFS(vp->v_mount)->um_snapinfo;
	struct buf *bp;
	daddr_t lbn, nextlbn;
	off_t fsbytes, bytesinfile;
	long size, xfersize, blkoffset;
	int error;

	fstrans_start(vp->v_mount, FSTRANS_SHARED);
	mutex_enter(&si->si_snaplock);

	fsbytes = lblktosize(fs, howmany(fs->fs_size, fs->fs_frag));
	for (error = 0, bp = NULL; uio->uio_resid > 0; bp = NULL) {
		bytesinfile = fsbytes - uio->uio_offset;
		if (bytesinfile <= 0)
			break;
		lbn = lblkno(fs, uio->uio_offset);
		nextlbn = lbn + 1;
		size = blksize(fs, ip, lbn);
		blkoffset = blkoff(fs, uio->uio_offset);
		xfersize = MIN(MIN(fs->fs_bsize - blkoffset, uio->uio_resid),
		    bytesinfile);

		if (lblktosize(fs, nextlbn) >= ip->i_size)
			error = bread(vp, lbn, size, NOCRED, 0, &bp);
		else {
			int nextsize = blksize(fs, ip, nextlbn);
			error = breadn(vp, lbn,
			    size, &nextlbn, &nextsize, 1, NOCRED, 0, &bp);
		}
		if (error)
			break;

		/*
		 * We should only get non-zero b_resid when an I/O error
		 * has occurred, which should cause us to break above.
		 * However, if the short read did not cause an error,
		 * then we want to ensure that we do not uiomove bad
		 * or uninitialized data.
		 */
		size -= bp->b_resid;
		if (size < xfersize) {
			if (size == 0)
				break;
			xfersize = size;
		}
		error = uiomove((char *)bp->b_data + blkoffset, xfersize, uio);
		if (error)
			break;
		brelse(bp, BC_AGE);
	}
	if (bp != NULL)
		brelse(bp, BC_AGE);

	mutex_exit(&si->si_snaplock);
	fstrans_done(vp->v_mount);
	return error;
}

/*
 * Lookup a snapshots data block address.
 * Simpler than UFS_BALLOC() as we know all metadata is already allocated
 * and safe even for the pagedaemon where we cannot bread().
 */
static int
snapblkaddr(struct vnode *vp, daddr_t lbn, daddr_t *res)
{
	struct indir indirs[NIADDR + 2];
	struct inode *ip = VTOI(vp);
	struct fs *fs = ip->i_fs;
	struct buf *bp;
	int error, num;

	KASSERT(lbn >= 0);

	if (lbn < NDADDR) {
		*res = db_get(ip, lbn);
		return 0;
	}
	if ((error = ufs_getlbns(vp, lbn, indirs, &num)) != 0)
		return error;
	if (curlwp == uvm.pagedaemon_lwp) {
		mutex_enter(&bufcache_lock);
		bp = incore(vp, indirs[num-1].in_lbn);
		if (bp && (bp->b_oflags & (BO_DONE | BO_DELWRI))) {
			*res = idb_get(ip, bp->b_data, indirs[num-1].in_off);
			error = 0;
		} else
			error = ENOMEM;
		mutex_exit(&bufcache_lock);
		return error;
	}
	error = bread(vp, indirs[num-1].in_lbn, fs->fs_bsize, NOCRED, 0, &bp);
	if (error == 0)
		*res = idb_get(ip, bp->b_data, indirs[num-1].in_off);
	brelse(bp, 0);

	return error;
}

/*
 * Read or write the specified block of the filesystem vp resides on
 * from or to the disk bypassing the buffer cache.
 */
static int
rwfsblk(struct vnode *vp, int flags, void *data, daddr_t lbn)
{
	int error;
	struct inode *ip = VTOI(vp);
	struct fs *fs = ip->i_fs;
	struct buf *nbp;

	nbp = getiobuf(NULL, true);
	nbp->b_flags = flags;
	nbp->b_bcount = nbp->b_bufsize = fs->fs_bsize;
	nbp->b_error = 0;
	nbp->b_data = data;
	nbp->b_blkno = nbp->b_rawblkno = fsbtodb(fs, blkstofrags(fs, lbn));
	nbp->b_proc = NULL;
	nbp->b_dev = ip->i_devvp->v_rdev;
	SET(nbp->b_cflags, BC_BUSY);	/* mark buffer busy */

	bdev_strategy(nbp);

	error = biowait(nbp);

	putiobuf(nbp);

	return error;
}

/*
 * Write all dirty buffers to disk and invalidate them.
 */
static int
syncsnap(struct vnode *vp)
{
	int error;
	buf_t *bp;
	struct fs *fs = VTOI(vp)->i_fs;

	mutex_enter(&bufcache_lock);
	while ((bp = LIST_FIRST(&vp->v_dirtyblkhd))) {
		KASSERT((bp->b_cflags & BC_BUSY) == 0);
		KASSERT(bp->b_bcount == fs->fs_bsize);
		bp->b_cflags |= BC_BUSY;
		mutex_exit(&bufcache_lock);
		error = rwfsblk(vp, B_WRITE, bp->b_data,
		    fragstoblks(fs, dbtofsb(fs, bp->b_blkno)));
		brelse(bp, BC_INVAL | BC_VFLUSH);
		if (error)
			return error;
		mutex_enter(&bufcache_lock);
	}
	mutex_exit(&bufcache_lock);

	return 0;
}

/*
 * Write the specified block to a snapshot.
 */
static int
wrsnapblk(struct vnode *vp, void *data, daddr_t lbn)
{
	struct inode *ip = VTOI(vp);
	struct fs *fs = ip->i_fs;
	struct buf *bp;
	int error;

	error = ffs_balloc(vp, lblktosize(fs, (off_t)lbn), fs->fs_bsize,
	    FSCRED, (ip->i_ffs_effnlink > 0 ? B_SYNC : 0), &bp);
	if (error)
		return error;
	bcopy(data, bp->b_data, fs->fs_bsize);
	if (ip->i_ffs_effnlink > 0)
		error = bwrite(bp);
	else
		bawrite(bp);

	return error;
}

/*
 * Get/Put direct block from inode or buffer containing disk addresses. Take
 * care for fs type (UFS1/UFS2) and byte swapping. These functions should go
 * into a global include.
 */
static inline daddr_t
db_get(struct inode *ip, int loc)
{
	if (ip->i_ump->um_fstype == UFS1)
		return ufs_rw32(ip->i_ffs1_db[loc], UFS_IPNEEDSWAP(ip));
	else
		return ufs_rw64(ip->i_ffs2_db[loc], UFS_IPNEEDSWAP(ip));
}

static inline void
db_assign(struct inode *ip, int loc, daddr_t val)
{
	if (ip->i_ump->um_fstype == UFS1)
		ip->i_ffs1_db[loc] = ufs_rw32(val, UFS_IPNEEDSWAP(ip));
	else
		ip->i_ffs2_db[loc] = ufs_rw64(val, UFS_IPNEEDSWAP(ip));
}

static inline daddr_t
ib_get(struct inode *ip, int loc)
{
	if (ip->i_ump->um_fstype == UFS1)
		return ufs_rw32(ip->i_ffs1_ib[loc], UFS_IPNEEDSWAP(ip));
	else
		return ufs_rw64(ip->i_ffs2_ib[loc], UFS_IPNEEDSWAP(ip));
}

static inline void
ib_assign(struct inode *ip, int loc, daddr_t val)
{
	if (ip->i_ump->um_fstype == UFS1)
		ip->i_ffs1_ib[loc] = ufs_rw32(val, UFS_IPNEEDSWAP(ip));
	else
		ip->i_ffs2_ib[loc] = ufs_rw64(val, UFS_IPNEEDSWAP(ip));
}

static inline daddr_t
idb_get(struct inode *ip, void *bf, int loc)
{
	if (ip->i_ump->um_fstype == UFS1)
		return ufs_rw32(((int32_t *)(bf))[loc], UFS_IPNEEDSWAP(ip));
	else
		return ufs_rw64(((int64_t *)(bf))[loc], UFS_IPNEEDSWAP(ip));
}

static inline void
idb_assign(struct inode *ip, void *bf, int loc, daddr_t val)
{
	if (ip->i_ump->um_fstype == UFS1)
		((int32_t *)(bf))[loc] = ufs_rw32(val, UFS_IPNEEDSWAP(ip));
	else
		((int64_t *)(bf))[loc] = ufs_rw64(val, UFS_IPNEEDSWAP(ip));
}
