
/*
 * cpu.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

////////////////////////////////////////////////////////////
///// CCpu

CCpu::CCpu()
  : Mem(MEM_SIZE),
    Btb(BTB_NSETS, BTB_NENTRIES),
    FetchBuf(FETCH_WIDTH),
    IQueue(IQUEUE_SIZE),
    Dtm(MEMOTABLEG_SIZE, MEMOTABLET_SIZE, MEMOTABLEL_SIZE),
    Disq(DISQ_SIZE),
    DBDisq(DBDISQ_SIZE),
    IReg(IREG_NGREGS, IREG_NUMWIN, IREG_WINSIZE, IREG_WINOFFSET),
    RsIu(RSIU_SIZE),
    RsBpu(RSBPU_SIZE),
    RsLsu(RSLSU_SIZE),
    Maq(MAQ_SIZE),
    Rob(ROB_SIZE),
    Iu(NUM_IU),
    Lsu(NUM_LSU),
    Bpu(NUM_BPU)
{
#if(DEBUG == 0)
  printf("\n[ CPU::PASS_1 ]");
#endif

  strcpy(FileName, "");

  Titled = FALSE;

  Running = FALSE;

  DtmBrkCnt = 1000000000;

  DtmBrkflag = FALSE;

  Breakflag = FALSE;

  Breakpt = 0;

  MemBrkflag = FALSE;

  MemBrkpt = 0;

  Cicles = 0;

  NSteps = 0;

  Mem.Init( & Dtm,
	    & MemBrkflag,
	    & MemBrkpt );

  Dtm.Init( & Stat,
	    & Btb,
	    & FetchBuf,
	    & Fetch,
	    & IQueue,
	    & Disq,
	    & Dispatch,
	    & SReg,
	    & IReg,
	    & RsLsu,
	    & Maq,
	    & Lsu,
	    & Rob );

  Predict.Init( & Btb );

  FetchBuf.Init( & Predict );

  IMmu.Init( & Mem,
	     & FetchBuf );

  DMmu.Init( & Mem );

  Fetch.Init( & IMmu,
	      & FetchBuf,
	      & IQueue,
	      & Dtm );

  Decode.Init( & IQueue,
	       & Disq );

  Dispatch.Init( &Dtm,
		 & Disq,
		 & DBDisq,
		 & SReg,
		 & IReg,
		 & RsIu,
		 & RsBpu,
		 & RsLsu,
		 & Maq,
		 & Rob );

  Iu.Init( & RsIu );

  Lsu.Init( & DMmu,
	    & FetchBuf,
	    & Maq,
	    & RsLsu,
	    & Rob );

  Bpu.Init( & Btb,
	    & FetchBuf,
	    & IQueue,
	    & Fetch,
	    & Disq, 
	    & DBDisq,
	    & IReg, 
	    & SReg,
	    & RsIu, 
	    & RsBpu, 
	    & RsLsu,
	    & Maq, 
	    & Rob,
	    & Iu, 
	    & Lsu );

  Complete.Init( & IReg,
		 & SReg,
		 & RsIu,
		 & RsBpu,
		 & RsLsu,
		 & Iu,
		 & Bpu,
		 & Lsu,
		 & Rob ); 

  Wb.Init( & Stat,
	   & FetchBuf,
	   & Fetch,
	   & IQueue,
	   & Dtm,
	   & Disq,
	   & DBDisq,
	   & Dispatch,
	   & IReg,
	   & SReg,
	   & RsIu,
	   & RsBpu,
	   & RsLsu,
	   & Maq,
	   & Rob,
	   & Iu,
	   & Lsu,
	   & Bpu,
	   & Trap );

  Trap.Init( & Running,
	     & Mem,
	     & IReg,
	     & SReg,
	     & Dtm );
  
#if(DEBUG == 0)
  printf("\n[ CPU::PASS_2 ]");
#endif

  Argc = (int*) Mem.GetMemPtr(INISP + 0x40);
  (*Argc) = 0;

  Argp = (char**) Mem.GetMemPtr(INISP + 0x44);

  for(int i = 0; i < ARGNUM; i++) {
#ifdef BIGENDIAN
    Argp[i] = (char*) LB_WORD( (ARGPTR + (64 * i)) );
    Argv[i] = (char*) Mem.GetMemPtr(LB_WORD( (uword_t) Argp[i] ));
#else
    Argp[i] = (char*) (ARGPTR + (64 * i));
    Argv[i] = (char*) Mem.GetMemPtr( (uword_t) Argp[i] );
#endif
  }

  strcpy(Argv[0], FileName);
#ifdef BIGENDIAN
  (*Argc) = LB_WORD( (LB_WORD( *Argc ) + 1) );
#else
  (*Argc) += 1;
#endif
}

udword_t CCpu::GetNumCicles()
{
  return Cicles;
}

void CCpu::SetFileName(char* _filename)
{
  strcpy(FileName, _filename);
  Titled = TRUE;
}

void CCpu::SetNumSteps(udword_t _nsteps)
{
  NSteps = _nsteps;
}

void CCpu::SetBreakpt(uword_t _memaddr)
{
  Breakpt = _memaddr;
}

void CCpu::SetMemBreakpt(uword_t _memaddr)
{
  MemBrkpt = _memaddr;
}

void CCpu::SetDtmBreakCounter(uword_t _cnt)
{
  DtmBrkCnt = _cnt;
}

void CCpu::ShowArgs()
{
  int tmp_argc;

  printf("\n\nArgument List");
  printf("\n=============\n");

#ifdef BIGENDIAN
  tmp_argc = LB_WORD( *Argc );
#else
  tmp_argc = (*Argc);
#endif

  printf("\nArgc = %d\n", tmp_argc);

  for(int i = 0; i < tmp_argc; i++)
    printf("\nArg[%d] = %s", i, Argv[i]);

  printf("\n");
}

void CCpu::Tick(bool_t _steprun)
{
  /* estagio de escrita nos registradores (WB) */

#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(BEFORE::WB)");
#endif
  if(_steprun == TRUE)
    Wb.Execute(& NSteps);
  else
    Wb.Execute();
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(AFTER::WB)");
#endif

  /* estagio de conclusao da instrucao (C) */

  Complete.Execute();
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(COMPLETE)");
#endif

  /* estagio de execucao (EX) */
  Iu.Execute();
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(IU)\n");
#endif

  Lsu.Execute();
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(LSU)");
#endif

  Bpu.Execute();
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(BPU)");
#endif

  /* estagio de despacho (DS) */

  Dispatch.Execute(& DtmBrkflag, & DtmBrkCnt);
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(DISPATCH)");
#endif

  /* estagio de decodificacao (ID) */

  Decode.Execute();
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(DECODE)");
#endif


  /* estagio de busca (IF) */

#if(DEBUG == 0)
  printf("\nCPU::FETCHBUF_LASTBLEVEL(%lu)", FetchBuf.GetLastBranchLevel());
  printf("\nCPU::FETCHBUF_SEQNUM(%lu) <BEFORE_FETCH>", FetchBuf.GetCurrSeqNum());
#endif
  Fetch.Execute(& Breakflag, Breakpt);
#if(DEBUG == 0)
  fprintf(stderr, "\nCPU::TICK(IF)");
#endif

#if(DEBUG == 0)
  printf("\nCPU::FETCHBUF_SEQNUM(%lu) <AT_THE_END>", FetchBuf.GetCurrSeqNum());
#endif

  Stat.IncCicle();

  Cicles += 1;
}

void CCpu::Step()
{
  Breakflag = FALSE;
  DtmBrkflag = FALSE;
  MemBrkflag = FALSE;

  if( Running == TRUE )
    Tick();
  else {
    if(Breakflag == TRUE)
      printf("\n[ ATT: Breakpt found (memaddr=%08lx). ]", Breakpt);
    else if(DtmBrkflag == TRUE)
      printf("\n[ ATT: DTM Breakpt found. ]");
    else if(MemBrkflag == TRUE)
      printf("\n[ ATT: Memory breakpt found (memaddr=%08lx). ]", MemBrkpt);
    else
      printf("\n[ ATT: Execucao concluida. ]");
  }
}

void CCpu::Run()
{
  Breakflag = FALSE;
  DtmBrkflag = FALSE;
  MemBrkflag = FALSE;

  Stat.StartClock();
  while( (Running == TRUE) &&
	 (Breakflag == FALSE) &&
	 (MemBrkflag == FALSE) &&
	 (DtmBrkflag == FALSE) )
    Tick();
  Stat.StopClock();

  if(Breakflag == TRUE)
    printf("\n[ ATT: Breakpt found (memaddr=%08lx). ]", Breakpt);
  else if(DtmBrkflag == TRUE)
    printf("\n[ ATT: DTM Breakpt found. ]");
  else if(MemBrkflag == TRUE)
    printf("\n[ ATT: Memory breakpt found (memaddr=%08lx). ]", MemBrkpt);
  else
    printf("\n[ ATT: Execucao concluida. ]");
}

void CCpu::Runn()
{
  Breakflag = FALSE;
  DtmBrkflag = FALSE;
  MemBrkflag = FALSE;

  Stat.StartClock();
  while( (NSteps > 0) &&
	 (Running == TRUE) &&
	 (Breakflag == FALSE) &&
	 (MemBrkflag == FALSE) &&
	 (DtmBrkflag == FALSE) )
    Tick(TRUE);
  Stat.StopClock();

  if(Breakflag == TRUE)
    printf("\n[ ATT: Breakpt found (memaddr=%08lx). ]", Breakpt);
  else if(DtmBrkflag == TRUE)
    printf("\n[ ATT: DTM Breakpt found. ]");
  else if(MemBrkflag == TRUE)
    printf("\n[ ATT: Memory breakpt found (memaddr=%08lx). ]", MemBrkpt);
  else
    printf("\n[ ATT: Execucao concluida. ]");
}

void CCpu::Reset()
{
  Stat.Reset();

  Btb.Reset();

  FetchBuf.Reset();

  Fetch.Reset(Mem.GetEntryPtr());

  IQueue.Reset();

  Disq.Reset();

  DBDisq.Reset();

  SReg.Reset();

  IReg.Reset();

  RsIu.Reset();

  RsBpu.Reset();

  RsLsu.Reset();

  Maq.Reset();

  Rob.Reset();

  Iu.Reset();

  Lsu.Reset();

  Bpu.Reset();

  if(Titled == TRUE)
    Running = TRUE;

  Cicles = 0;

  NSteps = 0;
}

void CCpu::Load()
{
  if( Titled == TRUE )
    Mem.Load(FileName);
  Reset();
}
