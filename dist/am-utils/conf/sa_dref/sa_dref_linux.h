/*	$NetBSD: sa_dref_linux.h,v 1.1.1.2 2000/11/19 23:43:12 wiz Exp $	*/

/* $srcdir/conf/sa_dref/sa_dref_linux.h */
#define	NFS_SA_DREF(dst, src) memmove((char *)&dst->addr, (char *) src, sizeof(struct sockaddr_in))
