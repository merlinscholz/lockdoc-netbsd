/*	$NetBSD: syscall.h,v 1.93 1998/11/26 16:13:56 kleink Exp $	*/

/*
 * System call numbers.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * created from	NetBSD: syscalls.master,v 1.86 1998/11/26 16:07:05 kleink Exp 
 */

/* syscall: "syscall" ret: "int" args: "int" "..." */
#define	SYS_syscall	0

/* syscall: "exit" ret: "void" args: "int" */
#define	SYS_exit	1

/* syscall: "fork" ret: "int" args: */
#define	SYS_fork	2

/* syscall: "read" ret: "ssize_t" args: "int" "void *" "size_t" */
#define	SYS_read	3

/* syscall: "write" ret: "ssize_t" args: "int" "const void *" "size_t" */
#define	SYS_write	4

/* syscall: "open" ret: "int" args: "const char *" "int" "..." */
#define	SYS_open	5

/* syscall: "close" ret: "int" args: "int" */
#define	SYS_close	6

/* syscall: "wait4" ret: "int" args: "int" "int *" "int" "struct rusage *" */
#define	SYS_wait4	7

#define	SYS_compat_43_ocreat	8

/* syscall: "link" ret: "int" args: "const char *" "const char *" */
#define	SYS_link	9

/* syscall: "unlink" ret: "int" args: "const char *" */
#define	SYS_unlink	10

				/* 11 is obsolete execv */
/* syscall: "chdir" ret: "int" args: "const char *" */
#define	SYS_chdir	12

/* syscall: "fchdir" ret: "int" args: "int" */
#define	SYS_fchdir	13

/* syscall: "mknod" ret: "int" args: "const char *" "mode_t" "dev_t" */
#define	SYS_mknod	14

/* syscall: "chmod" ret: "int" args: "const char *" "mode_t" */
#define	SYS_chmod	15

/* syscall: "chown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	SYS_chown	16

/* syscall: "break" ret: "int" args: "char *" */
#define	SYS_break	17

/* syscall: "getfsstat" ret: "int" args: "struct statfs *" "long" "int" */
#define	SYS_getfsstat	18

#define	SYS_compat_43_olseek	19

/* syscall: "getpid" ret: "pid_t" args: */
#define	SYS_getpid	20

/* syscall: "mount" ret: "int" args: "const char *" "const char *" "int" "void *" */
#define	SYS_mount	21

/* syscall: "unmount" ret: "int" args: "const char *" "int" */
#define	SYS_unmount	22

/* syscall: "setuid" ret: "int" args: "uid_t" */
#define	SYS_setuid	23

/* syscall: "getuid" ret: "uid_t" args: */
#define	SYS_getuid	24

/* syscall: "geteuid" ret: "uid_t" args: */
#define	SYS_geteuid	25

/* syscall: "ptrace" ret: "int" args: "int" "pid_t" "caddr_t" "int" */
#define	SYS_ptrace	26

/* syscall: "recvmsg" ret: "ssize_t" args: "int" "struct msghdr *" "int" */
#define	SYS_recvmsg	27

/* syscall: "sendmsg" ret: "ssize_t" args: "int" "const struct msghdr *" "int" */
#define	SYS_sendmsg	28

/* syscall: "recvfrom" ret: "ssize_t" args: "int" "void *" "size_t" "int" "struct sockaddr *" "int *" */
#define	SYS_recvfrom	29

/* syscall: "accept" ret: "int" args: "int" "struct sockaddr *" "int *" */
#define	SYS_accept	30

/* syscall: "getpeername" ret: "int" args: "int" "struct sockaddr *" "int *" */
#define	SYS_getpeername	31

/* syscall: "getsockname" ret: "int" args: "int" "struct sockaddr *" "int *" */
#define	SYS_getsockname	32

/* syscall: "access" ret: "int" args: "const char *" "int" */
#define	SYS_access	33

/* syscall: "chflags" ret: "int" args: "const char *" "u_long" */
#define	SYS_chflags	34

/* syscall: "fchflags" ret: "int" args: "int" "u_long" */
#define	SYS_fchflags	35

/* syscall: "sync" ret: "void" args: */
#define	SYS_sync	36

/* syscall: "kill" ret: "int" args: "int" "int" */
#define	SYS_kill	37

#define	SYS_compat_43_stat43	38

/* syscall: "getppid" ret: "pid_t" args: */
#define	SYS_getppid	39

#define	SYS_compat_43_lstat43	40

/* syscall: "dup" ret: "int" args: "int" */
#define	SYS_dup	41

/* syscall: "pipe" ret: "int" args: */
#define	SYS_pipe	42

/* syscall: "getegid" ret: "gid_t" args: */
#define	SYS_getegid	43

/* syscall: "profil" ret: "int" args: "caddr_t" "size_t" "u_long" "u_int" */
#define	SYS_profil	44

/* syscall: "ktrace" ret: "int" args: "const char *" "int" "int" "int" */
#define	SYS_ktrace	45

#define	SYS_compat_13_sigaction13	46

/* syscall: "getgid" ret: "gid_t" args: */
#define	SYS_getgid	47

#define	SYS_compat_13_sigprocmask13	48

/* syscall: "__getlogin" ret: "int" args: "char *" "u_int" */
#define	SYS___getlogin	49

/* syscall: "setlogin" ret: "int" args: "const char *" */
#define	SYS_setlogin	50

/* syscall: "acct" ret: "int" args: "const char *" */
#define	SYS_acct	51

#define	SYS_compat_13_sigpending13	52

#define	SYS_compat_13_sigaltstack13	53

/* syscall: "ioctl" ret: "int" args: "int" "u_long" "..." */
#define	SYS_ioctl	54

#define	SYS_compat_12_oreboot	55

/* syscall: "revoke" ret: "int" args: "const char *" */
#define	SYS_revoke	56

/* syscall: "symlink" ret: "int" args: "const char *" "const char *" */
#define	SYS_symlink	57

/* syscall: "readlink" ret: "int" args: "const char *" "char *" "size_t" */
#define	SYS_readlink	58

/* syscall: "execve" ret: "int" args: "const char *" "char *const *" "char *const *" */
#define	SYS_execve	59

/* syscall: "umask" ret: "mode_t" args: "mode_t" */
#define	SYS_umask	60

/* syscall: "chroot" ret: "int" args: "const char *" */
#define	SYS_chroot	61

#define	SYS_compat_43_fstat43	62

#define	SYS_compat_43_ogetkerninfo	63

#define	SYS_compat_43_ogetpagesize	64

#define	SYS_compat_12_msync	65

/* syscall: "vfork" ret: "int" args: */
#define	SYS_vfork	66

				/* 67 is obsolete vread */
				/* 68 is obsolete vwrite */
/* syscall: "sbrk" ret: "int" args: "int" */
#define	SYS_sbrk	69

/* syscall: "sstk" ret: "int" args: "int" */
#define	SYS_sstk	70

#define	SYS_compat_43_ommap	71

/* syscall: "vadvise" ret: "int" args: "int" */
#define	SYS_vadvise	72

/* syscall: "munmap" ret: "int" args: "void *" "size_t" */
#define	SYS_munmap	73

/* syscall: "mprotect" ret: "int" args: "void *" "size_t" "int" */
#define	SYS_mprotect	74

/* syscall: "madvise" ret: "int" args: "void *" "size_t" "int" */
#define	SYS_madvise	75

				/* 76 is obsolete vhangup */
				/* 77 is obsolete vlimit */
/* syscall: "mincore" ret: "int" args: "caddr_t" "size_t" "char *" */
#define	SYS_mincore	78

/* syscall: "getgroups" ret: "int" args: "int" "gid_t *" */
#define	SYS_getgroups	79

/* syscall: "setgroups" ret: "int" args: "int" "const gid_t *" */
#define	SYS_setgroups	80

/* syscall: "getpgrp" ret: "int" args: */
#define	SYS_getpgrp	81

/* syscall: "setpgid" ret: "int" args: "int" "int" */
#define	SYS_setpgid	82

/* syscall: "setitimer" ret: "int" args: "int" "const struct itimerval *" "struct itimerval *" */
#define	SYS_setitimer	83

#define	SYS_compat_43_owait	84

#define	SYS_compat_12_oswapon	85

/* syscall: "getitimer" ret: "int" args: "int" "struct itimerval *" */
#define	SYS_getitimer	86

#define	SYS_compat_43_ogethostname	87

#define	SYS_compat_43_osethostname	88

#define	SYS_compat_43_ogetdtablesize	89

/* syscall: "dup2" ret: "int" args: "int" "int" */
#define	SYS_dup2	90

/* syscall: "fcntl" ret: "int" args: "int" "int" "..." */
#define	SYS_fcntl	92

/* syscall: "select" ret: "int" args: "int" "fd_set *" "fd_set *" "fd_set *" "struct timeval *" */
#define	SYS_select	93

/* syscall: "fsync" ret: "int" args: "int" */
#define	SYS_fsync	95

/* syscall: "setpriority" ret: "int" args: "int" "int" "int" */
#define	SYS_setpriority	96

/* syscall: "socket" ret: "int" args: "int" "int" "int" */
#define	SYS_socket	97

/* syscall: "connect" ret: "int" args: "int" "const struct sockaddr *" "int" */
#define	SYS_connect	98

#define	SYS_compat_43_oaccept	99

/* syscall: "getpriority" ret: "int" args: "int" "int" */
#define	SYS_getpriority	100

#define	SYS_compat_43_osend	101

#define	SYS_compat_43_orecv	102

#define	SYS_compat_13_sigreturn13	103

/* syscall: "bind" ret: "int" args: "int" "const struct sockaddr *" "int" */
#define	SYS_bind	104

/* syscall: "setsockopt" ret: "int" args: "int" "int" "int" "const void *" "int" */
#define	SYS_setsockopt	105

/* syscall: "listen" ret: "int" args: "int" "int" */
#define	SYS_listen	106

				/* 107 is obsolete vtimes */
#define	SYS_compat_43_osigvec	108

#define	SYS_compat_43_osigblock	109

#define	SYS_compat_43_osigsetmask	110

#define	SYS_compat_13_sigsuspend13	111

#define	SYS_compat_43_osigstack	112

#define	SYS_compat_43_orecvmsg	113

#define	SYS_compat_43_osendmsg	114

/* syscall: "vtrace" ret: "int" args: "int" "int" */
#define	SYS_vtrace	115

				/* 115 is obsolete vtrace */
/* syscall: "gettimeofday" ret: "int" args: "struct timeval *" "struct timezone *" */
#define	SYS_gettimeofday	116

/* syscall: "getrusage" ret: "int" args: "int" "struct rusage *" */
#define	SYS_getrusage	117

/* syscall: "getsockopt" ret: "int" args: "int" "int" "int" "void *" "int *" */
#define	SYS_getsockopt	118

				/* 119 is obsolete resuba */
/* syscall: "readv" ret: "ssize_t" args: "int" "const struct iovec *" "int" */
#define	SYS_readv	120

/* syscall: "writev" ret: "ssize_t" args: "int" "const struct iovec *" "int" */
#define	SYS_writev	121

/* syscall: "settimeofday" ret: "int" args: "const struct timeval *" "const struct timezone *" */
#define	SYS_settimeofday	122

/* syscall: "fchown" ret: "int" args: "int" "uid_t" "gid_t" */
#define	SYS_fchown	123

/* syscall: "fchmod" ret: "int" args: "int" "mode_t" */
#define	SYS_fchmod	124

#define	SYS_compat_43_orecvfrom	125

/* syscall: "setreuid" ret: "int" args: "uid_t" "uid_t" */
#define	SYS_setreuid	126

/* syscall: "setregid" ret: "int" args: "gid_t" "gid_t" */
#define	SYS_setregid	127

/* syscall: "rename" ret: "int" args: "const char *" "const char *" */
#define	SYS_rename	128

#define	SYS_compat_43_otruncate	129

#define	SYS_compat_43_oftruncate	130

/* syscall: "flock" ret: "int" args: "int" "int" */
#define	SYS_flock	131

/* syscall: "mkfifo" ret: "int" args: "const char *" "mode_t" */
#define	SYS_mkfifo	132

/* syscall: "sendto" ret: "ssize_t" args: "int" "const void *" "size_t" "int" "const struct sockaddr *" "int" */
#define	SYS_sendto	133

/* syscall: "shutdown" ret: "int" args: "int" "int" */
#define	SYS_shutdown	134

/* syscall: "socketpair" ret: "int" args: "int" "int" "int" "int *" */
#define	SYS_socketpair	135

/* syscall: "mkdir" ret: "int" args: "const char *" "mode_t" */
#define	SYS_mkdir	136

/* syscall: "rmdir" ret: "int" args: "const char *" */
#define	SYS_rmdir	137

/* syscall: "utimes" ret: "int" args: "const char *" "const struct timeval *" */
#define	SYS_utimes	138

				/* 139 is obsolete 4.2 sigreturn */
/* syscall: "adjtime" ret: "int" args: "const struct timeval *" "struct timeval *" */
#define	SYS_adjtime	140

#define	SYS_compat_43_ogetpeername	141

#define	SYS_compat_43_ogethostid	142

#define	SYS_compat_43_osethostid	143

#define	SYS_compat_43_ogetrlimit	144

#define	SYS_compat_43_osetrlimit	145

#define	SYS_compat_43_okillpg	146

/* syscall: "setsid" ret: "int" args: */
#define	SYS_setsid	147

/* syscall: "quotactl" ret: "int" args: "const char *" "int" "int" "caddr_t" */
#define	SYS_quotactl	148

#define	SYS_compat_43_oquota	149

#define	SYS_compat_43_ogetsockname	150

/* syscall: "nfssvc" ret: "int" args: "int" "void *" */
#define	SYS_nfssvc	155

#define	SYS_compat_43_ogetdirentries	156

/* syscall: "statfs" ret: "int" args: "const char *" "struct statfs *" */
#define	SYS_statfs	157

/* syscall: "fstatfs" ret: "int" args: "int" "struct statfs *" */
#define	SYS_fstatfs	158

/* syscall: "getfh" ret: "int" args: "const char *" "fhandle_t *" */
#define	SYS_getfh	161

#define	SYS_compat_09_ogetdomainname	162

#define	SYS_compat_09_osetdomainname	163

#define	SYS_compat_09_ouname	164

/* syscall: "sysarch" ret: "int" args: "int" "void *" */
#define	SYS_sysarch	165

#define	SYS_compat_10_osemsys	169

#define	SYS_compat_10_omsgsys	170

#define	SYS_compat_10_oshmsys	171

/* syscall: "pread" ret: "ssize_t" args: "int" "void *" "size_t" "int" "off_t" */
#define	SYS_pread	173

/* syscall: "pwrite" ret: "ssize_t" args: "int" "const void *" "size_t" "int" "off_t" */
#define	SYS_pwrite	174

/* syscall: "ntp_gettime" ret: "int" args: "struct ntptimeval *" */
#define	SYS_ntp_gettime	175

/* syscall: "ntp_adjtime" ret: "int" args: "struct timex *" */
#define	SYS_ntp_adjtime	176

/* syscall: "setgid" ret: "int" args: "gid_t" */
#define	SYS_setgid	181

/* syscall: "setegid" ret: "int" args: "gid_t" */
#define	SYS_setegid	182

/* syscall: "seteuid" ret: "int" args: "uid_t" */
#define	SYS_seteuid	183

/* syscall: "lfs_bmapv" ret: "int" args: "fsid_t *" "struct block_info *" "int" */
#define	SYS_lfs_bmapv	184

/* syscall: "lfs_markv" ret: "int" args: "fsid_t *" "struct block_info *" "int" */
#define	SYS_lfs_markv	185

/* syscall: "lfs_segclean" ret: "int" args: "fsid_t *" "u_long" */
#define	SYS_lfs_segclean	186

/* syscall: "lfs_segwait" ret: "int" args: "fsid_t *" "struct timeval *" */
#define	SYS_lfs_segwait	187

#define	SYS_compat_12_stat12	188

#define	SYS_compat_12_fstat12	189

#define	SYS_compat_12_lstat12	190

/* syscall: "pathconf" ret: "long" args: "const char *" "int" */
#define	SYS_pathconf	191

/* syscall: "fpathconf" ret: "long" args: "int" "int" */
#define	SYS_fpathconf	192

/* syscall: "getrlimit" ret: "int" args: "int" "struct rlimit *" */
#define	SYS_getrlimit	194

/* syscall: "setrlimit" ret: "int" args: "int" "const struct rlimit *" */
#define	SYS_setrlimit	195

#define	SYS_compat_12_getdirentries	196

/* syscall: "mmap" ret: "void *" args: "void *" "size_t" "int" "int" "int" "long" "off_t" */
#define	SYS_mmap	197

/* syscall: "__syscall" ret: "quad_t" args: "quad_t" "..." */
#define	SYS___syscall	198

/* syscall: "lseek" ret: "off_t" args: "int" "int" "off_t" "int" */
#define	SYS_lseek	199

/* syscall: "truncate" ret: "int" args: "const char *" "int" "off_t" */
#define	SYS_truncate	200

/* syscall: "ftruncate" ret: "int" args: "int" "int" "off_t" */
#define	SYS_ftruncate	201

/* syscall: "__sysctl" ret: "int" args: "int *" "u_int" "void *" "size_t *" "void *" "size_t" */
#define	SYS___sysctl	202

/* syscall: "mlock" ret: "int" args: "const void *" "size_t" */
#define	SYS_mlock	203

/* syscall: "munlock" ret: "int" args: "const void *" "size_t" */
#define	SYS_munlock	204

/* syscall: "undelete" ret: "int" args: "const char *" */
#define	SYS_undelete	205

/* syscall: "futimes" ret: "int" args: "int" "const struct timeval *" */
#define	SYS_futimes	206

/* syscall: "getpgid" ret: "pid_t" args: "pid_t" */
#define	SYS_getpgid	207

/* syscall: "reboot" ret: "int" args: "int" "char *" */
#define	SYS_reboot	208

/* syscall: "poll" ret: "int" args: "struct pollfd *" "u_int" "int" */
#define	SYS_poll	209

/* syscall: "__semctl" ret: "int" args: "int" "int" "int" "union semun *" */
#define	SYS___semctl	220

/* syscall: "semget" ret: "int" args: "key_t" "int" "int" */
#define	SYS_semget	221

/* syscall: "semop" ret: "int" args: "int" "struct sembuf *" "size_t" */
#define	SYS_semop	222

/* syscall: "semconfig" ret: "int" args: "int" */
#define	SYS_semconfig	223

/* syscall: "msgctl" ret: "int" args: "int" "int" "struct msqid_ds *" */
#define	SYS_msgctl	224

/* syscall: "msgget" ret: "int" args: "key_t" "int" */
#define	SYS_msgget	225

/* syscall: "msgsnd" ret: "int" args: "int" "const void *" "size_t" "int" */
#define	SYS_msgsnd	226

/* syscall: "msgrcv" ret: "ssize_t" args: "int" "void *" "size_t" "long" "int" */
#define	SYS_msgrcv	227

/* syscall: "shmat" ret: "void *" args: "int" "const void *" "int" */
#define	SYS_shmat	228

/* syscall: "shmctl" ret: "int" args: "int" "int" "struct shmid_ds *" */
#define	SYS_shmctl	229

/* syscall: "shmdt" ret: "int" args: "const void *" */
#define	SYS_shmdt	230

/* syscall: "shmget" ret: "int" args: "key_t" "size_t" "int" */
#define	SYS_shmget	231

/* syscall: "clock_gettime" ret: "int" args: "clockid_t" "struct timespec *" */
#define	SYS_clock_gettime	232

/* syscall: "clock_settime" ret: "int" args: "clockid_t" "const struct timespec *" */
#define	SYS_clock_settime	233

/* syscall: "clock_getres" ret: "int" args: "clockid_t" "struct timespec *" */
#define	SYS_clock_getres	234

/* syscall: "nanosleep" ret: "int" args: "const struct timespec *" "struct timespec *" */
#define	SYS_nanosleep	240

/* syscall: "fdatasync" ret: "int" args: "int" */
#define	SYS_fdatasync	241

/* syscall: "__posix_rename" ret: "int" args: "const char *" "const char *" */
#define	SYS___posix_rename	270

/* syscall: "swapctl" ret: "int" args: "int" "const void *" "int" */
#define	SYS_swapctl	271

/* syscall: "getdents" ret: "int" args: "int" "char *" "size_t" */
#define	SYS_getdents	272

/* syscall: "minherit" ret: "int" args: "void *" "size_t" "int" */
#define	SYS_minherit	273

/* syscall: "lchmod" ret: "int" args: "const char *" "mode_t" */
#define	SYS_lchmod	274

/* syscall: "lchown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	SYS_lchown	275

/* syscall: "lutimes" ret: "int" args: "const char *" "const struct timeval *" */
#define	SYS_lutimes	276

/* syscall: "__msync13" ret: "int" args: "void *" "size_t" "int" */
#define	SYS___msync13	277

/* syscall: "__stat13" ret: "int" args: "const char *" "struct stat *" */
#define	SYS___stat13	278

/* syscall: "__fstat13" ret: "int" args: "int" "struct stat *" */
#define	SYS___fstat13	279

/* syscall: "__lstat13" ret: "int" args: "const char *" "struct stat *" */
#define	SYS___lstat13	280

/* syscall: "__sigaltstack14" ret: "int" args: "const struct sigaltstack *" "struct sigaltstack *" */
#define	SYS___sigaltstack14	281

/* syscall: "__vfork14" ret: "int" args: */
#define	SYS___vfork14	282

/* syscall: "__posix_chown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	SYS___posix_chown	283

/* syscall: "__posix_fchown" ret: "int" args: "int" "uid_t" "gid_t" */
#define	SYS___posix_fchown	284

/* syscall: "__posix_lchown" ret: "int" args: "const char *" "uid_t" "gid_t" */
#define	SYS___posix_lchown	285

/* syscall: "getsid" ret: "pid_t" args: "pid_t" */
#define	SYS_getsid	286

/* syscall: "fktrace" ret: "int" args: "const int" "int" "int" "int" */
#define	SYS_fktrace	288

/* syscall: "preadv" ret: "ssize_t" args: "int" "const struct iovec *" "int" "int" "off_t" */
#define	SYS_preadv	289

/* syscall: "pwritev" ret: "ssize_t" args: "int" "const struct iovec *" "int" "int" "off_t" */
#define	SYS_pwritev	290

/* syscall: "__sigaction14" ret: "int" args: "int" "const struct sigaction *" "struct sigaction *" */
#define	SYS___sigaction14	291

/* syscall: "__sigpending14" ret: "int" args: "sigset_t *" */
#define	SYS___sigpending14	292

/* syscall: "__sigprocmask14" ret: "int" args: "int" "const sigset_t *" "sigset_t *" */
#define	SYS___sigprocmask14	293

/* syscall: "__sigsuspend14" ret: "int" args: "const sigset_t *" */
#define	SYS___sigsuspend14	294

/* syscall: "__sigreturn14" ret: "int" args: "struct sigcontext *" */
#define	SYS___sigreturn14	295

#define	SYS_MAXSYSCALL	296
