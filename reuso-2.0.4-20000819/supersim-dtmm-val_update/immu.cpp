
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

////////////////////////////////////////////////////////////////////////////////////
//void CIMmu::Execute(uword_t _addr, bool_t *_breakflag, uword_t _breakpt)
//{
//  CFetchBufItem *pI0, *pI1, *pI2, *pI3;
//  uword_t I0, I1, I2, I3;
//
//  uword_t offset;
//
//  offset = 0;
//  while( !FetchBuf.IsFull() ) {
//    Mem->Read(_addr + offset, (ubyte_t*) & I0, sizeof(uword_t));
//    pI0 = FetchBuf->InsertItem(_addr + offset, I0);
//    BREAKPT(*_breakflag, _addr + offset == _breakpt);
//
//    if( !pI0->IsBranchInst() ) 
//    {
//      /* i0 NAO E' instrucao de desvio */
//      offset += 4;
//      pI0->SetNewPC(_addr + offset);
//    }
//    else
//    {
//      /* i0 E' instrucao de desvio */
//      switch( pI0->GetBranchType() ) {
//      case __BA_ANNUL:
//	pI0->SetNewPC(pI0->GetTargetAddr());
//	return;
//
//      case __BA:
//      case __CALL:
//	offset += 4;
//	pI0->SetNewPC(_addr + offset);
//	
//	Mem->Read(_addr + offset, (ubyte_t*) & I1, sizeof(uword_t));
//	pI1 = FetchBuf->InsertItem(_addr + offset, I1, TRUE, TRUE);
//	BREAKPT(*_breakflag, _addr + offset == _breakpt);
//
//	if( !pI1->IsBranchInst() ) 
//	{
//	  /* i1 NAO E' instrucao de desvio */
//	  offset += 4;
//	  pI1->SetNewPC(pI0->GetTargetAddr());
//	}
//	else
//	{
//	  /* i1 E' instrucao de desvio */
//	  switch( pI1->GetBranchType() ) {
//	  case __BA_ANNUL:
//	    pI1->SetNewPC(pI1->GetTargetAddr());
//	    return;
//	    
//	  case __JUMPL:
//	    sprintf(errtile, "(DTCIu-JMPL, %08lx, inst=%08lx)", pI1->GetTargetAddr(), inst);
//	    errmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
//	    return;
//
//	  case __CALL:
//	  case __BA:
//	    pI1->SetNewPC(pI0->GetTargetAddr());
//	    
//	    Mem->Read(pI0->GetTargetAddr(), (ubyte_t*) & I2, sizeof(uword_t));
//	    pI2 = FetchBuf->InsertItem(pI0->GetTargetAddr(), I2, TRUE, TRUE);
//	    BREAKPT(*_breakflag, pI0->GetTargetAddr() == _breakpt);
//	    if( !pI2->IsBranchInst() ) 
//	    {
//	      /* i2 NAO E' instrucao de desvio */
//	      pI2->SetNewPC(pI1->GetTargetAddr());
//	    }
//	    else
//	    {
//	      /* i2 E' instrucao de desvio - ERRO */
//	      sprintf(errtile, "(DCTIu-DCTIu-CTIany, %08lx, inst=%08lx)", pI0->GetTargetAddr(), inst);
//	      errmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
//	    }
//	    return;
//
//	  case __BICC:
//	    /* i1 E' instrucao de desvio - ERRO */
//	    sprintf(errtile, "(DCTIu-Bicc, %08lx, inst=%08lx)", _addr + offset, inst);
//	    errmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
//	    break;
//	    
//	  case __BICC_ANNUL:
//	    /* i1 E' instrucao de desvio - ERRO */
//	    sprintf(errtile, "(DCTIu-Bicc (a=1), %08lx, inst=%08lx)", _addr + offset, inst);
//	    errmsg(__IMMU_H, ERR_BRANCHAFTERBRANCH, errtile);
//	    break;
//	    
//	  case __BN_ANNUL:
//
//	    break;
//	    
//	  case __BN:
//	    break;
//	    
//	  }
//
//	}
//	    
//	break;
//
//      case __JUMPL:
//	break;
//
//      case __BICC:
//	break;
//
//      case __BICC_ANNUL:
//	break;
//
//      case __BN_ANNUL:
//	break;
//
//      case __BN:
//	break;
//
//      }
//
//    }
//
//  }
//
//}
////////////////////////////////////////////////////////////////////////////////////
