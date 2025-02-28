
/*
 * disq.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#ifndef __DISQ_H
#define __DISQ_H 150

/////////////////////////////////////////////////////////////
///// CDisqItem

class CDisqItem : public CIDec {
 public:

  CMemoTableEntryG RedundantInst;           // bloco de dados para reuso de instrucoes

  CMemoTableEntryT TraceData;               // bloco de dados para reuso de traces


  void SetItem(CIDec _idec);

  CDisqItem& operator=(CDisqItem& _di);

};


/////////////////////////////////////////////////////////////
///// CDisq

class CDisq : public CQueue {
 private:

  CDisqItem* pDisqArray;

 public:


  CDisq(uword_t _nentries);

  ~CDisq();


  void FreeSeqNum(uword_t _seq);

  void FreeBranchLevel(uword_t _blevel);


  CDisqItem* InsertItem(CIDec* _idec);

  CDisqItem* InsertItem(CDisqItem* _di);


  CDisqItem* GetFirstItem();

  CDisqItem* GetNthItem(int nth);

  CDisqItem* GetNotBranchDepItem();


  void RemoveFirstItem();


  CDisqItem* GetLastItem();


  CDisqItem* GetBranchDepItem(uword_t _bseq);


  void RemoveBranchDepItem(uword_t _bseq);

  void RemoveNotBranchDepItem();



  void Flush(uword_t _seq);

  void Flush(uword_t _blevel, uword_t _bseq);


  void Reset();


  void Show();

};

#endif
