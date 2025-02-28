
/*
 * lsu.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/17/99
 */

#include<stdio.h>
#include"all.h"

////////////////////////////////////////////////////////////
///// CLsu

void CLsu::Init( CDMmu* _dmmu,
		 CFetchBuf* _fetchbuf,
		 CMaq* _maq,
		 CRs* _rslsu,
		 CRob* _rob )
{
  DMmu = _dmmu;
  FetchBuf = _fetchbuf;
  Maq = _maq;
  RsLsu = _rslsu;
  Rob = _rob;
}

CLsu::CLsu(uword_t _nentries)
{
  NEntries = _nentries;
  if((ResBuf = new CResBuf [NEntries]) == 0)
    errmsg(__LSU_H, ERR_CANTALLOCMEM, "");
}

CLsu::~CLsu()
{
  delete [] ResBuf;
}

uword_t CLsu::GetNumUnits()
{
  return NEntries;
}

CResBuf* CLsu::GetNthResBuf(uword_t _idx)
{
  if(_idx < NEntries)
    return & ResBuf[_idx];
  return 0;
}

CMaqItem* CLsu::GetItemFromMaq(CRsItem** _rsit)
{
  CMaqItem* maqit;
  CRsItem* rsit;

  uword_t i;

  if( Maq->IsEmpty() )
    return 0;

  for(i = 0; i < Maq->GetQueueSize(); i++) {

    if( !(maqit = Maq->GetNthItem(i))->IsMAddrReady() ) {

      rsit = RsLsu->GetItem(maqit->GetSeqNum());

      if( rsit->IsReady() ) {
	(*_rsit) = rsit;
	return maqit;
      }

    }

  }

  return 0;
}

CMaqItem* CLsu::GetReadyItemFromMaq(CRsItem** _rsit)
{
  CMaqItem* maqit;

  uword_t i;

  for(i = 0; i < Maq->GetQueueSize(); i++) {
    if( (maqit = Maq->GetNthItem(i))->IsWaiting() ) return 0;
    if( !Rob->BranchNotSolved(maqit->GetSeqNum()) && maqit->IsMAddrReady() ) {
      (*_rsit) = RsLsu->GetItem(maqit->GetSeqNum());
      if( (*_rsit)->IsReady() )
	return maqit; 
    }

  }

  return 0;
}

void CLsu::Step_AddressCalculation()
{
  CMaqItem* maqit;
  CRsItem* rsit = 0;

  if((maqit = GetItemFromMaq(& rsit)) == 0) return;

  // rsit->SetBusy();

  switch(rsit->GetInstOpcode()) {
  case LDSB:
  case LDSH:
  case LDUB:
  case LDUH:
  case LD:
  case LDD:
    if((rsit->fi).i == 0)
      Address = (rsit->Rs1).GetValue() + (rsit->Rs2).GetValue();
    else
      Address = (rsit->Rs1).GetValue() + (rsit->fi).simm13;
    break;

  case STB:
  case STH:
  case ST:
  case STD:
    if((rsit->fi).i == 0)
      Address = (rsit->Rs1).GetValue() + (rsit->Rs2).GetValue();
    else
      Address = (rsit->Rs1).GetValue() + (rsit->fi).simm13;
    break;

  case LDSTUB:
    if((rsit->fi).i == 0)
      Address = (rsit->Rs1).GetValue() + (rsit->Rs2).GetValue();
    else
      Address = (rsit->Rs1).GetValue() + (rsit->fi).simm13;
    break;

  case SWAP:
    if((rsit->fi).i == 0)
      Address = (rsit->Rs1).GetValue() + (rsit->Rs2).GetValue();
    else
      Address = (rsit->Rs1).GetValue() + (rsit->fi).simm13;

  }

  maqit->SetMAddr(Address);

}

void CLsu::Step_MemoryAccess(CResBuf* _rb)
{
  CMaqItem* maqit;
  CRsItem* rsit;

  uword_t tmp1, tmp2;

  _rb->Reset();

  if((maqit = GetReadyItemFromMaq(& rsit)) == 0) return;

  if( !rsit->IsTraceRedundant() ) {

    switch(rsit->GetInstOpcode()) {
    case LDSB:
    case LDSH:
    case LDUB:
    case LDUH:
    case LD:
    case LDD:
      Result1 = 0;
      Result2 = 0;
      break;

    case STB:
    case STH:
    case ST:
    case STD:
      Result1 = (rsit->Rd_hi).GetValue();
      if(rsit->GetInstOpcode() == STD)
	Result2 = (rsit->Rd_lo).GetValue();
      break;

    case LDSTUB:
      Result1 = (rsit->Rd_hi).GetValue() & 0x0FF;
      break;

    case SWAP:
      Result1 = (rsit->Rd_hi).GetValue();
    }

    if(rsit->RType != RD_NONE) {
      DMmu->MemRead( rsit->RType,
		     maqit->MAddr,
		     tmp1,
		     tmp2 );
      
      if( (rsit->fi).rd == 0 ) tmp1 = 0;
      
    }
    
    if(rsit->WType != WR_NONE) {
      DMmu->MemWrite( rsit->WType,
		      maqit->MAddr,
		      Result1,
		      Result2 );
    }
    
    if(rsit->RType != RD_NONE) {
      Result1 = tmp1;
      Result2 = tmp2;
    }
    
  }
  else {

#if(DEBUG <= 14)
	printf( "\nLSU::MEMORY_ACCESS(seq=%ld, addr=%lx, rtype=%lx, wtype=%lx, maddr=%lx, reqsrc=%d)\n",
		rsit->GetSeqNum(),
		(rsit->TraceData).PC,
		(rsit->TraceData).RType,
		(rsit->TraceData).WType,
		(rsit->TraceData).MAddr,
		(rsit->TraceData).Rd_hi.IsReqAsSource() );
	(rsit->TraceData).Display();
	getchar();
#endif

    if( (rsit->TraceData).Rd_hi.IsReqAsSource() )
      Result1 = (rsit->TraceData).Rd_hi.GetValue();
    else
      Result1 = 0;

    if( (rsit->TraceData).Rd_lo.IsReqAsSource() )
      Result2 = (rsit->TraceData).Rd_lo.GetValue();
    else
      Result2 = 0;

    if((rsit->TraceData).RType != RD_NONE) {
      DMmu->MemRead( (rsit->TraceData).RType,
		     (rsit->TraceData).MAddr,
		     tmp1,
		     tmp2 );
      
      if( (rsit->TraceData).Rd_hi.GetRegAddr() == 0 ) tmp1 = 0;
    }

    if((rsit->TraceData).WType != WR_NONE) {
      DMmu->MemWrite( (rsit->TraceData).WType,
		      (rsit->TraceData).MAddr,
		      Result1,
		      Result2 );

#if(DEBUG <= 14)
      if(rsit->Addr == 0xa4fc) {
	fprintf( stderr,
		 "\nLSU::MEMORY_ACCESS(wtype=%lx, M[%lx]<-%lx)\n", 
		 (rsit->TraceData).WType,
		 (rsit->TraceData).MAddr,
		 Result1 );
	getchar();
      }
#endif

    }

    if( (rsit->TraceData).RType != RD_NONE ) {
      (rsit->TraceData).Rd_hi.SetValue((rsit->TraceData).Rd_hi.GetRegAddr(), tmp1);
      if( (rsit->TraceData).Rd_lo.IsReqAsTarget() )
	(rsit->TraceData).Rd_lo.SetValue((rsit->TraceData).Rd_lo.GetRegAddr(), tmp2);
    }

    Result1 = 0;
    Result2 = 0;

  }

  _rb->Set( rsit,
	    0,
	    Result1,
	    Result2,
	    0,
	    0,
	    FALSE,
	    0,
	    FALSE,
	    0,
	    maqit->MAddr );

  Maq->RemoveItem(rsit->GetSeqNum());

}

void CLsu::Execute()
{

  for(uword_t i = 0; i < NEntries; i++)
    Step_MemoryAccess(& ResBuf[i]);

  for(uword_t i = 0; i < NEntries; i++)
    Step_AddressCalculation();

  Maq->EvalInstState();

}

void CLsu::Flush(uword_t _blevel)
{
  for(uword_t i = 0; i < NEntries; i++) {
    if( ResBuf[i].IsValid() && ((ResBuf[i].Rsit)->GetBranchLevel() > _blevel) )
      ResBuf[i].Reset();
  }
}

void CLsu::Reset()
{
  for(uword_t i = 0; i < NEntries; i++)
    ResBuf[i].Reset();
}

void CLsu::Show()
{
  CResBuf* rb;

  CRsItem* p;
  CAsm it;

  str_t s1;

  printf("\nLSU");
  printf("\n===\n");

  for(uword_t i = 0; i < GetNumUnits(); i++) {

    if( !(rb = & ResBuf[i])->IsValid() ) {
      printf("\n%2lu) *** NOT_VALID ***", i);
      continue;
    }
 
    p = rb->Rsit;

    it.Set(p->Addr, p->Inst);
    it.Get(s1, 22);

    printf("\n%2lu) ", i);

    printf(  "%2lu %2lu %08lx %08lx %s ", p->Seq, p->BLevel, p->Addr, p->Inst, s1);

    printf("[ ");

    if( (p->Psr).IsReqAsTarget() ) {
      if( (p->Psr).IsReady() )
	printf("psr=%lu ", (p->Psr).GetValue());
      else
	printf("(psr=?) ");
    }

    if( (p->Y).IsReqAsTarget() ) {
      if( (p->Y).IsReady() )
	printf("y=%lu ", (p->Y).GetValue());
      else
	printf("(y=?) ");
    }

    if( !p->IsTraceRedundant() ) {

      if( (p->Rd_lo).IsReqAsTarget() ) {
	if( (p->Rd_lo).IsReady() )
	  printf("rd%lu=%lu ", (p->Rd_lo).GetRegAddr(), (p->Rd_lo).GetValue());
	else
	  printf("(rd%lu) ", (p->Rd_lo).GetRegAddr());
      }
    
      if( (p->Rd_hi).IsReqAsTarget() ) {
	if( (p->Rd_hi).IsReady() )
	  printf("rd%lu=%lu ", (p->Rd_hi).GetRegAddr(), (p->Rd_hi).GetValue());
	else
	  printf("(rd%lu) ", (p->Rd_hi).GetRegAddr());
      }

    }
    else {

      if( (p->TraceData).Rd_lo.IsReqAsTarget() ) {
	if( (p->TraceData).Rd_lo.IsReady() )
	  printf( "rd%lu=%lu ",
		  (p->TraceData).Rd_lo.GetRegAddr(),
		  (p->TraceData).Rd_lo.GetValue() );
	else
	  printf("(rd%lu) ", (p->TraceData).Rd_lo.GetRegAddr());
      }
    
      if( (p->TraceData).Rd_hi.IsReqAsTarget() ) {
	if( (p->TraceData).Rd_hi.IsReady() )
	  printf( "rd%lu=%lu ",
		  (p->TraceData).Rd_hi.GetRegAddr(),
		  (p->TraceData).Rd_hi.GetValue() );
	else
	  printf("(rd%lu) ", (p->TraceData).Rd_hi.GetRegAddr());
      }

    }

    printf("]");

    if( p->IsTraceRedundant() )
      (p->TraceData).Display();

  }

  printf("\n");

}
