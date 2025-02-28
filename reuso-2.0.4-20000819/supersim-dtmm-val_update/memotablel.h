
/*
 * memotablel.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#ifndef __MEMOTABLEL_H
#define __MEMOTABLEL_H 480


///////////////////////////////////////////////////////////////
///// CMemoTableEntryL

class CMemoTableEntryL {
 public:

  uword_t MAddr;      // endereco de memoria para reuso de load/store

  uword_t MVal;       // valor contido na memoria

  uword_t Count;      // numero de vezes em que uma instrucao e reusada

  CMemoTableEntryL* next;

  CMemoTableEntryL* prev;

  CMemoTableEntryL* hashnext;

  CMemoTableEntryL* hashprev;

  CMemoTableEntryL();

  ~CMemoTableEntryL();

  CMemoTableEntryL* Link(CMemoTableEntryL* p);

  CMemoTableEntryL* Unlink();

  CMemoTableEntryL* HashLink(CMemoTableEntryL* p);

  CMemoTableEntryL* HashUnlink();

  CMemoTableEntryL & operator=(CMemoTableEntryL & _it);

  void Display();

};


///////////////////////////////////////////////////////////////
///// CMemoTableG

class CMemoTableL {
 private:

  CMemoTableEntryL* pMemoTableL;

  uword_t NEntries;

  uword_t Size;


  struct lrulist_t {

    CMemoTableEntryL* p_mru;
  
    CMemoTableEntryL* p_lru;

  };


  struct lrulist_t LRUList;

  struct lrulist_t HashTable [ MEMOTABLEL_NSLOTS ];


  CMemoTableEntryL* pCurrent;

 public:

  CMemoTableL(uword_t _nentries);

  ~CMemoTableL();

  CMemoTableEntryL* AddNew( uword_t _maddr,
       			    uword_t _mval );

  CMemoTableEntryL* Update(CMemoTableEntryL* p);


  bool_t AtEnd();


  CMemoTableEntryL* GetCurrent();


  CMemoTableEntryL* GoTop();

  CMemoTableEntryL* MoveNext();


  CMemoTableEntryL* FindFirst(uword_t _maddr);

  CMemoTableEntryL* FindNext(uword_t _maddr);

};

#endif
