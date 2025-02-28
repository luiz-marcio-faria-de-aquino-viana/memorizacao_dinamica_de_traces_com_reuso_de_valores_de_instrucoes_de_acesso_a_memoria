
/*
 * trap.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/11/99
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
#include"all.h"

extern int errno;

extern "C" {

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

}

///////////////////////////////////////////////////////////
///// CTrap

/* implementacao das chamadas de sistema operacional */

void CTrap::Init( bool_t* _running,
		  CMem* _mem,
		  CIReg* _ireg,
		  CSReg* _sreg,
		  CComponent* _dtm )
{
  Running = _running;
  Mem = _mem;
  IReg = _ireg;
  SReg = _sreg;
  Dtm = _dtm;
}

/* read and write integer registers */

uword_t CTrap::RdIReg(uword_t _r)
{
  return IReg->GetRValue(RegWin, _r);
}

uword_t CTrap::WrIReg(uword_t _r, uword_t _val)
{
  IReg->SetRValue(RegWin, _r, _val);
  return _val;
}

uword_t CTrap::ClrCarry()
{
  uword_t psr;
  psr = ((SReg->Psr).GetRValue() & 0xffefffff);
  (SReg->Psr).SetRValue(psr);
  return psr;
}

/* vetor de chamadas ao sistema operacional */

int CTrap::osCall()
{
  int rst;

  switch(TrapCode) {
  case SYSCALL_exit:
    rst = Exit();
    break;
  case SYSCALL_read:
    rst = Read();
    break;
  case SYSCALL_write:
    rst = Write();
    break;
  case SYSCALL_open:
    rst = Open();
    break;
  case SYSCALL_close:
    rst = Close();
    break;
  case SYSCALL_creat:
    rst = Creat();
    break;
  case SYSCALL_unlink:
    rst = Unlink();
    break;
  case SYSCALL_chdir:
    rst = Chdir();
    break;
  case SYSCALL_chmod:
    rst = Chmod();
    break;
  case SYSCALL_brk:
    rst = Brk();
    break;
  case SYSCALL_lseek:
    rst = Lseek();
    break;
  case SYSCALL_getpid:
    rst = Getpid();
    break;
  case SYSCALL_access:
    rst = Access();
    break;
  case SYSCALL_kill:
    rst = Kill();
    break;
  case SYSCALL_stat:
    rst = Stat();
    break;
  case SYSCALL_ioctl:
    rst = Ioctl();
    break;
  case SYSCALL_fstat:
    rst = Fstat();
    break;
  case SYSCALL_getpagesize:
    rst = GetPageSize();
    break;
  case SYSCALL_mmap:
    rst = Mmap();
    break;
  case SYSCALL_getdtablesize:
    rst = GetDTableSize();
    break;
  case SYSCALL_fcntl:
    rst = Fcntl();
    break;
  case SYSCALL_sigvec:
    rst = SigVec();
    break;
  case SYSCALL_sigblock:
    rst = SigBlock();
    break;
  case SYSCALL_sigsetmask:
    rst = SigSetMask();
    break;
  case SYSCALL_sigstack:
    rst = SigStack();
    break;
  case SYSCALL_gettimeofday:
    rst = Gettimeofday();
    break;
  case SYSCALL_getrusage:
    rst = Getrusage();
    break;
  case SYSCALL_writev:	
    rst = Writev();
    break;
  case SYSCALL_rename:
    rst = Rename();
    break;
  case SYSCALL_truncate:
    rst = Truncate();
    break;
  case SYSCALL_mkdir:
    rst = Mkdir();
    break;
  case SYSCALL_utimes:
    rst = Utime();
    break;
  case SYSCALL_getcntxt:
    rst = GetCntxt();
    break;
  case SYSCALL_gethostid:	
    rst = GetHostId();
    break;
  case SYSCALL_getrlimit:
    rst = Getrlimit();
    break;
  case SYSCALL_setrlimit:
    rst = Setrlimit();
    break;
  default:
    rst = Unimp();
  }

  return rst;
}

int CTrap::DoCall(uword_t _regwin, uword_t _trapnum)
{
  RegWin = _regwin;

  TrapNum = _trapnum;
  TrapCode = RdIReg(G1);

  printf("\nSystem call: trap = %lu, code = %lu\n", TrapNum, TrapCode);

  osCall();

  ClrCarry();

  return 0;
}

/* implementacao da operacao de invalidacao das tabelas do DTM */

void CTrap::InvDtmTables(uword_t _ptr, uword_t _len)
{
  ((CDtm*) Dtm)->Invalidate(_ptr, _len);
}

void CTrap::UpdDtmTables(uword_t _maddr, ubyte_t * _ptr, uword_t _len)
{
  ((CDtm*) Dtm)->UpdateMemValue(_maddr, _ptr, _len);
}

/* implementacao das chamadas de sistema operacional */

int CTrap::Unimp()
{
  int code;
  str_t s;

  code = (int) RdIReg(G1);
  sprintf(s, "(syscall: %d)", code);
  warnmsg(__TRAP_H, WARN_SYSCALLNOTIMPLEMENTED, s);
  return(FALSE);
}

int CTrap::Open()
{
  char *path;
  int flags, mode;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  flags = (int) RdIReg(O1);
  mode = (int) RdIReg(O2);
  ret = open(path, flags, mode);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Close()
{
  int fd;
  int ret;

  fd = (int) RdIReg(O0);
  ret = 0;
  if (fd != 1)
    ret = close(fd);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Lseek()
{
  int fd;
  off_t offset;
  int whence;
  off_t ret;

  fd = (int) RdIReg(O0);
  offset = (off_t) RdIReg(O1);
  whence = (int) RdIReg(O2);
  ret = lseek(fd, offset, whence);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Read()
{
  int fd;
  char *buf;
  int nbyte;
  int ret;

  fd = (int) RdIReg(O0);
  buf = (char*) Mem->GetMemPtr(RdIReg(O1));
  nbyte = (int) RdIReg(O2);
  ret = read(fd, buf, nbyte);
  WrIReg(O0, (unsigned) ret);

  UpdDtmTables(RdIReg(O1), (ubyte_t*) buf, nbyte);

  return(TRUE);
}

int CTrap::Write()
{
  int fd;
  char *buf;
  int nbyte;
  int ret;

  fd = (int) RdIReg(O0);
  buf = (char*) Mem->GetMemPtr(RdIReg(O1));
  nbyte = (int) RdIReg(O2);
  ret = write(fd, buf, nbyte);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Writev()
{
  int fd;
  struct iovec tmp_iov, *iov;
  int iovcnt;
  int ret;

  fd = (int) RdIReg(O0);
  iov = (struct iovec*) Mem->GetMemPtr(RdIReg(O1));
  iovcnt = (int) RdIReg(O2);

#ifdef BIGENDIAN
  ret = ss_writev(fd, & tmp_iov, iovcnt);
  iov->iov_base = (__ptr_t) LB_WORD((uword_t) tmp_iov.iov_base);
  iov->iov_len = LB_WORD(tmp_iov.iov_len);
#else
  ret = ss_writev(fd, iov, iovcnt);
#endif

  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Creat()
{
  char *path;
  int mode;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  mode = (int) RdIReg(O1);
  ret = creat(path, mode);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Stat()
{
  char *path;
  struct statbuf *buf;
  int ret;

  struct stat tmp;
 
  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  buf = (struct statbuf*) Mem->GetMemPtr(RdIReg(O1));

  ret = stat(path, & tmp);

#ifdef BIGENDIAN
  buf->_dev = LB_HWORD(tmp.st_dev);
  buf->_ino = LB_WORD(tmp.st_ino);
  buf->_mode = LB_HWORD(tmp.st_mode);
  buf->_nlink = LB_HWORD(tmp.st_nlink);
  buf->_uid = LB_HWORD(tmp.st_uid);
  buf->_gid = LB_HWORD(tmp.st_gid);
  buf->_rdev = LB_HWORD(tmp.st_rdev);
  buf->_size = LB_WORD(tmp.st_size);
  buf->_atime = LB_WORD(tmp.st_atime);
  buf->_mtime = LB_WORD(tmp.st_mtime);
  buf->_ctime = LB_WORD(tmp.st_ctime);
  buf->_blksize = LB_WORD(tmp.st_blksize);
  buf->_blocks = LB_WORD(tmp.st_blocks);
#else
  buf->_dev = tmp.st_dev;
  buf->_ino = tmp.st_ino;
  buf->_mode = tmp.st_mode;
  buf->_nlink = tmp.st_nlink;
  buf->_uid = tmp.st_uid;
  buf->_gid = tmp.st_gid;
  buf->_rdev = tmp.st_rdev;
  buf->_size = tmp.st_size;
  buf->_atime = tmp.st_atime;
  buf->_mtime = tmp.st_mtime;
  buf->_ctime = tmp.st_ctime;
  buf->_blksize = tmp.st_blksize;
  buf->_blocks = tmp.st_blocks;
#endif

#if(DEBUG == 0)
  printf( "TRAP::SYSCALL_stat\n");

  printf("\tID of device = %i\n", buf->_dev);
  printf("\tInode number = %lu\n", buf->_ino);
  printf("\tFile mode = %u\n", buf->_mode);
  printf("\tNumber of links = %i\n", buf->_nlink);
  printf("\tUser ID of the file's owner = %u\n", buf->_uid);
  printf("\tGroup ID of the file's group = %u\n", buf->_gid);
  printf("\tID of device containing = %i\n", buf->_rdev);
  printf("\tFile size in bytes = %li\n", buf->_size);
  printf("\tTime of last access = %li\n", buf->_atime);
  printf("\tTime of last data modification = %li\n", buf->_mtime);
  printf("\tTime of last file status change = %li\n", buf->_ctime);
  printf("\tBlock size = %li\n", buf->_blksize);
  printf("\tNumber of blocks = %li\n", buf->_blocks);

  printf("\n\tRet = %u\n", ret);
#endif
  WrIReg(O0, (unsigned) ret);

  UpdDtmTables(RdIReg(O1), (ubyte_t*) buf, sizeof(struct statbuf));

  return(TRUE);
}

int CTrap::Fstat()
{
  int fd;
  struct statbuf *buf;
  int ret;

  struct stat tmp;
 
  fd = (int) RdIReg(O0);
  buf = (struct statbuf*) Mem->GetMemPtr(RdIReg(O1));

  ret = fstat(fd, & tmp);

#ifdef BIGENDIAN
  buf->_dev = LB_HWORD(tmp.st_dev);
  buf->_ino = LB_WORD(tmp.st_ino);
  buf->_mode = LB_HWORD(tmp.st_mode);
  buf->_nlink = LB_HWORD(tmp.st_nlink);
  buf->_uid = LB_HWORD(tmp.st_uid);
  buf->_gid = LB_HWORD(tmp.st_gid);
  buf->_rdev = LB_HWORD(tmp.st_rdev);
  buf->_size = LB_WORD(tmp.st_size);
  buf->_atime = LB_WORD(tmp.st_atime);
  buf->_mtime = LB_WORD(tmp.st_mtime);
  buf->_ctime = LB_WORD(tmp.st_ctime);
  buf->_blksize = LB_WORD(tmp.st_blksize);
  buf->_blocks = LB_WORD(tmp.st_blocks);
#else
  buf->_dev = tmp.st_dev;
  buf->_ino = tmp.st_ino;
  buf->_mode = tmp.st_mode;
  buf->_nlink = tmp.st_nlink;
  buf->_uid = tmp.st_uid;
  buf->_gid = tmp.st_gid;
  buf->_rdev = tmp.st_rdev;
  buf->_size = tmp.st_size;
  buf->_atime = tmp.st_atime;
  buf->_mtime = tmp.st_mtime;
  buf->_ctime = tmp.st_ctime;
  buf->_blksize = tmp.st_blksize;
  buf->_blocks = tmp.st_blocks;
#endif

#if(DEBUG == 0)
  printf( "TRAP::SYSCALL_fstat\n");

  printf("\tID of device = %i\n", buf->_dev);
  printf("\tInode number = %lu\n", buf->_ino);
  printf("\tFile mode = %u\n", buf->_mode);
  printf("\tNumber of links = %i\n", buf->_nlink);
  printf("\tUser ID of the file's owner = %u\n", buf->_uid);
  printf("\tGroup ID of the file's group = %u\n", buf->_gid);
  printf("\tID of device containing = %i\n", buf->_rdev);
  printf("\tFile size in bytes = %li\n", buf->_size);
  printf("\tTime of last access = %li\n", buf->_atime);
  printf("\tTime of last data modification = %li\n", buf->_mtime);
  printf("\tTime of last file status change = %li\n", buf->_ctime);
  printf("\tBlock size = %li\n", buf->_blksize);
  printf("\tNumber of blocks = %li\n", buf->_blocks);

  printf("\n\tRet = %u\n", ret);
#endif
  WrIReg(O0, (unsigned) ret);

  UpdDtmTables(RdIReg(O1), (ubyte_t*) buf, sizeof(struct statbuf));

  return(TRUE);
}

int CTrap::Link()
{
  char *path1, *path2;
  int ret;

  path1 = (char*) Mem->GetMemPtr(RdIReg(O0));
  path2 = (char*) Mem->GetMemPtr(RdIReg(O1));
  ret = link(path1, path2);

  printf("trap::link(ret=%d)\n", ret);

  return(TRUE);
}

int CTrap::Unlink()
{
  char *path;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  ret = unlink(path);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Chdir()
{
  char *path;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  ret = chdir(path);

  printf("trap::chdir(ret=%d)\n", ret);

  return(TRUE);
}

int CTrap::Chmod()
{
  char *path;
  mode_t mode;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  mode = (mode_t) RdIReg(O1);
  ret = chmod(path, mode);

  printf("trap::chmod(ret=%d)\n", ret);

  return(TRUE);
}

int CTrap::Chown()
{
  char *path;
  int owner,group;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  owner = (int) RdIReg(O1);
  group = (int) RdIReg(O2);
  ret = chown(path, owner, group);
  WrIReg(O0, (unsigned) ret);

  printf("trap::fchown(ret=%d)\n", ret);

  return(TRUE);
}

int CTrap::Fchown()
{
  int fd, owner, group;
  int ret;

  fd = (int) RdIReg(O0);
  owner = (int) RdIReg(O1);
  group = (int) RdIReg(O2);
  ret = ss_fchown(fd, owner, group);

  printf("trap::fchown(fd=%d,owner=%d,group=%d,ret=%d)\n", fd, owner, group, ret);

  return(TRUE);
}

int CTrap::Umask()
{
  int mask;
  int ret;

  mask = (int) RdIReg(O0);
  ret = umask(mask);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Access()
{
  char *path;
  int mode, ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  mode = (int) RdIReg(O1);
  ret = access(path, mode);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Rename()
{
  char *from, *to;
  int ret;

  from = (char*) Mem->GetMemPtr(RdIReg(O0));
  to   = (char*) Mem->GetMemPtr(RdIReg(O1));
  ret = rename(from, to);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Truncate()
{
  char *path;
  int length;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  length = (int) RdIReg(O1);
  ret = ss_truncate(path, length);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Ftruncate()
{
  int fd, length;
  int ret;

  fd = (int) RdIReg(O0);
  length = (int) RdIReg(O1);
  ret = ss_ftruncate(fd, length);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Mkdir()
{
  char *path;
  int mode;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  mode = (int) RdIReg(O1);
  ret = mkdir(path, mode);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Rmdir()
{
  char *path;
  int ret;

  path = (char*) Mem->GetMemPtr(RdIReg(O0));
  ret = rmdir(path);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Fcntl()
{
  int fd, cmd, arg;
  int ret;

  fd = (int) RdIReg(O0);
  cmd = (int) RdIReg(O1);
  arg = (int) RdIReg(O2);
  ret = fcntl(fd, cmd, arg);
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Sync()
{
  int ret;
#if(DEBUG <= 17)
  fprintf(stderr, "TRAP::SYSCALL_sync\n");
#endif
  ret = 0;
  WrIReg(O0, (unsigned) ret);
  return(TRUE);
}

int CTrap::Fsync()
{
  int fd;
  int ret;

  fd = (int) RdIReg(O0);
  ret = ss_fsync(fd);
  WrIReg(O0, (unsigned) ret);
  return(TRUE);
}

int CTrap::Getpid()
{
  pid_t ret;
  
  ret = getpid();
  WrIReg(O0, (unsigned) ret);
  return(TRUE);
}

int CTrap::Getuid()
{
  int ret;
  
  ret = getuid();
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::Getgid()
{
  int ret;
  
  ret = getgid();
  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::SigVec()
{
  int sig, ret;
  struct sigvec *vec, *oldvec;
  
#if(DEBUG <= 17)
  fprintf(stderr, "TRAP::SYSCALL_sigvec\n");
#endif

  sig = (int) RdIReg(O0);
  vec = (struct sigvec *) Mem->GetMemPtr(RdIReg(O1));
  oldvec = (struct sigvec *) Mem->GetMemPtr(RdIReg(O2));
  // ret = sigvec(sig, vec, oldvec);
  ret = 0;

  printf("trap::sigvec(sig=%d,vec=%ld,oldvec=%ld,ret=%d)\n", sig, (uword_t)vec, (uword_t)oldvec, ret);

  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}
	
int CTrap::SigBlock()
{
  int mask, ret;

#if(DEBUG <= 17)
  fprintf(stderr, "TRAP::SYSCALL_sigblock\n");
#endif

  mask = (int) RdIReg(O0);
  // ret = sigblock(mask);
  ret = 0;

  printf("trap::sigblock(mask=%d,ret=%d)\n", mask, ret);

  WrIReg(O0, (unsigned) ret);
 
  return(TRUE);
}

int CTrap::SigSetMask()
{
  int mask, ret;
  
#if(DEBUG <= 17)
  fprintf(stderr, "TRAP::SYSCALL_sigsetmask\n");
#endif

  mask = (int) RdIReg(O0);
  // ret = sigsetmask(mask);
  ret = 0x1000;

  printf("trap::sigsetmask(mask=%d,ret=%d)\n", mask, ret);

  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::SigStack()
{
  warnmsg(__TRAP_H, WARN_SIGSTACKSYSCALL, "");
  return(TRUE);
}

int CTrap::Kill()
{
  warnmsg(__TRAP_H, WARN_KILLSYSCALL, "");
  return(TRUE);
}

int CTrap::Exit()
{
  warnmsg(__TRAP_H, WARN_EXITSYSCALL, "");
  (*Running) = FALSE;
  return(TRUE);
}

int CTrap::Pipe()
{
  warnmsg(__TRAP_H, WARN_PIPESYSCALL, "");
  return(TRUE);
}

int CTrap::Dup()
{  
  warnmsg(__TRAP_H, WARN_DUPSYSCALL, "");
  return(TRUE);
}

int CTrap::Dup2()
{
  warnmsg(__TRAP_H, WARN_DUP2SYSCALL, "");
  return(TRUE);
}

int CTrap::Ioctl()
{
  int fd, cmd, arg;
  int ret;
  
  fd = (int) RdIReg(O0);
  cmd = (int) RdIReg(O1);
  arg = (int) RdIReg(O2);
  ret = ss_ioctl(fd, cmd, (char*) arg);

  printf("\nioctl(fd=%d, cmd=%x)", fd, cmd);

#if(DEBUG == 0)
  if(ret == -1) {
    printf("TRAP::IOCTL(errno=%d)", errno);
    perror(0);
  }
#endif

  WrIReg(O0, (unsigned) ret);
  return(TRUE);
}

int CTrap::GetPageSize()
{
  WrIReg(O0, (unsigned) 4096);
  return(TRUE);
}

int CTrap::Getrusage()
{
  WrIReg(O0, (unsigned) 0);
  return(TRUE);
}

int CTrap::Getrlimit()
{
  int resource;
  struct rlimit *rlp;
  int ret;
  
  resource = (int) RdIReg(O0);
  rlp = (struct rlimit *) Mem->GetMemPtr(RdIReg(O1));
  ret = ss_getrlimit(resource, rlp);

  WrIReg(O0, (unsigned) ret);

  UpdDtmTables(RdIReg(O1), (ubyte_t*) rlp, sizeof(struct rlimit));

  return(TRUE);
}

int CTrap::Setrlimit()
{
  int resource;
  struct rlimit *rlp;
  int ret;
  
  resource = (int) RdIReg(O0);
  rlp = (struct rlimit *) Mem->GetMemPtr(RdIReg(O1));
  ret = ss_setrlimit(resource, rlp);

  WrIReg(O0, (unsigned) ret);

  return(TRUE);
}

int CTrap::GetCntxt()
{
  warnmsg(__TRAP_H, WARN_GETCNTXTSYSCALL, "");
  return(TRUE);
}


int CTrap::Mmap()
{
#if(DEBUG <= 17)
  fprintf(stderr, "TRAP::SYSCALL_mmap\n");
#endif

  caddr_t addr;
  size_t len;
  int prot, flags, fd;
  off_t off;
  
  addr = (caddr_t) Mem->GetMemPtr(RdIReg(O0));
  len = (size_t) RdIReg(O1);
  prot = (int) RdIReg(O2);
  flags = (int) RdIReg(O3);
  fd = (int) RdIReg(O4);
  off = (off_t) RdIReg(O5);

  printf("trap::mmap(addr=%ld,len=%ld,prot=%d,flags=%d,fd=%d,off=%ld)\n", (uword_t)addr, (uword_t)len, prot, flags, fd, (uword_t)off);

  WrIReg(O0, (unsigned long int) 0xffffffffL); 

  return(TRUE);
}


int CTrap::GetDTableSize()
{
  int size;

  size = ss_getdtablesize();
  WrIReg(O0, (unsigned long int) size);
  return(TRUE);
}

int CTrap::Brk()
{
  WrIReg(O0, 0);
  return(TRUE);
}	

int CTrap::Sbrk()
{
  warnmsg(__TRAP_H, WARN_SBRKSYSCALL, "");
  return(TRUE);
}

int CTrap::Utime()
{
  warnmsg(__TRAP_H, WARN_UTIMESYSCALL, "");
  return(TRUE);
}

int CTrap::GetHostId()
{
  unsigned long int ret;
  
  ret = (unsigned long int) 0x55006127; 
  WrIReg(O0, (unsigned long int) ret);
  return(TRUE);
}


int CTrap::Mount()
{
  char *specialfile, *dir;
  int rwflag;
  
  specialfile = (char*) Mem->GetMemPtr(RdIReg(O0));
  dir = (char*) Mem->GetMemPtr(RdIReg(O1));
  rwflag = (int) RdIReg(O2);

  printf("trap::mount(specialfile=%ld,dir=%ld,rwflag=%d)\n", (uword_t)specialfile, (uword_t)dir, rwflag);

  return(TRUE);
}


int CTrap::Fchdir()
{
  warnmsg(__TRAP_H, WARN_FCHDIRSYSCALL, "");
  return(TRUE);
}


int CTrap::Fchroot()
{
  warnmsg(__TRAP_H, WARN_FCHROOTSYSCALL, "");
  return(TRUE);
}


int CTrap::Ustat()
{
  warnmsg(__TRAP_H, WARN_USTATSYSCALL, "");
  return(TRUE);
}

int CTrap::Gettimeofday()
{
  struct timeval tmp_tp, *tp;
  struct timezone tmp_tzp, *tzp;
  int ret;
  
  tp = (struct timeval*) Mem->GetMemPtr(RdIReg(O0));
  tzp = (struct timezone*) Mem->GetMemPtr(RdIReg(O1));

  // ret = ss_gettimeofday(& tmp_tp, & tmp_tzp);
  ret = 0;
#ifdef BIGENDIAN
  tp->tv_sec = LB_WORD(tmp_tp.tv_sec);
  tp->tv_usec = LB_WORD(tmp_tp.tv_usec);

  tzp->tz_minuteswest = LB_WORD(tmp_tzp.tz_minuteswest);
  tzp->tz_dsttime = LB_WORD(tmp_tzp.tz_dsttime);
#else
  tp->tv_sec = tmp_tp.tv_sec;
  tp->tv_usec = tmp_tp.tv_usec;

  tzp->tz_minuteswest = tmp_tzp.tz_minuteswest;
  tzp->tz_dsttime = tmp_tzp.tz_dsttime;
#endif
  WrIReg(O0, (uword_t) ret);

  UpdDtmTables(RdIReg(O0), (ubyte_t*) tp, sizeof(struct timeval));
  UpdDtmTables(RdIReg(O1), (ubyte_t*) tzp, sizeof(struct timezone));

  return(TRUE);
}
