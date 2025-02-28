
/*
 * lsu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/17/99
 */

#ifndef __LSU_H
#define __LSU_H 180

////////////////////////////////////////////////////////////
///// CLsu

class CLsu {
 private:

  CDMmu* DMmu;

  CFetchBuf* FetchBuf;

  CMaq* Maq;

  CRs* RsLsu;

  CRob* Rob;


  CResBuf* ResBuf;

  uword_t NEntries;


  uword_t Address;

  uword_t Result1;

  uword_t Result2;


 public:


  void Init( CDMmu* _dmmu,
	     CFetchBuf* _fetchbuf,
	     CMaq* _maq,
	     CRs* _rslsu,
	     CRob* _rob );


  CLsu(uword_t _nentries);

  ~CLsu();


  bool_t HaveStorePending();


  uword_t GetNumUnits();

  CResBuf* GetNthResBuf(uword_t _idx);


  CMaqItem* GetItemFromMaq(CRsItem** _rsit);

  CMaqItem* GetReadyItemFromMaq(CRsItem** _rsit);

  void Step_AddressCalculation();

  void Step_MemoryAccess(CResBuf* _rb);


  void Execute();


  void Flush(uword_t _blevel);


  void Reset();


  void Show();

};

#endif







