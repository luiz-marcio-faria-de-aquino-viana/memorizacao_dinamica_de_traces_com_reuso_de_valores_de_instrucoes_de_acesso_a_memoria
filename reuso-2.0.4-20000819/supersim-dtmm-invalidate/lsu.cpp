
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

bool_t CLsu::HaveStorePending()
{
  CMaqItem* p;
  uword_t i;

  if( Maq->IsEmpty() ) return FALSE;

  for(i = 0; i < Maq->GetQueueSize(); i++) {
    p = Maq->GetNthItem(i);
    if( p->IsTraceRedundant() ) {
      if( (p->TraceData).CheckForLoadStoreInst(IOM_STORE) )
	return TRUE;
    }
    else {
      if(p->WType != WR_NONE)
	return TRUE;
    }
  }

  return FALSE;
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

  uword_t tmp1 = 0, tmp2 = 0;
  int i;

  _rb->Reset();

  if((maqit = GetReadyItemFromMaq(& rsit)) == 0) return;

  if( !rsit->IsTraceRedundant() ) {
    /* se instrucao NAO e um trace redundante */

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

#if(DEBUG <= 18)
      if(maqit->MAddr == 0x120a4) {
	fprintf( stderr, 
		 "LSU::STEP_MEMORY_ACCESS(LD - seq=%lu, addr=%lx, rtype=%lx, maddr=%lx, hi=%lx, lo=%lx)\n",
		 rsit->GetSeqNum(),
		 rsit->Addr,
		 rsit->RType,
		 maqit->MAddr,
		 tmp1,
		 tmp2 );
      }
#endif

    }
    
    if(rsit->WType != WR_NONE) {
      DMmu->MemWrite( rsit->WType,
		      maqit->MAddr,
		      Result1,
		      Result2 );
#if(DEBUG <= 18)
      if(maqit->MAddr == 0x120a4) {
	fprintf( stderr, 
		 "LSU::STEP_MEMORY_ACCESS(ST - seq=%lu, addr=%lx, wtype=%lx, maddr=%lx, hi=%lx, lo=%lx)\n",
		 rsit->GetSeqNum(),
		 rsit->Addr,
		 rsit->WType,
		 maqit->MAddr,
		 Result1,
		 Result2 );
      }
#endif

      // invalida instrucoes ja executadas que leem o mesmo endereco de memoria
      Rob->InvMemVal( rsit->GetSeqNum(), 
		      rsit->WType, 
		      maqit->MAddr );

    }
    
    if(rsit->RType != RD_NONE) {
      Result1 = tmp1;
      Result2 = tmp2;
    }
    
  }
  else {

    /* instrucao e um trace redundante */

    for(i = 0; i < (rsit->TraceData).LdStSz; i++) {

      if( ((rsit->TraceData).LdStInst[i].LdSt & IOM_STORE) != 0 ) {
	// traces com instrucoes de escrita

	if( (rsit->TraceData).LdStInst[i].Rdhi.IsReqAsSource() )
	  Result1 = (rsit->TraceData).LdStInst[i].MValhi;
	else
	  Result1 = 0;
	
	if( (rsit->TraceData).LdStInst[i].Rdlo.IsReqAsSource() )
	  Result2 = (rsit->TraceData).LdStInst[i].MVallo;
	else
	  Result2 = 0;

	DMmu->MemWrite( (rsit->TraceData).LdStInst[i].Type,
			(rsit->TraceData).LdStInst[i].MAddr,
			Result1,
			Result2 );

	// invalida instrucoes ja executadas que leem o mesmo endereco de memoria
	Rob->InvMemVal( rsit->GetSeqNum(), 
			(rsit->TraceData).LdStInst[i].Type, 
			(rsit->TraceData).LdStInst[i].MAddr );

      }
      else if( ((rsit->TraceData).LdStInst[i].LdSt & IOM_LOAD) != 0 ) {
	// trace com instrucao de leitura
	// obs: trace com instrucao de leitura e executado se (mvalid == false)

	DMmu->MemRead( (rsit->TraceData).LdStInst[i].Type,
		       (rsit->TraceData).LdStInst[i].MAddr,
		       tmp1,
		       tmp2 );
	
	if( (rsit->TraceData).LdStInst[i].Rdhi.GetRegAddr() == 0 ) tmp1 = 0;

	// obs: em condicoes normais o codigo a seguir sera executado no maximo uma vez

	(rsit->TraceData).LdStInst[i].Rdhi.SetValue( (rsit->TraceData).LdStInst[i].Rdhi.GetRegAddr(),
						     tmp1 );
	(rsit->TraceData).LdStInst[i].MValhi = tmp1;
	// obs: esta operacao aparenta ser redundante mas e necessaria pois para reuso de traces com valores validos
	// os campos de valores sao utilizados para atualizar o banco de registradores quando a instrucao sai do pipe

	if( (rsit->TraceData).LdStInst[i].Rdlo.IsReqAsTarget() ) {
	  (rsit->TraceData).LdStInst[i].Rdlo.SetValue( (rsit->TraceData).LdStInst[i].Rdlo.GetRegAddr(),
						       tmp2 );
	  (rsit->TraceData).LdStInst[i].MVallo = tmp2;
	  // obs: novamente a operacao aparentemente redundante atuando sobre o valor menos significativo lido
	}

      }
    
      Result1 = 0;
      Result2 = 0;

    }
    
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
  uword_t i;

  printf("\nLSU");
  printf("\n===\n");

  for(i = 0; i < GetNumUnits(); i++) {

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

      // apresenta as entradas de leitura/escrita a memoria dos traces

      for(i = 0; i < (uword_t) (p->TraceData).LdStSz; i++) {

	if( ((p->TraceData).LdStInst[i].LdSt & IOM_LOAD) == IOM_LOAD ) {

	  // instrucoes de leitura

	  if( (p->TraceData).LdStInst[i].Rdlo.IsReqAsTarget() ) {
	    if( (p->TraceData).LdStInst[i].Rdlo.IsReady() )
	      printf( "rd%lu=%lu ",
		      (p->TraceData).LdStInst[i].Rdlo.GetRegAddr(),
		      (p->TraceData).LdStInst[i].Rdlo.GetValue() );
	    else
	      printf("(rd%lu) ", (p->TraceData).LdStInst[i].Rdlo.GetRegAddr());
	  }
	
	  if( (p->TraceData).LdStInst[i].Rdhi.IsReqAsTarget() ) {
	    if( (p->TraceData).LdStInst[i].Rdhi.IsReady() )
	      printf( "rd%lu=%lu ",
		      (p->TraceData).LdStInst[i].Rdhi.GetRegAddr(),
		      (p->TraceData).LdStInst[i].Rdhi.GetValue() );
	    else
	      printf("(rd%lu) ", (p->TraceData).LdStInst[i].Rdhi.GetRegAddr());
	  }

	}
	else if( ((p->TraceData).LdStInst[i].LdSt & IOM_STORE) == IOM_STORE ) {

	  // instrucoes de escrita

	  if( (p->TraceData).LdStInst[i].Rdlo.IsReqAsTarget() ) {
	    if( (p->TraceData).LdStInst[i].Rdlo.IsReady() )
	      printf( "rd%lu=%lu ",
		      (p->TraceData).LdStInst[i].Rdlo.GetRegAddr(),
		      (p->TraceData).LdStInst[i].Rdlo.GetValue() );
	    else
	      printf("(rd%lu) ", (p->TraceData).LdStInst[i].Rdlo.GetRegAddr());
	  }
	
	  if( (p->TraceData).LdStInst[i].Rdhi.IsReqAsTarget() ) {
	    if( (p->TraceData).LdStInst[i].Rdhi.IsReady() )
	      printf( "rd%lu=%lu ",
		      (p->TraceData).LdStInst[i].Rdhi.GetRegAddr(),
		      (p->TraceData).LdStInst[i].Rdhi.GetValue() );
	    else
	      printf("(rd%lu) ", (p->TraceData).LdStInst[i].Rdhi.GetRegAddr());
	  }

	}

      }

    }

    printf("]");

    if( p->IsTraceRedundant() )
      (p->TraceData).Display();

  }

  printf("\n");

}
