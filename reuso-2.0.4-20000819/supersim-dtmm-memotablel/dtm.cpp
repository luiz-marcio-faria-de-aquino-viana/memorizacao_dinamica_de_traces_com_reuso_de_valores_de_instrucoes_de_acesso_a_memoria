
/*
 * dtm.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/17/2000
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////////
///// CDtm

CDtm::CDtm(uword_t _memotbgsz, uword_t _memotbtsz, uword_t _memotblsz)
  : CComponent(__DTM_H),
    MemoTableG(_memotbgsz),
    MemoTableT(_memotbtsz),
    MemoTableL(_memotblsz)
{
  Status = DTM_DISABLED;
  ClearBufferT();
  Reset();
}

bool_t CDtm::IsEnabled()
{
  return (Status != DTM_DISABLED);
}

bool_t CDtm::IsEnabledWithMem()
{
  return (Status == DTM_WITH_MEM_ENABLED);
}

void CDtm::SetEnabled(int _status)
{
  Status = _status;
}

bool_t CDtm::IsValidDtmInst(CRobItem* _robit)
{
  switch((_robit->fi).op) {
  case OP_VALUE_1:          /* instrucao do formato 1 (CALL) */
    return TRUE;
    break;

  case OP_VALUE_0:          /* instrucao do formato 2 (UNIMP, Bicc, FBfcc, CBccc, SETHI) */
    switch((_robit->fi).op2) {
    case OP2_VALUE_0:           /* instrucao UNIMP */
      return FALSE;
      break;
    case OP2_VALUE_2:           /* instrucao Bicc */
    case OP2_VALUE_4:           /* instrucao SETHI */
      return TRUE;
      break;
    case OP2_VALUE_6:           /* branch on floating-point condition */
    case OP2_VALUE_7:           /* branch on coprocessor condition */
    case OP2_VALUE_1:
    case OP2_VALUE_3:
    case OP2_VALUE_5:
      return FALSE;
    }
    break;

  case OP_VALUE_3:          /* instrucao do formato 3 (Load/Store) */
    switch((_robit->fi).op3) {
    case LDSBA:                  /* load integer instructions from alternate space */
    case LDSHA:
    case LDUBA:
    case LDUHA:
    case LDA:
    case LDDA:
      return FALSE;
      break;
    case LDSB:                  /* load integer instructions (byte) */
    case LDSH:                  /* load integer instructions (half word) */
    case LDUB:                  /* load integer instructions (unsigned byte) */
    case LDUH:                  /* load integer instructions (unsigned half word) */
    case LD:                    /* load integer instructions (word) */
    case LDD:                   /* load integer instructions (double word) */
      return TRUE;
      break;
    case LDF:                   /* load floating-point instructions */
    case LDDF:
    case LDFSR:
      return FALSE;
      break;
    case LDC:                   /* load coprocessor instructions */
    case LDDC:
    case LDCSR:
      return FALSE;
      break;
    case STBA:                  /* store integer instructions to alternate space */
    case STHA:
    case STA:
    case STDA:
      return FALSE;
      break;
    case STB:                   /* store integer instructions (byte) */
    case STH:                   /* store integer instructions (hword) */
    case ST:                    /* store integer instructions (word) */
    case STD:                   /* store integer instructions (dword) */
      return TRUE;
      break;
    case STF:                   /* store floating-point instructions */
    case STDF:
    case STFSR:
    case STDFQ:
      return FALSE;
      break;
    case STC:                   /* store coprocessor instructions */
    case STDC:
    case STCSR:
    case STDCQ:
      return FALSE;
      break;
    case LDSTUBA:               /* atomic load-store unsigned byte from alternate space */
    case LDSTUB:                /* atomic load-store unsigned byte */
    case SWAPA:                 /* swap register with memory from alternate space */
    case SWAP:                  /* swap register with memory */
      return FALSE;
      break;
    default:
      return FALSE;
    }
    break;

  case OP_VALUE_2:          /* instrucao do formato 3 */
    switch((_robit->fi).op3) {
    case ADD:                   /* add instructions */
    case ADDCC:
    case ADDX:
    case ADDXCC:
    case TADDCC:                /* tagged add instructions */
      return TRUE;
      break;
    case TADDCCTV:              /* tagged add instructions and trap on overflow */
      return FALSE;
      break;
    case SUB:                   /* subtract instructions */
    case SUBCC:
    case SUBX:
    case SUBXCC:
    case TSUBCC:                /* tagged subtract instructions */
      return TRUE;
      break;
    case TSUBCCTV:              /* tagged subtract instructions and trap on overflow */
      return FALSE;
      break;
    case MULSCC:                /* multiply step instruction */
    case AND:                   /* logical instructions */
    case ANDCC:
    case ANDN:
    case ANDNCC:
    case OR:
    case ORCC:
    case ORN:
    case ORNCC:
    case XOR:
    case XORCC:
    case XNOR:
    case XNORCC:
    case SLL:                   /* shift instructions */
    case SRL:
    case SRA:
      return TRUE;
      break;
    case SAVE:                  /* SAVE instruction */
    case RESTORE:               /* RESTORE instruction */
      return FALSE;
      break;
    case JMPL:                  /* jump and link instruction */
      return TRUE;
      break;
    case RETT:                  /* return from trap instruction */
      return FALSE;
      break;
    case TICC:                  /* trap on integer condition instruction */
      return FALSE;
      break;
    case RDY:                   /* read Y register instruction */
    case RDPSR:                 /* read processor state register instruction */
      return TRUE;
      break;
    case RDWIM:                 /* read window invalid mask register instruction */
    case RDTBR:                 /* read trap base register instruction */
      return FALSE;
      break;
    case WRY:                   /* write Y register instruction */
    case WRPSR:                 /* write processor state register instruction */
      return TRUE;
      break;
    case WRWIM:                 /* write window invalid mask register instruction */
    case WRTBR:                 /* write trap base register instruction */
      return FALSE;
      break;
    case IFLUSH:                /* instruction cache flush instruction */
    case FPop1:                 /* floating-point operate (FPop) instructions */
    case FPop2:
      return FALSE;
      break;
    default:
      return FALSE;
    }
  };

  return FALSE;
}

bool_t CDtm::EmptyBufferT()
{
  return(BufferT.Tracesz <= 1);
}

void CDtm::ClearBufferT()
{
  int i;
  Ic_PsrMap = Oc_PsrMap = FALSE;
  Ic_YMap = Oc_YMap = FALSE;
  for(i = 0; i < IREG_NGREGS + IREG_WINSIZE; i++)
    Ic_IRegMap[i] = Oc_IRegMap[i] = FALSE;
  BufferT.Reset();
}

void CDtm::UpdateBufferT(CRobItem* _robit)
{

  if( _robit->IsTraceRedundant() ) {

    /* adiciona trace reusado ao trace em construcao */
    // Obs: esta versao do DTM nao concatena traces

#if(DEBUG <= 7)
    printf("\nDTM::UPDATE_BUFFER_T::ADD_TRACE");
    getchar();
#endif

  }
  else {

    /* adiciona instrucao reusada ao trace em construcao */

    // BufferT: atualiza contexto de entrada

    if( _robit->IsDelaySlotInst() ) {
#if(DEBUG <= 1)
      printf("\nDTM::UPDATE_BUFFER_T::DELAYSLOT(%lx)", _robit->Addr);
#endif
      if(BufferT.Tracesz == 0) return;
    }
    
#if(DEBUG <= 1)
    printf("\nDTM::UPDATE_BUFFER_T::PASSO_1");
#endif
    
    if( BufferT.Tracesz == 0 )
      BufferT.PC = _robit->Addr;
    
    if( (_robit->Psr).IsReqAsSource() &&
	(Ic_PsrMap == FALSE) &&
	(Oc_PsrMap == FALSE) ) {
      BufferT.NewInputContext(CTX_PSR, (_robit->RedundantInst).Ic_psr);
      Ic_PsrMap = TRUE;
    }
    
    if( (_robit->Y).IsReqAsSource() &&
	(Ic_YMap == FALSE) &&
	(Oc_YMap == FALSE) ) {
      BufferT.NewInputContext(CTX_Y, (_robit->RedundantInst).Ic_y);
      Ic_YMap = TRUE;
    }
    
    if( (_robit->Rs1).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rs1).GetRegAddr(),
			       (_robit->RedundantInst).Ic_src1 );
      Ic_IRegMap[(_robit->Rs1).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rs2).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rs2).GetRegAddr(),
			       (_robit->RedundantInst).Ic_src2 );
      Ic_IRegMap[(_robit->Rs2).GetRegAddr()] = TRUE;
    }

    if( (_robit->Rd_hi).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rd_hi).GetRegAddr(),
			       (_robit->RedundantInst).Ic_rdhi );
      Ic_IRegMap[(_robit->Rd_hi).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rd_lo).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rd_lo).GetRegAddr(),
			       (_robit->RedundantInst).Ic_rdlo );
      Ic_IRegMap[(_robit->Rd_lo).GetRegAddr()] = TRUE;
    }
    
    // BufferT: atualiza contexto de saida
    
    if( (_robit->Psr).IsReqAsTarget() ) {
      BufferT.NewOutputContext(CTX_PSR, (_robit->RedundantInst).Oc_psr);
      Oc_PsrMap = TRUE;
    }
    
    if( (_robit->Y).IsReqAsTarget() ) {
      BufferT.NewOutputContext(CTX_Y, (_robit->RedundantInst).Oc_y);
      Oc_YMap = TRUE;
    }
    
    if( (_robit->Rs1).IsReqAsTarget() ) {
      BufferT.NewOutputContext( CTX_GPR,
				(_robit->Rs1).GetRegAddr(),
				(_robit->RedundantInst).Oc_rdhi );
      Oc_IRegMap[(_robit->Rs1).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rs2).IsReqAsTarget() ) {
      BufferT.NewOutputContext( CTX_GPR,
				(_robit->Rs2).GetRegAddr(),
				(_robit->RedundantInst).Oc_rdhi );
      Oc_IRegMap[(_robit->Rs2).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rd_hi).IsReqAsTarget() ) {
      BufferT.NewOutputContext( CTX_GPR,
				(_robit->Rd_hi).GetRegAddr(),
				(_robit->Rd_hi).GetValue() );
      Oc_IRegMap[(_robit->Rd_hi).GetRegAddr()] = TRUE;
    }

    if( (_robit->Rd_lo).IsReqAsTarget() ) {
      BufferT.NewOutputContext( CTX_GPR,
				(_robit->Rd_lo).GetRegAddr(),
				(_robit->Rd_lo).GetValue() );
      Oc_IRegMap[(_robit->Rd_lo).GetRegAddr()] = TRUE;
    }
    
    // BufferT: atualiza a mascara de padroes de desvios do trace

    if( _robit->IsBranchInst() ) {
#if(DEBUG <= 7)
      fprintf( stderr,
	       "\nDTM::UPDATE_BUFFER_T::BRANCH_INST(%u, %hu)",
	       _robit->GetBranchType(),
	       _robit->BResult);
#endif
      switch( _robit->GetBranchType() ) {
      case __JUMPL:
	BufferT.AddBranchMask( __JUMPL,
			       TAKEN, 
			       _robit->Addr, 
			       _robit->BAddr );
	break;
      case __BICC:
      case __BICC_ANNUL:
	BufferT.AddBranchMask( _robit->GetBranchType(),
			       _robit->BResult,
			       _robit->Addr,
			       _robit->BAddr );
      };
    }

    // BufferT: atualiza as entradas apropriadas para instrucoes de load/store

    if( _robit->RType != RD_NONE ) {
      BufferT.AddLoadStoreInst( IOM_LOAD,
				_robit->RType,
				_robit->MAddr,
				_robit->MValidVal,
				& _robit->Rd_hi,
				(_robit->Rd_hi).GetValue(),
				& _robit->Rd_lo,
				(_robit->Rd_lo).GetValue() );
    }
    else if( _robit->WType != WR_NONE ) {
      BufferT.AddLoadStoreInst( IOM_STORE,
				_robit->WType,
				_robit->MAddr,
				_robit->MValidVal,
				& _robit->Rd_hi,
				(_robit->RedundantInst).Ic_rdhi,
				& _robit->Rd_lo,
				(_robit->RedundantInst).Ic_rdlo );
    }

    // BufferT: atualiza nPC para instrucoes de delay slot
    
    if( !_robit->IsDelaySlotInst() )
      BufferT.nPC = ( _robit->IsBranchInst() ) ? _robit->NewPC : _robit->Addr + 4;    

    BufferT.Tracesz += 1;

    ///////////////////////////////////////////////////////////////////////
    //// contabilizacao estatistica da frequencia de tipos de instrucoes nos traces
    BufferT.AddInstrCount(_robit->GetExecUnit(), _robit->GetInstOpcode());

  }

}

bool_t CDtm::BranchMaskOverflow(CRobItem* _robit)
{
  if( _robit->IsBranchInst() &&
      ( (_robit->GetBranchType() == __JUMPL) ||
	(_robit->GetBranchType() == __BICC) ||
	(_robit->GetBranchType() == __BICC_ANNUL) ) )
    return BufferT.IsFullBranchMask();
  return FALSE;
}

void CDtm::BtbUpdate(CMemoTableEntryT* p)
{
  int i;
  for(i = 0; i < p->BrcSz; i++) {
    Btb->Update((p->Brc[i]).baddr, & (p->Brc[i]).btaken, & (p->Brc[i]).bdest);
  }
}

bool_t CDtm::MatchBranchPattern(CMemoTableEntryT* p)
{
  CFetchBufItem *fb = 0, *fb_tmp = 0;
  CIQueueItem *iq = 0, *iq_tmp = 0;
  CDisqItem *di = 0, *di_tmp = 0;

  uword_t i;

  /* pesquisa fila de despacho
   */

  for(i = 0; i < Disq->GetQueueSize(); i++) {
    // loop. pesquisa todas as instrucoes da fila de despacho

    di = Disq->GetNthItem(i);

    if( p->Delay ) {
      if(p->DAddr == di->Addr) {
	if((i + 1) < Disq->GetQueueSize()) {
	  di_tmp = Disq->GetNthItem(i + 1);
	  if(p->nPC == di_tmp->Addr)
	    return TRUE;
	}
	else if( !IQueue->IsEmpty() ) {
	  iq_tmp = IQueue->GetFirstItem();
	  if(p->nPC == iq_tmp->Addr)
	    return TRUE;
	}
	else if( !FetchBuf->IsEmpty() ) {
	  fb_tmp = FetchBuf->GetFirstItem();
	  if(p->nPC == fb_tmp->Addr)
	    return TRUE;
	}
      }
    }
    else {
      if(p->nPC == di->Addr) return TRUE;
    }

    di->SetInvalidBit();
  }

  /* pesquisa fila de instrucoes
   */

  for(i = 0; i < IQueue->GetQueueSize(); i++) {
    // loop. pesquisa todas as instrucoes da fila de instrucoes

    iq = IQueue->GetNthItem(i);

    if( p->Delay ) {
      if(p->DAddr == iq->Addr) {
	if((i + 1) < IQueue->GetQueueSize()) {
	  iq_tmp = IQueue->GetNthItem(i + 1);
	  if(p->nPC == iq_tmp->Addr)
	    return TRUE;
	}
	else if( !FetchBuf->IsEmpty() ) {
	  fb_tmp = FetchBuf->GetFirstItem();
	  if(p->nPC == fb_tmp->Addr)
	    return TRUE;
	}
      }
    }
    else {
      if(p->nPC == iq->Addr) return TRUE;
    }

    iq->SetInvalidBit();
  }

  /* pesquisa fetch buffer
   */

  for(i = 0; i < FetchBuf->GetQueueSize(); i++) {
    // loop. pesquisa todas as instrucoes no fetch buffer

    fb = FetchBuf->GetNthItem(i);

    if( p->Delay ) {
      if(p->DAddr == fb->Addr) {
	if((i + 1) < FetchBuf->GetQueueSize() ) {
	  fb_tmp = FetchBuf->GetNthItem(i + 1);
	  if(p->nPC == fb_tmp->Addr)
	    return TRUE;
	}
      }
    }
    else {
      if(p->nPC == fb->Addr) return TRUE;
    }

    fb->SetInvalidBit();
  }

  /*
   * obs: Sempre que todas as instrucoes do pipeline sao invalidadas e realizado
   * um flush para redirecionar o PC ao endereco correto
   */

  return FALSE;

}

bool_t CDtm::HaveStorePending()
{
  //return (Maq->HaveStorePending() || Lsu->HaveStorePending());
  return RsLsu->HaveStorePending();
}

void CDtm::CompleteBufferT(uword_t _daddr, uword_t _dinst)
{
  if( !EmptyBufferT() ) {
    BufferT.AddDelayInst(_daddr, _dinst);
    MemoTableT.AddNew(BufferT);    

    ///////////////////////////////////////////////////////////////
    //// contabilizacao dos tipos de instrucoes por faixa
    Stat->AddInstrCount(& BufferT.TraceStats, IC_TRACE);

    ///////////////////////////////////////////////////////////////
    //// contabilizacao estatistica sobre o tamanho do trace
    Stat->AddTraceSz(BufferT.Tracesz, TSZ_TRACE, & BufferT.TraceStats);
  }
  ClearBufferT();
}

void CDtm::CompleteBufferT(CRobItem* _robit)
{
  if( !EmptyBufferT() ) {

    if( (_robit->Rs1).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rs1).GetRegAddr(),
			       (_robit->RedundantInst).Ic_src1 );
      Ic_IRegMap[(_robit->Rs1).GetRegAddr()] = TRUE;
    }
  
    if( (_robit->Rs2).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rs2).GetRegAddr(),
			       (_robit->RedundantInst).Ic_src2 );
      Ic_IRegMap[(_robit->Rs2).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rd_hi).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rd_hi).GetRegAddr(),
			       (_robit->RedundantInst).Ic_rdhi );
      Ic_IRegMap[(_robit->Rd_hi).GetRegAddr()] = TRUE;
    }
  
    if( (_robit->Rd_lo).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext( CTX_GPR,
			       (_robit->Rd_lo).GetRegAddr(),
			       (_robit->RedundantInst).Ic_rdlo );
      Ic_IRegMap[(_robit->Rd_lo).GetRegAddr()] = TRUE;
    }
    
    // BufferT: atualiza as entradas apropriadas para instrucoes de load/store

    if( _robit->RType != RD_NONE ) {
      BufferT.AddLoadStoreInst( IOM_LOAD,
				_robit->RType,
				_robit->MAddr,
				_robit->MValidVal,
				& _robit->Rd_hi,
				_robit->MValhi,
				& _robit->Rd_lo,
				_robit->MVallo );
    }
    else if( _robit->WType != WR_NONE ) {
      BufferT.AddLoadStoreInst( IOM_STORE,
				_robit->WType,
				_robit->MAddr,
				_robit->MValidVal,
				& _robit->Rd_hi,
				(_robit->RedundantInst).MValhi,
				& _robit->Rd_lo,
				(_robit->RedundantInst).MVallo );
    }
    
    // BufferT: atualiza nPC com endereco da proxima instrucao
    
    BufferT.nPC = _robit->Addr + 4;    

    BufferT.Tracesz += 1;


    ///////////////////////////////////////////////////////////////////////
    //// contabilizacao estatistica da frequencia de tipos de instrucoes nos traces
    BufferT.AddInstrCount(_robit->GetExecUnit(), _robit->GetInstOpcode());


    ///////////////////////////////////////////////////////////////
    //// contabilizacao dos tipos de instrucoes por faixa
    Stat->AddInstrCount(& BufferT.TraceStats, IC_TRACE);

    ///////////////////////////////////////////////////////////////
    //// contabiliza estatisticas sobre do tamanho do trace
    Stat->AddTraceSz(BufferT.Tracesz, TSZ_TRACE, & BufferT.TraceStats);


    MemoTableT.AddNew(BufferT);

  }

  ClearBufferT();
}

void CDtm::CompleteBufferT()
{
  if( !EmptyBufferT() ) {
    MemoTableT.AddNew(BufferT);

    ///////////////////////////////////////////////////////////////
    //// contabilizacao dos tipos de instrucoes por faixa
    Stat->AddInstrCount(& BufferT.TraceStats, IC_TRACE);

    ///////////////////////////////////////////////////////////////
    //// contabiliza estatisticas sobre do tamanho do trace
    Stat->AddTraceSz(BufferT.Tracesz, TSZ_TRACE, & BufferT.TraceStats);
  }
  ClearBufferT();
}

bool_t CDtm::ContextOverflow(CRobItem* _robit)
{
  uword_t icreq, ocreq;
  int i;

  // verifica overflow de instrucoes de load/store no trace

  if( _robit->IsLdStInst() && BufferT.IsFullLoadStore() )
    return TRUE;

  // verifica se existe espaco para o contexto de entrada

  icreq = 0;

  if( (_robit->Psr).IsReqAsSource() &&
      (Ic_PsrMap == FALSE) &&
      (Oc_PsrMap == FALSE) )
    icreq += 1;
  
  if( (_robit->Y).IsReqAsSource() &&
      (Ic_YMap == FALSE) &&
      (Oc_YMap == FALSE) )
    icreq += 1;

  if( (_robit->Rs1).IsReqAsSource() &&
      (Ic_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) &&
      (Oc_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) )
    icreq += 1;
  
  if( (_robit->Rs2).IsReqAsSource() &&
      (Ic_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) &&
      (Oc_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) &&
      ((_robit->Rs2).GetRegAddr() != (_robit->Rs1).GetRegAddr()) )
    icreq += 1;

  if( (_robit->Rd_hi).IsReqAsSource() &&
      (Ic_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) &&
      (Oc_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) )
    icreq += 1;
  
  if( (_robit->Rd_lo).IsReqAsSource() &&
      (Ic_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) &&
      (Oc_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) )
    icreq += 1;

  for(i = 0; (icreq > 0) && (i < IC_NUM); i++)
    if((BufferT.Ic[i]).regtype == CTX_NONE) icreq -= 1;
  if(icreq > 0) return TRUE;

  // verifica se existe espaco para o contexto de saida

  ocreq = 0;

  if( !_robit->IsLdStInst() ) {

    if( (_robit->Psr).IsReqAsTarget() &&
	(Oc_PsrMap == FALSE) )
      ocreq += 1;

    if( (_robit->Y).IsReqAsTarget() &&
	(Oc_YMap == FALSE) )
      ocreq += 1;

    if( (_robit->Rs1).IsReqAsTarget() &&
	(Oc_IRegMap[(_robit->Rs1).GetRegAddr()] == FALSE) )
      ocreq += 1;

    if( (_robit->Rs2).IsReqAsTarget() &&
	(Oc_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) )
      ocreq += 1;

    if( (_robit->Rd_lo).IsReqAsTarget() &&
	(Oc_IRegMap[(_robit->Rd_lo).GetRegAddr()] == FALSE) )
      ocreq += 1;

    if( (_robit->Rd_hi).IsReqAsTarget() &&
	(Oc_IRegMap[(_robit->Rd_hi).GetRegAddr()] == FALSE) )
      ocreq += 1;

    ocreq += BufferT.LdStSz;      // cada instrucao ld/st consome uma entrada no contexto de saida

  }
  else {

    // endereco de acesso a memoria requer uma entrada do contexto de saida
    ocreq += (BufferT.LdStSz + 1);   // se ld/st entao o contexto de saida requer mais uma entrada

  }

  for(i = 0; (ocreq > 0) && (i < OC_NUM); i++)
    if((BufferT.Oc[i]).regtype == CTX_NONE) ocreq -= 1;
  if(ocreq > 0) return TRUE;

  return FALSE;  
}

bool_t CDtm::VerifyReuse(CDisqItem* _di) 
{

  CMemoTableEntryG* p;
  bool_t redundant = FALSE;

  p = MemoTableG.FindFirst(_di->Addr);
  while( !redundant && (p != 0) ) {

    redundant = TRUE;

    // verifica se instrucao e redundante comparando o contexto de entrada

    if( (_di->Psr).IsReqAsSource() )
      if( (SReg->Psr).IsBusy() || ((SReg->Psr).GetValue() != p->Ic_psr) )
	redundant = FALSE;

    if( (_di->Y).IsReqAsSource() )
      if( (SReg->Y).IsBusy()  || ((SReg->Y).GetValue() != p->Ic_y) )
	redundant = FALSE;

    if( (_di->Rs1).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rs1).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rs1).GetRegAddr()) != p->Ic_src1) )
	redundant = FALSE;

    if( (_di->Rs2).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rs2).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rs2).GetRegAddr()) != p->Ic_src2) )
	redundant = FALSE;
  }

  return redundant;
}

void CDtm::ReservRegs(CRsItem* _rsit, CRobItem* _robit, CMemoTableEntryG* _it)
{

  // armazena a janela dos registradores fonte dos dados

  _rsit->SetSourceWin( IReg->GetCurrentWin() );
  _robit->SetSourceWin( IReg->GetCurrentWin() );

  // armazena a janela dos registradores destino dos dados

  _rsit->SetTargetWin( IReg->GetCurrentWin() );
  _robit->SetTargetWin( IReg->GetCurrentWin() );

  /*
   * se registrador fonte disponivel
   *   -> numero de sequencia enviado a estacao de reserva
   * senao
   *   -> valor do registrador e enviado a estacao de reserva
   */

  if( (_rsit->Rs1).IsReqAsSource() )
    (_rsit->Rs1).SetValue(_it->Ic_src1);

  if( (_rsit->Rs2).IsReqAsSource() )
    (_rsit->Rs2).SetValue(_it->Ic_src2);

  if( (_rsit->Rd_hi).IsReqAsSource() ) {
    if( IReg->IsBusy(_rsit->GetSourceWin(), (_rsit->Rd_hi).GetRegAddr()) )
      (_rsit->Rd_hi).SetDep( (_rsit->Rd_hi).GetRegAddr(),
			     IReg->GetSeqNum(_rsit->GetSourceWin(), (_rsit->Rd_hi).GetRegAddr()) );
    else
      (_rsit->Rd_hi).SetValue( IReg->GetValue(_rsit->GetSourceWin(), (_rsit->Rd_hi).GetRegAddr()) );
  }

  if( (_rsit->Rd_lo).IsReqAsSource() ) {
    if( IReg->IsBusy(_rsit->GetSourceWin(), (_rsit->Rd_lo).GetRegAddr()) )
      (_rsit->Rd_lo).SetDep( (_rsit->Rd_lo).GetRegAddr(),
			     IReg->GetSeqNum(_rsit->GetSourceWin(), (_rsit->Rd_lo).GetRegAddr()) );
    else
      (_rsit->Rd_lo).SetValue( IReg->GetValue(_rsit->GetSourceWin(), (_rsit->Rd_lo).GetRegAddr()) );
  }

  /*
   * se registrador requerido como destino
   *  -> marca registrador como ocupado e assinala o numero de sequencia da instrucao
   */

  if( (_rsit->Rd_hi).IsReqAsTarget() )
    IReg->SetBusy(_rsit->GetTargetWin(), (_rsit->Rd_hi).GetRegAddr(), _rsit->GetSeqNum());

  if((_rsit->Rd_lo).IsReqAsTarget())
    IReg->SetBusy(_rsit->GetTargetWin(), (_rsit->Rd_lo).GetRegAddr(), _rsit->GetSeqNum());

}

void CDtm::ReservRegs(CRsItem* _rsit, CRobItem* _robit, CMemoTableEntryT* _it)
{
  int i;

  // armazena a janela dos registradores fonte dos dados

  _rsit->SetSourceWin( IReg->GetCurrentWin() );
  _robit->SetSourceWin( IReg->GetCurrentWin() );

  // armazena a janela dos registradores destino dos dados

  _rsit->SetTargetWin( IReg->GetCurrentWin() );
  _robit->SetTargetWin( IReg->GetCurrentWin() );

  /*
   * elimina dependencia das entradas com valores fontes requeridos
   */

  if( (_rsit->Rs1).IsReqAsSource() )
    (_rsit->Rs1).ClearDep();

  if( (_rsit->Rs2).IsReqAsSource() )
    (_rsit->Rs2).ClearDep();

  if( (_rsit->Rd_hi).IsReqAsSource() )
    (_rsit->Rd_hi).ClearDep();

  if( (_rsit->Rd_lo).IsReqAsSource() )
    (_rsit->Rd_lo).ClearDep();

  /*
   * se registrador requerido como destino
   *  -> marca registrador como ocupado e assinala o numero de sequencia da instrucao
   * (apenas para operacoes pendentes de leitura a memoria)
   */

  // operacoes de leitura da memoria

  for(i = 0; i < _it->LdStSz; i++) {
    if( (_it->LdStInst[i].LdSt & IOM_LOAD) != 0 ) {

      if( (_it->LdStInst[i]).Rdhi.IsReqAsTarget() )
	IReg->SetBusy( _rsit->GetTargetWin(),
		       (_it->LdStInst[i]).Rdhi.GetRegAddr(),
		       _rsit->GetSeqNum() );
      
      if( (_it->LdStInst[i]).Rdlo.IsReqAsTarget() )
	IReg->SetBusy( _rsit->GetTargetWin(),
		       (_it->LdStInst[i]).Rdlo.GetRegAddr(),
		       _rsit->GetSeqNum() );

    }
    
  }

}

bool_t CDtm::Stage_DS1(CFetchBufItem* _fb)
{

  if( !IsEnabled() ) return FALSE;

  if(
#ifndef NO_TRACE_REUSE
     (MemoTableT.FindFirst(_fb->Addr) != 0) ||
#endif
     (MemoTableG.FindFirst(_fb->Addr) != 0) ||
     (MemoTableL.FindFirst(_fb->Addr) != 0)
  ) {
    _fb->SetProbablyRedundant();
    return TRUE;
  }

  return FALSE;
}

bool_t CDtm::Stage_DS2(CDisqItem* _di)
{
  CMemoTableEntryG* p;
  CMemoTableEntryT* q;
  CMemoTableEntryL* r;

  bool_t redundant = FALSE;

  CRsItem* rsit;
  CMaqItem* maqit;
  CRobItem* robit;

  bool_t chgpc = FALSE;
  
  bool_t bres = NTAKEN;
  int i;

  if( !IsEnabled() ) return FALSE;

  if( _di->IsInvalid() ) return FALSE;

  if( !_di->IsProbablyRedundant() )
    return FALSE;

#ifndef NO_TRACE_REUSE

  // MEMO_TABLE_T: procura um trace redundante na tabela que inicia com a instrucao

  q = MemoTableT.FindFirst(_di->Addr);
  while(q != 0)
  {
    redundant = TRUE;

    // verifica se trace e redundante comparando o contexto de entrada

    for(i = 0; (redundant != FALSE) && (i < IC_NUM); i++) {
      switch( q->Ic[i].regtype ) {
      case CTX_PSR:
	if( (SReg->Psr).IsBusy() || ((SReg->Psr).GetValue() != q->Ic[i].regval) )
	  redundant = FALSE;
	break;
      case CTX_Y:
	if( (SReg->Y).IsBusy() || ((SReg->Y).GetValue() != q->Ic[i].regval) )
	  redundant = FALSE;
	break;
      case CTX_GPR:
	if( IReg->IsBusy(IReg->GetCurrentWin(), q->Ic[i].regaddr) ||
	    (IReg->GetValue(IReg->GetCurrentWin(), q->Ic[i].regaddr) != q->Ic[i].regval) )
	  redundant = FALSE;
      };
    }

    if(redundant == TRUE) {        // se existir algum trace redundante

      if( !q->CheckForLoadStoreInst(IOM_STORE | IOM_LOAD) ||
	  (q->CheckForLoadStoreInst(IOM_LOAD) && q->IsValidLoad() && !HaveStorePending()) )
      {
	/* se trace NAO possuir instrucao de load/store ou
	 * se trace possuir instrucao de load com valor validado e nenhuma operacao de store pendente no pipeline
	 */
#if(DEBUG <= 17)
	fprintf( stderr, 
		 "DTM::STAGE_DS2(TRACE/LD-V-, seq=%ld, addr=%lx)\n",
		 _di->GetSeqNum(),
		 _di->Addr );
#endif

	// (1) marca a instrucao como sendo inicio de um trace reusado

	_di->SetTraceRedundant();

	// (2) insere trace no reorder buffer
 
	robit = Rob->InsertItem(_di);
	robit->SetSourceWin( IReg->GetCurrentWin() );
	robit->SetTargetWin( IReg->GetCurrentWin() );

	// (3) atualiza contexto de saida

	for(i = 0; i < OC_NUM; i++) {
	  switch( q->Oc[i].regtype ) {
	  case CTX_PSR:
	    (SReg->Psr).SetBusy(_di->GetSeqNum());
	    (SReg->Psr).SetFValue(q->Oc[i].regval);
	    (SReg->Psr).ClearBusy();
	    break;
	  case CTX_Y:
	    (SReg->Y).SetBusy(_di->GetSeqNum());
	    (SReg->Y).SetFValue(q->Oc[i].regval);
	    (SReg->Y).ClearBusy();
	    break;
	  case CTX_GPR:
	    IReg->SetBusy(IReg->GetCurrentWin(), q->Oc[i].regaddr, _di->GetSeqNum());
	    IReg->SetFValue(IReg->GetCurrentWin(), q->Oc[i].regaddr, q->Oc[i].regval);
	    IReg->ClearBusy(IReg->GetCurrentWin(), q->Oc[i].regaddr);
	  };
	}

	// (4) atualiza contexto de saida para instrucoes de load

	for(i = 0; i < q->LdStSz; i++) {
	  if( ((q->LdStInst[i]).LdSt == IOM_LOAD) ) {
	    IReg->SetBusy(IReg->GetCurrentWin(), (q->LdStInst[i]).Rdhi.GetRegAddr(), _di->GetSeqNum());
	    IReg->SetFValue(IReg->GetCurrentWin(), (q->LdStInst[i]).Rdhi.GetRegAddr(), (q->LdStInst[i]).MValhi);
	    IReg->ClearBusy(IReg->GetCurrentWin(), (q->LdStInst[i]).Rdhi.GetRegAddr());

	    if( (q->LdStInst[i]).Rdlo.IsReqAsTarget() ) {
	      IReg->SetBusy(IReg->GetCurrentWin(), (q->LdStInst[i]).Rdlo.GetRegAddr(), _di->GetSeqNum());
	      IReg->SetFValue(IReg->GetCurrentWin(), (q->LdStInst[i]).Rdlo.GetRegAddr(), (q->LdStInst[i]).MVallo);
	      IReg->ClearBusy(IReg->GetCurrentWin(), (q->LdStInst[i]).Rdlo.GetRegAddr());
	    }

	  }

	}

	// (5) atualiza a BTB

	BtbUpdate(q);

	// (6) verifica se padrao de desvios coincide com fluxo no pipeline

	if( !MatchBranchPattern(q) ) {
	  // se padrao nao coincide redireciona o pc para o endereco destino dos traces
	  
	  IQueue->Flush();
	  
	  Disq->Flush(_di->GetSeqNum());

	  FetchBuf->SetLastBranchLevel(_di->GetBranchLevel() + 1);

	  FetchBuf->Flush(_di->GetSeqNum(), _di->GetBranchLevel());

	  Fetch->SetNewPC(q->nPC);

	  if( q->BiccAtEnd() )
	    FetchBuf->InsertItem(q->DAddr, q->DInst, TRUE);
	}

	// (7) atualiza reorder buffer

	Rob->Update( _di->GetSeqNum(),
		     0,
		     0,
		     0,
		     0,
		     0,
		     0,
		     FALSE,
		     0,
		     TRUE,
		     q->nPC,
		     q );
	
	MemoTableT.Update(q);

	return redundant;

      }
      else {    
	/* se trace possuir instrucao de store ou
	 * se trace possuir instrucao de load com valor invalidado ou
	 * se existir alguma operacao de store pendente no pipeline
	 */
#if(DEBUG <= 17)
	fprintf( stderr, 
		 "DTM::STAGE_DS2(TRACE/LD-I-/ST-Pending, seq=%ld, addr=%lx)\n",
		 _di->GetSeqNum(),
		 _di->Addr );
#endif

	// (1) verifica entradas livres na estacao de reserva e fila de acesso a memoria

	if( RsLsu->IsFull() || Maq->IsFull() )
	  return (redundant = FALSE);

	// (2) marca a instrucao como sendo inicio de um trace reusado

	_di->SetTraceRedundant();

	// (3) adiciona entrada a estacao de reserva

	rsit = RsLsu->InsertItem(_di);

	// (4) adiciona entrada no reorder buffer
 
	robit = Rob->InsertItem(_di);
	
	// (5) atualiza contexto de saida

	for(i = 0; i < OC_NUM; i++) {
	  switch( q->Oc[i].regtype ) {
	  case CTX_PSR:
	    (SReg->Psr).SetBusy(_di->GetSeqNum());
	    (SReg->Psr).SetFValue(q->Oc[i].regval);
	    (SReg->Psr).ClearBusy();
	    break;
	  case CTX_Y:
	    (SReg->Y).SetBusy(_di->GetSeqNum());
	    (SReg->Y).SetFValue(q->Oc[i].regval);
	    (SReg->Y).ClearBusy();
	    break;
	  case CTX_GPR:
	    IReg->SetBusy(IReg->GetCurrentWin(), q->Oc[i].regaddr, _di->GetSeqNum());
	    IReg->SetFValue(IReg->GetCurrentWin(), q->Oc[i].regaddr, q->Oc[i].regval);
	    IReg->ClearBusy(IReg->GetCurrentWin(), q->Oc[i].regaddr);
	  };
	}

	// (6) marca registradores destino do load como ocupados

	ReservRegs(rsit, robit, q);

	// (7) adiciona entrada a fila de acesso a memoria

	maqit = Maq->InsertItem(_di);

	// (8) disponibiliza dados do trace as entradas em uso pela instrucao

	rsit->TraceData = (*q);
	maqit->TraceData = (*q);
       	robit->TraceData = (*q);

	// (9) atualiza endereco de leitura/escrita na memoria
	// (necessario apenas para liberar a instrucao na maq)

	maqit->SetMAddrReady();

	// (10) atualiza a BTB

	BtbUpdate(q);

	// (11) verifica se padrao de desvios coincide com fluxo no pipeline

	if( !MatchBranchPattern(q) ) {
	  // se padrao nao coincide redireciona o pc para o endereco destino dos traces
	  
	  IQueue->Flush();
	  
	  Disq->Flush(_di->GetSeqNum());

	  FetchBuf->SetLastBranchLevel(_di->GetBranchLevel() + 1);

	  FetchBuf->Flush(_di->GetSeqNum(), _di->GetBranchLevel());

	  Fetch->SetNewPC(q->nPC);

	  if( q->BiccAtEnd() )
	    FetchBuf->InsertItem(q->DAddr, q->DInst, TRUE);
	}

	// (12) atualiza reorder buffer

	Rob->Update( _di->GetSeqNum(),
		     0,
		     0,
		     0,
		     0,
		     0,
		     0,
		     FALSE,
		     0,
		     TRUE,
		     q->nPC,
		     q );

	robit->Done = FALSE;

	MemoTableT.Update(q);

	return redundant;

      }

    }

    q = MemoTableT.FindNext(_di->Addr);
  }  

#endif

  // MEMO_TABLE_G: procura uma instancia equivalente da instrucao na tabela 

  p = MemoTableG.FindFirst(_di->Addr);
  while(p != 0)
  {
    redundant = TRUE;

    // verifica se instrucao e redundante comparando o contexto de entrada

    if( (_di->Psr).IsReqAsSource() )
      if( (SReg->Psr).IsBusy() ||
	  ((SReg->Psr).GetValue() != p->Ic_psr) )
	redundant = FALSE;

    if( (_di->Y).IsReqAsSource() )
      if( (SReg->Y).IsBusy()  ||
	  ((SReg->Y).GetValue() != p->Ic_y) )
	redundant = FALSE;

    if( (_di->Rs1).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rs1).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rs1).GetRegAddr()) != p->Ic_src1) )
	redundant = FALSE;

    if( (_di->Rs2).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rs2).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rs2).GetRegAddr()) != p->Ic_src2) )
	redundant = FALSE;

    if( (_di->Rd_hi).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr()) != p->Ic_rdhi) )
	redundant = FALSE;

    if( (_di->Rd_lo).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rd_lo).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rd_lo).GetRegAddr()) != p->Ic_rdlo) )
	redundant = FALSE;

    if(redundant == TRUE) {                    // se instrucao e redundante

      if( _di->IsLdStInst() ) {                // se instrucao e de load/store

	if( !HaveStorePending() && (_di->RType != RD_NONE) && p->MValid ) {
	  // se instrucao de load, NAO existe store pendente e valor para reuso valido

	  // (1) marca a instrucao como sendo redundante

	  _di->SetRedundant();

	  // (2) insere instrucao no reorder buffer
	  
	  robit = Rob->InsertItem(_di);
	  robit->SetSourceWin( IReg->GetCurrentWin() );
	  robit->SetTargetWin( IReg->GetCurrentWin() );

	  robit->RedundantInst = (*p);     // atribui as instrucoes redundantes ao Reorder Buffer

	  // (3) atualiza registradores destino

	  if( (_di->Rd_hi).IsReqAsTarget() ) {
	    IReg->SetBusy( IReg->GetCurrentWin(),
			   (_di->Rd_hi).GetRegAddr(),
			   _di->GetSeqNum() );
	    IReg->SetFValue( IReg->GetCurrentWin(),
			     (_di->Rd_hi).GetRegAddr(),
			     p->MValhi );
	    IReg->ClearBusy( IReg->GetCurrentWin(),
			     (_di->Rd_hi).GetRegAddr() );
	  }

	  if( (_di->Rd_lo).IsReqAsTarget() ) {
	    IReg->SetBusy( IReg->GetCurrentWin(),
			   (_di->Rd_lo).GetRegAddr(),
			   _di->GetSeqNum() );
	    IReg->SetFValue( IReg->GetCurrentWin(),
			     (_di->Rd_lo).GetRegAddr(),
			     p->MVallo );
	    IReg->ClearBusy( IReg->GetCurrentWin(),
			     (_di->Rd_lo).GetRegAddr() );
	  }

	  // (4) atualiza reorder buffer

	  Rob->Update( _di->GetSeqNum(),
		       0,
		       0,
		       0,
		       0,
		       p->MValhi,
		       p->MVallo,
		       FALSE,
		       0,
		       FALSE,
		       0,
		       0,
		       NTAKEN,
		       p->MAddr );

	}
	else {
	  // se instrucao de store ou load com valor para reuso invalidado

	  // (1) verifica entradas livres na estacao de reserva e fila de acesso a memoria

	  if( RsLsu->IsFull() || Maq->IsFull() ) return FALSE;
	  
	  // (2) marca a instrucao como sendo inicio de um trace reusado

	  _di->SetRedundant();

	  // (3) insere instrucao na estacao de reserva
 
	  rsit = RsLsu->InsertItem(_di);

	  // (4) insere instrucao na fila de reordenacao

	  robit = Rob->InsertItem(_di);

	  // (5) marca registradores destino como ocupados

	  ReservRegs(rsit, robit, p);

	  // (6) insere instrucao na fila de acesso a memoria

	  maqit = Maq->InsertItem(_di);

	  // (7) atualiza endereco de leitura/escrita na memoria

	  maqit->SetMAddr(p->MAddr);

	  // (8) disponibiliza dados do reuso as entradas em uso pela instrucao

	  rsit->RedundantInst = (*p);
	  maqit->RedundantInst = (*p);
	  robit->RedundantInst = (*p);

	}

      }
      else {     // se instrucao NAO e de load/store

      	// (1) marca a instrucao como sendo redundante

	_di->SetRedundant();

	// (2) insere instrucao no reorder buffer
 
	robit = Rob->InsertItem(_di);
	robit->SetSourceWin( IReg->GetCurrentWin() );
	robit->SetTargetWin( IReg->GetCurrentWin() );

	robit->RedundantInst = (*p);     // atribui as instrucoes redundantes ao Reorder Buffer

	// (3) atualiza registradores destino

	if( (_di->Psr).IsReqAsTarget()) {
	  (SReg->Psr).SetBusy(_di->GetSeqNum());
	  (SReg->Psr).SetFValue(p->Oc_psr);
	  (SReg->Psr).ClearBusy();
	}
      
	if( (_di->Y).IsReqAsTarget() ) {
	  (SReg->Y).SetBusy(_di->GetSeqNum());
	  (SReg->Y).SetFValue(p->Oc_y);
	  (SReg->Y).ClearBusy();
	}
      
	if( (_di->Rd_hi).IsReqAsTarget() ) {
	  IReg->SetBusy( IReg->GetCurrentWin(),
			 (_di->Rd_hi).GetRegAddr(),
			 _di->GetSeqNum() );
	  IReg->SetFValue( IReg->GetCurrentWin(),
			   (_di->Rd_hi).GetRegAddr(),
			   p->Oc_rdhi );
	  IReg->ClearBusy( IReg->GetCurrentWin(),
			   (_di->Rd_hi).GetRegAddr() );
	}


	// (4) atualiza pipeline para instrucoes de desvio

	if( _di->IsBranchInst() ) {

	  FetchBuf->SetLastBranchLevel( _di->GetBranchLevel() + 1 );

	  if( (_di->GetBranchType() == __BICC) ||    // instrucao de desvio condicional
	      (_di->GetBranchType() == __BICC_ANNUL) ) {

	    if( (p->BTaken != _di->BPred) ) {        // previsao efetuada incorretamente
	    
	      // atualiza a BTB
	      
	      if(_di->BPred == TAKEN)
		Btb->Update(_di->Addr, & (_di->BPred = NTAKEN), & p->nPC);
	      else
		Btb->Update(_di->Addr, & (_di->BPred = TAKEN), & p->nPC);


	      // efetua flush nos estagios anteriores do pipeline
	      
	      IQueue->Flush(_di->GetBranchLevel(), _di->GetSeqNum());
	      
	      Disq->Flush(_di->GetBranchLevel(), _di->GetSeqNum());

	      FetchBuf->Flush(_di->GetSeqNum(), _di->GetBranchLevel());

	      Fetch->SetNewPC(p->nPC);


	      // atualizacao do sinalizador de previsao de desvio
	    
	      _di->BPred = (p->BTaken == TRUE) ? TAKEN : NTAKEN;
	      chgpc = TRUE;
	    
	    }
	    else {	                                 // previsao efetuada corretamente

	      // atualiza a BTB

	      Btb->Update(_di->Addr, & _di->BPred, & _di->BAddr);


	      // remove instrucao do delay-slot

	      IQueue->RemoveBranchDepItem(_di->GetSeqNum());
	      Disq->RemoveBranchDepItem(_di->GetSeqNum());
	      
	    }

	    bres = p->BTaken;
	  }
	  else if(_di->GetBranchType() == __JUMPL) {    // instrucao de descio incondicional 

	    // atualiza a BTB
	
	    Btb->Update(_di->Addr, & _di->BPred, & p->nPC);

	    if(p->nPC != _di->BAddr) {                  // endereco alvo previsto incorretamente

	      // efetua flush nos estagios anteriores do pipeline

	      IQueue->Flush(_di->GetBranchLevel(), _di->GetSeqNum());

	      Disq->Flush(_di->GetBranchLevel(), _di->GetSeqNum());

	      FetchBuf->Flush(_di->GetSeqNum(), _di->GetBranchLevel());

	      Fetch->SetNewPC(p->nPC);


	      // atualizacao do sinalizador de previsao de desvio

	      chgpc = TRUE;

	    }

	    bres = TAKEN;
	  }
	  else if( (_di->GetBranchType() != __BN) &&
		   (_di->GetBranchType() == __BN_ANNUL) ) { // instrucao nao for branch never
	    bres = TAKEN;
	  }

	}

	// (5) atualiza reorder buffer

	Rob->Update( _di->GetSeqNum(),
		     p->Oc_psr,
		     p->Oc_y,
		     0,
		     0,
		     p->Oc_rdhi,
		     0,
		     FALSE,
		     0,
		     chgpc,
		     p->nPC,
		     0,
		     bres );

      }

      MemoTableG.Update(p);

      return redundant;

    }

    p = MemoTableG.FindNext(_di->Addr);
  }

  // MEMO_TABLE_L: procura uma instancia equivalente da instrucao na tabela 

  r = MemoTableL.FindFirst(_di->Addr);
  while(r != 0)
  {
    redundant = TRUE;

    // verifica se instrucao e redundante comparando o contexto de entrada

    if( (_di->Psr).IsReqAsSource() )
      if( (SReg->Psr).IsBusy() ||
	  ((SReg->Psr).GetValue() != r->Ic_psr) )
	redundant = FALSE;

    if( (_di->Y).IsReqAsSource() )
      if( (SReg->Y).IsBusy()  ||
	  ((SReg->Y).GetValue() != r->Ic_y) )
	redundant = FALSE;

    if( (_di->Rs1).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rs1).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rs1).GetRegAddr()) != r->Ic_src1) )
	redundant = FALSE;

    if( (_di->Rs2).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rs2).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rs2).GetRegAddr()) != r->Ic_src2) )
	redundant = FALSE;

    if( (_di->Rd_hi).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr()) != r->Ic_rdhi) )
	redundant = FALSE;

    if( (_di->Rd_lo).IsReqAsSource() )
      if( IReg->IsBusy(IReg->GetCurrentWin(), (_di->Rd_lo).GetRegAddr()) ||
	  (IReg->GetValue(IReg->GetCurrentWin(), (_di->Rd_lo).GetRegAddr()) != r->Ic_rdlo) )
	redundant = FALSE;

    if(redundant == TRUE) {                    // se instrucao e redundante

      if( _di->IsLdStInst() ) {                // se instrucao e de load/store

	if( !HaveStorePending() && (_di->RType != RD_NONE) && r->MValid ) {
	  // se instrucao de load, NAO existe store pendente e valor para reuso valido

	  // (1) marca a instrucao como sendo redundante

	  _di->SetRedundant();

	  // (2) insere instrucao no reorder buffer
	  
	  robit = Rob->InsertItem(_di);
	  robit->SetSourceWin( IReg->GetCurrentWin() );
	  robit->SetTargetWin( IReg->GetCurrentWin() );

	  robit->RedundantInst = (*r);     // atribui as instrucoes redundantes ao Reorder Buffer

	  // (3) atualiza registradores destino

	  if( (_di->Rd_hi).IsReqAsTarget() ) {
	    IReg->SetBusy( IReg->GetCurrentWin(),
			   (_di->Rd_hi).GetRegAddr(),
			   _di->GetSeqNum() );
	    IReg->SetFValue( IReg->GetCurrentWin(),
			     (_di->Rd_hi).GetRegAddr(),
			     r->MValhi );
	    IReg->ClearBusy( IReg->GetCurrentWin(),
			     (_di->Rd_hi).GetRegAddr() );
	  }

	  if( (_di->Rd_lo).IsReqAsTarget() ) {
	    IReg->SetBusy( IReg->GetCurrentWin(),
			   (_di->Rd_lo).GetRegAddr(),
			   _di->GetSeqNum() );
	    IReg->SetFValue( IReg->GetCurrentWin(),
			     (_di->Rd_lo).GetRegAddr(),
			     r->MVallo );
	    IReg->ClearBusy( IReg->GetCurrentWin(),
			     (_di->Rd_lo).GetRegAddr() );
	  }

	  // (4) atualiza reorder buffer

	  Rob->Update( _di->GetSeqNum(),
		       0,
		       0,
		       0,
		       0,
		       r->MValhi,
		       r->MVallo,
		       FALSE,
		       0,
		       FALSE,
		       0,
		       0,
		       NTAKEN,
		       r->MAddr );

	}
	else {
	  // se instrucao de store ou load com valor para reuso invalidado

	  // (1) verifica entradas livres na estacao de reserva e fila de acesso a memoria

	  if( RsLsu->IsFull() || Maq->IsFull() ) return FALSE;
	  
	  // (2) marca a instrucao como sendo inicio de um trace reusado

	  _di->SetRedundant();

	  // (3) insere instrucao na estacao de reserva
 
	  rsit = RsLsu->InsertItem(_di);

	  // (4) insere instrucao na fila de reordenacao

	  robit = Rob->InsertItem(_di);

	  // (5) marca registradores destino como ocupados

	  ReservRegs(rsit, robit, r);

	  // (6) insere instrucao na fila de acesso a memoria

	  maqit = Maq->InsertItem(_di);

	  // (7) atualiza endereco de leitura/escrita na memoria

	  maqit->SetMAddr(r->MAddr);

	  // (8) disponibiliza dados do reuso as entradas em uso pela instrucao

	  rsit->RedundantInst = (*r);
	  maqit->RedundantInst = (*r);
	  robit->RedundantInst = (*r);

	}

      }
      else {     // se instrucao NAO e de load/store

      	// (1) marca a instrucao como sendo redundante

	_di->SetRedundant();

	// (2) insere instrucao no reorder buffer
 
	robit = Rob->InsertItem(_di);
	robit->SetSourceWin( IReg->GetCurrentWin() );
	robit->SetTargetWin( IReg->GetCurrentWin() );

	robit->RedundantInst = (*r);     // atribui as instrucoes redundantes ao Reorder Buffer

	// (3) atualiza registradores destino

	if( (_di->Psr).IsReqAsTarget()) {
	  (SReg->Psr).SetBusy(_di->GetSeqNum());
	  (SReg->Psr).SetFValue(r->Oc_psr);
	  (SReg->Psr).ClearBusy();
	}
      
	if( (_di->Y).IsReqAsTarget() ) {
	  (SReg->Y).SetBusy(_di->GetSeqNum());
	  (SReg->Y).SetFValue(r->Oc_y);
	  (SReg->Y).ClearBusy();
	}
      
	if( (_di->Rd_hi).IsReqAsTarget() ) {
	  IReg->SetBusy( IReg->GetCurrentWin(),
			 (_di->Rd_hi).GetRegAddr(),
			 _di->GetSeqNum() );
	  IReg->SetFValue( IReg->GetCurrentWin(),
			   (_di->Rd_hi).GetRegAddr(),
			   r->Oc_rdhi );
	  IReg->ClearBusy( IReg->GetCurrentWin(),
			   (_di->Rd_hi).GetRegAddr() );
	}


	// (4) atualiza pipeline para instrucoes de desvio

	if( _di->IsBranchInst() ) {

	  FetchBuf->SetLastBranchLevel( _di->GetBranchLevel() + 1 );

	  if( (_di->GetBranchType() == __BICC) ||    // instrucao de desvio condicional
	      (_di->GetBranchType() == __BICC_ANNUL) ) {

	    if( (r->BTaken != _di->BPred) ) {        // previsao efetuada incorretamente
	    
	      // atualiza a BTB
	      
	      if(_di->BPred == TAKEN)
		Btb->Update(_di->Addr, & (_di->BPred = NTAKEN), & r->nPC);
	      else
		Btb->Update(_di->Addr, & (_di->BPred = TAKEN), & r->nPC);


	      // efetua flush nos estagios anteriores do pipeline
	      
	      IQueue->Flush(_di->GetBranchLevel(), _di->GetSeqNum());
	      
	      Disq->Flush(_di->GetBranchLevel(), _di->GetSeqNum());

	      FetchBuf->Flush(_di->GetSeqNum(), _di->GetBranchLevel());

	      Fetch->SetNewPC(r->nPC);


	      // atualizacao do sinalizador de previsao de desvio
	    
	      _di->BPred = (r->BTaken == TRUE) ? TAKEN : NTAKEN;
	      chgpc = TRUE;
	    
	    }
	    else {	                                 // previsao efetuada corretamente

	      // atualiza a BTB

	      Btb->Update(_di->Addr, & _di->BPred, & _di->BAddr);


	      // remove instrucao do delay-slot

	      IQueue->RemoveBranchDepItem(_di->GetSeqNum());
	      Disq->RemoveBranchDepItem(_di->GetSeqNum());
	      
	    }

	    bres = r->BTaken;
	  }
	  else if(_di->GetBranchType() == __JUMPL) {    // instrucao de descio incondicional 

	    // atualiza a BTB
	
	    Btb->Update(_di->Addr, & _di->BPred, & r->nPC);

	    if(r->nPC != _di->BAddr) {                  // endereco alvo previsto incorretamente

	      // efetua flush nos estagios anteriores do pipeline

	      IQueue->Flush(_di->GetBranchLevel(), _di->GetSeqNum());

	      Disq->Flush(_di->GetBranchLevel(), _di->GetSeqNum());

	      FetchBuf->Flush(_di->GetSeqNum(), _di->GetBranchLevel());

	      Fetch->SetNewPC(r->nPC);


	      // atualizacao do sinalizador de previsao de desvio

	      chgpc = TRUE;

	    }

	    bres = TAKEN;
	  }
	  else if( (_di->GetBranchType() != __BN) &&
		   (_di->GetBranchType() == __BN_ANNUL) ) { // instrucao nao for branch never
	    bres = TAKEN;
	  }

	}

	// (5) atualiza reorder buffer

	Rob->Update( _di->GetSeqNum(),
		     r->Oc_psr,
		     r->Oc_y,
		     0,
		     0,
		     r->Oc_rdhi,
		     0,
		     FALSE,
		     0,
		     chgpc,
		     r->nPC,
		     0,
		     bres );

      }

      MemoTableL.Update(r);

      return redundant;

    }

    r = MemoTableL.FindNext(_di->Addr);
  }

  return redundant;
}

bool_t CDtm::Stage_DS3_TraceReuse(CRobItem* _robit)
{
  int i;

#ifndef NO_TRACE_REUSE

#if(DEBUG <= 15)
  if(_robit->Addr == 0x31C0L) {
    fprintf(stderr, "\nDS3::TRACEREUSE(seq=%ld, pc=%lx)", _robit->Seq, _robit->Addr);
    getchar();
  }
#endif

  CMemoTableEntryT* q = & _robit->TraceData;
  for(i = 0; i < OC_NUM; i++) {
    switch( q->Oc[i].regtype ) {
    case CTX_PSR:
      (SReg->Psr).SetValue(q->Oc[i].regval);
      break;
    case CTX_Y:
      (SReg->Y).SetValue(q->Oc[i].regval);
      break;
    case CTX_GPR:
      IReg->SetValue( _robit->GetTargetWin(),
		      q->Oc[i].regaddr,
		      q->Oc[i].regval );
    };
  }

  // atualiza os registradores com base nos traces com operacoes de load

  for(i = 0; i < q->LdStSz; i++) {
    if( ((q->LdStInst[i]).LdSt & IOM_LOAD) == IOM_LOAD ) {

      if( (q->LdStInst[i]).Rdhi.IsReqAsTarget() )
	IReg->SetValue( _robit->GetTargetWin(),
			(q->LdStInst[i]).Rdhi.GetRegAddr(),
			(q->LdStInst[i]).MValhi );
      
      if( (q->LdStInst[i]).Rdlo.IsReqAsTarget() )
	IReg->SetValue( _robit->GetTargetWin(),
			(q->LdStInst[i]).Rdlo.GetRegAddr(),
			(q->LdStInst[i]).MVallo );
    }
  }

#endif

  return TRUE;
}

bool_t CDtm::Stage_DS3_TraceConstruction(CRobItem* _robit)
{
  uword_t pc, npc;
  
  bool_t  jmp, brc, btaken;
  
  uword_t ic_src1, ic_src2, ic_y;
  uword_t ic_rdhi, ic_rdlo;
  uword_t ic_psr;

  uword_t oc_rdhi, oc_rdlo;
  uword_t oc_y;
  uword_t oc_psr;

  ubyte_t ldst;
  uword_t ldsttype;
  bool_t  mvalid;
  uword_t maddr;
  uword_t mvalhi, mvallo;

  if( !IsEnabled() ) return FALSE;

  if( IsValidDtmInst(_robit) ) {  // se instrucao pertence ao dominio de instrucoes validas do DTM

    if( !_robit->IsDelaySlotInst() ) {  // se instrucao NAO for de delay slot

      if( !_robit->IsRedundant() && !_robit->IsDependentRedundant() ) {
	// se instrucao NAO for redundante

	// (1) adiciona instrucao a MEMO_TABLE_G / MEMO_TABLE_L

	pc = _robit->Addr;

	jmp = (_robit->GetBranchType() == __JUMPL) ? TRUE : FALSE;

	brc = ((_robit->GetBranchType() == __BICC) || (_robit->GetBranchType() == __BICC_ANNUL)) ? TRUE : FALSE;

	btaken = _robit->BResult;

	if( (_robit->Psr).IsReqAsSource() )
	  ic_psr = (_robit->Psr).GetSrcValue();
	else
	  ic_psr = 0;

	if( (_robit->Y).IsReqAsSource() )
	  ic_y = (_robit->Y).GetSrcValue();
	else
	  ic_y = 0;

	if( (_robit->Rs1).IsReqAsSource() )
	  ic_src1 = (_robit->Rs1).GetSrcValue();
	else
	  ic_src1 = 0;

	if( (_robit->Rs2).IsReqAsSource() )
	  ic_src2 = (_robit->Rs2).GetSrcValue();
	else
	  ic_src2 = 0;

	if( (_robit->Rd_hi).IsReqAsSource() )
	  ic_rdhi = (_robit->Rd_hi).GetSrcValue();
	else
	  ic_rdhi = 0;

	if( (_robit->Rd_lo).IsReqAsSource() )
	  ic_rdlo = (_robit->Rd_lo).GetSrcValue();
	else
	  ic_rdlo = 0;

	/* atualiza o contexto de saida da instrucao */
	if( !_robit->IsLdStInst() ) {
	  // instrucoes que NAO acessam memoria

	  if( (_robit->Rd_hi).IsReqAsTarget() )
	    oc_rdhi = (_robit->Rd_hi).GetValue();
	  else
	    oc_rdhi = 0;

	  if( (_robit->Rd_lo).IsReqAsTarget() )
	    oc_rdlo = (_robit->Rd_lo).GetValue();
	  else
	    oc_rdlo = 0;

	  if( (_robit->Psr).IsReqAsTarget() )
	    oc_psr = (_robit->Psr).GetValue();
	  else
	    oc_psr = 0;

	  if( (_robit->Y).IsReqAsTarget() )
	    oc_y = (_robit->Y).GetValue();
	  else
	    oc_y = 0;

	  ldst = IOM_NONE;
	  ldsttype = RD_NONE;
	  maddr = 0;
	  mvalid = FALSE;
	  mvalhi = 0;
	  mvallo = 0;

	}
	else {
	  // instrucoes de acesso a memoria

	  oc_rdhi = 0;
	  oc_rdlo = 0;
	  oc_psr = 0;
	  oc_y = 0;

	  maddr = _robit->MAddr;
	  if( _robit->RType != RD_NONE ) {
	    ldst = IOM_LOAD;
	    ldsttype = _robit->RType;
	    mvalid = _robit->MValidVal;
	    //mvalhi = _robit->Rd_hi.GetValue();
	    mvalhi = _robit->MValhi;
	    if( (_robit->Rd_lo).IsReqAsTarget() )
	      //mvallo = _robit->Rd_lo.GetValue();
	      mvallo = _robit->MVallo;
	    else
	      mvallo = 0;
	  }
	  else if( _robit->WType != WR_NONE ) {
	    ldst = IOM_STORE;
	    ldsttype = _robit->WType;
	    mvalid = TRUE;
	    mvalhi = (_robit->Rd_hi).GetSrcValue();
	    if( (_robit->Rd_lo).IsReqAsSource() )
	      mvallo = (_robit->Rd_lo).GetSrcValue();
	    else
	      mvallo = 0;
	  }

	}

	npc = _robit->NewPC;

	if( _robit->RType != RD_NONE ) {
	  // MEMOTABLE_L
	  MemoTableL.AddNew( pc,
			     jmp,
			     brc, 
			     btaken, 
			     ic_src1, 
			     ic_src2,
			     ic_rdhi,
			     ic_rdlo,
			     ic_psr, 
			     ic_y, 
			     oc_rdhi,
			     oc_rdlo, 
			     oc_psr, 
			     oc_y,
			     ldst,
			     ldsttype,
			     maddr,
			     mvalid,
			     mvalhi,
			     mvallo,
			     npc );
	}
	else {
	  // MEMOTABLE_G
	  MemoTableG.AddNew( pc,
			     jmp,
			     brc, 
			     btaken, 
			     ic_src1, 
			     ic_src2,
			     ic_rdhi,
			     ic_rdlo,
			     ic_psr, 
			     ic_y, 
			     oc_rdhi,
			     oc_rdlo, 
			     oc_psr, 
			     oc_y,
			     ldst,
			     ldsttype,
			     maddr,
			     mvalid,
			     mvalhi,
			     mvallo,
			     npc );
	}

#ifndef NO_TRACE_REUSE

	// (2) finaliza o trace em construcao
	
	//////////////////////////////////////////////////////////////////////
	//// contabiliza estatistica das formas de finalizacao dos traces
	if( !EmptyBufferT() ) {
	  Stat->AddTraceTerm(TFN_NREDUNDANT, TSZ_TRACE);
	  BufferT.TraceStats.TFN_traces = TFN_NREDUNDANT;
	}
	else {
	  if(BufferT.Tracesz > 0)
	    Stat->AddTraceTerm(TFN_NREDUNDANT, TSZ_NOT_FORMED);
	}
	CompleteBufferT();

#endif

      }
      else {                   // se instrucao for redundante

#ifndef NO_TRACE_REUSE

	// (3) adiciona instrucao ao trace em construcao

	if( !_robit->IsTraceRedundant() ) {

	  if( !_robit->IsLdStInst() ) {

	    // (4) adiciona instrucao simples

	    if( !ContextOverflow(_robit) && !BranchMaskOverflow(_robit) ) {

	      UpdateBufferT(_robit);

	    }
	    else {

	      //////////////////////////////////////////////////////////////////////
	      //// contabiliza estatistica das formas de finalizacao dos traces
	      if( !EmptyBufferT() ) {
		Stat->AddTraceTerm(TFN_OVERFLOW, TSZ_TRACE);
		BufferT.TraceStats.TFN_traces = TFN_OVERFLOW;
	      }
	      else {
		if(BufferT.Tracesz > 0)
		  Stat->AddTraceTerm(TFN_OVERFLOW, TSZ_NOT_FORMED);
	      }
	      CompleteBufferT();

	      UpdateBufferT(_robit);
	    }

	  }
	  else {

	    // (4) adiciona instrucao de load/store

	    if( _robit->RType != RD_NONE ) {
	      // instrucao de load

	      //////////////////////////////////////////////////////////////////////
	      //// contabiliza estatistica das formas de finalizacao dos traces
	      if( !EmptyBufferT() ) {
		Stat->AddTraceTerm(TFN_LOADSTORE, TSZ_TRACE);
		BufferT.TraceStats.TFN_traces = TFN_LOADSTORE;
	      }
	      else {
		if(BufferT.Tracesz > 0)
		  Stat->AddTraceTerm(TFN_LOADSTORE, TSZ_NOT_FORMED);
	      }
	      CompleteBufferT();  
	    }
	    else {
	      // instrucao de store

	      if( !ContextOverflow(_robit) && !BranchMaskOverflow(_robit) )
		UpdateBufferT(_robit);
	      else {

		//////////////////////////////////////////////////////////////////////
		//// contabiliza estatistica das formas de finalizacao dos traces
		if( !EmptyBufferT() ) {
		  Stat->AddTraceTerm(TFN_OVERFLOW, TSZ_TRACE);
		  BufferT.TraceStats.TFN_traces = TFN_OVERFLOW;
		}
		else {
		  if(BufferT.Tracesz > 0)
		    Stat->AddTraceTerm(TFN_OVERFLOW, TSZ_NOT_FORMED);
		}
		CompleteBufferT();  

		UpdateBufferT(_robit);
	      }

	    }

	  }

	}
	else {

	  // (4) finaliza trace em construcao

	  //////////////////////////////////////////////////////////////////////
	  //// contabiliza estatistica das formas de finalizacao dos traces
	  if( !EmptyBufferT() ) {
	    Stat->AddTraceTerm(TFN_TRACE, TSZ_TRACE);
	    BufferT.TraceStats.TFN_traces = TFN_TRACE;
	  }
	  else {
	    if(BufferT.Tracesz > 0)
	      Stat->AddTraceTerm(TFN_TRACE, TSZ_NOT_FORMED);
	  }
	  CompleteBufferT();

	}

#endif
      }

    }
    else {                           // se instrucao for de delay slot

#ifndef NO_TRACE_REUSE

      // (2) finaliza o trace em construcao
	
      if( !_robit->IsTraceRedundant() ) {

	// (3) finaliza trace em construcao e adiciona instrucao de delay slot

	//////////////////////////////////////////////////////////////////////
	//// contabiliza estatistica das formas de finalizacao dos traces
	if( !EmptyBufferT() ) {
	  Stat->AddTraceTerm(TFN_DELAYINST, TSZ_TRACE);
	  BufferT.TraceStats.TFN_traces = TFN_DELAYINST;
	}
	else {
	  if(BufferT.Tracesz > 0)
	    Stat->AddTraceTerm(TFN_DELAYINST, TSZ_NOT_FORMED);
	}
	CompleteBufferT(_robit->Addr, _robit->Inst);

      }
      else {

	// (3) finaliza trace em construcao

	//////////////////////////////////////////////////////////////////////
	//// contabiliza estatistica das formas de finalizacao dos traces
	if( !EmptyBufferT() ) {
	  Stat->AddTraceTerm(TFN_TRACE, TSZ_TRACE);
	  BufferT.TraceStats.TFN_traces = TFN_TRACE;
	}
	else {
	  if(BufferT.Tracesz > 0)
	    Stat->AddTraceTerm(TFN_TRACE, TSZ_NOT_FORMED);
	}
	CompleteBufferT();

      }

#endif

    }
    
  }
  else {             // se instrucao NAO pertence ao conjunto de instrucoes validas do DTM

#ifndef NO_TRACE_REUSE

    // (4) finaliza o trace em construcao

    if( _robit->IsDelaySlotInst() ) {
      //////////////////////////////////////////////////////////////////////
      //// contabiliza estatistica das formas de finalizacao dos traces
      if( !EmptyBufferT() ) {
	Stat->AddTraceTerm(TFN_DELAYINST, TSZ_TRACE);
	BufferT.TraceStats.TFN_traces = TFN_DELAYINST;
      }
      else {
	if(BufferT.Tracesz > 0)
	  Stat->AddTraceTerm(TFN_DELAYINST, TSZ_NOT_FORMED);
      }
      CompleteBufferT(_robit->Addr, _robit->Inst);
    }
    else {
      //////////////////////////////////////////////////////////////////////
      //// contabiliza estatistica das formas de finalizacao dos traces
      if( !EmptyBufferT() ) {
	Stat->AddTraceTerm(TFN_NVALID, TSZ_TRACE);
	BufferT.TraceStats.TFN_traces = TFN_NVALID;
      }
      else {
	if(BufferT.Tracesz > 0)
	  Stat->AddTraceTerm(TFN_NVALID, TSZ_NOT_FORMED);
      }
      CompleteBufferT();
    }

#endif

  }

  return _robit->IsRedundant();
}

void CDtm::Invalidate(uword_t _ptr, uword_t _len)
{
  if( !IsEnabled() ) return;
  MemoTableG.Invalidate(_ptr, _len);
#ifndef NO_TRACE_REUSE
  MemoTableT.Invalidate(_ptr, _len);
#endif
}

void CDtm::UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len)
{
  if( !IsEnabled() ) return;
  MemoTableG.UpdateMemValue(_maddr, _ptr, _len);
  MemoTableL.UpdateMemValue(_maddr, _ptr, _len);
#ifndef NO_TRACE_REUSE
  MemoTableT.UpdateMemValue(_maddr, _ptr, _len);
#endif
}

void CDtm::Init( CStat* _stat,
		 CBtb* _btb,
		 CFetchBuf* _fetchbuf,
		 CFetch* _fetch,
		 CIQueue* _iqueue,
		 CDisq* _disq,
		 CDispatch* _dispatch,
		 CSReg* _sreg,
		 CIReg* _ireg,
		 CRs* _rslsu,
		 CMaq* _maq,
		 CLsu* _lsu,
		 CRob* _rob )
{
  Stat = _stat;
  Btb = _btb;
  FetchBuf = _fetchbuf;
  Fetch = _fetch;
  IQueue = _iqueue;
  Disq = _disq;
  Dispatch = _dispatch;
  SReg = _sreg;
  IReg = _ireg;
  RsLsu = _rslsu;
  Maq = _maq;
  Lsu = _lsu;
  Rob = _rob;
}

void CDtm::Reset()
{
  for(int i = 0; i < IREG_NGREGS + (IREG_WINOFFSET * IREG_NUMWIN); i++)
    DepMap[i] = 0;

  DepMapY = 0;

  DepMapPSR = 0;
}

void CDtm::ShowMemoTableG()
{
  CMemoTableEntryG* p;
  int i;

  printf("\n\nMEMO_TABLE_G");
  printf("\n============\n");

  printf("\n => MRU");

  MemoTableG.GoTop();
  for(i = 0; (i < 20) && !MemoTableG.AtEnd(); i++) {
    (MemoTableG.GetCurrent())->Display();
    MemoTableG.MoveNext();
  }

  if((p = MemoTableG.GetCurrent()) != 0)
    p->Display();

  printf("\n => LRU\n\n");
}

void CDtm::ShowMemoTableT()
{
  CMemoTableEntryT* p;
  int i;

  printf("\n\nMEMO_TABLE_T");
  printf("\n============\n");

  printf("\n => MRU");

  MemoTableT.GoTop();
  for(i = 0; (i < 20) && !MemoTableT.AtEnd(); i++) {
    (MemoTableT.GetCurrent())->Display();
    MemoTableT.MoveNext();
  }

  if((p = MemoTableT.GetCurrent()) != 0)
    p->Display();

  printf("\n => LRU\n\n");
}

void CDtm::ShowMemoTableL()
{
  CMemoTableEntryL* p;
  int i;

  printf("\n\nMEMO_TABLE_L");
  printf("\n============\n");

  printf("\n => MRU");

  MemoTableL.GoTop();
  for(i = 0; (i < 20) && !MemoTableL.AtEnd(); i++) {
    (MemoTableL.GetCurrent())->Display();
    MemoTableL.MoveNext();
  }

  if((p = MemoTableL.GetCurrent()) != 0)
    p->Display();

  printf("\n => LRU\n\n");
}

//////////////////////////////////////////////////////////////////
//// contabilacao estatistica de traces dependentes de loads

void CDtm::SetDepMap(CRobItem* _robit)
{
  CMemoTableEntryT * q = NULL;
  bool_t opt = TLD_NOLOAD;

  if( !_robit->IsTraceRedundant() ) {
    // se instrucao nao for inicio de um trace redundante

    if( _robit->RType != RD_NONE ) {
      if( _robit->IsRedundant() )
	opt = TLD_REUSED_LOAD;
      else
	opt = TLD_INST_LOAD;
    }

    if( (_robit->Y).IsReqAsTarget() )
      DepMapY = opt;
    
    if( (_robit->Psr).IsReqAsTarget() )
      DepMapPSR = opt;
    
    if( (_robit->Rs1).IsReqAsTarget() )
      DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), (_robit->Rs1).GetRegAddr()) ] = opt;
    
    if( (_robit->Rs2).IsReqAsTarget() )
      DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), (_robit->Rs2).GetRegAddr()) ] = opt;
    
    if( (_robit->Rd_hi).IsReqAsTarget() )
      DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), (_robit->Rd_hi).GetRegAddr()) ] = opt;
    
    if( (_robit->Rd_lo).IsReqAsTarget() )
      DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), (_robit->Rd_lo).GetRegAddr()) ] = opt;
  }
  else {
    // se instrucao e inicio de um trace redundante
    q = & _robit->TraceData;
    for(int i = 0; i < OC_NUM; i++) {
      switch( q->Oc[i].regtype ) {
      case CTX_PSR:
	DepMapPSR = TLD_NOLOAD; 
	break;
      case CTX_Y:
	DepMapY = TLD_NOLOAD; 
	break;
      case CTX_GPR:
	DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), q->Oc[i].regaddr) ] = TLD_NOLOAD; 
	break;
      };
    }

    for(int i = 0; i < q->LdStSz; i++) {
      if(((q->LdStInst[i]).LdSt & IOM_LOAD) != 0) {
	if( (q->LdStInst[i]).Rdhi.IsReqAsTarget() )
	  DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), (q->LdStInst[i]).Rdhi.GetRegAddr()) ] = TLD_TRACE_LOAD;
	
	if( (q->LdStInst[i]).Rdlo.IsReqAsTarget() )
	  DepMap[ IREG_GETABSADDR(_robit->GetTargetWin(), (q->LdStInst[i]).Rdlo.GetRegAddr()) ] = TLD_TRACE_LOAD;
      }

    }

  }

}

int CDtm::CheckDepMap(CRobItem* _robit)
{
  CMemoTableEntryT * q = & _robit->TraceData;
  int dep = 0;

  // verifica dependencia  comparando o contexto de entrada
  for(int i = 0; i < IC_NUM; i++) {
    switch( q->Ic[i].regtype ) {
    case CTX_PSR:
      if(DepMapPSR > dep) 
	dep = DepMapPSR;
      break;
    case CTX_Y:
      if(DepMapY > dep) 
	dep = DepMapY;
      break;
    case CTX_GPR:
      if(DepMap[ IREG_GETABSADDR(_robit->GetSourceWin(), q->Ic[i].regaddr) ] > dep) 
	dep = DepMap[ IREG_GETABSADDR(_robit->GetSourceWin(), q->Ic[i].regaddr) ];
      break;
    };
  }

  return dep;
}
