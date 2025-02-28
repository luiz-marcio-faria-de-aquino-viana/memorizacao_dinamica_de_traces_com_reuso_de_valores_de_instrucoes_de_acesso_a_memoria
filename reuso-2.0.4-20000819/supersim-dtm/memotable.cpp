
/*
 * memotable.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#include<stdio.h>
#include<string.h>
#include"all.h"


///////////////////////////////////////////////////////////////
///// CContext

CContext::CContext()
{
  regtype = CTX_NONE;
}

CContext::CContext(uword_t _regtype, uword_t _regaddr, uword_t _regval)
{
  regtype = _regtype;
  regaddr = _regaddr;
  regval = _regval;
}

CContext & CContext::operator=(CContext & op)
{
  regtype = op.regtype;
  regaddr = op.regaddr;
  regval = op.regval;

  return *this;
}

int CContext::operator==(CContext & _op)
{
  return ( (regtype == _op.regtype) &&
	   (regaddr == _op.regaddr) &&
	   (regval == _op.regval) );
}


///////////////////////////////////////////////////////////////
///// CMemoTableEntry

CMemoTableEntry::CMemoTableEntry()
{
  next = prev = 0;
}

CMemoTableEntry::~CMemoTableEntry()
{
  Unlink();
}

CMemoTableEntry* CMemoTableEntry::Link(CMemoTableEntry* p)
{
  next = p;

  if(p != 0) {
    prev = p->prev;
    p->prev = this;
  }
  else
    prev = 0;

  return this;
}

CMemoTableEntry* CMemoTableEntry::Unlink()
{
  if(prev != 0) prev->next = next;
  if(next != 0) next->prev = prev;
  next = prev = 0;
  return this;
}

CMemoTableEntry & CMemoTableEntry::operator=(CMemoTableEntry & _it)
{
  PC = _it.PC;
  return * this;
}


///////////////////////////////////////////////////////////////
///// CMemoTable

CMemoTable::CMemoTable(uword_t _nentries)
{
  if((iMemoTable = new CMemoTableEntry* [(NEntries = _nentries)]) == 0)
    errmsg(__MEMOTABLE_H, ERR_CANTALLOCMEM, "");
  LRUList.p_mru = LRUList.p_lru = 0;
  pCurrent = 0;
  Size = 0;
}

CMemoTable::~CMemoTable()
{
  delete [] iMemoTable;
}

void CMemoTable::Init(CMemoTableEntry* arr)
{
  for(uword_t i = 0; i < NEntries; i++)
    iMemoTable[i] = & arr[i];
}

CMemoTableEntry* CMemoTable::AddNew(uword_t _pc)
{
  CMemoTableEntry* p;

#if(DEBUG == 0)
  printf("\nMEMOTABLE::ADDNEW(%lx)", _pc);
#endif

  if(Size < NEntries) {
#if(DEBUG == 0)
    printf("\n\tMEMOTABLE::ADDNEW(%lx)::FROMBAG", _pc);
#endif
    p = iMemoTable [Size];
    Size += 1;
  }
  else {
#if(DEBUG == 0)
    printf("\n\tMEMOTABLE::ADDNEW(%lx)::REALLOC", _pc);
#endif
    if(LRUList.p_lru == 0)
      errmsg(__MEMOTABLE_H, ERR_SIMARCHFAIL, "");
    p = LRUList.p_lru;
    LRUList.p_lru = p->prev;
    p->Unlink();
  }

  p->PC = _pc;
  p->Link(LRUList.p_mru);

  LRUList.p_mru = p;

  if(LRUList.p_lru == 0)
    LRUList.p_lru = p;

  return p;
}

bool_t CMemoTable::AtEnd()
{
  return (LRUList.p_lru == pCurrent);
}

CMemoTableEntry* CMemoTable::GetCurrent()
{
  return pCurrent;
}

CMemoTableEntry* CMemoTable::GoTop()
{
  return (pCurrent = LRUList.p_mru);
}

CMemoTableEntry* CMemoTable::MoveNext()
{
  if((pCurrent != 0) && (pCurrent->next != 0))
    return (pCurrent = pCurrent->next);
  return 0;
}

CMemoTableEntry* CMemoTable::FindFirst(uword_t _pc)
{
#if(DEBUG == 0)
  printf("\nMEMOTABLE::FINDFIRST(%lx)", _pc);
#endif

  pCurrent = LRUList.p_mru;
  while(pCurrent != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
    pCurrent = pCurrent->next;
  }
  return 0;
}

CMemoTableEntry* CMemoTable::FindNext(uword_t _pc)
{
#if(DEBUG == 0)
  printf("\nMEMOTABLE::FINDNEXT(%lx)", _pc);
#endif

  if(pCurrent == 0)
    return 0;

  while((pCurrent = pCurrent->next) != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
  }
  return 0;
}

CMemoTableEntry* CMemoTable::Update(CMemoTableEntry* p)
{
#if(DEBUG == 0)
  printf("\nMEMOTABLE::UPDATE(%lx)", p->PC);
#endif

  LRUList.p_lru = p->prev;
  p->Unlink();

  p->Link(LRUList.p_mru);
  LRUList.p_mru = p;

  if(LRUList.p_lru == 0)
    LRUList.p_lru = p;

  return p;
}
