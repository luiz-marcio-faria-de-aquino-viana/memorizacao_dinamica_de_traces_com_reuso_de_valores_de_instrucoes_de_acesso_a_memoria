
/*
 * ireg.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#ifndef __IREG_H
#define __IREG_H 155

/////////////////////////////////////////////////////////////
///// CIReg

class CIReg {
 private:

  uword_t GNumRegs;

  CReg* pGRegArray;


  uword_t NumWin;

  uword_t WinSize;

  uword_t WinOffset;

  CReg* pWRegArray;


  uword_t Cwp;


  CReg* GetNthReg(uword_t _cwp, uword_t _reg);


 public:


  CIReg( uword_t _gnumregs,
	 uword_t _numwin,
	 uword_t _winsize,
	 uword_t _winoffset );

  ~CIReg();


  int IsBusy(uword_t _cwp, uword_t _reg);

  void SetBusy(uword_t _cwp, uword_t _reg, uword_t _seq);

  void ClearBusy(uword_t _cwp, uword_t _reg);


  uword_t GetSeqNum(uword_t _cwp, uword_t _reg);


  uword_t GetValue(uword_t _cwp, uword_t _reg);

  uword_t GetRValue(uword_t _cwp, uword_t _reg);


  void SetValue(uword_t _cwp, uword_t _reg, uword_t _val);

  void SetRValue(uword_t _cwp, uword_t _reg, uword_t _val);

  void SetFValue(uword_t _cwp, uword_t _reg, uword_t _val);


  void Update(uword_t _seq, uword_t _cwp, uword_t _reg, uword_t _val);


  void FreeSeqNum(uword_t _seq);


  uword_t GetCurrentWin();

  void MoveToNextWin();

  void MoveToPrevWin();


  void Flush(uword_t _cwp);

  void Flush(uword_t _cwp, uword_t _seq);


  void Reset();


  void Show();

  void Show2();


  void Watch(uword_t _cwp, uword_t _reg);

};

#endif
