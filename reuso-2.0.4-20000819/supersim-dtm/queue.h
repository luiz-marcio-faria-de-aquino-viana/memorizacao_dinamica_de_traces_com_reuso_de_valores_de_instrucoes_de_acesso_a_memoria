
/*
 * queue.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/4/1999
 */

#ifndef __QUEUE_H
#define __QUEUE_H 133

//////////////////////////////////////////////////////////
///// CQueueItem

class CQueueItem {
 public:

  ubyte_t* pData;

  uword_t szData;

  CQueueItem();

};

//////////////////////////////////////////////////////////
///// CQueue

class CQueue {
 public:  

  CQueueItem* pQueueArray;

  uword_t NEntries;

  uword_t First, Last;

  uword_t Size;

  int QId;        /* identificador da fila (usado na depuracao) */

  str_t errtile;


  void Init( ubyte_t* _pdata, uword_t _szdata );

  CQueue(int _qid, uword_t _nentries);

  ~CQueue();


  uword_t GetNumEntries();


  int IsEmpty();

  int IsFull();


  uword_t GetQueueSize();


  CQueueItem* InsertItem();

  CQueueItem* GetFirstItem();

  CQueueItem* GetNthItem(uword_t _idx);

  CQueueItem* GetLastItem();


  void RemoveNthItem(uword_t _idx);

  void RemoveFirstItem();


  void Swap(uword_t _idx1, uword_t _idx2);


  void Flush();

};

#endif
