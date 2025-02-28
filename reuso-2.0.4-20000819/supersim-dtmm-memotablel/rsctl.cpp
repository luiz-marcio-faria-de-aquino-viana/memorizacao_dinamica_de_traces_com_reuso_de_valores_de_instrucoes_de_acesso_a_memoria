
/*
 * rs.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/13/99
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////
///// CRsctl

CRsctl::CRsctl()
{
  Ready = FALSE;
}

int CRsctl::IsReady()
{
  return ( !IsReqAsSource() ? TRUE : Ready );
}

void CRsctl::SetDep(uword_t _seq)
{
  Ready = FALSE;
  Dep = _seq;
}

void CRsctl::SetDep(uword_t _r, uword_t _seq)
{
  Ready = FALSE;
  Reg = _r;
  Dep = _seq;
}

uword_t CRsctl::GetDep()
{
  return Dep;
}

void CRsctl::DecDep(uword_t _seq)
{
  if(Dep > _seq) Dep -= 1;
}

void CRsctl::SetValue(uword_t _val)
{
  Ready = TRUE;
  Val = _val;
}

void CRsctl::SetValue(uword_t _r, uword_t _val)
{
  Ready = TRUE;
  Reg = _r;
  Val = _val;
}

uword_t CRsctl::GetValue()
{
  return Val;
}

void CRsctl::ClearDep(uword_t _val)
{
  Ready = TRUE;
  Val = _val;
}

int CRsctl::Update(uword_t _seq, uword_t _val)
{
  if( !IsReady() && (Dep == _seq) ) {
    Ready = TRUE;
    Val = _val;
  }
  return Ready;
}

int CRsctl::Update(uword_t _seq, uword_t _srcwin, uword_t _cwp, uword_t _reg, uword_t _val)
{
  if( !IsReady() && (Dep == _seq) && (IREG_GETABSADDR(_srcwin, Reg) == IREG_GETABSADDR(_cwp, _reg)) ) {
    Ready = TRUE;
    Val = _val;
  }
  return Ready;
}

void CRsctl::Set(CIDecctl _it)
{
  *((CIDecctl*) this) = _it;
  Ready = FALSE;
}
