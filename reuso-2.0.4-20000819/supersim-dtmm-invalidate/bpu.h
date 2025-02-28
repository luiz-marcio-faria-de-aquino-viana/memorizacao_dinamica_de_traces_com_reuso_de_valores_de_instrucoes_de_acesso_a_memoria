
/*
 * bpu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/23/99
 */

#ifndef __BPU_H
#define __BPU_H 185

////////////////////////////////////////////////////////////
///// CBpu

class CBpu {
 private:

  CBtb* Btb;

  CFetchBuf* FetchBuf;

  CFetch* Fetch;

  CIQueue* IQueue;

  CDisq* Disq;

  CDisq* DBDisq;

  CIReg* IReg;

  CSReg* SReg;

  CRs* RsIu;

  CRs* RsBpu;

  CRs* RsLsu;

  CMaq* Maq;

  CRob* Rob;

  CIu* Iu;

  CLsu* Lsu;


  CResBuf* ResBuf;

  uword_t NEntries;


  bool_t Trapsig;

  uword_t Trapnum;


  uword_t Operand1;

  uword_t Operand2;

  uword_t Result1;


  bool_t BResult;

  uword_t NewPC;


 public:

  void Init( CBtb* _btb,
	     CFetchBuf* _fetchbuf,
	     CIQueue* _iqueue,
	     CFetch* _fetch,
	     CDisq* _disq, 
	     CDisq* _dbdisq,
	     CIReg* _ireg, 
	     CSReg* _sreg,
	     CRs* _rsiu, 
	     CRs* _rsbpu, 
	     CRs* _rslsu,
	     CMaq* _maq, 
	     CRob* _rob,
	     CIu* _iu, 
	     CLsu* _lsu );
 

 CBpu(uword_t _nentries);

  ~CBpu();


  uword_t GetNumUnits();

  CResBuf* GetNthResBuf(uword_t _idx);


  void Step(CResBuf* _rb);


  void Execute();


  void Flush(uword_t _seq);


  void Reset();


  void Show();

};

#endif

