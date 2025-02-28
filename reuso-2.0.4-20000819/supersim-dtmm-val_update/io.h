
/*
 * io.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 4/7/2000
 */

#include<sys/types.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/uio.h>
#include"defs.h"

#ifndef __IO_H
#define __IO_H 277

/* declaracao das funcoes da libc nao declaradas nos cabecalhos do GCC 2.5.2
 */

#ifdef OLDGCC

int writev(int fd, struct iovec* iov, int iovcnt);

int fchown(int fd, int owner, int group);

int truncate(char* path, off_t length);

int ftruncate(int fd, off_t length);

int fsync(int fd);

int ioctl(int fd, int request, caddr_t arg);

int getrlimit(int resource, struct rlimit* rlp);

int setrlimit(int resource, struct rlimit* rlp);

int getdtablesize();

int gettimeofday(struct timeval *tp, struct timezone* tzp);

#endif


/* declaracao das funcoes encapsuladas da libc
 */

int ss_writev(int fd, struct iovec* iov, int iovcnt);

int ss_fchown(int fd, int owner, int group);

int ss_truncate(char* path, off_t length);

int ss_ftruncate(int fd, off_t length);

int ss_fsync(int fd);

int ss_ioctl(int fd, int request, caddr_t arg);

int ss_getrlimit(int resource, struct rlimit* rlp);

int ss_setrlimit(int resource, struct rlimit* rlp);

int ss_getdtablesize();

int ss_gettimeofday(struct timeval *tp, struct timezone* tzp);

#endif
