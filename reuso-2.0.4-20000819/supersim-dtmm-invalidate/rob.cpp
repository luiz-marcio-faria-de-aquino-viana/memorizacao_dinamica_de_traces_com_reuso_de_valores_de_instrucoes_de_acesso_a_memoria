
/*
 * rob.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/15/99
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////////
///// CRobctl

CRobctl::CRobctl()
{
  Ready = FALSE;
}

int CRobctl::IsReady()
{
  return ( !IsReqAsTarget() || (Ready == TRUE) );
}

void CRobctl::Update(uword_t _val)
{
  Ready = TRUE;
  Val = _val;
}

void CRobctl::UpdateSrc(uword_t _val)
{
  SrcVal = _val;
}

uword_t CRobctl::GetSrcValue()
{
  return SrcVal;
}

uword_t CRobctl::GetValue()
{
  return Val;
}

void CRobctl::Set(CIDecctl _it)
{
  *((CIDecctl*) this) = _it;
  Ready = FALSE;
}

/////////////////////////////////////////////////////////////////
///// CRobItem

int CRobItem::IsReady()
{
  return ( IsBadInst() ||
	   ( Done &&
	     Psr.IsReady() &&
	     Y.IsReady() &&
	     Rs1.IsReady() &&
	     Rs2.IsReady() &&
	     Rd_lo.IsReady() &&
	     Rd_hi.IsReady() ) );
}

void CRobItem::SetSourceWin(uword_t _cwin)
{
  SourceWin = _cwin;
}

uword_t CRobItem::GetSourceWin()
{
  return SourceWin;
}

void CRobItem::SetTargetWin(uword_t _cwin)
{
  TargetWin = _cwin;
}

uword_t CRobItem::GetTargetWin()
{
  return TargetWin;
}

void CRobItem::UpdateSrc( CRsItem* _rsit)
{
  if( !_rsit->IsTraceRedundant() ) {

    if( (_rsit->Psr).IsReqAsSource() )
      Psr.UpdateSrc( (_rsit->Psr).GetValue() );
    
    if( (_rsit->Y).IsReqAsSource() )
      Y.UpdateSrc( (_rsit->Y).GetValue() );
    
    if( (_rsit->Rs1).IsReqAsSource() )
      Rs1.UpdateSrc( (_rsit->Rs1).GetValue() );
    
    if( (_rsit->Rs2).IsReqAsSource() )
      Rs2.UpdateSrc( (_rsit->Rs2).GetValue() );
    
    if( (_rsit->Rd_hi).IsReqAsSource() )
      Rd_hi.UpdateSrc( (_rsit->Rd_hi).GetValue() );
    
    if( (_rsit->Rd_lo).IsReqAsSource() )
      Rd_lo.UpdateSrc( (_rsit->Rd_lo).GetValue() );

  }
  else {

    TraceData = _rsit->TraceData;

  }

}

void CRobItem::Update( uword_t _psr_val,
		       uword_t _y_val,
		       uword_t _rs1_val,
		       uword_t _rs2_val,
		       uword_t _rd_lo_val,
		       uword_t _rd_hi_val,
		       bool_t _trapsig,
		       uword_t _trapnum,
		       bool_t _chgpc,
		       uword_t _newpc,
		       CMemoTableEntryT* _tracedata,
		       bool_t _bres,
		       uword_t _maddr)
{
  Done = TRUE;

  if( Psr.IsReqAsTarget() )
    Psr.Update(_psr_val);

  if( Y.IsReqAsTarget() )
    Y.Update(_y_val);

  if( Rs1.IsReqAsTarget() )
    Rs1.Update(_rs1_val);

  if( Rs2.IsReqAsTarget() )
    Rs2.Update(_rs2_val);

  if( Rd_lo.IsReqAsTarget() )
    Rd_lo.Update(_rd_lo_val);

  if( Rd_hi.IsReqAsTarget() )
    Rd_hi.Update(_rd_hi_val);

  if(_tracedata != 0)
    TraceData = *_tracedata;

  Trapsig = _trapsig;

  Trapnum = _trapnum;

  NewPC = _newpc;

  BResult = _bres;

  MAddr = _maddr;
}

void CRobItem::InvalidateMem(uword_t _maddr, uword_t _nbytes)
{
  uword_t n;

  if( !IsTraceRedundant() ) {
    // se instrucao NAO inicia um trace
    // (obs: traces sao invalidados pelo DTM)

    if(RType != RD_NONE) {
      // se instrucao e de leitura da memoria

      switch( RType ) {
      case RD_BYTE:
      case RD_UBYTE:
	/* acessa byte */
	n = sizeof(byte_t);
	break;
	
      case RD_HWORD:
      case RD_UHWORD:
	/* acessa hword */
	n = sizeof(hword_t);
	break;

      case RD_WORD:
	/* acessa word */
	n = sizeof(word_t);
	break;

      case RD_DWORD:
	/* acessa dword */
	n = sizeof(dword_t);

      }

      if( (_maddr <= (MAddr + n - 1)) && ((_maddr + _nbytes - 1) >= MAddr) )
	MValidVal = FALSE;

    }

  }

}

void CRobItem::UpdateMem(uword_t _maddr, ubyte_t * _val, uword_t _nbytes)
{
  uword_t pos, n;
  dword_t res;

  if( !IsTraceRedundant() ) 
  {
    // se instrucao NAO inicia um trace
    // (obs: traces sao atualizados pelo DTM)

    if(RType != RD_NONE) 
    {
      // se instrucao e de leitura da memoria
      switch( RType ) {
      case RD_BYTE:
      case RD_UBYTE:
	/* acessa byte */
	n = sizeof(byte_t);
	break;
	
      case RD_HWORD:
      case RD_UHWORD:
	/* acessa hword */
	n = sizeof(hword_t);
	break;

      case RD_WORD:
	/* acessa word */
	n = sizeof(word_t);
	break;

      case RD_DWORD:
	/* acessa dword */
	n = sizeof(dword_t);

      }

      if( (MAddr <= (_maddr + _nbytes - 1)) && ((MAddr + n - 1) >= _maddr) )
      {
	res = 0;
	for(uword_t i = 0; i < n; i++)
	{
	  if( ((MAddr + i) >= _maddr) && ((MAddr + i) <= (_maddr + _nbytes - 1)) )
	  {
	    pos = ((MAddr + i) - _maddr);
	    res = (res << 32) | _val[pos];
	  }
	  else
	  {
	    res = (res << 32);
	  }
	}
      }

      Rd_hi.Update((uword_t) ((res >> 32) & 0x00000000FFFFFFFFL));
      if(RType == RD_DWORD)
	Rd_lo.Update((uword_t) (res & 0x00000000FFFFFFFFL));

    }

  }

}

void CRobItem::SetItem(CDisqItem _di)
{
  *((CDisqItem*) this) = _di;

  Psr.Set(_di.Psr);

  Y.Set(_di.Y);

  Rs1.Set(_di.Rs1);

  Rs2.Set(_di.Rs2);

  Rd_hi.Set(_di.Rd_hi);

  Rd_lo.Set(_di.Rd_lo);


  Trapsig = FALSE;

  Trapnum = 0;


  MValidVal = ( ((RType != RD_NONE) || (WType != WR_NONE)) ? TRUE : FALSE );


  Done = FALSE;

}

///////////////////////////////////////////////////////////////
///// CRob

CRob::CRob(uword_t _nentries)
  : CQueue(__ROB_H, _nentries)
{
  if((pRobArray = new CRobItem [(NEntries = _nentries)]) == 0)
    errmsg(__ROB_H, ERR_CANTALLOCMEM, "");
  CQueue::Init((ubyte_t*) pRobArray, sizeof(CRobItem));
}

CRob::~CRob()
{
  delete [] pRobArray;
}

void CRob::FreeSeqNum(uword_t _seq)
{
  CRobItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetSeqNum() > _seq)
      p->DecSeqNum();
    if(( p->IsBranchDep() ) && (p->GetBranchDepNum() > _seq))
      p->DecBranchDepNum();
  }
}

void CRob::FreeBranchLevel(uword_t _blevel)
{
  CRobItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetBranchLevel() > _blevel)
      p->DecBranchLevel();
  }
}

bool_t CRob::BranchNotSolved(uword_t _seq)
{
  CRobItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    if((p = GetNthItem(i))->GetSeqNum() >= _seq) return FALSE;
    if( p->IsBranchInst() ) return TRUE;
  }
  return FALSE;
}

CRobItem* CRob::InsertItem(CDisqItem* _di)
{
  CRobItem* p = (CRobItem*) (CQueue::InsertItem()->pData);
  p->SetItem(*_di);
  return p;
}

CRobItem* CRob::GetNthItem(uword_t _idx)
{
  return ((CRobItem*) (CQueue::GetNthItem(_idx)->pData));
}

CRobItem* CRob::GetFirstItem()
{
  return ((CRobItem*) (CQueue::GetFirstItem()->pData));
}

CRobItem* CRob::GetLastItem()
{
  return ((CRobItem*) (CQueue::GetLastItem()->pData));
}

void CRob::RemoveFirstItem()
{
  CQueue::RemoveFirstItem();
}

CRobItem* CRob::GetReadyItem()
{
  CRobItem* p = GetFirstItem();
  if(p->IsReady())
    return p;
  return 0;
}

void CRob::RemoveReadyItem()
{
  CRobItem* p = GetFirstItem();
  if(p->IsReady())
    CQueue::RemoveFirstItem();
}

void CRob::UpdateSrc( uword_t _seq,
		      CRsItem* _rsit )
{
  CRobItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++)
    if((p = GetNthItem(i))->GetSeqNum() == _seq) {
      p->UpdateSrc( _rsit );
      break;
    }
}

CRobItem* CRob::Update( uword_t _seq,
		   uword_t _psr_val,
		   uword_t _y_val,
		   uword_t _rs1_val,
		   uword_t _rs2_val,
		   uword_t _rd_hi_val,
		   uword_t _rd_lo_val,
		   bool_t _trapsig,
		   uword_t _trapnum,
		   bool_t _chgpc,
		   uword_t _newpc,
		   CMemoTableEntryT* _tracedata,
		   bool_t _bres,
		   uword_t _maddr)
{
  CRobItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    if((p = GetNthItem(i))->GetSeqNum() == _seq) {
      p->Update( _psr_val,
		 _y_val,
		 _rs1_val,
		 _rs2_val,
		 _rd_lo_val, 
		 _rd_hi_val,
		 _trapsig,
		 _trapnum,
		 _chgpc,
		 _newpc,
		 _tracedata,
		 _bres,
		 _maddr );
      return p;
    }
  }
  return NULL;
}

void CRob::Flush(uword_t _blevel)
{
  uword_t i = 0;
  while(i < GetQueueSize()) {
    if( GetNthItem(i)->GetBranchLevel() > _blevel )
      RemoveNthItem(i);
    else
      i += 1;
  }
}

void CRob::InvMemVal(uword_t _seq, uword_t _wtype, uword_t _maddr)
{
  CRobItem * p;
  uword_t i;

  for(i = 0; i < GetQueueSize(); i++) {
    if((p = GetNthItem(i))->GetSeqNum() >= _seq) break;

    switch( _wtype ) {
    case WR_BYTE:
      p->InvalidateMem(_maddr, sizeof(byte_t));
      break;

    case WR_HWORD:
      p->InvalidateMem(_maddr, sizeof(hword_t));
      break;

    case WR_WORD:
      p->InvalidateMem(_maddr, sizeof(word_t));
      break;

    case WR_DWORD:
      p->InvalidateMem(_maddr, sizeof(dword_t));

    }

  }

}

void CRob::UpdMemVal(uword_t _seq, uword_t _wtype, uword_t _maddr, uword_t _res1, uword_t _res2)
{
  CRobItem * p;

  databuf_t dt;
  uword_t i;

  for(i = 0; i < GetQueueSize(); i++) {
    if((p = GetNthItem(i))->GetSeqNum() >= _seq) break;

    switch( _wtype ) {
    case WR_BYTE:
      dt.ub = (byte_t) _res1;
      p->UpdateMem(_maddr, (ubyte_t*) & dt.ub, sizeof(byte_t));
      break;

    case WR_HWORD:
      dt.uh = (hword_t) _res1;
      p->UpdateMem(_maddr, (ubyte_t*) & dt.uh, sizeof(hword_t));
      break;

    case WR_WORD:
      dt.w = (word_t) _res1;
      p->UpdateMem(_maddr, (ubyte_t*) & dt.w, sizeof(word_t));
      break;

    case WR_DWORD:
      dt.d = (((dword_t) _res1) << 32) + ((dword_t) _res2);
      p->UpdateMem(_maddr, (ubyte_t*) & dt.d, sizeof(dword_t));

    }

  }

}

void CRob::Reset()
{
  CQueue::Flush();
}

void CRob::Show()
{
  CRobItem *p;
  CAsm it;

  uword_t i;
  str_t s1;

  printf("\nReorder Buffer");
  printf("\n==============\n");

  if( IsEmpty() ) {
    printf("\nReorder buffer is empty\n");
    return;
  }

  if( IsFull() )
    printf("\nReorder buffer is full\n");

    for(i = 0; i < GetQueueSize(); i++) {
      p = GetNthItem(i);

      it.Set(p->Addr, p->Inst);
      it.Get(s1, MNMONIC_LEN);

      printf("\n");

      if( p->IsDelaySlotInst() )
	printf("+ ");
      else
	printf("  ");

      if( p->IsRedundant() )
	  printf("IR ");
      else if( p->IsTraceRedundant() )
	  printf("TR ");
      else
	printf("   ");

      if( p->IsReady() )
	printf("R ");
      else
	printf("  ");

      if( p->IsBranchDep() )
	printf("* i(%02lu) ", i);
      else
	printf("  i(%02lu) ", i);
      
      printf(  "%2lu %2lu %08lx %08lx %s ", p->Seq % MAX_SEQNUM, p->BLevel, p->Addr, p->Inst, s1);

      if( p->IsBranchInst() ) {
	if( p->IsReady() ) {
	  if( p->BResult == TAKEN )
	    printf("<BT, %08lx> ", p->BAddr);
	  else
	    printf("<BN, %08lx> ", p->BAddr);
	}
	else {
	  if( p->IsPredTaken() )
	    printf("<BT, %08lx> ", p->BAddr);
	  else
	    printf("<BN, %08lx> ", p->BAddr);
	}
      }
      else
	printf("      ---      ");

      if( !p->IsRedundant() && !p->IsTraceRedundant() ) {
	if(p->RType != RD_NONE) {
	  if( p->IsReady() )
	    printf("[ LD -%c- maddr=%lx ] ", ( p->MValidVal ? 'V' : 'I' ), p->MAddr);
	  else
	    printf("[ LD -%c- maddr=?] ", ( p->MValidVal ? 'V' : 'I' ));
	}
	if(p->WType != WR_NONE) {
	  if( p->IsReady() )
	    printf("[ ST -%c- maddr=%lx ] ", ( p->MValidVal ? 'V' : 'I' ), p->MAddr);
	  else
	    printf("[ ST -%c- maddr=?] ", ( p->MValidVal ? 'V' : 'I' ));
	}
      }

      if( p->IsRedundant() ) {

	/* apresenta resultado de execucao de instrucoes redundantes */

	(p->RedundantInst).Display();

      }
      else if( p->IsTraceRedundant() ) {
	
	/* apresenta resultado de execucao dos traces redundantes */

        (p->TraceData).Display();

      }
      else {

	/* apresenta resultado de execucao das instrucoes NAO redundantes */

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

  }
  printf("\n");
}
