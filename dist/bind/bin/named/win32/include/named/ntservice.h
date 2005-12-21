/*	$NetBSD: ntservice.h,v 1.1.1.3 2005/12/21 23:08:12 christos Exp $	*/

/*
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2003  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: ntservice.h,v 1.1.14.3 2004/03/08 04:04:22 marka Exp */

#ifndef NTSERVICE_H
#define NTSERVICE_H

#include <winsvc.h>

#define BIND_DISPLAY_NAME "ISC BIND"
#define BIND_SERVICE_NAME "named"

void
ntservice_init();
void UpdateSCM(DWORD);
void ServiceControl(DWORD dwCtrlCode);
void 
ntservice_shutdown();
BOOL ntservice_isservice();
#endif
