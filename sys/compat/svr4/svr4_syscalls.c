/* $NetBSD: svr4_syscalls.c,v 1.84 2009/12/14 00:58:37 matt Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.61 2009/01/13 22:27:44 pooka Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: svr4_syscalls.c,v 1.84 2009/12/14 00:58:37 matt Exp $");

#if defined(_KERNEL_OPT)
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
#endif /* _KERNEL_OPT */

const char *const svr4_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"wait",
	/*   8 */	"creat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"execv",
	/*  12 */	"chdir",
	/*  13 */	"time",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"chown",
	/*  17 */	"break",
	/*  18 */	"stat",
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
	/*  40 */	"#40 (unimplemented xenix)",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"times",
	/*  44 */	"#44 (unimplemented profil)",
	/*  45 */	"#45 (unimplemented plock)",
	/*  46 */	"setgid",
	/*  47 */	"getgid_with_egid",
	/*  48 */	"signal",
#ifdef SYSVMSG
	/*  49 */	"msgsys",
#else
	/*  49 */	"#49 (unimplemented msgsys)",
#endif
	/*  50 */	"sysarch",
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
	/*  56 */	"#56 (unimplemented exch)",
	/*  57 */	"utssys",
	/*  58 */	"fsync",
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
	/*  75 */	"#75 (obsolete sigret)",
	/*  76 */	"#76 (obsolete rdebug)",
	/*  77 */	"#77 (obsolete rfstop)",
	/*  78 */	"#78 (unimplemented rfsys)",
	/*  79 */	"rmdir",
	/*  80 */	"mkdir",
	/*  81 */	"getdents",
	/*  82 */	"#82 (obsolete libattach)",
	/*  83 */	"#83 (obsolete libdetach)",
	/*  84 */	"#84 (unimplemented sysfs)",
	/*  85 */	"getmsg",
	/*  86 */	"putmsg",
	/*  87 */	"poll",
	/*  88 */	"lstat",
	/*  89 */	"symlink",
	/*  90 */	"readlink",
	/*  91 */	"getgroups",
	/*  92 */	"setgroups",
	/*  93 */	"fchmod",
	/*  94 */	"fchown",
	/*  95 */	"sigprocmask",
	/*  96 */	"sigsuspend",
	/*  97 */	"sigaltstack",
	/*  98 */	"sigaction",
	/*  99 */	"sigpending",
	/* 100 */	"context",
	/* 101 */	"#101 (unimplemented evsys)",
	/* 102 */	"#102 (unimplemented evtrapret)",
	/* 103 */	"statvfs",
	/* 104 */	"fstatvfs",
	/* 105 */	"#105 (unimplemented)",
	/* 106 */	"#106 (unimplemented nfssvc)",
	/* 107 */	"waitsys",
	/* 108 */	"#108 (unimplemented sigsendsys)",
	/* 109 */	"hrtsys",
	/* 110 */	"#110 (unimplemented acancel)",
	/* 111 */	"#111 (unimplemented async)",
	/* 112 */	"#112 (unimplemented priocntlsys)",
	/* 113 */	"pathconf",
	/* 114 */	"#114 (unimplemented mincore)",
	/* 115 */	"mmap",
	/* 116 */	"mprotect",
	/* 117 */	"munmap",
	/* 118 */	"fpathconf",
	/* 119 */	"vfork",
	/* 120 */	"fchdir",
	/* 121 */	"readv",
	/* 122 */	"writev",
	/* 123 */	"xstat",
	/* 124 */	"lxstat",
	/* 125 */	"fxstat",
	/* 126 */	"xmknod",
	/* 127 */	"#127 (unimplemented clocal)",
	/* 128 */	"setrlimit",
	/* 129 */	"getrlimit",
	/* 130 */	"lchown",
	/* 131 */	"memcntl",
	/* 132 */	"#132 (unimplemented getpmsg)",
	/* 133 */	"#133 (unimplemented putpmsg)",
	/* 134 */	"rename",
	/* 135 */	"uname",
	/* 136 */	"setegid",
	/* 137 */	"sysconfig",
	/* 138 */	"adjtime",
	/* 139 */	"systeminfo",
	/* 140 */	"#140 (unimplemented)",
	/* 141 */	"seteuid",
	/* 142 */	"#142 (unimplemented vtrace)",
	/* 143 */	"fork1",
	/* 144 */	"#144 (unimplemented sigtimedwait)",
	/* 145 */	"_lwp_info",
	/* 146 */	"#146 (unimplemented yield)",
	/* 147 */	"#147 (unimplemented lwp_sema_wait)",
	/* 148 */	"#148 (unimplemented lwp_sema_post)",
	/* 149 */	"#149 (unimplemented lwp_sema_trywait)",
	/* 150 */	"#150 (unimplemented)",
	/* 151 */	"#151 (unimplemented)",
	/* 152 */	"#152 (unimplemented modctl)",
	/* 153 */	"fchroot",
	/* 154 */	"utimes",
	/* 155 */	"vhangup",
	/* 156 */	"gettimeofday",
	/* 157 */	"getitimer",
	/* 158 */	"setitimer",
	/* 159 */	"_lwp_create",
	/* 160 */	"_lwp_exit",
	/* 161 */	"_lwp_suspend",
	/* 162 */	"_lwp_continue",
	/* 163 */	"_lwp_kill",
	/* 164 */	"_lwp_self",
	/* 165 */	"_lwp_getprivate",
	/* 166 */	"_lwp_setprivate",
	/* 167 */	"_lwp_wait",
	/* 168 */	"#168 (unimplemented lwp_mutex_unlock)",
	/* 169 */	"#169 (unimplemented lwp_mutex_lock)",
	/* 170 */	"#170 (unimplemented lwp_cond_wait)",
	/* 171 */	"#171 (unimplemented lwp_cond_signal)",
	/* 172 */	"#172 (unimplemented lwp_cond_broadcast)",
	/* 173 */	"pread",
	/* 174 */	"pwrite",
	/* 175 */	"llseek",
	/* 176 */	"#176 (unimplemented inst_sync)",
	/* 177 */	"#177 (unimplemented)",
	/* 178 */	"#178 (unimplemented kaio)",
	/* 179 */	"#179 (unimplemented)",
	/* 180 */	"#180 (unimplemented)",
	/* 181 */	"#181 (unimplemented)",
	/* 182 */	"#182 (unimplemented)",
	/* 183 */	"#183 (unimplemented)",
	/* 184 */	"#184 (unimplemented tsolsys)",
	/* 185 */	"acl",
	/* 186 */	"auditsys",
	/* 187 */	"#187 (unimplemented processor_bind)",
	/* 188 */	"#188 (unimplemented processor_info)",
	/* 189 */	"#189 (unimplemented p_online)",
	/* 190 */	"#190 (unimplemented sigqueue)",
	/* 191 */	"#191 (unimplemented clock_gettime)",
	/* 192 */	"#192 (unimplemented clock_settime)",
	/* 193 */	"#193 (unimplemented clock_getres)",
	/* 194 */	"#194 (unimplemented timer_create)",
	/* 195 */	"#195 (unimplemented timer_delete)",
	/* 196 */	"#196 (unimplemented timer_settime)",
	/* 197 */	"#197 (unimplemented timer_gettime)",
	/* 198 */	"#198 (unimplemented timer_getoverrun)",
	/* 199 */	"nanosleep",
	/* 200 */	"facl",
	/* 201 */	"#201 (unimplemented door)",
	/* 202 */	"setreuid",
	/* 203 */	"setregid",
	/* 204 */	"#204 (unimplemented install_utrap)",
	/* 205 */	"#205 (unimplemented signotify)",
	/* 206 */	"schedctl",
	/* 207 */	"#207 (unimplemented pset)",
	/* 208 */	"#208 (unimplemented)",
	/* 209 */	"resolvepath",
	/* 210 */	"#210 (unimplemented signotifywait)",
	/* 211 */	"#211 (unimplemented lwp_sigredirect)",
	/* 212 */	"#212 (unimplemented lwp_alarm)",
	/* 213 */	"getdents64",
	/* 214 */	"mmap64",
	/* 215 */	"stat64",
	/* 216 */	"lstat64",
	/* 217 */	"fstat64",
	/* 218 */	"statvfs64",
	/* 219 */	"fstatvfs64",
	/* 220 */	"setrlimit64",
	/* 221 */	"getrlimit64",
	/* 222 */	"pread64",
	/* 223 */	"pwrite64",
	/* 224 */	"creat64",
	/* 225 */	"open64",
	/* 226 */	"#226 (unimplemented rpcsys)",
	/* 227 */	"#227 (unimplemented)",
	/* 228 */	"#228 (unimplemented)",
	/* 229 */	"#229 (unimplemented)",
	/* 230 */	"socket",
	/* 231 */	"socketpair",
	/* 232 */	"bind",
	/* 233 */	"listen",
	/* 234 */	"accept",
	/* 235 */	"connect",
	/* 236 */	"shutdown",
	/* 237 */	"recv",
	/* 238 */	"recvfrom",
	/* 239 */	"recvmsg",
	/* 240 */	"send",
	/* 241 */	"sendmsg",
	/* 242 */	"sendto",
	/* 243 */	"getpeername",
	/* 244 */	"getsockname",
	/* 245 */	"getsockopt",
	/* 246 */	"setsockopt",
	/* 247 */	"#247 (unimplemented sockconfig)",
	/* 248 */	"#248 (unimplemented sys_ntp_gettime)",
#if defined(NTP) || !defined(_KERNEL)
	/* 249 */	"ntp_adjtime",
#else
	/* 249 */	"#249 (excluded ntp_adjtime)",
#endif
	/* 250 */	"#250 (unimplemented lwp_mutex_unlock)",
	/* 251 */	"#251 (unimplemented lwp_mutex_trylock)",
	/* 252 */	"#252 (unimplemented lwp_mutex_init)",
	/* 253 */	"#253 (unimplemented cladm)",
	/* 254 */	"#254 (unimplemented lwp_sigtimedwait)",
	/* 255 */	"#255 (unimplemented umount2)",
};
