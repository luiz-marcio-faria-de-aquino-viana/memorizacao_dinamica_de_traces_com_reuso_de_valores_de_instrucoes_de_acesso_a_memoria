
/*
 * maq.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#ifndef __MAQ_H
#define __MAQ_H 190

/////////////////////////////////////////////////////////////
///// CMaqItem

class CMaqItem : public CDisqItem {
 private:

  bool_t Waiting;

  bool_t MAddrReady;

 public:

  uword_t MAddr;         // endereco de acesso a memoria


  uword_t HiVal;         // bits de maior ordem do registrador fonte

  uword_t LoVal;         // bits de menor ordem do registrador fonte


  int IsReady();


  int IsMAddrReady();


  void SetMAddr(uword_t _maddr, uword_t _hival = 0, uword_t _loval = 0);

  void SetMAddrReady();


  int IsWaiting();

  void SetWaiting();

  void SetNotWaiting();


  void SetItem(CDisqItem _di);

};


/////////////////////////////////////////////////////////////
///// CMaq

class CMaq : public CQueue {
 private:

  CMaqItem* pMaqArray;

 public:

  CMaq(uword_t _nentries);

  ~CMaq();

  
  bool_t HaveStorePending();


  void FreeSeqNum(uword_t _seq);

  void FreeBranchLevel(uword_t _blevel);


  CMaqItem* InsertItem(CDisqItem* _di);

  CMaqItem* GetItem(uword_t _seq);

  void RemoveItem(uword_t _seq);


  CMaqItem* GetNthItem(uword_t _idx);


  CMaqItem* GetLastItem();


  void EvalInstState();


  void Flush(uword_t _blevel);


  void Reset();

  void Show();

};

#endif
