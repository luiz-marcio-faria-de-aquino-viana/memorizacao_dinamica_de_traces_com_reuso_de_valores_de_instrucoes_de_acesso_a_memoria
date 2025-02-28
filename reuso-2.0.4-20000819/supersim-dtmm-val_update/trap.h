
/*
 * trap.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/11/99
 */


#ifndef __TRAP_H
#define __TRAP_H 280


/////////////////////////////////////////////////////////
///// CTrap

class CTrap {
 private:

  bool_t* Running;


  CMem*  Mem;

  CIReg* IReg;

  CSReg* SReg;

  CComponent* Dtm;


  uword_t RegWin;


  uword_t TrapNum;

  uword_t TrapCode;

 
  /* declaracao das chamadas de sistema operacional */

  int Unimp();          /* Unimplemented Syscall */

  int Exit();           /* SYSCALL_exit */

  int Read();           /* SYSCALL_read */

  int Write();          /* SYSCALL_write */

  int Open();           /* SYSCALL_open */

  int Close();          /* SYSCALL_close */

  int Creat();          /* SYSCALL_creat */

  int Unlink();         /* SYSCALL_unlink */

  int Chdir();          /* SYSCALL_chdir */

  int Chmod();          /* SYSCALL_chmod */

  int Brk();            /* SYSCALL_brk */

  int Lseek();          /* SYSCALL_lseek */

  int Getpid();         /* SYSCALL_getpid */

  int Access();         /* SYSCALL_access */

  int Kill();           /* SYSCALL_kill */

  int Stat();           /* SYSCALL_fstatfs */

  int Ioctl();          /* SYSCALL_ioctl */

  int Fstat();          /* SYSCALL_fstat */

  int GetPageSize();    /* SYSCALL_getpagesize */

  int Mmap();           /* SYSCALL_mmap */

  int GetDTableSize();  /* SYSCALL_getdtablesize */

  int Fcntl();          /* SYSCALL_fcntl */

  int SigVec();         /* SYSCALL_sigvec */

  int SigBlock();       /* SYSCALL_sigblock */

  int SigSetMask();     /* SYSCALL_sigsetmask */

  int SigStack();       /* SYSCALL_sigstack */

  int Gettimeofday();   /* SYSCALL_gettimeofday */

  int Getrusage();      /* SYSCALL_getrusage */

  int Writev();         /* SYSCALL_writev */

  int Rename();         /* SYSCALL_rename */

  int Chown();          /* SYSCALL_chown */

  int Fchown();         /* SYSCALL_fchown */

  int Umask();          /* SYSCALL_umask */

  int Ftruncate();      /* SYSCALL_ftruncate */

  int Rmdir();          /* SYSCALL_rmdir */

  int Sync();           /* SYSCALL_sync */

  int Fsync();          /* SYSCALL_fsync */

  int Getuid();         /* SYSCALL_getuid */

  int Getgid();         /* SYSCALL_getgid */

  int Truncate();       /* SYSCALL_truncate */

  int Link();           /* SYSCALL_link */

  int Mkdir();          /* SYSCALL_mkdir */

  int Utime();          /* SYSCALL_utimes */

  int GetCntxt();       /* SYSCALL_getcntxt */

  int GetHostId();      /* SYSCALL_gethostid */

  int Getrlimit();      /* SYSCALL_getrlimit */

  int Setrlimit();      /* SYSCALL_setrlimit */

  int Pipe();           /* SYSCALL_pipe */

  int Dup();            /* SYSCALL_dup */

  int Dup2();           /* SYSCALL_dup2 */

  int Sbrk();           /* SYSCALL_sbrk */

  int Mount();          /* SYSCALL_mount */

  int Fchdir();         /* SYSCALL_fchdir */

  int Fchroot();        /* SYSCALL_fchroot */

  int Ustat();          /* SYSCALL_ustat */


 public:

  void Init( bool_t* _running,
	     CMem* _mem,
	     CIReg* _ireg,
	     CSReg* _sreg,
	     CComponent* _dtm );


  /* read and write integer registers */

  uword_t RdIReg(uword_t _r);

  uword_t WrIReg(uword_t _r, uword_t _val);


  /* limpa o flag de carry */

  uword_t ClrCarry();


  /* implementacao da operacao de invalidacao das tabelas do DTM */

  void InvDtmTables(uword_t _ptr, uword_t _len);

  void UpdDtmTables(uword_t _maddr, ubyte_t * _ptr, uword_t _len);


  /* vetor de chamadas ao sistema operacional */

  int osCall();

  int DoCall(uword_t _regwin, uword_t _trapnum);

};

#endif
