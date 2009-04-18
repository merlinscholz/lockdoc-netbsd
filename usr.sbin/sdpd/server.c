/*	$NetBSD: server.c,v 1.6 2009/04/18 13:53:59 lukem Exp $	*/

/*-
 * Copyright (c) 2006 Itronix Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of Itronix Inc. may not be used to endorse
 *    or promote products derived from this software without specific
 *    prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ITRONIX INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ITRONIX INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * server.c
 *
 * Copyright (c) 2004 Maksim Yevmenkin <m_evmenkin@yahoo.com>
 * All rights reserved.
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
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: server.c,v 1.6 2009/04/18 13:53:59 lukem Exp $
 * $FreeBSD: src/usr.sbin/bluetooth/sdpd/server.c,v 1.2 2005/12/06 17:56:36 emax Exp $
 */

#include <sys/cdefs.h>
__RCSID("$NetBSD: server.c,v 1.6 2009/04/18 13:53:59 lukem Exp $");

#include <sys/param.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/queue.h>
#include <sys/ucred.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <bluetooth.h>
#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <sdp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "log.h"
#include "profile.h"
#include "provider.h"
#include "server.h"

static void	server_accept_client		(server_p srv, int32_t fd);
static int32_t	server_process_request		(server_p srv, int32_t fd);
static int32_t	server_send_error_response	(server_p srv, int32_t fd,
						 uint16_t error);
static void	server_close_fd			(server_p srv, int32_t fd);
static int	server_auth_check		(server_p srv, struct sockcred *cred);

/*
 * Initialize server
 */

int32_t
server_init(server_p srv, char const *control, char const *sgroup)
{
	struct sockaddr_un	un;
	struct sockaddr_bt	l2;
	socklen_t		size;
	int32_t			unsock, l2sock;
	uint16_t		imtu;
	int			opt;

	assert(srv != NULL);
	assert(control != NULL);

	memset(srv, 0, sizeof(srv));
	srv->sgroup = sgroup;

	/* Open control socket */
	if (unlink(control) < 0 && errno != ENOENT) {
		log_crit("Could not unlink(%s). %s (%d)",
			control, strerror(errno), errno);
		return (-1);
	}

	unsock = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (unsock < 0) {
		log_crit("Could not create control socket. %s (%d)",
			strerror(errno), errno);
		return (-1);
	}

	opt = 1;
	if (setsockopt(unsock, 0, LOCAL_CREDS, &opt, sizeof(opt)) < 0)
		log_crit("Warning: No credential checks on control socket");

	memset(&un, 0, sizeof(un));
	un.sun_len = sizeof(un);
	un.sun_family = AF_LOCAL;
	strlcpy(un.sun_path, control, sizeof(un.sun_path));

	if (bind(unsock, (struct sockaddr *) &un, sizeof(un)) < 0) {
		log_crit("Could not bind control socket. %s (%d)",
			strerror(errno), errno);
		close(unsock);
		return (-1);
	}

	if (chmod(control, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH) < 0) {
		log_crit("Could not change permissions on control socket. " \
			"%s (%d)", strerror(errno), errno);
		close(unsock);
		return (-1);
	}

	if (listen(unsock, 10) < 0) {
		log_crit("Could not listen on control socket. %s (%d)",
			strerror(errno), errno);
		close(unsock);
		return (-1);
	}

	/* Open L2CAP socket */
	l2sock = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (l2sock < 0) {
		log_crit("Could not create L2CAP socket. %s (%d)",
			strerror(errno), errno);
		close(unsock);
		return (-1);
	}

	size = sizeof(imtu);
        if (getsockopt(l2sock, BTPROTO_L2CAP, SO_L2CAP_IMTU, &imtu, &size) < 0) {
		log_crit("Could not get L2CAP IMTU. %s (%d)",
			strerror(errno), errno);
		close(unsock);
		close(l2sock);
		return (-1);
        }

	memset(&l2, 0, sizeof(l2));
	l2.bt_len = sizeof(l2);
	l2.bt_family = AF_BLUETOOTH;
	l2.bt_psm = L2CAP_PSM_SDP;
	bdaddr_copy(&l2.bt_bdaddr, BDADDR_ANY);

	if (bind(l2sock, (struct sockaddr *) &l2, sizeof(l2)) < 0) {
		log_crit("Could not bind L2CAP socket. %s (%d)",
			strerror(errno), errno);
		close(unsock);
		close(l2sock);
		return (-1);
	}

	if (listen(l2sock, 10) < 0) {
		log_crit("Could not listen on L2CAP socket. %s (%d)",
			strerror(errno), errno);
		close(unsock);
		close(l2sock);
		return (-1);
	}

	/* Allocate incoming buffer */
	srv->imtu = (imtu > SDP_LOCAL_MTU)? imtu : SDP_LOCAL_MTU;
	srv->req = (uint8_t *) calloc(srv->imtu, sizeof(srv->req[0]));
	if (srv->req == NULL) {
		log_crit("Could not allocate request buffer");
		close(unsock);
		close(l2sock);
		return (-1);
	}

	/* Allocate memory for descriptor index */
	srv->fdidx = (fd_idx_p) calloc(FD_SETSIZE, sizeof(srv->fdidx[0]));
	if (srv->fdidx == NULL) {
		log_crit("Could not allocate fd index");
		free(srv->req);
		close(unsock);
		close(l2sock);
		return (-1);
	}

	/* Register Service Discovery profile (attach it to control socket) */
	if (provider_register_sd(unsock) < 0) {
		log_crit("Could not register Service Discovery profile");
		free(srv->fdidx);
		free(srv->req);
		close(unsock);
		close(l2sock);
		return (-1);
	}

	/*
	 * If we got here then everything is fine. Add both control sockets
	 * to the index.
	 */

	FD_ZERO(&srv->fdset);
	srv->maxfd = (unsock > l2sock)? unsock : l2sock;

	FD_SET(unsock, &srv->fdset);
	srv->fdidx[unsock].valid = 1;
	srv->fdidx[unsock].server = 1;
	srv->fdidx[unsock].control = 1;
	srv->fdidx[unsock].priv = 0;
	srv->fdidx[unsock].rsp_cs = 0;
	srv->fdidx[unsock].rsp_size = 0;
	srv->fdidx[unsock].rsp_limit = 0;
	srv->fdidx[unsock].omtu = SDP_LOCAL_MTU;
	srv->fdidx[unsock].rsp = NULL;

	FD_SET(l2sock, &srv->fdset);
	srv->fdidx[l2sock].valid = 1;
	srv->fdidx[l2sock].server = 1;
	srv->fdidx[l2sock].control = 0;
	srv->fdidx[l2sock].priv = 0;
	srv->fdidx[l2sock].rsp_cs = 0;
	srv->fdidx[l2sock].rsp_size = 0;
	srv->fdidx[l2sock].rsp_limit = 0;
	srv->fdidx[l2sock].omtu = 0; /* unknown */
	srv->fdidx[l2sock].rsp = NULL;

	return (0);
}

/*
 * Shutdown server
 */

void
server_shutdown(server_p srv)
{
	int	fd;

	assert(srv != NULL);

	for (fd = 0; fd < srv->maxfd + 1; fd ++)
		if (srv->fdidx[fd].valid)
			server_close_fd(srv, fd);

	free(srv->req);
	free(srv->fdidx);

	memset(srv, 0, sizeof(*srv));
}

/*
 * Do one server iteration
 */

int32_t
server_do(server_p srv)
{
	fd_set	fdset;
	int32_t	n, fd;

	assert(srv != NULL);

	/* Copy cached version of the fd set and call select */
	memcpy(&fdset, &srv->fdset, sizeof(fdset));
	n = select(srv->maxfd + 1, &fdset, NULL, NULL, NULL);
	if (n < 0) {
		if (errno == EINTR)
			return (0);

		log_err("Could not select(%d, %p). %s (%d)",
			srv->maxfd + 1, &fdset, strerror(errno), errno);

		return (-1);
	}

	/* Process  descriptors */
	for (fd = 0; fd < srv->maxfd + 1 && n > 0; fd ++) {
		if (!FD_ISSET(fd, &fdset))
			continue;

		assert(srv->fdidx[fd].valid);
		n --;

		if (srv->fdidx[fd].server)
			server_accept_client(srv, fd);
		else if (server_process_request(srv, fd) != 0)
			server_close_fd(srv, fd);
	}

	return (0);

}

/*
 * Accept new client connection and register it with index
 */

static void
server_accept_client(server_p srv, int32_t fd)
{
	uint8_t		*rsp = NULL;
	socklen_t	 size;
	int32_t		 cfd;
	uint16_t	 omtu;

	do {
		cfd = accept(fd, NULL, NULL);
	} while (cfd < 0 && errno == EINTR);

	if (cfd < 0) {
		log_err("Could not accept connection on %s socket. %s (%d)",
			srv->fdidx[fd].control? "control" : "L2CAP",
			strerror(errno), errno);
		return;
	}

	assert(!FD_ISSET(cfd, &srv->fdset));
	assert(!srv->fdidx[cfd].valid);

	if (!srv->fdidx[fd].control) {
		/* Get local BD_ADDR */
		size = sizeof(srv->req_sa);
		if (getsockname(cfd,(struct sockaddr*)&srv->req_sa, &size) < 0) {
			log_err("Could not get local BD_ADDR. %s (%d)",
				strerror(errno), errno);
			close(cfd);
			return;
		}

		/* Get outgoing MTU */
		size = sizeof(omtu);
	        if (getsockopt(cfd, BTPROTO_L2CAP, SO_L2CAP_OMTU, &omtu, &size) < 0) {
			log_err("Could not get L2CAP OMTU. %s (%d)",
				strerror(errno), errno);
			close(cfd);
			return;
		}

		/*
		 * The maximum size of the L2CAP packet is 65536 bytes.
		 * The minimum L2CAP MTU is 43 bytes. That means we need
		 * 65536 / 43 = ~1524 chunks to transfer maximum packet
		 * size with minimum MTU. The "rsp_cs" field in fd_idx_t
		 * is 11 bit wide that gives us upto 2048 chunks.
		 */

		if (omtu < L2CAP_MTU_MINIMUM) {
			log_err("L2CAP OMTU is too small (%d bytes)", omtu);
			close(cfd);
			return;
		}
	} else {
		bdaddr_copy(&srv->req_sa.bt_bdaddr, BDADDR_ANY);
		omtu = srv->fdidx[fd].omtu;
	}

	/*
	 * Allocate buffer. This is an overkill, but we can not know how
	 * big our reply is going to be.
	 */

	rsp = (uint8_t *) calloc(L2CAP_MTU_MAXIMUM, sizeof(rsp[0]));
	if (rsp == NULL) {
		log_crit("Could not allocate response buffer");
		close(cfd);
		return;
	}

	/* Add client descriptor to the index */
	FD_SET(cfd, &srv->fdset);
	if (srv->maxfd < cfd)
		srv->maxfd = cfd;
	srv->fdidx[cfd].valid = 1;
	srv->fdidx[cfd].server = 0;
	srv->fdidx[cfd].control = srv->fdidx[fd].control;
	srv->fdidx[cfd].priv = 0;
	srv->fdidx[cfd].rsp_cs = 0;
	srv->fdidx[cfd].rsp_size = 0;
	srv->fdidx[cfd].rsp_limit = 0;
	srv->fdidx[cfd].omtu = omtu;
	srv->fdidx[cfd].rsp = rsp;
}

/*
 * Process request from the client
 */

static int32_t
server_process_request(server_p srv, int32_t fd)
{
	uint8_t		ctl[128];
	sdp_pdu_p	pdu = (sdp_pdu_p) srv->req;
	struct msghdr	msg;
	struct iovec	iov;
	int32_t		len, error;
	struct cmsghdr	*cmsg;

	assert(srv->imtu > 0);
	assert(srv->req != NULL);
	assert(FD_ISSET(fd, &srv->fdset));
	assert(srv->fdidx[fd].valid);
	assert(!srv->fdidx[fd].server);
	assert(srv->fdidx[fd].rsp != NULL);
	assert(srv->fdidx[fd].omtu >= L2CAP_MTU_MINIMUM);

	iov.iov_base = srv->req;
	iov.iov_len = srv->imtu;

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = ctl;
	msg.msg_controllen = sizeof(ctl);
	msg.msg_flags = 0;

	do {
		len = recvmsg(fd, &msg, 0);
	} while (len < 0 && errno == EINTR);

	if (len < 0) {
		log_err("Could not receive SDP request from %s socket. %s (%d)",
			srv->fdidx[fd].control? "control" : "L2CAP",
			strerror(errno), errno);
		return (-1);
	}
	if (len == 0) {
		log_info("Client on %s socket has disconnected",
			srv->fdidx[fd].control? "control" : "L2CAP");
		return (-1);
	}

	if (msg.msg_flags & MSG_TRUNC) {
		log_err("Truncated message on %s socket",
			srv->fdidx[fd].control? "control" : "L2CAP");
		return (-1);
	}

	if ((msg.msg_flags & MSG_CTRUNC) == 0
	    && (cmsg = CMSG_FIRSTHDR(&msg)) != NULL
	    && cmsg->cmsg_level == SOL_SOCKET
	    && cmsg->cmsg_type == SCM_CREDS
	    && cmsg->cmsg_len >= CMSG_LEN(SOCKCREDSIZE(0)))
	    	srv->fdidx[fd].priv =
		    server_auth_check(srv, (struct sockcred *)CMSG_DATA(cmsg));

	if ((uint32_t)len >= sizeof(*pdu)
	    && (sizeof(*pdu) + (pdu->len = ntohs(pdu->len))) == (uint32_t)len) {
		switch (pdu->pid) {
		case SDP_PDU_SERVICE_SEARCH_REQUEST:
			error = server_prepare_service_search_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_ATTRIBUTE_REQUEST:
			error = server_prepare_service_attribute_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
			error = server_prepare_service_search_attribute_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_REGISTER_REQUEST:
			error = server_prepare_service_register_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_UNREGISTER_REQUEST:
			error = server_prepare_service_unregister_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_CHANGE_REQUEST:
			error = server_prepare_service_change_response(srv, fd);
			break;

		default:
			error = SDP_ERROR_CODE_INVALID_REQUEST_SYNTAX;
			break;
		}
	} else
		error = SDP_ERROR_CODE_INVALID_PDU_SIZE;

	if (error == 0) {
		switch (pdu->pid) {
		case SDP_PDU_SERVICE_SEARCH_REQUEST:
			error = server_send_service_search_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_ATTRIBUTE_REQUEST:
			error = server_send_service_attribute_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_SEARCH_ATTRIBUTE_REQUEST:
			error = server_send_service_search_attribute_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_REGISTER_REQUEST:
			error = server_send_service_register_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_UNREGISTER_REQUEST:
			error = server_send_service_unregister_response(srv, fd);
			break;

		case SDP_PDU_SERVICE_CHANGE_REQUEST:
			error = server_send_service_change_response(srv, fd);
			break;

		default:
			error = SDP_ERROR_CODE_INVALID_REQUEST_SYNTAX;
			break;
		}

		if (error != 0)
			log_err("Could not send SDP response to %s socket, " \
				"pdu->pid=%d, pdu->tid=%d, error=%d",
				srv->fdidx[fd].control? "control" : "L2CAP",
				pdu->pid, ntohs(pdu->tid), error);
	} else {
		log_err("Could not process SDP request from %s socket, " \
			"pdu->pid=%d, pdu->tid=%d, pdu->len=%d, len=%d, " \
			"error=%d",
			srv->fdidx[fd].control? "control" : "L2CAP",
			pdu->pid, ntohs(pdu->tid), pdu->len, len, error);

		error = server_send_error_response(srv, fd, error);
		if (error != 0)
			log_err("Could not send SDP error response to %s " \
				"socket, pdu->pid=%d, pdu->tid=%d, error=%d",
				srv->fdidx[fd].control? "control" : "L2CAP",
				pdu->pid, ntohs(pdu->tid), error);
	}

	/* On error forget response (if any) */
	if (error != 0) {
		srv->fdidx[fd].rsp_cs = 0;
		srv->fdidx[fd].rsp_size = 0;
		srv->fdidx[fd].rsp_limit = 0;
	}

	return (error);
}

/*
 * Send SDP_Error_Response PDU
 */

static int32_t
server_send_error_response(server_p srv, int32_t fd, uint16_t error)
{
	int32_t	size;

	struct {
		sdp_pdu_t		pdu;
		uint16_t		error;
	} __packed	rsp;

	/* Prepare and send SDP error response */
	rsp.pdu.pid = SDP_PDU_ERROR_RESPONSE;
	rsp.pdu.tid = ((sdp_pdu_p)(srv->req))->tid;
	rsp.pdu.len = htons(sizeof(rsp.error));
	rsp.error   = htons(error);

	do {
		size = write(fd, &rsp, sizeof(rsp));
	} while (size < 0 && errno == EINTR);

	return ((size < 0)? errno : 0);
}

/*
 * Close descriptor and remove it from index
 */

static void
server_close_fd(server_p srv, int32_t fd)
{
	provider_p	provider = NULL, provider_next = NULL;

	assert(FD_ISSET(fd, &srv->fdset));
	assert(srv->fdidx[fd].valid);

	close(fd);

	FD_CLR(fd, &srv->fdset);
	if (fd == srv->maxfd)
		srv->maxfd --;

	if (srv->fdidx[fd].rsp != NULL)
		free(srv->fdidx[fd].rsp);

	memset(&srv->fdidx[fd], 0, sizeof(srv->fdidx[fd]));

	for (provider = provider_get_first();
	     provider != NULL;
	     provider = provider_next) {
		provider_next = provider_get_next(provider);

		if (provider->fd == fd)
			provider_unregister(provider);
	}
}

static int
server_auth_check(server_p srv, struct sockcred *cred)
{
	struct group *grp;
	int n;

	if (cred == NULL)
		return 0;

	if (cred->sc_uid == 0 || cred->sc_euid == 0)
		return 1;

	if (srv->sgroup == NULL)
		return 0;

	grp = getgrnam(srv->sgroup);
	if (grp == NULL) {
		log_err("No gid for group '%s'", srv->sgroup);
		srv->sgroup = NULL;
		return 0;
	}

	if (cred->sc_gid == grp->gr_gid || cred->sc_egid == grp->gr_gid)
		return 1;

	for (n = 0 ; n < cred->sc_ngroups ; n++) {
		if (cred->sc_groups[n] == grp->gr_gid)
			return 1;
	}

	return 0;
}
