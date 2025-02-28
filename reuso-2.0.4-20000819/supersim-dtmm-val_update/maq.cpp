
/*
 * maq.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/15/99
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////////
///// CMaqItem

int CMaqItem::IsReady()
{
  return ( !IsWaiting() && (BLevel == 0) && ( IsMAddrReady() ));
}

int CMaqItem::IsWaiting()
{
  return Waiting;
}

int CMaqItem::IsMAddrReady()
{
  return MAddrReady;
}

void CMaqItem::SetMAddr(uword_t _maddr, uword_t _hival, uword_t _loval)
{
  MAddr = _maddr;
  HiVal = _hival;
  LoVal = _loval;
  MAddrReady = TRUE;
}

void CMaqItem::SetMAddrReady()
{
  MAddr = 0xFFFFFFFFL;
  HiVal = 0;
  LoVal = 0;
  MAddrReady = TRUE;
}

void CMaqItem::SetWaiting()
{
  Waiting = TRUE;
}

void CMaqItem::SetNotWaiting()
{
  Waiting = FALSE;
}

void CMaqItem::SetItem(CDisqItem _di)
{
  *((CDisqItem*) this) = _di;
  Waiting = TRUE;
  MAddrReady = FALSE;
}

///////////////////////////////////////////////////////////////
///// CMaq

CMaq::CMaq(uword_t _nentries)
  : CQueue(__MAQ_H, _nentries)
{
  if((pMaqArray = new CMaqItem[(NEntries = _nentries)]) == 0)
    errmsg(__MAQ_H, ERR_CANTALLOCMEM, "");
  CQueue::Init((ubyte_t*) pMaqArray, sizeof(CMaqItem));
}

CMaq::~CMaq()
{
  delete [] pMaqArray;
}

bool_t CMaq::HaveStorePending()
{
  CMaqItem* p;

  if( IsEmpty() ) return FALSE;

  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if( p->IsTraceRedundant() ) {
      if( p->TraceData.CheckForLoadStoreInst(IOM_STORE) )
	return TRUE;
    }
    else {
      if(p->WType != WR_NONE)
	return TRUE;
    }
  }

  return FALSE;
}

void CMaq::FreeSeqNum(uword_t _seq)
{
  CMaqItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetSeqNum() > _seq)
      p->DecSeqNum();
    if(( p->IsBranchDep() ) && (p->GetBranchDepNum() > _seq))
      p->DecBranchDepNum();
  }
}

void CMaq::FreeBranchLevel(uword_t _blevel)
{
  CMaqItem* p;
  if( IsEmpty() ) return;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetBranchLevel() > _blevel)
      p->DecBranchLevel();
  }
}

CMaqItem* CMaq::InsertItem(CDisqItem* _di)
{
  CMaqItem *p, *p_last = 0;

  if( !IsEmpty() )
    p_last = GetLastItem();

  p = (CMaqItem*) (CQueue::InsertItem()->pData);
  p->SetItem(*_di);

  if( (p_last == 0) ||
      ( !p_last->IsWaiting() &&
	(p_last->WType == WR_NONE) &&
	(p->WType == WR_NONE) ) )
    p->SetNotWaiting();

  return p;
}

CMaqItem* CMaq::GetItem(uword_t _seq)
{
  CMaqItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++)
    if( (p = GetNthItem(i))->GetSeqNum() == _seq )
      return p;
  return 0;
}

void CMaq::RemoveItem(uword_t _seq)
{
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    if( GetNthItem(i)->GetSeqNum() == _seq ) {
      RemoveNthItem(i);
      return;
    }
  }
}

CMaqItem* CMaq::GetNthItem(uword_t _idx)
{
  return ((CMaqItem*) (CQueue::GetNthItem(_idx)->pData));
}

CMaqItem* CMaq::GetLastItem()
{
  return ((CMaqItem*) (CQueue::GetLastItem()->pData));
}

void CMaq::EvalInstState()
{
  CMaqItem* p;
  uword_t i = 0;
  if(i < GetQueueSize()) {
    (p = GetNthItem(i))->SetNotWaiting();
    if(p->WType == WR_NONE) {
      while(++i < GetQueueSize()) {
	if((p = GetNthItem(i))->WType != WR_NONE) return;
	p->SetNotWaiting();
      }
    }
  }
}

void CMaq::Flush(uword_t _blevel)
{
  uword_t i = 0;
  while(i < GetQueueSize()) {
    if( GetNthItem(i)->GetBranchLevel() > _blevel )
      RemoveNthItem(i);
    else
      i += 1;
  }
}

void CMaq::Reset()
{
  CQueue::Flush();
}

void CMaq::Show()
{
  CMaqItem *p;
  CAsm it;

  str_t s1;

  printf("\nMemory access queue");
  printf("\n===================\n");

  if( IsEmpty() ) {
    printf("\nMemory access queue is empty\n");
    return;
  }

  if( IsFull() )
    printf("\nMemory access queue is full\n");

  for(uword_t i = 0; i < GetQueueSize(); i++) {
      p = GetNthItem(i);

      it.Set(p->Addr, p->Inst);
      it.Get(s1, 22);

      if( p->IsReady() )
	printf("\nR");
      else
	printf("\n ");

      if( p->IsWaiting() )
	printf("W");
      else
	printf(" ");

      if( p->IsBranchDep() )
	printf("* i(%02lu) ", i);
      else
	printf("  i(%02lu) ", i);
      
      printf( "%2lu %2lu %08lx %08lx %s",
	      p->Seq % MAX_SEQNUM,
	      p->BLevel,
	      p->Addr,
	      p->Inst,
	      s1 );
      
      if( !p->IsTraceRedundant() ) {

	printf(" <");
	if( p->RType != RD_NONE )
	  printf(" RD ");
	if( p->WType != WR_NONE )
	  printf(" WR ");
	printf("> ");

	if( p->IsMAddrReady() )
	  printf( "{ maddr=%lx }", p->MAddr );

      }
      else {

	printf(" <");
	if( (p->TraceData).CheckForLoadStoreInst(IOM_LOAD) )
	  printf(" RD ");

	if( (p->TraceData).CheckForLoadStoreInst(IOM_STORE) )
	  printf(" WR ");

	printf("> ");

        (p->TraceData).Display();

      }
  }
  printf("\n");
}

