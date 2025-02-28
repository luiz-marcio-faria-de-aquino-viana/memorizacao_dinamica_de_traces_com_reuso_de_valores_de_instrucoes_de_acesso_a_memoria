
/*
 * predict.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/8/99
 */

#include<stdio.h>
#include"all.h"

///////////////////////////////////////////////////////////////
///// CPredict

void CPredict::Init( CBtb* _btb )
{
  Btb = _btb;
}

int CPredict::Evaluate(uword_t _addr, bool_t* _bpred, uword_t* _baddr)
{
  return (Btb->Evaluate(_addr, _bpred, _baddr));
}
