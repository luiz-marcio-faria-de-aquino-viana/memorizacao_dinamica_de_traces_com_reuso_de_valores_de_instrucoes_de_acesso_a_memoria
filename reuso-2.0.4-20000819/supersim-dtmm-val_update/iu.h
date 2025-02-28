
/*
 * iu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/17/99
 */

#ifndef __IU_H
#define __IU_H 175

////////////////////////////////////////////////////////////
///// CIu

class CIu {
 private:

  CRs* RsIu;


  CResBuf* ResBuf;

  uword_t NEntries;


  uword_t Operand1;

  uword_t Operand2;

  uword_t Result1;

  uword_t Y;

  uword_t Psr;

  icc_t Icc;


  uword_t Shift_count;


 public:


  void Init( CRs* RsIu );


  CIu(uword_t _nentries);

  ~CIu();


  uword_t GetNumUnits();

  CResBuf* GetNthResBuf(uword_t _idx);

  
  void Step(CResBuf* _rb);


  void Execute();


  void Flush(uword_t _seq);


  void Reset();


  void Show();

};

#endif
