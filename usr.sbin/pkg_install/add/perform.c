/*	$NetBSD: perform.c,v 1.69 2002/07/19 19:04:34 yamt Exp $	*/

#include <sys/cdefs.h>
#ifndef lint
#if 0
static const char *rcsid = "from FreeBSD Id: perform.c,v 1.44 1997/10/13 15:03:46 jkh Exp";
#else
__RCSID("$NetBSD: perform.c,v 1.69 2002/07/19 19:04:34 yamt Exp $");
#endif
#endif

/*
 * FreeBSD install - a package for the installation and maintainance
 * of non-core utilities.
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
 * Jordan K. Hubbard
 * 18 July 1993
 *
 * This is the main body of the add module.
 *
 */

#include <assert.h>
#include <err.h>
#include "lib.h"
#include "add.h"
#include "verify.h"

#include <signal.h>
#include <string.h>
#include <sys/wait.h>

static char LogDir[FILENAME_MAX];
static int zapLogDir;		/* Should we delete LogDir? */

static package_t Plist;
static char *Home;

static int
sanity_check(const char *pkg)
{
	int     errc = 0;

	if (!fexists(CONTENTS_FNAME)) {
		warnx("package %s has no CONTENTS file!", pkg);
		errc = 1;
	} else if (!fexists(COMMENT_FNAME)) {
		warnx("package %s has no COMMENT file!", pkg);
		errc = 1;
	} else if (!fexists(DESC_FNAME)) {
		warnx("package %s has no DESC file!", pkg);
		errc = 1;
	}
	return errc;
}

/* install a pre-requisite package. Returns 1 if it installed it */
static int
installprereq(const char *name, int *errc)
{
	int	ret;
	ret = 0;

	if (Verbose)
		printf("Loading it from %s.\n", name);
	path_setenv("PKG_PATH");
	if (vsystem("%s/pkg_add -s %s %s%s%s %s%s",
			BINDIR,
			get_verification(),
			Force ? "-f " : "",
			Prefix ? "-p " : "",
			Prefix ? Prefix : "",
			Verbose ? "-v " : "",
			name)) {
		warnx("autoload of dependency `%s' failed%s",
			name, Force ? " (proceeding anyway)" : "!");
		if (!Force)
			++(*errc);
	} else {
		ret = 1;
	}

	return ret;
}

/*
 * This is seriously ugly code following.  Written very fast!
 * [And subsequently made even worse..  Sigh!  This code was just born
 * to be hacked, I guess.. :) -jkh]
 */
static int
pkg_do(const char *pkg)
{
	char    playpen[FILENAME_MAX];
	char    extract_contents[FILENAME_MAX];
	char    upgrade_from[FILENAME_MAX];
	char    upgrade_via[FILENAME_MAX];
	char    upgrade_to[FILENAME_MAX];
	int	upgrading = 0;
	char   *where_to, *tmp, *extract;
	char   *dbdir;
	const char *exact;
	FILE   *cfile;
	int     errc;
	plist_t *p;
	struct stat sb;
	int     inPlace;
	int	rc;

	errc = 0;
	zapLogDir = 0;
	LogDir[0] = '\0';
	strcpy(playpen, FirstPen);
	inPlace = 0;
	dbdir = (tmp = getenv(PKG_DBDIR)) ? tmp : DEF_LOG_DIR;

	/* make sure dbdir actually exists! */
	if (!(isdir(dbdir) || islinktodir(dbdir))) {
		if (vsystem("/bin/mkdir -p -m 755 %s", dbdir)) {
			errx(1, "Database-dir %s cannot be generated, aborting.",
			    dbdir);
		}
	}

	/* Are we coming in for a second pass, everything already extracted?
	 * (Slave mode) */
	if (!pkg) {
		fgets(playpen, FILENAME_MAX, stdin);
		playpen[strlen(playpen) - 1] = '\0';	/* remove newline! */
		if (chdir(playpen) == FAIL) {
			warnx("add in SLAVE mode can't chdir to %s", playpen);
			return 1;
		}
		read_plist(&Plist, stdin);
		where_to = playpen;
	}
	/* Nope - do it now */
	else {
		const char *tmppkg;

		tmppkg = fileFindByPath(pkg);
		if (tmppkg == NULL) {
			warnx("no pkg found for '%s', sorry.", pkg);
			return 1;
		}

		pkg = tmppkg;

		if (IS_URL(pkg)) {
			Home = fileGetURL(pkg);
			if (Home == NULL) {
				warnx("unable to fetch `%s' by URL", pkg);
			}
			where_to = Home;

			/* make sure the pkg is verified */
			if (!verify(pkg)) {
				warnx("Package %s will not be extracted", pkg);
				goto bomb;
			}
		}
		else { /* local */
			if (!IS_STDIN(pkg)) {
			        /* not stdin */
				if (!ispkgpattern(pkg)) {
					if (stat(pkg, &sb) == FAIL) {
						warnx("can't stat package file '%s'", pkg);
						goto bomb;
					}
					/* make sure the pkg is verified */
					if (!verify(pkg)) {
						warnx("Package %s will not be extracted", pkg);
						goto bomb;
					}
				}
				(void) snprintf(extract_contents, sizeof(extract_contents), "--fast-read %s", CONTENTS_FNAME);
				extract = extract_contents;
			} else {
			        /* some values for stdin */
				extract = NULL;
				sb.st_size = 100000;	/* Make up a plausible average size */
			}
			Home = make_playpen(playpen, sizeof(playpen), sb.st_size * 4);
			if (!Home)
				warnx("unable to make playpen for %ld bytes",
				      (long) (sb.st_size * 4));
			where_to = Home;
			if (unpack(pkg, extract)) {
				warnx("unable to extract table of contents file from `%s' - not a package?",
				      pkg);
				goto bomb;
			}
		}

		cfile = fopen(CONTENTS_FNAME, "r");
		if (!cfile) {
			warnx("unable to open table of contents file `%s' - not a package?",
			      CONTENTS_FNAME);
			goto bomb;
		}
		read_plist(&Plist, cfile);
		fclose(cfile);

		if (!IS_URL(pkg)) {
			/* Extract directly rather than moving?  Oh goodie! */
			if (find_plist_option(&Plist, "extract-in-place")) {
				if (Verbose)
					printf("Doing in-place extraction for %s\n", pkg);
				p = find_plist(&Plist, PLIST_CWD);
				if (p) {
					if (!(isdir(p->name) || islinktodir(p->name)) && !Fake) {
						if (Verbose)
							printf("Desired prefix of %s does not exist, creating.\n", p->name);
						(void) vsystem("mkdir -p %s", p->name);
					}
					if (chdir(p->name) == -1) {
						warn("unable to change directory to `%s'", p->name);
						goto bomb;
					}
					where_to = p->name;
					inPlace = 1;
				} else {
					warnx(
					    "no prefix specified in `%s' - this is a bad package!",
					    pkg);
					goto bomb;
				}
			}

			/*
			 * Apply a crude heuristic to see how much space the package will
			 * take up once it's unpacked.  I've noticed that most packages
			 * compress an average of 75%, so multiply by 4 for good measure.
			 */

			if (!inPlace && min_free(playpen) < sb.st_size * 4) {
				warnx("projected size of %ld exceeds available free space.\n"
				    "Please set your PKG_TMPDIR variable to point to a location with more\n"
				    "free space and try again", (long) (sb.st_size * 4));
				warnx("not extracting %s\ninto %s, sorry!",
				    pkg, where_to);
				goto bomb;
			}

			/* If this is a direct extract and we didn't want it, stop now */
			if (inPlace && Fake)
				goto success;

			/* Finally unpack the whole mess */
			if (unpack(pkg, NULL)) {
				warnx("unable to extract `%s'!", pkg);
				goto bomb;
			}
		}

		/* Check for sanity */
		if (sanity_check(pkg))
			goto bomb;

		/* If we're running in MASTER mode, just output the plist and return */
		if (AddMode == MASTER) {
			printf("%s\n", where_playpen());
			write_plist(&Plist, stdout, NULL);
			return 0;
		}
	}

	/*
         * If we have a prefix, delete the first one we see and add this
         * one in place of it.
         */
	if (Prefix) {
		delete_plist(&Plist, FALSE, PLIST_CWD, NULL);
		add_plist_top(&Plist, PLIST_CWD, Prefix);
	}

	setenv(PKG_PREFIX_VNAME, (p = find_plist(&Plist, PLIST_CWD)) ? p->name : ".", 1);
	/* Protect against old packages with bogus @name fields */
	PkgName = (p = find_plist(&Plist, PLIST_NAME)) ? p->name : "anonymous";

	/* See if this package (exact version) is already registered */
	(void) snprintf(LogDir, sizeof(LogDir), "%s/%s", dbdir, PkgName);
	if ((isdir(LogDir) || islinktodir(LogDir)) && !Force) {
		warnx("package `%s' already recorded as installed", PkgName);
		goto success;	/* close enough for government work */
	}

	/* See if some other version of us is already installed */
	{
		char   *s;

		if ((s = strrchr(PkgName, '-')) != NULL) {
			char    buf[FILENAME_MAX];
			char    installed[FILENAME_MAX];

			(void) snprintf(buf, sizeof(buf), "%.*s[0-9]*",
				(int)(s - PkgName) + 1, PkgName);
			if (findmatchingname(dbdir, buf, note_whats_installed, installed) > 0) {
				if (upgrade) {
					snprintf(upgrade_from, sizeof(upgrade_from), "%s/%s/" REQUIRED_BY_FNAME,
						 dbdir, installed);
					snprintf(upgrade_via, sizeof(upgrade_via), "%s/.%s." REQUIRED_BY_FNAME,
						 dbdir, installed);
					snprintf(upgrade_to, sizeof(upgrade_to), "%s/%s/" REQUIRED_BY_FNAME,
						 dbdir, PkgName);

					if (Verbose)
						printf("Upgrading %s to %s.\n", installed, PkgName);

					if (fexists(upgrade_from)) {  /* Are there any dependencies? */
						if (Verbose)
							printf("mv %s %s\n", upgrade_from, upgrade_via);
						rc = rename(upgrade_from, upgrade_via);
						assert(rc == 0);
						
						upgrading = 1;
					}

					if (Verbose)
						printf("pkg_delete '%s'\n", installed);
					vsystem("pkg_delete '%s'\n", installed);
					
				} else {
					warnx("other version '%s' already installed", installed);

					errc = 1;
					goto success;	/* close enough for government work */
				}
			}
		}
	}

	/* See if there are conflicting packages installed */
	for (p = Plist.head; p; p = p->next) {
		char    installed[FILENAME_MAX];

		if (p->type != PLIST_PKGCFL)
			continue;
		if (Verbose)
			printf("Package `%s' conflicts with `%s'.\n", PkgName, p->name);

		/* was: */
		/* if (!vsystem("/usr/sbin/pkg_info -qe '%s'", p->name)) { */
		if (findmatchingname(dbdir, p->name, note_whats_installed, installed) > 0) {
			warnx("Conflicting package `%s'installed, please use\n"
			      "\t\"pkg_delete %s\" first to remove it!\n", installed, installed);
			++errc;
		}
	}

	/* Quick pre-check if any conflicting dependencies are installed
	 * (e.g. version X is installed, but version Y is required)
	 */
	for (p = Plist.head; p; p = p->next) {
		char installed[FILENAME_MAX];
		
		if (p->type != PLIST_PKGDEP)
			continue;
		if (Verbose)
			printf("Depends pre-scan: `%s' required.\n", p->name);
		/* if (vsystem("/usr/sbin/pkg_info -qe '%s'", p->name)) { */
		if (findmatchingname(dbdir, p->name, note_whats_installed, installed) <= 0) {
			/* 
			 * required pkg not found. look if it's available with a more liberal
			 * pattern. If so, this will lead to problems later (check on "some
			 * other version of us is already installed" will fail, see above),
			 * and we better stop right now.
			 */
			char *s;
			int skip = -1;

			/* doing this right required to parse the full version(s),
			 * do a 99% solution here for now */
			if (strchr(p->name, '{'))
				continue;	/* would remove trailing '}' else */

			if ((s = strpbrk(p->name, "<>")) != NULL) {
				skip = 0;
			} else if ((s = strrchr(p->name, '-')) != NULL) {
				skip = 1;
			}
			
			if (skip >= 0) {
				char    buf[FILENAME_MAX];
		
				(void) snprintf(buf, sizeof(buf),
				    skip ? "%.*s[0-9]*" : "%.*s-[0-9]*",
				    (int)(s - p->name) + skip, p->name);
				if (findmatchingname(dbdir, buf, note_whats_installed, installed) > 0) {
					warnx("pkg `%s' required, but `%s' found installed.",
					      p->name, installed);

					if (upgrading) {
						printf("HF: upgrade note -- could 'pkg_delete %s', and let the normal\n"
						       "dependency handling reinstall the updated package, assuming one IS\n"
						       "available. But then I'd expect proper binary pkgs being available for\n"
						       "the upgrade case.\n", installed);
					}

					if (Force) {
						warnx("Proceeding anyways.");
					} else {	
						warnx("Please resolve this conflict!");
						errc = 1;
						goto success; /* close enough */
					}
				}
			}
		}
	}
	

	/* Now check the packing list for dependencies */
	for (exact = NULL, p = Plist.head; p; p = p->next) {
		char    installed[FILENAME_MAX];

		if (p->type == PLIST_BLDDEP) {
			exact = p->name;
			continue;
		}
		if (p->type != PLIST_PKGDEP) {
			exact = NULL;
			continue;
		}
		if (Verbose)
			printf("Package `%s' depends on `%s'.\n", PkgName, p->name);

		if (findmatchingname(dbdir, p->name, note_whats_installed, installed) != 1) {
			/* required pkg not found - need to pull in */

			if (Fake) {
			        /* fake install (???) */
				if (Verbose)
					printf("Package dependency %s for %s not installed%s\n", p->name, pkg,
					    Force ? " (proceeding anyway)" : "!");
			} else {
				int done = 0;
				int errc0 = 0;

				if (exact != NULL) {
					/* first try the exact name, from the @blddep */
					done = installprereq(exact, &errc0);
				}
				if (!done) {
					done = installprereq(p->name, &errc0);
				}
				if (!done && !Force) {
					errc += errc0;
				}
			}
		} else if (Verbose) {
			printf(" - %s already installed.\n", installed);
		}
	}

	if (errc != 0)
		goto bomb;

	/* Look for the requirements file */
	if (fexists(REQUIRE_FNAME)) {
		vsystem("%s +x %s", CHMOD, REQUIRE_FNAME);	/* be sure */
		if (Verbose)
			printf("Running requirements file first for %s.\n", PkgName);
		if (!Fake && vsystem("./%s %s INSTALL", REQUIRE_FNAME, PkgName)) {
			warnx("package %s fails requirements %s", pkg,
			    Force ? "installing anyway" : "- not installed");
			if (!Force) {
				errc = 1;
				goto success;	/* close enough for government work */
			}
		}
	}
	
	/* If we're really installing, and have an installation file, run it */
	if (!NoInstall && fexists(INSTALL_FNAME)) {
		vsystem("%s +x %s", CHMOD, INSTALL_FNAME);	/* make sure */
		if (Verbose)
			printf("Running install with PRE-INSTALL for %s.\n", PkgName);
		if (!Fake && vsystem("./%s %s PRE-INSTALL", INSTALL_FNAME, PkgName)) {
			warnx("install script returned error status");
			errc = 1;
			goto success;	/* nothing to uninstall yet */
		}
	}

	/* Now finally extract the entire show if we're not going direct */
	if (!inPlace && !Fake)
	    if (!extract_plist(".", &Plist)) {
		errc = 1;
		goto fail;
	    }

	if (!Fake && fexists(MTREE_FNAME)) {
		if (Verbose)
			printf("Running mtree for %s.\n", PkgName);
		p = find_plist(&Plist, PLIST_CWD);
		if (Verbose)
			printf("mtree -U -f %s -d -e -p %s\n", MTREE_FNAME, p ? p->name : "/");
		if (!Fake) {
			if (vsystem("%s/mtree -U -f %s -d -e -p %s", BINDIR, MTREE_FNAME, p ? p->name : "/"))
				warnx("mtree returned a non-zero status - continuing");
		}
		unlink(MTREE_FNAME); /* remove this line to tar up pkg later  - HF */
	}

	/* Run the installation script one last time? */
	if (!NoInstall && fexists(INSTALL_FNAME)) {
		if (Verbose)
			printf("Running install with POST-INSTALL for %s.\n", PkgName);
		if (!Fake && vsystem("./%s %s POST-INSTALL", INSTALL_FNAME, PkgName)) {
			warnx("install script returned error status");
			errc = 1;
			goto fail;
		}
	}

	/* Time to record the deed? */
	if (!NoRecord && !Fake) {
		char    contents[FILENAME_MAX];

		umask(022);
		if (getuid() != 0)
			warnx("not running as root - trying to record install anyway");
		if (!PkgName) {
			warnx("no package name! can't record package, sorry");
			errc = 1;
			goto success;	/* well, partial anyway */
		}
		(void) snprintf(LogDir, sizeof(LogDir), "%s/%s", dbdir, PkgName);
		zapLogDir = 1; /* LogDir contains something valid now */
		if (Verbose)
			printf("Attempting to record package into %s.\n", LogDir);
		if (make_hierarchy(LogDir)) {
			warnx("can't record package into '%s', you're on your own!",
			    LogDir);
			memset(LogDir, 0, sizeof(LogDir));
			errc = 1;
			goto success;	/* close enough for government work */
		}
		/* Make sure pkg_info can read the entry */
		vsystem("%s a+rx %s", CHMOD, LogDir);
		if (fexists(DEINSTALL_FNAME))
			move_file(".", DEINSTALL_FNAME, LogDir);
		if (fexists(REQUIRE_FNAME))
			move_file(".", REQUIRE_FNAME, LogDir);
		if (fexists(SIZE_PKG_FNAME))
			move_file(".", SIZE_PKG_FNAME, LogDir);
		if (fexists(SIZE_ALL_FNAME))
			move_file(".", SIZE_ALL_FNAME, LogDir);
		(void) snprintf(contents, sizeof(contents), "%s/%s", LogDir, CONTENTS_FNAME);
		cfile = fopen(contents, "w");
		if (!cfile) {
			warnx("can't open new contents file '%s'! can't register pkg",
			    contents);
			goto success;	/* can't log, but still keep pkg */
		}
		write_plist(&Plist, cfile, NULL);
		fclose(cfile);
		move_file(".", DESC_FNAME, LogDir);
		move_file(".", COMMENT_FNAME, LogDir);
		if (fexists(BUILD_VERSION_FNAME))
			move_file(".", BUILD_VERSION_FNAME, LogDir);
		if (fexists(BUILD_INFO_FNAME))
			move_file(".", BUILD_INFO_FNAME, LogDir);
		if (fexists(DISPLAY_FNAME))
			move_file(".", DISPLAY_FNAME, LogDir);

		/* register dependencies */
		/* we could save some cycles here if we remembered what we
		 * installed above (in case we got a wildcard dependency)  */
		/* XXX remembering in p->name would NOT be good! */
		for (p = Plist.head; p; p = p->next) {
			if (p->type != PLIST_PKGDEP)
				continue;
			if (Verbose)
				printf("Attempting to record dependency on package `%s'\n", p->name);
			(void) snprintf(contents, sizeof(contents), "%s/%s", dbdir,
			    basename_of(p->name));
			if (ispkgpattern(p->name)) {
				char   *s;
				s = findbestmatchingname(dirname_of(contents),
				    basename_of(contents));
				if (s != NULL) {
					char   *t;
					t = strrchr(contents, '/');
					strcpy(t + 1, s);
					free(s);
				} else {
					errx(1, "Where did our dependency go?!");
					/* this shouldn't happen... X-) */
				}
			}
			strcat(contents, "/");
			strcat(contents, REQUIRED_BY_FNAME);

			cfile = fopen(contents, "a");
			if (!cfile)
				warnx("can't open dependency file '%s'!\n"
				    "dependency registration is incomplete", contents);
			else {
				fprintf(cfile, "%s\n", PkgName);
				if (fclose(cfile) == EOF)
					warnx("cannot properly close file %s", contents);
			}
		}
		if (Verbose)
			printf("Package %s registered in %s\n", PkgName, LogDir);
	}

	if ((p = find_plist(&Plist, PLIST_DISPLAY)) != NULL) {
		FILE   *fp;
		char    buf[BUFSIZ];

		(void) snprintf(buf, sizeof(buf), "%s/%s", LogDir, p->name);
		fp = fopen(buf, "r");
		if (fp) {
			putc('\n', stdout);
			while (fgets(buf, sizeof(buf), fp))
				fputs(buf, stdout);
			putc('\n', stdout);
			(void) fclose(fp);
		} else
			warnx("cannot open %s as display file", buf);
	}

	goto success;

bomb:
	errc = 1;
	goto success;

fail:
	/* Nuke the whole (installed) show, XXX but don't clean directories */
	if (!Fake)
		delete_package(FALSE, FALSE, &Plist);

success:
	/* delete the packing list contents */
	free_plist(&Plist);
	leave_playpen(Home);

	if (upgrading) {
		rc = rename(upgrade_via, upgrade_to);
		assert(rc == 0);
	}

	return errc;
}

void
cleanup(int signo)
{
	static int alreadyCleaning;
	void    (*oldint) (int);
	void    (*oldhup) (int);
	oldint = signal(SIGINT, SIG_IGN);
	oldhup = signal(SIGHUP, SIG_IGN);

	if (!alreadyCleaning) {
		alreadyCleaning = 1;
		if (signo)
			printf("Signal %d received, cleaning up.\n", signo);
		if (!Fake && zapLogDir && LogDir[0])
			vsystem("%s -rf %s", REMOVE_CMD, LogDir);
		leave_playpen(Home);
		if (signo)
			exit(1);
	}
	signal(SIGINT, oldint);
	signal(SIGHUP, oldhup);
}

int
pkg_perform(lpkg_head_t *pkgs)
{
	int     err_cnt = 0;
	lpkg_t *lpp;

	signal(SIGINT, cleanup);
	signal(SIGHUP, cleanup);

	if (AddMode == SLAVE)
		err_cnt = pkg_do(NULL);
	else {
		while ((lpp = TAILQ_FIRST(pkgs)) != NULL) {
			err_cnt += pkg_do(lpp->lp_name);
			TAILQ_REMOVE(pkgs, lpp, lp_link);
			free_lpkg(lpp);
		}
	}
	
	ftp_stop();
	
	return err_cnt;
}
