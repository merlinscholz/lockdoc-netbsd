/*
 * Copyright (c) 1997 Erez Zadok
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
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
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      %W% (Berkeley) %G%
 *
 * $Id: misc_rpc.c,v 1.1.1.1 1997/07/24 21:20:07 christos Exp $
 *
 */

/*
 * Additions to Sun RPC.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */
#include <am_defs.h>
#include <amu.h>

/*
 * Some systems renamed _seterr_reply to __seterr_reply (with two
 * leading underscores)
 */
#if !defined(HAVE__SETERR_REPLY) && defined(HAVE___SETERR_REPLY)
# define _seterr_reply	__seterr_reply
#endif /* !defined(HAVE__SETERR_REPLY) && defined(HAVE___SETERR_REPLY) */


void
rpc_msg_init(struct rpc_msg *mp, u_long prog, u_long vers, u_long proc)
{
  /*
   * Initialise the message
   */
  memset((voidp) mp, 0, sizeof(*mp));
  mp->rm_xid = 0;
  mp->rm_direction = CALL;
  mp->rm_call.cb_rpcvers = RPC_MSG_VERSION;
  mp->rm_call.cb_prog = prog;
  mp->rm_call.cb_vers = vers;
  mp->rm_call.cb_proc = proc;
}


/*
 * Field reply to call to mountd
 */
int
pickup_rpc_reply(voidp pkt, int len, voidp where, XDRPROC_T_TYPE where_xdr)
{
  XDR reply_xdr;
  int ok;
  struct rpc_err err;
  struct rpc_msg reply_msg;
  int error = 0;

  /* memset((voidp) &err, 0, sizeof(err)); */
  memset((voidp) &reply_msg, 0, sizeof(reply_msg));
  memset((voidp) &reply_xdr, 0, sizeof(reply_xdr));

  reply_msg.acpted_rply.ar_results.where = (caddr_t) where;
  reply_msg.acpted_rply.ar_results.proc = where_xdr;

  xdrmem_create(&reply_xdr, pkt, len, XDR_DECODE);

  ok = xdr_replymsg(&reply_xdr, &reply_msg);
  if (!ok) {
    error = EIO;
    goto drop;
  }
  _seterr_reply(&reply_msg, &err);
  if (err.re_status != RPC_SUCCESS) {
    error = EIO;
    goto drop;
  }

drop:
  if (reply_msg.rm_reply.rp_stat == MSG_ACCEPTED &&
      reply_msg.acpted_rply.ar_verf.oa_base) {
    reply_xdr.x_op = XDR_FREE;
    (void) xdr_opaque_auth(&reply_xdr,
			   &reply_msg.acpted_rply.ar_verf);
  }
  xdr_destroy(&reply_xdr);

  return error;
}


int
make_rpc_packet(char *buf, int buflen, u_long proc, struct rpc_msg *mp, voidp arg, XDRPROC_T_TYPE arg_xdr, AUTH *auth)
{
  XDR msg_xdr;
  int len;

  xdrmem_create(&msg_xdr, buf, buflen, XDR_ENCODE);

  /*
   * Basic protocol header
   */
  if (!xdr_callhdr(&msg_xdr, mp))
    return -EIO;

  /*
   * Called procedure number
   */
  if (!xdr_enum(&msg_xdr, (enum_t *) & proc))
    return -EIO;

  /*
   * Authorization
   */
  if (!AUTH_MARSHALL(auth, &msg_xdr))
    return -EIO;

  /*
   * Arguments
   */
  if (!(*arg_xdr) (&msg_xdr, arg))
    return -EIO;

  /*
   * Determine length
   */
  len = xdr_getpos(&msg_xdr);

  /*
   * Throw away xdr
   */
  xdr_destroy(&msg_xdr);

  return len;
}
