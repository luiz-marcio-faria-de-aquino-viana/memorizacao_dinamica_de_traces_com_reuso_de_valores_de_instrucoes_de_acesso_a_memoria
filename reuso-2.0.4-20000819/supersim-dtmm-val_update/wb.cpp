
/*
 * wb.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/5/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

extern str_t errtile;

/////////////////////////////////////////////////////////////
///// CWb

void CWb::Init( CStat* _stat,
		CFetchBuf* _fetchbuf,
		CFetch* _fetch,
		CIQueue* _iqueue,
		CComponent* _dtm,
		CDisq* _disq,
		CDisq* _dbdisq,
		CDispatch* _dispatch,
		CIReg* _ireg,
		CSReg* _sreg,
		CRs* _rsiu,
		CRs* _rsbpu,
		CRs* _rslsu,
		CMaq* _maq,
		CRob* _rob,
		CIu* _iu,
		CLsu* _lsu,
		CBpu* _bpu,
		CTrap* _trap )
{
  Stat = _stat;
  FetchBuf = _fetchbuf;
  Fetch = _fetch;
  IQueue = _iqueue;
  Dtm = _dtm;
  Disq = _disq;
  DBDisq = _dbdisq;
  Dispatch = _dispatch;
  IReg = _ireg;
  SReg = _sreg;
  RsIu = _rsiu;
  RsBpu = _rsbpu;
  RsLsu = _rslsu;
  Maq = _maq;
  Rob = _rob;
  Iu = _iu;
  Lsu = _lsu;
  Bpu = _bpu;
  Trap = _trap;

  BB_State = BB_BEGIN;

  WTrace = 0;

#if(DEBUG == 0)
  NTraps = MAX_TRAPS;
#endif
}

CWb::CWb()
  : Trace(EXECTRACE_SIZE),
    STrace(STRACE_BUFSZ)
{
  /* nothing to do */
}

void CWb::Execute(udword_t* _nsteps)
{
  CRobItem *robit = 0;
  uword_t i;

  uword_t regwin;
  uword_t trapnum;

  for(i = 0; i < WB_WIDTH; i++) {
    if(Rob->IsEmpty() || ((robit = Rob->GetReadyItem()) == 0)) break;

    if( robit->IsBadInst() ) {
      sprintf(errtile, "(@%08lX, %08lX)", robit->Addr, robit->Inst);
      errmsg(__IDEC_H, ERR_NOTIMPLEMENTED, errtile);
    }

    if( robit->IsTraceRedundant() ) {     // atualiza os registradores reais a partir do trace
#if(DEBUG == 0)
      fprintf(stderr, "\nWB::EXECUTE(T, addr=%ld, inst=%ld)", robit->Addr, robit->Inst);
#endif

      ((CDtm*) Dtm)->Stage_DS3_TraceReuse(robit);

      //////////////////////////////////////////////////////////////////////
      //// contabiliza estatistica das formas de finalizacao dos traces
      if( !((CDtm*) Dtm)->EmptyBufferT() ) {
	Stat->AddTraceTerm(TFN_TRACE, TSZ_TRACE);
	((CDtm*) Dtm)->BufferT.TraceStats.TFN_traces = TFN_TRACE;
      }
      else {
	if(((CDtm*) Dtm)->BufferT.Tracesz > 0)
	  Stat->AddTraceTerm(TFN_NREDUNDANT, TSZ_NOT_FORMED);
      }
      ((CDtm*) Dtm)->CompleteBufferT();

      Stat->IncICount( (robit->TraceData).Tracesz );
      Stat->IncIReuse( (robit->TraceData).Tracesz );
      Stat->AddTraceSz((robit->TraceData).Tracesz, TSZ_TRACE_REUSE, & (robit->TraceData).TraceStats);
      Stat->AddInstrCount(& (robit->TraceData).TraceStats, IC_TRACE_REUSE);

      if((robit->TraceData).TraceStats.TFN_traces == -1) {
	printf("\n::::OOPS::::");
	(robit->TraceData).Display();
      }
      else {
	Stat->AddTraceTerm((robit->TraceData).TraceStats.TFN_traces, TSZ_TRACE_REUSE);
      }

      Stat->AddTraceFollowingLoad( ((CDtm*) Dtm)->CheckDepMap(robit) );
      ((CDtm*) Dtm)->SetDepMap(robit);
    }
    else {     // atualiza os registradores reais a partir de instrucoes simples
#if(DEBUG == 0)
      fprintf(stderr, "\nWB::EXECUTE(I, addr=%ld, inst=%ld)", robit->Addr, robit->Inst);
#endif

      if( (robit->Rs1).IsReqAsTarget() )
	IReg->SetValue( robit->GetTargetWin(),
			(robit->Rs1).GetRegAddr(),
			(robit->Rs1).GetValue() );
    
      if( (robit->Rs2).IsReqAsTarget() )
	IReg->SetValue( robit->GetTargetWin(),
			(robit->Rs2).GetRegAddr(),
			(robit->Rs2).GetValue() );

      if( (robit->Rd_hi).IsReqAsTarget() )
	IReg->SetValue( robit->GetTargetWin(),
			(robit->Rd_hi).GetRegAddr(),
			(robit->Rd_hi).GetValue() );

      if( (robit->Rd_lo).IsReqAsTarget() )
	IReg->SetValue( robit->GetTargetWin(),
			(robit->Rd_lo).GetRegAddr(),
			(robit->Rd_lo).GetValue() );
    
      if( (robit->Psr).IsReqAsTarget() )
	(SReg->Psr).SetValue( (robit->Psr).GetValue() );

      if( (robit->Y).IsReqAsTarget() )
	(SReg->Y).SetValue( (robit->Y).GetValue() );

      ((CDtm*) Dtm)->SetDepMap(robit);
      ((CDtm*) Dtm)->Stage_DS3_TraceConstruction(robit);

      /////////////////////////////////////////////////
      //// contabilizacao estatisticas das instrucoes executadas

      Stat->IncICount();
      Stat->AddInstrCount(robit->GetExecUnit(), robit->GetInstOpcode());
      if( robit->IsRedundant() ) {
	Stat->IncIReuse( 1 );
	Stat->AddInstrCount( robit->GetExecUnit(), 
			     robit->GetInstOpcode(),
			     1,
			     IC_INST_REUSE );
      }

    }

#if(DEBUG == 0)
    fprintf(stderr, "\nWB::EXECUTE(BEFORE_WRITETRACE)");
#endif
    WriteTrace( robit );
#if(DEBUG == 0)
    fprintf(stderr, "\nWB::EXECUTE(AFTER_WRITETRACE)");
#endif

    if( robit->Trapsig ) {
      regwin = robit->GetTargetWin();
      trapnum = robit->Trapnum;
      FetchBuf->Flush();
      Fetch->SetNewPC(robit->Addr + 4);
      IQueue->Flush();
      Disq->Reset();
      DBDisq->Reset();
      RsIu->Reset();
      RsBpu->Reset();
      RsLsu->Reset();
      Maq->Reset();
      Rob->Reset();
      Iu->Reset();
      Lsu->Reset();
      Bpu->Reset(); 
      IReg->Flush(regwin);
      SReg->Flush();
      Trap->DoCall(regwin, trapnum);

      FetchBuf->SetLastBranchLevel(robit->GetBranchLevel() + 1);

#if(DEBUG == 0)
      printf("WB::EXECUTE(addr=%lx, TRAP)\n", robit->Addr);
      if((NTraps--) == 0)
	exit(1);
#endif
    }
    else {
#if(DEBUG == 0)
      fprintf(stderr, "\nWB::EXECUTE(BEFORE_REMOVE_FIRSTITEM)");
#endif
      Rob->RemoveFirstItem();
#if(DEBUG == 0)
      fprintf(stderr, "\nWB::EXECUTE(AFTER_REMOVE_FIRSTITEM)");
#endif
    }

#if(DEBUG == 0)
    fprintf(stderr, "\nWB::EXECUTE(BEFORE_DEC_NSTEPS)");
#endif

    if(_nsteps != 0) {
      if( robit->IsTraceRedundant() ) {
	if((robit->TraceData).Tracesz >= (*_nsteps)) {
	  (*_nsteps) = 0;
	  return;
	}
	else {
	  (*_nsteps) = (*_nsteps) - (robit->TraceData).Tracesz;
	}
      }
      else {
	if(((*_nsteps) -= 1) == 0) return;
      }
    }

#if(DEBUG == 0)
    fprintf(stderr, "\nWB::EXECUTE(AFTER_DEC_NSTEPS)");
#endif

  }

}

void CWb::WriteTrace(CRobItem* _it)
{
  CAsm tmp;

  str_t s1, s2;
  int len = 0;

  tmp.Set(_it->Addr, _it->Inst);
  tmp.Get(s2, 30);

  sprintf(& s1[len], "%08lx %08lx %s ", _it->Addr, _it->Inst, s2);
  len = strlen(s1);

  sprintf(& s1[len], "[");
  len = strlen(s1);

  if( (_it->Psr).IsReqAsTarget() ) {
    sprintf(& s1[len], "psr=%08lx ", (_it->Psr).GetValue());
    len = strlen(s1);
  }

  if( (_it->Y).IsReqAsTarget() ) {
    sprintf(& s1[len], "y=%08lx ", (_it->Y).GetValue());
    len = strlen(s1);
  }

  if( (_it->Rd_lo).IsReqAsTarget() ) {
    sprintf( & s1[len],
	     "rd%lu=%08lx ",
	     (_it->Rd_lo).GetRegAddr(),
	     (_it->Rd_lo).GetValue() );
    len = strlen(s1);
  }

  if( (_it->Rd_hi).IsReqAsTarget() ) {
    sprintf( & s1[len],
	     "rd%lu=%08lx ",
	     (_it->Rd_hi).GetRegAddr(),
	     (_it->Rd_hi).GetValue() );
    len = strlen(s1);
  }

  sprintf(& s1[len], "]");
  len += strlen(s1);

  sprintf(& s1[len], "  maddr=%lu", _it->MAddr);
  len += strlen(s1);

  Trace.InsertItem(s1);

  if((WTrace & WTRACE_INSTRUCTIONS) != 0)
    ITrace.InsertItem(s1);

  if((WTrace & WTRACE_BASICBLOCKS) != 0)
    WriteBTrace(_it, s1);

  if((WTrace & WTRACE_SIMSPARC) != 0)
    WriteSTrace(_it);

}

void CWb::WriteBTrace(CRobItem* _it, char* s1)
{
  switch( BB_State ) {
  case BB_BEGIN:
    if( _it->IsBranchInst() ) {
      switch( _it->GetBranchType() ) {
      case __BA_ANNUL:
      case __BN_ANNUL:
      case __BICC_ANNUL:
	BTrace.InsertItem(s1);
	BTrace.InsertItem("-------------------------------------------------------------");
	BB_State = BB_BEGIN;
	break;
      case __BA:
      case __BN:
      case __BICC:
      case __CALL:
      case __JUMPL:
	BTrace.InsertItem(s1);
	BB_State = BB_END;
      }
    }
    else {
      BTrace.InsertItem(s1);
      BB_State = BB_BODY;
    }
    break;
  case BB_BODY:
    if( _it->IsBranchInst() ) {
      switch( _it->GetBranchType() ) {
      case __BA_ANNUL:
      case __BN_ANNUL:
      case __BICC_ANNUL:
	BTrace.InsertItem(s1);
	BTrace.InsertItem("-------------------------------------------------------------");
	BB_State = BB_BEGIN;
	break;
      case __BA:
      case __BN:
      case __BICC:
      case __CALL:
      case __JUMPL:
	BB_State = BB_END;
      }
    }
    break;
  case BB_END:
    BTrace.InsertItem(s1);
    BTrace.InsertItem("-------------------------------------------------------------");
    BB_State = BB_BEGIN;
  }
}  

void CWb::WriteSTrace(CRobItem* _it)
{
  uword_t n = 1;

  if( _it->IsTraceRedundant() )
    n = (_it->TraceData).Tracesz;

  if( (_it->Rd_hi).IsReqAsTarget() )
    STrace.InsertItem(_it->Addr, (_it->Rd_hi).GetRegAddr(), (_it->Rd_hi).GetValue(), n);
  else
    STrace.InsertItem(_it->Addr, 0xff, 0xffffffffL, n);
}

void CWb::ShowTrace()
{
  printf("\n");
  printf("\nCompletions Trace");
  printf("\n=================\n");
  printf("\n");
  Trace.Show(TRUE);  
}

void CWb::SetTraceFile(char* _s)
{
  str_t s;

  strcat(strcpy(s, _s), ITRACE_FILE_EXT);
  ITrace.SetTraceFile(s);

  strcat(strcpy(s, _s), BTRACE_FILE_EXT);
  BTrace.SetTraceFile(s);

  strcat(strcpy(s, _s), STRACE_FILE_EXT);
  STrace.SetTraceFile(s);
}

void CWb::EnableTrace(int _wtmask)
{
  WTrace |= _wtmask;

  if((_wtmask & WTRACE_INSTRUCTIONS) != 0)
    ITrace.Open();

  if((_wtmask & WTRACE_BASICBLOCKS) != 0)
    BTrace.Open();

  if((_wtmask & WTRACE_SIMSPARC) != 0)
    STrace.Open();
}

void CWb::DisableTrace(int _wtmask)
{
  WTrace &= ~_wtmask;

  if((_wtmask & WTRACE_INSTRUCTIONS) != 0)
    ITrace.Close();

  if((_wtmask & WTRACE_BASICBLOCKS) != 0)
    BTrace.Close();

  if((_wtmask & WTRACE_SIMSPARC) != 0)
    STrace.Close();
}
