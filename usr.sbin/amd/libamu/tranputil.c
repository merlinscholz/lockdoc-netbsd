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
 * $Id: tranputil.c,v 1.1.1.1 1997/07/24 21:20:10 christos Exp $
 *
 * Socket specific utilities.
 *      -Erez Zadok <ezk@cs.columbia.edu>
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif /* HAVE_CONFIG_H */
#include <am_defs.h>
#include <amu.h>


/*
 * find the IP address that can be used to connect to the local host
 */
void
amu_get_myaddress(struct in_addr *iap)
{
  struct sockaddr_in sin;

  memset((char *) &sin, 0, sizeof(sin));
  get_myaddress(&sin);
  iap->s_addr = sin.sin_addr.s_addr;
}


/*
 * How to bind to reserved ports.
 */
int
bind_resv_port(int so, u_short *pp)
{
  struct sockaddr_in sin;
  int rc;
  u_short port;

  memset((voidp) &sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;

  port = IPPORT_RESERVED;

  do {
    --port;
    sin.sin_port = htons(port);
    rc = bind(so, (struct sockaddr *) &sin, sizeof(sin));
  } while (rc < 0 && (int) port > IPPORT_RESERVED / 2);

  if (pp && rc == 0)
    *pp = port;

  return rc;
}

/*
 * close a descriptot, Sockets style
 */
int
amu_close(int fd)
{
  return close(fd);
}


/*
 * Create an rpc client attached to the mount daemon.
 */
CLIENT *
get_mount_client(char *unused_host, struct sockaddr_in *sin, struct timeval *tv, int *sock, u_long mnt_version)
{
  CLIENT *client;

  /*
   * First try a TCP socket
   */
  if ((*sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) > 0) {
    /*
     * Bind to a privileged port
     */
    if (bind_resv_port(*sock, (u_short *) 0) < 0)
      plog(XLOG_ERROR, "can't bind privileged port");

    /*
     * Find mountd port to connect to.
     * Connect to mountd.
     * Create a tcp client.
     */
    if ((sin->sin_port = htons(pmap_getport(sin, MOUNTPROG, mnt_version, IPPROTO_TCP))) != 0) {
      if (connect(*sock, (struct sockaddr *) sin, sizeof(*sin)) >= 0
	  && ((client = clnttcp_create(sin, MOUNTPROG, mnt_version, sock, 0, 0)) != NULL))
	return client;
    }
    /*
     * Failed so close socket
     */
    (void) close(*sock);
  }				/* tcp socket opened */
  /* TCP failed so try UDP */
  if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    plog(XLOG_ERROR, "Can't create socket to connect to mountd: %m");
    *sock = RPC_ANYSOCK;
    return NULL;
  }
  /*
   * Bind to a privileged port
   */
  if (bind_resv_port(*sock, (u_short *) 0) < 0)
    plog(XLOG_ERROR, "can't bind privileged port");

  /*
   * Zero out the port - make sure we recompute
   */
  sin->sin_port = 0;

  /*
   * Make a UDP client
   */
  if ((client = clntudp_create(sin, MOUNTPROG, mnt_version, *tv, sock)) == NULL) {
    (void) close(*sock);
    *sock = RPC_ANYSOCK;
    return NULL;
  }
#ifdef DEBUG
  dlog("get_mount_client: Using udp, port %d", sin->sin_port);
#endif /* DEBUG */
  return client;
}


/*
 * find the address of the caller of an RPC procedure.
 */
struct sockaddr_in *
amu_svc_getcaller(SVCXPRT *xprt)
{
  return svc_getcaller(xprt);
}


/*
 * Bind NFS to a reserved port.
 */
static int
bindnfs_port(int so, u_short *nfs_portp)
{
  u_short port;
  int error = bind_resv_port(so, &port);

  if (error == 0)
    *nfs_portp = port;
  return error;
}


/*
 * Create the nfs service for amd
 */
int
create_nfs_service(int *soNFSp, u_short *nfs_portp, SVCXPRT **nfs_xprtp, void (*dispatch_fxn)(struct svc_req *rqstp, SVCXPRT *transp))
{

  *soNFSp = socket(AF_INET, SOCK_DGRAM, 0);

  if (*soNFSp < 0 || bindnfs_port(*soNFSp, nfs_portp) < 0) {
    plog(XLOG_FATAL, "Can't create privileged nfs port");
    return 1;
  }

  if ((*nfs_xprtp = svcudp_create(*soNFSp)) == NULL) {
    plog(XLOG_FATAL, "cannot create rpc/udp service");
    return 2;
  }

  if (!svc_register(*nfs_xprtp, NFS_PROGRAM, NFS_VERSION, dispatch_fxn, 0)) {
    plog(XLOG_FATAL, "unable to register (NFS_PROGRAM, NFS_VERSION, 0)");
    return 3;
  }

  return 0;			/* all is well */
}


/*
 * Create the amq service for amd (both TCP and UDP)
 */
int
create_amq_service(int *udp_soAMQp, SVCXPRT **udp_amqpp, int *tcp_soAMQp, SVCXPRT **tcp_amqpp)
{
  /* first create TCP service */
  if (tcp_soAMQp) {
    *tcp_soAMQp = socket(AF_INET, SOCK_STREAM, 0);
    if (*tcp_soAMQp < 0) {
      plog(XLOG_FATAL, "cannot create tcp socket for amq service: %m");
      return 1;
    }

    /* now create RPC service handle for amq */
    if (tcp_amqpp &&
	(*tcp_amqpp = svctcp_create(*tcp_soAMQp, AMQ_SIZE, AMQ_SIZE)) == NULL) {
      plog(XLOG_FATAL, "cannot create tcp service for amq: soAMQp=%d", *tcp_soAMQp);
      return 2;
    }
  }

  /* next create UDP service */
  if (udp_soAMQp) {
    *udp_soAMQp = socket(AF_INET, SOCK_DGRAM, 0);
    if (*udp_soAMQp < 0) {
      plog(XLOG_FATAL, "cannot create udp socket for amq service: %m");
      return 3;
    }

    /* now create RPC service handle for amq */
    if (udp_amqpp &&
	(*udp_amqpp = svcudp_bufcreate(*udp_soAMQp, AMQ_SIZE, AMQ_SIZE)) == NULL) {
      plog(XLOG_FATAL, "cannot create udp service for amq: soAMQp=%d", *udp_soAMQp);
      return 4;
    }
  }

  return 0;			/* all is well */
}


/*
 * Find the best NFS version for a host and protocol.
 */
u_long
get_nfs_version(char *host, struct sockaddr_in *sin, u_long nfs_version, const char *proto)
{
  CLIENT *clnt;
  int again = 0;
  enum clnt_stat clnt_stat;
  struct timeval tv;
  int sock;

  /*
   * If not set or set wrong, then try from NFS_VERS_MAX on down. If
   * set, then try from nfs_version on down.
   */
  if (nfs_version <= 0 || nfs_version > NFS_VERS_MAX) {
    nfs_version = NFS_VERS_MAX;
    again = 1;
  }
  tv.tv_sec = 2;
  tv.tv_usec = 0;

try_again:

  sock = RPC_ANYSOCK;
  if (STREQ(proto, "tcp"))
    clnt = clnttcp_create(sin, NFS_PROGRAM, nfs_version, &sock, 0, 0);
  else if (STREQ(proto, "udp"))
    clnt = clntudp_create(sin, NFS_PROGRAM, nfs_version, tv, &sock);
  else
    clnt = NULL;

  if (clnt == NULL) {
    plog(XLOG_INFO, "get_nfs_version NFS(%d,%s) failed for %s :%s",
	 nfs_version, proto, host, clnt_spcreateerror(""));
    return 0;
  }

  /* Try a couple times to verify the CLIENT handle. */
  tv.tv_sec = 6;
  clnt_stat = clnt_call(clnt, NFSPROC_NULL, (xdrproc_t) xdr_void, 0,
			(xdrproc_t) xdr_void, 0, tv);
  close(sock);
  clnt_destroy(clnt);
  if (clnt_stat != RPC_SUCCESS) {
    if (again) {
#ifdef HAVE_FS_NFS3
      if (nfs_version == NFS_VERSION3) {
	plog(XLOG_INFO, "get_nfs_version trying a lower version");
	nfs_version = NFS_VERSION;
	again = 0;
      }
      goto try_again;
#endif /* HAVE_FS_NFS3 */
    }
    plog(XLOG_INFO, "get_nfs_version NFS(%d,%s) failed for %s",
 	 nfs_version, proto, host);
    return 0;
  }

  plog(XLOG_INFO, "get_nfs_version: returning (%d,%s) on host %s",
       nfs_version, proto, host);
  return nfs_version;
}
