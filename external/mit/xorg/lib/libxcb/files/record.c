/*
 * This file generated automatically from record.xml by c_client.py.
 * Edit at your peril.
 */

#include <string.h>
#include <assert.h>
#include "xcbext.h"
#include "record.h"

xcb_extension_t xcb_record_id = { "RECORD", 0 };


/*****************************************************************************
 **
 ** void xcb_record_context_next
 ** 
 ** @param xcb_record_context_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_context_next (xcb_record_context_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_context_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_context_end
 ** 
 ** @param xcb_record_context_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_context_end (xcb_record_context_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** void xcb_record_range_8_next
 ** 
 ** @param xcb_record_range_8_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_range_8_next (xcb_record_range_8_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_range_8_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_range_8_end
 ** 
 ** @param xcb_record_range_8_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_range_8_end (xcb_record_range_8_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** void xcb_record_range_16_next
 ** 
 ** @param xcb_record_range_16_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_range_16_next (xcb_record_range_16_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_range_16_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_range_16_end
 ** 
 ** @param xcb_record_range_16_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_range_16_end (xcb_record_range_16_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** void xcb_record_ext_range_next
 ** 
 ** @param xcb_record_ext_range_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_ext_range_next (xcb_record_ext_range_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_ext_range_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_ext_range_end
 ** 
 ** @param xcb_record_ext_range_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_ext_range_end (xcb_record_ext_range_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** void xcb_record_range_next
 ** 
 ** @param xcb_record_range_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_range_next (xcb_record_range_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_range_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_range_end
 ** 
 ** @param xcb_record_range_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_range_end (xcb_record_range_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** void xcb_record_element_header_next
 ** 
 ** @param xcb_record_element_header_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_element_header_next (xcb_record_element_header_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_element_header_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_element_header_end
 ** 
 ** @param xcb_record_element_header_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_element_header_end (xcb_record_element_header_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** void xcb_record_client_spec_next
 ** 
 ** @param xcb_record_client_spec_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_client_spec_next (xcb_record_client_spec_iterator_t *i  /**< */)
{
    --i->rem;
    ++i->data;
    i->index += sizeof(xcb_record_client_spec_t);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_client_spec_end
 ** 
 ** @param xcb_record_client_spec_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_client_spec_end (xcb_record_client_spec_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    ret.data = i.data + i.rem;
    ret.index = i.index + ((char *) ret.data - (char *) i.data);
    ret.rem = 0;
    return ret;
}


/*****************************************************************************
 **
 ** xcb_record_range_t * xcb_record_client_info_ranges
 ** 
 ** @param const xcb_record_client_info_t *R
 ** @returns xcb_record_range_t *
 **
 *****************************************************************************/
 
xcb_record_range_t *
xcb_record_client_info_ranges (const xcb_record_client_info_t *R  /**< */)
{
    return (xcb_record_range_t *) (R + 1);
}


/*****************************************************************************
 **
 ** int xcb_record_client_info_ranges_length
 ** 
 ** @param const xcb_record_client_info_t *R
 ** @returns int
 **
 *****************************************************************************/
 
int
xcb_record_client_info_ranges_length (const xcb_record_client_info_t *R  /**< */)
{
    return R->num_ranges;
}


/*****************************************************************************
 **
 ** xcb_record_range_iterator_t xcb_record_client_info_ranges_iterator
 ** 
 ** @param const xcb_record_client_info_t *R
 ** @returns xcb_record_range_iterator_t
 **
 *****************************************************************************/
 
xcb_record_range_iterator_t
xcb_record_client_info_ranges_iterator (const xcb_record_client_info_t *R  /**< */)
{
    xcb_record_range_iterator_t i;
    i.data = (xcb_record_range_t *) (R + 1);
    i.rem = R->num_ranges;
    i.index = (char *) i.data - (char *) R;
    return i;
}


/*****************************************************************************
 **
 ** void xcb_record_client_info_next
 ** 
 ** @param xcb_record_client_info_iterator_t *i
 ** @returns void
 **
 *****************************************************************************/
 
void
xcb_record_client_info_next (xcb_record_client_info_iterator_t *i  /**< */)
{
    xcb_record_client_info_t *R = i->data;
    xcb_generic_iterator_t child = xcb_record_range_end(xcb_record_client_info_ranges_iterator(R));
    --i->rem;
    i->data = (xcb_record_client_info_t *) child.data;
    i->index = child.index;
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_client_info_end
 ** 
 ** @param xcb_record_client_info_iterator_t i
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_client_info_end (xcb_record_client_info_iterator_t i  /**< */)
{
    xcb_generic_iterator_t ret;
    while(i.rem > 0)
        xcb_record_client_info_next(&i);
    ret.data = i.data;
    ret.rem = i.rem;
    ret.index = i.index;
    return ret;
}


/*****************************************************************************
 **
 ** xcb_record_query_version_cookie_t xcb_record_query_version
 ** 
 ** @param xcb_connection_t *c
 ** @param uint16_t          major_version
 ** @param uint16_t          minor_version
 ** @returns xcb_record_query_version_cookie_t
 **
 *****************************************************************************/
 
xcb_record_query_version_cookie_t
xcb_record_query_version (xcb_connection_t *c  /**< */,
                          uint16_t          major_version  /**< */,
                          uint16_t          minor_version  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_QUERY_VERSION,
        /* isvoid */ 0
    };
    
    struct iovec xcb_parts[4];
    xcb_record_query_version_cookie_t xcb_ret;
    xcb_record_query_version_request_t xcb_out;
    
    xcb_out.major_version = major_version;
    xcb_out.minor_version = minor_version;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_record_query_version_cookie_t xcb_record_query_version_unchecked
 ** 
 ** @param xcb_connection_t *c
 ** @param uint16_t          major_version
 ** @param uint16_t          minor_version
 ** @returns xcb_record_query_version_cookie_t
 **
 *****************************************************************************/
 
xcb_record_query_version_cookie_t
xcb_record_query_version_unchecked (xcb_connection_t *c  /**< */,
                                    uint16_t          major_version  /**< */,
                                    uint16_t          minor_version  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_QUERY_VERSION,
        /* isvoid */ 0
    };
    
    struct iovec xcb_parts[4];
    xcb_record_query_version_cookie_t xcb_ret;
    xcb_record_query_version_request_t xcb_out;
    
    xcb_out.major_version = major_version;
    xcb_out.minor_version = minor_version;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_record_query_version_reply_t * xcb_record_query_version_reply
 ** 
 ** @param xcb_connection_t                   *c
 ** @param xcb_record_query_version_cookie_t   cookie
 ** @param xcb_generic_error_t               **e
 ** @returns xcb_record_query_version_reply_t *
 **
 *****************************************************************************/
 
xcb_record_query_version_reply_t *
xcb_record_query_version_reply (xcb_connection_t                   *c  /**< */,
                                xcb_record_query_version_cookie_t   cookie  /**< */,
                                xcb_generic_error_t               **e  /**< */)
{
    return (xcb_record_query_version_reply_t *) xcb_wait_for_reply(c, cookie.sequence, e);
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_create_context_checked
 ** 
 ** @param xcb_connection_t               *c
 ** @param xcb_record_context_t            context
 ** @param xcb_record_element_header_t     element_header
 ** @param uint32_t                        num_client_specs
 ** @param uint32_t                        num_ranges
 ** @param const xcb_record_client_spec_t *client_specs
 ** @param const xcb_record_range_t       *ranges
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_create_context_checked (xcb_connection_t               *c  /**< */,
                                   xcb_record_context_t            context  /**< */,
                                   xcb_record_element_header_t     element_header  /**< */,
                                   uint32_t                        num_client_specs  /**< */,
                                   uint32_t                        num_ranges  /**< */,
                                   const xcb_record_client_spec_t *client_specs  /**< */,
                                   const xcb_record_range_t       *ranges  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 6,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_CREATE_CONTEXT,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[8];
    xcb_void_cookie_t xcb_ret;
    xcb_record_create_context_request_t xcb_out;
    
    xcb_out.context = context;
    xcb_out.element_header = element_header;
    memset(xcb_out.pad0, 0, 3);
    xcb_out.num_client_specs = num_client_specs;
    xcb_out.num_ranges = num_ranges;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_parts[4].iov_base = (char *) client_specs;
    xcb_parts[4].iov_len = num_client_specs * sizeof(xcb_record_client_spec_t);
    xcb_parts[5].iov_base = 0;
    xcb_parts[5].iov_len = -xcb_parts[4].iov_len & 3;
    xcb_parts[6].iov_base = (char *) ranges;
    xcb_parts[6].iov_len = num_ranges * sizeof(xcb_record_range_t);
    xcb_parts[7].iov_base = 0;
    xcb_parts[7].iov_len = -xcb_parts[6].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_create_context
 ** 
 ** @param xcb_connection_t               *c
 ** @param xcb_record_context_t            context
 ** @param xcb_record_element_header_t     element_header
 ** @param uint32_t                        num_client_specs
 ** @param uint32_t                        num_ranges
 ** @param const xcb_record_client_spec_t *client_specs
 ** @param const xcb_record_range_t       *ranges
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_create_context (xcb_connection_t               *c  /**< */,
                           xcb_record_context_t            context  /**< */,
                           xcb_record_element_header_t     element_header  /**< */,
                           uint32_t                        num_client_specs  /**< */,
                           uint32_t                        num_ranges  /**< */,
                           const xcb_record_client_spec_t *client_specs  /**< */,
                           const xcb_record_range_t       *ranges  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 6,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_CREATE_CONTEXT,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[8];
    xcb_void_cookie_t xcb_ret;
    xcb_record_create_context_request_t xcb_out;
    
    xcb_out.context = context;
    xcb_out.element_header = element_header;
    memset(xcb_out.pad0, 0, 3);
    xcb_out.num_client_specs = num_client_specs;
    xcb_out.num_ranges = num_ranges;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_parts[4].iov_base = (char *) client_specs;
    xcb_parts[4].iov_len = num_client_specs * sizeof(xcb_record_client_spec_t);
    xcb_parts[5].iov_base = 0;
    xcb_parts[5].iov_len = -xcb_parts[4].iov_len & 3;
    xcb_parts[6].iov_base = (char *) ranges;
    xcb_parts[6].iov_len = num_ranges * sizeof(xcb_record_range_t);
    xcb_parts[7].iov_base = 0;
    xcb_parts[7].iov_len = -xcb_parts[6].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_register_clients_checked
 ** 
 ** @param xcb_connection_t               *c
 ** @param xcb_record_context_t            context
 ** @param xcb_record_element_header_t     element_header
 ** @param uint32_t                        num_client_specs
 ** @param uint32_t                        num_ranges
 ** @param const xcb_record_client_spec_t *client_specs
 ** @param const xcb_record_range_t       *ranges
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_register_clients_checked (xcb_connection_t               *c  /**< */,
                                     xcb_record_context_t            context  /**< */,
                                     xcb_record_element_header_t     element_header  /**< */,
                                     uint32_t                        num_client_specs  /**< */,
                                     uint32_t                        num_ranges  /**< */,
                                     const xcb_record_client_spec_t *client_specs  /**< */,
                                     const xcb_record_range_t       *ranges  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 6,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_REGISTER_CLIENTS,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[8];
    xcb_void_cookie_t xcb_ret;
    xcb_record_register_clients_request_t xcb_out;
    
    xcb_out.context = context;
    xcb_out.element_header = element_header;
    memset(xcb_out.pad0, 0, 3);
    xcb_out.num_client_specs = num_client_specs;
    xcb_out.num_ranges = num_ranges;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_parts[4].iov_base = (char *) client_specs;
    xcb_parts[4].iov_len = num_client_specs * sizeof(xcb_record_client_spec_t);
    xcb_parts[5].iov_base = 0;
    xcb_parts[5].iov_len = -xcb_parts[4].iov_len & 3;
    xcb_parts[6].iov_base = (char *) ranges;
    xcb_parts[6].iov_len = num_ranges * sizeof(xcb_record_range_t);
    xcb_parts[7].iov_base = 0;
    xcb_parts[7].iov_len = -xcb_parts[6].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_register_clients
 ** 
 ** @param xcb_connection_t               *c
 ** @param xcb_record_context_t            context
 ** @param xcb_record_element_header_t     element_header
 ** @param uint32_t                        num_client_specs
 ** @param uint32_t                        num_ranges
 ** @param const xcb_record_client_spec_t *client_specs
 ** @param const xcb_record_range_t       *ranges
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_register_clients (xcb_connection_t               *c  /**< */,
                             xcb_record_context_t            context  /**< */,
                             xcb_record_element_header_t     element_header  /**< */,
                             uint32_t                        num_client_specs  /**< */,
                             uint32_t                        num_ranges  /**< */,
                             const xcb_record_client_spec_t *client_specs  /**< */,
                             const xcb_record_range_t       *ranges  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 6,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_REGISTER_CLIENTS,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[8];
    xcb_void_cookie_t xcb_ret;
    xcb_record_register_clients_request_t xcb_out;
    
    xcb_out.context = context;
    xcb_out.element_header = element_header;
    memset(xcb_out.pad0, 0, 3);
    xcb_out.num_client_specs = num_client_specs;
    xcb_out.num_ranges = num_ranges;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_parts[4].iov_base = (char *) client_specs;
    xcb_parts[4].iov_len = num_client_specs * sizeof(xcb_record_client_spec_t);
    xcb_parts[5].iov_base = 0;
    xcb_parts[5].iov_len = -xcb_parts[4].iov_len & 3;
    xcb_parts[6].iov_base = (char *) ranges;
    xcb_parts[6].iov_len = num_ranges * sizeof(xcb_record_range_t);
    xcb_parts[7].iov_base = 0;
    xcb_parts[7].iov_len = -xcb_parts[6].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_unregister_clients_checked
 ** 
 ** @param xcb_connection_t               *c
 ** @param xcb_record_context_t            context
 ** @param uint32_t                        num_client_specs
 ** @param const xcb_record_client_spec_t *client_specs
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_unregister_clients_checked (xcb_connection_t               *c  /**< */,
                                       xcb_record_context_t            context  /**< */,
                                       uint32_t                        num_client_specs  /**< */,
                                       const xcb_record_client_spec_t *client_specs  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 4,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_UNREGISTER_CLIENTS,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[6];
    xcb_void_cookie_t xcb_ret;
    xcb_record_unregister_clients_request_t xcb_out;
    
    xcb_out.context = context;
    xcb_out.num_client_specs = num_client_specs;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_parts[4].iov_base = (char *) client_specs;
    xcb_parts[4].iov_len = num_client_specs * sizeof(xcb_record_client_spec_t);
    xcb_parts[5].iov_base = 0;
    xcb_parts[5].iov_len = -xcb_parts[4].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_unregister_clients
 ** 
 ** @param xcb_connection_t               *c
 ** @param xcb_record_context_t            context
 ** @param uint32_t                        num_client_specs
 ** @param const xcb_record_client_spec_t *client_specs
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_unregister_clients (xcb_connection_t               *c  /**< */,
                               xcb_record_context_t            context  /**< */,
                               uint32_t                        num_client_specs  /**< */,
                               const xcb_record_client_spec_t *client_specs  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 4,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_UNREGISTER_CLIENTS,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[6];
    xcb_void_cookie_t xcb_ret;
    xcb_record_unregister_clients_request_t xcb_out;
    
    xcb_out.context = context;
    xcb_out.num_client_specs = num_client_specs;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_parts[4].iov_base = (char *) client_specs;
    xcb_parts[4].iov_len = num_client_specs * sizeof(xcb_record_client_spec_t);
    xcb_parts[5].iov_base = 0;
    xcb_parts[5].iov_len = -xcb_parts[4].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_record_get_context_cookie_t xcb_record_get_context
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_record_get_context_cookie_t
 **
 *****************************************************************************/
 
xcb_record_get_context_cookie_t
xcb_record_get_context (xcb_connection_t     *c  /**< */,
                        xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_GET_CONTEXT,
        /* isvoid */ 0
    };
    
    struct iovec xcb_parts[4];
    xcb_record_get_context_cookie_t xcb_ret;
    xcb_record_get_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_record_get_context_cookie_t xcb_record_get_context_unchecked
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_record_get_context_cookie_t
 **
 *****************************************************************************/
 
xcb_record_get_context_cookie_t
xcb_record_get_context_unchecked (xcb_connection_t     *c  /**< */,
                                  xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_GET_CONTEXT,
        /* isvoid */ 0
    };
    
    struct iovec xcb_parts[4];
    xcb_record_get_context_cookie_t xcb_ret;
    xcb_record_get_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** int xcb_record_get_context_intercepted_clients_length
 ** 
 ** @param const xcb_record_get_context_reply_t *R
 ** @returns int
 **
 *****************************************************************************/
 
int
xcb_record_get_context_intercepted_clients_length (const xcb_record_get_context_reply_t *R  /**< */)
{
    return R->num_intercepted_clients;
}


/*****************************************************************************
 **
 ** xcb_record_client_info_iterator_t xcb_record_get_context_intercepted_clients_iterator
 ** 
 ** @param const xcb_record_get_context_reply_t *R
 ** @returns xcb_record_client_info_iterator_t
 **
 *****************************************************************************/
 
xcb_record_client_info_iterator_t
xcb_record_get_context_intercepted_clients_iterator (const xcb_record_get_context_reply_t *R  /**< */)
{
    xcb_record_client_info_iterator_t i;
    i.data = (xcb_record_client_info_t *) (R + 1);
    i.rem = R->num_intercepted_clients;
    i.index = (char *) i.data - (char *) R;
    return i;
}


/*****************************************************************************
 **
 ** xcb_record_get_context_reply_t * xcb_record_get_context_reply
 ** 
 ** @param xcb_connection_t                 *c
 ** @param xcb_record_get_context_cookie_t   cookie
 ** @param xcb_generic_error_t             **e
 ** @returns xcb_record_get_context_reply_t *
 **
 *****************************************************************************/
 
xcb_record_get_context_reply_t *
xcb_record_get_context_reply (xcb_connection_t                 *c  /**< */,
                              xcb_record_get_context_cookie_t   cookie  /**< */,
                              xcb_generic_error_t             **e  /**< */)
{
    return (xcb_record_get_context_reply_t *) xcb_wait_for_reply(c, cookie.sequence, e);
}


/*****************************************************************************
 **
 ** xcb_record_enable_context_cookie_t xcb_record_enable_context
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_record_enable_context_cookie_t
 **
 *****************************************************************************/
 
xcb_record_enable_context_cookie_t
xcb_record_enable_context (xcb_connection_t     *c  /**< */,
                           xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_ENABLE_CONTEXT,
        /* isvoid */ 0
    };
    
    struct iovec xcb_parts[4];
    xcb_record_enable_context_cookie_t xcb_ret;
    xcb_record_enable_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_record_enable_context_cookie_t xcb_record_enable_context_unchecked
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_record_enable_context_cookie_t
 **
 *****************************************************************************/
 
xcb_record_enable_context_cookie_t
xcb_record_enable_context_unchecked (xcb_connection_t     *c  /**< */,
                                     xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_ENABLE_CONTEXT,
        /* isvoid */ 0
    };
    
    struct iovec xcb_parts[4];
    xcb_record_enable_context_cookie_t xcb_ret;
    xcb_record_enable_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** uint8_t * xcb_record_enable_context_data
 ** 
 ** @param const xcb_record_enable_context_reply_t *R
 ** @returns uint8_t *
 **
 *****************************************************************************/
 
uint8_t *
xcb_record_enable_context_data (const xcb_record_enable_context_reply_t *R  /**< */)
{
    return (uint8_t *) (R + 1);
}


/*****************************************************************************
 **
 ** int xcb_record_enable_context_data_length
 ** 
 ** @param const xcb_record_enable_context_reply_t *R
 ** @returns int
 **
 *****************************************************************************/
 
int
xcb_record_enable_context_data_length (const xcb_record_enable_context_reply_t *R  /**< */)
{
    return (R->length * 4);
}


/*****************************************************************************
 **
 ** xcb_generic_iterator_t xcb_record_enable_context_data_end
 ** 
 ** @param const xcb_record_enable_context_reply_t *R
 ** @returns xcb_generic_iterator_t
 **
 *****************************************************************************/
 
xcb_generic_iterator_t
xcb_record_enable_context_data_end (const xcb_record_enable_context_reply_t *R  /**< */)
{
    xcb_generic_iterator_t i;
    i.data = ((uint8_t *) (R + 1)) + ((R->length * 4));
    i.rem = 0;
    i.index = (char *) i.data - (char *) R;
    return i;
}


/*****************************************************************************
 **
 ** xcb_record_enable_context_reply_t * xcb_record_enable_context_reply
 ** 
 ** @param xcb_connection_t                    *c
 ** @param xcb_record_enable_context_cookie_t   cookie
 ** @param xcb_generic_error_t                **e
 ** @returns xcb_record_enable_context_reply_t *
 **
 *****************************************************************************/
 
xcb_record_enable_context_reply_t *
xcb_record_enable_context_reply (xcb_connection_t                    *c  /**< */,
                                 xcb_record_enable_context_cookie_t   cookie  /**< */,
                                 xcb_generic_error_t                **e  /**< */)
{
    return (xcb_record_enable_context_reply_t *) xcb_wait_for_reply(c, cookie.sequence, e);
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_disable_context_checked
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_disable_context_checked (xcb_connection_t     *c  /**< */,
                                    xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_DISABLE_CONTEXT,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_record_disable_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_disable_context
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_disable_context (xcb_connection_t     *c  /**< */,
                            xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_DISABLE_CONTEXT,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_record_disable_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_free_context_checked
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_free_context_checked (xcb_connection_t     *c  /**< */,
                                 xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_FREE_CONTEXT,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_record_free_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, XCB_REQUEST_CHECKED, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}


/*****************************************************************************
 **
 ** xcb_void_cookie_t xcb_record_free_context
 ** 
 ** @param xcb_connection_t     *c
 ** @param xcb_record_context_t  context
 ** @returns xcb_void_cookie_t
 **
 *****************************************************************************/
 
xcb_void_cookie_t
xcb_record_free_context (xcb_connection_t     *c  /**< */,
                         xcb_record_context_t  context  /**< */)
{
    static const xcb_protocol_request_t xcb_req = {
        /* count */ 2,
        /* ext */ &xcb_record_id,
        /* opcode */ XCB_RECORD_FREE_CONTEXT,
        /* isvoid */ 1
    };
    
    struct iovec xcb_parts[4];
    xcb_void_cookie_t xcb_ret;
    xcb_record_free_context_request_t xcb_out;
    
    xcb_out.context = context;
    
    xcb_parts[2].iov_base = (char *) &xcb_out;
    xcb_parts[2].iov_len = sizeof(xcb_out);
    xcb_parts[3].iov_base = 0;
    xcb_parts[3].iov_len = -xcb_parts[2].iov_len & 3;
    xcb_ret.sequence = xcb_send_request(c, 0, xcb_parts + 2, &xcb_req);
    return xcb_ret;
}

