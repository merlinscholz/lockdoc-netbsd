/* $NetBSD: ultrix_syscalls.c,v 1.60 2009/12/14 00:58:38 matt Exp $ */

/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.50 2009/01/17 15:48:06 he Exp
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD: ultrix_syscalls.c,v 1.60 2009/12/14 00:58:38 matt Exp $");

#if defined(_KERNEL_OPT)
#if defined(_KERNEL_OPT)
#include "fs_nfs.h"
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/signal.h>
#include <sys/mount.h>
#include <sys/sched.h>
#include <sys/syscallargs.h>
#include <compat/ultrix/ultrix_syscallargs.h>
#endif /* _KERNEL_OPT */

const char *const ultrix_syscallnames[] = {
	/*   0 */	"syscall",
	/*   1 */	"exit",
	/*   2 */	"fork",
	/*   3 */	"read",
	/*   4 */	"write",
	/*   5 */	"open",
	/*   6 */	"close",
	/*   7 */	"owait",
	/*   8 */	"creat",
	/*   9 */	"link",
	/*  10 */	"unlink",
	/*  11 */	"execv",
	/*  12 */	"chdir",
	/*  13 */	"#13 (obsolete time)",
	/*  14 */	"mknod",
	/*  15 */	"chmod",
	/*  16 */	"__posix_chown",
	/*  17 */	"break",
	/*  18 */	"#18 (obsolete stat)",
	/*  19 */	"lseek",
	/*  20 */	"getpid",
	/*  21 */	"mount",
	/*  22 */	"#22 (obsolete sysV_unmount)",
	/*  23 */	"setuid",
	/*  24 */	"getuid",
	/*  25 */	"#25 (obsolete v7 stime)",
	/*  26 */	"#26 (obsolete v7 ptrace)",
	/*  27 */	"#27 (obsolete v7 alarm)",
	/*  28 */	"#28 (obsolete v7 fstat)",
	/*  29 */	"#29 (obsolete v7 pause)",
	/*  30 */	"#30 (obsolete v7 utime)",
	/*  31 */	"#31 (obsolete v7 stty)",
	/*  32 */	"#32 (obsolete v7 gtty)",
	/*  33 */	"access",
	/*  34 */	"#34 (obsolete v7 nice)",
	/*  35 */	"#35 (obsolete v7 ftime)",
	/*  36 */	"sync",
	/*  37 */	"kill",
	/*  38 */	"stat43",
	/*  39 */	"#39 (obsolete v7 setpgrp)",
	/*  40 */	"lstat43",
	/*  41 */	"dup",
	/*  42 */	"pipe",
	/*  43 */	"#43 (obsolete v7 times)",
	/*  44 */	"profil",
	/*  45 */	"#45 (unimplemented)",
	/*  46 */	"#46 (obsolete v7 setgid)",
	/*  47 */	"getgid",
	/*  48 */	"#48 (unimplemented ssig)",
	/*  49 */	"#49 (unimplemented reserved for USG)",
	/*  50 */	"#50 (unimplemented reserved for USG)",
	/*  51 */	"acct",
	/*  52 */	"#52 (unimplemented)",
	/*  53 */	"#53 (unimplemented syslock)",
	/*  54 */	"ioctl",
	/*  55 */	"reboot",
	/*  56 */	"#56 (unimplemented v7 mpxchan)",
	/*  57 */	"symlink",
	/*  58 */	"readlink",
	/*  59 */	"execve",
	/*  60 */	"umask",
	/*  61 */	"chroot",
	/*  62 */	"fstat",
	/*  63 */	"#63 (unimplemented)",
	/*  64 */	"getpagesize",
	/*  65 */	"#65 (unimplemented mremap)",
	/*  66 */	"vfork",
	/*  67 */	"#67 (obsolete vread)",
	/*  68 */	"#68 (obsolete vwrite)",
	/*  69 */	"sbrk",
	/*  70 */	"sstk",
	/*  71 */	"mmap",
	/*  72 */	"vadvise",
	/*  73 */	"munmap",
	/*  74 */	"mprotect",
	/*  75 */	"madvise",
	/*  76 */	"vhangup",
	/*  77 */	"#77 (unimplemented old vlimit)",
	/*  78 */	"mincore",
	/*  79 */	"getgroups",
	/*  80 */	"setgroups",
	/*  81 */	"getpgrp",
	/*  82 */	"setpgrp",
	/*  83 */	"__setitimer50",
	/*  84 */	"wait3",
	/*  85 */	"swapon",
	/*  86 */	"__getitimer50",
	/*  87 */	"gethostname",
	/*  88 */	"sethostname",
	/*  89 */	"getdtablesize",
	/*  90 */	"dup2",
	/*  91 */	"#91 (unimplemented getdopt)",
	/*  92 */	"fcntl",
	/*  93 */	"select",
	/*  94 */	"#94 (unimplemented setdopt)",
	/*  95 */	"fsync",
	/*  96 */	"setpriority",
	/*  97 */	"socket",
	/*  98 */	"connect",
	/*  99 */	"accept",
	/* 100 */	"getpriority",
	/* 101 */	"send",
	/* 102 */	"recv",
	/* 103 */	"sigreturn",
	/* 104 */	"bind",
	/* 105 */	"setsockopt",
	/* 106 */	"listen",
	/* 107 */	"#107 (unimplemented vtimes)",
	/* 108 */	"sigvec",
	/* 109 */	"sigblock",
	/* 110 */	"sigsetmask",
	/* 111 */	"sigsuspend",
	/* 112 */	"sigstack",
	/* 113 */	"recvmsg",
	/* 114 */	"sendmsg",
	/* 115 */	"#115 (obsolete vtrace)",
	/* 116 */	"__gettimeofday50",
	/* 117 */	"__getrusage50",
	/* 118 */	"getsockopt",
	/* 119 */	"#119 (unimplemented resuba)",
	/* 120 */	"readv",
	/* 121 */	"writev",
	/* 122 */	"__settimeofday50",
	/* 123 */	"__posix_fchown",
	/* 124 */	"fchmod",
	/* 125 */	"recvfrom",
	/* 126 */	"setreuid",
	/* 127 */	"setregid",
	/* 128 */	"rename",
	/* 129 */	"truncate",
	/* 130 */	"ftruncate",
	/* 131 */	"flock",
	/* 132 */	"#132 (unimplemented)",
	/* 133 */	"sendto",
	/* 134 */	"shutdown",
	/* 135 */	"socketpair",
	/* 136 */	"mkdir",
	/* 137 */	"rmdir",
	/* 138 */	"__utimes50",
	/* 139 */	"sigcleanup",
	/* 140 */	"__adjtime50",
	/* 141 */	"getpeername",
	/* 142 */	"gethostid",
	/* 143 */	"#143 (unimplemented old sethostid)",
	/* 144 */	"getrlimit",
	/* 145 */	"setrlimit",
	/* 146 */	"killpg",
	/* 147 */	"#147 (unimplemented)",
	/* 148 */	"#148 (unimplemented setquota)",
	/* 149 */	"#149 (unimplemented quota / * needs to be nullop to boot on Ultrix root partition * /)",
	/* 150 */	"getsockname",
	/* 151 */	"#151 (unimplemented sysmips / * 4 args * /)",
#ifdef __mips
	/* 152 */	"cacheflush",
	/* 153 */	"cachectl",
#else	/* !mips */
	/* 152 */	"#152 (unimplemented)",
	/* 153 */	"#153 (unimplemented)",
#endif	/* !mips */
	/* 154 */	"#154 (unimplemented)",
	/* 155 */	"#155 (unimplemented atomic_op)",
	/* 156 */	"#156 (unimplemented)",
	/* 157 */	"#157 (unimplemented)",
	/* 158 */	"nfssvc",
	/* 159 */	"getdirentries",
	/* 160 */	"statfs",
	/* 161 */	"fstatfs",
	/* 162 */	"#162 (unimplemented umount)",
#ifdef NFS
	/* 163 */	"async_daemon",
	/* 164 */	"getfh",
#else
	/* 163 */	"#163 (unimplemented async_daemon)",
	/* 164 */	"#164 (unimplemented getfh)",
#endif
	/* 165 */	"getdomainname",
	/* 166 */	"setdomainname",
	/* 167 */	"#167 (unimplemented)",
	/* 168 */	"quotactl",
	/* 169 */	"exportfs",
	/* 170 */	"#170 (unimplemented ultrix_sys_mount)",
	/* 171 */	"#171 (unimplemented 4 hdwconf)",
	/* 172 */	"#172 (unimplemented msgctl)",
	/* 173 */	"#173 (unimplemented msgget)",
	/* 174 */	"#174 (unimplemented msgrcv)",
	/* 175 */	"#175 (unimplemented msgsnd)",
	/* 176 */	"#176 (unimplemented semctl)",
	/* 177 */	"#177 (unimplemented semget)",
	/* 178 */	"#178 (unimplemented semop)",
	/* 179 */	"uname",
	/* 180 */	"shmsys",
	/* 181 */	"#181 (unimplemented 0 plock)",
	/* 182 */	"#182 (unimplemented 0 lockf)",
	/* 183 */	"ustat",
	/* 184 */	"getmnt",
	/* 185 */	"#185 (unimplemented notdef)",
	/* 186 */	"#186 (unimplemented notdef)",
	/* 187 */	"sigpending",
	/* 188 */	"setsid",
	/* 189 */	"waitpid",
	/* 190 */	"#190 (unimplemented)",
	/* 191 */	"#191 (unimplemented)",
	/* 192 */	"#192 (unimplemented)",
	/* 193 */	"#193 (unimplemented)",
	/* 194 */	"#194 (unimplemented)",
	/* 195 */	"#195 (unimplemented)",
	/* 196 */	"#196 (unimplemented)",
	/* 197 */	"#197 (unimplemented)",
	/* 198 */	"#198 (unimplemented)",
	/* 199 */	"#199 (unimplemented)",
	/* 200 */	"#200 (unimplemented)",
	/* 201 */	"#201 (unimplemented)",
	/* 202 */	"#202 (unimplemented)",
	/* 203 */	"#203 (unimplemented)",
	/* 204 */	"#204 (unimplemented)",
	/* 205 */	"#205 (unimplemented)",
	/* 206 */	"#206 (unimplemented)",
	/* 207 */	"#207 (unimplemented)",
	/* 208 */	"#208 (unimplemented)",
	/* 209 */	"#209 (unimplemented)",
	/* 210 */	"#210 (unimplemented)",
	/* 211 */	"#211 (unimplemented)",
	/* 212 */	"#212 (unimplemented)",
	/* 213 */	"#213 (unimplemented)",
	/* 214 */	"#214 (unimplemented)",
	/* 215 */	"#215 (unimplemented)",
	/* 216 */	"#216 (unimplemented)",
	/* 217 */	"#217 (unimplemented)",
	/* 218 */	"#218 (unimplemented)",
	/* 219 */	"#219 (unimplemented)",
	/* 220 */	"#220 (unimplemented)",
	/* 221 */	"#221 (unimplemented)",
	/* 222 */	"#222 (unimplemented)",
	/* 223 */	"#223 (unimplemented)",
	/* 224 */	"#224 (unimplemented)",
	/* 225 */	"#225 (unimplemented)",
	/* 226 */	"#226 (unimplemented)",
	/* 227 */	"#227 (unimplemented)",
	/* 228 */	"#228 (unimplemented)",
	/* 229 */	"#229 (unimplemented)",
	/* 230 */	"#230 (unimplemented)",
	/* 231 */	"#231 (unimplemented)",
	/* 232 */	"#232 (unimplemented)",
	/* 233 */	"#233 (unimplemented 1 utc_gettime)",
	/* 234 */	"#234 (unimplemented 2 utc_adjtime)",
	/* 235 */	"#235 (unimplemented)",
	/* 236 */	"#236 (unimplemented)",
	/* 237 */	"#237 (unimplemented)",
	/* 238 */	"#238 (unimplemented)",
	/* 239 */	"#239 (unimplemented)",
	/* 240 */	"#240 (unimplemented)",
	/* 241 */	"#241 (unimplemented)",
	/* 242 */	"#242 (unimplemented)",
	/* 243 */	"#243 (unimplemented)",
	/* 244 */	"#244 (unimplemented)",
	/* 245 */	"#245 (unimplemented)",
	/* 246 */	"#246 (unimplemented)",
	/* 247 */	"#247 (unimplemented)",
	/* 248 */	"#248 (unimplemented)",
	/* 249 */	"#249 (unimplemented)",
	/* 250 */	"#250 (unimplemented)",
	/* 251 */	"#251 (unimplemented)",
	/* 252 */	"#252 (unimplemented audctl / * Make no-op for installation on Ultrix rootpartition? * /)",
	/* 253 */	"#253 (unimplemented audgen / * Make no-op for installation on Ultrix rootpartition? * /)",
	/* 254 */	"#254 (unimplemented startcpu)",
	/* 255 */	"#255 (unimplemented stopcpu)",
	/* 256 */	"getsysinfo",
	/* 257 */	"setsysinfo",
};
