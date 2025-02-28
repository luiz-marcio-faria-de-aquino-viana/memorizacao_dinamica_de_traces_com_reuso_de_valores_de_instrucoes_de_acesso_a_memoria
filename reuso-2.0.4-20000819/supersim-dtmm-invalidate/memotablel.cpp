
/*
 * memotablel.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#include<stdio.h>
#include<string.h>
#include"all.h"


///////////////////////////////////////////////////////////////
///// CMemoTableEntryL

CMemoTableEntryL::CMemoTableEntryL()
{
  Count = 0;
  next = prev = 0;
  hashnext = hashprev = 0;
}

CMemoTableEntryL::~CMemoTableEntryL()
{
  Unlink();
  HashUnlink();
}

CMemoTableEntryL* CMemoTableEntryL::Link(CMemoTableEntryL* p)
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

CMemoTableEntryL* CMemoTableEntryL::Unlink()
{
  if(prev != 0) prev->next = next;
  if(next != 0) next->prev = prev;
  next = prev = 0;
  return this;
}

CMemoTableEntryL* CMemoTableEntryL::HashLink(CMemoTableEntryL* p)
{
  hashnext = p;

  if(p != 0) {
    hashprev = p->hashprev;
    p->hashprev = this;
  }
  else
    hashprev = 0;

  return this;
}

CMemoTableEntryL* CMemoTableEntryL::HashUnlink()
{
  if(hashprev != 0) hashprev->hashnext = hashnext;
  if(hashnext != 0) hashnext->hashprev = hashprev;
  hashnext = hashprev = 0;
  return this;
}

CMemoTableEntryL & CMemoTableEntryL::operator=(CMemoTableEntryL & _it)
{
  MAddr = _it.MAddr;
  MVal = _it.MVal;
  return *this;
}

void CMemoTableEntryL::Display()
{
    printf( "\n%4lu { maddr=%lx, mval=%lx }",
	    Count,
	    MAddr,
	    MVal );
}


///////////////////////////////////////////////////////////////
///// CMemoTableL

CMemoTableL::CMemoTableL(uword_t _nentries)
{
  uword_t i;
  if((pMemoTableL = new CMemoTableEntryL [(NEntries = _nentries)]) == 0)
    errmsg(__MEMOTABLEL_H, ERR_CANTALLOCMEM, "");
  LRUList.p_mru = LRUList.p_lru = 0;
  for(i = 0; i < MEMOTABLEL_NSLOTS; i++)
    HashTable[i].p_mru = HashTable[i].p_lru = 0;
  pCurrent = 0;
  Size = 0;
}

CMemoTableL::~CMemoTableL()
{
  delete [] pMemoTableL;
}

CMemoTableEntryL* CMemoTableL::AddNew( uword_t _maddr,
				       uword_t _mval )
{
  CMemoTableEntryL* p;
  uword_t nslot, nslotold;

  nslot = (_maddr / 4) % MEMOTABLEL_NSLOTS;

  if(Size < NEntries) {
    p = & pMemoTableL [Size];
    Size += 1;
  }
  else {
#if(DEBUG <= 9)
  fprintf(stderr, "\nMEMOTABLEL::ADDNEW::BEGIN(pc=%lx,nslot=%lu)", _maddr, nslot);
#endif

    if(LRUList.p_lru == 0)
      errmsg(__MEMOTABLEL_H, ERR_SIMARCHFAIL, "");

    p = LRUList.p_lru;

    LRUList.p_lru = p->prev;
    if(LRUList.p_lru == 0)
      LRUList.p_mru = 0;
    p->Unlink();

    nslotold = (p->MAddr / 4) % MEMOTABLEL_NSLOTS;

    if(p->hashprev == 0)
      HashTable[nslotold].p_mru = p->hashnext;
    if(p->hashnext == 0)
      HashTable[nslotold].p_lru = p->hashprev;
    p->HashUnlink();

#if(DEBUG <= 9)
  fprintf(stderr, "\nMEMOTABLEL::ADDNEW::END");
#endif
  }

  p->MAddr = _maddr;
  p->MVal = _mval;

  p->Count = 0;

  p->Link(LRUList.p_mru);
  LRUList.p_mru = p;
  if(LRUList.p_lru == 0)
    LRUList.p_lru = p;

  p->HashLink(HashTable[nslot].p_mru);
  HashTable[nslot].p_mru = p;
  if(HashTable[nslot].p_lru == 0)
    HashTable[nslot].p_lru = p;

  return p;
}

bool_t CMemoTableL::AtEnd()
{
  return (LRUList.p_lru == pCurrent);
}

CMemoTableEntryL* CMemoTableL::GetCurrent()
{
  return pCurrent;
}

CMemoTableEntryL* CMemoTableL::GoTop()
{
  return (pCurrent = LRUList.p_mru);
}

CMemoTableEntryL* CMemoTableL::MoveNext()
{
  if((pCurrent != 0) && (pCurrent->next != 0))
    return (pCurrent = pCurrent->next);
  return 0;
}

CMemoTableEntryL* CMemoTableL::FindFirst(uword_t _maddr)
{
  uword_t nslot;
  nslot = (_maddr / 4) % MEMOTABLEL_NSLOTS;

  pCurrent = HashTable[nslot].p_mru;
  while(pCurrent != 0) {
    if(pCurrent->MAddr == _maddr)
      return pCurrent;
    pCurrent = pCurrent->hashnext;
  }
  return 0;
}

CMemoTableEntryL* CMemoTableL::FindNext(uword_t _maddr)
{
  if(pCurrent == 0)
    return 0;

   while((pCurrent = pCurrent->hashnext) != 0) {
    if(pCurrent->MAddr == _maddr)
      return pCurrent;
  }
  return 0;
}

CMemoTableEntryL* CMemoTableL::Update(CMemoTableEntryL* p)
{
  uword_t nslot;
  nslot = (p->MAddr / 4) % MEMOTABLEL_NSLOTS;

  if(p->next == 0)
    LRUList.p_lru = p->prev;
  if(p->prev == 0)
    LRUList.p_mru = p->next;
  p->Unlink();

  if(p->hashnext == 0)
    HashTable[nslot].p_lru = p->hashprev;
  if(p->hashprev == 0)
    HashTable[nslot].p_mru = p->hashnext;
  p->HashUnlink();

  p->Link(LRUList.p_mru);
  LRUList.p_mru = p;
  if(LRUList.p_lru == 0)
    LRUList.p_lru = p;

  p->HashLink(HashTable[nslot].p_mru);
  HashTable[nslot].p_mru = p;
  if(HashTable[nslot].p_lru == 0)
    HashTable[nslot].p_lru = p;

  p->Count += 1;

  return p;
}
