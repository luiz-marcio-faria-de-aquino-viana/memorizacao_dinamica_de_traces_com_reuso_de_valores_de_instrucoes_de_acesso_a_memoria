
/*
 * syscall.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/25/99
 */


#ifndef __SYSCALL_H
#define __SYSCALL_H 275

#define	SYSCALL_exit	1
#define	SYSCALL_fork	2
#define	SYSCALL_read	3
#define	SYSCALL_write	4
#define	SYSCALL_open	5
#define	SYSCALL_close	6
#define	SYSCALL_wait4	7
#define	SYSCALL_creat	8
#define	SYSCALL_link	9
#define	SYSCALL_unlink	10
#define	SYSCALL_execv	11
#define	SYSCALL_chdir	12
				/* 13 is old: time */
#define	SYSCALL_mknod	14
#define	SYSCALL_chmod	15
#define	SYSCALL_chown	16
#define SYSCALL_brk		17	/* 17 is old: sbreak */
				/* 18 is old: stat */
#define	SYSCALL_lseek	19
#define	SYSCALL_getpid	20
				/* 21 is old: mount */
				/* 22 is old: umount */
				/* 23 is old: setuid */
#define	SYSCALL_getuid	24
				/* 25 is old: stime */
#define	SYSCALL_ptrace	26
				/* 27 is old: alarm */
				/* 28 is old: fstat */
				/* 29 is old: pause */
				/* 30 is old: utime */
				/* 31 is old: stty */
				/* 32 is old: gtty */
#define	SYSCALL_access	33
				/* 34 is old: nice */
				/* 35 is old: ftime */
#define	SYSCALL_sync	36
#define	SYSCALL_kill	37
#define	SYSCALL_stat	38
				/* 39 is old: setpgrp */
#define	SYSCALL_lstat	40
#define	SYSCALL_dup		41
#define	SYSCALL_pipe	42
				/* 43 is old: times */
#define	SYSCALL_profil	44
				/* 45 is unused */
				/* 46 is old: setgid */
#define	SYSCALL_getgid	47
				/* 48 is old: sigsys */
				/* 49 is unused */
				/* 50 is unused */
#define	SYSCALL_acct	51
				/* 52 is old: phys */
#define	SYSCALL_mctl	53
#define	SYSCALL_ioctl	54
#define	SYSCALL_reboot	55
				/* 56 is old: mpxchan */
#define	SYSCALL_symlink	57
#define	SYSCALL_readlink	58
#define	SYSCALL_execve	59
#define	SYSCALL_umask	60
#define	SYSCALL_chroot	61
#define	SYSCALL_fstat	62
				/* 63 is unused */
#define	SYSCALL_getpagesize 64
#define	SYSCALL_msync	65
				/* 66 is old: vfork */
				/* 67 is old: vread */
				/* 68 is old: vwrite */
#define	SYSCALL_sbrk	69
#define	SYSCALL_sstk	70
#define	SYSCALL_mmap	71
#define	SYSCALL_vadvise	72
#define	SYSCALL_munmap	73
#define	SYSCALL_mprotect	74
#define	SYSCALL_madvise	75
#define	SYSCALL_vhangup	76
				/* 77 is old: vlimit */
#define	SYSCALL_mincore	78
#define	SYSCALL_getgroups	79
#define	SYSCALL_setgroups	80
#define	SYSCALL_getpgrp	81
#define	SYSCALL_setpgrp	82
#define	SYSCALL_setitimer	83
				/* 84 is old: wait & wait3 */
#define	SYSCALL_swapon	85
#define	SYSCALL_getitimer	86
#define	SYSCALL_gethostname	87
#define	SYSCALL_sethostname	88
#define	SYSCALL_getdtablesize 89
#define	SYSCALL_dup2	90
#define	SYSCALL_getdopt	91
#define	SYSCALL_fcntl	92
#define	SYSCALL_select	93
#define	SYSCALL_setdopt	94
#define	SYSCALL_fsync	95
#define	SYSCALL_setpriority	96
#define	SYSCALL_socket	97
#define	SYSCALL_connect	98
#define	SYSCALL_accept	99
#define	SYSCALL_getpriority	100
#define	SYSCALL_send	101
#define	SYSCALL_recv	102
				/* 103 was socketaddr */
#define	SYSCALL_bind	104
#define	SYSCALL_setsockopt	105
#define	SYSCALL_listen	106
				/* 107 was vtimes */
#define	SYSCALL_sigvec	108
#define	SYSCALL_sigblock	109
#define	SYSCALL_sigsetmask	110
#define	SYSCALL_sigpause	111
#define	SYSCALL_sigstack	112
#define	SYSCALL_recvmsg	113
#define	SYSCALL_sendmsg	114
#define	SYSCALL_vtrace	115
#define	SYSCALL_gettimeofday 116
#define	SYSCALL_getrusage	117
#define	SYSCALL_getsockopt	118
				/* 119 is old resuba */
#define	SYSCALL_readv	120
#define	SYSCALL_writev	121
#define	SYSCALL_settimeofday 122
#define	SYSCALL_fchown	123
#define	SYSCALL_fchmod	124
#define	SYSCALL_recvfrom	125
#define	SYSCALL_setreuid	126
#define	SYSCALL_setregid	127
#define	SYSCALL_rename	128
#define	SYSCALL_truncate	129
#define	SYSCALL_ftruncate	130
#define	SYSCALL_flock	131
				/* 132 is unused */
#define	SYSCALL_sendto	133
#define	SYSCALL_shutdown	134
#define	SYSCALL_socketpair	135
#define	SYSCALL_mkdir	136
#define	SYSCALL_rmdir	137
#define	SYSCALL_utimes	138
#define SYSCALL_getcntxt	139	/* restore register context */
#define	SYSCALL_adjtime	140
#define	SYSCALL_getpeername	141
#define	SYSCALL_gethostid	142
				/* 143 is old: sethostid */
#define	SYSCALL_getrlimit	144
#define	SYSCALL_setrlimit	145
#define	SYSCALL_killpg	146
				/* 147 is unused */
				/* 148 is old: setquota */
				/* 149 is old: quota */
#define	SYSCALL_getsockname	150
#define	SYSCALL_getmsg	151
#define	SYSCALL_putmsg	152
#define	SYSCALL_poll	153
				/* 154 is old: nfs_mount */
#define	SYSCALL_nfssvc	155
#define	SYSCALL_getdirentries 156
#define	SYSCALL_statfs	157
#define	SYSCALL_fstatfs	158
#define	SYSCALL_unmount	159
#define	SYSCALL_async_daemon 160
#define	SYSCALL_getfh	161
#define	SYSCALL_getdomainname 162
#define	SYSCALL_setdomainname 163
				/* 164 is old: pcfs_mount */
#define	SYSCALL_quotactl	165
#define	SYSCALL_exportfs	166
#define	SYSCALL_mount	167
#define	SYSCALL_ustat	168
#define	SYSCALL_semsys	169
#define	SYSCALL_msgsys	170
#define	SYSCALL_shmsys	171
#define	SYSCALL_auditsys	172
#define	SYSCALL_rfssys	173
#define	SYSCALL_getdents	174
#define	SYSCALL_setsid	175
#define	SYSCALL_fchdir	176
#define	SYSCALL_fchroot	177
#define	SYSCALL_vpixsys	178

#define	SYSCALL_aioread	179
#define	SYSCALL_aiowrite	180
#define	SYSCALL_aiowait	181
#define	SYSCALL_aiocancel	182

#define	SYSCALL_sigpending	183
				/* 184 is available */
#define	SYSCALL_setpgid	185
#define	SYSCALL_pathconf	186
#define	SYSCALL_fpathconf	187
#define	SYSCALL_sysconf	188

#define	SYSCALL_uname	189

#endif
