/*	$NetBSD: ibcs2_sysent.c,v 1.13 1998/01/09 06:21:31 thorpej Exp $	*/

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.10 1996/09/07 14:21:59 mycroft Exp 
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/ibcs2/ibcs2_types.h>
#include <compat/ibcs2/ibcs2_signal.h>
#include <compat/ibcs2/ibcs2_syscallargs.h>
#include <compat/ibcs2/ibcs2_statfs.h>

#ifdef COMPAT_43
#define compat_43(func) __CONCAT(compat_43_,func)
#else
#define compat_43(func) sys_nosys
#endif

#ifdef COMPAT_09
#define compat_09(func) __CONCAT(compat_09_,func)
#else
#define compat_09(func) sys_nosys
#endif

#ifdef COMPAT_10
#define compat_10(func) __CONCAT(compat_10_,func)
#else
#define compat_10(func) sys_nosys
#endif

#define	s(type)	sizeof(type)

struct sysent ibcs2_sysent[] = {
	{ 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args),
	    sys_exit },				/* 1 = exit */
	{ 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct ibcs2_sys_read_args),
	    ibcs2_sys_read },			/* 3 = read */
	{ 3, s(struct sys_write_args),
	    sys_write },			/* 4 = write */
	{ 3, s(struct ibcs2_sys_open_args),
	    ibcs2_sys_open },			/* 5 = open */
	{ 1, s(struct sys_close_args),
	    sys_close },			/* 6 = close */
	{ 3, s(struct ibcs2_sys_waitsys_args),
	    ibcs2_sys_waitsys },		/* 7 = waitsys */
	{ 2, s(struct ibcs2_sys_creat_args),
	    ibcs2_sys_creat },			/* 8 = creat */
	{ 2, s(struct sys_link_args),
	    sys_link },				/* 9 = link */
	{ 1, s(struct ibcs2_sys_unlink_args),
	    ibcs2_sys_unlink },			/* 10 = unlink */
	{ 2, s(struct ibcs2_sys_execv_args),
	    ibcs2_sys_execv },			/* 11 = execv */
	{ 1, s(struct ibcs2_sys_chdir_args),
	    ibcs2_sys_chdir },			/* 12 = chdir */
	{ 1, s(struct ibcs2_sys_time_args),
	    ibcs2_sys_time },			/* 13 = time */
	{ 3, s(struct ibcs2_sys_mknod_args),
	    ibcs2_sys_mknod },			/* 14 = mknod */
	{ 2, s(struct ibcs2_sys_chmod_args),
	    ibcs2_sys_chmod },			/* 15 = chmod */
	{ 3, s(struct ibcs2_sys_chown_args),
	    ibcs2_sys_chown },			/* 16 = chown */
	{ 1, s(struct sys_obreak_args),
	    sys_obreak },			/* 17 = obreak */
	{ 2, s(struct ibcs2_sys_stat_args),
	    ibcs2_sys_stat },			/* 18 = stat */
	{ 3, s(struct compat_43_sys_lseek_args),
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0,
	    sys_getpid },			/* 20 = getpid */
	{ 6, s(struct ibcs2_sys_mount_args),
	    ibcs2_sys_mount },			/* 21 = mount */
	{ 1, s(struct ibcs2_sys_umount_args),
	    ibcs2_sys_umount },			/* 22 = umount */
	{ 1, s(struct ibcs2_sys_setuid_args),
	    ibcs2_sys_setuid },			/* 23 = setuid */
	{ 0, 0,
	    sys_getuid },			/* 24 = getuid */
	{ 1, s(struct ibcs2_sys_stime_args),
	    ibcs2_sys_stime },			/* 25 = stime */
	{ 0, 0,
	    sys_nosys },			/* 26 = unimplemented ibcs2_ptrace */
	{ 1, s(struct ibcs2_sys_alarm_args),
	    ibcs2_sys_alarm },			/* 27 = alarm */
	{ 2, s(struct ibcs2_sys_fstat_args),
	    ibcs2_sys_fstat },			/* 28 = fstat */
	{ 0, 0,
	    ibcs2_sys_pause },			/* 29 = pause */
	{ 2, s(struct ibcs2_sys_utime_args),
	    ibcs2_sys_utime },			/* 30 = utime */
	{ 0, 0,
	    sys_nosys },			/* 31 = unimplemented was stty */
	{ 0, 0,
	    sys_nosys },			/* 32 = unimplemented was gtty */
	{ 2, s(struct ibcs2_sys_access_args),
	    ibcs2_sys_access },			/* 33 = access */
	{ 1, s(struct ibcs2_sys_nice_args),
	    ibcs2_sys_nice },			/* 34 = nice */
	{ 4, s(struct ibcs2_sys_statfs_args),
	    ibcs2_sys_statfs },			/* 35 = statfs */
	{ 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct ibcs2_sys_kill_args),
	    ibcs2_sys_kill },			/* 37 = kill */
	{ 4, s(struct ibcs2_sys_fstatfs_args),
	    ibcs2_sys_fstatfs },		/* 38 = fstatfs */
	{ 4, s(struct ibcs2_sys_pgrpsys_args),
	    ibcs2_sys_pgrpsys },		/* 39 = pgrpsys */
	{ 0, 0,
	    sys_nosys },			/* 40 = unimplemented ibcs2_xenix */
	{ 1, s(struct sys_dup_args),
	    sys_dup },				/* 41 = dup */
	{ 0, 0,
	    sys_pipe },				/* 42 = pipe */
	{ 1, s(struct ibcs2_sys_times_args),
	    ibcs2_sys_times },			/* 43 = times */
	{ 0, 0,
	    sys_nosys },			/* 44 = unimplemented profil */
	{ 1, s(struct ibcs2_sys_plock_args),
	    ibcs2_sys_plock },			/* 45 = plock */
	{ 1, s(struct ibcs2_sys_setgid_args),
	    ibcs2_sys_setgid },			/* 46 = setgid */
	{ 0, 0,
	    sys_getgid },			/* 47 = getgid */
	{ 2, s(struct ibcs2_sys_sigsys_args),
	    ibcs2_sys_sigsys },			/* 48 = sigsys */
#ifdef SYSVMSG
	{ 6, s(struct ibcs2_sys_msgsys_args),
	    ibcs2_sys_msgsys },			/* 49 = msgsys */
#else
	{ 0, 0,
	    sys_nosys },			/* 49 = unimplemented msgsys */
#endif
	{ 2, s(struct ibcs2_sys_sysi86_args),
	    ibcs2_sys_sysi86 },			/* 50 = sysi86 */
	{ 0, 0,
	    sys_nosys },			/* 51 = unimplemented ibcs2_acct */
#ifdef SYSVSHM
	{ 4, s(struct ibcs2_sys_shmsys_args),
	    ibcs2_sys_shmsys },			/* 52 = shmsys */
#else
	{ 0, 0,
	    sys_nosys },			/* 52 = unimplemented shmsys */
#endif
#ifdef SYSVSEM
	{ 5, s(struct ibcs2_sys_semsys_args),
	    ibcs2_sys_semsys },			/* 53 = semsys */
#else
	{ 0, 0,
	    sys_nosys },			/* 53 = unimplemented semsys */
#endif
	{ 3, s(struct ibcs2_sys_ioctl_args),
	    ibcs2_sys_ioctl },			/* 54 = ioctl */
	{ 3, s(struct ibcs2_sys_uadmin_args),
	    ibcs2_sys_uadmin },			/* 55 = uadmin */
	{ 0, 0,
	    sys_nosys },			/* 56 = unimplemented */
	{ 3, s(struct ibcs2_sys_utssys_args),
	    ibcs2_sys_utssys },			/* 57 = utssys */
	{ 0, 0,
	    sys_nosys },			/* 58 = unimplemented */
	{ 3, s(struct ibcs2_sys_execve_args),
	    ibcs2_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args),
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args),
	    sys_chroot },			/* 61 = chroot */
	{ 3, s(struct ibcs2_sys_fcntl_args),
	    ibcs2_sys_fcntl },			/* 62 = fcntl */
	{ 2, s(struct ibcs2_sys_ulimit_args),
	    ibcs2_sys_ulimit },			/* 63 = ulimit */
	{ 0, 0,
	    sys_nosys },			/* 64 = unimplemented reserved for unix/pc */
	{ 0, 0,
	    sys_nosys },			/* 65 = unimplemented reserved for unix/pc */
	{ 0, 0,
	    sys_nosys },			/* 66 = unimplemented reserved for unix/pc */
	{ 0, 0,
	    sys_nosys },			/* 67 = unimplemented reserved for unix/pc */
	{ 0, 0,
	    sys_nosys },			/* 68 = unimplemented reserved for unix/pc */
	{ 0, 0,
	    sys_nosys },			/* 69 = unimplemented reserved for unix/pc */
	{ 0, 0,
	    sys_nosys },			/* 70 = obsolete rfs_advfs */
	{ 0, 0,
	    sys_nosys },			/* 71 = obsolete rfs_unadvfs */
	{ 0, 0,
	    sys_nosys },			/* 72 = obsolete rfs_rmount */
	{ 0, 0,
	    sys_nosys },			/* 73 = obsolete rfs_rumount */
	{ 0, 0,
	    sys_nosys },			/* 74 = obsolete rfs_rfstart */
	{ 0, 0,
	    sys_nosys },			/* 75 = obsolete rfs_sigret */
	{ 0, 0,
	    sys_nosys },			/* 76 = obsolete rfs_rdebug */
	{ 0, 0,
	    sys_nosys },			/* 77 = obsolete rfs_rfstop */
	{ 0, 0,
	    sys_nosys },			/* 78 = unimplemented rfs_rfsys */
	{ 1, s(struct ibcs2_sys_rmdir_args),
	    ibcs2_sys_rmdir },			/* 79 = rmdir */
	{ 2, s(struct ibcs2_sys_mkdir_args),
	    ibcs2_sys_mkdir },			/* 80 = mkdir */
	{ 3, s(struct ibcs2_sys_getdents_args),
	    ibcs2_sys_getdents },		/* 81 = getdents */
	{ 0, 0,
	    sys_nosys },			/* 82 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 83 = unimplemented */
	{ 3, s(struct ibcs2_sys_sysfs_args),
	    ibcs2_sys_sysfs },			/* 84 = sysfs */
	{ 4, s(struct ibcs2_sys_getmsg_args),
	    ibcs2_sys_getmsg },			/* 85 = getmsg */
	{ 4, s(struct ibcs2_sys_putmsg_args),
	    ibcs2_sys_putmsg },			/* 86 = putmsg */
	{ 3, s(struct sys_poll_args),
	    sys_poll },				/* 87 = poll */
	{ 0, 0,
	    sys_nosys },			/* 88 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 89 = unimplemented */
	{ 2, s(struct ibcs2_sys_symlink_args),
	    ibcs2_sys_symlink },		/* 90 = symlink */
	{ 2, s(struct ibcs2_sys_lstat_args),
	    ibcs2_sys_lstat },			/* 91 = lstat */
	{ 3, s(struct ibcs2_sys_readlink_args),
	    ibcs2_sys_readlink },		/* 92 = readlink */
	{ 0, 0,
	    sys_nosys },			/* 93 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 94 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 95 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 96 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 97 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 98 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 99 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 100 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 101 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 102 = unimplemented */
	{ 1, s(struct sys_sigreturn_args),
	    sys_sigreturn },			/* 103 = sigreturn */
	{ 0, 0,
	    sys_nosys },			/* 104 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 105 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 106 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 107 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 108 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 109 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 110 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 111 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 112 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 113 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 114 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 115 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 116 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 117 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 118 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 119 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 120 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 121 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 122 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 123 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 124 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 125 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 126 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 127 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 128 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 129 = unimplemented xenix_xlocking */
	{ 0, 0,
	    sys_nosys },			/* 130 = unimplemented xenix_creatsem */
	{ 0, 0,
	    sys_nosys },			/* 131 = unimplemented xenix_opensem */
	{ 0, 0,
	    sys_nosys },			/* 132 = unimplemented xenix_sigsem */
	{ 0, 0,
	    sys_nosys },			/* 133 = unimplemented xenix_waitsem */
	{ 0, 0,
	    sys_nosys },			/* 134 = unimplemented xenix_nbwaitsem */
	{ 1, s(struct xenix_sys_rdchk_args),
	    xenix_sys_rdchk },			/* 135 = rdchk */
	{ 0, 0,
	    sys_nosys },			/* 136 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 137 = unimplemented */
	{ 2, s(struct xenix_sys_chsize_args),
	    xenix_sys_chsize },			/* 138 = chsize */
	{ 1, s(struct xenix_sys_ftime_args),
	    xenix_sys_ftime },			/* 139 = ftime */
	{ 1, s(struct xenix_sys_nap_args),
	    xenix_sys_nap },			/* 140 = nap */
	{ 0, 0,
	    sys_nosys },			/* 141 = unimplemented xenix_sdget */
	{ 0, 0,
	    sys_nosys },			/* 142 = unimplemented xenix_sdfree */
	{ 0, 0,
	    sys_nosys },			/* 143 = unimplemented xenix_sdenter */
	{ 0, 0,
	    sys_nosys },			/* 144 = unimplemented xenix_sdleave */
	{ 0, 0,
	    sys_nosys },			/* 145 = unimplemented xenix_sdgetv */
	{ 0, 0,
	    sys_nosys },			/* 146 = unimplemented xenix_sdwaitv */
	{ 0, 0,
	    sys_nosys },			/* 147 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 148 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 149 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 150 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 151 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 152 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 153 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 154 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 155 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 156 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 157 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 158 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 159 = unimplemented */
	{ 0, 0,
	    sys_nosys },			/* 160 = unimplemented xenix_proctl */
	{ 0, 0,
	    sys_nosys },			/* 161 = unimplemented xenix_execseg */
	{ 0, 0,
	    sys_nosys },			/* 162 = unimplemented xenix_unexecseg */
	{ 0, 0,
	    sys_nosys },			/* 163 = unimplemented */
	{ 5, s(struct sys_select_args),
	    sys_select },			/* 164 = select */
	{ 2, s(struct ibcs2_sys_eaccess_args),
	    ibcs2_sys_eaccess },		/* 165 = eaccess */
	{ 0, 0,
	    sys_nosys },			/* 166 = unimplemented xenix_paccess */
	{ 3, s(struct ibcs2_sys_sigaction_args),
	    ibcs2_sys_sigaction },		/* 167 = sigaction */
	{ 3, s(struct ibcs2_sys_sigprocmask_args),
	    ibcs2_sys_sigprocmask },		/* 168 = sigprocmask */
	{ 1, s(struct ibcs2_sys_sigpending_args),
	    ibcs2_sys_sigpending },		/* 169 = sigpending */
	{ 1, s(struct ibcs2_sys_sigsuspend_args),
	    ibcs2_sys_sigsuspend },		/* 170 = sigsuspend */
	{ 2, s(struct ibcs2_sys_getgroups_args),
	    ibcs2_sys_getgroups },		/* 171 = getgroups */
	{ 2, s(struct ibcs2_sys_setgroups_args),
	    ibcs2_sys_setgroups },		/* 172 = setgroups */
	{ 1, s(struct ibcs2_sys_sysconf_args),
	    ibcs2_sys_sysconf },		/* 173 = sysconf */
	{ 2, s(struct ibcs2_sys_pathconf_args),
	    ibcs2_sys_pathconf },		/* 174 = pathconf */
	{ 2, s(struct ibcs2_sys_fpathconf_args),
	    ibcs2_sys_fpathconf },		/* 175 = fpathconf */
	{ 2, s(struct ibcs2_sys_rename_args),
	    ibcs2_sys_rename },			/* 176 = rename */
};

