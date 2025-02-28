
/*
 * rs.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/13/99
 */

#include<stdio.h>
#include"all.h"

///////////////////////////////////////////////////////////////
///// CRsItem

int CRsItem::IsBusy()
{
  return Busy;
}

void CRsItem::SetBusy()
{
  Busy = TRUE;
}

int CRsItem::IsReady()
{
  if( !IsTraceRedundant() ) {
    /* se instrucao NAO for inicio de um trace */
    // retorna verdadeiro se instrucao NAO esta em execucao e operandos estao prontos
    return ( !IsBusy() &&
	     ( Psr.IsReady() &&
	       Y.IsReady() &&
	       Rs1.IsReady() &&
	       Rs2.IsReady() &&
	       Rd_hi.IsReady() &&
	       Rd_lo.IsReady() ) );
  }

  /* se instrucao for inicio de um trace */
  // retorna verdadeiro se instrucao NAO esta em execucao
  return ( !IsBusy() );
}

void CRsItem::SetSourceWin(uword_t _cwin)
{
  SourceWin = _cwin;
}

uword_t CRsItem::GetSourceWin()
{
  return SourceWin;
}

void CRsItem::SetTargetWin(uword_t _cwin)
{
  TargetWin = _cwin;
}

uword_t CRsItem::GetTargetWin()
{
  return TargetWin;
}

void CRsItem::SetItem(CDisqItem _di)
{
  *((CDisqItem*) this) = _di;

  Psr.Set(_di.Psr);

  Y.Set(_di.Y);

  Rs1.Set(_di.Rs1);

  Rs2.Set(_di.Rs2);

  if( !IsTraceRedundant() ) {
    Rd_hi.Set(_di.Rd_hi);
    
    Rd_lo.Set(_di.Rd_lo);
  }

  Busy = FALSE;
}

///////////////////////////////////////////////////////////////
///// CRs

CRs::CRs(uword_t _nentries)
  : CQueue(__RS_H, _nentries)
{
  if((pRsArray = new CRsItem [(NEntries = _nentries)]) == 0)
    errmsg(__RS_H, ERR_CANTALLOCMEM, "");
  CQueue::Init((ubyte_t*) pRsArray, sizeof(CRsItem));
}

CRs::~CRs()
{
  delete [] pRsArray;
}

bool_t CRs::HaveStorePending()
{
  CRsItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
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

void CRs::FreeSeqNum(uword_t _seq)
{
  CRsItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);

    if(p->GetSeqNum() > _seq)
      p->DecSeqNum();

    if(( p->IsBranchDep() ) && (p->GetBranchDepNum() > _seq))
      p->DecBranchDepNum();

    (p->Psr).DecDep(_seq);

    (p->Y).DecDep(_seq);

    (p->Rs1).DecDep(_seq);

    (p->Rs2).DecDep(_seq);

    if( !p->IsTraceRedundant() ) {

      (p->Rd_lo).DecDep(_seq);

      (p->Rd_hi).DecDep(_seq);

    }

  }
}

void CRs::FreeBranchLevel(uword_t _blevel)
{
  CRsItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
    if(p->GetBranchLevel() > _blevel)
      p->DecBranchLevel();
  }
}

CRsItem* CRs::InsertItem(CDisqItem* _di)
{
  CRsItem* p = (CRsItem*) (CQueue::InsertItem()->pData);
  p->SetItem(*_di);
  return p;
}

CRsItem* CRs::GetItem(uword_t _seq)
{
  CRsItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++)
    if((p = (CRsItem*) (GetNthItem(i)->pData))->GetSeqNum() == _seq )
      return p;
  return 0;
}

void CRs::RemoveItem(uword_t _seq)
{
  uword_t i = 0;
  while(i < GetQueueSize()) {
    if( ((CRsItem*) (GetNthItem(i)->pData))->GetSeqNum() == _seq )
      RemoveNthItem(i);
    else
      i += 1;
  }
}

CRsItem* CRs::GetFirstItem()
{
  return ((CRsItem*) (CQueue::GetFirstItem()->pData));
}

void CRs::RemoveFirstItem()
{
  CQueue::RemoveFirstItem();
}

CRsItem* CRs::GetReadyItem()
{
  CRsItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
    if( p->IsReady() )
      return p;
  }
  return 0;
}

void CRs::Update( uword_t _seq,
		  uword_t _cwp,
		  uword_t _rd_hi,
		  uword_t _val_hi,
		  uword_t _rd_lo,
		  uword_t _val_lo,
		  uword_t _val_psr,
		  uword_t _val_y )
{
  CRsItem* p;

  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
    if( !p->IsBusy() && !p->IsReady() ) {

      if( !p->IsTraceRedundant() ) {
      
	(p->Psr).Update(_seq, _val_psr);

	(p->Y).Update(_seq, _val_y);

	if( ((p->Rs1).Update(_seq, p->GetSourceWin(), _cwp, _rd_hi, _val_hi) != TRUE) &&
	    (_rd_hi != _rd_lo) )
	  (p->Rs1).Update(_seq, p->GetSourceWin(), _cwp, _rd_lo, _val_lo);
      
	if( ((p->Rs2).Update(_seq, p->GetSourceWin(), _cwp, _rd_hi, _val_hi) != TRUE) &&
	    (_rd_hi != _rd_lo) )
	  (p->Rs2).Update(_seq, p->GetSourceWin(), _cwp, _rd_lo, _val_lo);
	
	if( ((p->Rd_hi).Update(_seq, p->GetSourceWin(), _cwp, _rd_hi, _val_hi) != TRUE) &&
	    (_rd_hi != _rd_lo) )
	  (p->Rd_hi).Update(_seq, p->GetSourceWin(), _cwp, _rd_lo, _val_lo);
      
	if( ((p->Rd_lo).Update(_seq, p->GetSourceWin(), _cwp, _rd_hi, _val_hi) != TRUE) &&
	    (_rd_hi != _rd_lo) )
	  (p->Rd_lo).Update(_seq, p->GetSourceWin(), _cwp, _rd_lo, _val_lo);
	
      }

    }

  }

}

void CRs::UpdatePsr( uword_t _seq,
		     uword_t _val_psr )
{
  CRsItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
    if( !p->IsBusy() && !p->IsReady() )
	(p->Psr).Update(_seq, _val_psr);
  }
}

void CRs::UpdateY( uword_t _seq,
		   uword_t _val_y )
{
  CRsItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
    if( !p->IsBusy() && !p->IsReady() )
	(p->Y).Update(_seq, _val_y);
  }
}

void CRs::UpdateIReg( uword_t _seq,
		      uword_t _cwp,
		      uword_t _raddr,
		      uword_t _rval )
{
  CRsItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = (CRsItem*) (GetNthItem(i)->pData);
    if( !p->IsBusy() && !p->IsReady() ) {
      if( !p->IsTraceRedundant() ) {
	(p->Rs1).Update(_seq, p->GetSourceWin(), _cwp, _raddr, _rval);
	(p->Rs2).Update(_seq, p->GetSourceWin(), _cwp, _raddr, _rval);
	(p->Rd_hi).Update(_seq, p->GetSourceWin(), _cwp, _raddr, _rval);
	(p->Rd_lo).Update(_seq, p->GetSourceWin(), _cwp, _raddr, _rval);
      }
    }
  }
}

void CRs::Flush(uword_t _blevel)
{
  uword_t i = 0;
  while(i < GetQueueSize()) {
    if( ((CRsItem*) (GetNthItem(i)->pData))->GetBranchLevel() > _blevel )
      RemoveNthItem(i);
    else
      i += 1;
  }
}

void CRs::Reset()
{
  CQueue::Flush();
}

void CRs::Show()
{
  CRsItem *p;
  CAsm it;

  str_t s1;

  printf("\nReservation stations");
  printf("\n====================\n");

  if( IsEmpty() ) {
    printf("\nReservation station is empty\n");
    return;
  }

  if( IsFull() )
    printf("\nReservation station is full\n");

  for(uword_t i = 0; i < GetQueueSize(); i++) {
      p = (CRsItem*) (GetNthItem(i)->pData);

      it.Set(p->Addr, p->Inst);
      it.Get(s1, MNMONIC_LEN);

      if( p->IsReady() )
	printf("\nR ");
      else
	printf("\n  ");

      if( p->IsBusy() )
	printf("B ");
      else
	printf("  ");

      if( p->IsBranchDep() )
	printf("* i(%02lu) ", i);
      else
	printf("  i(%02lu) ", i);

      printf(  "%2lu %2lu %08lx %08lx %s ",
	       p->Seq % MAX_SEQNUM,
	       p->BLevel,
	       p->Addr,
	       p->Inst,
	       s1 );

      if( p->IsBranchInst() ) {
	if( p->IsPredTaken() )
	  printf("<BT, %08lx> ", p->BAddr);
	else
	  printf("<BN, %08lx> ", p->BAddr);
      }
      else {
	printf("      ---      ");
      }

      printf("[ ");

      if( !p->IsTraceRedundant() ) {

	if( (p->Psr).IsReqAsSource() ) {
	  if( (p->Psr).IsReady() )
	    printf("psr=%lx ", (p->Psr).GetValue());
	  else
	    printf("(psr<-%lu) ", (p->Psr).GetDep());
	}

	if( (p->Y).IsReqAsSource() ) {
	  if( (p->Y).IsReady() )
	    printf("y=%lx ", (p->Y).GetValue());
	  else
	    printf("(y<-%lu) ", (p->Y).GetDep());
	}

	if( (p->Rs1).IsReqAsSource() ) {
	  if( (p->Rs1).IsReady() )
	    printf( "rs%lu=%lx ", 
		    (p->Rs1).GetRegAddr(), 
		    (p->Rs1).GetValue() );
	  else
	    printf( "(rs%lu<-%lu) ",
		    (p->Rs1).GetRegAddr(),
		    (p->Rs1).GetDep() );
	}

	if( (p->Rs2).IsReqAsSource() ) {
	  if( (p->Rs2).IsReady() )
	    printf( "rs%lu=%lx ",
		    (p->Rs2).GetRegAddr(),
		    (p->Rs2).GetValue() );
	  else
	    printf( "(rs%lu<-%lu) ",
		    (p->Rs2).GetRegAddr(),
		    (p->Rs2).GetDep() );
	}

	if( (p->Rd_lo).IsReqAsSource() ) {
	  if( (p->Rd_lo).IsReady() )
	    printf( "rs%lu=%lx ",
		    (p->Rd_lo).GetRegAddr(),
		    (p->Rd_lo).GetValue() );
	  else
	    printf( "(rs%lu<-%lu) ",
		    (p->Rd_lo).GetRegAddr(),
		    (p->Rd_lo).GetDep() );
	}
	
	if( (p->Rd_hi).IsReqAsSource() ) {
	  if( (p->Rd_hi).IsReady() )
	    printf( "rs%lu=%lx ",
		    (p->Rd_hi).GetRegAddr(),
		    (p->Rd_hi).GetValue() );
	  else
	    printf( "(rs%lu<-%lu) ",
		    (p->Rd_hi).GetRegAddr(),
		    (p->Rd_hi).GetDep() );
	}
	
      }

      printf("]");

      if( p->IsTraceRedundant() )
	(p->TraceData).Display();

  }
  printf("\n");
}
