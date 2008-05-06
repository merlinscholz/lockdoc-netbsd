/*	$NetBSD: env.c,v 1.2 2008/05/06 16:09:18 dyoung Exp $	*/

/*-
 * Copyright (c)2008 David Young.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <util.h>

#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "env.h"
#include "util.h"

prop_dictionary_t
prop_dictionary_augment(prop_dictionary_t bottom, prop_dictionary_t top)
{
	prop_object_iterator_t i;
	prop_dictionary_t d;
	prop_object_t ko, o;
	prop_dictionary_keysym_t k;
	const char *key;

	d = prop_dictionary_copy_mutable(bottom);

	i = prop_dictionary_iterator(top);

	while ((ko = prop_object_iterator_next(i)) != NULL) {
		k = (prop_dictionary_keysym_t)ko;
		key = prop_dictionary_keysym_cstring_nocopy(k);
		o = prop_dictionary_get_keysym(top, k);
		if (o == NULL || !prop_dictionary_set(d, key, o)) {
			prop_object_release((prop_object_t)d);
			d = NULL;
			break;
		}
	}
	prop_object_iterator_release(i);
	prop_dictionary_make_immutable(d);
	return d;
}

int
getifflags(prop_dictionary_t env, prop_dictionary_t oenv,
    unsigned short *flagsp)
{
	struct ifreq ifr;
	prop_number_t num;
	const char *ifname;
	int s;

	num = (prop_number_t)prop_dictionary_get(env, "ifflags");

	if (num != NULL) {
		*flagsp = (unsigned short)prop_number_integer_value(num);
		return 0;
	}

	if ((s = getsock(AF_UNSPEC)) == -1)
		return -1;

	if ((ifname = getifname(env)) == NULL)
		return -1;

	memset(&ifr, 0, sizeof(ifr));
	estrlcpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));
	if (ioctl(s, SIOCGIFFLAGS, &ifr) == -1)
		return -1;

	*flagsp = (unsigned short)ifr.ifr_flags;

	num = prop_number_create_unsigned_integer(
	    (unsigned short)ifr.ifr_flags);
	(void)prop_dictionary_set(oenv, "ifflags", num);
	prop_object_release((prop_object_t)num);

	return 0;
}

const char *
getifinfo(prop_dictionary_t env, prop_dictionary_t oenv, unsigned short *flagsp)
{
	if (getifflags(env, oenv, flagsp) == -1)
		return NULL;

	return getifname(env);
}

const char *
getifname(prop_dictionary_t env)
{
	prop_string_t str;

	str = (prop_string_t)prop_dictionary_get(env, "if");
	if (str == NULL)
		return NULL;
	return prop_string_cstring_nocopy(str);
}

ssize_t
getargdata(prop_dictionary_t env, const char *key, uint8_t *buf, size_t buflen)
{
	prop_data_t data;
	size_t datalen;

	data = (prop_data_t)prop_dictionary_get(env, key);
	if (data == NULL) {
		errno = ENOENT;
		return -1;
	}
	datalen = prop_data_size(data);
	if (datalen > buflen) {
		errno = ENAMETOOLONG; 
		return -1;
	}
	memset(buf, 0, buflen);
	memcpy(buf, prop_data_data_nocopy(data), datalen);
	return datalen;
}

ssize_t
getargstr(prop_dictionary_t env, const char *key, char *buf, size_t buflen)
{
	prop_data_t data;
	size_t datalen;

	data = (prop_data_t)prop_dictionary_get(env, "bssid");
	if (data == NULL) {
		errno = ENOENT;
		return -1;
	}
	datalen = prop_data_size(data);
	if (datalen >= buflen) {
		errno = ENAMETOOLONG; 
		return -1;
	}
	memset(buf, 0, buflen);
	memcpy(buf, prop_data_data_nocopy(data), datalen);
	return datalen;
}

int
getaf(prop_dictionary_t env)
{
	prop_number_t num;

	num = (prop_number_t)prop_dictionary_get(env, "af");
	if (num == NULL) {
		errno = ENOENT;
		return -1;
	}
	return (int)prop_number_integer_value(num);
}
