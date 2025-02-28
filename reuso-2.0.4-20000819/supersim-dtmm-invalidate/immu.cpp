
/*
 * immu.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/3/1999
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////////////////
///// CIMmu

void CIMmu::Init( CMem* _mem,
		  CFetchBuf* _fetchbuf )
{
  Mem = _mem;
  FetchBuf = _fetchbuf;
}

CIMmu::CIMmu()
{
  Mem = 0;
  FetchBuf = 0;
}

void CIMmu::Execute(uword_t _addr, bool_t *_breakflag, uword_t _breakpt)
{
  CFetchBufItem *pItem1, *pItem2;

  uword_t inst;
  uword_t offset = 0;

  while( !FetchBuf->IsFull() ) {
    Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
    pItem1 = FetchBuf->InsertItem(_addr + offset, inst, FALSE);
    if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
    if( !pItem1->IsBranchInst() ) {
      pItem1->SetNewPC(_addr + (offset += 4));
    }
    else {
      switch( pItem1->GetBranchType() ) {
      case __BA_ANNUL:
	pItem1->SetNewPC(pItem1->GetTargetAddr());
	return;
      case __BA:
	pItem1->SetNewPC(_addr + (offset += 4));
	Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
	pItem2 = FetchBuf->InsertItem(_addr + offset, inst, TRUE);
	if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
	if( pItem2->IsBranchInst() ) {
	  sprintf(errtile, "(%08lx, inst=%08lx)", _addr + offset, inst);
	  warnmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
	}
        pItem2->SetNewPC(pItem1->GetTargetAddr());
	return;
      case __BN_ANNUL:
	offset += 4;
      case __BN:
	pItem1->SetNewPC(_addr + (offset += 4));
	break;
      case __BICC:
	pItem1->SetNewPC(_addr + (offset += 4));
	Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
	pItem2 = FetchBuf->InsertItem(_addr + offset, inst, TRUE, TRUE);
	if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
	if( pItem2->IsBranchInst() ) {
	  sprintf(errtile, "(%08lx, inst=%08lx)", _addr + offset, inst);
	  warnmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
	}
	if( pItem1->IsPredTaken() ) {
	  pItem2->SetNewPC(pItem1->GetTargetAddr());
	  return;
	}
	else {
	  pItem2->SetNewPC(_addr + (offset += 4));
	  break;
	}
      case __BICC_ANNUL:
	if( pItem1->IsPredTaken() ) {
	  pItem1->SetNewPC(_addr + (offset += 4));
	  Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
	  pItem2 = FetchBuf->InsertItem(_addr + offset, inst, TRUE);
	  if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
	  if( pItem2->IsBranchInst() ) {
	    sprintf(errtile, "(%08lx, inst=%08lx)", _addr + offset, inst);
	    warnmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
	  }
	  pItem2->SetNewPC(pItem1->GetTargetAddr());
	  return;
	}
	else {
	  pItem1->SetNewPC(_addr + (offset += 4) + 4);
	  Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
	  pItem2 = FetchBuf->InsertItem(_addr + offset, inst, TRUE, TRUE, TRUE, pItem1->GetSeqNum());
	  if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
	  if( pItem2->IsBranchInst() ) {
	    sprintf(errtile, "(%08lx, inst=%08lx)", _addr + offset, inst);
	    warnmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
	  }
	  pItem2->SetNewPC(_addr + (offset += 4));
	  break;
	}
      case __CALL:
	pItem1->SetNewPC(_addr + (offset += 4));
	Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
	pItem2 = FetchBuf->InsertItem(_addr + offset, inst, TRUE);
	if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
	if( pItem2->IsBranchInst() ) {
	  sprintf(errtile, "(%08lx, inst=%08lx)", _addr + offset, inst);
	  warnmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
	}
	pItem2->SetNewPC(pItem1->GetTargetAddr());
	return;
      case __JUMPL:
	pItem1->SetNewPC(_addr + (offset += 4));
	Mem->Read(_addr + offset, (ubyte_t*) & inst, sizeof(uword_t));
	pItem2 = FetchBuf->InsertItem(_addr + offset, inst, TRUE, TRUE);
	if((_addr + offset) == _breakpt) (*_breakflag) = TRUE;
	if( pItem2->IsBranchInst() ) {
	  sprintf(errtile, "(%08lx, inst=%08lx)", _addr + offset, inst);
	  warnmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
	}
	pItem2->SetNewPC(pItem1->GetTargetAddr());
	return;
      }
    }
  }
}
