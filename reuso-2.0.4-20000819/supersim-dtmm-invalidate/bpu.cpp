
/*
 * bpu.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/17/99
 */

#include<stdio.h>
#include"all.h"

extern str_t errtile;

////////////////////////////////////////////////////////////
///// CBpu

void CBpu::Init( CBtb* _btb, 
		 CFetchBuf* _fetchbuf,
		 CIQueue* _iqueue,
		 CFetch* _fetch,
		 CDisq* _disq,
		 CDisq* _dbdisq,
		 CIReg* _ireg,
		 CSReg* _sreg,
		 CRs* _rsiu,
		 CRs* _rsbpu,
		 CRs* _rslsu,
		 CMaq* _maq,
		 CRob* _rob,
		 CIu* _iu, 
		 CLsu* _lsu )
{
  Btb = _btb;
  FetchBuf = _fetchbuf;
  IQueue = _iqueue;
  Fetch = _fetch;
  Disq = _disq;
  DBDisq = _dbdisq;
  IReg = _ireg;
  SReg = _sreg;
  RsIu = _rsiu;
  RsBpu = _rsbpu;
  RsLsu = _rslsu;
  Maq = _maq;
  Rob = _rob;
  Iu = _iu;
  Lsu = _lsu;
}

CBpu::CBpu(uword_t _nentries)
{
  if((ResBuf = new CResBuf [(NEntries = _nentries)]) == 0)
    errmsg(__BPU_H, ERR_CANTALLOCMEM, "");
}

CBpu::~CBpu()
{
  delete [] ResBuf;
}

uword_t CBpu::GetNumUnits()
{
  return NEntries;
}

CResBuf* CBpu::GetNthResBuf(uword_t _idx)
{
  if(_idx < NEntries)
    return & ResBuf[_idx];
  return 0;
}

void CBpu::Step(CResBuf* _rb)
{
  CRsItem* rsit = 0;
  CDisqItem* di;

  CRobItem* robit;
  CRobItem* p;

  uword_t i, j;

  _rb->Reset();

  if( RsBpu->IsEmpty() || ((rsit = RsBpu->GetReadyItem()) == 0) ) return;
  
#if(DEBUG == 0)
  printf( "\n\tBPU::STEP(seq=%lu, addr=%lx, inst=%lx)",
	  rsit->GetSeqNum() % MAX_SEQNUM,
	  rsit->Addr,
	  rsit->Inst );
#endif

  rsit->SetBusy();

  Trapsig = FALSE;
  BResult = NTAKEN;

  switch(rsit->GetInstOpcode()) {
  case CALL:
    BResult = TAKEN;
    NewPC = rsit->Addr + ((rsit->fi).disp30 << 2);
    Result1 = rsit->Addr;
    FetchBuf->SetLastBranchLevel(rsit->GetBranchLevel() + 1);
    break;

  case BICC:
    switch((rsit->fi).cond) {
    case BA:
      BResult = TAKEN;
      FetchBuf->SetLastBranchLevel(rsit->GetBranchLevel() + 1);
      break;

    case BN:
      BResult = NTAKEN;
      FetchBuf->SetLastBranchLevel(rsit->GetBranchLevel() + 1);
      break;

    case BNE:
      if( ICC_Z((rsit->Psr).GetValue() ) == 0) {
	BResult = TAKEN;
#if(DEBUG == 0)
	printf("\nBPU::STEP(%lx, bne, <BT>)", rsit->Addr);
#endif
      }
      else {
#if(DEBUG == 0)
	printf("\nBPU::STEP(%lx, bne, <BN>)", rsit->Addr);
#endif
      }
      break;

    case BE:
      if( ICC_Z((rsit->Psr).GetValue() ) == 1)
	BResult = TAKEN;
      break;

    case BG:
      if( ( ICC_Z((rsit->Psr).GetValue()) || ( ICC_N((rsit->Psr).GetValue()) ^ ICC_V((rsit->Psr).GetValue()) ) ) == 0)
	BResult = TAKEN;
      break;

    case BLE:
      if( ( ICC_Z((rsit->Psr).GetValue()) || ( ICC_N((rsit->Psr).GetValue()) ^ ICC_V((rsit->Psr).GetValue()) ) ) == 1)
	BResult = TAKEN;
      break;

    case BGE:
      if( ( ICC_N((rsit->Psr).GetValue()) ^ ICC_V((rsit->Psr).GetValue()) ) == 0)
	BResult = TAKEN;
      break;

    case BL:
      if( ( ICC_N((rsit->Psr).GetValue()) ^ ICC_V((rsit->Psr).GetValue()) ) == 1)
	BResult = TAKEN;
      break;

    case BGU:
      if( (ICC_C((rsit->Psr).GetValue()) == 0) && (ICC_Z((rsit->Psr).GetValue()) == 0) )
	BResult = TAKEN;
      break;

    case BLEU:
      if( (ICC_C((rsit->Psr).GetValue()) == 1) || (ICC_Z((rsit->Psr).GetValue()) == 1) )
	BResult = TAKEN;
      break;

    case BCC:
      if(ICC_C((rsit->Psr).GetValue()) == 0)
	BResult = TAKEN;
      break;

    case BCS:
      if(ICC_C((rsit->Psr).GetValue()) == 1)
	BResult = TAKEN;
      break;

    case BPOS:
      if(ICC_N((rsit->Psr).GetValue()) == 0)
	BResult = TAKEN;
      break;

    case BNEG:
      if(ICC_N((rsit->Psr).GetValue()) == 1)
	BResult = TAKEN;
      break;

    case BVC:
      if(ICC_V((rsit->Psr).GetValue()) == 0)
	BResult = TAKEN;
      break;

    case BVS:
      if(ICC_V((rsit->Psr).GetValue()) == 1)
	BResult = TAKEN;
    }

    if(BResult == TAKEN)
      NewPC = rsit->Addr + ((rsit->fi).disp22 << 2);
    else
      NewPC = rsit->Addr + 8;
    break;

  case JMPL:
    Operand1 = (rsit->Rs1).GetValue();
    if((rsit->fi).i == 0)
      Operand2 = (rsit->Rs2).GetValue();
    else
      Operand2 = (rsit->fi).simm13;
    if((rsit->fi).rd != 0)
      Result1 = rsit->Addr;
    else
      Result1 = 0;
    BResult = TAKEN;
    NewPC = Operand1 + Operand2;
    break;

  case TICC:
    if((rsit->fi).cond == TA) {
      Operand1 = (rsit->Rs1).GetValue();
      if((rsit->fi).i == 0)
	Operand2 = (rsit->Rs2).GetValue();
      else
	Operand2 = (rsit->fi).simm13;
      Trapsig = TRUE;
      Trapnum = (Operand1 + Operand2) & 0x7FL;
    }
    else {
      sprintf(errtile, "(trap=%lx)", rsit->Inst);
      errmsg(__BPU_H, ERR_NOTIMPLEMENTED, errtile);
    }
    break;
  }

  rsit->BResult = BResult;   // atualiza o resultado do desvio para o DTM

  // se instrucao de desvio condicional ou jump entao confirma a previsao

  if( (rsit->GetBranchType() == __BICC) ||
      (rsit->GetBranchType() == __BICC_ANNUL) ||
      (rsit->GetBranchType() == __JUMPL) ) {
      
    if( ((BResult == NTAKEN) && (rsit->BPred == NTAKEN)) ||
	((BResult == TAKEN) && (rsit->BPred == TAKEN) && (NewPC == rsit->BAddr)) ) {

#if(DEBUG == 0)
      printf("\nBPU::BRANCH_SUCCESS(addr=%lx)", rsit->Addr);
#endif

      Btb->Update(rsit->Addr, & rsit->BPred, & rsit->BAddr);
#if(DEBUG == 0)
      printf("\nBPU::BRANCH_SUCCESS(BTB::UPDATE)");
#endif

      uword_t blevel;
      blevel = rsit->GetBranchLevel();
#if(DEBUG == 0)
      printf("\nBPU::BRANCH_SUCCESS(RS::GET_BRANCH_LEVEL)");
#endif
      FetchBuf->SetLastBranchLevel(blevel + 1);
#if(DEBUG == 0)
      printf("\nBPU::BRANCH_SUCCESS(FETCHBUF::SET_LAST_BRANCH_LEVEL)");
#endif

      Disq->RemoveBranchDepItem(rsit->GetSeqNum());
#if(DEBUG == 0)
      printf("\nBPU::BRANCH_SUCCESS(DISQ::REMOVE_BRANCH_DEP_ITEM)");
#endif

      DBDisq->RemoveBranchDepItem(rsit->GetSeqNum());
#if(DEBUG == 0)
      printf("\nBPU::BRANCH_SUCCESS(DBDISQ::REMOVE_BRANCH_DEP_ITEM)");
#endif

    }
    else {

#if(DEBUG == 0)
      printf("\nBPU::BRANCH_FAIL(addr=%lx,oPC=%lx,nPC=%lx)", rsit->Addr, rsit->NewPC, NewPC);
#endif

      if(rsit->GetBranchType() == __JUMPL)
	Btb->Update(rsit->Addr, & rsit->BPred, & NewPC);
      else if(rsit->BPred == TAKEN)
	Btb->Update(rsit->Addr, & (rsit->BPred = NTAKEN), & NewPC);
      else
	Btb->Update(rsit->Addr, & (rsit->BPred = TAKEN), & NewPC);

      IQueue->Flush();
      Disq->Flush(rsit->GetBranchLevel(), rsit->GetSeqNum());
      DBDisq->Flush(rsit->GetBranchLevel(), rsit->GetSeqNum());
      RsIu->Flush(rsit->GetBranchLevel());
      RsLsu->Flush(rsit->GetBranchLevel());
      RsBpu->Flush(rsit->GetBranchLevel());
      Maq->Flush(rsit->GetBranchLevel());
      Rob->Flush(rsit->GetBranchLevel());
      Iu->Flush(rsit->GetBranchLevel());
      Lsu->Flush(rsit->GetBranchLevel());
      Flush(rsit->GetBranchLevel());

#if(DEBUG == 0)
      printf( "\nBPU::LAST_IN_ROB(seq=%lu, addr=%lx, inst=%lx)",
	      (Rob->GetLastItem())->GetSeqNum() % MAX_SEQNUM,
	      (Rob->GetLastItem())->Addr,
	      (Rob->GetLastItem())->Inst );

      if( !Disq->IsEmpty() )
	printf( "\nBPU::LAST_IN_DISQ(seq=%lu, addr=%lx, inst=%lx)",
		(Disq->GetLastItem())->GetSeqNum() % MAX_SEQNUM,
		(Disq->GetLastItem())->Addr,
		(Disq->GetLastItem())->Inst );

      if( !DBDisq->IsEmpty() )
	printf( "\nBPU::LAST_IN_DBDISQ(seq=%lu, addr=%lx, inst=%lx)",
		(DBDisq->GetLastItem())->GetSeqNum() % MAX_SEQNUM,
		(DBDisq->GetLastItem())->Addr,
		(DBDisq->GetLastItem())->Inst );
#endif

      robit = Rob->GetLastItem();

      IReg->Flush(robit->GetTargetWin(), robit->GetSeqNum());

      SReg->Flush(robit->GetSeqNum());

      /* necessario devido as instrucoes no delay slot */

      for(j = 0; j < Rob->GetQueueSize(); j++) {

	p = Rob->GetNthItem(j);

	if( p->IsReady() ) {

	  // restaura resultado das instrucoes prontas e...

	  if( p->IsTraceRedundant() ) {

	    // ...das instrucoes que iniciam traces

	    for(i = 0; i < OC_NUM; i++) {
	      switch( (p->TraceData).Oc[i].regtype ) {
	      case CTX_PSR:
		(SReg->Psr).SetBusy(p->GetSeqNum());
		(SReg->Psr).SetFValue((p->TraceData).Oc[i].regval);
		(SReg->Psr).ClearBusy();
		break;
	      case CTX_Y:
		(SReg->Y).SetBusy(p->GetSeqNum());
		(SReg->Y).SetFValue((p->TraceData).Oc[i].regval);
		(SReg->Y).ClearBusy();
		break;
	      case CTX_GPR:
		IReg->SetBusy( p->GetTargetWin(),
			       (p->TraceData).Oc[i].regaddr,
			       p->GetSeqNum());
		IReg->SetFValue( p->GetTargetWin(),
				 (p->TraceData).Oc[i].regaddr,
				 (p->TraceData).Oc[i].regval);
		IReg->ClearBusy( p->GetTargetWin(),
				 (p->TraceData).Oc[i].regaddr);
	      };
	    }

	    /* restaura resultado dos registradores destino do acesso a memoria */

	    for(i = 0; i < (uword_t) (p->TraceData).LdStSz; i++) {

	      if( ((p->TraceData).LdStInst[i].LdSt & IOM_LOAD) != 0 ) {

		if( (p->TraceData).LdStInst[i].Rdhi.IsReqAsTarget() ) {
		  IReg->SetBusy( p->GetTargetWin(),
				 (p->TraceData).LdStInst[i].Rdhi.GetRegAddr(),
				 p->GetSeqNum() );
		  IReg->SetFValue( p->GetTargetWin(),
				   (p->TraceData).LdStInst[i].Rdhi.GetRegAddr(),
				   (p->TraceData).LdStInst[i].MValhi );
		  IReg->ClearBusy( p->GetTargetWin(),
				   (p->TraceData).LdStInst[i].Rdhi.GetRegAddr() );
		}

		if( (p->TraceData).LdStInst[i].Rdlo.IsReqAsTarget() ) {
		  IReg->SetBusy( p->GetTargetWin(),
				 (p->TraceData).LdStInst[i].Rdlo.GetRegAddr(),
				 p->GetSeqNum() );
		  IReg->SetFValue( p->GetTargetWin(),
				   (p->TraceData).LdStInst[i].Rdlo.GetRegAddr(),
				   (p->TraceData).LdStInst[i].MVallo );
		  IReg->ClearBusy( p->GetTargetWin(),
				   (p->TraceData).LdStInst[i].Rdlo.GetRegAddr() );
		}

	      }

	    }

	  }
	  else {

	    // ...das instrucoes simples

	    if( (p->Rs1).IsReqAsTarget() ) {
	      IReg->SetFValue( p->GetTargetWin(),
			       (p->Rs1).GetRegAddr(),
			       (p->Rs1).GetValue() );

	      IReg->ClearBusy( p->GetTargetWin(),
			       (p->Rs1).GetRegAddr() );
	    }

	    if( (p->Rs2).IsReqAsTarget() ) {
	      IReg->SetFValue( p->GetTargetWin(),
			       (p->Rs2).GetRegAddr(),
			       (p->Rs2).GetValue() );
	      
	      IReg->ClearBusy( p->GetTargetWin(),
			       (p->Rs2).GetRegAddr() );
	    }

	    if( (p->Rd_hi).IsReqAsTarget() ) {
	      IReg->SetFValue( p->GetTargetWin(),
			       (p->Rd_hi).GetRegAddr(),
			       (p->Rd_hi).GetValue() );
	      
	      IReg->ClearBusy( p->GetTargetWin(),
			       (p->Rd_hi).GetRegAddr() );
	    }	  

	    if( (p->Rd_lo).IsReqAsTarget() ) {
	      IReg->SetFValue( p->GetTargetWin(),
			       (p->Rd_lo).GetRegAddr(),
			       (p->Rd_lo).GetValue() );

	      IReg->ClearBusy( p->GetTargetWin(),
			       (p->Rd_lo).GetRegAddr() );
	    }

	    if( (p->Psr).IsReqAsTarget() ) {
	      (SReg->Psr).SetFValue( (p->Psr).GetValue() );
	      (SReg->Psr).ClearBusy();
	    }

	    if( (p->Y).IsReqAsTarget() ) {
	      (SReg->Y).SetFValue( (p->Y).GetValue() );
	      (SReg->Y).ClearBusy();
	    }

	  }

	}
	else {

	  // ativa estado de ocupado dos registradores das instrucoes NAO prontas e que...

	  if( p->IsTraceRedundant() ) {

	    // ...iniciam traces que possuem instrucoes de leitura da memoria

	    /* atualiza contexto de saida */

	    for(i = 0; i < OC_NUM; i++) {
	      switch( (p->TraceData).Oc[i].regtype ) {
	      case CTX_PSR:
		(SReg->Psr).SetBusy(p->GetSeqNum());
		(SReg->Psr).SetFValue((p->TraceData).Oc[i].regval);
		(SReg->Psr).ClearBusy();
		break;
	      case CTX_Y:
		(SReg->Y).SetBusy(p->GetSeqNum());
		(SReg->Y).SetFValue((p->TraceData).Oc[i].regval);
		(SReg->Y).ClearBusy();
		break;
	      case CTX_GPR:
		IReg->SetBusy( p->GetTargetWin(),
			       (p->TraceData).Oc[i].regaddr,
			       p->GetSeqNum());
		IReg->SetFValue( p->GetTargetWin(),
				 (p->TraceData).Oc[i].regaddr,
				 (p->TraceData).Oc[i].regval);
		IReg->ClearBusy( p->GetTargetWin(),
				 (p->TraceData).Oc[i].regaddr);
	      };
	    }

	    /* marca ocupado registradores destino do acesso a memoria */

	    for(i = 0; i < (uword_t) (p->TraceData).LdStSz; i++) {

	      if( ((p->TraceData).LdStInst[i].LdSt & IOM_LOAD) != 0 ) {

		if( (p->TraceData).LdStInst[i].Rdhi.IsReqAsTarget() )
		  IReg->SetBusy( p->GetTargetWin(),
				 (p->TraceData).LdStInst[i].Rdhi.GetRegAddr(),
				 p->GetSeqNum() );

		if( (p->TraceData).LdStInst[i].Rdlo.IsReqAsTarget() )
		  IReg->SetBusy( p->GetTargetWin(),
				 (p->TraceData).LdStInst[i].Rdlo.GetRegAddr(),
				 p->GetSeqNum() );

	      }

	    }

	  }
	  else {

	    // ...sao instrucoes simples

	    if( (p->Rs1).IsReqAsTarget() )
	      IReg->SetBusy( p->GetTargetWin(),
			     (p->Rs1).GetRegAddr(),
			     p->GetSeqNum() );
	    
	    if( (p->Rs2).IsReqAsTarget() )
	      IReg->SetBusy( p->GetTargetWin(),
			     (p->Rs2).GetRegAddr(),
			     p->GetSeqNum() );
	    
	    if( (p->Rd_hi).IsReqAsTarget() )
	      IReg->SetBusy( p->GetTargetWin(),
			     (p->Rd_hi).GetRegAddr(),
			     p->GetSeqNum() );
	    
	    if( (p->Rd_lo).IsReqAsTarget() )
	      IReg->SetBusy( p->GetTargetWin(),
			     (p->Rd_lo).GetRegAddr(),
			     p->GetSeqNum() );
	    
	    if( (p->Psr).IsReqAsTarget() )
	      (SReg->Psr).SetBusy( p->GetSeqNum() );
	    
	    if( (p->Y).IsReqAsTarget() )
	      (SReg->Y).SetBusy( p->GetSeqNum() );

	  }

	}

      }

      if( !Disq->IsEmpty() )
	di = Disq->GetLastItem();
      else if( !DBDisq->IsEmpty() )
	di = DBDisq->GetLastItem();
      else
	di = robit;

      if(di->GetSeqNum() < robit->GetSeqNum())
	di = robit;

      FetchBuf->SetLastBranchLevel(rsit->GetBranchLevel() + 1);

      FetchBuf->Flush(di->GetSeqNum(), rsit->GetBranchLevel());

      Fetch->SetNewPC(NewPC);

      _rb->Set( rsit,
		0,
		Result1,
		0,
		0,
		0,
		Trapsig,
		Trapnum,
		TRUE,
		NewPC );

      return;
    }
  }
  
  _rb->Set( rsit,
	    0,
	    Result1,
	    0,
	    0,
	    0,
	    Trapsig,
	    Trapnum,
	    FALSE,
	    NewPC );

#if(DEBUG == 0)
  printf("\n\tBPU::STEP(END)");
#endif

}

void CBpu::Execute()
{
  for(uword_t i = 0; i < NEntries; i++) {
#if(DEBUG == 0)
    printf( "\nBPU::EXECUTE(%lu)", i);
#endif
    Step(& ResBuf[i]);
  }
}

void CBpu::Flush(uword_t _blevel)
{
  for(uword_t i = 0; i < NEntries; i++)
    if( ResBuf[i].IsValid() && ((ResBuf[i].Rsit)->GetBranchLevel() > _blevel) )
      ResBuf[i].Reset();
}

void CBpu::Reset()
{
  for(uword_t i = 0; i < NEntries; i++)
    ResBuf[i].Reset();
}

void CBpu::Show()
{
  CResBuf* rb;

  CRsItem* p;
  CAsm it;

  str_t s1;

  printf("\nBPU");
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

    printf(  "%2lu %2lu %08lx %08lx %s ", p->Seq % MAX_SEQNUM, p->BLevel, p->Addr, p->Inst, s1);

    if( p->IsBranchInst() ) {
      if( p->IsPredTaken() )
	printf("<BT, %08lx> ", p->BAddr);
      else
	printf("<BN, %08lx> ", p->BAddr);
    }
    else
      printf("      ---      ");

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

    printf("]");

  }
  printf("\n");
}
