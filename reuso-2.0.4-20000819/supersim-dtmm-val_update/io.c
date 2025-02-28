
/*
 * io.c
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/7/2000
 */

#include<stdio.h>
#include<errno.h>
#include<stdlib.h>
#include<ctype.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/uio.h>
#include<sys/time.h>
#include<sys/resource.h>
#include<sys/times.h>
#include<sys/mman.h>
#include<sys/ioctl.h>
#include<ustat.h>
#include"io.h"

int ss_writev(int fd, struct iovec* iov, int iovcnt)
{
  return writev(fd, iov, iovcnt);
}

int ss_fchown(int fd, int owner, int group)
{
  return fchown(fd, owner, group);
}

int ss_truncate(char* path, off_t length)
{
  return truncate(path, length);
}

int ss_ftruncate(int fd, off_t length)
{
  return ftruncate(fd, length);
}

int ss_fsync(int fd)
{
  return fsync(fd);
}

int ss_ioctl(int fd, int request, caddr_t arg)
{
  return ioctl(fd, request, arg);
}

int ss_getrlimit(int resource, struct rlimit* rlp)
{
  return getrlimit(resource, rlp);
}

int ss_setrlimit(int resource, struct rlimit* rlp)
{
  return setrlimit(resource, rlp);
}

int ss_getdtablesize()
{
  return getdtablesize();
}

int ss_gettimeofday(struct timeval *tp, struct timezone* tzp)
{
  return gettimeofday(tp, tzp);
}
