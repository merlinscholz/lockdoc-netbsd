/*	$NetBSD: sys_machdep.c,v 1.7 2007/12/20 23:02:39 dsl Exp $	*/

/*	$OpenBSD: sys_machdep.c,v 1.1 1998/12/29 18:06:48 mickey Exp $	*/


#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: sys_machdep.c,v 1.7 2007/12/20 23:02:39 dsl Exp $");

#include <sys/param.h>
#include <sys/systm.h>

#include <sys/mount.h>
#include <sys/syscallargs.h>

int
sys_sysarch(struct lwp *l, const struct sys_sysarch_args *uap, register_t *retval)
{
	/* {
		syscallarg(int) op;
		syscallarg(char *) parms;
	} */
	int error = 0;

	switch (SCARG(uap, op)) {
	default:
		error = EINVAL;
		break;
	}
	return (error);
}
