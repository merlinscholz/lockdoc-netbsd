/*	$NetBSD: linux_syscalls.c,v 1.3 1999/03/23 03:01:51 thorpej Exp $	*/

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.3 1999/03/23 03:00:52 thorpej Exp 
 */

#if defined(_KERNEL) && !defined(_LKM)
#include "opt_compat_netbsd.h"
#include "opt_compat_43.h"
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/syscallargs.h>
#include <compat/linux/common/linux_types.h>
#include <compat/linux/common/linux_signal.h>
#include <compat/linux/common/linux_siginfo.h>
#include <compat/linux/common/linux_machdep.h>
#include <compat/linux/linux_syscallargs.h>
#endif /* _KERNEL && ! _LKM */

char *linux_syscallnames[] = {
	"syscall",			/* 0 = syscall */
	"exit",			/* 1 = exit */
	"fork",			/* 2 = fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"open",			/* 5 = open */
	"close",			/* 6 = close */
	"waitpid",			/* 7 = waitpid */
	"creat",			/* 8 = creat */
	"link",			/* 9 = link */
	"unlink",			/* 10 = unlink */
	"execve",			/* 11 = execve */
	"chdir",			/* 12 = chdir */
	"time",			/* 13 = time */
	"mknod",			/* 14 = mknod */
	"chmod",			/* 15 = chmod */
	"chown",			/* 16 = chown */
	"#17 (obsolete break)",		/* 17 = obsolete break */
	"#18 (obsolete ostat)",		/* 18 = obsolete ostat */
#if !defined(_KERNEL) || defined(COMPAT_43)
	"lseek",			/* 19 = lseek */
#else
	"#19 (unimplemented compat_43_sys_lseek)",		/* 19 = unimplemented compat_43_sys_lseek */
#endif
	"getpid",			/* 20 = getpid */
	"#21 (unimplemented mount)",		/* 21 = unimplemented mount */
	"#22 (obsolete umount)",		/* 22 = obsolete umount */
	"setuid",			/* 23 = setuid */
	"getuid",			/* 24 = getuid */
	"#25 (unimplemented stime)",		/* 25 = unimplemented stime */
	"#26 (unimplemented ptrace)",		/* 26 = unimplemented ptrace */
	"alarm",			/* 27 = alarm */
	"#28 (obsolete ofstat)",		/* 28 = obsolete ofstat */
	"pause",			/* 29 = pause */
	"utime",			/* 30 = utime */
	"#31 (obsolete stty)",		/* 31 = obsolete stty */
	"#32 (obsolete gtty)",		/* 32 = obsolete gtty */
	"access",			/* 33 = access */
	"nice",			/* 34 = nice */
	"#35 (obsolete ftime)",		/* 35 = obsolete ftime */
	"sync",			/* 36 = sync */
	"kill",			/* 37 = kill */
	"rename",			/* 38 = rename */
	"mkdir",			/* 39 = mkdir */
	"rmdir",			/* 40 = rmdir */
	"dup",			/* 41 = dup */
	"pipe",			/* 42 = pipe */
	"times",			/* 43 = times */
	"#44 (obsolete prof)",		/* 44 = obsolete prof */
	"brk",			/* 45 = brk */
	"setgid",			/* 46 = setgid */
	"getgid",			/* 47 = getgid */
	"signal",			/* 48 = signal */
	"geteuid",			/* 49 = geteuid */
	"getegid",			/* 50 = getegid */
	"acct",			/* 51 = acct */
	"#52 (unimplemented umount)",		/* 52 = unimplemented umount */
	"#53 (obsolete lock)",		/* 53 = obsolete lock */
	"ioctl",			/* 54 = ioctl */
	"fcntl",			/* 55 = fcntl */
	"#56 (obsolete mpx)",		/* 56 = obsolete mpx */
	"setpgid",			/* 57 = setpgid */
	"#58 (obsolete ulimit)",		/* 58 = obsolete ulimit */
	"#59 (unimplemented oldolduname)",		/* 59 = unimplemented oldolduname */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"#62 (unimplemented ustat)",		/* 62 = unimplemented ustat */
	"dup2",			/* 63 = dup2 */
	"getppid",			/* 64 = getppid */
	"getpgrp",			/* 65 = getpgrp */
	"setsid",			/* 66 = setsid */
	"sigaction",			/* 67 = sigaction */
	"siggetmask",			/* 68 = siggetmask */
	"sigsetmask",			/* 69 = sigsetmask */
	"setreuid",			/* 70 = setreuid */
	"setregid",			/* 71 = setregid */
	"sigsuspend",			/* 72 = sigsuspend */
	"sigpending",			/* 73 = sigpending */
#if !defined(_KERNEL) || defined(COMPAT_43)
	"sethostname",			/* 74 = sethostname */
	"setrlimit",			/* 75 = setrlimit */
	"getrlimit",			/* 76 = getrlimit */
#else
	"#74 (unimplemented compat_43_sys_sethostname)",		/* 74 = unimplemented compat_43_sys_sethostname */
	"#75 (unimplemented compat_43_sys_setrlimit)",		/* 75 = unimplemented compat_43_sys_setrlimit */
	"#76 (unimplemented compat_43_sys_getrlimit)",		/* 76 = unimplemented compat_43_sys_getrlimit */
#endif
	"getrusage",			/* 77 = getrusage */
	"gettimeofday",			/* 78 = gettimeofday */
	"settimeofday",			/* 79 = settimeofday */
	"getgroups",			/* 80 = getgroups */
	"setgroups",			/* 81 = setgroups */
	"oldselect",			/* 82 = oldselect */
	"symlink",			/* 83 = symlink */
#if !defined(_KERNEL) || defined(COMPAT_43)
	"oolstat",			/* 84 = oolstat */
#else
	"#84 (unimplemented compat_43_sys_lstat)",		/* 84 = unimplemented compat_43_sys_lstat */
#endif
	"readlink",			/* 85 = readlink */
	"uselib",			/* 86 = uselib */
#if !defined(_KERNEL) || defined(COMPAT_12)
	"swapon",			/* 87 = swapon */
#else
	"#87 (unimplemented compat_12_sys_swapon)",		/* 87 = unimplemented compat_12_sys_swapon */
#endif
	"reboot",			/* 88 = reboot */
	"readdir",			/* 89 = readdir */
	"old_mmap",			/* 90 = old_mmap */
	"munmap",			/* 91 = munmap */
	"truncate",			/* 92 = truncate */
#if !defined(_KERNEL) || defined(COMPAT_43)
	"ftruncate",			/* 93 = ftruncate */
#else
	"#93 (unimplemented compat_43_sys_ftruncate)",		/* 93 = unimplemented compat_43_sys_ftruncate */
#endif
	"fchmod",			/* 94 = fchmod */
	"fchown",			/* 95 = fchown */
	"getpriority",			/* 96 = getpriority */
	"setpriority",			/* 97 = setpriority */
	"profil",			/* 98 = profil */
	"statfs",			/* 99 = statfs */
	"fstatfs",			/* 100 = fstatfs */
	"#101 (unimplemented ioperm)",		/* 101 = unimplemented ioperm */
	"socketcall",			/* 102 = socketcall */
	"#103 (unimplemented syslog)",		/* 103 = unimplemented syslog */
	"setitimer",			/* 104 = setitimer */
	"getitimer",			/* 105 = getitimer */
	"stat",			/* 106 = stat */
	"lstat",			/* 107 = lstat */
	"fstat",			/* 108 = fstat */
	"#109 (unimplemented olduname)",		/* 109 = unimplemented olduname */
	"#110 (unimplemented iopl)",		/* 110 = unimplemented iopl */
	"#111 (unimplemented vhangup)",		/* 111 = unimplemented vhangup */
	"#112 (unimplemented idle)",		/* 112 = unimplemented idle */
	"#113 (unimplemented vm86old)",		/* 113 = unimplemented vm86old */
	"wait4",			/* 114 = wait4 */
	"#115 (unimplemented swapoff)",		/* 115 = unimplemented swapoff */
	"#116 (unimplemented sysinfo)",		/* 116 = unimplemented sysinfo */
	"ipc",			/* 117 = ipc */
	"fsync",			/* 118 = fsync */
	"sigreturn",			/* 119 = sigreturn */
	"#120 (unimplemented clone)",		/* 120 = unimplemented clone */
#if !defined(_KERNEL) || defined(COMPAT_09)
	"setdomainname",			/* 121 = setdomainname */
#else
	"#121 (unimplemented compat_09_sys_setdomainname)",		/* 121 = unimplemented compat_09_sys_setdomainname */
#endif
	"uname",			/* 122 = uname */
	"cacheflush",			/* 123 = cacheflush */
	"#124 (unimplemented adjtimex)",		/* 124 = unimplemented adjtimex */
	"mprotect",			/* 125 = mprotect */
	"sigprocmask",			/* 126 = sigprocmask */
	"#127 (unimplemented create_module)",		/* 127 = unimplemented create_module */
	"#128 (unimplemented init_module)",		/* 128 = unimplemented init_module */
	"#129 (unimplemented delete_module)",		/* 129 = unimplemented delete_module */
	"#130 (unimplemented get_kernel_syms)",		/* 130 = unimplemented get_kernel_syms */
	"#131 (unimplemented quotactl)",		/* 131 = unimplemented quotactl */
	"getpgid",			/* 132 = getpgid */
	"fchdir",			/* 133 = fchdir */
	"#134 (unimplemented bdflush)",		/* 134 = unimplemented bdflush */
	"#135 (unimplemented sysfs)",		/* 135 = unimplemented sysfs */
	"personality",			/* 136 = personality */
	"#137 (unimplemented afs_syscall)",		/* 137 = unimplemented afs_syscall */
	"#138 (unimplemented setfsuid)",		/* 138 = unimplemented setfsuid */
	"#139 (unimplemented getfsuid)",		/* 139 = unimplemented getfsuid */
	"llseek",			/* 140 = llseek */
	"getdents",			/* 141 = getdents */
	"select",			/* 142 = select */
	"flock",			/* 143 = flock */
	"msync",			/* 144 = msync */
	"readv",			/* 145 = readv */
	"writev",			/* 146 = writev */
	"getsid",			/* 147 = getsid */
	"fdatasync",			/* 148 = fdatasync */
	"__sysctl",			/* 149 = __sysctl */
	"mlock",			/* 150 = mlock */
	"munlock",			/* 151 = munlock */
	"#152 (unimplemented mlockall)",		/* 152 = unimplemented mlockall */
	"#153 (unimplemented munlockall)",		/* 153 = unimplemented munlockall */
	"#154 (unimplemented sched_setparam)",		/* 154 = unimplemented sched_setparam */
	"#155 (unimplemented sched_getparam)",		/* 155 = unimplemented sched_getparam */
	"#156 (unimplemented sched_setscheduler)",		/* 156 = unimplemented sched_setscheduler */
	"#157 (unimplemented sched_getscheduler)",		/* 157 = unimplemented sched_getscheduler */
	"#158 (unimplemented sched_yield)",		/* 158 = unimplemented sched_yield */
	"#159 (unimplemented sched_get_priority_max)",		/* 159 = unimplemented sched_get_priority_max */
	"#160 (unimplemented sched_get_priority_min)",		/* 160 = unimplemented sched_get_priority_min */
	"#161 (unimplemented sched_rr_get_interval)",		/* 161 = unimplemented sched_rr_get_interval */
	"nanosleep",			/* 162 = nanosleep */
	"mremap",			/* 163 = mremap */
	"#164 (unimplemented setresuid)",		/* 164 = unimplemented setresuid */
	"#165 (unimplemented getresuid)",		/* 165 = unimplemented getresuid */
	"#166 (unimplemented vm86)",		/* 166 = unimplemented vm86 */
	"#167 (unimplemented query_module)",		/* 167 = unimplemented query_module */
	"#168 (unimplemented poll)",		/* 168 = unimplemented poll */
	"#169 (unimplemented nfsservctl)",		/* 169 = unimplemented nfsservctl */
	"#170 (unimplemented setresgid)",		/* 170 = unimplemented setresgid */
	"#171 (unimplemented setresgid)",		/* 171 = unimplemented setresgid */
	"#172 (unimplemented prctl)",		/* 172 = unimplemented prctl */
	"rt_sigreturn",			/* 173 = rt_sigreturn */
	"rt_sigaction",			/* 174 = rt_sigaction */
	"rt_sigprocmask",			/* 175 = rt_sigprocmask */
	"rt_sigpending",			/* 176 = rt_sigpending */
	"#177 (unimplemented rt_sigtimedwait)",		/* 177 = unimplemented rt_sigtimedwait */
	"rt_queueinfo",			/* 178 = rt_queueinfo */
	"rt_sigsuspend",			/* 179 = rt_sigsuspend */
	"pread",			/* 180 = pread */
	"pwrite",			/* 181 = pwrite */
	"lchown",			/* 182 = lchown */
	"#183 (unimplemented getcwd)",		/* 183 = unimplemented getcwd */
	"#184 (unimplemented capget)",		/* 184 = unimplemented capget */
	"#185 (unimplemented capset)",		/* 185 = unimplemented capset */
	"#186 (unimplemented sigaltstack)",		/* 186 = unimplemented sigaltstack */
};
