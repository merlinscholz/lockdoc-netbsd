/*	$NetBSD: mach_namemap.c,v 1.25 2003/04/30 07:32:17 manu Exp $ */

/*-
 * Copyright (c) 2002-2003 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Emmanuel Dreyfus
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *        This product includes software developed by the NetBSD
 *        Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: mach_namemap.c,v 1.25 2003/04/30 07:32:17 manu Exp $");

#include <sys/types.h>
#include <sys/param.h>

#include <compat/mach/mach_types.h>
#include <compat/mach/mach_message.h>
#include <compat/mach/mach_bootstrap.h>
#include <compat/mach/mach_iokit.h>
#include <compat/mach/mach_clock.h>
#include <compat/mach/mach_host.h>
#include <compat/mach/mach_port.h>
#include <compat/mach/mach_task.h>
#include <compat/mach/mach_thread.h>
#include <compat/mach/mach_semaphore.h>
#include <compat/mach/mach_vm.h>

struct mach_subsystem_namemap mach_namemap[] = {
	{ 65, NULL, "notify_port_deleted" },
	{ 69, NULL, "notify_port_destroyed" },
	{ 70, NULL, "notify_no_senders" },
	{ 71, NULL, "notify_send_once" },
	{ 72, NULL, "notify_dead_name" },
	{ 200, mach_host_info, "host_info" },
	{ 202, mach_host_page_size, "host_page_size" },
	{ 205, mach_host_get_io_master, "host_get_io_master" },
	{ 206, mach_host_get_clock_service, "host_get_clock_service" },
/*	{ 403, mach_boostrap_register, "boostrap_register" }, */
 	{ 404, mach_bootstrap_look_up, "bootstrap_look_up" }, 
	{ 1000, mach_clock_get_time, "clock_get_time" },
	{ 2800, mach_io_object_get_class, "io_object_get_class" },
	{ 2801, mach_io_object_conforms_to, "io_object_conforms_to" },
	{ 2802, mach_io_iterator_next, "io_iterator_next" },
	{ 2803, mach_io_iterator_reset, "io_iterator_reset" },
	{ 2804, mach_io_service_get_matching_services, 
	    "io_service_get_matching_services" },
	{ 2805, mach_io_registry_entry_get_property,
	    "io_registry_entry_get_property" },
	{ 2811, mach_io_registry_entry_get_properties,
	    "io_registry_entry_get_properties" },
	{ 2813, mach_io_registry_entry_get_child_iterator,
	    "io_registry_entry_get_child_iterator" },
	{ 2815, mach_io_service_open, "io_service_open" },
	{ 2817, mach_io_connect_get_service, "io_connect_get_service" },
	{ 2818, mach_io_connect_set_notification_port,
	    "io_connect_set_notification_port" },
	{ 2819, mach_io_connect_map_memory, "io_connect_map_memory" },
	{ 2822, mach_io_connect_method_scalari_scalaro,
	    "io_connect_method_scalari_scalaro" },
	{ 2826, mach_io_registry_entry_get_path,
	    "io_registry_entry_get_path" },
	{ 2827, mach_io_registry_get_root_entry, 
	    "io_registry_get_root_entry" },
	{ 2833, mach_io_registry_entry_create_iterator,
	    "io_registry_entry_create_iterator" },
	{ 2843, mach_io_registry_entry_get_name_in_plane,
	    "io_registry_entry_get_name_in_plane" },
	{ 2850, mach_io_service_add_interest_notification,
	    "io_service_add_interest_notification" },
	{ 2854, mach_io_registry_entry_get_location_in_plane,
	    "io_registry_entry_get_location_in_plane" },
	{ 3201, mach_port_type, "port_type" },
	{ 3204, mach_port_allocate, "port_allocate" },
	{ 3205, mach_port_destroy, "port_destroy" },
	{ 3206, mach_port_deallocate, "port_deallocate" },
	{ 3212, mach_port_move_member, "port_move_member" },
	{ 3213, mach_port_request_notification, "port_request_notification" },
	{ 3214, mach_port_insert_right, "port_insert_right" },
	{ 3218, mach_port_set_attributes, "port_set_attributes" },
	{ 3226, mach_port_insert_member, "port_insert_member" },
	{ 3402, mach_task_threads, "task_threads" },
	{ 3404, mach_ports_lookup, "ports_lookup" },
	{ 3405, mach_task_info, "task_info" },
	{ 3407, mach_task_suspend, "task_suspend" },
	{ 3408, mach_task_resume, "task_resume" },
	{ 3409, mach_task_get_special_port, "task_get_special_port" },
	{ 3410, mach_task_set_special_port, "task_set_special_port" },
	{ 3412, mach_thread_create_running, "thread_create_running" },
	{ 3413, mach_task_set_exception_ports, "task_set_exception_ports" },
	{ 3414, mach_task_get_exception_ports, "task_get_exception_ports" },
	{ 3418, mach_semaphore_create, "semaphore_create" },
	{ 3419, mach_semaphore_destroy, "semaphore_destroy" },
	{ 3616, mach_thread_policy, "thread_policy" },
	{ 3800, mach_vm_region, "vm_region" },
	{ 3801, mach_vm_allocate, "vm_allocate" },
	{ 3802, mach_vm_deallocate, "vm_deallocate" },
	{ 3803, mach_vm_protect, "vm_protect" },
	{ 3804, mach_vm_inherit, "vm_inherit" },
	{ 3810, mach_vm_msync, "vm_msync" },
	{ 3812, mach_vm_map, "vm_map" },
	{ 3825, mach_vm_make_memory_entry, "vm_make_memory_entry" },
	{ 0, NULL, NULL },
};


