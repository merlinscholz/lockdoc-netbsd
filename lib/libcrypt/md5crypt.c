/*	$NetBSD: md5crypt.c,v 1.6 2003/07/14 12:37:13 itojun Exp $	*/

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <phk@login.dknet.dk> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.   Poul-Henning Kamp
 * ----------------------------------------------------------------------------
 *
 * from FreeBSD: crypt.c,v 1.5 1996/10/14 08:34:02 phk Exp
 * via OpenBSD: md5crypt.c,v 1.9 1997/07/23 20:58:27 kstailey Exp
 *
 */

#include <sys/cdefs.h>
#if !defined(lint)
__RCSID("$NetBSD: md5crypt.c,v 1.6 2003/07/14 12:37:13 itojun Exp $");
#endif /* not lint */

/*
 * NOTE: We are also built for inclusion in libcrypto; when built for that
 * environment, use the libcrypto versions of the MD5 routines, so save
 * having to pull two versions into the same program.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#ifdef libcrypto
#include <openssl/md5.h>
#else
#include <md5.h>
#endif
#include <string.h>

#define MD5_MAGIC	"$1$"
#define MD5_MAGIC_LEN	3

char	*__md5crypt(const char *pw, const char *salt);	/* XXX */

static unsigned char itoa64[] =		/* 0 ... 63 => ascii - 64 */
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

#ifdef libcrypto
#define	INIT(x)			MD5_Init((x))
#define	UPDATE(x, b, l)		MD5_Update((x), (b), (l))
#define	FINAL(v, x)		MD5_Final((v), (x))
#else
#define	INIT(x)			MD5Init((x))
#define	UPDATE(x, b, l)		MD5Update((x), (b), (l))
#define	FINAL(v, x)		MD5Final((v), (x))
#endif

static void
to64(char *s, u_int32_t v, int n)
{

	while (--n >= 0) {
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

/*
 * MD5 password encryption.
 */
char *
__md5crypt(const char *pw, const char *salt)
{
	static char passwd[120], *p;
	const char *sp, *ep;
	unsigned char final[16];
	unsigned int i, sl, pwl;
	MD5_CTX	ctx, ctx1;
	u_int32_t l;
	int pl;
	
	pwl = strlen(pw);
	
	/* Refine the salt first */
	sp = salt;

	/* If it starts with the magic string, then skip that */
	if (strncmp(sp, MD5_MAGIC, MD5_MAGIC_LEN) == 0)
		sp += MD5_MAGIC_LEN;

	/* It stops at the first '$', max 8 chars */
	for (ep = sp; *ep != '\0' && *ep != '$' && ep < (sp + 8); ep++)
		continue;

	/* get the length of the true salt */
	sl = ep - sp;

	INIT(&ctx);

	/* The password first, since that is what is most unknown */
	UPDATE(&ctx, (const unsigned char *)pw, pwl);

	/* Then our magic string */
	UPDATE(&ctx, (const unsigned char *)MD5_MAGIC, MD5_MAGIC_LEN);

	/* Then the raw salt */
	UPDATE(&ctx, (const unsigned char *)sp, sl);

	/* Then just as many characters of the MD5(pw,salt,pw) */
	INIT(&ctx1);
	UPDATE(&ctx1, (const unsigned char *)pw, pwl);
	UPDATE(&ctx1, (const unsigned char *)sp, sl);
	UPDATE(&ctx1, (const unsigned char *)pw, pwl);
	FINAL(final, &ctx1);

	for (pl = pwl; pl > 0; pl -= 16)
		UPDATE(&ctx, final, (unsigned int)(pl > 16 ? 16 : pl));

	/* Don't leave anything around in vm they could use. */
	memset(final, 0, sizeof(final));

	/* Then something really weird... */
	for (i = pwl; i != 0; i >>= 1)
		if ((i & 1) != 0)
		    UPDATE(&ctx, final, 1);
		else
		    UPDATE(&ctx, (const unsigned char *)pw, 1);

	/* Now make the output string */
	memcpy(passwd, MD5_MAGIC, MD5_MAGIC_LEN);
	strlcpy(passwd + MD5_MAGIC_LEN, sp, sl + 1);
	strlcat(passwd, "$", sizeof(passwd));

	FINAL(final, &ctx);

	/*
	 * And now, just to make sure things don't run too fast. On a 60 MHz
	 * Pentium this takes 34 msec, so you would need 30 seconds to build
	 * a 1000 entry dictionary...
	 */
	for (i = 0; i < 1000; i++) {
		INIT(&ctx1);

		if ((i & 1) != 0)
			UPDATE(&ctx1, (const unsigned char *)pw, pwl);
		else
			UPDATE(&ctx1, final, 16);

		if ((i % 3) != 0)
			UPDATE(&ctx1, (const unsigned char *)sp, sl);

		if ((i % 7) != 0)
			UPDATE(&ctx1, (const unsigned char *)pw, pwl);

		if ((i & 1) != 0)
			UPDATE(&ctx1, final, 16);
		else
			UPDATE(&ctx1, (const unsigned char *)pw, pwl);

		FINAL(final, &ctx1);
	}

	p = passwd + sl + MD5_MAGIC_LEN + 1;

	l = (final[ 0]<<16) | (final[ 6]<<8) | final[12]; to64(p,l,4); p += 4;
	l = (final[ 1]<<16) | (final[ 7]<<8) | final[13]; to64(p,l,4); p += 4;
	l = (final[ 2]<<16) | (final[ 8]<<8) | final[14]; to64(p,l,4); p += 4;
	l = (final[ 3]<<16) | (final[ 9]<<8) | final[15]; to64(p,l,4); p += 4;
	l = (final[ 4]<<16) | (final[10]<<8) | final[ 5]; to64(p,l,4); p += 4;
	l =		       final[11]		; to64(p,l,2); p += 2;
	*p = '\0';

	/* Don't leave anything around in vm they could use. */
	memset(final, 0, sizeof(final));
	return (passwd);
}
