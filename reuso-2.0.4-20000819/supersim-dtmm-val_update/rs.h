
/*
 * rs.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/13/99
 */

#ifndef __RS_H
#define __RS_H 170

///////////////////////////////////////////////////////////
///// CRsItem

class CRsItem : public CDisqItem {
 private:

  bool_t Busy;


  uword_t SourceWin;      // janela de registradores fonte das operacoes

  uword_t TargetWin;      // janela de registradores destino das operacoes

 public:

  CRsctl Psr;          // controle do registrador de estado

  CRsctl Y;            // controle do registrador Y

  CRsctl Rs1;          // controle do primeiro registrador fonte

  CRsctl Rs2;          // controle do segundo registrador fonte

  CRsctl Rd_hi;        // controle da palavra mais significativa do registrador destino

  CRsctl Rd_lo;        // controle da palavra menos significativa do registrador destino


  int IsBusy();

  void SetBusy();


  int IsReady();


  void SetSourceWin(uword_t _cwin);

  uword_t GetSourceWin();


  void SetTargetWin(uword_t _cwin);

  uword_t GetTargetWin();


  void SetItem(CDisqItem _di);

};

//////////////////////////////////////////////////////////////
///// CRs

class CRs : public CQueue {
 private:

  CRsItem* pRsArray;

 public:

  CRs(uword_t _nentries);

  ~CRs();


  bool_t HaveStorePending();


  void FreeSeqNum(uword_t _seq);

  void FreeBranchLevel(uword_t _blevel);


  CRsItem* InsertItem(CDisqItem* _di);

  CRsItem* GetItem(uword_t _seq);

  void RemoveItem(uword_t _seq);


  CRsItem* GetFirstItem();

  void RemoveFirstItem();


  CRsItem* GetReadyItem();


  void Update( uword_t _seq,
	       uword_t _cwp,
	       uword_t _rd_hi,
	       uword_t _val_hi,
	       uword_t _rd_lo,
	       uword_t _val_lo,
	       uword_t _val_psr,
	       uword_t _val_y );

  void UpdatePsr(uword_t _seq, uword_t _val_psr);

  void UpdateY(uword_t _seq, uword_t _val_y);

  void UpdateIReg( uword_t _seq,
		   uword_t _cwp,
		   uword_t _raddr,
		   uword_t _rval );


  void Flush(uword_t _blevel);


  void Reset();

  void Show();
};

#endif
