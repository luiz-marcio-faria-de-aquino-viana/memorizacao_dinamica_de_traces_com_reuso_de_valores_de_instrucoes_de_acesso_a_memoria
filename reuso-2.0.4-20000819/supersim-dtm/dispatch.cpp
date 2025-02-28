
/*
 * dispatch.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/24/99
 */

#include<stdio.h>
#include"all.h"

//////////////////////////////////////////////////////////////////
///// CDispatch

void CDispatch::Init( CComponent* _dtm,
		      CDisq* _disq,
		      CDisq* _dbdisq,
		      CSReg* _sreg,
		      CIReg* _ireg,
		      CRs* _rsiu, 
		      CRs* _rsbpu,
		      CRs* _rslsu,
		      CMaq* _maq,
		      CRob* _rob )
{
  Dtm = _dtm;

  Disq = _disq;

  DBDisq = _dbdisq;

  SReg = _sreg;

  IReg = _ireg;

  RsLsu = _rslsu;

  RsBpu = _rsbpu;

  RsIu = _rsiu;

  Rob = _rob;

  Maq = _maq;
}

CDispatch::CDispatch()
{
  Reset();
}

int CDispatch::HaveDependence(CDisqItem* _di)
{
  int rst = FALSE;
  int i;

  if( (_di->Psr).IsReqAsSource() && (MapPsr == 0x01) )
    rst = TRUE;
  else {
    if( (_di->Y).IsReqAsSource() && (MapY == 0x01) )
      rst = TRUE;
    else {
      for(i = 0; i < DISPATCH_WIDTH; i++) {
	if( (_di->Rs1).IsReqAsSource() && (MapR[ i ] == (_di->Rs1).GetRegAddr()) ) {
	  rst = TRUE;
	  break;
	}

	if( (_di->Rs2).IsReqAsSource() && (MapR[ i ] == (_di->Rs2).GetRegAddr()) ) {
	  rst = TRUE;
	  break;
	}

	if( (_di->Rd_hi).IsReqAsSource() && (MapR[ i ] == (_di->Rd_hi).GetRegAddr()) ) {
	  rst = TRUE;
	  break;
	}

	if( (_di->Rd_lo).IsReqAsSource() && (MapR[ i ] == (_di->Rd_lo).GetRegAddr()) ) {
	  rst = TRUE;
	  break;
	}

      }
    }
  }

  if( (_di->Psr).IsReqAsTarget() )
    MapPsr = 0x01;

  if( (_di->Y).IsReqAsTarget() )
    MapY = 0x01;

  for(i = 0; i < DISPATCH_WIDTH; i++) {
    if( (_di->Rs1).IsReqAsTarget() )
      MapR[ i ] = (_di->Rs1).GetRegAddr();
    else if( (_di->Rs2).IsReqAsTarget() )
      MapR[ i ] = (_di->Rs2).GetRegAddr();
    else if( (_di->Rd_hi).IsReqAsTarget() )
      MapR[ i ] = (_di->Rd_hi).GetRegAddr();
    else if( (_di->Rd_lo).IsReqAsTarget() )
      MapR[ i ] = (_di->Rd_lo).GetRegAddr();
  }

  return rst;
}

void CDispatch::ClearMap()
{
  MapPsr = 0xFFFFFFFFL;
  MapY = 0xFFFFFFFFL;
  for(int i = 0; i < DISPATCH_WIDTH; i++)
    MapR[i] = 0xFFFFFFFFL;
}

void CDispatch::Reserv(CRsItem* _rsit, CRobItem* _robit)
{
  _rsit->SetSourceWin( IReg->GetCurrentWin() );

  _robit->SetSourceWin( IReg->GetCurrentWin() );

  if( _rsit->GetInstOpcode() == SAVE )
    IReg->MoveToPrevWin();
  else if( _rsit->GetInstOpcode() == RESTORE )
    IReg->MoveToNextWin();

  _rsit->SetTargetWin( IReg->GetCurrentWin() );

  _robit->SetTargetWin( IReg->GetCurrentWin() );

  /*
   * se registrador fonte disponivel
   *   -> numero de sequencia enviado a estacao de reserva
   * senao
   *   -> valor do registrador e enviado a estacao de reserva
   */

  if( (_rsit->Psr).IsReqAsSource() ) {
    if( (SReg->Psr).IsBusy() )    
      (_rsit->Psr).SetDep( (SReg->Psr).GetSeqNum() );
    else
      (_rsit->Psr).SetValue( (SReg->Psr).GetValue() );
  }

  if( (_rsit->Y).IsReqAsSource() ) {
    if( (SReg->Y).IsBusy() )
      (_rsit->Y).SetDep( (SReg->Y).GetSeqNum() );
    else
      (_rsit->Y).SetValue( (SReg->Y).GetValue() );
  }

  if( (_rsit->Rs1).IsReqAsSource() ) {
    if( IReg->IsBusy(_rsit->GetSourceWin(), (_rsit->Rs1).GetRegAddr()) )
      (_rsit->Rs1).SetDep( (_rsit->Rs1).GetRegAddr(),
			   IReg->GetSeqNum(_rsit->GetSourceWin(), (_rsit->Rs1).GetRegAddr()) );
    else
      (_rsit->Rs1).SetValue( IReg->GetValue(_rsit->GetSourceWin(), (_rsit->Rs1).GetRegAddr()) );
  }

  if( (_rsit->Rs2).IsReqAsSource() ) {
    if( IReg->IsBusy(_rsit->GetSourceWin(), (_rsit->Rs2).GetRegAddr()) )
      (_rsit->Rs2).SetDep( (_rsit->Rs2).GetRegAddr(),
			   IReg->GetSeqNum(_rsit->GetSourceWin(), (_rsit->Rs2).GetRegAddr()) );
    else
      (_rsit->Rs2).SetValue( IReg->GetValue(_rsit->GetSourceWin(), (_rsit->Rs2).GetRegAddr()) );
  }

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

  if( (_rsit->Psr).IsReqAsTarget())
    (SReg->Psr).SetBusy( _rsit->GetSeqNum() );

  if( (_rsit->Y).IsReqAsTarget() )
    (SReg->Y).SetBusy( _rsit->GetSeqNum() );

  if( (_rsit->Rd_hi).IsReqAsTarget() )
    IReg->SetBusy(_rsit->GetTargetWin(), (_rsit->Rd_hi).GetRegAddr(), _rsit->GetSeqNum());

  if((_rsit->Rd_lo).IsReqAsTarget())
    IReg->SetBusy(_rsit->GetTargetWin(), (_rsit->Rd_lo).GetRegAddr(), _rsit->GetSeqNum());

}

void CDispatch::Execute(bool_t* _dtmbrkflag, uword_t* _dtmbrkcnt)
{
  CRsItem* rsit = 0;
  CRobItem* robit = 0;

  CDisqItem* di = 0;
  int i;

#if(DEBUG == 0)
  fprintf(stderr, "\nDISPATCH::EXECUTE(BEGIN)");
#endif

  for(i = 0; i < DISPATCH_WIDTH; i++) {
    // #loop. pesquisa em todas as entradas do despacho

    if( Rob->IsFull() ) return;    // se reorder buffer esta cheio retorna

    if( !DBDisq->IsEmpty() && ((di = DBDisq->GetNotBranchDepItem()) != 0) ) {
      // #1a. se existe instrucao na fila de instrucoes dependentes de desvios

#if(DEBUG == 0)
      fprintf( stderr,
	       "\nDISPATCH::EXECUTE(DBDisq, seq=%ld, pc=%08lx, inst=%08lx)",
	       di->Seq,
	       di->Addr, 
	       di->Inst );
#endif

      if( di->IsBadInst() ) {
	// #2a. se instrucao e uma instrucao de simulacao invalida

	robit = Rob->InsertItem(di);

	// #2a.
      }
      else {
	// #2b. se instrucao pertence ao dominio de instrucoes validas

	if( !HaveDependence(di) ) {
	  // #3a. se instrucao NAO for dependente entao avalia redundancia

	  if( !di->IsProbablyRedundant() ||
	      ( ((CDtm*) Dtm)->Stage_DS2(di) != TRUE ) ) {
	    // #4a. se instrucao nao for redundante

	    switch( di->GetExecUnit() ) {
	    case IU:
	      if( RsIu->IsFull() ) return;
	      rsit = RsIu->InsertItem(di);
	      robit = Rob->InsertItem(di);
	      Reserv(rsit, robit);
	      break;
	    case BPU:
	      if( RsBpu->IsFull() ) return;
	      rsit = RsBpu->InsertItem(di);
	      robit = Rob->InsertItem(di);
	      Reserv(rsit, robit);
	      break;
	    case LSU:
	      if( RsLsu->IsFull() || Maq->IsFull() ) return;
	      rsit = RsLsu->InsertItem(di);
	      robit = Rob->InsertItem(di);
	      Reserv(rsit, robit);
	      Maq->InsertItem(di);
	    };

	    // #4a.
	  }
	  else {
	    // #4b. se instrucao e redundante

	    (*_dtmbrkcnt) -= 1;
	    if((*_dtmbrkcnt) == 0)
	      (*_dtmbrkflag) = true;

	    // #4b.
	  }

	  // #3a.
	}
	else {
	  // #3b. se instrucao for dependente avalia a redundancia sem reusar

	  if(((CDtm*) Dtm)->VerifyReuse(di) == TRUE)
	    di->SetDependentRedundant();

	  switch( di->GetExecUnit() ) {
	  case IU:
	    if( RsIu->IsFull() ) return;
	    rsit = RsIu->InsertItem(di);
	    robit = Rob->InsertItem(di);
	    Reserv(rsit, robit);
	    break;
	  case BPU:
	    if( RsBpu->IsFull() ) return;
	    rsit = RsBpu->InsertItem(di);
	    robit = Rob->InsertItem(di);
	    Reserv(rsit, robit);
	    break;
	  case LSU:
	    if( RsLsu->IsFull() || Maq->IsFull() ) return;
	    rsit = RsLsu->InsertItem(di);
	    robit = Rob->InsertItem(di);
	    Reserv(rsit, robit);
	    Maq->InsertItem(di);
	  };

	  // #3b.
	}

	// #2a.
      }
      
      DBDisq->RemoveNotBranchDepItem();

      // #1a.
    }
    else {
      // #1b. se NAO existe instrucao na fila de instrucoes dependentes de desvios

      if( Disq->IsEmpty() ) return;         // se fila de despacho vazia retorna

      di = Disq->GetFirstItem();
      
#if(DEBUG == 0)
  fprintf(stderr, "\nDISPATCH::EXECUTE(Disq, seq=%ld, pc=%08lx, inst=%08lx)", di->Seq, di->Addr, di->Inst);
#endif

      if( !di->IsInvalid() ) {
	// #6a. se instrucao nao foi invalidada

	if( di->IsBadInst() ) {
	  // #7a. se instrucao e uma instrucao de simulacao invalida

	  robit = Rob->InsertItem(di);

	  // #7a.
	}
	else {
	  // #7b. se instrucao pertence ao dominio de instrucoes validas

	  if( di->IsBranchDep() ) {
	    // #8a. se instrucao ocupa o delay slot nao avaliar redundancia

	    if( DBDisq->IsFull() ) return;
	    DBDisq->InsertItem(di);
	   
	    // #8a.
	  }
	  else {
	    // #8b. se instrucao NAO ocupa um delay slot

	    if( !HaveDependence(di) ) {
	      // #9a. se instrucao NAO for dependente entao avalia validas

	      if( !di->IsProbablyRedundant() ||
		  ( ((CDtm*) Dtm)->Stage_DS2(di) != TRUE ) ) {
		// #10a. se instrucao NAO for redundante

		switch( di->GetExecUnit() ) {
		case IU:
		  if( RsIu->IsFull() ) return;
		  rsit = RsIu->InsertItem(di);
		  robit = Rob->InsertItem(di);
		  Reserv(rsit, robit);
		  break;
		case BPU:
		  if( RsBpu->IsFull() ) return;
		  rsit = RsBpu->InsertItem(di);
		  robit = Rob->InsertItem(di);
		  Reserv(rsit, robit);
		  break;
		case LSU:
		  if( RsLsu->IsFull() || Maq->IsFull() ) return;
		  rsit = RsLsu->InsertItem(di);
		  robit = Rob->InsertItem(di);
		  Reserv(rsit, robit);
		  Maq->InsertItem(di);
		};
		
		// #10a.
	      }
	      else {
		// #10b. se instrucao for redundante

		(*_dtmbrkcnt) -= 1;
		if((*_dtmbrkcnt) == 0)
		  (*_dtmbrkflag) = true;

		// #10b.
	      }

	      // #9a.
	    }
	    else {
	      // #9b. se instrucao NAO for dependente entao avalia validas

	      switch( di->GetExecUnit() ) {
	      case IU:
		if( RsIu->IsFull() ) return;
		rsit = RsIu->InsertItem(di);
		robit = Rob->InsertItem(di);
		Reserv(rsit, robit);
		break;
	      case BPU:
		if( RsBpu->IsFull() ) return;
		rsit = RsBpu->InsertItem(di);
		robit = Rob->InsertItem(di);
		Reserv(rsit, robit);
		break;
	      case LSU:
		if( RsLsu->IsFull() || Maq->IsFull() ) return;
		rsit = RsLsu->InsertItem(di);
		robit = Rob->InsertItem(di);
		Reserv(rsit, robit);
		Maq->InsertItem(di);
	      };

	      // #9b.
	    }
	    
	    // #8b.
	  }
	 
	  // #7b.
	}

	// #6a.
      }

      Disq->RemoveFirstItem();
      
      // #1b.
    }

    // #loop.
  }

  ClearMap();     // limpa mapa de analise de dependencias

}

void CDispatch::Reset()
{
  ClearMap();
}
