/*++
/* NAME
/*	recipient 3
/* SUMMARY
/*	deliver to one local recipient
/* SYNOPSIS
/*	#include "local.h"
/*
/*	int	deliver_recipient(state, usr_attr)
/*	LOCAL_STATE state;
/*	USER_ATTR *usr_attr;
/* DESCRIPTION
/*	deliver_recipient() delivers a message to a local recipient.
/*	It is called initially when the queue manager requests
/*	delivery to a local recipient, and is called recursively
/*	when an alias or forward file expands to a local recipient.
/*
/*	When called recursively with, for example, a result from alias
/*	or forward file expansion, aliases are expanded immediately,
/*	but mail for non-alias destinations is submitted as a new
/*	message, so that each recipient has a dedicated queue file
/*	message delivery status record (in a shared queue file).
/*
/*	When the \fIrecipient_delimiter\fR configuration parameter
/*	is set, it is used to separate cookies off recipient names.
/*	A common setting is to have "recipient_delimiter = +"
/*	so that mail for \fIuser+foo\fR is delivered to \fIuser\fR,
/*	with a "Delivered-To: user+foo@domain" header line.
/*
/*	Arguments:
/* .IP state
/*	The attributes that specify the message, sender, and more.
/*	Attributes describing alias, include or forward expansion.
/*	A table with the results from expanding aliases or lists.
/*	A table with delivered-to: addresses taken from the message.
/* .IP usr_attr
/*	Attributes describing user rights and environment.
/* DIAGNOSTICS
/*	deliver_recipient() returns non-zero when delivery should be
/*	tried again.
/* BUGS
/*	Mutually-recursive aliases or $HOME/.forward files aren't
/*	detected when they could be. The resulting mail forwarding loop
/*	is broken by the use of the Delivered-To: message header.
/* SEE ALSO
/*	alias(3) delivery to aliases
/*	mailbox(3) delivery to mailbox
/*	dotforward(3) delivery to destinations in .forward file
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
#include <unistd.h>
#include <string.h>

#ifdef STRCASECMP_IN_STRINGS_H
#include <strings.h>
#endif

/* Utility library. */

#include <msg.h>
#include <mymalloc.h>
#include <htable.h>
#include <split_at.h>
#include <stringops.h>
#include <dict.h>

/* Global library. */

#include <bounce.h>
#include <mail_params.h>
#include <split_addr.h>
#include <ext_prop.h>

/* Application-specific. */

#include "local.h"

/* deliver_switch - branch on recipient type */

static int deliver_switch(LOCAL_STATE state, USER_ATTR usr_attr)
{
    char   *myname = "deliver_switch";
    int     status = 0;

    /*
     * Make verbose logging easier to understand.
     */
    state.level++;
    if (msg_verbose)
	MSG_LOG_STATE(myname, state);


    /*
     * \user is special: it means don't do any alias or forward expansion.
     */
    if (state.msg_attr.recipient[0] == '\\') {
	state.msg_attr.recipient++, state.msg_attr.local++, state.msg_attr.user++;
	if (deliver_mailbox(state, usr_attr, &status) == 0)
	    status = deliver_unknown(state, usr_attr);
	return (status);
    }

    /*
     * Otherwise, alias expansion has highest precedence. First look up the
     * full localpart, then the bare user. Obey the address extension
     * propagation policy.
     */
    state.msg_attr.unmatched = 0;
    if (deliver_alias(state, usr_attr, state.msg_attr.local, &status))
	return (status);
    if (state.msg_attr.extension != 0) {
	if (local_ext_prop_mask & EXT_PROP_ALIAS)
	    state.msg_attr.unmatched = state.msg_attr.extension;
	if (deliver_alias(state, usr_attr, state.msg_attr.user, &status))
	    return (status);
	state.msg_attr.unmatched = state.msg_attr.extension;
    }

    /*
     * Special case for mail locally forwarded or aliased to a different
     * local address. Resubmit the message via the cleanup service, so that
     * each recipient gets a separate delivery queue file status record in
     * the new queue file. The downside of this approach is that mutually
     * recursive .forward files cause a mail forwarding loop. Fortunately,
     * the loop can be broken by the use of the Delivered-To: message header.
     * 
     * The code below must not trigger on mail sent to an alias that has no
     * owner- companion, so that mail for an alias first.last->username is
     * delivered directly, instead of going through username->first.last
     * canonical mappings in the cleanup service. The downside of this
     * approach is that recipients in the expansion of an alias without
     * owner- won't have separate delivery queue file status records, because
     * for them, the message won't be resubmitted as a new queue file.
     * 
     * Do something sensible on systems that receive mail for multiple domains,
     * such as primary.name and secondary.name. Don't resubmit the message
     * when mail for `user@secondary.name' is delivered to a .forward file
     * that lists `user' or `user@primary.name'. We already know that the
     * recipient domain is local, so we only have to compare local parts.
     */
    if (state.msg_attr.owner != 0
	&& strcasecmp(state.msg_attr.owner, state.msg_attr.user) != 0)
	return (deliver_indirect(state));

    /*
     * Always forward recipients in :include: files.
     */
    if (state.msg_attr.exp_type == EXPAND_TYPE_INCL)
	return (deliver_indirect(state));

    /*
     * Delivery to local user. First try expansion of the recipient's
     * $HOME/.forward file, then mailbox delivery.
     */
    if (deliver_dotforward(state, usr_attr, &status) == 0
	&& deliver_mailbox(state, usr_attr, &status) == 0)
	status = deliver_unknown(state, usr_attr);
    return (status);
}

/* deliver_recipient - deliver one local recipient */

int     deliver_recipient(LOCAL_STATE state, USER_ATTR usr_attr)
{
    char   *myname = "deliver_recipient";
    int     rcpt_stat;

    /*
     * Make verbose logging easier to understand.
     */
    state.level++;
    if (msg_verbose)
	MSG_LOG_STATE(myname, state);

    /*
     * Duplicate filter.
     */
    if (been_here(state.dup_filter, "recipient %d %s",
		  state.level, state.msg_attr.recipient))
	return (0);

    /*
     * With each level of recursion, detect and break external message
     * forwarding loops.
     */
    if (delivered_find(state.loop_info, state.msg_attr.recipient))
	return (bounce_append(BOUNCE_FLAG_KEEP, BOUNCE_ATTR(state.msg_attr),
		  "mail forwarding loop for %s", state.msg_attr.recipient));

    /*
     * Set up the recipient-specific attributes. If this is forwarded mail,
     * leave the delivered attribute alone, so that the forwarded message
     * will show the correct forwarding recipient.
     */
    if (state.msg_attr.delivered == 0)
	state.msg_attr.delivered = state.msg_attr.recipient;
    state.msg_attr.local = mystrdup(state.msg_attr.recipient);
    lowercase(state.msg_attr.local);
    if ((state.msg_attr.domain = split_at_right(state.msg_attr.local, '@')) == 0)
	msg_warn("no @ in recipient address: %s", state.msg_attr.local);

    /*
     * Address extension management.
     */
    state.msg_attr.user = mystrdup(state.msg_attr.local);
    if (*var_rcpt_delim) {
	state.msg_attr.extension =
	    split_addr(state.msg_attr.user, *var_rcpt_delim);
	if (state.msg_attr.extension && strchr(state.msg_attr.extension, '/')) {
	    msg_warn("%s: address with illegal extension: %s",
		     state.msg_attr.queue_id, state.msg_attr.local);
	    state.msg_attr.extension = 0;
	}
    } else
	state.msg_attr.extension = 0;
    state.msg_attr.unmatched = state.msg_attr.extension;

    /*
     * Do not allow null usernames.
     */
    if (state.msg_attr.user[0] == 0)
	return (bounce_append(BOUNCE_FLAG_KEEP, BOUNCE_ATTR(state.msg_attr),
			  "null username in %s", state.msg_attr.recipient));

    /*
     * Run the recipient through the delivery switch.
     */
    if (msg_verbose)
	deliver_attr_dump(&state.msg_attr);
    rcpt_stat = deliver_switch(state, usr_attr);

    /*
     * Clean up.
     */
    myfree(state.msg_attr.local);
    myfree(state.msg_attr.user);

    return (rcpt_stat);
}
