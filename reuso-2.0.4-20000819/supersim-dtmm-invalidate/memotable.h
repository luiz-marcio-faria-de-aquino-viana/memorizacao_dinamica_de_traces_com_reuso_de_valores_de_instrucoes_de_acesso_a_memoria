
/*
 * memotable.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#ifndef __MEMOTABLE_H
#define __MEMOTABLE_H 410


///////////////////////////////////////////////////////////////
///// CContext

class CContext {
 public:

  uword_t regtype;

  uword_t regaddr;

  uword_t regval;

  CContext();

  CContext(uword_t _regtype, uword_t _regaddr, uword_t _regval);

  CContext & operator=(CContext & _op);

  int operator==(CContext & _op);

};


///////////////////////////////////////////////////////////////
///// CMemoTableEntry

class CMemoTableEntry {
 public:

  uword_t PC;

  CMemoTableEntry* next;

  CMemoTableEntry* prev;

  CMemoTableEntry();

  ~CMemoTableEntry();

  CMemoTableEntry* Link(CMemoTableEntry* p);

  CMemoTableEntry* Unlink();

  CMemoTableEntry & operator=(CMemoTableEntry & _it);

};


///////////////////////////////////////////////////////////////
///// CMemoTable

class CMemoTable : public CMemoTableEntry {
 protected:

  CMemoTableEntry** iMemoTable;

  uword_t NEntries;

  uword_t Size;

  struct {

    CMemoTableEntry* p_mru;

    CMemoTableEntry* p_lru;

  } LRUList;

  CMemoTableEntry* pCurrent;

 public:

  CMemoTable(uword_t _nentries);

  ~CMemoTable();

  void Init(CMemoTableEntry* arr);

  CMemoTableEntry* AddNew(uword_t _pc);

  CMemoTableEntry* Update(CMemoTableEntry* p);


  bool_t AtEnd();


  CMemoTableEntry* GetCurrent();


  CMemoTableEntry* GoTop();

  CMemoTableEntry* MoveNext();


  CMemoTableEntry* FindFirst(uword_t _pc);

  CMemoTableEntry* FindNext(uword_t _pc);

};

#endif
