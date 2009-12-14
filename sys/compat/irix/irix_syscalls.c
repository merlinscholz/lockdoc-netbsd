/* $NetBSD: irix_syscalls.c,v 1.61 2009/12/14 00:58:36 matt Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.54 2009/01/13 22:27:43 pooka Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: irix_syscalls.c,v 1.61 2009/12/14 00:58:36 matt Exp $");

#if defined(_KERNEL_OPT)
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
#include <compat/svr4/svr4_statvfs.h>
#include <compat/svr4/svr4_syscallargs.h>
#include <compat/irix/irix_types.h>
#include <compat/irix/irix_signal.h>
#include <compat/irix/irix_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const irix_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"#7 (obsolete wait)",
	/*   8 */	"creat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"execv",
	/*  12 */	"chdir",
	/*  13 */	"time",
	/*  14 */	"#14 (obsolete mknod)",
	/*  15 */	"chmod",
	/*  16 */	"chown",
	/*  17 */	"break",
	/*  18 */	"#18 (obsolete stat)",
	/*  19 */	"lseek",
	/*  20 */	"getpid",
	/*  21 */	"#21 (unimplemented old_mount)",
	/*  22 */	"#22 (unimplemented System V umount)",
	/*  23 */	"setuid",
	/*  24 */	"getuid_with_euid",
	/*  25 */	"#25 (unimplemented stime)",
	/*  26 */	"#26 (unimplemented ptrace)",
	/*  27 */	"alarm",
	/*  28 */	"fstat",
	/*  29 */	"pause",
	/*  30 */	"utime",
	/*  31 */	"#31 (unimplemented was stty)",
	/*  32 */	"#32 (unimplemented was gtty)",
	/*  33 */	"access",
	/*  34 */	"nice",
	/*  35 */	"#35 (unimplemented statfs)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"#38 (unimplemented fstatfs)",
	/*  39 */	"pgrpsys",
	/*  40 */	"syssgi",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"times",
	/*  44 */	"#44 (unimplemented profil)",
	/*  45 */	"#45 (unimplemented plock)",
	/*  46 */	"setgid",
	/*  47 */	"getgid_with_egid",
	/*  48 */	"#48 (obsolete ssig)",
#ifdef SYSVMSG
	/*  49 */	"msgsys",
#else
	/*  49 */	"#49 (unimplemented msgsys)",
#endif
	/*  50 */	"#50 (unimplemented sysmips)",
	/*  51 */	"#51 (unimplemented acct)",
#ifdef SYSVSHM
	/*  52 */	"shmsys",
#else
	/*  52 */	"#52 (unimplemented shmsys)",
#endif
#ifdef SYSVSEM
	/*  53 */	"semsys",
#else
	/*  53 */	"#53 (unimplemented semsys)",
#endif
	/*  54 */	"ioctl",
	/*  55 */	"#55 (unimplemented uadmin)",
	/*  56 */	"sysmp",
	/*  57 */	"utssys",
	/*  58 */	"#58 (unimplemented)",
	/*  59 */	"execve",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"fcntl",
	/*  63 */	"ulimit",
	/*  64 */	"#64 (unimplemented reserved for unix/pc)",
	/*  65 */	"#65 (unimplemented reserved for unix/pc)",
	/*  66 */	"#66 (unimplemented reserved for unix/pc)",
	/*  67 */	"#67 (unimplemented reserved for unix/pc)",
	/*  68 */	"#68 (unimplemented reserved for unix/pc)",
	/*  69 */	"#69 (unimplemented reserved for unix/pc)",
	/*  70 */	"#70 (obsolete advfs)",
	/*  71 */	"#71 (obsolete unadvfs)",
	/*  72 */	"#72 (obsolete rmount)",
	/*  73 */	"#73 (obsolete rumount)",
	/*  74 */	"#74 (obsolete rfstart)",
	/*  75 */	"getrlimit64",
	/*  76 */	"setrlimit64",
	/*  77 */	"nanosleep",
	/*  78 */	"lseek64",
	/*  79 */	"rmdir",
	/*  80 */	"mkdir",
	/*  81 */	"getdents",
	/*  82 */	"sginap",
	/*  83 */	"#83 (unimplemented sgikopt)",
	/*  84 */	"#84 (unimplemented sysfs)",
	/*  85 */	"getmsg",
	/*  86 */	"putmsg",
	/*  87 */	"poll",
	/*  88 */	"sigreturn",
	/*  89 */	"accept",
	/*  90 */	"bind",
	/*  91 */	"connect",
	/*  92 */	"gethostid",
	/*  93 */	"getpeername",
	/*  94 */	"getsockname",
	/*  95 */	"getsockopt",
	/*  96 */	"listen",
	/*  97 */	"recv",
	/*  98 */	"recvfrom",
	/*  99 */	"recvmsg",
	/* 100 */	"select",
	/* 101 */	"send",
	/* 102 */	"sendmsg",
	/* 103 */	"sendto",
	/* 104 */	"sethostid",
	/* 105 */	"setsockopt",
	/* 106 */	"shutdown",
	/* 107 */	"socket",
	/* 108 */	"gethostname",
	/* 109 */	"sethostname",
	/* 110 */	"getdomainname",
	/* 111 */	"setdomainname",
	/* 112 */	"truncate",
	/* 113 */	"ftruncate",
	/* 114 */	"rename",
	/* 115 */	"symlink",
	/* 116 */	"readlink",
	/* 117 */	"#117 (unimplemented lstat)",
	/* 118 */	"#118 (unimplemented)",
	/* 119 */	"#119 (unimplemented nfs_svc)",
	/* 120 */	"#120 (unimplemented nfs_getfh)",
	/* 121 */	"#121 (unimplemented async_daemon)",
	/* 122 */	"#122 (unimplemented exportfs)",
	/* 123 */	"setregid",
	/* 124 */	"setreuid",
	/* 125 */	"getitimer",
	/* 126 */	"setitimer",
	/* 127 */	"adjtime",
	/* 128 */	"gettimeofday",
	/* 129 */	"sproc",
	/* 130 */	"prctl",
	/* 131 */	"procblk",
	/* 132 */	"sprocsp",
	/* 133 */	"#133 (unimplemented sgigsc)",
	/* 134 */	"mmap",
	/* 135 */	"munmap",
	/* 136 */	"mprotect",
	/* 137 */	"__msync13",
	/* 138 */	"#138 (unimplemented madvise)",
	/* 139 */	"#139 (unimplemented pagelock)",
	/* 140 */	"#140 (unimplemented getpagesize)",
	/* 141 */	"#141 (unimplemented quotactl)",
	/* 142 */	"#142 (unimplemented)",
	/* 143 */	"getpgrp",
	/* 144 */	"setpgrp",
	/* 145 */	"#145 (unimplemented vhangup)",
	/* 146 */	"fsync",
	/* 147 */	"fchdir",
	/* 148 */	"getrlimit",
	/* 149 */	"setrlimit",
	/* 150 */	"#150 (unimplemented cacheflush)",
	/* 151 */	"#151 (unimplemented cachectl)",
	/* 152 */	"fchown",
	/* 153 */	"fchmod",
	/* 154 */	"#154 (unimplemented wait3)",
	/* 155 */	"#155 (unimplemented socketpair)",
	/* 156 */	"systeminfo",
	/* 157 */	"uname",
	/* 158 */	"xstat",
	/* 159 */	"lxstat",
	/* 160 */	"fxstat",
	/* 161 */	"#161 (unimplemented xmknod)",
	/* 162 */	"sigaction",
	/* 163 */	"sigpending",
	/* 164 */	"sigprocmask",
	/* 165 */	"sigsuspend",
	/* 166 */	"#166 (unimplemented sigpoll_sys)",
	/* 167 */	"swapctl",
	/* 168 */	"getcontext",
	/* 169 */	"setcontext",
	/* 170 */	"waitsys",
	/* 171 */	"#171 (unimplemented sigstack)",
	/* 172 */	"#172 (unimplemented sigaltstack)",
	/* 173 */	"#173 (unimplemented sigsendset)",
	/* 174 */	"statvfs",
	/* 175 */	"fstatvfs",
	/* 176 */	"#176 (unimplemented getpmsg)",
	/* 177 */	"#177 (unimplemented putpmsg)",
	/* 178 */	"#178 (unimplemented lchown)",
	/* 179 */	"#179 (unimplemented priocntl)",
	/* 180 */	"#180 (unimplemented sigqueue)",
	/* 181 */	"readv",
	/* 182 */	"writev",
	/* 183 */	"truncate64",
	/* 184 */	"ftruncate64",
	/* 185 */	"mmap64",
	/* 186 */	"#186 (unimplemented dmi)",
	/* 187 */	"pread",
	/* 188 */	"pwrite",
	/* 189 */	"#189 (unimplemented fdatasync)",
	/* 190 */	"#190 (unimplemented sgifastpath)",
	/* 191 */	"#191 (unimplemented attr_get)",
	/* 192 */	"#192 (unimplemented attr_getf)",
	/* 193 */	"#193 (unimplemented attr_set)",
	/* 194 */	"#194 (unimplemented attr_setf)",
	/* 195 */	"#195 (unimplemented attr_remove)",
	/* 196 */	"#196 (unimplemented attr_removef)",
	/* 197 */	"#197 (unimplemented attr_list)",
	/* 198 */	"#198 (unimplemented attr_listf)",
	/* 199 */	"#199 (unimplemented attr_multi)",
	/* 200 */	"#200 (unimplemented attr_multif)",
	/* 201 */	"#201 (unimplemented statvfs64)",
	/* 202 */	"#202 (unimplemented fstatvfs64)",
	/* 203 */	"getmountid",
	/* 204 */	"#204 (unimplemented nsproc)",
	/* 205 */	"getdents64",
	/* 206 */	"#206 (unimplemented afs_syscall)",
	/* 207 */	"ngetdents",
	/* 208 */	"ngetdents64",
	/* 209 */	"#209 (unimplemented sgi_sesmgr)",
	/* 210 */	"pidsprocsp",
	/* 211 */	"#211 (unimplemented rexec)",
	/* 212 */	"#212 (unimplemented timer_create)",
	/* 213 */	"#213 (unimplemented timer_delete)",
	/* 214 */	"#214 (unimplemented timer_settime)",
	/* 215 */	"#215 (unimplemented timer_gettime)",
	/* 216 */	"#216 (unimplemented timer_setoverrun)",
	/* 217 */	"#217 (unimplemented sched_rr_get_interval)",
	/* 218 */	"#218 (unimplemented sched_yield)",
	/* 219 */	"#219 (unimplemented sched_getscheduler)",
	/* 220 */	"#220 (unimplemented sched_setscheduler)",
	/* 221 */	"#221 (unimplemented sched_getparam)",
	/* 222 */	"#222 (unimplemented sched_setparam)",
	/* 223 */	"usync_cntl",
	/* 224 */	"#224 (unimplemented psema_cntl)",
	/* 225 */	"#225 (unimplemented restartreturn)",
	/* 226 */	"#226 (unimplemented sysget)",
	/* 227 */	"#227 (unimplemented xpg4_recvmsg)",
	/* 228 */	"#228 (unimplemented umfscall)",
	/* 229 */	"#229 (unimplemented nsproctid)",
	/* 230 */	"#230 (unimplemented rexec_complete)",
	/* 231 */	"#231 (unimplemented xpg4_sigaltstack)",
	/* 232 */	"#232 (unimplemented xpg4_sigaltstack)",
	/* 233 */	"#233 (unimplemented xpg4_setregid)",
	/* 234 */	"#234 (unimplemented linkfollow)",
	/* 235 */	"#235 (unimplemented utimets)",
};
