/*	$NetBSD: savedir.h,v 1.1.1.1 2003/01/26 23:15:13 wiz Exp $	*/

#if !defined SAVEDIR_H_
# define SAVEDIR_H_

#include "exclude.h"

# ifndef PARAMS
#  if defined PROTOTYPES || (defined __STDC__ && __STDC__)
#   define PARAMS(Args) Args
#  else
#   define PARAMS(Args) ()
#  endif
# endif

extern char *
savedir PARAMS ((const char *dir, off_t name_size,
		 struct exclude *, struct exclude *));

#endif
