/* $NetBSD: irix_syscallargs.h,v 1.61 2009/12/14 00:58:36 matt Exp $ */

/*
 * System call argument lists.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.54 2009/01/13 22:27:43 pooka Exp
 */

#ifndef _IRIX_SYS_SYSCALLARGS_H_
#define	_IRIX_SYS_SYSCALLARGS_H_

#define	IRIX_SYS_MAXSYSARGS	8

#undef	syscallarg
#define	syscallarg(x)							\
	union {								\
		register_t pad;						\
		struct { x datum; } le;					\
		struct { /* LINTED zero array dimension */		\
			int8_t pad[  /* CONSTCOND */			\
				(sizeof (register_t) < sizeof (x))	\
				? 0					\
				: sizeof (register_t) - sizeof (x)];	\
			x datum;					\
		} be;							\
	}

#undef check_syscall_args
#define check_syscall_args(call) \
	typedef char call##_check_args[sizeof (struct call##_args) \
		<= IRIX_SYS_MAXSYSARGS * sizeof (register_t) ? 1 : -1];

struct sys_exit_args;

struct sys_read_args;

struct sys_write_args;

struct irix_sys_open_args {
	syscallarg(const char *) path;
	syscallarg(int) flags;
	syscallarg(int) mode;
};
check_syscall_args(irix_sys_open)

struct sys_close_args;

struct svr4_sys_creat_args;

struct sys_link_args;

struct sys_unlink_args;

struct svr4_sys_execv_args;

struct sys_chdir_args;

struct svr4_sys_time_args;

struct sys_chmod_args;

struct sys___posix_chown_args;

struct irix_sys_break_args {
	syscallarg(void *) nsize;
};
check_syscall_args(irix_sys_break)

struct compat_43_sys_lseek_args;

struct sys_setuid_args;

struct svr4_sys_alarm_args;

struct svr4_sys_fstat_args;

struct svr4_sys_utime_args;

struct svr4_sys_access_args;

struct svr4_sys_nice_args;

struct svr4_sys_kill_args;

struct svr4_sys_pgrpsys_args;

struct irix_sys_syssgi_args {
	syscallarg(int) request;
	syscallarg(void *) arg1;
	syscallarg(void *) arg2;
	syscallarg(void *) arg3;
	syscallarg(void *) arg4;
	syscallarg(void *) arg5;
};
check_syscall_args(irix_sys_syssgi)

struct sys_dup_args;

struct svr4_sys_times_args;

struct sys_setgid_args;
#ifdef SYSVMSG

struct svr4_sys_msgsys_args;
#else
#endif
#ifdef SYSVSHM

struct irix_sys_shmsys_args {
	syscallarg(int) what;
	syscallarg(int) a2;
	syscallarg(int) a3;
	syscallarg(int) a4;
};
check_syscall_args(irix_sys_shmsys)
#else
#endif
#ifdef SYSVSEM

struct svr4_sys_semsys_args;
#else
#endif

struct irix_sys_ioctl_args {
	syscallarg(int) fd;
	syscallarg(u_long) com;
	syscallarg(void *) data;
};
check_syscall_args(irix_sys_ioctl)

struct irix_sys_sysmp_args {
	syscallarg(int) cmd;
	syscallarg(void *) arg1;
	syscallarg(void *) arg2;
	syscallarg(void *) arg3;
	syscallarg(void *) arg4;
};
check_syscall_args(irix_sys_sysmp)

struct irix_sys_utssys_args {
	syscallarg(void *) a1;
	syscallarg(void *) a2;
	syscallarg(int) sel;
	syscallarg(void *) a3;
};
check_syscall_args(irix_sys_utssys)

struct svr4_sys_execve_args;

struct sys_umask_args;

struct sys_chroot_args;

struct irix_sys_fcntl_args {
	syscallarg(int) fd;
	syscallarg(int) cmd;
	syscallarg(char *) arg;
};
check_syscall_args(irix_sys_fcntl)

struct svr4_sys_ulimit_args;

struct irix_sys_getrlimit64_args {
	syscallarg(int) resource;
	syscallarg(struct irix_rlimit64 *) rlp;
};
check_syscall_args(irix_sys_getrlimit64)

struct irix_sys_setrlimit64_args {
	syscallarg(int) resource;
	syscallarg(const struct irix_rlimit64 *) rlp;
};
check_syscall_args(irix_sys_setrlimit64)

struct compat_50_sys_nanosleep_args;

struct irix_sys_lseek64_args {
	syscallarg(int) fd;
	syscallarg(int) pad1;
	syscallarg(irix_off64_t) offset;
	syscallarg(int) whence;
	syscallarg(int) pad2;
	syscallarg(int) pad3;
	syscallarg(int) pad4;
};
check_syscall_args(irix_sys_lseek64)

struct sys_rmdir_args;

struct sys_mkdir_args;

struct irix_sys_getdents_args {
	syscallarg(int) fildes;
	syscallarg(irix_dirent_t *) buf;
	syscallarg(int) nbytes;
};
check_syscall_args(irix_sys_getdents)

struct irix_sys_sginap_args {
	syscallarg(long) ticks;
};
check_syscall_args(irix_sys_sginap)

struct svr4_sys_getmsg_args;

struct svr4_sys_putmsg_args;

struct sys_poll_args;

struct irix_sys_sigreturn_args {
	syscallarg(struct irix_sigcontext *) scp;
	syscallarg(struct irix_ucontext *) ucp;
	syscallarg(int) signo;
};
check_syscall_args(irix_sys_sigreturn)

struct compat_43_sys_accept_args;

struct sys_bind_args;

struct sys_connect_args;

struct compat_43_sys_getpeername_args;

struct compat_43_sys_getsockname_args;

struct sys_getsockopt_args;

struct sys_listen_args;

struct compat_43_sys_recv_args;

struct compat_43_sys_recvfrom_args;

struct compat_43_sys_recvmsg_args;

struct compat_50_sys_select_args;

struct compat_43_sys_send_args;

struct compat_43_sys_sendmsg_args;

struct sys_sendto_args;

struct compat_43_sys_sethostid_args;

struct sys_setsockopt_args;

struct sys_shutdown_args;

struct svr4_sys_socket_args;

struct compat_43_sys_gethostname_args;

struct compat_43_sys_sethostname_args;

struct compat_09_sys_getdomainname_args;

struct compat_09_sys_setdomainname_args;

struct sys_truncate_args;

struct sys_ftruncate_args;

struct sys_rename_args;

struct sys_symlink_args;

struct sys_readlink_args;

struct sys_setregid_args;

struct sys_setreuid_args;

struct compat_50_sys_getitimer_args;

struct compat_50_sys_setitimer_args;

struct compat_50_sys_adjtime_args;

struct svr4_sys_gettimeofday_args;

struct irix_sys_sproc_args {
	syscallarg(void *) entry;
	syscallarg(unsigned int) inh;
	syscallarg(void *) arg;
};
check_syscall_args(irix_sys_sproc)

struct irix_sys_prctl_args {
	syscallarg(unsigned int) option;
	syscallarg(void *) arg1;
};
check_syscall_args(irix_sys_prctl)

struct irix_sys_procblk_args {
	syscallarg(int) cmd;
	syscallarg(pid_t) pid;
	syscallarg(int) count;
};
check_syscall_args(irix_sys_procblk)

struct irix_sys_sprocsp_args {
	syscallarg(void *) entry;
	syscallarg(unsigned int) inh;
	syscallarg(void *) arg;
	syscallarg(void *) sp;
	syscallarg(irix_size_t) len;
};
check_syscall_args(irix_sys_sprocsp)

struct irix_sys_mmap_args {
	syscallarg(void *) addr;
	syscallarg(irix_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(irix_off_t) pos;
};
check_syscall_args(irix_sys_mmap)

struct irix_sys_munmap_args {
	syscallarg(void *) addr;
	syscallarg(int) len;
};
check_syscall_args(irix_sys_munmap)

struct irix_sys_mprotect_args {
	syscallarg(void *) addr;
	syscallarg(int) len;
	syscallarg(int) prot;
};
check_syscall_args(irix_sys_mprotect)

struct sys___msync13_args;

struct irix_sys_setpgrp_args {
	syscallarg(int) pid;
	syscallarg(int) pgid;
};
check_syscall_args(irix_sys_setpgrp)

struct sys_fsync_args;

struct sys_fchdir_args;

struct irix_sys_getrlimit_args {
	syscallarg(int) resource;
	syscallarg(struct irix_rlimit *) rlp;
};
check_syscall_args(irix_sys_getrlimit)

struct irix_sys_setrlimit_args {
	syscallarg(int) resource;
	syscallarg(const struct irix_rlimit *) rlp;
};
check_syscall_args(irix_sys_setrlimit)

struct sys___posix_fchown_args;

struct sys_fchmod_args;

struct irix_sys_systeminfo_args {
	syscallarg(int) what;
	syscallarg(char *) buf;
	syscallarg(long) len;
};
check_syscall_args(irix_sys_systeminfo)

struct irix_sys_uname_args {
	syscallarg(struct irix_utsname *) name;
};
check_syscall_args(irix_sys_uname)

struct irix_sys_xstat_args {
	syscallarg(const int) version;
	syscallarg(const char *) path;
	syscallarg(struct stat *) buf;
};
check_syscall_args(irix_sys_xstat)

struct irix_sys_lxstat_args {
	syscallarg(const int) version;
	syscallarg(const char *) path;
	syscallarg(struct stat *) buf;
};
check_syscall_args(irix_sys_lxstat)

struct irix_sys_fxstat_args {
	syscallarg(const int) version;
	syscallarg(const int) fd;
	syscallarg(struct stat *) buf;
};
check_syscall_args(irix_sys_fxstat)

struct irix_sys_sigaction_args {
	syscallarg(int) signum;
	syscallarg(const struct svr4_sigaction *) nsa;
	syscallarg(struct svr4_sigaction *) osa;
	syscallarg(void *) sigtramp;
};
check_syscall_args(irix_sys_sigaction)

struct svr4_sys_sigpending_args;

struct irix_sys_sigprocmask_args {
	syscallarg(int) how;
	syscallarg(const irix_sigset_t *) set;
	syscallarg(irix_sigset_t *) oset;
};
check_syscall_args(irix_sys_sigprocmask)

struct svr4_sys_sigsuspend_args;

struct irix_sys_swapctl_args {
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};
check_syscall_args(irix_sys_swapctl)

struct irix_sys_getcontext_args {
	syscallarg(irix_ucontext_t *) ucp;
};
check_syscall_args(irix_sys_getcontext)

struct irix_sys_setcontext_args {
	syscallarg(const irix_ucontext_t *) ucp;
};
check_syscall_args(irix_sys_setcontext)

struct irix_sys_waitsys_args {
	syscallarg(int) type;
	syscallarg(int) pid;
	syscallarg(struct irix_irix5_siginfo *) info;
	syscallarg(int) options;
	syscallarg(struct rusage *) ru;
};
check_syscall_args(irix_sys_waitsys)

struct svr4_sys_statvfs_args;

struct svr4_sys_fstatvfs_args;

struct sys_readv_args;

struct sys_writev_args;

struct sys_truncate_args;

struct sys_ftruncate_args;

struct irix_sys_mmap64_args {
	syscallarg(void *) addr;
	syscallarg(irix_size_t) len;
	syscallarg(int) prot;
	syscallarg(int) flags;
	syscallarg(int) fd;
	syscallarg(int) pad1;
	syscallarg(irix_off_t) pos;
};
check_syscall_args(irix_sys_mmap64)

struct svr4_sys_pread_args;

struct svr4_sys_pwrite_args;

struct irix_sys_getmountid_args {
	syscallarg(const char *) path;
	syscallarg(irix_mountid_t *) buf;
};
check_syscall_args(irix_sys_getmountid)

struct irix_sys_getdents64_args {
	syscallarg(int) fildes;
	syscallarg(irix_dirent64_t *) buf;
	syscallarg(int) nbytes;
};
check_syscall_args(irix_sys_getdents64)

struct irix_sys_ngetdents_args {
	syscallarg(int) fildes;
	syscallarg(irix_dirent_t *) buf;
	syscallarg(unsigned short) nbyte;
	syscallarg(int *) eof;
};
check_syscall_args(irix_sys_ngetdents)

struct irix_sys_ngetdents64_args {
	syscallarg(int) fildes;
	syscallarg(irix_dirent64_t *) buf;
	syscallarg(unsigned short) nbyte;
	syscallarg(int *) eof;
};
check_syscall_args(irix_sys_ngetdents64)

struct irix_sys_pidsprocsp_args {
	syscallarg(void *) entry;
	syscallarg(unsigned int) inh;
	syscallarg(void *) arg;
	syscallarg(void *) sp;
	syscallarg(irix_size_t) len;
	syscallarg(irix_pid_t) pid;
};
check_syscall_args(irix_sys_pidsprocsp)

struct irix_sys_usync_cntl_args {
	syscallarg(int) cmd;
	syscallarg(void *) arg;
};
check_syscall_args(irix_sys_usync_cntl)

/*
 * System call prototypes.
 */

int	sys_nosys(struct lwp *, const void *, register_t *);

int	sys_exit(struct lwp *, const struct sys_exit_args *, register_t *);

int	sys_fork(struct lwp *, const void *, register_t *);

int	sys_read(struct lwp *, const struct sys_read_args *, register_t *);

int	sys_write(struct lwp *, const struct sys_write_args *, register_t *);

int	irix_sys_open(struct lwp *, const struct irix_sys_open_args *, register_t *);

int	sys_close(struct lwp *, const struct sys_close_args *, register_t *);

int	svr4_sys_creat(struct lwp *, const struct svr4_sys_creat_args *, register_t *);

int	sys_link(struct lwp *, const struct sys_link_args *, register_t *);

int	sys_unlink(struct lwp *, const struct sys_unlink_args *, register_t *);

int	svr4_sys_execv(struct lwp *, const struct svr4_sys_execv_args *, register_t *);

int	sys_chdir(struct lwp *, const struct sys_chdir_args *, register_t *);

int	svr4_sys_time(struct lwp *, const struct svr4_sys_time_args *, register_t *);

int	sys_chmod(struct lwp *, const struct sys_chmod_args *, register_t *);

int	sys___posix_chown(struct lwp *, const struct sys___posix_chown_args *, register_t *);

int	irix_sys_break(struct lwp *, const struct irix_sys_break_args *, register_t *);

int	compat_43_sys_lseek(struct lwp *, const struct compat_43_sys_lseek_args *, register_t *);

int	sys_getpid(struct lwp *, const void *, register_t *);

int	sys_setuid(struct lwp *, const struct sys_setuid_args *, register_t *);

int	sys_getuid_with_euid(struct lwp *, const void *, register_t *);

int	svr4_sys_alarm(struct lwp *, const struct svr4_sys_alarm_args *, register_t *);

int	svr4_sys_fstat(struct lwp *, const struct svr4_sys_fstat_args *, register_t *);

int	svr4_sys_pause(struct lwp *, const void *, register_t *);

int	svr4_sys_utime(struct lwp *, const struct svr4_sys_utime_args *, register_t *);

int	svr4_sys_access(struct lwp *, const struct svr4_sys_access_args *, register_t *);

int	svr4_sys_nice(struct lwp *, const struct svr4_sys_nice_args *, register_t *);

int	sys_sync(struct lwp *, const void *, register_t *);

int	svr4_sys_kill(struct lwp *, const struct svr4_sys_kill_args *, register_t *);

int	svr4_sys_pgrpsys(struct lwp *, const struct svr4_sys_pgrpsys_args *, register_t *);

int	irix_sys_syssgi(struct lwp *, const struct irix_sys_syssgi_args *, register_t *);

int	sys_dup(struct lwp *, const struct sys_dup_args *, register_t *);

int	sys_pipe(struct lwp *, const void *, register_t *);

int	svr4_sys_times(struct lwp *, const struct svr4_sys_times_args *, register_t *);

int	sys_setgid(struct lwp *, const struct sys_setgid_args *, register_t *);

int	sys_getgid_with_egid(struct lwp *, const void *, register_t *);

#ifdef SYSVMSG
int	svr4_sys_msgsys(struct lwp *, const struct svr4_sys_msgsys_args *, register_t *);

#else
#endif
#ifdef SYSVSHM
int	irix_sys_shmsys(struct lwp *, const struct irix_sys_shmsys_args *, register_t *);

#else
#endif
#ifdef SYSVSEM
int	svr4_sys_semsys(struct lwp *, const struct svr4_sys_semsys_args *, register_t *);

#else
#endif
int	irix_sys_ioctl(struct lwp *, const struct irix_sys_ioctl_args *, register_t *);

int	irix_sys_sysmp(struct lwp *, const struct irix_sys_sysmp_args *, register_t *);

int	irix_sys_utssys(struct lwp *, const struct irix_sys_utssys_args *, register_t *);

int	svr4_sys_execve(struct lwp *, const struct svr4_sys_execve_args *, register_t *);

int	sys_umask(struct lwp *, const struct sys_umask_args *, register_t *);

int	sys_chroot(struct lwp *, const struct sys_chroot_args *, register_t *);

int	irix_sys_fcntl(struct lwp *, const struct irix_sys_fcntl_args *, register_t *);

int	svr4_sys_ulimit(struct lwp *, const struct svr4_sys_ulimit_args *, register_t *);

int	irix_sys_getrlimit64(struct lwp *, const struct irix_sys_getrlimit64_args *, register_t *);

int	irix_sys_setrlimit64(struct lwp *, const struct irix_sys_setrlimit64_args *, register_t *);

int	compat_50_sys_nanosleep(struct lwp *, const struct compat_50_sys_nanosleep_args *, register_t *);

int	irix_sys_lseek64(struct lwp *, const struct irix_sys_lseek64_args *, register_t *);

int	sys_rmdir(struct lwp *, const struct sys_rmdir_args *, register_t *);

int	sys_mkdir(struct lwp *, const struct sys_mkdir_args *, register_t *);

int	irix_sys_getdents(struct lwp *, const struct irix_sys_getdents_args *, register_t *);

int	irix_sys_sginap(struct lwp *, const struct irix_sys_sginap_args *, register_t *);

int	svr4_sys_getmsg(struct lwp *, const struct svr4_sys_getmsg_args *, register_t *);

int	svr4_sys_putmsg(struct lwp *, const struct svr4_sys_putmsg_args *, register_t *);

int	sys_poll(struct lwp *, const struct sys_poll_args *, register_t *);

int	irix_sys_sigreturn(struct lwp *, const struct irix_sys_sigreturn_args *, register_t *);

int	compat_43_sys_accept(struct lwp *, const struct compat_43_sys_accept_args *, register_t *);

int	sys_bind(struct lwp *, const struct sys_bind_args *, register_t *);

int	sys_connect(struct lwp *, const struct sys_connect_args *, register_t *);

int	compat_43_sys_gethostid(struct lwp *, const void *, register_t *);

int	compat_43_sys_getpeername(struct lwp *, const struct compat_43_sys_getpeername_args *, register_t *);

int	compat_43_sys_getsockname(struct lwp *, const struct compat_43_sys_getsockname_args *, register_t *);

int	sys_getsockopt(struct lwp *, const struct sys_getsockopt_args *, register_t *);

int	sys_listen(struct lwp *, const struct sys_listen_args *, register_t *);

int	compat_43_sys_recv(struct lwp *, const struct compat_43_sys_recv_args *, register_t *);

int	compat_43_sys_recvfrom(struct lwp *, const struct compat_43_sys_recvfrom_args *, register_t *);

int	compat_43_sys_recvmsg(struct lwp *, const struct compat_43_sys_recvmsg_args *, register_t *);

int	compat_50_sys_select(struct lwp *, const struct compat_50_sys_select_args *, register_t *);

int	compat_43_sys_send(struct lwp *, const struct compat_43_sys_send_args *, register_t *);

int	compat_43_sys_sendmsg(struct lwp *, const struct compat_43_sys_sendmsg_args *, register_t *);

int	sys_sendto(struct lwp *, const struct sys_sendto_args *, register_t *);

int	compat_43_sys_sethostid(struct lwp *, const struct compat_43_sys_sethostid_args *, register_t *);

int	sys_setsockopt(struct lwp *, const struct sys_setsockopt_args *, register_t *);

int	sys_shutdown(struct lwp *, const struct sys_shutdown_args *, register_t *);

int	svr4_sys_socket(struct lwp *, const struct svr4_sys_socket_args *, register_t *);

int	compat_43_sys_gethostname(struct lwp *, const struct compat_43_sys_gethostname_args *, register_t *);

int	compat_43_sys_sethostname(struct lwp *, const struct compat_43_sys_sethostname_args *, register_t *);

int	compat_09_sys_getdomainname(struct lwp *, const struct compat_09_sys_getdomainname_args *, register_t *);

int	compat_09_sys_setdomainname(struct lwp *, const struct compat_09_sys_setdomainname_args *, register_t *);

int	sys_truncate(struct lwp *, const struct sys_truncate_args *, register_t *);

int	sys_ftruncate(struct lwp *, const struct sys_ftruncate_args *, register_t *);

int	sys_rename(struct lwp *, const struct sys_rename_args *, register_t *);

int	sys_symlink(struct lwp *, const struct sys_symlink_args *, register_t *);

int	sys_readlink(struct lwp *, const struct sys_readlink_args *, register_t *);

int	sys_setregid(struct lwp *, const struct sys_setregid_args *, register_t *);

int	sys_setreuid(struct lwp *, const struct sys_setreuid_args *, register_t *);

int	compat_50_sys_getitimer(struct lwp *, const struct compat_50_sys_getitimer_args *, register_t *);

int	compat_50_sys_setitimer(struct lwp *, const struct compat_50_sys_setitimer_args *, register_t *);

int	compat_50_sys_adjtime(struct lwp *, const struct compat_50_sys_adjtime_args *, register_t *);

int	svr4_sys_gettimeofday(struct lwp *, const struct svr4_sys_gettimeofday_args *, register_t *);

int	irix_sys_sproc(struct lwp *, const struct irix_sys_sproc_args *, register_t *);

int	irix_sys_prctl(struct lwp *, const struct irix_sys_prctl_args *, register_t *);

int	irix_sys_procblk(struct lwp *, const struct irix_sys_procblk_args *, register_t *);

int	irix_sys_sprocsp(struct lwp *, const struct irix_sys_sprocsp_args *, register_t *);

int	irix_sys_mmap(struct lwp *, const struct irix_sys_mmap_args *, register_t *);

int	irix_sys_munmap(struct lwp *, const struct irix_sys_munmap_args *, register_t *);

int	irix_sys_mprotect(struct lwp *, const struct irix_sys_mprotect_args *, register_t *);

int	sys___msync13(struct lwp *, const struct sys___msync13_args *, register_t *);

int	sys_getpgrp(struct lwp *, const void *, register_t *);

int	irix_sys_setpgrp(struct lwp *, const struct irix_sys_setpgrp_args *, register_t *);

int	sys_fsync(struct lwp *, const struct sys_fsync_args *, register_t *);

int	sys_fchdir(struct lwp *, const struct sys_fchdir_args *, register_t *);

int	irix_sys_getrlimit(struct lwp *, const struct irix_sys_getrlimit_args *, register_t *);

int	irix_sys_setrlimit(struct lwp *, const struct irix_sys_setrlimit_args *, register_t *);

int	sys___posix_fchown(struct lwp *, const struct sys___posix_fchown_args *, register_t *);

int	sys_fchmod(struct lwp *, const struct sys_fchmod_args *, register_t *);

int	irix_sys_systeminfo(struct lwp *, const struct irix_sys_systeminfo_args *, register_t *);

int	irix_sys_uname(struct lwp *, const struct irix_sys_uname_args *, register_t *);

int	irix_sys_xstat(struct lwp *, const struct irix_sys_xstat_args *, register_t *);

int	irix_sys_lxstat(struct lwp *, const struct irix_sys_lxstat_args *, register_t *);

int	irix_sys_fxstat(struct lwp *, const struct irix_sys_fxstat_args *, register_t *);

int	irix_sys_sigaction(struct lwp *, const struct irix_sys_sigaction_args *, register_t *);

int	svr4_sys_sigpending(struct lwp *, const struct svr4_sys_sigpending_args *, register_t *);

int	irix_sys_sigprocmask(struct lwp *, const struct irix_sys_sigprocmask_args *, register_t *);

int	svr4_sys_sigsuspend(struct lwp *, const struct svr4_sys_sigsuspend_args *, register_t *);

int	irix_sys_swapctl(struct lwp *, const struct irix_sys_swapctl_args *, register_t *);

int	irix_sys_getcontext(struct lwp *, const struct irix_sys_getcontext_args *, register_t *);

int	irix_sys_setcontext(struct lwp *, const struct irix_sys_setcontext_args *, register_t *);

int	irix_sys_waitsys(struct lwp *, const struct irix_sys_waitsys_args *, register_t *);

int	svr4_sys_statvfs(struct lwp *, const struct svr4_sys_statvfs_args *, register_t *);

int	svr4_sys_fstatvfs(struct lwp *, const struct svr4_sys_fstatvfs_args *, register_t *);

int	sys_readv(struct lwp *, const struct sys_readv_args *, register_t *);

int	sys_writev(struct lwp *, const struct sys_writev_args *, register_t *);

int	irix_sys_mmap64(struct lwp *, const struct irix_sys_mmap64_args *, register_t *);

int	svr4_sys_pread(struct lwp *, const struct svr4_sys_pread_args *, register_t *);

int	svr4_sys_pwrite(struct lwp *, const struct svr4_sys_pwrite_args *, register_t *);

int	irix_sys_getmountid(struct lwp *, const struct irix_sys_getmountid_args *, register_t *);

int	irix_sys_getdents64(struct lwp *, const struct irix_sys_getdents64_args *, register_t *);

int	irix_sys_ngetdents(struct lwp *, const struct irix_sys_ngetdents_args *, register_t *);

int	irix_sys_ngetdents64(struct lwp *, const struct irix_sys_ngetdents64_args *, register_t *);

int	irix_sys_pidsprocsp(struct lwp *, const struct irix_sys_pidsprocsp_args *, register_t *);

int	irix_sys_usync_cntl(struct lwp *, const struct irix_sys_usync_cntl_args *, register_t *);

#endif /* _IRIX_SYS_SYSCALLARGS_H_ */
