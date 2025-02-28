
/*
 * idecctl.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#include<stdio.h>
#include"all.h"

extern str_t errtile;

//////////////////////////////////////////////////////////////
///// CIDecctl

CIDecctl::CIDecctl()
{
  Req = REQ_NONE;
}

void CIDecctl::Reset()
{
  Req = REQ_NONE;
}

void CIDecctl::Set(ubyte_t _req, uword_t _reg)
{
  Req |= _req;
  Reg = _reg;
}

bool_t CIDecctl::IsReqAsSource()
{
  return BITTEST(Req, REQ_SOURCE);
}

bool_t CIDecctl::IsReqAsTarget()
{
  return BITTEST(Req, REQ_TARGET);
}

uword_t CIDecctl::GetRegAddr()
{
  return Reg;
}

CIDecctl &
CIDecctl::operator=(CIDecctl & _it)
{
  Req = _it.Req;
  Reg = _it.Reg;
  return *this;
}
