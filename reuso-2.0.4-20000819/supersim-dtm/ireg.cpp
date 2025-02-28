
/*
 * ireg.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#include<stdio.h>
#include"all.h"

//////////////////////////////////////////////////////////////
///// CIReg

CIReg::CIReg( uword_t _gnumregs,
	      uword_t _numwin,
	      uword_t _winsize,
	      uword_t _winoffset )
{
  NumWin = _numwin;

  WinSize = _winsize;

  WinOffset = _winoffset;

  if((pGRegArray = new CReg [(GNumRegs = _gnumregs)]) == 0)
    errmsg(__IREG_H, ERR_CANTALLOCMEM, "(error in global registers)");

  if((pWRegArray = new CReg [NumWin * WinSize]) == 0)
    errmsg(__IREG_H, ERR_CANTALLOCMEM, "(error in local registers)");

  Reset();
}

CIReg::~CIReg()
{
  delete [] pGRegArray;
  delete [] pWRegArray;
}

CReg* CIReg::GetNthReg(uword_t _cwp, uword_t _reg)
{
  if(_reg >= (WinSize + GNumRegs))
    errmsg(__IREG_H, ERR_INVREGISTER, "");

  if(_reg < GNumRegs)
    return & pGRegArray[_reg];
  return & pWRegArray[(WinOffset * _cwp) + _reg - GNumRegs];
}

int CIReg::IsBusy(uword_t _cwp, uword_t _reg)
{
  return GetNthReg(_cwp, _reg)->IsBusy();
}

void CIReg::SetBusy(uword_t _cwp, uword_t _reg, uword_t _seq)
{
  GetNthReg(_cwp, _reg)->SetBusy(_seq);
}

void CIReg::ClearBusy(uword_t _cwp, uword_t _reg)
{
  GetNthReg(_cwp, _reg)->ClearBusy();
}

uword_t CIReg::GetSeqNum(uword_t _cwp, uword_t _reg)
{
  return GetNthReg(_cwp, _reg)->GetSeqNum();
}

uword_t CIReg::GetValue(uword_t _cwp, uword_t _reg)
{
  return GetNthReg(_cwp, _reg)->GetValue();
}

uword_t CIReg::GetRValue(uword_t _cwp, uword_t _reg)
{
  return GetNthReg(_cwp, _reg)->GetRValue();
}

void CIReg::SetValue(uword_t _cwp, uword_t _reg, uword_t _val)
{
  GetNthReg(_cwp, _reg)->SetValue(_val);
}

void CIReg::SetRValue(uword_t _cwp, uword_t _reg, uword_t _val)
{
  GetNthReg(_cwp, _reg)->SetRValue(_val);
}

void CIReg::SetFValue(uword_t _cwp, uword_t _reg, uword_t _val)
{
  GetNthReg(_cwp, _reg)->SetFValue(_val);
}

void CIReg::Update(uword_t _seq, uword_t _cwp, uword_t _reg, uword_t _val)
{
  GetNthReg(_cwp, _reg)->Update(_seq, _val);
}

void CIReg::FreeSeqNum(uword_t _seq)
{
  uword_t i;
  for(i = 0; i < GNumRegs; i++)
    pGRegArray[i].FreeSeqNum(_seq);
  for(i = 0; i < NumWin * WinSize; i++)
    pWRegArray[i].FreeSeqNum(_seq);
}

uword_t CIReg::GetCurrentWin()
{
  return Cwp;
}

void CIReg::MoveToNextWin()
{
  if(Cwp >= (NumWin - 1))
    errmsg(__IREG_H, ERR_WINOVERFLOW, "");
  Cwp += 1;
}

void CIReg::MoveToPrevWin()
{
  if(Cwp <= 0)
    errmsg(__IREG_H, ERR_WINUNDERFLOW, "");
  Cwp -= 1;
}

void CIReg::Flush(uword_t _cwp)
{
  uword_t i;
  for(i = 0; i < GNumRegs; i++)
    pGRegArray[i].Flush();
  for(i = 0; i < NumWin * WinSize; i++)
    pWRegArray[i].Flush();
  Cwp = _cwp;
}

void CIReg::Flush(uword_t _cwp, uword_t _seq)
{
  uword_t i;
  for(i = 0; i < GNumRegs; i++)
    pGRegArray[i].Flush(_seq);
  for(i = 0; i < NumWin * WinSize; i++)
    pWRegArray[i].Flush(_seq);
  Cwp = _cwp;
}

void CIReg::Reset()
{
  CReg* r;
  uword_t i;

  for(i = 0; i < GNumRegs; i++)
    pGRegArray[i].Reset();

  for(i = 0; i < NumWin * WinSize; i++)
    pWRegArray[i].Reset();

  Cwp = NumWin / 2;

  (r = GetNthReg(Cwp, R14))->SetValue(INISP);
  r->Flush();

  (r = GetNthReg(Cwp, R30))->SetValue(INISP);
  r->Flush();

}

void CIReg::Show()
{
  CReg* p;
  uword_t i;

  printf("\nInteger registers");
  printf("\n=================\n\n");

  if(Cwp < NumWin) {
    printf("\nPrevious CWP = %lu (after restore)", Cwp + 1);

    printf("\n\tB  Tag   R Value    F Value  ");
    printf("\n\t= ===== ========== ==========\n");

    for(i = 0; i < GNumRegs + WinSize; i++) {
      p = GetNthReg(Cwp + 1, i);
      printf("R[%02lu]\t%c  [%2lu] (%08lx) (%08lx)\n", i, (p->IsBusy() ? '*' : ' '), p->GetSeqNum() % MAX_SEQNUM, p->RVal, p->FVal);
    }

    printf("\n");
  }

  printf("\nCurrent CWP = %lu", Cwp);
  
  printf("\n\tB  Tag   R Value    F Value  ");
  printf("\n\t= ===== ========== ==========\n");
  
  for(i = 0; i < GNumRegs + WinSize; i++) {
    p = GetNthReg(Cwp, i);
    printf("R[%02lu]\t%c  [%2lu] (%08lx) (%08lx)\n", i, (p->IsBusy() ? '*' : ' '), p->GetSeqNum() % MAX_SEQNUM, p->RVal, p->FVal);
  }

  printf("\n");

  if(Cwp > 0) {
    printf("\nNext CWP = %lu (after save)", Cwp - 1);

    printf("\n\tB  Tag   R Value    F Value  ");
    printf("\n\t= ===== ========== ==========\n");

    for(i = 0; i < GNumRegs + WinSize; i++) {
      p = GetNthReg(Cwp - 1, i);
      printf("R[%02lu]\t%c  [%2lu] (%08lx) (%08lx)\n", i, (p->IsBusy() ? '*' : ' '), p->GetSeqNum() % MAX_SEQNUM, p->RVal, p->FVal);
    }

    printf("\n");
  }
}

void CIReg::Show2()
{
  CReg* p;
  uword_t i;

  int flg;

  printf("\nInteger registers");
  printf("\n=================\n");

  if(Cwp < NumWin) 
  {
    printf("\nPrevious CWP = %lu (after restore)\n", Cwp + 1);

    flg = 0;
    for(i = 0; i < GNumRegs + WinSize; i++) 
    {
      p = GetNthReg(Cwp + 1, i);
      if( p->IsBusy() ) 
      {
	if(flg == 0) 
        {
	  printf("\n\t Tag   R Value    F Value  ");
	  printf("\n\t===== ========== ==========\n");
	  flg = 1;
	}
	printf("R[%02lu]\t [%2lu] (%08lx) (%08lx)\n", i, p->GetSeqNum() % MAX_SEQNUM, p->RVal, p->FVal);
      }
    }

    if(flg == 0) printf("\n\t( none )\n");

    printf("\n");
  }

  printf("\nCurrent CWP = %lu\n", Cwp);
  
  flg = 0;
  for(i = 0; i < GNumRegs + WinSize; i++) 
  {
    p = GetNthReg(Cwp, i);
    if( p->IsBusy() ) 
    {
      if(flg == 0) 
      {
	printf("\n\t Tag   R Value    F Value  ");
	printf("\n\t===== ========== ==========\n");
	flg = 1;
      }
      printf("R[%02lu]\t [%2lu] (%08lx) (%08lx)\n", i, p->GetSeqNum() % MAX_SEQNUM, p->RVal, p->FVal);
    }
  }

  if(flg == 0) printf("\n\t( none )\n");

  printf("\n");

  if(Cwp > 0) 
  {
    printf("\nNext CWP = %lu (after save)\n", Cwp - 1);

    flg = 0;
    for(i = 0; i < GNumRegs + WinSize; i++) 
    {
      p = GetNthReg(Cwp - 1, i);
      if( p->IsBusy() ) 
      {
	if(flg == 0) 
        {
	  printf("\n\t Tag   R Value    F Value  ");
	  printf("\n\t===== ========== ==========\n");
	  flg = 1;
	}
	printf("R[%02lu]\t [%2lu] (%08lx) (%08lx)\n", i, p->GetSeqNum() % MAX_SEQNUM, p->RVal, p->FVal);
      }
    }

    if(flg == 0) printf("\n\t( none )\n");

    printf("\n");
  }
}

void CIReg::Watch(uword_t _cwp, uword_t _reg)
{
  CReg* p;
  p = GetNthReg(_cwp, _reg);
  printf( "R[%02lu]\t Seq=[%2lu] Real=(%08lx) Future=(%08lx)\n",
	  _reg,
	  p->GetSeqNum() % MAX_SEQNUM,
	  p->RVal,
	  p->FVal );
}

