
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
  MemoTableType = MEMOTABLEL;
}

CMemoTableEntryL::~CMemoTableEntryL()
{
  /* nothing todo! */
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

CMemoTableEntryL* CMemoTableL::AddNew( uword_t _pc,
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
				       uword_t _ldst,
				       uword_t _ldsttype,
				       uword_t _maddr,
				       uword_t _mvalid,
				       uword_t _mvalhi,
				       uword_t _mvallo,
				       uword_t _npc )
{
  CMemoTableEntryL* p;
  uword_t nslot, nslotold;

  nslot = (_pc / 4) % MEMOTABLEL_NSLOTS;

  if(Size < NEntries) {
    p = & pMemoTableL [Size];
    Size += 1;
  }
  else {
#if(DEBUG <= 9)
  fprintf(stderr, "\nMEMOTABLEL::ADDNEW::BEGIN(pc=%lx,nslot=%lu)", _pc, nslot);
#endif

    if(LRUList.p_lru == 0)
      errmsg(__MEMOTABLEL_H, ERR_SIMARCHFAIL, "");

    p = LRUList.p_lru;

    LRUList.p_lru = (CMemoTableEntryL*) p->prev;
    if(LRUList.p_lru == 0)
      LRUList.p_mru = 0;
    p->Unlink();

    nslotold = (p->PC / 4) % MEMOTABLEL_NSLOTS;

    if(p->hashprev == 0)
      HashTable[nslotold].p_mru = (CMemoTableEntryL*) p->hashnext;
    if(p->hashnext == 0)
      HashTable[nslotold].p_lru = (CMemoTableEntryL*) p->hashprev;
    p->HashUnlink();

#if(DEBUG <= 9)
    fprintf(stderr, "\nMEMOTABLEL::ADDNEW::END");
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
  p->LdSt = _ldst;
  p->LdStType = _ldsttype;
  p->MAddr = _maddr;
  p->MValid = _mvalid;
  p->MValhi = _mvalhi;
  p->MVallo = _mvallo;
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

void CMemoTableL::Invalidate(uword_t _ptr, uword_t _len)
{
  CMemoTableEntryL * p;

  p = LRUList.p_mru;
  while(p != NULL) {
    if((p->LdSt & IOM_LOAD) == IOM_LOAD) {

      // sobreposicao em memoria ocorre quando (Mmax >= Pmin) e (Mmin <= Pmax)

      switch(p->LdStType) {
      case RD_BYTE:
      case RD_UBYTE:
	if( ((p->MAddr + sizeof(byte_t) - 1) >= _ptr) &&
	    (p->MAddr <= (_ptr + _len)) )
	  p->MValid = FALSE;
	break;

      case RD_HWORD:
      case RD_UHWORD:
	if( ((p->MAddr + sizeof(hword_t) - 1) >= _ptr) &&
	    (p->MAddr <= (_ptr + _len)) )
	  p->MValid = FALSE;
	break;

      case RD_WORD:
	if( ((p->MAddr + sizeof(word_t) - 1) >= _ptr) &&
	    (p->MAddr <= (_ptr + _len)) )
	  p->MValid = FALSE;
	break;

      case RD_DWORD:
	if( ((p->MAddr + sizeof(dword_t) - 1) >= _ptr) &&
	    (p->MAddr <= (_ptr + _len)) )
	  p->MValid = FALSE;
	break;

      } 
    }
    p = (CMemoTableEntryL*) p->next;
  }
}

void CMemoTableL::UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len)
{
  CMemoTableEntryL * p;

  dword_t res;
  uword_t pos, n;

  dword_t mask;
  uword_t hi_mask, lo_mask;

  p = LRUList.p_mru;
  while(p != NULL) {
    if((p->LdSt & IOM_LOAD) == IOM_LOAD) {

      switch(p->LdStType) {
      case RD_BYTE:
      case RD_UBYTE:
	n = sizeof(byte_t);
	break;

      case RD_HWORD:
      case RD_UHWORD:
	n = sizeof(hword_t);
	break;

      case RD_WORD:
	n = sizeof(word_t);
	break;

      case RD_DWORD:
	n = sizeof(dword_t);

      } 

      // sobreposicao em memoria ocorre quando (Mmax >= Pmin) e (Mmin <= Pmax)

      res = 0; mask = 0;
      if( (p->MAddr <= (_maddr + _len - 1)) && ((p->MAddr + n - 1) >= _maddr) ) {
	for(uword_t i = 0; i < n ; i++) {
	  if( ((p->MAddr + i) >= _maddr) && ((p->MAddr + i) <= (_maddr + _len - 1)) ) {
	    pos = p->MAddr - _maddr + i;
	    res = (res << 8) | _ptr[pos];
	    mask = (mask << 8) | 0x0FFL;
#if(DEBUG <= 19)
	    fprintf(stderr, "%02x ", _ptr[pos]);
#endif
	  }
	  else {
	    res = (res << 8);
	    mask = (mask << 8);
#if(DEBUG <= 19)
	    fprintf(stderr, "00 ");
#endif
	  }
	}

	if(p->LdStType != RD_DWORD) {
	  // hi
	  hi_mask = (uword_t) (mask & 0x0FFFFFFFFL);
	  p->MValhi &= ~hi_mask;
	  p->MValhi |= (uword_t) (res & 0x0FFFFFFFFL);
	  // lo
	  p->MVallo = 0;
	}
	else {
	  // hi
	  hi_mask = (uword_t) ((mask >> 32) & 0x0FFFFFFFFL);
	  p->MValhi &= ~hi_mask;
	  p->MValhi |= (uword_t) ((res >> 32) & 0x0FFFFFFFFL);
	  // lo
	  lo_mask = (uword_t) (mask & 0x0FFFFFFFFL);
	  p->MVallo &= ~lo_mask;
	  p->MVallo |= (uword_t) (res & 0x0FFFFFFFFL);
	}

#if(DEBUG <= 19)
	fprintf( stderr,
		 "\nMEMO_TABLE_L::UPDATEMEMVALUE(maddr=%lx, nbytes=%lx, [m=%lx, k=%lx], res=%Lx, [valhi=%lx, vallo=%lx)\n",
		 p->MAddr,
		 n,
		 _maddr,
		 _len,
		 res,
		 p->MValhi,
		 p->MVallo );
#endif
      }

    }

    p = (CMemoTableEntryL*) p->next;
  }

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
    return (pCurrent = (CMemoTableEntryL*) pCurrent->next);
  return 0;
}

CMemoTableEntryL* CMemoTableL::FindFirst(uword_t _pc)
{
  uword_t nslot;
  nslot = (_pc / 4) % MEMOTABLEL_NSLOTS;

  pCurrent = HashTable[nslot].p_mru;
  while(pCurrent != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
    pCurrent = (CMemoTableEntryL*) pCurrent->hashnext;
  }
  return 0;
}

CMemoTableEntryL* CMemoTableL::FindNext(uword_t _pc)
{
  if(pCurrent == 0)
    return 0;

   while((pCurrent = (CMemoTableEntryL*) pCurrent->hashnext) != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
  }
  return 0;
}

CMemoTableEntryL* CMemoTableL::Update(CMemoTableEntryL* p)
{
  uword_t nslot;
  nslot = (p->PC / 4) % MEMOTABLEL_NSLOTS;

  if(p->next == 0)
    LRUList.p_lru = (CMemoTableEntryL*) p->prev;
  if(p->prev == 0)
    LRUList.p_mru = (CMemoTableEntryL*) p->next;
  p->Unlink();

  if(p->hashnext == 0)
    HashTable[nslot].p_lru = (CMemoTableEntryL*) p->hashprev;
  if(p->hashprev == 0)
    HashTable[nslot].p_mru = (CMemoTableEntryL*) p->hashnext;
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


