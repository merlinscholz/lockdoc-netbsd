/*	$NetBSD: xdr_mem.c,v 1.8 1998/02/10 04:55:01 lukem Exp $	*/

/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user.
 * 
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 * 
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 * 
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 * 
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 * 
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char *sccsid = "@(#)xdr_mem.c 1.19 87/08/11 Copyr 1984 Sun Micro";
static char *sccsid = "@(#)xdr_mem.c	2.1 88/07/29 4.0 RPCSRC";
#else
__RCSID("$NetBSD: xdr_mem.c,v 1.8 1998/02/10 04:55:01 lukem Exp $");
#endif
#endif

/*
 * xdr_mem.h, XDR implementation using memory buffers.
 *
 * Copyright (C) 1984, Sun Microsystems, Inc.
 *
 * If you have some data to be interpreted as external data representation
 * or to be converted to external data representation in a memory buffer,
 * then this is the package for you.
 *
 */

#include "namespace.h"

#include <sys/types.h>

#include <netinet/in.h>
#include <string.h>

#include <rpc/types.h>
#include <rpc/xdr.h>

#ifdef __weak_alias
__weak_alias(xdrmem_create,_xdrmem_create);
#endif

static void xdrmem_destroy __P((XDR *));
static bool_t xdrmem_getlong_aligned __P((XDR *, int32_t *));
static bool_t xdrmem_putlong_aligned __P((XDR *, int32_t *));
static bool_t xdrmem_getlong_unaligned __P((XDR *, int32_t *));
static bool_t xdrmem_putlong_unaligned __P((XDR *, int32_t *));
static bool_t xdrmem_getbytes __P((XDR *, caddr_t, size_t));
static bool_t xdrmem_putbytes __P((XDR *, caddr_t, size_t));
static size_t  xdrmem_getpos __P((XDR *));
static bool_t xdrmem_setpos __P((XDR *, size_t));
static int32_t *xdrmem_inline_aligned __P((XDR *, size_t));
static int32_t *xdrmem_inline_unaligned __P((XDR *, size_t));

static struct	xdr_ops xdrmem_ops_aligned = {
	xdrmem_getlong_aligned,
	xdrmem_putlong_aligned,
	xdrmem_getbytes,
	xdrmem_putbytes,
	xdrmem_getpos,
	xdrmem_setpos,
	xdrmem_inline_aligned,
	xdrmem_destroy
};

static struct	xdr_ops xdrmem_ops_unaligned = {
	xdrmem_getlong_unaligned,
	xdrmem_putlong_unaligned,
	xdrmem_getbytes,
	xdrmem_putbytes,
	xdrmem_getpos,
	xdrmem_setpos,
	xdrmem_inline_unaligned,
	xdrmem_destroy
};

/*
 * The procedure xdrmem_create initializes a stream descriptor for a
 * memory buffer.  
 */
void
xdrmem_create(xdrs, addr, size, op)
	XDR *xdrs;
	caddr_t addr;
	size_t size;
	enum xdr_op op;
{

	xdrs->x_op = op;
	xdrs->x_ops = ((size_t)addr & (sizeof(int32_t) - 1))
	    ? &xdrmem_ops_unaligned : &xdrmem_ops_aligned;
	xdrs->x_private = xdrs->x_base = addr;
	xdrs->x_handy = size;
}

/*ARGSUSED*/
static void
xdrmem_destroy(xdrs)
	XDR *xdrs;
{

}

static bool_t
xdrmem_getlong_aligned(xdrs, lp)
	XDR *xdrs;
	int32_t *lp;
{

	if ((xdrs->x_handy -= sizeof(int32_t)) < 0)
		return (FALSE);
	*lp = ntohl(*(int32_t *)xdrs->x_private);
	xdrs->x_private += sizeof(int32_t);
	return (TRUE);
}

static bool_t
xdrmem_putlong_aligned(xdrs, lp)
	XDR *xdrs;
	int32_t *lp;
{

	if ((xdrs->x_handy -= sizeof(int32_t)) < 0)
		return (FALSE);
	*(int32_t *)xdrs->x_private = htonl(*lp);
	xdrs->x_private += sizeof(int32_t);
	return (TRUE);
}

static bool_t
xdrmem_getlong_unaligned(xdrs, lp)
	XDR *xdrs;
	int32_t *lp;
{
	int32_t l;

	if ((xdrs->x_handy -= sizeof(int32_t)) < 0)
		return (FALSE);
	memmove(&l, xdrs->x_private, sizeof(int32_t));
	*lp = ntohl(l);
	xdrs->x_private += sizeof(int32_t);
	return (TRUE);
}

static bool_t
xdrmem_putlong_unaligned(xdrs, lp)
	XDR *xdrs;
	int32_t *lp;
{
	int32_t l;

	if ((xdrs->x_handy -= sizeof(int32_t)) < 0)
		return (FALSE);
	l = htonl(*lp);
	memmove(xdrs->x_private, &l, sizeof(int32_t));
	xdrs->x_private += sizeof(int32_t);
	return (TRUE);
}

static bool_t
xdrmem_getbytes(xdrs, addr, len)
	XDR *xdrs;
	caddr_t addr;
	size_t len;
{

	if ((xdrs->x_handy -= len) < 0)
		return (FALSE);
	memmove(addr, xdrs->x_private, len);
	xdrs->x_private += len;
	return (TRUE);
}

static bool_t
xdrmem_putbytes(xdrs, addr, len)
	XDR *xdrs;
	caddr_t addr;
	size_t len;
{

	if ((xdrs->x_handy -= len) < 0)
		return (FALSE);
	memmove(xdrs->x_private, addr, len);
	xdrs->x_private += len;
	return (TRUE);
}

static size_t
xdrmem_getpos(xdrs)
	XDR *xdrs;
{

	return ((size_t)xdrs->x_private - (size_t)xdrs->x_base);
}

static bool_t
xdrmem_setpos(xdrs, pos)
	XDR *xdrs;
	size_t pos;
{
	caddr_t newaddr = xdrs->x_base + pos;
	caddr_t lastaddr = xdrs->x_private + xdrs->x_handy;

	if (newaddr > lastaddr)
		return (FALSE);
	xdrs->x_private = newaddr;
	xdrs->x_handy = (int)(lastaddr - newaddr);
	return (TRUE);
}

static int32_t *
xdrmem_inline_aligned(xdrs, len)
	XDR *xdrs;
	size_t len;
{
	int32_t *buf = 0;

	if (xdrs->x_handy >= len) {
		xdrs->x_handy -= len;
		buf = (int32_t *)xdrs->x_private;
		xdrs->x_private += len;
	}
	return (buf);
}

static int32_t *
xdrmem_inline_unaligned(xdrs, len)
	XDR *xdrs;
	size_t len;
{

	return (0);
}
