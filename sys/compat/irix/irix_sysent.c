/* $NetBSD: irix_sysent.c,v 1.23 2002/02/17 22:49:55 manu Exp $ */

/*
 * System call switch table.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.21 2002/02/17 20:50:07 manu Exp 
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: irix_sysent.c,v 1.23 2002/02/17 22:49:55 manu Exp $");

#if defined(_KERNEL_OPT)
#include "opt_ntp.h"
#include "opt_sysv.h"
#include "opt_compat_43.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/poll.h>
#include <sys/ioctl_compat.h>
#include <sys/syscallargs.h>
#include <compat/svr4/svr4_types.h>
#include <compat/svr4/svr4_signal.h>
#include <compat/svr4/svr4_ucontext.h>
#include <compat/svr4/svr4_lwp.h>
#include <compat/svr4/svr4_syscallargs.h>
#include <compat/irix/irix_types.h>
#include <compat/irix/irix_signal.h>
#include <compat/irix/irix_syscallargs.h>

#define	s(type)	sizeof(type)

struct sysent irix_sysent[] = {
	{ 0, 0, 0,
	    sys_nosys },			/* 0 = syscall */
	{ 1, s(struct sys_exit_args), 0,
	    sys_exit },				/* 1 = exit */
	{ 0, 0, 0,
	    sys_fork },				/* 2 = fork */
	{ 3, s(struct sys_read_args), 0,
	    sys_read },				/* 3 = read */
	{ 3, s(struct sys_write_args), 0,
	    sys_write },			/* 4 = write */
	{ 3, s(struct svr4_sys_open_args), 0,
	    svr4_sys_open },			/* 5 = open */
	{ 1, s(struct sys_close_args), 0,
	    sys_close },			/* 6 = close */
	{ 0, 0, 0,
	    sys_nosys },			/* 7 = obsolete wait */
	{ 2, s(struct svr4_sys_creat_args), 0,
	    svr4_sys_creat },			/* 8 = creat */
	{ 2, s(struct sys_link_args), 0,
	    sys_link },				/* 9 = link */
	{ 1, s(struct sys_unlink_args), 0,
	    sys_unlink },			/* 10 = unlink */
	{ 2, s(struct svr4_sys_execv_args), 0,
	    svr4_sys_execv },			/* 11 = execv */
	{ 1, s(struct sys_chdir_args), 0,
	    sys_chdir },			/* 12 = chdir */
	{ 1, s(struct svr4_sys_time_args), 0,
	    svr4_sys_time },			/* 13 = time */
	{ 0, 0, 0,
	    sys_nosys },			/* 14 = obsolete mknod */
	{ 2, s(struct sys_chmod_args), 0,
	    sys_chmod },			/* 15 = chmod */
	{ 3, s(struct sys___posix_chown_args), 0,
	    sys___posix_chown },		/* 16 = chown */
	{ 1, s(struct svr4_sys_break_args), 0,
	    svr4_sys_break },			/* 17 = break */
	{ 0, 0, 0,
	    sys_nosys },			/* 18 = obsolete stat */
	{ 3, s(struct compat_43_sys_lseek_args), 0,
	    compat_43_sys_lseek },		/* 19 = lseek */
	{ 0, 0, SYCALL_MPSAFE | 0,
	    sys_getpid },			/* 20 = getpid */
	{ 0, 0, 0,
	    sys_nosys },			/* 21 = unimplemented old_mount */
	{ 0, 0, 0,
	    sys_nosys },			/* 22 = unimplemented System V umount */
	{ 1, s(struct sys_setuid_args), 0,
	    sys_setuid },			/* 23 = setuid */
	{ 0, 0, 0,
	    sys_getuid_with_euid },		/* 24 = getuid_with_euid */
	{ 0, 0, 0,
	    sys_nosys },			/* 25 = unimplemented stime */
	{ 0, 0, 0,
	    sys_nosys },			/* 26 = unimplemented ptrace */
	{ 1, s(struct svr4_sys_alarm_args), 0,
	    svr4_sys_alarm },			/* 27 = alarm */
	{ 2, s(struct svr4_sys_fstat_args), 0,
	    svr4_sys_fstat },			/* 28 = fstat */
	{ 0, 0, 0,
	    svr4_sys_pause },			/* 29 = pause */
	{ 2, s(struct svr4_sys_utime_args), 0,
	    svr4_sys_utime },			/* 30 = utime */
	{ 0, 0, 0,
	    sys_nosys },			/* 31 = unimplemented was stty */
	{ 0, 0, 0,
	    sys_nosys },			/* 32 = unimplemented was gtty */
	{ 2, s(struct svr4_sys_access_args), 0,
	    svr4_sys_access },			/* 33 = access */
	{ 1, s(struct svr4_sys_nice_args), 0,
	    svr4_sys_nice },			/* 34 = nice */
	{ 0, 0, 0,
	    sys_nosys },			/* 35 = unimplemented statfs */
	{ 0, 0, 0,
	    sys_sync },				/* 36 = sync */
	{ 2, s(struct svr4_sys_kill_args), 0,
	    svr4_sys_kill },			/* 37 = kill */
	{ 0, 0, 0,
	    sys_nosys },			/* 38 = unimplemented fstatfs */
	{ 3, s(struct svr4_sys_pgrpsys_args), 0,
	    svr4_sys_pgrpsys },			/* 39 = pgrpsys */
	{ 6, s(struct irix_sys_syssgi_args), 0,
	    irix_sys_syssgi },			/* 40 = syssgi */
	{ 1, s(struct sys_dup_args), 0,
	    sys_dup },				/* 41 = dup */
	{ 0, 0, 0,
	    sys_pipe },				/* 42 = pipe */
	{ 1, s(struct svr4_sys_times_args), 0,
	    svr4_sys_times },			/* 43 = times */
	{ 0, 0, 0,
	    sys_nosys },			/* 44 = unimplemented profil */
	{ 0, 0, 0,
	    sys_nosys },			/* 45 = unimplemented plock */
	{ 1, s(struct sys_setgid_args), 0,
	    sys_setgid },			/* 46 = setgid */
	{ 0, 0, 0,
	    sys_getgid_with_egid },		/* 47 = getgid_with_egid */
	{ 0, 0, 0,
	    sys_nosys },			/* 48 = obsolete ssig */
#ifdef SYSVMSG
	{ 5, s(struct svr4_sys_msgsys_args), 0,
	    svr4_sys_msgsys },			/* 49 = msgsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 49 = unimplemented msgsys */
#endif
	{ 0, 0, 0,
	    sys_nosys },			/* 50 = unimplemented sysmips */
	{ 0, 0, 0,
	    sys_nosys },			/* 51 = unimplemented acct */
#ifdef SYSVSHM
	{ 4, s(struct svr4_sys_shmsys_args), 0,
	    svr4_sys_shmsys },			/* 52 = shmsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 52 = unimplemented shmsys */
#endif
#ifdef SYSVSEM
	{ 5, s(struct svr4_sys_semsys_args), 0,
	    svr4_sys_semsys },			/* 53 = semsys */
#else
	{ 0, 0, 0,
	    sys_nosys },			/* 53 = unimplemented semsys */
#endif
	{ 3, s(struct svr4_sys_ioctl_args), 0,
	    svr4_sys_ioctl },			/* 54 = ioctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 55 = unimplemented uadmin */
	{ 5, s(struct irix_sys_sysmp_args), 0,
	    irix_sys_sysmp },			/* 56 = sysmp */
	{ 4, s(struct svr4_sys_utssys_args), 0,
	    svr4_sys_utssys },			/* 57 = utssys */
	{ 0, 0, 0,
	    sys_nosys },			/* 58 = unimplemented */
	{ 3, s(struct svr4_sys_execve_args), 0,
	    svr4_sys_execve },			/* 59 = execve */
	{ 1, s(struct sys_umask_args), 0,
	    sys_umask },			/* 60 = umask */
	{ 1, s(struct sys_chroot_args), 0,
	    sys_chroot },			/* 61 = chroot */
	{ 3, s(struct svr4_sys_fcntl_args), 0,
	    svr4_sys_fcntl },			/* 62 = fcntl */
	{ 2, s(struct svr4_sys_ulimit_args), 0,
	    svr4_sys_ulimit },			/* 63 = ulimit */
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
	{ 7, s(struct irix_sys_lseek64_args), 0,
	    irix_sys_lseek64 },			/* 78 = lseek64 */
	{ 1, s(struct sys_rmdir_args), 0,
	    sys_rmdir },			/* 79 = rmdir */
	{ 2, s(struct sys_mkdir_args), 0,
	    sys_mkdir },			/* 80 = mkdir */
	{ 3, s(struct irix_sys_getdents_args), 0,
	    irix_sys_getdents },		/* 81 = getdents */
	{ 1, s(struct irix_sys_sginap_args), 0,
	    irix_sys_sginap },			/* 82 = sginap */
	{ 0, 0, 0,
	    sys_nosys },			/* 83 = unimplemented sgikopt */
	{ 0, 0, 0,
	    sys_nosys },			/* 84 = unimplemented sysfs */
	{ 4, s(struct svr4_sys_getmsg_args), 0,
	    svr4_sys_getmsg },			/* 85 = getmsg */
	{ 4, s(struct svr4_sys_putmsg_args), 0,
	    svr4_sys_putmsg },			/* 86 = putmsg */
	{ 3, s(struct sys_poll_args), 0,
	    sys_poll },				/* 87 = poll */
	{ 1, s(struct irix_sys_sigreturn_args), 0,
	    irix_sys_sigreturn },		/* 88 = sigreturn */
	{ 3, s(struct compat_43_sys_accept_args), 0,
	    compat_43_sys_accept },		/* 89 = accept */
	{ 3, s(struct sys_bind_args), 0,
	    sys_bind },				/* 90 = bind */
	{ 3, s(struct sys_connect_args), 0,
	    sys_connect },			/* 91 = connect */
	{ 0, 0, 0,
	    sys_nosys },			/* 92 = unimplemented sys_gethostid */
	{ 3, s(struct compat_43_sys_getpeername_args), 0,
	    compat_43_sys_getpeername },	/* 93 = getpeername */
	{ 3, s(struct compat_43_sys_getsockname_args), 0,
	    compat_43_sys_getsockname },	/* 94 = getsockname */
	{ 5, s(struct sys_getsockopt_args), 0,
	    sys_getsockopt },			/* 95 = getsockopt */
	{ 2, s(struct sys_listen_args), 0,
	    sys_listen },			/* 96 = listen */
	{ 4, s(struct compat_43_sys_recv_args), 0,
	    compat_43_sys_recv },		/* 97 = recv */
	{ 6, s(struct compat_43_sys_recvfrom_args), 0,
	    compat_43_sys_recvfrom },		/* 98 = recvfrom */
	{ 3, s(struct compat_43_sys_recvmsg_args), 0,
	    compat_43_sys_recvmsg },		/* 99 = recvmsg */
	{ 5, s(struct sys_select_args), 0,
	    sys_select },			/* 100 = select */
	{ 4, s(struct compat_43_sys_send_args), 0,
	    compat_43_sys_send },		/* 101 = send */
	{ 3, s(struct compat_43_sys_sendmsg_args), 0,
	    compat_43_sys_sendmsg },		/* 102 = sendmsg */
	{ 6, s(struct sys_sendto_args), 0,
	    sys_sendto },			/* 103 = sendto */
	{ 0, 0, 0,
	    sys_nosys },			/* 104 = unimplemented sys_sethostid */
	{ 5, s(struct sys_setsockopt_args), 0,
	    sys_setsockopt },			/* 105 = setsockopt */
	{ 2, s(struct sys_shutdown_args), 0,
	    sys_shutdown },			/* 106 = shutdown */
	{ 3, s(struct svr4_sys_socket_args), 0,
	    svr4_sys_socket },			/* 107 = socket */
	{ 2, s(struct compat_43_sys_gethostname_args), 0,
	    compat_43_sys_gethostname },	/* 108 = gethostname */
	{ 2, s(struct compat_43_sys_sethostname_args), 0,
	    compat_43_sys_sethostname },	/* 109 = sethostname */
	{ 2, s(struct compat_09_sys_getdomainname_args), 0,
	    compat_09_sys_getdomainname },	/* 110 = getdomainname */
	{ 2, s(struct compat_09_sys_setdomainname_args), 0,
	    compat_09_sys_setdomainname },	/* 111 = setdomainname */
	{ 0, 0, 0,
	    sys_nosys },			/* 112 = unimplemented truncate */
	{ 0, 0, 0,
	    sys_nosys },			/* 113 = unimplemented ftruncate */
	{ 2, s(struct sys_rename_args), 0,
	    sys_rename },			/* 114 = rename */
	{ 2, s(struct sys_symlink_args), 0,
	    sys_symlink },			/* 115 = symlink */
	{ 3, s(struct sys_readlink_args), 0,
	    sys_readlink },			/* 116 = readlink */
	{ 0, 0, 0,
	    sys_nosys },			/* 117 = unimplemented lstat */
	{ 0, 0, 0,
	    sys_nosys },			/* 118 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 119 = unimplemented nfs_svc */
	{ 0, 0, 0,
	    sys_nosys },			/* 120 = unimplemented nfs_getfh */
	{ 0, 0, 0,
	    sys_nosys },			/* 121 = unimplemented async_daemon */
	{ 0, 0, 0,
	    sys_nosys },			/* 122 = unimplemented exportfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 123 = unimplemented setregid */
	{ 0, 0, 0,
	    sys_nosys },			/* 124 = unimplemented setreuid */
	{ 2, s(struct sys_getitimer_args), 0,
	    sys_getitimer },			/* 125 = getitimer */
	{ 3, s(struct sys_setitimer_args), 0,
	    sys_setitimer },			/* 126 = setitimer */
	{ 2, s(struct sys_adjtime_args), 0,
	    sys_adjtime },			/* 127 = adjtime */
	{ 1, s(struct svr4_sys_gettimeofday_args), 0,
	    svr4_sys_gettimeofday },		/* 128 = gettimeofday */
	{ 0, 0, 0,
	    sys_nosys },			/* 129 = unimplemented sproc */
	{ 2, s(struct irix_sys_prctl_args), 0,
	    irix_sys_prctl },			/* 130 = prctl */
	{ 0, 0, 0,
	    sys_nosys },			/* 131 = unimplemented procblk */
	{ 0, 0, 0,
	    sys_nosys },			/* 132 = unimplemented sprocsp */
	{ 0, 0, 0,
	    sys_nosys },			/* 133 = unimplemented sgigsc */
	{ 6, s(struct svr4_sys_mmap_args), 0,
	    svr4_sys_mmap },			/* 134 = mmap */
	{ 0, 0, 0,
	    sys_nosys },			/* 135 = unimplemented munmap */
	{ 0, 0, 0,
	    sys_nosys },			/* 136 = unimplemented mprotect */
	{ 0, 0, 0,
	    sys_nosys },			/* 137 = unimplemented msync */
	{ 0, 0, 0,
	    sys_nosys },			/* 138 = unimplemented madvise */
	{ 0, 0, 0,
	    sys_nosys },			/* 139 = unimplemented pagelock */
	{ 0, 0, 0,
	    sys_nosys },			/* 140 = unimplemented getpagesize */
	{ 0, 0, 0,
	    sys_nosys },			/* 141 = unimplemented quotactl */
	{ 0, 0, 0,
	    sys_nosys },			/* 142 = unimplemented */
	{ 0, 0, 0,
	    sys_nosys },			/* 143 = unimplemented getpgrp */
	{ 0, 0, 0,
	    sys_nosys },			/* 144 = unimplemented setpgrp */
	{ 0, 0, 0,
	    sys_nosys },			/* 145 = unimplemented vhangup */
	{ 0, 0, 0,
	    sys_nosys },			/* 146 = unimplemented fsync */
	{ 0, 0, 0,
	    sys_nosys },			/* 147 = unimplemented fchdir */
	{ 0, 0, 0,
	    sys_nosys },			/* 148 = unimplemented getrlimit */
	{ 0, 0, 0,
	    sys_nosys },			/* 149 = unimplemented setrlimit */
	{ 0, 0, 0,
	    sys_nosys },			/* 150 = unimplemented cacheflush */
	{ 0, 0, 0,
	    sys_nosys },			/* 151 = unimplemented cachectl */
	{ 0, 0, 0,
	    sys_nosys },			/* 152 = unimplemented fchown */
	{ 0, 0, 0,
	    sys_nosys },			/* 153 = unimplemented fchmod */
	{ 0, 0, 0,
	    sys_nosys },			/* 154 = unimplemented wait3 */
	{ 0, 0, 0,
	    sys_nosys },			/* 155 = unimplemented socketpair */
	{ 3, s(struct irix_sys_systeminfo_args), 0,
	    irix_sys_systeminfo },		/* 156 = systeminfo */
	{ 0, 0, 0,
	    sys_nosys },			/* 157 = unimplemented uname */
	{ 3, s(struct irix_sys_xstat_args), 0,
	    irix_sys_xstat },			/* 158 = xstat */
	{ 3, s(struct irix_sys_lxstat_args), 0,
	    irix_sys_lxstat },			/* 159 = lxstat */
	{ 3, s(struct irix_sys_fxstat_args), 0,
	    irix_sys_fxstat },			/* 160 = fxstat */
	{ 0, 0, 0,
	    sys_nosys },			/* 161 = unimplemented xmknod */
	{ 3, s(struct svr4_sys_sigaction_args), 0,
	    svr4_sys_sigaction },		/* 162 = sigaction */
	{ 2, s(struct svr4_sys_sigpending_args), 0,
	    svr4_sys_sigpending },		/* 163 = sigpending */
	{ 3, s(struct svr4_sys_sigprocmask_args), 0,
	    svr4_sys_sigprocmask },		/* 164 = sigprocmask */
	{ 1, s(struct svr4_sys_sigsuspend_args), 0,
	    svr4_sys_sigsuspend },		/* 165 = sigsuspend */
	{ 0, 0, 0,
	    sys_nosys },			/* 166 = unimplemented sigpoll_sys */
	{ 0, 0, 0,
	    sys_nosys },			/* 167 = unimplemented swapctl */
	{ 1, s(struct irix_sys_getcontext_args), 0,
	    irix_sys_getcontext },		/* 168 = getcontext */
	{ 1, s(struct irix_sys_setcontext_args), 0,
	    irix_sys_setcontext },		/* 169 = setcontext */
	{ 5, s(struct irix_sys_waitsys_args), 0,
	    irix_sys_waitsys },			/* 170 = waitsys */
	{ 0, 0, 0,
	    sys_nosys },			/* 171 = unimplemented sigstack */
	{ 0, 0, 0,
	    sys_nosys },			/* 172 = unimplemented sigaltstack */
	{ 0, 0, 0,
	    sys_nosys },			/* 173 = unimplemented sigsendset */
	{ 0, 0, 0,
	    sys_nosys },			/* 174 = unimplemented statvfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 175 = unimplemented fstatvfs */
	{ 0, 0, 0,
	    sys_nosys },			/* 176 = unimplemented getpmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 177 = unimplemented putpmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 178 = unimplemented lchown */
	{ 0, 0, 0,
	    sys_nosys },			/* 179 = unimplemented priocntl */
	{ 0, 0, 0,
	    sys_nosys },			/* 180 = unimplemented sigqueue */
	{ 3, s(struct sys_readv_args), 0,
	    sys_readv },			/* 181 = readv */
	{ 3, s(struct sys_writev_args), 0,
	    sys_writev },			/* 182 = writev */
	{ 0, 0, 0,
	    sys_nosys },			/* 183 = unimplemented truncate64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 184 = unimplemented ftruncate64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 185 = unimplemented mmap64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 186 = unimplemented dmi */
	{ 0, 0, 0,
	    sys_nosys },			/* 187 = unimplemented pread */
	{ 0, 0, 0,
	    sys_nosys },			/* 188 = unimplemented pwrite */
	{ 0, 0, 0,
	    sys_nosys },			/* 189 = unimplemented fdatasync */
	{ 0, 0, 0,
	    sys_nosys },			/* 190 = unimplemented sgifastpath */
	{ 0, 0, 0,
	    sys_nosys },			/* 191 = unimplemented attr_get */
	{ 0, 0, 0,
	    sys_nosys },			/* 192 = unimplemented attr_getf */
	{ 0, 0, 0,
	    sys_nosys },			/* 193 = unimplemented attr_set */
	{ 0, 0, 0,
	    sys_nosys },			/* 194 = unimplemented attr_setf */
	{ 0, 0, 0,
	    sys_nosys },			/* 195 = unimplemented attr_remove */
	{ 0, 0, 0,
	    sys_nosys },			/* 196 = unimplemented attr_removef */
	{ 0, 0, 0,
	    sys_nosys },			/* 197 = unimplemented attr_list */
	{ 0, 0, 0,
	    sys_nosys },			/* 198 = unimplemented attr_listf */
	{ 0, 0, 0,
	    sys_nosys },			/* 199 = unimplemented attr_multi */
	{ 0, 0, 0,
	    sys_nosys },			/* 200 = unimplemented attr_multif */
	{ 0, 0, 0,
	    sys_nosys },			/* 201 = unimplemented statvfs64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 202 = unimplemented fstatvfs64 */
	{ 2, s(struct irix_sys_getmountid_args), 0,
	    irix_sys_getmountid },		/* 203 = getmountid */
	{ 0, 0, 0,
	    sys_nosys },			/* 204 = unimplemented nsproc */
	{ 3, s(struct irix_sys_getdents64_args), 0,
	    irix_sys_getdents64 },		/* 205 = getdents64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 206 = unimplemented afs_syscall */
	{ 4, s(struct irix_sys_ngetdents_args), 0,
	    irix_sys_ngetdents },		/* 207 = ngetdents */
	{ 4, s(struct irix_sys_ngetdents64_args), 0,
	    irix_sys_ngetdents64 },		/* 208 = ngetdents64 */
	{ 0, 0, 0,
	    sys_nosys },			/* 209 = unimplemented sgi_sesmgr */
	{ 0, 0, 0,
	    sys_nosys },			/* 210 = unimplemented pidsprocsp */
	{ 0, 0, 0,
	    sys_nosys },			/* 211 = unimplemented rexec */
	{ 0, 0, 0,
	    sys_nosys },			/* 212 = unimplemented timer_create */
	{ 0, 0, 0,
	    sys_nosys },			/* 213 = unimplemented timer_delete */
	{ 0, 0, 0,
	    sys_nosys },			/* 214 = unimplemented timer_settime */
	{ 0, 0, 0,
	    sys_nosys },			/* 215 = unimplemented timer_gettime */
	{ 0, 0, 0,
	    sys_nosys },			/* 216 = unimplemented timer_setoverrun */
	{ 0, 0, 0,
	    sys_nosys },			/* 217 = unimplemented sched_rr_get_interval */
	{ 0, 0, 0,
	    sys_nosys },			/* 218 = unimplemented sched_yield */
	{ 0, 0, 0,
	    sys_nosys },			/* 219 = unimplemented sched_getscheduler */
	{ 0, 0, 0,
	    sys_nosys },			/* 220 = unimplemented sched_setscheduler */
	{ 0, 0, 0,
	    sys_nosys },			/* 221 = unimplemented sched_getparam */
	{ 0, 0, 0,
	    sys_nosys },			/* 222 = unimplemented sched_setparam */
	{ 0, 0, 0,
	    sys_nosys },			/* 223 = unimplemented usync_cntl */
	{ 0, 0, 0,
	    sys_nosys },			/* 224 = unimplemented psema_cntl */
	{ 0, 0, 0,
	    sys_nosys },			/* 225 = unimplemented restartreturn */
	{ 0, 0, 0,
	    sys_nosys },			/* 226 = unimplemented sysget */
	{ 0, 0, 0,
	    sys_nosys },			/* 227 = unimplemented xpg4_recvmsg */
	{ 0, 0, 0,
	    sys_nosys },			/* 228 = unimplemented umfscall */
	{ 0, 0, 0,
	    sys_nosys },			/* 229 = unimplemented nsproctid */
	{ 0, 0, 0,
	    sys_nosys },			/* 230 = unimplemented rexec_complete */
	{ 0, 0, 0,
	    sys_nosys },			/* 231 = unimplemented xpg4_sigaltstack */
	{ 0, 0, 0,
	    sys_nosys },			/* 232 = unimplemented xpg4_sigaltstack */
	{ 0, 0, 0,
	    sys_nosys },			/* 233 = unimplemented xpg4_setregid */
	{ 0, 0, 0,
	    sys_nosys },			/* 234 = unimplemented linkfollow */
	{ 0, 0, 0,
	    sys_nosys },			/* 235 = unimplemented utimets */
};

