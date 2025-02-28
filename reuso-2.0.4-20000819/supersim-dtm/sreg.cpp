
/*
 * sreg.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#include<stdio.h>
#include"all.h"

//////////////////////////////////////////////////////////////
///// CSReg

void CSReg::FreeSeqNum(uword_t _seq)
{
  Y.FreeSeqNum(_seq);
  Psr.FreeSeqNum(_seq);
}

void CSReg::Flush()
{
  Y.Flush();
  Psr.Flush();
}

void CSReg::Flush(uword_t _seq)
{
  Y.Flush(_seq);
  Psr.Flush(_seq);
}

void CSReg::Reset()
{
  Psr.Reset();
  Y.Reset();
}

void CSReg::Show()
{
  printf("\nService registers");
  printf("\n=================\n\n");

  printf("\n\tB  Tag   R Value    F Value  ");
  printf("\n\t= ===== ========== ==========\n");
  printf("PSR\t%c [%2lu] (%08lx) (%08lx)\n", (Psr.IsBusy() ? '*' : ' '), Psr.GetSeqNum() % MAX_SEQNUM, Psr.RVal, Psr.FVal);
  printf("Y  \t%c [%2lu] (%08lx) (%08lx)\n", (Y.IsBusy() ? '*' : ' '), Y.GetSeqNum() % MAX_SEQNUM, Y.RVal, Y.FVal);

  printf("\n");
}

void CSReg::Show2()
{
  printf("\nService registers");
  printf("\n=================\n\n");

  if( Psr.IsBusy() || Y.IsBusy() ) {
    printf("\n\t Tag   R Value    F Value  ");
    printf("\n\t===== ========== ==========\n");
  }

  if( Psr.IsBusy() )
    printf("PSR\t[%2lu] (%08lx) (%08lx)\n", Psr.GetSeqNum() % MAX_SEQNUM, Psr.RVal, Psr.FVal);
  if( Y.IsBusy() )
    printf("Y  \t[%2lu] (%08lx) (%08lx)\n", Y.GetSeqNum() % MAX_SEQNUM, Y.RVal, Y.FVal);

  printf("\n");
}

