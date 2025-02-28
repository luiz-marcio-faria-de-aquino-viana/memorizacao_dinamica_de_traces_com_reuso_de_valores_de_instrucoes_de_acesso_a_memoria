
/*
 * memotable.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/12/2000
 */

#ifndef __MEMOTABLEG_H
#define __MEMOTABLEG_H 417


///////////////////////////////////////////////////////////////
///// CMemoTableEntryG

class CMemoTableEntryG {
 public:

  uword_t PC;

  bool_t JMP;

  bool_t BRC;

  bool_t BTaken;

  uword_t Ic_src1;

  uword_t Ic_src2;

  uword_t Ic_rdhi;

  uword_t Ic_rdlo;

  uword_t Ic_psr;

  uword_t Ic_y;

  uword_t Oc_rdhi;

  uword_t Oc_rdlo;

  uword_t Oc_psr;

  uword_t Oc_y;

  uword_t LdSt;       // (0=none/1=load/2=store)

  uword_t LdStType;   // RType | WType

  uword_t MAddr;      // endereco de memoria para reuso de instrucoes de load/store

  uword_t MValid;     // sinalizador de valor valido para armazenamento

  uword_t MValhi;     // valor mais significativo a ser armazenado em memoria

  uword_t MVallo;     // valor menos significativo a ser armazenado em memoria

  uword_t nPC;

  uword_t Count;      // numero de vezes em que uma instrucao e reusada

  CMemoTableEntryG* next;

  CMemoTableEntryG* prev;

  CMemoTableEntryG* hashnext;

  CMemoTableEntryG* hashprev;

  CMemoTableEntryG();

  ~CMemoTableEntryG();

  CMemoTableEntryG* Link(CMemoTableEntryG* p);

  CMemoTableEntryG* Unlink();

  CMemoTableEntryG* HashLink(CMemoTableEntryG* p);

  CMemoTableEntryG* HashUnlink();

  CMemoTableEntryG & operator=(CMemoTableEntryG & _it);

  void Display();

};


///////////////////////////////////////////////////////////////
///// CMemoTableG

class CMemoTableG {
 private:

  CMemoTableEntryG* pMemoTableG;

  uword_t NEntries;

  uword_t Size;


  struct lrulist_t {

    CMemoTableEntryG* p_mru;
  
    CMemoTableEntryG* p_lru;

  };


  struct lrulist_t LRUList;

  struct lrulist_t HashTable [ MEMOTABLEG_NSLOTS ];


  CMemoTableEntryG* pCurrent;

 public:

  CMemoTableG(uword_t _nentries);

  ~CMemoTableG();

  CMemoTableEntryG* AddNew( uword_t _pc,
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

  CMemoTableEntryG* Update(CMemoTableEntryG* p);


  bool_t AtEnd();


  CMemoTableEntryG* GetCurrent();


  CMemoTableEntryG* GoTop();

  CMemoTableEntryG* MoveNext();


  CMemoTableEntryG* FindFirst(uword_t _pc);

  CMemoTableEntryG* FindNext(uword_t _pc);


  void Invalidate(uword_t _ptr, uword_t _len);

  void UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len);

};

#endif
