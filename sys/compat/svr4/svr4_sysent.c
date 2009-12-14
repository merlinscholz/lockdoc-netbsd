/* $NetBSD: svr4_sysent.c,v 1.87 2009/12/14 00:58:37 matt Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.61 2009/01/13 22:27:44 pooka Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: svr4_sysent.c,v 1.87 2009/12/14 00:58:37 matt Exp $");

#if defined(_KERNEL_OPT)
#include "opt_ntp.h"
#include "opt_sysv.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/sched.h>
#include <sys/syscallargs.h>
#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_time.h>
#include <compat/svr4/svr4_signal.h>
#include <compat/svr4/svr4_ucontext.h>
#include <compat/svr4/svr4_lwp.h>
#include <compat/svr4/svr4_syscallargs.h>
#include <compat/svr4/svr4_statvfs.h>
#include <compat/svr4/svr4_resource.h>
#include <compat/svr4/svr4_acl.h>
#include <compat/svr4/svr4_schedctl.h>

#define	s(type)	sizeof(type)
#define	n(type)	(sizeof(type)/sizeof (register_t))
#define	ns(type)	n(type), s(type)

struct sysent svr4_sysent[] = {
	{ 0, 0, 0,
	    (sy_call_t *)sys_nosys },		/* 0 = syscall */
	{ ns(struct sys_exit_args), 0,
	    (sy_call_t *)sys_exit },		/* 1 = exit */
	{ 0, 0, 0,
	    (sy_call_t *)sys_fork },		/* 2 = fork */
	{ ns(struct sys_read_args), 0,
	    (sy_call_t *)sys_read },		/* 3 = read */
	{ ns(struct sys_write_args), 0,
	    (sy_call_t *)sys_write },		/* 4 = write */
	{ ns(struct svr4_sys_open_args), 0,
	    (sy_call_t *)svr4_sys_open },	/* 5 = open */
	{ ns(struct sys_close_args), 0,
	    (sy_call_t *)sys_close },		/* 6 = close */
	{ ns(struct svr4_sys_wait_args), 0,
	    (sy_call_t *)svr4_sys_wait },	/* 7 = wait */
	{ ns(struct svr4_sys_creat_args), 0,
	    (sy_call_t *)svr4_sys_creat },	/* 8 = creat */
	{ ns(struct sys_link_args), 0,
	    (sy_call_t *)sys_link },		/* 9 = link */
	{ ns(struct sys_unlink_args), 0,
	    (sy_call_t *)sys_unlink },		/* 10 = unlink */
	{ ns(struct svr4_sys_execv_args), 0,
	    (sy_call_t *)svr4_sys_execv },	/* 11 = execv */
	{ ns(struct sys_chdir_args), 0,
	    (sy_call_t *)sys_chdir },		/* 12 = chdir */
	{ ns(struct svr4_sys_time_args), 0,
	    (sy_call_t *)svr4_sys_time },	/* 13 = time */
	{ ns(struct svr4_sys_mknod_args), 0,
	    (sy_call_t *)svr4_sys_mknod },	/* 14 = mknod */
	{ ns(struct sys_chmod_args), 0,
	    (sy_call_t *)sys_chmod },		/* 15 = chmod */
	{ ns(struct sys___posix_chown_args), 0,
	    (sy_call_t *)sys___posix_chown },	/* 16 = chown */
	{ ns(struct svr4_sys_break_args), 0,
	    (sy_call_t *)svr4_sys_break },	/* 17 = break */
	{ ns(struct svr4_sys_stat_args), 0,
	    (sy_call_t *)svr4_sys_stat },	/* 18 = stat */
	{ ns(struct compat_43_sys_lseek_args), 0,
	    (sy_call_t *)compat_43_sys_lseek },	/* 19 = lseek */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getpid },		/* 20 = getpid */
	{ 0, 0, 0,
	    sys_nosys },			/* 21 = unimplemented old_mount */
	{ 0, 0, 0,
	    sys_nosys },			/* 22 = unimplemented System V umount */
	{ ns(struct sys_setuid_args), 0,
	    (sy_call_t *)sys_setuid },		/* 23 = setuid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getuid_with_euid },/* 24 = getuid_with_euid */
	{ 0, 0, 0,
	    sys_nosys },			/* 25 = unimplemented stime */
	{ 0, 0, 0,
	    sys_nosys },			/* 26 = unimplemented ptrace */
	{ ns(struct svr4_sys_alarm_args), 0,
	    (sy_call_t *)svr4_sys_alarm },	/* 27 = alarm */
	{ ns(struct svr4_sys_fstat_args), 0,
	    (sy_call_t *)svr4_sys_fstat },	/* 28 = fstat */
	{ 0, 0, 0,
	    (sy_call_t *)svr4_sys_pause },	/* 29 = pause */
	{ ns(struct svr4_sys_utime_args), 0,
	    (sy_call_t *)svr4_sys_utime },	/* 30 = utime */
	{ 0, 0, 0,
	    sys_nosys },			/* 31 = unimplemented was stty */
	{ 0, 0, 0,
	    sys_nosys },			/* 32 = unimplemented was gtty */
	{ ns(struct svr4_sys_access_args), 0,
	    (sy_call_t *)svr4_sys_access },	/* 33 = access */
	{ ns(struct svr4_sys_nice_args), 0,
	    (sy_call_t *)svr4_sys_nice },	/* 34 = nice */
	{ 0, 0, 0,
	    sys_nosys },			/* 35 = unimplemented statfs */
	{ 0, 0, 0,
	    (sy_call_t *)sys_sync },		/* 36 = sync */
	{ ns(struct svr4_sys_kill_args), 0,
	    (sy_call_t *)svr4_sys_kill },	/* 37 = kill */
	{ 0, 0, 0,
	    sys_nosys },			/* 38 = unimplemented fstatfs */
	{ ns(struct svr4_sys_pgrpsys_args), 0,
	    (sy_call_t *)svr4_sys_pgrpsys },	/* 39 = pgrpsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 40 = unimplemented xenix */
	{ ns(struct sys_dup_args), 0,
	    (sy_call_t *)sys_dup },		/* 41 = dup */
	{ 0, 0, 0,
	    (sy_call_t *)sys_pipe },		/* 42 = pipe */
	{ ns(struct svr4_sys_times_args), 0,
	    (sy_call_t *)svr4_sys_times },	/* 43 = times */
	{ 0, 0, 0,
	    sys_nosys },			/* 44 = unimplemented profil */
	{ 0, 0, 0,
	    sys_nosys },			/* 45 = unimplemented plock */
	{ ns(struct sys_setgid_args), 0,
	    (sy_call_t *)sys_setgid },		/* 46 = setgid */
	{ 0, 0, 0,
	    (sy_call_t *)sys_getgid_with_egid },/* 47 = getgid_with_egid */
	{ ns(struct svr4_sys_signal_args), 0,
	    (sy_call_t *)svr4_sys_signal },	/* 48 = signal */
#ifdef SYSVMSG
	{ ns(struct svr4_sys_msgsys_args), 0,
	    (sy_call_t *)svr4_sys_msgsys },	/* 49 = msgsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 49 = unimplemented msgsys */
#endif
	{ ns(struct svr4_sys_sysarch_args), 0,
	    (sy_call_t *)svr4_sys_sysarch },	/* 50 = sysarch */
	{ 0, 0, 0,
	    sys_nosys },			/* 51 = unimplemented acct */
#ifdef SYSVSHM
	{ ns(struct svr4_sys_shmsys_args), 0,
	    (sy_call_t *)svr4_sys_shmsys },	/* 52 = shmsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 52 = unimplemented shmsys */
#endif
#ifdef SYSVSEM
	{ ns(struct svr4_sys_semsys_args), 0,
	    (sy_call_t *)svr4_sys_semsys },	/* 53 = semsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 53 = unimplemented semsys */
#endif
	{ ns(struct svr4_sys_ioctl_args), 0,
	    (sy_call_t *)svr4_sys_ioctl },	/* 54 = ioctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 55 = unimplemented uadmin */
	{ 0, 0, 0,
	    sys_nosys },			/* 56 = unimplemented exch */
	{ ns(struct svr4_sys_utssys_args), 0,
	    (sy_call_t *)svr4_sys_utssys },	/* 57 = utssys */
	{ ns(struct sys_fsync_args), 0,
	    (sy_call_t *)sys_fsync },		/* 58 = fsync */
	{ ns(struct svr4_sys_execve_args), 0,
	    (sy_call_t *)svr4_sys_execve },	/* 59 = execve */
	{ ns(struct sys_umask_args), 0,
	    (sy_call_t *)sys_umask },		/* 60 = umask */
	{ ns(struct sys_chroot_args), 0,
	    (sy_call_t *)sys_chroot },		/* 61 = chroot */
	{ ns(struct svr4_sys_fcntl_args), 0,
	    (sy_call_t *)svr4_sys_fcntl },	/* 62 = fcntl */
	{ ns(struct svr4_sys_ulimit_args), 0,
	    (sy_call_t *)svr4_sys_ulimit },	/* 63 = ulimit */
	{ 0, 0, 0,
	    sys_nosys },			/* 64 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 65 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 66 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 67 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 68 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 69 = unimplemented reserved for unix/pc */
	{ 0, 0, 0,
	    sys_nosys },			/* 70 = obsolete advfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 71 = obsolete unadvfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 72 = obsolete rmount */
	{ 0, 0, 0,
	    sys_nosys },			/* 73 = obsolete rumount */
	{ 0, 0, 0,
	    sys_nosys },			/* 74 = obsolete rfstart */
	{ 0, 0, 0,
	    sys_nosys },			/* 75 = obsolete sigret */
	{ 0, 0, 0,
	    sys_nosys },			/* 76 = obsolete rdebug */
	{ 0, 0, 0,
	    sys_nosys },			/* 77 = obsolete rfstop */
	{ 0, 0, 0,
	    sys_nosys },			/* 78 = unimplemented rfsys */
	{ ns(struct sys_rmdir_args), 0,
	    (sy_call_t *)sys_rmdir },		/* 79 = rmdir */
	{ ns(struct sys_mkdir_args), 0,
	    (sy_call_t *)sys_mkdir },		/* 80 = mkdir */
	{ ns(struct svr4_sys_getdents_args), 0,
	    (sy_call_t *)svr4_sys_getdents },	/* 81 = getdents */
	{ 0, 0, 0,
	    sys_nosys },			/* 82 = obsolete libattach */
	{ 0, 0, 0,
	    sys_nosys },			/* 83 = obsolete libdetach */
	{ 0, 0, 0,
	    sys_nosys },			/* 84 = unimplemented sysfs */
	{ ns(struct svr4_sys_getmsg_args), 0,
	    (sy_call_t *)svr4_sys_getmsg },	/* 85 = getmsg */
	{ ns(struct svr4_sys_putmsg_args), 0,
	    (sy_call_t *)svr4_sys_putmsg },	/* 86 = putmsg */
	{ ns(struct sys_poll_args), 0,
	    (sy_call_t *)sys_poll },		/* 87 = poll */
	{ ns(struct svr4_sys_lstat_args), 0,
	    (sy_call_t *)svr4_sys_lstat },	/* 88 = lstat */
	{ ns(struct sys_symlink_args), 0,
	    (sy_call_t *)sys_symlink },		/* 89 = symlink */
	{ ns(struct sys_readlink_args), 0,
	    (sy_call_t *)sys_readlink },	/* 90 = readlink */
	{ ns(struct sys_getgroups_args), 0,
	    (sy_call_t *)sys_getgroups },	/* 91 = getgroups */
	{ ns(struct sys_setgroups_args), 0,
	    (sy_call_t *)sys_setgroups },	/* 92 = setgroups */
	{ ns(struct sys_fchmod_args), 0,
	    (sy_call_t *)sys_fchmod },		/* 93 = fchmod */
	{ ns(struct sys___posix_fchown_args), 0,
	    (sy_call_t *)sys___posix_fchown },	/* 94 = fchown */
	{ ns(struct svr4_sys_sigprocmask_args), 0,
	    (sy_call_t *)svr4_sys_sigprocmask },/* 95 = sigprocmask */
	{ ns(struct svr4_sys_sigsuspend_args), 0,
	    (sy_call_t *)svr4_sys_sigsuspend },	/* 96 = sigsuspend */
	{ ns(struct svr4_sys_sigaltstack_args), 0,
	    (sy_call_t *)svr4_sys_sigaltstack },/* 97 = sigaltstack */
	{ ns(struct svr4_sys_sigaction_args), 0,
	    (sy_call_t *)svr4_sys_sigaction },	/* 98 = sigaction */
	{ ns(struct svr4_sys_sigpending_args), 0,
	    (sy_call_t *)svr4_sys_sigpending },	/* 99 = sigpending */
	{ ns(struct svr4_sys_context_args), 0,
	    (sy_call_t *)svr4_sys_context },	/* 100 = context */
	{ 0, 0, 0,
	    sys_nosys },			/* 101 = unimplemented evsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 102 = unimplemented evtrapret */
	{ ns(struct svr4_sys_statvfs_args), 0,
	    (sy_call_t *)svr4_sys_statvfs },	/* 103 = statvfs */
	{ ns(struct svr4_sys_fstatvfs_args), 0,
	    (sy_call_t *)svr4_sys_fstatvfs },	/* 104 = fstatvfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 105 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 106 = unimplemented nfssvc */
	{ ns(struct svr4_sys_waitsys_args), 0,
	    (sy_call_t *)svr4_sys_waitsys },	/* 107 = waitsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 108 = unimplemented sigsendsys */
	{ ns(struct svr4_sys_hrtsys_args), 0,
	    (sy_call_t *)svr4_sys_hrtsys },	/* 109 = hrtsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 110 = unimplemented acancel */
	{ 0, 0, 0,
	    sys_nosys },			/* 111 = unimplemented async */
	{ 0, 0, 0,
	    sys_nosys },			/* 112 = unimplemented priocntlsys */
	{ ns(struct svr4_sys_pathconf_args), 0,
	    (sy_call_t *)svr4_sys_pathconf },	/* 113 = pathconf */
	{ 0, 0, 0,
	    sys_nosys },			/* 114 = unimplemented mincore */
	{ ns(struct svr4_sys_mmap_args), 0,
	    (sy_call_t *)svr4_sys_mmap },	/* 115 = mmap */
	{ ns(struct sys_mprotect_args), 0,
	    (sy_call_t *)sys_mprotect },	/* 116 = mprotect */
	{ ns(struct sys_munmap_args), 0,
	    (sy_call_t *)sys_munmap },		/* 117 = munmap */
	{ ns(struct svr4_sys_fpathconf_args), 0,
	    (sy_call_t *)svr4_sys_fpathconf },	/* 118 = fpathconf */
	{ 0, 0, 0,
	    (sy_call_t *)sys_vfork },		/* 119 = vfork */
	{ ns(struct sys_fchdir_args), 0,
	    (sy_call_t *)sys_fchdir },		/* 120 = fchdir */
	{ ns(struct sys_readv_args), 0,
	    (sy_call_t *)sys_readv },		/* 121 = readv */
	{ ns(struct sys_writev_args), 0,
	    (sy_call_t *)sys_writev },		/* 122 = writev */
	{ ns(struct svr4_sys_xstat_args), 0,
	    (sy_call_t *)svr4_sys_xstat },	/* 123 = xstat */
	{ ns(struct svr4_sys_lxstat_args), 0,
	    (sy_call_t *)svr4_sys_lxstat },	/* 124 = lxstat */
	{ ns(struct svr4_sys_fxstat_args), 0,
	    (sy_call_t *)svr4_sys_fxstat },	/* 125 = fxstat */
	{ ns(struct svr4_sys_xmknod_args), 0,
	    (sy_call_t *)svr4_sys_xmknod },	/* 126 = xmknod */
	{ 0, 0, 0,
	    sys_nosys },			/* 127 = unimplemented clocal */
	{ ns(struct svr4_sys_setrlimit_args), 0,
	    (sy_call_t *)svr4_sys_setrlimit },	/* 128 = setrlimit */
	{ ns(struct svr4_sys_getrlimit_args), 0,
	    (sy_call_t *)svr4_sys_getrlimit },	/* 129 = getrlimit */
	{ ns(struct sys___posix_lchown_args), 0,
	    (sy_call_t *)sys___posix_lchown },	/* 130 = lchown */
	{ ns(struct svr4_sys_memcntl_args), 0,
	    (sy_call_t *)svr4_sys_memcntl },	/* 131 = memcntl */
	{ 0, 0, 0,
	    sys_nosys },			/* 132 = unimplemented getpmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 133 = unimplemented putpmsg */
	{ ns(struct sys___posix_rename_args), 0,
	    (sy_call_t *)sys___posix_rename },	/* 134 = rename */
	{ ns(struct svr4_sys_uname_args), 0,
	    (sy_call_t *)svr4_sys_uname },	/* 135 = uname */
	{ ns(struct sys_setegid_args), 0,
	    (sy_call_t *)sys_setegid },		/* 136 = setegid */
	{ ns(struct svr4_sys_sysconfig_args), 0,
	    (sy_call_t *)svr4_sys_sysconfig },	/* 137 = sysconfig */
	{ ns(struct compat_50_sys_adjtime_args), 0,
	    (sy_call_t *)compat_50_sys_adjtime },/* 138 = adjtime */
	{ ns(struct svr4_sys_systeminfo_args), 0,
	    (sy_call_t *)svr4_sys_systeminfo },	/* 139 = systeminfo */
	{ 0, 0, 0,
	    sys_nosys },			/* 140 = unimplemented */
	{ ns(struct sys_seteuid_args), 0,
	    (sy_call_t *)sys_seteuid },		/* 141 = seteuid */
	{ 0, 0, 0,
	    sys_nosys },			/* 142 = unimplemented vtrace */
	{ 0, 0, 0,
	    (sy_call_t *)sys_fork },		/* 143 = fork1 */
	{ 0, 0, 0,
	    sys_nosys },			/* 144 = unimplemented sigtimedwait */
	{ ns(struct svr4_sys__lwp_info_args), 0,
	    (sy_call_t *)svr4_sys__lwp_info },	/* 145 = _lwp_info */
	{ 0, 0, 0,
	    sys_nosys },			/* 146 = unimplemented yield */
	{ 0, 0, 0,
	    sys_nosys },			/* 147 = unimplemented lwp_sema_wait */
	{ 0, 0, 0,
	    sys_nosys },			/* 148 = unimplemented lwp_sema_post */
	{ 0, 0, 0,
	    sys_nosys },			/* 149 = unimplemented lwp_sema_trywait */
	{ 0, 0, 0,
	    sys_nosys },			/* 150 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 151 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 152 = unimplemented modctl */
	{ ns(struct sys_fchroot_args), 0,
	    (sy_call_t *)sys_fchroot },		/* 153 = fchroot */
	{ ns(struct svr4_sys_utimes_args), 0,
	    (sy_call_t *)svr4_sys_utimes },	/* 154 = utimes */
	{ 0, 0, 0,
	    (sy_call_t *)svr4_sys_vhangup },	/* 155 = vhangup */
	{ ns(struct svr4_sys_gettimeofday_args), 0,
	    (sy_call_t *)svr4_sys_gettimeofday },/* 156 = gettimeofday */
	{ ns(struct compat_50_sys_getitimer_args), 0,
	    (sy_call_t *)compat_50_sys_getitimer },/* 157 = getitimer */
	{ ns(struct compat_50_sys_setitimer_args), 0,
	    (sy_call_t *)compat_50_sys_setitimer },/* 158 = setitimer */
	{ ns(struct svr4_sys__lwp_create_args), 0,
	    (sy_call_t *)svr4_sys__lwp_create },/* 159 = _lwp_create */
	{ 0, 0, 0,
	    (sy_call_t *)svr4_sys__lwp_exit },	/* 160 = _lwp_exit */
	{ ns(struct svr4_sys__lwp_suspend_args), 0,
	    (sy_call_t *)svr4_sys__lwp_suspend },/* 161 = _lwp_suspend */
	{ ns(struct svr4_sys__lwp_continue_args), 0,
	    (sy_call_t *)svr4_sys__lwp_continue },/* 162 = _lwp_continue */
	{ ns(struct svr4_sys__lwp_kill_args), 0,
	    (sy_call_t *)svr4_sys__lwp_kill },	/* 163 = _lwp_kill */
	{ 0, 0, 0,
	    (sy_call_t *)svr4_sys__lwp_self },	/* 164 = _lwp_self */
	{ 0, 0, 0,
	    (sy_call_t *)svr4_sys__lwp_getprivate },/* 165 = _lwp_getprivate */
	{ ns(struct svr4_sys__lwp_setprivate_args), 0,
	    (sy_call_t *)svr4_sys__lwp_setprivate },/* 166 = _lwp_setprivate */
	{ ns(struct svr4_sys__lwp_wait_args), 0,
	    (sy_call_t *)svr4_sys__lwp_wait },	/* 167 = _lwp_wait */
	{ 0, 0, 0,
	    sys_nosys },			/* 168 = unimplemented lwp_mutex_unlock */
	{ 0, 0, 0,
	    sys_nosys },			/* 169 = unimplemented lwp_mutex_lock */
	{ 0, 0, 0,
	    sys_nosys },			/* 170 = unimplemented lwp_cond_wait */
	{ 0, 0, 0,
	    sys_nosys },			/* 171 = unimplemented lwp_cond_signal */
	{ 0, 0, 0,
	    sys_nosys },			/* 172 = unimplemented lwp_cond_broadcast */
	{ ns(struct svr4_sys_pread_args), 0,
	    (sy_call_t *)svr4_sys_pread },	/* 173 = pread */
	{ ns(struct svr4_sys_pwrite_args), 0,
	    (sy_call_t *)svr4_sys_pwrite },	/* 174 = pwrite */
	{ ns(struct svr4_sys_llseek_args), 0,
	    (sy_call_t *)svr4_sys_llseek },	/* 175 = llseek */
	{ 0, 0, 0,
	    sys_nosys },			/* 176 = unimplemented inst_sync */
	{ 0, 0, 0,
	    sys_nosys },			/* 177 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 178 = unimplemented kaio */
	{ 0, 0, 0,
	    sys_nosys },			/* 179 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 180 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 181 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 182 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 183 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 184 = unimplemented tsolsys */
	{ ns(struct svr4_sys_acl_args), 0,
	    (sy_call_t *)svr4_sys_acl },	/* 185 = acl */
	{ ns(struct svr4_sys_auditsys_args), 0,
	    (sy_call_t *)svr4_sys_auditsys },	/* 186 = auditsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 187 = unimplemented processor_bind */
	{ 0, 0, 0,
	    sys_nosys },			/* 188 = unimplemented processor_info */
	{ 0, 0, 0,
	    sys_nosys },			/* 189 = unimplemented p_online */
	{ 0, 0, 0,
	    sys_nosys },			/* 190 = unimplemented sigqueue */
	{ 0, 0, 0,
	    sys_nosys },			/* 191 = unimplemented clock_gettime */
	{ 0, 0, 0,
	    sys_nosys },			/* 192 = unimplemented clock_settime */
	{ 0, 0, 0,
	    sys_nosys },			/* 193 = unimplemented clock_getres */
	{ 0, 0, 0,
	    sys_nosys },			/* 194 = unimplemented timer_create */
	{ 0, 0, 0,
	    sys_nosys },			/* 195 = unimplemented timer_delete */
	{ 0, 0, 0,
	    sys_nosys },			/* 196 = unimplemented timer_settime */
	{ 0, 0, 0,
	    sys_nosys },			/* 197 = unimplemented timer_gettime */
	{ 0, 0, 0,
	    sys_nosys },			/* 198 = unimplemented timer_getoverrun */
	{ ns(struct compat_50_sys_nanosleep_args), 0,
	    (sy_call_t *)compat_50_sys_nanosleep },/* 199 = nanosleep */
	{ ns(struct svr4_sys_facl_args), 0,
	    (sy_call_t *)svr4_sys_facl },	/* 200 = facl */
	{ 0, 0, 0,
	    sys_nosys },			/* 201 = unimplemented door */
	{ ns(struct sys_setreuid_args), 0,
	    (sy_call_t *)sys_setreuid },	/* 202 = setreuid */
	{ ns(struct sys_setregid_args), 0,
	    (sy_call_t *)sys_setregid },	/* 203 = setregid */
	{ 0, 0, 0,
	    sys_nosys },			/* 204 = unimplemented install_utrap */
	{ 0, 0, 0,
	    sys_nosys },			/* 205 = unimplemented signotify */
	{ ns(struct svr4_sys_schedctl_args), 0,
	    (sy_call_t *)svr4_sys_schedctl },	/* 206 = schedctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 207 = unimplemented pset */
	{ 0, 0, 0,
	    sys_nosys },			/* 208 = unimplemented */
	{ ns(struct svr4_sys_resolvepath_args), 0,
	    (sy_call_t *)svr4_sys_resolvepath },/* 209 = resolvepath */
	{ 0, 0, 0,
	    sys_nosys },			/* 210 = unimplemented signotifywait */
	{ 0, 0, 0,
	    sys_nosys },			/* 211 = unimplemented lwp_sigredirect */
	{ 0, 0, 0,
	    sys_nosys },			/* 212 = unimplemented lwp_alarm */
	{ ns(struct svr4_sys_getdents64_args), 0,
	    (sy_call_t *)svr4_sys_getdents64 },	/* 213 = getdents64 */
	{ ns(struct svr4_sys_mmap64_args), 0,
	    (sy_call_t *)svr4_sys_mmap64 },	/* 214 = mmap64 */
	{ ns(struct svr4_sys_stat64_args), 0,
	    (sy_call_t *)svr4_sys_stat64 },	/* 215 = stat64 */
	{ ns(struct svr4_sys_lstat64_args), 0,
	    (sy_call_t *)svr4_sys_lstat64 },	/* 216 = lstat64 */
	{ ns(struct svr4_sys_fstat64_args), 0,
	    (sy_call_t *)svr4_sys_fstat64 },	/* 217 = fstat64 */
	{ ns(struct svr4_sys_statvfs64_args), 0,
	    (sy_call_t *)svr4_sys_statvfs64 },	/* 218 = statvfs64 */
	{ ns(struct svr4_sys_fstatvfs64_args), 0,
	    (sy_call_t *)svr4_sys_fstatvfs64 },	/* 219 = fstatvfs64 */
	{ ns(struct svr4_sys_setrlimit64_args), 0,
	    (sy_call_t *)svr4_sys_setrlimit64 },/* 220 = setrlimit64 */
	{ ns(struct svr4_sys_getrlimit64_args), 0,
	    (sy_call_t *)svr4_sys_getrlimit64 },/* 221 = getrlimit64 */
	{ ns(struct svr4_sys_pread64_args), 0,
	    (sy_call_t *)svr4_sys_pread64 },	/* 222 = pread64 */
	{ ns(struct svr4_sys_pwrite64_args), 0,
	    (sy_call_t *)svr4_sys_pwrite64 },	/* 223 = pwrite64 */
	{ ns(struct svr4_sys_creat64_args), 0,
	    (sy_call_t *)svr4_sys_creat64 },	/* 224 = creat64 */
	{ ns(struct svr4_sys_open64_args), 0,
	    (sy_call_t *)svr4_sys_open64 },	/* 225 = open64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 226 = unimplemented rpcsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 227 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 228 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 229 = unimplemented */
	{ ns(struct svr4_sys_socket_args), 0,
	    (sy_call_t *)svr4_sys_socket },	/* 230 = socket */
	{ ns(struct sys_socketpair_args), 0,
	    (sy_call_t *)sys_socketpair },	/* 231 = socketpair */
	{ ns(struct sys_bind_args), 0,
	    (sy_call_t *)sys_bind },		/* 232 = bind */
	{ ns(struct sys_listen_args), 0,
	    (sy_call_t *)sys_listen },		/* 233 = listen */
	{ ns(struct compat_43_sys_accept_args), 0,
	    (sy_call_t *)compat_43_sys_accept },/* 234 = accept */
	{ ns(struct sys_connect_args), 0,
	    (sy_call_t *)sys_connect },		/* 235 = connect */
	{ ns(struct sys_shutdown_args), 0,
	    (sy_call_t *)sys_shutdown },	/* 236 = shutdown */
	{ ns(struct compat_43_sys_recv_args), 0,
	    (sy_call_t *)compat_43_sys_recv },	/* 237 = recv */
	{ ns(struct compat_43_sys_recvfrom_args), 0,
	    (sy_call_t *)compat_43_sys_recvfrom },/* 238 = recvfrom */
	{ ns(struct compat_43_sys_recvmsg_args), 0,
	    (sy_call_t *)compat_43_sys_recvmsg },/* 239 = recvmsg */
	{ ns(struct compat_43_sys_send_args), 0,
	    (sy_call_t *)compat_43_sys_send },	/* 240 = send */
	{ ns(struct compat_43_sys_sendmsg_args), 0,
	    (sy_call_t *)compat_43_sys_sendmsg },/* 241 = sendmsg */
	{ ns(struct sys_sendto_args), 0,
	    (sy_call_t *)sys_sendto },		/* 242 = sendto */
	{ ns(struct compat_43_sys_getpeername_args), 0,
	    (sy_call_t *)compat_43_sys_getpeername },/* 243 = getpeername */
	{ ns(struct compat_43_sys_getsockname_args), 0,
	    (sy_call_t *)compat_43_sys_getsockname },/* 244 = getsockname */
	{ ns(struct sys_getsockopt_args), 0,
	    (sy_call_t *)sys_getsockopt },	/* 245 = getsockopt */
	{ ns(struct sys_setsockopt_args), 0,
	    (sy_call_t *)sys_setsockopt },	/* 246 = setsockopt */
	{ 0, 0, 0,
	    sys_nosys },			/* 247 = unimplemented sockconfig */
	{ 0, 0, 0,
	    sys_nosys },			/* 248 = unimplemented sys_ntp_gettime */
#if defined(NTP) || !defined(_KERNEL)
	{ ns(struct sys_ntp_adjtime_args), 0,
	    (sy_call_t *)sys_ntp_adjtime },	/* 249 = ntp_adjtime */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 249 = excluded ntp_adjtime */
#endif
	{ 0, 0, 0,
	    sys_nosys },			/* 250 = unimplemented lwp_mutex_unlock */
	{ 0, 0, 0,
	    sys_nosys },			/* 251 = unimplemented lwp_mutex_trylock */
	{ 0, 0, 0,
	    sys_nosys },			/* 252 = unimplemented lwp_mutex_init */
	{ 0, 0, 0,
	    sys_nosys },			/* 253 = unimplemented cladm */
	{ 0, 0, 0,
	    sys_nosys },			/* 254 = unimplemented lwp_sigtimedwait */
	{ 0, 0, 0,
	    sys_nosys },			/* 255 = unimplemented umount2 */
};
