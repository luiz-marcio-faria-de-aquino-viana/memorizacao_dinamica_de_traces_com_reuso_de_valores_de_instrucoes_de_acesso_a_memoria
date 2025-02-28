
/*
 * fetch.cpp
 * Copyright(C) 1999 by Luiz Marcio F A Viana, 9/3/99
 */

#include<stdio.h>
#include"all.h"

///////////////////////////////////////////////////////////////
///// CFetch

void CFetch::Init( CIMmu* _immu,
		   CFetchBuf* _fetchbuf,
		   CIQueue* _iqueue,
		   CComponent* _dtm)
{
  IMmu = _immu;
  FetchBuf = _fetchbuf;
  IQueue = _iqueue;
  Dtm = _dtm;
}

CFetch::CFetch()
{
  IMmu = 0;
  FetchBuf = 0;
  IQueue = 0;
}

void CFetch::Execute(bool_t *_breakflag, uword_t _breakpt)
{
  CFetchBufItem* fb;

  IMmu->Execute(RegPC, _breakflag, _breakpt);

  while( !FetchBuf->IsEmpty() && !IQueue->IsFull() ) {
    fb = FetchBuf->GetFirstItem();
    if( !fb->IsInvalid() ) {
#if(DEBUG == 0)
      if(fb->BInst == TRUE)
	printf( "\nFETCH::EXECUTE(B,addr=%08lx,inst=%08lx,baddr=%08lx)", fb->Addr, fb->Inst, fb->BAddr );
      else
	printf( "\nFETCH::EXECUTE(I,addr=%08lx,inst=%08lx)", fb->Addr, fb->Inst );
#endif
      RegPC = fb->NewPC;
      ((CDtm*) Dtm)->Stage_DS1(fb);
      IQueue->InsertItem(fb);
    }
    FetchBuf->RemoveFirstItem();
  }

  if( !FetchBuf->IsEmpty() ) {
    fb = FetchBuf->GetLastItem();
    RegPC = fb->NewPC;
  }
}

void CFetch::SetNewPC(uword_t _npc)
{
  RegPC = _npc;
}

void CFetch::Reset(uword_t _entryPt)
{
  RegPC = _entryPt;
}

void CFetch::Show()
{
  printf("\n\n=> PC=%08lx\n", RegPC);    
}
