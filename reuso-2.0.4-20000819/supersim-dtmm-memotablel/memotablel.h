
/*
 * memotablel.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#ifndef __MEMOTABLEL_H
#define __MEMOTABLEL_H 419


///////////////////////////////////////////////////////////////
///// CMemoTableEntryL

class CMemoTableEntryL : public CMemoTableEntryG {
 public:

  CMemoTableEntryL();

  ~CMemoTableEntryL();

};

///////////////////////////////////////////////////////////////
///// CMemoTableL

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

  CMemoTableEntryL* AddNew( uword_t _pc,
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
			    uword_t _npc );

  CMemoTableEntryL* Update(CMemoTableEntryL* p);


  bool_t AtEnd();


  CMemoTableEntryL* GetCurrent();


  CMemoTableEntryL* GoTop();

  CMemoTableEntryL* MoveNext();


  CMemoTableEntryL* FindFirst(uword_t _pc);

  CMemoTableEntryL* FindNext(uword_t _pc);


  void Invalidate(uword_t _ptr, uword_t _len);

  void UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len);

};

#endif
