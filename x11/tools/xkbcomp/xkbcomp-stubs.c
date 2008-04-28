/*	$NetBSD: xkbcomp-stubs.c,v 1.3 2008/04/28 20:24:18 martin Exp $	*/

/*-
 * Copyright (c) 2003-2004 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Luke Mewburn.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
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

#include <stdio.h>

#include "Xlibint.h"
#include "Xlcint.h"
#include "XKBlibint.h"
#include <X11/extensions/XKBfile.h>

Display *
XOpenDisplay(const char *display)
{
	return NULL;
}

int
XCloseDisplay(Display *dpy)
{
	return 0;
}

int (*
XSynchronize(Display *dpy, int onoff))()
{
	return NULL;
}

XrmMethods
_XrmInitParseInfo(XPointer *state)
{
	return NULL;
}

int
XGetErrorText(Display *dpy, int code, char *buffer, int nbytes)
{
	return 0;
}


char *
XGetAtomName(Display *dpy, Atom atom)
{
	return NULL;
}

Atom
XInternAtom(Display *dpy, const char *name, Bool onlyIfExists)
{
	return None;
}

XkbDescPtr
XkbGetMap(Display *dpy,unsigned which,unsigned deviceSpec)
{
	return NULL;
}

Status
XkbGetIndicatorMap(Display *dpy,unsigned long which,XkbDescPtr xkb)
{
	return BadValue;
}

Status
XkbGetControls(Display *dpy, unsigned long which, XkbDescPtr xkb)
{
	return BadValue;
}

Status
XkbGetCompatMap(Display *dpy,unsigned which,XkbDescPtr xkb)
{
	return BadValue;
}

Status
XkbGetNames(Display *dpy,unsigned which,XkbDescPtr xkb)
{
	return BadValue;
}

Status
XkbChangeKbdDisplay(Display *newDpy,XkbFileInfo *result)
{
	return BadValue;
}

Bool
XkbWriteToServer(XkbFileInfo *result)
{
	return False;
}

void
_XFlush(Display *dpy)
{
}

Bool
XkbUseExtension(Display *dpy,int *major_rtrn,int *minor_rtrn)
{
	return False;
}

Status
_XReply(Display *dpy, xReply *rep, int extra, Bool discard)
{
	return False;
}

int
_XRead(Display *dpy, char *data, long size)
{
	return 0;
}
