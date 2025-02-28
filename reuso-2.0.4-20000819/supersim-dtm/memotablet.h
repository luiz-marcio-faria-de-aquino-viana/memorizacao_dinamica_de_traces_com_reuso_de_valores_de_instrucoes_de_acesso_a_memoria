
/*
 * memotablet.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/22/2000
 */

#ifndef __MEMOTABLET_H
#define __MEMOTABLET_H 420


//////////////////////////////////////////////////////////////////
///// CMemoTableEntryT

class CMemoTableEntryT {
 public:

  uword_t Id;

  uword_t PC;


  struct {

    uword_t btype;

    bool_t btaken;

    uword_t baddr;

    uword_t bdest;

  } Brc [ BRC_NUM ];

  int BrcSz;


  CContext Ic [ IC_NUM ];  // contexto de entrada

  CContext Oc [ OC_NUM ];  // contexto de saida


  uword_t nPC;


  uword_t Tracesz;         // tamanho do trace

  uword_t Count;           // numero de vezes que um trace e reusado


  // instrucao de delay slot

  bool_t Delay;            // bit indicador de instrucao de delay slot apos o trace

  uword_t DAddr;           // endereco da instrucao no delay slot

  uword_t DInst;           // codigo da instrucao no delay slot 


  // instrucao de acesso a memoria

  uword_t RType;       // 0x00 = se instrucao nao faz leitura
                       // 0x01 = se instrucao le byte
                       // 0x02 = se instrucao le byte sem sinal
                       // 0x04 = se instrucao le meia palavra
                       // 0x08 = se instrucao le meia palavra sem sinal
                       // 0x10 = se instrucao le palavra
                       // 0x40 = se instrucao le palavra dupla

  uword_t WType;       // 0x00 = se instrucao nao faz escrita
                       // 0x01 = se instrucao escreve byte
                       // 0x04 = se instrucao escreve meia palavra
                       // 0x10 = se instrucao escreve palavra inteira
                       // 0x40 = se instrucao escreve palavra dupla

  uword_t MAddr;       // endereco de acesso a memoria

  CRsctl Rd_hi;      // palavra mais significativa do registrador destino

  CRsctl Rd_lo;      // palavra menos significativa do registrador destino


  //////////////////////////////////////////////////////////////////
  //// contabilizacao estatistica da frequencia dos tipos de instrucoes

  statistics_t TraceStats;


  CMemoTableEntryT* next;

  CMemoTableEntryT* prev;


  CMemoTableEntryT* hashnext;

  CMemoTableEntryT* hashprev;


  CMemoTableEntryT();

  ~CMemoTableEntryT();


  bool_t BiccAtEnd();


  void AddDelayInst(uword_t _daddr, uword_t _dinst);

  void AddMemInst( uword_t _rtype,
		   uword_t _wtype,
		   uword_t _maddr,
		   CIDecctl* _rd_hi,
		   CIDecctl* _rd_lo );


  bool_t IsFullBranchMask();

  void AddBranchMask( uword_t _btype,
		      bool_t _btaken,
		      uword_t _baddr,
		      uword_t _bdest );


  int IsLdStInst();


  void Reset();


  void NewInputContext(uword_t _regtype, uword_t _regval);

  void NewInputContext(uword_t _regtype, uword_t _regaddr, uword_t _regval);

  void NewOutputContext(uword_t _regtype, uword_t _regval);

  void NewOutputContext(uword_t _regtype, uword_t _regaddr, uword_t _regval);


  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _execunit - unidade de execucao
  // _instop - opcode da instrucao
  void AddInstrCount(uword_t _execunit, uword_t _instop);  


  CMemoTableEntryT* Link(CMemoTableEntryT* p);

  CMemoTableEntryT* Unlink();


  CMemoTableEntryT* HashLink(CMemoTableEntryT* p);

  CMemoTableEntryT* HashUnlink();


  CMemoTableEntryT & operator=(CMemoTableEntryT & _it);

  void Display();

};


//////////////////////////////////////////////////////////////////
///// CMemoTableT

class CMemoTableT {
 private:

  CMemoTableEntryT* pMemoTableT;

  uword_t NEntries;

  uword_t Size;

  struct lrulist_t {

    CMemoTableEntryT* p_mru;
  
    CMemoTableEntryT* p_lru;

  };


  struct lrulist_t LRUList;

  struct lrulist_t HashTable [ MEMOTABLET_NSLOTS ];


  CMemoTableEntryT* pCurrent;

 public:

  uword_t NextId;


  CMemoTableT(uword_t _nentries);

  ~CMemoTableT();


  CMemoTableEntryT* AddNew( uword_t _pc,
			    CContext* _ic,
			    CContext* _oc,
			    uword_t _npc );


  CMemoTableEntryT* AddNew(CMemoTableEntryT & _it);

  CMemoTableEntryT* Update(CMemoTableEntryT* p);


  bool_t AtEnd();

  CMemoTableEntryT* GetCurrent();

  CMemoTableEntryT* GoTop();

  CMemoTableEntryT* MoveNext();

  CMemoTableEntryT* FindFirst(uword_t _pc);

  CMemoTableEntryT* FindNext(uword_t _pc);

};

#endif
