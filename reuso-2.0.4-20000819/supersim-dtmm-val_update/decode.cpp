
/*
 * decode.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#include<stdio.h>
#include"all.h"

//////////////////////////////////////////////////////////////
///// CDecode

void CDecode::Init( CIQueue* _iqueue,
		    CDisq* _disq )
{
  IQueue = _iqueue;
  Disq = _disq;
}

void CDecode::Execute()
{
  CIDec idec;
  for(int i = 0; i < DECODE_WIDTH; i++) {
    if( IQueue->IsEmpty() || Disq->IsFull() ) return;
    idec.Set(* (IQueue->GetFirstItem()) );
    Disq->InsertItem(& idec);
    IQueue->RemoveFirstItem();
  }
}
