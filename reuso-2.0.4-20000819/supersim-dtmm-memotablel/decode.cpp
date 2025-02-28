
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
    //// TESTE INSTRUCAO NAO IMPLEMENTADA - ERRO DE COMPILACAO COM 64-BITS
    // if( idec.IsBadInst() ) {
    //   str_t errtile;
    //   sprintf(errtile, "- DECODE::EXECUTE(@%08lX, %08lX)", idec.Addr, idec.Inst);
    //   errmsg(__DECODE_H, ERR_NOTIMPLEMENTED, errtile);
    // }
    Disq->InsertItem(& idec);
    IQueue->RemoveFirstItem();
  }
}
