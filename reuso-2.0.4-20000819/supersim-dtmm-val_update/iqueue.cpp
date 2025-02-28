
/*
 * iqueue.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/6/99
 */

#include<stdio.h>
#include"all.h"

///////////////////////////////////////////////////////////////
///// CIQueueItem

void CIQueueItem::SetItem(CFetchBufItem _fb)
{
  *((CFetchBufItem*) this) = _fb;
}

CIQueueItem & CIQueueItem::operator=(CIQueueItem & _iq)
{
  *((CFetchBufItem*) this) = *((CFetchBufItem*) & _iq);
  return *this;
}

///////////////////////////////////////////////////////////////
///// CIQueue

CIQueue::CIQueue(uword_t _nentries)
  : CQueue(__IQUEUE_H, _nentries)
{
  if((pIQueueArray = new CIQueueItem [(NEntries = _nentries)]) == 0)
    errmsg(__IQUEUE_H, ERR_CANTALLOCMEM, "");
  CQueue::Init((ubyte_t*) pIQueueArray, sizeof(CIQueueItem));
}

CIQueue::~CIQueue()
{
  delete [] pIQueueArray;
}

void CIQueue::FreeSeqNum(uword_t _seq)
{
  CIQueueItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetSeqNum() > _seq)
      p->DecSeqNum();
    if(( p->IsBranchDep() ) && (p->GetBranchDepNum() > _seq))
      p->DecBranchDepNum();
  }
}

void CIQueue::FreeBranchLevel(uword_t _blevel)
{
  CIQueueItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetBranchLevel() > _blevel)
      p->DecBranchLevel();
  }
}

CIQueueItem* CIQueue::InsertItem(CFetchBufItem* _fb)
{
  CIQueueItem* p = (CIQueueItem*) (CQueue::InsertItem()->pData);
  p->SetItem(*_fb);
  return p;
}

CIQueueItem* CIQueue::GetFirstItem()
{
  return ((CIQueueItem*) (CQueue::GetFirstItem()->pData));
}

CIQueueItem* CIQueue::GetNthItem(int nth)
{
  return ((CIQueueItem*) (CQueue::GetNthItem(nth)->pData));
}

void CIQueue::RemoveFirstItem()
{
  CQueue::RemoveFirstItem();
}

void CIQueue::RemoveBranchDepItem(uword_t _bseq)
{
  CIQueueItem* p;
  uword_t i = 0;
  while(i < GetQueueSize()) {
    p = GetNthItem(i);
    if( p->IsBranchDep() && (p->GetBranchDepNum() == _bseq) )
      RemoveNthItem(i);
    else
      i += 1;
  }  
}

void CIQueue::Flush()
{
  CQueue::Flush();
}

void CIQueue::Flush(uword_t _blevel, uword_t _bseq)
{
  CIQueueItem* p;
  uword_t i = 0;
  while(i < GetQueueSize()) {
    p = GetNthItem(i);
    if( p->GetBranchLevel() > _blevel )
      RemoveNthItem(i);
    else {
      if( p->IsBranchDep() && (p->GetBranchDepNum() == _bseq) )
	p->ClearBranchDep();
      i += 1;
    }
  }
}

void CIQueue::Reset()
{
  CQueue::Flush();
}

void CIQueue::Show()
{
  CIQueueItem *p;
  CAsm it;

  str_t s;

  printf("\nInstruction queue");
  printf("\n=================\n");

  if( IsEmpty() ) {
    printf("\nQueue is empty\n");
    return;
  }

  if( IsFull() )
    printf("\nQueue is full\n");

  for(uword_t i = 0; i < GetQueueSize(); i++) {
      p = GetNthItem(i);

      it.Set(p->Addr, p->Inst);
      it.Get(s, MNMONIC_LEN);

      if( p->IsBranchDep() )
	printf( "\n%c (*%02lu) i(%02lu) ",
		(p->IsInvalid() ? 'I' : ' '),
		p->BSeq % MAX_SEQNUM,
		i );
      else
	printf( "\n%c       i(%02lu) ",
		(p->IsInvalid() ? 'I' : ' '),
		i );

      printf(  "%2lu %2lu %08lx %08lx %s ", p->Seq % MAX_SEQNUM, p->BLevel, p->Addr, p->Inst, s);

      if( p->IsBranchInst() ) {
	if( p->IsPredTaken() )
	  printf("<BT, %08lx> ", p->BAddr);
	else
	  printf("<BN, %08lx> ", p->BAddr);
      }
      else
	printf("      ---      ");
  }
  printf("\n");
}

