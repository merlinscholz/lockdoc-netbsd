/*	$NetBSD: _err.c,v 1.11 2005/09/13 01:44:09 christos Exp $	*/

/*
 * J.T. Conklin, December 12, 1994
 * Public Domain
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
__RCSID("$NetBSD: _err.c,v 1.11 2005/09/13 01:44:09 christos Exp $");
#endif /* LIBC_SCCS and not lint */

#if defined(__indr_reference)
__indr_reference(_err, err)
#else

#include <stdarg.h>

__dead void _verr(int eval, const char *, _BSD_VA_LIST_);

__dead void
err(int eval, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	_verr(eval, fmt, ap);
	va_end(ap);
}
#endif
