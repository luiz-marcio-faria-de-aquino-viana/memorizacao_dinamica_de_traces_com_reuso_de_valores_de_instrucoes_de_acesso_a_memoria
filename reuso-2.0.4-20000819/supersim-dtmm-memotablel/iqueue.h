
/*
 * iqueue.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/6/99
 */

#ifndef __IQUEUE_H
#define __IQUEUE_H 140


///////////////////////////////////////////////////////////
///// CIQueueItem

class CIQueueItem : public CFetchBufItem {
 public:

  void SetItem(CFetchBufItem _fb);

  CIQueueItem & operator=(CIQueueItem & _iq);

};

///////////////////////////////////////////////////////////
///// CIQueue

class CIQueue : public CQueue {
 private:

  CIQueueItem* pIQueueArray;

 public:


  CIQueue(uword_t _nentries);

  ~CIQueue();


  void FreeSeqNum(uword_t _seq);

  void FreeBranchLevel(uword_t _blevel);


  CIQueueItem* InsertItem(CFetchBufItem* _fb);

  CIQueueItem* GetFirstItem();

  CIQueueItem* GetNthItem(int nth);

  void RemoveFirstItem();


  void RemoveBranchDepItem(uword_t _bseq);


  void Flush();

  void Flush(uword_t _blevel, uword_t _bseq);


  void Reset();

  void Show();
};

#endif
