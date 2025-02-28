
/*
 * disq.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////
///// CDisqItem

void CDisqItem::SetItem(CIDec _idec)
{
  *((CIDec*) this) = _idec;
}

CDisqItem& CDisqItem::operator=(CDisqItem& _di)
{
  *((CIDec*) this) = *((CIDec*) & _di);
  RedundantInst = _di.RedundantInst;
  TraceData = _di.TraceData;
  return *this;
}


///////////////////////////////////////////////////////////////
///// CDisq

CDisq::CDisq(uword_t _nentries)
  : CQueue(__DISQ_H, _nentries)
{
  if((pDisqArray = new CDisqItem [(NEntries = _nentries)]) == 0)
    errmsg(__DISQ_H, ERR_CANTALLOCMEM, "");
  CQueue::Init((ubyte_t*) pDisqArray, sizeof(CDisqItem));
}

CDisq::~CDisq()
{
  delete [] pDisqArray;
}

void CDisq::FreeSeqNum(uword_t _seq)
{
  CDisqItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetSeqNum() > _seq)
      p->DecSeqNum();
    if(( p->IsBranchDep() ) && (p->GetBranchDepNum() > _seq))
      p->DecBranchDepNum();
  }
}

void CDisq::FreeBranchLevel(uword_t _blevel)
{
  CDisqItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetBranchLevel() > _blevel)
      p->DecBranchLevel();
  }
}

CDisqItem* CDisq::InsertItem(CIDec* _idec)
{
  CDisqItem* p = (CDisqItem*) (CQueue::InsertItem()->pData);
  p->SetItem(*_idec);
  return p;
}

CDisqItem* CDisq::InsertItem(CDisqItem* _di)
{
  CDisqItem* p = (CDisqItem*) (CQueue::InsertItem()->pData);
  (*p) = (*_di);
  return p;
}

CDisqItem* CDisq::GetFirstItem()
{
  return ((CDisqItem*) (CQueue::GetFirstItem()->pData));
}

CDisqItem* CDisq::GetNthItem(int nth)
{
  return ((CDisqItem*) (CQueue::GetNthItem(nth)->pData));
}

CDisqItem* CDisq::GetNotBranchDepItem()
{
  CDisqItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if( !p->IsBranchDep() )
      return p;
  }  
  return 0;
}

void CDisq::RemoveFirstItem()
{
  CQueue::RemoveFirstItem();
}

CDisqItem* CDisq::GetLastItem()
{
  return ((CDisqItem*) (CQueue::GetLastItem()->pData));
}

CDisqItem* CDisq::GetBranchDepItem(uword_t _bseq)
{
  CDisqItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if( p->IsBranchDep() && (p->GetBranchDepNum() == _bseq) )
      return p;
  }  
  return 0;
}

void CDisq::RemoveBranchDepItem(uword_t _bseq)
{
  CDisqItem* p;
  uword_t i = 0;
  while(i < GetQueueSize()) {
    p = GetNthItem(i);
    if( p->IsBranchDep() && (p->GetBranchDepNum() == _bseq) )
      RemoveNthItem(i);
    else
      i += 1;
  }  
}

void CDisq::RemoveNotBranchDepItem()
{
  CDisqItem* p;
  uword_t i = 0;
  while(i < GetQueueSize()) {
    p = GetNthItem(i);
    if( !p->IsBranchDep() ) {
      RemoveNthItem(i);
      break;
    }
    i += 1;
  }  
}

void CDisq::Flush(uword_t _seq)
{
  CDisqItem* p;
  uword_t i = 0;
  while(i < GetQueueSize()) {
    if( (p = GetNthItem(i))->GetSeqNum() > _seq )
      RemoveNthItem(i);
    else
      i += 1;
  }
}

void CDisq::Flush(uword_t _blevel, uword_t _bseq)
{
  CDisqItem* p;
  uword_t i = 0;
  while(i < GetQueueSize()) {
    if( (p = GetNthItem(i))->GetBranchLevel() > _blevel )
      RemoveNthItem(i);
    else {
      if( p->IsBranchDep() && (p->GetBranchDepNum() == _bseq) )
	p->ClearBranchDep();
      i += 1;
    }
  }
}

void CDisq::Reset()
{
  CQueue::Flush();
}

void CDisq::Show()
{
  CDisqItem *p;
  CAsm it;

  str_t s1;

  if( IsEmpty() ) {
    printf("\nQueue is empty\n");
    return;
  }

  if( IsFull() )
    printf("\nQueue is full\n");

  for(uword_t i = 0; i < GetQueueSize(); i++) {
      p = GetNthItem(i);

      it.Set(p->Addr, p->Inst);
      it.Get(s1, MNMONIC_LEN);

      if( p->IsBranchDep() )
	printf( "\n%c * i(%02lu) ",
		(p->IsInvalid() ? 'I' : ' '),
		i );
      else
	printf( "\n%c   i(%02lu) ",
		(p->IsInvalid() ? 'I' : ' '),
		i );
      
      printf(  "%2lu %2lu %08lx %08lx %s ", p->Seq % MAX_SEQNUM, p->BLevel, p->Addr, p->Inst, s1);

      if( p->IsBranchInst() ) {
	if( p->IsPredTaken() )
	  printf("<BT, %08lx> ", p->BAddr);
	else
	  printf("<BN, %08lx> ", p->BAddr);
      }
      else
	printf("      ---      ");
 
      printf("< src=[ ");
      if( p->Psr.IsReqAsSource() )
	printf("psr ");
      if( p->Y.IsReqAsSource() )
	printf("y ");
      if( p->Rs1.IsReqAsSource() )
	printf("r%lu ", p->Rs1.GetRegAddr());
      if( p->Rs2.IsReqAsSource() )
	printf("r%lu ", p->Rs2.GetRegAddr());
      if( p->Rd_hi.IsReqAsSource() )
	printf("hi(r%lu) ", p->Rd_hi.GetRegAddr());
      if( p->Rd_lo.IsReqAsSource() )
	printf("lo(r%lu) ", p->Rd_lo.GetRegAddr());
      printf("], ");
      
      printf("dst=[ ");
      if( p->Psr.IsReqAsTarget() )
	printf("psr ");
      if( p->Y.IsReqAsTarget() )
	printf("y ");
      if( p->Rs1.IsReqAsTarget() )
	printf("r%lu ", p->Rs1.GetRegAddr());
      if( p->Rs2.IsReqAsTarget() )
	printf("r%lu ", p->Rs2.GetRegAddr());
      if( p->Rd_hi.IsReqAsTarget() )
	printf("hi(r%lu) ", p->Rd_hi.GetRegAddr());
      if( p->Rd_lo.IsReqAsTarget() )
	printf("lo(r%lu) ", p->Rd_lo.GetRegAddr());
      printf("] >  ");

      if( p->IsBranchDep() )
	printf("bseq=%lu ", p->GetBranchDepNum());

  }
  printf("\n");
}

