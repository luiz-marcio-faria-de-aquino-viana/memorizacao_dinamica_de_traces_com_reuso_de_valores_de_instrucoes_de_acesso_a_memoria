
/*
 * queue.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/4/1999
 */

#include<stdio.h>
#include"defs.h"
#include"types.h"
#include"error.h"
#include"queue.h"

////////////////////////////////////////////////////////////
///// CQueueItem

CQueueItem::CQueueItem()
{
  pData = 0;
  szData = 0;
}

////////////////////////////////////////////////////////////
///// CQueue

void CQueue::Init( ubyte_t* _pdata, uword_t _szdata )
{
  uword_t i;
  for(i = 0; i < NEntries; i++) {
    pQueueArray[i].pData = _pdata;
    _pdata += _szdata;
  }
}

CQueue::CQueue(int _qid, uword_t _nentries)
{
  if((pQueueArray = new CQueueItem [(NEntries = _nentries)]) == 0)
    errmsg(__QUEUE_H, ERR_CANTALLOCMEM, "");
  First = Last = 0;
  Size = 0;
  QId = _qid;
}

CQueue::~CQueue()
{
  delete [] pQueueArray;
}

uword_t CQueue::GetNumEntries()
{
  return NEntries;
}

int CQueue::IsEmpty()
{
  return (Size == 0);
}

int CQueue::IsFull()
{
  return (Size == NEntries);
}

uword_t CQueue::GetQueueSize()
{
  return Size;
}

CQueueItem* CQueue::InsertItem()
{
  CQueueItem* p;

  if( IsFull() ) {
    sprintf(errtile, "(insert, qid=%d)", QId); 
    errmsg(__QUEUE_H, ERR_QUEUEFULL, errtile);
  }

  p = & pQueueArray[Last];
  Last = (Last + 1) % NEntries;
  Size += 1;

  return p;
}

CQueueItem* CQueue::GetFirstItem()
{
  if( IsEmpty() ) {
    sprintf(errtile, "(getfirst, qid=%d)", QId);
    errmsg(__QUEUE_H, ERR_QUEUEEMPTY, errtile);
  }

  return & pQueueArray[First];
}

CQueueItem* CQueue::GetNthItem(uword_t _idx)
{
  if(_idx >= Size) {
    sprintf(errtile, "(getnth, qid=%d)", QId);
    errmsg(__QUEUE_H, ERR_OUTOFBOUNDS, errtile);
  }
  return & pQueueArray[(First + _idx) % NEntries];
}

CQueueItem* CQueue::GetLastItem()
{
  if( IsEmpty() ) {
    sprintf(errtile, "(getlast, qid=%d)", QId);
    errmsg(__QUEUE_H, ERR_QUEUEEMPTY, errtile);
  }

  return & pQueueArray[((Last > 0) ? Last - 1 : NEntries - 1)];
}

void CQueue::RemoveNthItem(uword_t _idx)
{
  ubyte_t* p;
  uword_t i;

  if(_idx >= Size) {
    sprintf(errtile, "(removenth, qid=%d)", QId);
    errmsg(__QUEUE_H, ERR_OUTOFBOUNDS, errtile);
  }

  p = pQueueArray[(First + _idx) % NEntries].pData;
  for(i = _idx; i < Size - 1; i++)
    pQueueArray[(First + i) % NEntries].pData = pQueueArray[(First + i + 1) % NEntries].pData;
  pQueueArray[(First + i) % NEntries].pData = p;

  Last = (Last > 0) ? Last - 1 : NEntries - 1;
  Size -= 1;
}

void CQueue::RemoveFirstItem()
{
  if( IsEmpty() ) {
    sprintf(errtile, "(remove, qid=%d)", QId);
    errmsg(__QUEUE_H, ERR_QUEUEEMPTY, errtile);
  }
  First = (First + 1) % NEntries;
  Size -= 1;
}

void CQueue::Swap(uword_t _idx1, uword_t _idx2)
{
  ubyte_t* p = pQueueArray[(First + _idx1) % NEntries].pData;
  pQueueArray[(First + _idx1) % NEntries].pData = pQueueArray[(First + _idx2) % NEntries].pData;
  pQueueArray[(First + _idx2) % NEntries].pData = p;
}

void CQueue::Flush()
{
  First = Last;
  Size = 0;
}
