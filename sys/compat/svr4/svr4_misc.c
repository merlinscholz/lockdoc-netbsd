/*	$NetBSD: svr4_misc.c,v 1.10 1994/10/29 00:43:22 christos Exp $	 */

/*
 * Copyright (c) 1994 Christos Zoulas
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
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * SVR4 compatibility module.
 *
 * SVR4 system calls that are implemented differently in BSD are
 * handled here.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/namei.h>
#include <sys/dir.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/filedesc.h>
#include <sys/ioctl.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/resourcevar.h>
#include <sys/socket.h>
#include <sys/vnode.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <sys/unistd.h>
#include <sys/times.h>

#include <netinet/in.h>
#include <sys/syscallargs.h>

#include <miscfs/specfs/specdev.h>

#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_syscallargs.h>
#include <compat/svr4/svr4_util.h>
#include <compat/svr4/svr4_time.h>
#include <compat/svr4/svr4_dirent.h>

#include <vm/vm.h>

int
svr4_wait(p, uap, retval)
	register struct proc		*p;
	register struct svr4_wait_args	*uap;
	register_t			*retval;
{
	struct wait4_args w4;

	SCARG(&w4, rusage) = NULL;
	SCARG(&w4, options) = 0;
	SCARG(&w4, status) = SCARG(uap, status);
	SCARG(&w4, pid) = WAIT_ANY;

	return wait4(p, &w4, retval);
}


int
svr4_execv(p, uap, retval)
	register struct proc		*p;
	register struct svr4_execv_args	*uap;
	register_t			*retval;
{
	struct execve_args ex;

	caddr_t sg = stackgap_init();
	CHECKALT(p, &sg, SCARG(uap, path));

	SCARG(&ex, path) = SCARG(uap, path);
	SCARG(&ex, argp) = SCARG(uap, argp);
	SCARG(&ex, envp) = NULL;

	return execve(p, &ex, retval);
}


int
svr4_execve(p, uap, retval)
	register struct proc		*p;
	register struct execve_args	*uap;
	register_t			*retval;
{
	caddr_t sg = stackgap_init();
	CHECKALT(p, &sg, SCARG(uap, path));

	return execve(p, uap, retval);
}


int
svr4_time(p, uap, retval)
	register struct proc		*p;
	register struct svr4_time_args	*uap;
	register_t			*retval;
{
	int		error = 0;
	struct timeval	tv;

	microtime(&tv);
	if (SCARG(uap, t))
		error = copyout(&tv.tv_sec, SCARG(uap, t),
				sizeof(*(SCARG(uap, t))));
	else
		*retval = (int) tv.tv_sec;

	return error;
}


int
svr4_sigpending(p, uap, retval)
	register struct proc			*p;
	register struct svr4_sigpending_args	*uap;
	register_t				*retval;
{
	int	mask = p->p_siglist & p->p_sigmask;

	return copyout(&mask, SCARG(uap, mask), sizeof(int));
}

/*
 * Read SVR4-style directory entries.  We suck them into kernel space so
 * that they can be massaged before being copied out to user code.  Like
 * SunOS, we squish out `empty' entries.
 *
 * This is quite ugly, but what do you expect from compatibility code?
 */
int
svr4_getdents(p, uap, retval)
	register struct proc			*p;
	register struct svr4_getdents_args	*uap;
	register_t				*retval;
{
	struct vnode		*vp;
	caddr_t			 inp, buf;	/* BSD-format */
	int			 len, reclen;	/* BSD-format */
	caddr_t			 outp;		/* SVR4-format */
	int			 resid;		/* SVR4-format */
	struct file		*fp;
	struct uio		 auio;
	struct iovec		 aiov;
	struct svr4_dirent	 idb;
	off_t			 off;		/* true file offset */
	svr4_off_t		 soff;		/* SYSV file offset */
	int			 buflen, error, eofflag;

#define	BSD_DIRENT(cp)		((struct dirent *)(cp))
#define SVR4_DIRENT(cp)		((struct svr4_dirent *)(cp))
#define	SVR4_RECLEN(reclen)	(reclen + sizeof(u_short))

	if ((error = getvnode(p->p_fd, SCARG(uap, fd), &fp)) != 0)
		return (error);
	if ((fp->f_flag & FREAD) == 0)
		return (EBADF);

	vp = (struct vnode *) fp->f_data;

	if (vp->v_type != VDIR)	/* XXX  vnode readdir op should do this */
		return (EINVAL);

	buflen = min(MAXBSIZE, SCARG(uap, nbytes));
	buf = malloc(buflen, M_TEMP, M_WAITOK);
	VOP_LOCK(vp);
	off = fp->f_offset;
again:
	aiov.iov_base = buf;
	aiov.iov_len = buflen;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_READ;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_procp = p;
	auio.uio_resid = buflen;
	auio.uio_offset = off;
	/*
         * First we read into the malloc'ed buffer, then
         * we massage it into user space, one record at a time.
         */
	error = VOP_READDIR(vp, &auio, fp->f_cred, &eofflag, (u_long *) 0, 0);
	if (error)
		goto out;

	inp = buf;
	outp = SCARG(uap, buf);
	resid = SCARG(uap, nbytes);
	if ((len = buflen - auio.uio_resid) == 0)
		goto eof;

	for (; len > 0; len -= reclen) {
		reclen = ((struct dirent *) inp)->d_reclen;
		if (reclen & 3)
			panic("svr4_getdents");
		off += reclen;	/* each entry points to next */
		if (BSD_DIRENT(inp)->d_fileno == 0) {
			inp += reclen;	/* it is a hole; squish it out */
			continue;
		}
		if (reclen > len || resid < SVR4_RECLEN(reclen)) {
			/* entry too big for buffer, so just stop */
			outp++;
			break;
		}
		/*
		 * Massage in place to make a SVR4-shaped dirent (otherwise
		 * we have to worry about touching user memory outside of
		 * the copyout() call).
		 */
		idb.d_ino = (svr4_ino_t) BSD_DIRENT(inp)->d_fileno;
		idb.d_off = (svr4_off_t) off;
		idb.d_reclen = (u_short) SVR4_RECLEN(reclen);
		if ((error = copyout((caddr_t) & idb, outp, 10)) != 0 ||
		    (error = copyout(BSD_DIRENT(inp)->d_name, outp + 10,
				     BSD_DIRENT(inp)->d_namlen + 1)) != 0)
			goto out;
		/* advance past this real entry */
		inp += reclen;
		/* advance output past Sun-shaped entry */
		outp += SVR4_RECLEN(reclen);
		resid -= SVR4_RECLEN(reclen);
	}

	/* if we squished out the whole block, try again */
	if (outp == SCARG(uap, buf))
		goto again;
	fp->f_offset = off;	/* update the vnode offset */

eof:
	*retval = SCARG(uap, nbytes) - resid;
out:
	VOP_UNLOCK(vp);
	free(buf, M_TEMP);
	return error;
}

#define DEVZERO makedev(2, 12)

int
svr4_mmap(p, uap, retval)
	register struct proc		*p;
	register struct svr4_mmap_args	*uap;
	register_t			*retval;
{
	struct filedesc		*fdp;
	struct file		*fp;
	struct vnode		*vp;
	struct mmap_args	 mm;
	caddr_t 		 rp;

	/*
         * Verify the arguments.
         */
	if (SCARG(uap, prot) & ~(PROT_READ | PROT_WRITE | PROT_EXEC))
		return EINVAL;	/* XXX still needed? */

	SCARG(&mm, prot) = SCARG(uap, prot);
	SCARG(&mm, len) = SCARG(uap, len);
	SCARG(&mm, flags) = SCARG(uap, flags);
	SCARG(&mm, fd) = SCARG(uap, fd);
	SCARG(&mm, addr) = SCARG(uap, addr);
	SCARG(&mm, pos) = SCARG(uap, pos);

	rp = (caddr_t) round_page(p->p_vmspace->vm_daddr + MAXDSIZ);
	if ((SCARG(&mm, flags) & MAP_FIXED) == 0 &&
	    SCARG(&mm, addr) != 0 && SCARG(&mm, addr) < rp)
		SCARG(&mm, addr) = rp;

	/*
         * Special case: if fd refers to /dev/zero, map as MAP_ANON.  (XXX)
         */
	fdp = p->p_fd;
	if ((unsigned) SCARG(uap, fd) < fdp->fd_nfiles &&	 /* XXX */
	    (fp = fdp->fd_ofiles[SCARG(uap, fd)]) != NULL &&	 /* XXX */
	    fp->f_type == DTYPE_VNODE &&			 /* XXX */
	    (vp = (struct vnode *) fp->f_data)->v_type == VCHR &&/* XXX */
	    vp->v_rdev == DEVZERO) {	/* XXX */
		SCARG(&mm, flags) |= MAP_ANON;
		SCARG(&mm, fd) = -1;
	}

	return mmap(p, &mm, retval);
}

int
svr4_fchroot(p, uap, retval)
	register struct proc			*p;
	register struct svr4_fchroot_args	*uap;
	register_t				*retval;
{
	struct filedesc	*fdp = p->p_fd;
	struct vnode	*vp;
	struct file	*fp;
	int		 error;

	if ((error = suser(p->p_ucred, &p->p_acflag)) != 0)
		return error;
	if ((error = getvnode(fdp, SCARG(uap, fd), &fp)) != 0)
		return error;
	vp = (struct vnode *) fp->f_data;
	VOP_LOCK(vp);
	if (vp->v_type != VDIR)
		error = ENOTDIR;
	else
		error = VOP_ACCESS(vp, VEXEC, p->p_ucred, p);
	VOP_UNLOCK(vp);
	if (error)
		return error;
	VREF(vp);
	if (fdp->fd_rdir != NULL)
		vrele(fdp->fd_rdir);
	fdp->fd_rdir = vp;
	return 0;
}


int
svr4_mknod(p, uap, retval)
	register struct proc		*p;
	register struct svr4_mknod_args	*uap;
	register_t			*retval;
{
	caddr_t sg = stackgap_init();
	CHECKALT(p, &sg, SCARG(uap, path));

	if (S_ISFIFO(SCARG(uap, mode))) {
		struct mkfifo_args ap;
		SCARG(&ap, path) = SCARG(uap, path);
		SCARG(&ap, mode) = SCARG(uap, mode);
		return mkfifo(p, &ap, retval);
	} else {
		struct mknod_args ap;
		SCARG(&ap, path) = SCARG(uap, path);
		SCARG(&ap, mode) = SCARG(uap, mode);
		SCARG(&ap, dev) = SCARG(uap, dev);
		return mknod(p, &ap, retval);
	}
}

int
svr4_vhangup(p, uap, retval)
	struct proc	*p;
	void		*uap;
	int		*retval;
{
	return 0;
}

#define SVR4_CONFIG_UNUSED	1
#define SVR4_CONFIG_NGROUPS	2
#define SVR4_CONFIG_CHILD_MAX	3
#define SVR4_CONFIG_OPEN_FILES	4
#define SVR4_CONFIG_POSIX_VER	5
#define SVR4_CONFIG_PAGESIZE	6
#define SVR4_CONFIG_CLK_TCK	7
#define SVR4_CONFIG_XOPEN_VER	8
#define SVR4_CONFIG_PROF_TCK	10

int
svr4_sysconfig(p, uap, retval)
	register struct proc			*p;
	register struct svr4_sysconfig_args	*uap;
	register_t				*retval;
{
	extern int	maxfiles;

	switch (SCARG(uap, name)) {
	case SVR4_CONFIG_UNUSED:
		*retval = 0;
		break;
	case SVR4_CONFIG_NGROUPS:
		*retval = NGROUPS_MAX;
		break;
	case SVR4_CONFIG_CHILD_MAX:
		*retval = maxproc;
		break;
	case SVR4_CONFIG_OPEN_FILES:
		*retval = maxfiles;
		break;
	case SVR4_CONFIG_POSIX_VER:
		*retval = 198808;
		break;
	case SVR4_CONFIG_PAGESIZE:
		*retval = NBPG;
		break;
	case SVR4_CONFIG_CLK_TCK:
		*retval = 60;	/* should this be `hz', ie. 100? */
		break;
	case SVR4_CONFIG_XOPEN_VER:
		*retval = 2;	/* XXX: What should that be? */
		break;
	case SVR4_CONFIG_PROF_TCK:
		*retval = 60;	/* XXX: What should that be? */
		break;
	default:
		return EINVAL;
	}
	return 0;
}

#define SVR4_RLIMIT_NOFILE	5	/* Other RLIMIT_* are the same */
#define SVR4_RLIMIT_VMEM	6	/* Other RLIMIT_* are the same */
#define SVR4_RLIM_NLIMITS	7

int
svr4_getrlimit(p, uap, retval)
	register struct proc			*p;
	register struct svr4_getrlimit_args	*uap;
	register_t				*retval;
{
	struct compat_43_getrlimit_args ap;

	if (SCARG(uap, which) >= SVR4_RLIM_NLIMITS)
		return EINVAL;

	if (SCARG(uap, which) == SVR4_RLIMIT_NOFILE)
		SCARG(uap, which) = RLIMIT_NOFILE;
	if (SCARG(uap, which) == SVR4_RLIMIT_VMEM)
		SCARG(uap, which) = RLIMIT_RSS;

	SCARG(&ap, which) = SCARG(uap, which);
	SCARG(&ap, rlp) = SCARG(uap, rlp);

	return compat_43_getrlimit(p, &ap, retval);
}

int
svr4_setrlimit(p, uap, retval)
	register struct proc			*p;
	register struct svr4_setrlimit_args	*uap;
	register_t				*retval;
{
	struct compat_43_setrlimit_args ap;

	if (SCARG(uap, which) >= SVR4_RLIM_NLIMITS)
		return EINVAL;

	if (SCARG(uap, which) == SVR4_RLIMIT_NOFILE)
		SCARG(uap, which) = RLIMIT_NOFILE;
	if (SCARG(uap, which) == SVR4_RLIMIT_VMEM)
		SCARG(uap, which) = RLIMIT_RSS;

	SCARG(&ap, which) = SCARG(uap, which);
	SCARG(&ap, rlp) = SCARG(uap, rlp);

	return compat_43_setrlimit(p, uap, retval);
}


int
svr4_syssun(p, uap, retval)
	register struct proc			*p;
	register struct svr4_syssun_args	*uap;
	register_t				*retval;
{
#ifdef DEBUG_SVR4
	printf("syssun(%d)\n", SCARG(uap, gate));
#endif
	return 0;
}

/* ARGSUSED */
int
svr4_break(p, uap, retval)
	register struct proc		*p;
	register struct svr4_break_args	*uap;
	register_t			*retval;
{
	register struct vmspace *vm = p->p_vmspace;
	vm_offset_t     new, old;
	int             rv;
	register int    diff;

	old = (vm_offset_t) vm->vm_daddr;
	new = round_page(SCARG(uap, nsize));
	diff = new - old;

	printf("break(1): old %x new %x diff %x\n", old, new, diff);

	if ((int) diff > p->p_rlimit[RLIMIT_DATA].rlim_cur)
		return ENOMEM;

	old = round_page(old + ctob(vm->vm_dsize));
	printf("break(2): dsize = %x ctob %x\n",
	       vm->vm_dsize, ctob(vm->vm_dsize));

	new = old + diff;
	printf("break(3): old %x new %x diff %x\n", old, new, diff);

	if (diff > 0) {
		rv = vm_allocate(&vm->vm_map, &old, diff, FALSE);
		if (rv != KERN_SUCCESS) {
			uprintf("sbrk: grow failed, return = %d\n", rv);
			return ENOMEM;
		}
		vm->vm_dsize += btoc(diff);
	} else if (diff < 0) {
		diff = -diff;
		rv = vm_deallocate(&vm->vm_map, new, diff);
		if (rv != KERN_SUCCESS) {
			uprintf("sbrk: shrink failed, return = %d\n", rv);
			return ENOMEM;
		}
		vm->vm_dsize -= btoc(diff);
	}
	return 0;
}

static __inline clock_t
timeval_to_clock_t(tv)
	struct timeval *tv;
{
	return tv->tv_sec * hz + tv->tv_usec / (1000000 / hz);
}

int
svr4_times(p, uap, retval)
	register struct proc		*p;
	register struct svr4_times_args	*uap;
	register_t			*retval;
{
	int			 error;
	struct tms		 tms;
	struct timeval		 t;
	struct rusage		*ru;
	struct rusage		 r;
	struct getrusage_args 	 ga;

	caddr_t sg = stackgap_init();
	ru = stackgap_alloc(&sg, sizeof(struct rusage));

	SCARG(&ga, who) = RUSAGE_SELF;
	SCARG(&ga, rusage) = ru;

	error = getrusage(p, &ga, retval);
	if (error)
		return error;

	if (error = copyin(ru, &r, sizeof r))
		return error;

	tms.tms_utime = timeval_to_clock_t(r.ru_utime);
	tms.tms_stime = timeval_to_clock_t(r.ru_stime);

	SCARG(&ga, who) = RUSAGE_CHILDREN;
	error = getrusage(p, &ga, retval);
	if (error)
		return error;

	if (error = copyin(ru, &r, sizeof r))
		return error;

	tms.tms_cutime = timeval_to_clock_t(&r.ru_utime);
	tms.tms_cstime = timeval_to_clock_t(&r.ru_stime);

	microtime(&t);
	*retval = timeval_to_clock_t(t);

	return copyout(&tms, SCARG(uap, tp), sizeof(tms));
}
