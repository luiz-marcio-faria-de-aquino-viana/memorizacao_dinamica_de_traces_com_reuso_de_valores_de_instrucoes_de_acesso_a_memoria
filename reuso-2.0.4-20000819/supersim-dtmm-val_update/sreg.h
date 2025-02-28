
/*
 * sreg.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#ifndef __SREG_H
#define __SREG_H 157

/////////////////////////////////////////////////////////////
///// CSReg

class CSReg {
 public:

  CReg Psr;

  CReg Y;

  void FreeSeqNum(uword_t _seq);


  void Flush();

  void Flush(uword_t _seq);


  void Reset();

  void Show();

  void Show2();

};

#endif
