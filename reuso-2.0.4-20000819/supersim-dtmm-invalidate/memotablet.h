
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

  struct {

    ubyte_t LdSt;       // (0=none/1=load/2=store)

    bool_t LdStChain;  // TRUE=instrucao de load servido por store no mesmo trace
                       // FALSE=instrucao de load servida por store externo ao trace

    uword_t Type;      // 0x00 = se instrucao nao faz leitura/escrita
                       // 0x01 = se instrucao le/escreve byte
                       // 0x02 = se instrucao le byte sem sinal
                       // 0x04 = se instrucao le/escreve meia palavra
                       // 0x08 = se instrucao le meia palavra sem sinal
                       // 0x10 = se instrucao le/escreve palavra
                       // 0x40 = se instrucao le/escreve palavra dupla


    uword_t MAddr;     // endereco de acesso a memoria da instrucao de escrita


    bool_t MValid;     // sinalizador de valor valido para armazenamento

    uword_t MValhi;    // valor mais significativo a ser armazenado na memoria

    uword_t MVallo;    // valor menos significativo a ser armazenado na memoria


    CRsctl Rdhi;       // registrador destino para a palavra mais significativa

    CRsctl Rdlo;       // registrador destino para a palavra menos significativa

  } LdStInst [ LDST_NUM ];

  int LdStSz;


  //////////////////////////////////////////////////////////////////
  //// contabilizacao estatistica da frequencia dos tipos de instrucoes

  statistics_t TraceStats;


  CMemoTableEntryT* next;

  CMemoTableEntryT* prev;


  CMemoTableEntryT* hashnext;

  CMemoTableEntryT* hashprev;


  /* construtores da classe */

  CMemoTableEntryT();

  ~CMemoTableEntryT();


  /* funcoes para manipulacao da mascara de desvio */

  bool_t IsFullBranchMask();

  void AddBranchMask( uword_t _btype,
		      bool_t _btaken,
		      uword_t _baddr,
		      uword_t _bdest );


  /* funcoes para manipulacao de instrucoes de acesso a memoria */

  bool_t IsValidLoad();             // retorna verdadeiro quando trace possuir instrucoes de load com valor valido

  bool_t IsFullLoadStore();         // retorna verdadeiro para maximo numero de load/store

  bool_t CheckForLoadStoreChain(uword_t _maddr);   // verifica escritas/leitura na mesma posiacao de memoria

  bool_t CheckForLoadStoreInst(uword_t _type); // verifica se trace possui acesso a memoria
                                               // (use: IOM_LOAD | IOM_STORE)

  void AddLoadStoreInst( bool_t _ldst,
			 uword_t _type,
			 uword_t _maddr,
			 bool_t _mvalid,
			 CIDecctl* _rd_hi,
			 uword_t _mvalhi,
			 CIDecctl* _rd_lo,
			 uword_t _mvallo );


  /* funcoes de manipulacao do contexto */

  // contexto de entrada
  void NewInputContext(uword_t _regtype, uword_t _regval);

  void NewInputContext(uword_t _regtype, uword_t _regaddr, uword_t _regval);

  // contexto de saida
  void NewOutputContext(uword_t _regtype, uword_t _regval);

  void NewOutputContext(uword_t _regtype, uword_t _regaddr, uword_t _regval);


  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _execunit - unidade de execucao
  // _instop - opcode da instrucao
  void AddInstrCount(uword_t _execunit, uword_t _instop);  


  /* funcoes para manipulacao de instrucoes de delay slot */

  bool_t BiccAtEnd();

  void AddDelayInst(uword_t _daddr, uword_t _dinst);


  /* funcao de reinicializacao da entrada */

  void Reset();


  /* funcao padrao de apresentacao da entrada no terminal */

  void Display();


  /* funcoes de basicas controle */

  // controle da fila RLU
  CMemoTableEntryT* Link(CMemoTableEntryT* p);

  CMemoTableEntryT* Unlink();

  // controle da tabela hash
  CMemoTableEntryT* HashLink(CMemoTableEntryT* p);

  CMemoTableEntryT* HashUnlink();


  /* funcoes de sobreposicao de operadores */

  CMemoTableEntryT & operator=(CMemoTableEntryT & _it);

};



//////////////////////////////////////////////////////////////////
///// CMemoTableT

class CMemoTableT {
 private:

  /* definicao do pool de entradas que podem ser utilizadas */

  CMemoTableEntryT* pMemoTableT;        // vetor de entradas

  uword_t NEntries;                     // numero total de entradas do vetor

  uword_t Size;                         // numero de entradas ocupadas


  /* definicao do tipo descritor para as listas de controle da MEMO_TABLE_T */

  struct lrulist_t {

    CMemoTableEntryT* p_mru;
  
    CMemoTableEntryT* p_lru;

  };


  /* listas de controle da MEMO_TABLE_T */

  struct lrulist_t LRUList;                            // descritor da lista LRU

  struct lrulist_t HashTable [ MEMOTABLET_NSLOTS ];    // descritor da tabela hash


  /* ponteiro para a entrada corrente da MEMO_TABLE_T */

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


  void Invalidate(uword_t _ptr, uword_t _len);

  void UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len);

};

#endif
