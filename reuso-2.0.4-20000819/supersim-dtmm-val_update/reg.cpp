
/*
 * reg.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#include<stdio.h>
#include"all.h"

////////////////////////////////////////////////////////////////
///// CReg

CReg::CReg()
{
  Busy = FALSE;

  Seq = 0;

  RVal = 0;

  FVal = 0;
}

int CReg::IsBusy()
{
  return Busy;
}

void CReg::SetBusy(uword_t _seq)
{
  Busy = TRUE;
  Seq = _seq;
}

void CReg::ClearBusy()
{
  Busy = FALSE;
}

uword_t CReg::GetSeqNum()
{
  return Seq;
}

uword_t CReg::GetValue()
{
  return FVal;
}

uword_t CReg::GetRValue()
{
  return RVal;
}

void CReg::SetValue(uword_t _val)
{
  RVal = _val;
}

void CReg::SetRValue(uword_t _val)
{
  RVal = _val;
  FVal = _val;
}

void CReg::SetFValue(uword_t _val)
{
  FVal = _val;
}

void CReg::Update(uword_t _seq, uword_t _val)
{
  if( IsBusy() && (GetSeqNum() == _seq) ) {
    FVal = _val;
    Busy = FALSE;
  }
}

void CReg::FreeSeqNum(uword_t _seq)
{
  if(Seq > _seq)
    Seq -= 1;
}

void CReg::Flush()
{
  FVal = RVal;
  Busy = FALSE;
}

void CReg::Flush(uword_t _seq)
{
  if(Seq > _seq) {
    FVal = RVal;
    Busy = FALSE;
  }
}

void CReg::Reset()
{
  Busy = FALSE;

  Seq = 0;

  RVal = 0;

  FVal = 0;
}
