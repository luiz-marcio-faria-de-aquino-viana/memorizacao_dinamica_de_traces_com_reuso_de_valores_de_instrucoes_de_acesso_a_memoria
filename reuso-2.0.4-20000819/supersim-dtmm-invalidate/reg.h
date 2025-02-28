
/*
 * reg.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#ifndef __REG_H
#define __REG_H 153

/////////////////////////////////////////////////////////////
///// CReg

class CReg {
 private:

  bool_t  Busy;             // indicador de registrador ocupado

  uword_t Seq;              // numero de sequencia

 public:

  uword_t RVal;             // armazenamento do valor real

  uword_t FVal;             // armazenamento do valor futuro


  CReg();


  int IsBusy();


  void SetBusy(uword_t _seq);

  void ClearBusy();


  uword_t GetSeqNum();


  uword_t GetValue();

  uword_t GetRValue();


  void SetValue(uword_t _val);

  void SetRValue(uword_t _val);

  void SetFValue(uword_t _val);


  void Update(uword_t _seq, uword_t _val);

  void FreeSeqNum(uword_t _seq);

  void Flush();

  void Flush(uword_t _seq);

  void Reset();

};

#endif
