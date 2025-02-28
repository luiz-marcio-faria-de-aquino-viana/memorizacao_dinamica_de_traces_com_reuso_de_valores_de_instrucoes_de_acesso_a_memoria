
/*
 * memotableg.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#include<stdio.h>
#include<string.h>
#include"all.h"


///////////////////////////////////////////////////////////////
///// CMemoTableEntryG

CMemoTableEntryG::CMemoTableEntryG()
{
  Count = 0;
  next = prev = 0;
  hashnext = hashprev = 0;
}

CMemoTableEntryG::~CMemoTableEntryG()
{
  Unlink();
  HashUnlink();
}

CMemoTableEntryG* CMemoTableEntryG::Link(CMemoTableEntryG* p)
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

CMemoTableEntryG* CMemoTableEntryG::Unlink()
{
  if(prev != 0) prev->next = next;
  if(next != 0) next->prev = prev;
  next = prev = 0;
  return this;
}

CMemoTableEntryG* CMemoTableEntryG::HashLink(CMemoTableEntryG* p)
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

CMemoTableEntryG* CMemoTableEntryG::HashUnlink()
{
  if(hashprev != 0) hashprev->hashnext = hashnext;
  if(hashnext != 0) hashnext->hashprev = hashprev;
  hashnext = hashprev = 0;
  return this;
}

CMemoTableEntryG & CMemoTableEntryG::operator=(CMemoTableEntryG & _it)
{
  PC = _it.PC;

  JMP = _it.JMP;

  BRC = _it.BRC;

  BTaken = _it.BTaken;

  Ic_src1 = _it.Ic_src1;

  Ic_src2 = _it.Ic_src2;

  Ic_rdhi = _it.Ic_rdhi;

  Ic_rdlo = _it.Ic_rdlo;

  Ic_psr = _it.Ic_psr;

  Ic_y = _it.Ic_y;

  Oc_rdhi = _it.Oc_rdhi;

  Oc_rdlo = _it.Oc_rdlo;

  Oc_psr = _it.Oc_psr;

  Oc_y = _it.Oc_y;

  MAddr = _it.MAddr;

  nPC = _it.nPC;

  return *this;
}

void CMemoTableEntryG::Display()
{
    printf( "\n%4lu (%08lx) ",
	    Count,
	    PC );

    if(JMP == TRUE)
      printf("J ");
    else
      printf("- ");

    if(BRC == TRUE) {
      if(BTaken == TRUE)
	printf("BT ");
      else
	printf("BN ");
    }
    else
      printf("-- ");

    printf( "{ ic(src1=%lx, src2=%lx, rdhi=%lx, rdlo=%lx, psr=%lx, y=%lx), oc(rdhi=%lx, rdlo=%lx, psr=%lx, y=%lx), maddr=%lx, npc=%lx }",
	    Ic_src1,
	    Ic_src2,
	    Ic_rdhi,
	    Ic_rdlo,
	    Ic_psr,
	    Ic_y,
	    Oc_rdhi,
	    Oc_rdlo,
	    Oc_psr,
	    Oc_y,
	    MAddr,
	    nPC );

}


///////////////////////////////////////////////////////////////
///// CMemoTableG

CMemoTableG::CMemoTableG(uword_t _nentries)
{
  uword_t i;
  if((pMemoTableG = new CMemoTableEntryG [(NEntries = _nentries)]) == 0)
    errmsg(__MEMOTABLEG_H, ERR_CANTALLOCMEM, "");
  LRUList.p_mru = LRUList.p_lru = 0;
  for(i = 0; i < MEMOTABLEG_NSLOTS; i++)
    HashTable[i].p_mru = HashTable[i].p_lru = 0;
  pCurrent = 0;
  Size = 0;
}

CMemoTableG::~CMemoTableG()
{
  delete [] pMemoTableG;
}

CMemoTableEntryG* CMemoTableG::AddNew( uword_t _pc,
				       bool_t  _jmp,
				       bool_t  _brc,
				       bool_t  _btaken,
				       uword_t _ic_src1,
				       uword_t _ic_src2,
				       uword_t _ic_rdhi,
				       uword_t _ic_rdlo,
				       uword_t _ic_psr,
				       uword_t _ic_y,
				       uword_t _oc_rdhi,
				       uword_t _oc_rdlo,
				       uword_t _oc_psr,
				       uword_t _oc_y,
				       uword_t _maddr,
				       uword_t _npc )
{
  CMemoTableEntryG* p;
  uword_t nslot, nslotold;

  nslot = (_pc / 4) % MEMOTABLEG_NSLOTS;

  if(Size < NEntries) {
    p = & pMemoTableG [Size];
    Size += 1;
  }
  else {
#if(DEBUG <= 9)
  fprintf(stderr, "\nMEMOTABLEG::ADDNEW::BEGIN(pc=%lx,nslot=%lu)", _pc, nslot);
#endif

    if(LRUList.p_lru == 0)
      errmsg(__MEMOTABLEG_H, ERR_SIMARCHFAIL, "");

    p = LRUList.p_lru;

    LRUList.p_lru = p->prev;
    if(LRUList.p_lru == 0)
      LRUList.p_mru = 0;
    p->Unlink();

    nslotold = (p->PC / 4) % MEMOTABLEG_NSLOTS;

    if(p->hashprev == 0)
      HashTable[nslotold].p_mru = p->hashnext;
    if(p->hashnext == 0)
      HashTable[nslotold].p_lru = p->hashprev;
    p->HashUnlink();

#if(DEBUG <= 9)
  fprintf(stderr, "\nMEMOTABLEG::ADDNEW::END");
#endif
  }

  p->PC = _pc;
  p->JMP = _jmp;
  p->BRC = _brc;
  p->BTaken = _btaken;
  p->Ic_src1 = _ic_src1;
  p->Ic_src2 = _ic_src2;
  p->Ic_rdhi = _ic_rdhi;
  p->Ic_rdlo = _ic_rdlo;
  p->Ic_psr = _ic_psr;
  p->Ic_y = _ic_y;
  p->Oc_rdhi = _oc_rdhi;
  p->Oc_rdlo = _oc_rdlo;
  p->Oc_psr = _oc_psr;
  p->Oc_y = _oc_y;
  p->MAddr = _maddr;
  p->nPC = _npc;

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

bool_t CMemoTableG::AtEnd()
{
  return (LRUList.p_lru == pCurrent);
}

CMemoTableEntryG* CMemoTableG::GetCurrent()
{
  return pCurrent;
}

CMemoTableEntryG* CMemoTableG::GoTop()
{
  return (pCurrent = LRUList.p_mru);
}

CMemoTableEntryG* CMemoTableG::MoveNext()
{
  if((pCurrent != 0) && (pCurrent->next != 0))
    return (pCurrent = pCurrent->next);
  return 0;
}

CMemoTableEntryG* CMemoTableG::FindFirst(uword_t _pc)
{
  uword_t nslot;
  nslot = (_pc / 4) % MEMOTABLEG_NSLOTS;

  pCurrent = HashTable[nslot].p_mru;
  while(pCurrent != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
    pCurrent = pCurrent->hashnext;
  }
  return 0;
}

CMemoTableEntryG* CMemoTableG::FindNext(uword_t _pc)
{
  if(pCurrent == 0)
    return 0;

   while((pCurrent = pCurrent->hashnext) != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
  }
  return 0;
}

CMemoTableEntryG* CMemoTableG::Update(CMemoTableEntryG* p)
{
  uword_t nslot;
  nslot = (p->PC / 4) % MEMOTABLEG_NSLOTS;

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
