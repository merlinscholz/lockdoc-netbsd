/*	$NetBSD: sa_dref_aoi.h,v 1.1.1.2 2000/11/19 23:43:12 wiz Exp $	*/

/* $srcdir/conf/sa_dref/sa_dref_aoi.h */
#define	NFS_SA_DREF(dst, src) { \
		(dst)->addr->buf = (char *) (src); \
		(dst)->addr->len = sizeof(struct sockaddr_in); \
		(dst)->addr->maxlen = sizeof(struct sockaddr_in); \
	}
