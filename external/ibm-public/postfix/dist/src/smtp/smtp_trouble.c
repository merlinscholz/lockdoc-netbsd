/*	$NetBSD: smtp_trouble.c,v 1.1.1.1 2009/06/23 10:08:54 tron Exp $	*/

/*++
/* NAME
/*	smtp_trouble 3
/* SUMMARY
/*	error handler policies
/* SYNOPSIS
/*	#include "smtp.h"
/*
/*	int	smtp_sess_fail(state)
/*	SMTP_STATE *state;
/*
/*	int	smtp_site_fail(state, mta_name, resp, format, ...)
/*	SMTP_STATE *state;
/*	const char *mta_name;
/*	SMTP_RESP *resp;
/*	const char *format;
/*
/*	int	smtp_mesg_fail(state, mta_name, resp, format, ...)
/*	SMTP_STATE *state;
/*	const char *mta_name;
/*	SMTP_RESP *resp;
/*	const char *format;
/*
/*	void	smtp_rcpt_fail(state, recipient, mta_name, resp, format, ...)
/*	SMTP_STATE *state;
/*	RECIPIENT *recipient;
/*	const char *mta_name;
/*	SMTP_RESP *resp;
/*	const char *format;
/*
/*	int	smtp_stream_except(state, exception, description)
/*	SMTP_STATE *state;
/*	int	exception;
/*	const char *description;
/* DESCRIPTION
/*	This module handles all non-fatal errors that can happen while
/*	attempting to deliver mail via SMTP, and implements the policy
/*	of how to deal with the error. Depending on the nature of
/*	the problem, delivery of a single message is deferred, delivery
/*	of all messages to the same domain is deferred, or one or more
/*	recipients are given up as non-deliverable and a bounce log is
/*	updated. In any case, the recipient is marked as either KEEP
/*	(try again with a backup host) or DROP (delete recipient from
/*	delivery request).
/*
/*	In addition, when an unexpected response code is seen such
/*	as 3xx where only 4xx or 5xx are expected, or any error code
/*	that suggests a syntax error or something similar, the
/*	protocol error flag is set so that the postmaster receives
/*	a transcript of the session. No notification is generated for
/*	what appear to be configuration errors - very likely, they
/*	would suffer the same problem and just cause more trouble.
/*
/*	In case of a soft error, action depends on whether the error
/*	qualifies for trying the request with other mail servers (log
/*	an informational record only and try a backup server) or
/*	whether this is the final server (log recipient delivery status
/*	records and delete the recipient from the request).
/*
/*	smtp_sess_fail() takes a pre-formatted error report after
/*	failure to complete some protocol handshake.  The policy is
/*	as with smtp_site_fail().
/*
/*	smtp_site_fail() handles the case where the program fails to
/*	complete the initial handshake: the server is not reachable,
/*	is not running, does not want talk to us, or we talk to ourselves.
/*	The \fIcode\fR gives an error status code; the \fIformat\fR
/*	argument gives a textual description.
/*	The policy is: soft error, non-final server: log an informational
/*	record why the host is being skipped; soft error, final server:
/*	defer delivery of all remaining recipients and mark the destination
/*	as problematic; hard error: bounce all remaining recipients.
/*	The session is marked as "do not cache".
/*	The result is non-zero.
/*
/*	smtp_mesg_fail() handles the case where the smtp server
/*	does not accept the sender address or the message data,
/*	or when the local MTA is unable to convert the message data.
/*	The policy is: soft error, non-final server: log an informational
/*	record why the host is being skipped; soft error, final server:
/*	defer delivery of all remaining recipients; hard error: bounce all
/*	remaining recipients.
/*	The result is non-zero.
/*
/*	smtp_rcpt_fail() handles the case where a recipient is not
/*	accepted by the server for reasons other than that the server
/*	recipient limit is reached.
/*	The policy is: soft error, non-final server: log an informational
/*	record why the recipient is being skipped; soft error, final server:
/*	defer delivery of this recipient; hard error: bounce this
/*	recipient.
/*
/*	smtp_stream_except() handles the exceptions generated by
/*	the smtp_stream(3) module (i.e. timeouts and I/O errors).
/*	The \fIexception\fR argument specifies the type of problem.
/*	The \fIdescription\fR argument describes at what stage of
/*	the SMTP dialog the problem happened.
/*	The policy is: non-final server: log an informational record
/*	with the reason why the host is being skipped; final server:
/*	defer delivery of all remaining recipients.
/*	The session is marked as "do not cache".
/*	The result is non-zero.
/*
/*	Arguments:
/* .IP state
/*	SMTP client state per delivery request.
/* .IP resp
/*	Server response including reply code and text.
/* .IP recipient
/*	Undeliverable recipient address information.
/* .IP format
/*	Human-readable description of why mail is not deliverable.
/* DIAGNOSTICS
/*	Panic: unknown exception code.
/* SEE ALSO
/*	smtp_proto(3) smtp high-level protocol
/*	smtp_stream(3) smtp low-level protocol
/*	defer(3) basic message defer interface
/*	bounce(3) basic message bounce interface
/* LICENSE
/* .ad
/* .fi
/*	The Secure Mailer license must be distributed with this software.
/* AUTHOR(S)
/*	Wietse Venema
/*	IBM T.J. Watson Research
/*	P.O. Box 704
/*	Yorktown Heights, NY 10598, USA
/*--*/

/* System library. */

#include <sys_defs.h>
#include <stdlib.h>			/* 44BSD stdarg.h uses abort() */
#include <stdarg.h>
#include <string.h>

/* Utility library. */

#include <msg.h>
#include <vstring.h>
#include <stringops.h>

/* Global library. */

#include <smtp_stream.h>
#include <deliver_request.h>
#include <deliver_completed.h>
#include <bounce.h>
#include <defer.h>
#include <mail_error.h>
#include <dsn_buf.h>
#include <dsn.h>

/* Application-specific. */

#include "smtp.h"

#define SMTP_THROTTLE	1
#define SMTP_NOTHROTTLE	0

/* smtp_check_code - check response code */

static void smtp_check_code(SMTP_SESSION *session, int code)
{

    /*
     * The intention of this code is to alert the postmaster when the local
     * Postfix SMTP client screws up, protocol wise. RFC 821 says that x0z
     * replies "refer to syntax errors, syntactically correct commands that
     * don't fit any functional category, and unimplemented or superfluous
     * commands". Unfortunately, this also triggers postmaster notices when
     * remote servers screw up, protocol wise. This is becoming a common
     * problem now that response codes are configured manually as part of
     * anti-UCE systems, by people who aren't aware of RFC details.
     */
    if (code < 400 || code > 599
	|| code == 555			/* RFC 1869, section 6.1. */
	|| (code >= 500 && code < 510))
	session->error_mask |= MAIL_ERROR_PROTOCOL;
}

/* smtp_bulk_fail - skip, defer or bounce recipients, maybe throttle queue */

static int smtp_bulk_fail(SMTP_STATE *state, int throttle_queue)
{
    DELIVER_REQUEST *request = state->request;
    SMTP_SESSION *session = state->session;
    DSN_BUF *why = state->why;
    RECIPIENT *rcpt;
    int     status;
    int     soft_error = (STR(why->status)[0] == '4');
    int     nrcpt;

    /*
     * Don't defer the recipients just yet when this error qualifies them for
     * delivery to a backup server. Just log something informative to show
     * why we're skipping this host.
     */
    if (soft_error && (state->misc_flags & SMTP_MISC_FLAG_FINAL_SERVER) == 0) {
	msg_info("%s: %s", request->queue_id, STR(why->reason));
	for (nrcpt = 0; nrcpt < SMTP_RCPT_LEFT(state); nrcpt++) {
	    rcpt = request->rcpt_list.info + nrcpt;
	    if (SMTP_RCPT_ISMARKED(rcpt))
		continue;
	    SMTP_RCPT_KEEP(state, rcpt);
	}
    }

    /*
     * Defer or bounce all the remaining recipients, and delete them from the
     * delivery request. If a bounce fails, defer instead and do not qualify
     * the recipient for delivery to a backup server.
     */
    else {

	/*
	 * If we are still in the connection set-up phase, update the set-up
	 * completion time here, otherwise the time spent in set-up latency
	 * will be attributed as message transfer latency.
	 * 
	 * All remaining recipients have failed at this point, so we update the
	 * delivery completion time stamp so that multiple recipient status
	 * records show the same delay values.
	 */
	if (request->msg_stats.conn_setup_done.tv_sec == 0) {
	    GETTIMEOFDAY(&request->msg_stats.conn_setup_done);
	    request->msg_stats.deliver_done =
		request->msg_stats.conn_setup_done;
	} else
	    GETTIMEOFDAY(&request->msg_stats.deliver_done);

	(void) DSN_FROM_DSN_BUF(why);
	for (nrcpt = 0; nrcpt < SMTP_RCPT_LEFT(state); nrcpt++) {
	    rcpt = request->rcpt_list.info + nrcpt;
	    if (SMTP_RCPT_ISMARKED(rcpt))
		continue;
	    status = (soft_error ? defer_append : bounce_append)
		(DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
		 &request->msg_stats, rcpt,
		 session ? session->namaddrport : "none", &why->dsn);
	    if (status == 0)
		deliver_completed(state->src, rcpt->offset);
	    SMTP_RCPT_DROP(state, rcpt);
	    state->status |= status;
	}
	if ((state->misc_flags & SMTP_MISC_FLAG_COMPLETE_SESSION) == 0
	    && throttle_queue && soft_error && request->hop_status == 0)
	    request->hop_status = DSN_COPY(&why->dsn);
    }

    /*
     * Don't cache this session. We can't talk to this server.
     */
    if (throttle_queue && session)
	DONT_CACHE_BAD_SESSION;

    return (-1);
}

/* smtp_sess_fail - skip site, defer or bounce all recipients */

int     smtp_sess_fail(SMTP_STATE *state)
{

    /*
     * We can't avoid copying copying lots of strings into VSTRING buffers,
     * because this error information is collected by a routine that
     * terminates BEFORE the error is reported.
     */
    return (smtp_bulk_fail(state, SMTP_THROTTLE));
}

/* vsmtp_fill_dsn - fill in temporary DSN structure */

static void vsmtp_fill_dsn(SMTP_STATE *state, const char *mta_name,
			           const char *status, const char *reply,
			           const char *format, va_list ap)
{
    DSN_BUF *why = state->why;

    /*
     * We could avoid copying lots of strings into VSTRING buffers, because
     * this error information is given to us by a routine that terminates
     * AFTER the error is reported. However, this results in ugly kludges
     * when informal text needs to be formatted. So we maintain consistency
     * with other error reporting in the SMTP client even if we waste a few
     * cycles.
     */
    VSTRING_RESET(why->reason);
    if (mta_name && reply && reply[0] != '4' && reply[0] != '5') {
	vstring_strcpy(why->reason, "Protocol error: ");
	status = "5.5.0";
    }
    vstring_vsprintf_append(why->reason, format, ap);
    dsb_formal(why, status, DSB_DEF_ACTION,
	       mta_name ? DSB_MTYPE_DNS : DSB_MTYPE_NONE, mta_name,
	       reply ? DSB_DTYPE_SMTP : DSB_DTYPE_NONE, reply);
}

/* smtp_site_fail - throttle this queue; skip, defer or bounce all recipients */

int     smtp_site_fail(SMTP_STATE *state, const char *mta_name, SMTP_RESP *resp,
		               const char *format,...)
{
    va_list ap;

    /*
     * Initialize.
     */
    va_start(ap, format);
    vsmtp_fill_dsn(state, mta_name, resp->dsn, resp->str, format, ap);
    va_end(ap);

    if (state->session && mta_name)
	smtp_check_code(state->session, resp->code);

    /*
     * Skip, defer or bounce recipients, and throttle this queue.
     */
    return (smtp_bulk_fail(state, SMTP_THROTTLE));
}

/* smtp_mesg_fail - skip, defer or bounce all recipients; no queue throttle */

int     smtp_mesg_fail(SMTP_STATE *state, const char *mta_name, SMTP_RESP *resp,
		               const char *format,...)
{
    va_list ap;

    /*
     * Initialize.
     */
    va_start(ap, format);
    vsmtp_fill_dsn(state, mta_name, resp->dsn, resp->str, format, ap);
    va_end(ap);

    if (state->session && mta_name)
	smtp_check_code(state->session, resp->code);

    /*
     * Skip, defer or bounce recipients, but don't throttle this queue.
     */
    return (smtp_bulk_fail(state, SMTP_NOTHROTTLE));
}

/* smtp_rcpt_fail - skip, defer, or bounce recipient */

void    smtp_rcpt_fail(SMTP_STATE *state, RECIPIENT *rcpt, const char *mta_name,
		               SMTP_RESP *resp, const char *format,...)
{
    DELIVER_REQUEST *request = state->request;
    SMTP_SESSION *session = state->session;
    DSN_BUF *why = state->why;
    int     status;
    int     soft_error;
    va_list ap;

    /*
     * Sanity check.
     */
    if (SMTP_RCPT_ISMARKED(rcpt))
	msg_panic("smtp_rcpt_fail: recipient <%s> is marked", rcpt->address);

    /*
     * Initialize.
     */
    va_start(ap, format);
    vsmtp_fill_dsn(state, mta_name, resp->dsn, resp->str, format, ap);
    va_end(ap);
    soft_error = STR(why->status)[0] == '4';

    if (state->session && mta_name)
	smtp_check_code(state->session, resp->code);

    /*
     * Don't defer this recipient record just yet when this error qualifies
     * for trying other mail servers. Just log something informative to show
     * why we're skipping this recipient now.
     */
    if (soft_error && (state->misc_flags & SMTP_MISC_FLAG_FINAL_SERVER) == 0) {
	msg_info("%s: %s", request->queue_id, STR(why->reason));
	SMTP_RCPT_KEEP(state, rcpt);
    }

    /*
     * Defer or bounce this recipient, and delete from the delivery request.
     * If the bounce fails, defer instead and do not qualify the recipient
     * for delivery to a backup server.
     * 
     * Note: we may still make an SMTP connection to deliver other recipients
     * that did qualify for delivery to a backup server.
     */
    else {
	(void) DSN_FROM_DSN_BUF(state->why);
	status = (soft_error ? defer_append : bounce_append)
	    (DEL_REQ_TRACE_FLAGS(request->flags), request->queue_id,
	     &request->msg_stats, rcpt,
	     session ? session->namaddrport : "none", &why->dsn);
	if (status == 0)
	    deliver_completed(state->src, rcpt->offset);
	SMTP_RCPT_DROP(state, rcpt);
	state->status |= status;
    }
}

/* smtp_stream_except - defer domain after I/O problem */

int     smtp_stream_except(SMTP_STATE *state, int code, const char *description)
{
    SMTP_SESSION *session = state->session;
    DSN_BUF *why = state->why;

    /*
     * Sanity check.
     */
    if (session == 0)
	msg_panic("smtp_stream_except: no session");

    /*
     * Initialize.
     */
    switch (code) {
    default:
	msg_panic("smtp_stream_except: unknown exception %d", code);
    case SMTP_ERR_EOF:
	dsb_simple(why, "4.4.2", "lost connection with %s while %s",
		   session->namaddr, description);
	break;
    case SMTP_ERR_TIME:
	dsb_simple(why, "4.4.2", "conversation with %s timed out while %s",
		   session->namaddr, description);
	break;
    }
    return (smtp_bulk_fail(state, SMTP_THROTTLE));
}
