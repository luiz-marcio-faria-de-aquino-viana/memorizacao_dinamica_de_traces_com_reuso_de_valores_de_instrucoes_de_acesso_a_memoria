
/*
 * dtm.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/17/2000
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////////
///// CDtm

CDtm::CDtm(uword_t _memotbgsz, uword_t _memotbtsz)
  : CComponent(__DTM_H),
    MemoTableG(_memotbgsz),
    MemoTableT(_memotbtsz)
{
  Enabled = FALSE;
  ClearBufferT();
}

bool_t CDtm::IsEnabled()
{
  return Enabled;
}

void CDtm::SetEnabled()
{
  Enabled = TRUE;
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
    case LDSB:                  /* load integer instructions (byte) */
    case LDSH:                  /* load integer instructions (half word) */
    case LDUB:                  /* load integer instructions (unsigned byte) */
    case LDUH:                  /* load integer instructions (unsigned half word) */
    case LD:                    /* load integer instructions (word) */
    case LDD:                   /* load integer instructions (double word) */
    case LDF:                   /* load floating-point instructions */
    case LDDF:
    case LDFSR:
    case LDC:                   /* load coprocessor instructions */
    case LDDC:
    case LDCSR:
    case STBA:                  /* store integer instructions to alternate space */
    case STHA:
    case STA:
    case STDA:
    case STB:                   /* store integer instructions (byte) */
    case STH:                   /* store integer instructions (hword) */
    case ST:                    /* store integer instructions (word) */
    case STD:                   /* store integer instructions (dword) */
    case STF:                   /* store floating-point instructions */
    case STDF:
    case STFSR:
    case STDFQ:
    case STC:                   /* store coprocessor instructions */
    case STDC:
    case STCSR:
    case STDCQ:
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
      BufferT.NewInputContext(CTX_GPR, (_robit->Rs1).GetRegAddr(), (_robit->RedundantInst).Ic_src1);
      Ic_IRegMap[(_robit->Rs1).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rs2).IsReqAsSource() &&
	(Ic_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) &&
	(Oc_IRegMap[(_robit->Rs2).GetRegAddr()] == FALSE) ) {
      BufferT.NewInputContext(CTX_GPR, (_robit->Rs2).GetRegAddr(), (_robit->RedundantInst).Ic_src2);
      Ic_IRegMap[(_robit->Rs2).GetRegAddr()] = TRUE;
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
      BufferT.NewOutputContext(CTX_GPR, (_robit->Rs1).GetRegAddr(), (_robit->RedundantInst).Oc_rd);
      Oc_IRegMap[(_robit->Rs1).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rs2).IsReqAsTarget() ) {
      BufferT.NewOutputContext(CTX_GPR, (_robit->Rs2).GetRegAddr(), (_robit->RedundantInst).Oc_rd);
      Oc_IRegMap[(_robit->Rs2).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rd_lo).IsReqAsTarget() ) {
      BufferT.NewOutputContext(CTX_GPR, (_robit->Rd_lo).GetRegAddr(), (_robit->RedundantInst).Oc_rd);
      Oc_IRegMap[(_robit->Rd_lo).GetRegAddr()] = TRUE;
    }
    
    if( (_robit->Rd_hi).IsReqAsTarget() ) {
      BufferT.NewOutputContext(CTX_GPR, (_robit->Rd_hi).GetRegAddr(), (_robit->RedundantInst).Oc_rd);
      Oc_IRegMap[(_robit->Rd_hi).GetRegAddr()] = TRUE;
    }

    // BufferT: atualiza a mascara de padroes de desvios do trace

    if( _robit->IsBranchInst() ) {
#if(DEBUG <= 7)
      fprintf(stderr, "\nDTM::UPDATE_BUFFER_T::BRANCH_INST(%u, %hu)", _robit->GetBranchType(), _robit->BResult);
#endif
      switch( _robit->GetBranchType() ) {
      case __JUMPL:
	BufferT.AddBranchMask(__JUMPL, TAKEN);
	break;
      case __BICC:
      case __BICC_ANNUL:
	BufferT.AddBranchMask(_robit->GetBranchType(), _robit->BResult);
      };
    }

    // BufferT: atualiza nPC para instrucoes de delay slot
    
    if( !_robit->IsDelaySlotInst() )
      BufferT.nPC = ( _robit->IsBranchInst() ) ? _robit->NewPC : _robit->Addr + 4;    

    BufferT.Tracesz += 1;

  }

#if(DEBUG <= 6)

  if(BufferT.PC == 0x6b5c){

    printf("\nDTM::UPDATE_BUFFER_T(PC=%lx)", _robit->Addr);

    printf("\n%4lu %4lu (%08lx) [sz=%ld] { ic(", MemoTableT.NextId, BufferT.Count, BufferT.PC, BufferT.Tracesz);

    for(int j = 0 ; j < IC_NUM; j++) {
      if(BufferT.Ic[j].regtype == CTX_NONE) break;
    
      switch( BufferT.Ic[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", BufferT.Ic[j].regval);
	break;
      case CTX_Y:
	printf("y=%lx ", BufferT.Ic[j].regval);
	break;
      case CTX_GPR:
	printf("r%ld=%lx ", BufferT.Ic[j].regaddr, BufferT.Ic[j].regval);
      }
    }

    printf(") oc(");
  
    for(int j = 0 ; j < OC_NUM; j++) {
      if(BufferT.Oc[j].regtype == CTX_NONE) break;
      
      switch( BufferT.Oc[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", BufferT.Oc[j].regval);
	break;
      case CTX_Y:
	printf("y=%lx ", BufferT.Oc[j].regval);
	break;
      case CTX_GPR:
	printf("r%ld=%lx ", BufferT.Oc[j].regaddr, BufferT.Oc[j].regval);
      }
    }

    printf(") npc=%08lx }", BufferT.nPC);

    if( BufferT.BiccAtEnd() )
      printf(", DI");

    getchar();

  }

#endif

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

bool_t CDtm::MatchBranchPattern(CMemoTableEntryT* p)
{
  CFetchBufItem* fb = 0;
  CIQueueItem* iq = 0;
  CDisqItem* di = 0;

  uword_t btype;

  int pos = 0;
  uword_t i;

  uword_t n = 0;

  /* pesquisa fila de despacho */

  for(i = 0; i < Disq->GetQueueSize(); i++) {
    // loop. pesquisa todas as instrucoes da fila de despacho

    if(n >= p->Tracesz) return TRUE;

    di = Disq->GetNthItem(i);

    if( di->IsBranchDep() ) {
      di->SetInvalidBit();
#if(DEBUG <= 8)
    fprintf( stderr,
	     "\nDTM::MATCH_BRANCH_PATTERN-DISQ(pc=%08lx, inv=%s)",
	     di->Addr,
	     (di->IsInvalid() ? "TRUE" : "FALSE") );
#endif
      continue;
    }

    if( di->IsBranchInst() ) {
      // #1a. se instrucao e de desvio

      btype = di->GetBranchType();

      if(btype == __JUMPL) { 
	// #2a. se jumpl retorna falso para efetuar flush

	if( (pos >= p->BrcSz) && (p->Brc[pos].btype != btype) )
	  errmsg(__DTM_H, ERR_SIMARCHFAIL, "(MatchBranchPattern-disq-jumpl)");

	return FALSE;

	// #2a.
      }

      if((btype == __BICC) || (btype == __BICC_ANNUL)) {
	// #3a. se bicc/bicc,a verifica resultado da predicao de desvio

	if( (pos >= p->BrcSz) && (p->Brc[pos].btype != btype) )
	  errmsg(__DTM_H, ERR_SIMARCHFAIL, "(MatchBranchPattern-disq-bicc)");

	if(di->IsPredTaken() != p->Brc[pos].btaken)  // se nao coincide retorna falso
	  return FALSE;

	pos += 1;
	
	// #3a.
      }

      // #1a.
    }

    di->SetInvalidBit();
#if(DEBUG <= 8)
    fprintf( stderr,
	     "\nDTM::MATCH_BRANCH_PATTERN-DISQ(pc=%08lx, inv=%s)",
	     di->Addr,
	     (di->IsInvalid() ? "TRUE" : "FALSE") );
#endif

    n += 1;

    // #loop.
  }


  /* pesquisa fila de instrucoes */

  for(i = 0; i < IQueue->GetQueueSize(); i++) {
    // loop. pesquisa todas as instrucoes da fila de instrucoes

    if(n >= p->Tracesz) return TRUE;

    iq = IQueue->GetNthItem(i);

    if( iq->IsBranchDep() ) {
      iq->SetInvalidBit();
#if(DEBUG <= 8)
    fprintf( stderr,
	     "\nDTM::MATCH_BRANCH_PATTERN-DISQ(pc=%08lx, inv=%s)",
	     iq->Addr,
	     (iq->IsInvalid() ? "TRUE" : "FALSE") );
#endif
      continue;
    }

    if( iq->IsBranchInst() ) {
      // #1a. se instrucao e de desvio

      btype = iq->GetBranchType();

      if(btype == __JUMPL) {
	// #2a. se jumpl retorna falso para efetuar flush

	if( (pos >= p->BrcSz) && (p->Brc[pos].btype != btype) )
	  errmsg(__DTM_H, ERR_SIMARCHFAIL, "(MatchBranchPattern-iqueue-jumpl)");

	return FALSE;

	// #2a.
      }

      if((btype == __BICC) || (btype == __BICC_ANNUL)) {
	// #3a. se bicc/bicc,a verifica resultado da predicao de desvio
	
	if( (pos >= p->BrcSz) && (p->Brc[pos].btype != btype) )
	  errmsg(__DTM_H, ERR_SIMARCHFAIL, "(MatchBranchPattern-iqueue-bicc)");

	if(iq->IsPredTaken() != p->Brc[pos].btaken)  // se nao coincide retorna falso
	  return FALSE;

	pos += 1;
	
	// #3a.
      }

      // #1a.
    }

    iq->SetInvalidBit();
#if(DEBUG <= 8)
    fprintf( stderr,
	     "\nDTM::MATCH_BRANCH_PATTERN-IQUEUE(pc=%08lx, inv=%s)",
	     iq->Addr,
	     (iq->IsInvalid() ? "TRUE" : "FALSE") );
#endif

    n += 1;

    // #loop.
  }


  /* pesquisa fetch buffer */

  for(i = 0; i < FetchBuf->GetQueueSize(); i++) {
    // loop. pesquisa todas as instrucoes no fetch buffer

    if(n >= p->Tracesz) return TRUE;

    fb = FetchBuf->GetNthItem(i);

    if( fb->IsBranchDep() ) {
      fb->SetInvalidBit();
#if(DEBUG <= 8)
    fprintf( stderr,
	     "\nDTM::MATCH_BRANCH_PATTERN-DISQ(pc=%08lx, inv=%s)",
	     fb->Addr,
	     (fb->IsInvalid() ? "TRUE" : "FALSE") );
#endif
      continue;
    }

    if( fb->IsBranchInst() ) {
      // #1a. se instrucao e de desvio

      btype = fb->GetBranchType();

      if(btype == __JUMPL) {
	// #2a. se jumpl retorna falso para efetuar flush

	if( (pos >= p->BrcSz) && (p->Brc[pos].btype != btype) )
	  errmsg(__DTM_H, ERR_SIMARCHFAIL, "(MatchBranchPattern-fetchbuf-jumpl)");

	return FALSE;

	// #2a.
      }

      if((btype == __BICC) || (btype == __BICC_ANNUL)) {
	// #3a. se bicc/bicc,a verifica resultado da predicao de desvio

	if( (pos >= p->BrcSz) && (p->Brc[pos].btype != btype) )
	  errmsg(__DTM_H, ERR_SIMARCHFAIL, "(MatchBranchPattern-fetchbuf-bicc)");

	if(fb->IsPredTaken() != p->Brc[pos].btaken)  // se nao coincide retorna falso
	  return FALSE;

	pos += 1;
	
	// #3a.
      }

      // #1a.
    }

    fb->SetInvalidBit();
#if(DEBUG <= 8)
    fprintf( stderr,
	     "\nDTM::MATCH_BRANCH_PATTERN-FETCHBUF(pc=%08lx, inv=%s)",
	     fb->Addr,
	     (fb->IsInvalid() ? "TRUE" : "FALSE") );
#endif

    n += 1;

    // #loop.
  }

  return FALSE;

  /*
   * obs: Sempre que todas as instrucoes do pipeline sao invalidadas e realizado
   * um flush para redirecionar o PC ao endereco correto
   */

}

void CDtm::CompleteBufferT(uword_t _daddr, uword_t _dinst)
{
  if( !EmptyBufferT() ) {
    BufferT.AddDelayInst(_daddr, _dinst);
    MemoTableT.AddNew(BufferT);

#if(DEBUG <= 6)

    if(BufferT.PC == 0x6b5c){

      printf("\nDTM::COMPLETE_BUFFER_T(daddr=%lx, dinst=%08lx)", _daddr, _dinst);
    
      printf("\n%4lu %4lu (%08lx) [sz=%ld] { ic(", MemoTableT.NextId, BufferT.Count, BufferT.PC, BufferT.Tracesz);
    
      for(int j = 0 ; j < IC_NUM; j++) {
	if(BufferT.Ic[j].regtype == CTX_NONE) break;
      
	switch( BufferT.Ic[j].regtype ) {
	case CTX_PSR:
	  printf("psr=%lx ", BufferT.Ic[j].regval);
	  break;
	case CTX_Y:
	  printf("y=%lx ", BufferT.Ic[j].regval);
	  break;
	case CTX_GPR:
	  printf("r%ld=%lx ", BufferT.Ic[j].regaddr, BufferT.Ic[j].regval);
	}
      }
    
      printf(") oc(");
    
      for(int j = 0 ; j < OC_NUM; j++) {
	if(BufferT.Oc[j].regtype == CTX_NONE) break;
	
	switch( BufferT.Oc[j].regtype ) {
	case CTX_PSR:
	  printf("psr=%lx ", BufferT.Oc[j].regval);
	  break;
	case CTX_Y:
	  printf("y=%lx ", BufferT.Oc[j].regval);
	  break;
	case CTX_GPR:
	  printf("r%ld=%lx ", BufferT.Oc[j].regaddr, BufferT.Oc[j].regval);
	}
      }

      printf(") npc=%08lx }", BufferT.nPC);

      if( BufferT.BiccAtEnd() )
	printf(", DI");
      
      getchar();

    }

#endif
    
  }
  ClearBufferT();
}

void CDtm::CompleteBufferT()
{
  if( !EmptyBufferT() ) {
    MemoTableT.AddNew(BufferT);

#if(DEBUG <= 6)

    if(BufferT.PC == 0x6b5c){

      printf("\nDTM::COMPLETE_BUFFER_T");
    
      printf("\n%4lu %4lu (%08lx) [sz=%ld] { ic(", MemoTableT.NextId, BufferT.Count, BufferT.PC, BufferT.Tracesz);
    
      for(int j = 0 ; j < IC_NUM; j++) {
	if(BufferT.Ic[j].regtype == CTX_NONE) break;
      
	switch( BufferT.Ic[j].regtype ) {
	case CTX_PSR:
	  printf("psr=%lx ", BufferT.Ic[j].regval);
	  break;
	case CTX_Y:
	  printf("y=%lx ", BufferT.Ic[j].regval);
	  break;
	case CTX_GPR:
	  printf("r%ld=%lx ", BufferT.Ic[j].regaddr, BufferT.Ic[j].regval);
	}
      }
    
      printf(") oc(");
    
      for(int j = 0 ; j < OC_NUM; j++) {
	if(BufferT.Oc[j].regtype == CTX_NONE) break;
      
	switch( BufferT.Oc[j].regtype ) {
	case CTX_PSR:
	  printf("psr=%lx ", BufferT.Oc[j].regval);
	  break;
	case CTX_Y:
	  printf("y=%lx ", BufferT.Oc[j].regval);
	  break;
	case CTX_GPR:
	  printf("r%ld=%lx ", BufferT.Oc[j].regaddr, BufferT.Oc[j].regval);
	}
      }

      printf(") npc=%08lx }", BufferT.nPC);

      if( BufferT.BiccAtEnd() )
	printf(", DI");

    }

#endif

  }
  ClearBufferT();
}

bool_t CDtm::ContextOverflow(CRobItem* _robit)
{
  uword_t icreq, ocreq;
  int i;

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

  for(i = 0; (icreq > 0) && (i < IC_NUM); i++)
    if((BufferT.Ic[i]).regtype == CTX_NONE) icreq -= 1;

  if(icreq != 0)
    return TRUE;

  // verifica se existe espaco para o contexto de saida

  ocreq = 0;

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

  for(i = 0; (ocreq > 0) && (i < OC_NUM); i++)
    if((BufferT.Oc[i]).regtype == CTX_NONE) ocreq -= 1;

  if(ocreq != 0)
    return TRUE;

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

bool_t CDtm::Stage_DS1(CFetchBufItem* _fb)
{

  if(Enabled != TRUE) return FALSE;

  if(
#ifndef NO_TRACE_REUSE
     (MemoTableT.FindFirst(_fb->Addr) != 0) ||
#endif
     (MemoTableG.FindFirst(_fb->Addr) != 0)
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

  bool_t redundant = FALSE;

  CRobItem* robit;
  bool_t chgpc = FALSE;
  
  bool_t bres = NTAKEN;
  int i;

  if(Enabled != TRUE) return FALSE;

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

      _di->SetTraceRedundant();    // marca a instrucao como sendo inicio de um trace reusado

      // (1) insere trace no reorder buffer
 
      robit = Rob->InsertItem(_di);
      robit->SetTargetWin( IReg->GetCurrentWin() );


      // (2) atualiza contexto de saida

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


      // (3) verifica se padrao de desvios coincide com fluxo no pipeline

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

      // (4) atualiza reorder buffer

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

    if(redundant == TRUE) {                    // se instrucao e redundante

      _di->SetRedundant();


      // (1) insere instrucao no reorder buffer
 
      robit = Rob->InsertItem(_di);
      robit->SetTargetWin( IReg->GetCurrentWin() );

      robit->RedundantInst = (*p);             // atribui as instrucoes redundantes ao Reorder Buffer


      // (2) atualiza registradores destino

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
	IReg->SetBusy(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr(), _di->GetSeqNum());
	IReg->SetFValue(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr(), p->Oc_rd);
	IReg->ClearBusy(IReg->GetCurrentWin(), (_di->Rd_hi).GetRegAddr());
      }


      // (3) atualiza pipeline para instrucoes de desvio

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
	else if(_di->GetBranchType() == __JUMPL) { // instrucao de descio incondicional 

	  // atualiza a BTB
	
	  Btb->Update(_di->Addr, & _di->BPred, & p->nPC);
	
	  if(p->nPC != _di->BAddr) {               // endereco alvo previsto incorretamente

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

      // (4) atualiza reorder buffer

      Rob->Update( _di->GetSeqNum(),
		   p->Oc_psr,
		   p->Oc_y,
		   0,
		   0,
		   p->Oc_rd,
		   0,
		   FALSE,
		   0,
		   chgpc,
		   p->nPC,
		   0,
		   bres );

      MemoTableG.Update(p);

      return redundant;

    }

    p = MemoTableG.FindNext(_di->Addr);
  }

  return redundant;
}

bool_t CDtm::Stage_DS3_TraceReuse(CRobItem* _robit)
{
  int i;

#ifndef NO_TRACE_REUSE

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

#endif

  return TRUE;
}

bool_t CDtm::Stage_DS3_TraceConstruction(CRobItem* _robit)
{
  uword_t pc, npc;
  
  bool_t jmp, brc, btaken;
  
  uword_t ic_src1, ic_src2, ic_y;
  uword_t ic_psr;
  
  uword_t oc_rd, oc_y;
  uword_t oc_psr;

  if(Enabled != TRUE) return FALSE;

  if( IsValidDtmInst(_robit) ) {  // se instrucao pertence ao dominio de instrucoes validas do DTM

    if( !_robit->IsDelaySlotInst() ) {  // se instrucao NAO for de delay slot

      if( !_robit->IsRedundant() && !_robit->IsDependentRedundant() ) {
	// se instrucao NAO for redundante

	// (1) adiciona instrucao a MEMO_TABLE_G

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

	if( (_robit->Rd_hi).IsReqAsTarget() )
	  oc_rd = (_robit->Rd_hi).GetValue();
	else
	  oc_rd = 0;

	if( (_robit->Psr).IsReqAsTarget() )
	  oc_psr = (_robit->Psr).GetValue();
	else
	  oc_psr = 0;

	if( (_robit->Y).IsReqAsTarget() )
	  oc_y = (_robit->Y).GetValue();
	else
	  oc_y = 0;

	npc = _robit->NewPC;

	MemoTableG.AddNew( pc,
			   jmp,
			   brc, 
			   btaken, 
			   ic_src1, 
			   ic_src2, 
			   ic_psr, 
			   ic_y, 
			   oc_rd, 
			   oc_psr, 
			   oc_y, 
			   npc );

#ifndef NO_TRACE_REUSE

	// (2) finaliza o trace em construcao
	
	CompleteBufferT();

#endif

      }
      else {                   // se instrucao for redundante

#ifndef NO_TRACE_REUSE

	// (3) adiciona instrucao ao trace em construcao

	if( !_robit->IsTraceRedundant() ) {
	  if( !ContextOverflow(_robit) && !BranchMaskOverflow(_robit) )
	    UpdateBufferT(_robit);
	  else {
	    CompleteBufferT();
	    UpdateBufferT(_robit);
	  }
	}
	else {
	  CompleteBufferT();
	}

#endif
      }

    }
    else {                           // se instrucao for de delay slot

#ifndef NO_TRACE_REUSE

      // (2) finaliza o trace em construcao
	
      if( !_robit->IsTraceRedundant() ) {
	CompleteBufferT(_robit->Addr, _robit->Inst);
      }
      else {
	CompleteBufferT();
      }

#endif

    }
    
  }
  else {                          // se instrucao NAO pertence ao conjunto de instrucoes validas do DTM

#ifndef NO_TRACE_REUSE

    // (4) finaliza o trace em construcao

    if( _robit->IsDelaySlotInst() )
      CompleteBufferT(_robit->Addr, _robit->Inst);
    else
      CompleteBufferT();

#endif

  }

  return _robit->IsRedundant();
}


void CDtm::Init( CBtb* _btb,
		 CFetchBuf* _fetchbuf,
		 CFetch* _fetch,
		 CIQueue* _iqueue,
		 CDisq* _disq,
		 CSReg* _sreg,
		 CIReg* _ireg,
		 CRob* _rob )
{
  Btb = _btb;
  FetchBuf = _fetchbuf;
  Fetch = _fetch;
  IQueue = _iqueue;
  Disq = _disq;
  SReg = _sreg;
  IReg = _ireg;
  Rob = _rob;
}

void CDtm::ShowMemoTableG()
{
  CMemoTableEntryG* p;

  printf("\n\nMEMO_TABLE_G");
  printf("\n============\n");

  printf("\n => MRU");

  MemoTableG.GoTop();
  for(int i = 0; (i < 20) && !MemoTableG.AtEnd(); i++) {
    p = MemoTableG.GetCurrent();

    printf("\n%4lu (%08lx) ", p->Count, p->PC);

    if(p->JMP == TRUE)
      printf("J ");
    else
      printf("- ");

    if(p->BRC == TRUE) {
      if(p->BTaken == TRUE)
	printf("BT ");
      else
	printf("BN ");
    }
    else
      printf("-- ");

    printf( "{ ic(src1=%lx, src2=%lx, psr=%lx, y=%lx), oc(rd=%lx, psr=%lx, y=%lx), npc=%lx }",
	    p->Ic_src1,
	    p->Ic_src2,
	    p->Ic_psr,
	    p->Ic_y,
	    p->Oc_rd,
	    p->Oc_psr,
	    p->Oc_y,
	    p->nPC );

    MemoTableG.MoveNext();
  }

  if((p = MemoTableG.GetCurrent()) != 0) {
    printf("\n%4lu (%08lx) ", p->Count, p->PC);

    if(p->JMP == TRUE)
      printf("J ");
    else
      printf("- ");

    if(p->BRC == TRUE) {
      if(p->BTaken == TRUE)
	printf("BT ");
      else
	printf("BN ");
    }
    else
      printf("-- ");

    printf( "{ ic(src1=%lx, src2=%lx, psr=%lx, y=%lx), oc(rd=%lx, psr=%lx, y=%lx), npc=%lx }",
	    p->Ic_src1,
	    p->Ic_src2,
	    p->Ic_psr,
	    p->Ic_y,
	    p->Oc_rd,
	    p->Oc_psr,
	    p->Oc_y,
	    p->nPC );

  }

  printf("\n => LRU\n\n");
}

void CDtm::ShowMemoTableT()
{
  CMemoTableEntryT* p;
  int i, j;

  printf("\n\nMEMO_TABLE_T");
  printf("\n============\n");

  printf("\n => MRU");

  MemoTableT.GoTop();
  for(i = 0; (i < 20) && !MemoTableT.AtEnd(); i++) {
    p = MemoTableT.GetCurrent();

    printf("\n%4lu %4lu (%08lx) [sz=%ld] ", p->Id, p->Count, p->PC, p->Tracesz);

    for(j = 0; j < BRC_NUM; j++) {
      if(j < p->BrcSz) {
	if(p->Brc[j].btype == __JUMPL)
	  printf("J");
	else {
	  if(p->Brc[i].btaken == TRUE) 
	    printf("T");
	  else
	    printf("N");
	}
      }
      else
	printf(".");
    }

    printf(" { ic(");

    for(j = 0 ; j < IC_NUM; j++) {
      if(p->Ic[j].regtype == CTX_NONE) break;

      switch( p->Ic[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", p->Ic[j].regval);
	break;
      case CTX_Y:
	printf("y=%lx ", p->Ic[j].regval);
	break;
      case CTX_GPR:
	printf("r%ld=%lx ", p->Ic[j].regaddr, p->Ic[j].regval);
      }
    }

    printf(") oc(");

    for(j = 0 ; j < OC_NUM; j++) {
      if(p->Oc[j].regtype == CTX_NONE) break;

      switch( p->Oc[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", p->Oc[j].regval);
	break;
      case CTX_Y:
	printf("y=%lx ", p->Oc[j].regval);
	break;
      case CTX_GPR:
	printf("r%ld=%lx ", p->Oc[j].regaddr, p->Oc[j].regval);
      }
    }

    printf(", npc=%08lx) }", p->nPC);

    if( p->BiccAtEnd() )
      printf(", DI");

    MemoTableT.MoveNext();
  }

  if((p = MemoTableT.GetCurrent()) != 0) {
    printf("\n%4lu %4lu (%08lx) [sz=%ld] ", p->Id, p->Count, p->PC, p->Tracesz);

    for(j = 0; j < BRC_NUM; j++) {
      if(j < p->BrcSz) {
	if(p->Brc[j].btype == __JUMPL)
	  printf("J");
	else {
	  if(p->Brc[i].btaken == TRUE) 
	    printf("T");
	  else
	    printf("N");
	}
      }
      else
	printf(".");
    }

    printf(" { ic(");

    for(j = 0 ; j < IC_NUM; j++) {
      if(p->Ic[j].regtype == CTX_NONE) break;
      
      switch( p->Ic[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", p->Ic[j].regval);
	break;
      case CTX_Y:
	printf("y=%lx ", p->Ic[j].regval);
	break;
      case CTX_GPR:
	printf("r%ld=%lx ", p->Ic[j].regaddr, p->Ic[j].regval);
      }
    }

    printf(") oc(");

    for(j = 0 ; j < OC_NUM; j++) {
      if(p->Oc[j].regtype == CTX_NONE) break;

      switch( p->Oc[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", p->Oc[j].regval);
	break;
      case CTX_Y:
	printf("y=%lx ", p->Oc[j].regval);
	break;
      case CTX_GPR:
	printf("r%ld=%lx ", p->Oc[j].regaddr, p->Oc[j].regval);
      }
    }

    printf(", npc=%08lx) }", p->nPC);

    if( p->BiccAtEnd() )
      printf(", DI");
  }

  printf("\n => LRU\n\n");
}
