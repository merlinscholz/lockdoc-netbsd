/*	$NetBSD: config.h,v 1.1.1.3 2005/12/21 23:08:10 christos Exp $	*/

/*
 * Copyright (C) 2004  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 2001, 2002  Internet Software Consortium.
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

/* Id: config.h,v 1.4.12.4 2004/04/20 14:12:10 marka Exp */

#ifndef NAMED_CONFIG_H
#define NAMED_CONFIG_H 1

#include <isccfg/cfg.h>

#include <dns/types.h>
#include <dns/zone.h>

isc_result_t
ns_config_parsedefaults(cfg_parser_t *parser, cfg_obj_t **conf);

isc_result_t
ns_config_get(cfg_obj_t **maps, const char* name, cfg_obj_t **obj);

isc_result_t
ns_checknames_get(cfg_obj_t **maps, const char* name, cfg_obj_t **obj);

int
ns_config_listcount(cfg_obj_t *list);

isc_result_t
ns_config_getclass(cfg_obj_t *classobj, dns_rdataclass_t defclass,
		   dns_rdataclass_t *classp);

isc_result_t
ns_config_gettype(cfg_obj_t *typeobj, dns_rdatatype_t deftype,
		  dns_rdatatype_t *typep);

dns_zonetype_t
ns_config_getzonetype(cfg_obj_t *zonetypeobj);

isc_result_t
ns_config_getiplist(cfg_obj_t *config, cfg_obj_t *list,
		    in_port_t defport, isc_mem_t *mctx,
		    isc_sockaddr_t **addrsp, isc_uint32_t *countp);

void
ns_config_putiplist(isc_mem_t *mctx, isc_sockaddr_t **addrsp,
		    isc_uint32_t count);

isc_result_t
ns_config_getipandkeylist(cfg_obj_t *config, cfg_obj_t *list, isc_mem_t *mctx,
			  isc_sockaddr_t **addrsp, dns_name_t ***keys,
			  isc_uint32_t *countp);

void
ns_config_putipandkeylist(isc_mem_t *mctx, isc_sockaddr_t **addrsp,
			  dns_name_t ***keys, isc_uint32_t count);

isc_result_t
ns_config_getport(cfg_obj_t *config, in_port_t *portp);

isc_result_t
ns_config_getkeyalgorithm(const char *str, dns_name_t **name);

#endif /* NAMED_CONFIG_H */
