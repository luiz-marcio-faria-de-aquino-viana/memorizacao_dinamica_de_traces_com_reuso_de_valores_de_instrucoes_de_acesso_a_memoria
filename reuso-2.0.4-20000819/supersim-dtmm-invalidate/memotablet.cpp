
/*
 * memotablet.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/22/2000
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////////////////
///// CMemoTableEntryT

/* construtores da classe */

CMemoTableEntryT::CMemoTableEntryT()
{
  Reset();
  next = prev = 0;
  hashnext = hashprev = 0;
}

CMemoTableEntryT::~CMemoTableEntryT()
{
  Unlink();
  HashUnlink();
}


/* funcoes para manipulacao da mascara de desvio */

bool_t CMemoTableEntryT::IsFullBranchMask()
{
  return (BrcSz >= BRC_NUM);
}

void CMemoTableEntryT::AddBranchMask( uword_t _btype,
				      bool_t _btaken,
				      uword_t _baddr,
				      uword_t _bdest )
{
  if( IsFullBranchMask() )
    errmsg(__MEMOTABLET_H, ERR_SIMARCHFAIL, "(AddBranchMask)");

  Brc[BrcSz].btype = _btype;
  Brc[BrcSz].btaken = _btaken;
  Brc[BrcSz].baddr = _baddr;
  Brc[BrcSz].bdest = _bdest;

  BrcSz += 1;
}


/* funcoes para manipulacao de instrucoes de acesso a memoria */

bool_t CMemoTableEntryT::IsValidLoad()
{
  for(int i = 0; i < LdStSz; i++) {
    if( ((LdStInst[i].LdSt & IOM_LOAD) == IOM_LOAD) && (LdStInst[i].MValid == FALSE) )
      return FALSE;
  }
  return TRUE;
}

bool_t CMemoTableEntryT::IsFullLoadStore()
{
  return(LdStSz >= LDST_NUM);
}

bool_t CMemoTableEntryT::CheckForLoadStoreChain(uword_t _maddr)
{
  for(int i = 0; i < LdStSz; i++)
    if(LdStInst[i].MAddr == _maddr) return TRUE;
  return FALSE;
}

bool_t CMemoTableEntryT::CheckForLoadStoreInst(uword_t _type)
{
  for(int i = 0; i < LdStSz; i++)
    if((LdStInst[i].LdSt & _type) != 0) return TRUE;

  return FALSE;
}

void CMemoTableEntryT::AddLoadStoreInst( bool_t _ldst,
					 uword_t _type,
					 uword_t _maddr,
					 bool_t _mvalid,
					 CIDecctl* _rd_hi,
					 uword_t _mvalhi,
					 CIDecctl* _rd_lo,
					 uword_t _mvallo )
{
  if( IsFullLoadStore() )
    errmsg(__MEMOTABLET_H, ERR_SIMARCHFAIL, "(AddLoadStoreInst)");

  LdStInst[LdStSz].LdSt = _ldst;

  if(_ldst == IOM_LOAD) {
    // se instrucao e de leitura da memoria entao
    //   verifica se existe alguma escrita a memoria no trace no mesmo endereco
    if( CheckForLoadStoreChain(_maddr) )
      LdStInst[LdStSz].LdStChain = TRUE;
    else
      LdStInst[LdStSz].LdStChain = FALSE;
  }

  LdStInst[LdStSz].Type = _type;
  LdStInst[LdStSz].MAddr = _maddr;

  LdStInst[LdStSz].MValid = _mvalid;

  LdStInst[LdStSz].Rdhi.Set( *_rd_hi );
  LdStInst[LdStSz].MValhi = _mvalhi;

  LdStInst[LdStSz].Rdlo.Set( *_rd_lo );
  LdStInst[LdStSz].MVallo = _mvallo;

  LdStSz += 1;
}


/* funcoes de manipulacao do contexto */

// contexto de entrada
void CMemoTableEntryT::NewInputContext(uword_t _regtype, uword_t _regval)
{
  int i;
  for(i = 0; i < IC_NUM; i++) {
    if(Ic[i].regtype == _regtype) return;

    if(Ic[i].regtype == CTX_NONE) {
      Ic[i].regtype = _regtype;
      Ic[i].regval = _regval;
      return;
    }
  }
}

void CMemoTableEntryT::NewInputContext(uword_t _regtype, uword_t _regaddr, uword_t _regval)
{
  int i;
  for(i = 0; i < IC_NUM; i++) {
    if((Ic[i].regtype == _regtype) && (Ic[i].regaddr == _regaddr)) return;

    if(Ic[i].regtype == CTX_NONE) {
      Ic[i].regtype = _regtype;
      Ic[i].regaddr = _regaddr;
      Ic[i].regval = _regval;
      return;
    }
  }
}

// contexto de saida
void CMemoTableEntryT::NewOutputContext(uword_t _regtype, uword_t _regval)
{
  int i;
  for(i = 0; i < OC_NUM; i++) {
    if(Oc[i].regtype == _regtype) {
      Oc[i].regval = _regval;
      return;
    }
    if(Oc[i].regtype == CTX_NONE) {
      Oc[i].regtype = _regtype;
      Oc[i].regval = _regval;
      return;
    }
  }
}

void CMemoTableEntryT::NewOutputContext(uword_t _regtype, uword_t _regaddr, uword_t _regval)
{
  int i;
  for(i = 0; i < OC_NUM; i++) {
    if((Oc[i].regtype == _regtype) && (Oc[i].regaddr == _regaddr)) {
      Oc[i].regval = _regval;
      return;
    }
    if(Oc[i].regtype == CTX_NONE) {
      Oc[i].regtype = _regtype;
      Oc[i].regaddr = _regaddr;
      Oc[i].regval = _regval;
      return;
    }
  }
}

// AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
// _execunit - unidade de execucao
// _instop - opcode da instrucao
void CMemoTableEntryT::AddInstrCount(uword_t _execunit, uword_t _instop)
{
  switch( _execunit ) {
  case IU:
    /* instrucoes logicas, aritimeticas e de multiplicacao */
    switch( _instop ) {
    case ADD:
    case ADDCC:
    case ADDX:
    case ADDXCC:
    case SUB:
    case SUBCC:
    case SUBX:
    case SUBXCC:
      TraceStats.IC_arithmetic += 1;
      break;

    case MULSCC:
      TraceStats.IC_mult += 1;
      break;

    case AND:
    case ANDCC:
    case ANDN:
    case ANDNCC:
    case OR:
    case ORCC:
    case ORN:
    case ORNCC:
    case XOR:
    case XORCC:
    case XNOR:
    case XNORCC:
    case SLL:
    case SRL:
    case SRA:
      TraceStats.IC_logic += 1;
      break;

    case SETHI:
      TraceStats.IC_sethi += 1;
      break;

    case SAVE:
      TraceStats.IC_save += 1;
      break;

    case RESTORE:
      TraceStats.IC_restore += 1;
      break;

    case RDPSR:
    case RDY:
    case WRPSR:
    case WRY:
      TraceStats.IC_others += 1;
      break;

    default:
      TraceStats.IC_others += 1;

    }
    break;

  case BPU:
    /* instrucoes de desvios e chamadas ao sistema operacional */
    switch( _instop ) {
    case CALL:
      TraceStats.IC_call += 1;
      break;

    case BICC:
      TraceStats.IC_bicc += 1;
      break;

    case JMPL:
      TraceStats.IC_jmpl += 1;
      break;

    case TICC:
      TraceStats.IC_ticc += 1;
      break;

    default:
      TraceStats.IC_others += 1;
    }
    break;

  case LSU:
    /* instrucoes de acesso a memoria */
    switch(_instop) {
    case LDSB:
    case LDSH:
    case LDUB:
    case LDUH:
    case LD:
    case LDD:
      TraceStats.IC_load += 1;
      break;

    case STB:
    case STH:
    case ST:
    case STD:
      TraceStats.IC_store += 1;
      break;
      
    case LDSTUB:
    case SWAP:
      TraceStats.IC_others += 1;
      break;

    default:
      TraceStats.IC_others += 1;

    }
    break;

  default:
    TraceStats.IC_others += 1;

  }

}

/* funcoes para manipulacao de instrucoes de delay slot */

bool_t CMemoTableEntryT::BiccAtEnd()
{
  return(Delay == TRUE);
}

void CMemoTableEntryT::AddDelayInst(uword_t _daddr, uword_t _dinst)
{
  Delay = TRUE;
  DAddr = _daddr;
  DInst = _dinst;
}


/* funcao de reinicializacao da entrada */

void CMemoTableEntryT::Reset()
{
  int i;

  Id = 0;

  PC = 0;

  BrcSz = 0;

  for(i = 0; i < IC_NUM; i++)
    Ic[i].regtype = CTX_NONE;

  for(i = 0; i < OC_NUM; i++)
    Oc[i].regtype = CTX_NONE;

  nPC = 0;

  Delay = FALSE;
  DAddr = 0;
  DInst = 0;

  Tracesz = 0;

  Count = 0;

  /* instrucoes de acesso a memoria */
  LdStSz = 0;


  ////////////////////////////////////////////////////////////////
  //// contabilizacao estatistica das instrucoes que compoem o trace
 
  /* instrucoes de desvios */

  TraceStats.IC_call = 0;

  TraceStats.IC_bicc = 0;

  TraceStats.IC_jmpl = 0;

  /* chamadas ao sistema operacional */

  TraceStats.IC_ticc = 0;

  /* instrucoes de acesso a memoria */

  TraceStats.IC_load = 0;

  TraceStats.IC_store = 0;

  /* instrucoes logicas e aritimeticas */

  TraceStats.IC_arithmetic = 0;

  TraceStats.IC_logic = 0;

  /* instrucoes de multiplicacao */

  TraceStats.IC_mult = 0;

  /* instrucoes de save/restore */

  TraceStats.IC_save = 0;

  TraceStats.IC_restore = 0;

  /* instrucao sethi */

  TraceStats.IC_sethi = 0;

  /* outras instrucoes */

  TraceStats.IC_others = 0;

  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces

  TraceStats.TFN_traces = -1;

}


/* funcao padrao de apresentacao da entrada no terminal */

void CMemoTableEntryT::Display()
{
  int i, j;

  printf( "\n%4lu %4lu (%08lx) [sz=%ld] ",
	  Id,
	  Count,
	  PC,
	  Tracesz );

    for(j = 0; j < BRC_NUM; j++) {

      if(j < BrcSz) {
	if(Brc[j].btype == __JUMPL)
	  printf("J");
	else {
	  if(Brc[j].btaken == TRUE) 
	    printf("T");
	  else
	    printf("N");
	}

      }
      else {

	printf(".");

      }

    }

    printf(" { ic(");

    for(j = 0 ; j < IC_NUM; j++) {
      if(Ic[j].regtype == CTX_NONE) break;

      switch( Ic[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", Ic[j].regval);
	break;

      case CTX_Y:
	printf("y=%lx ", Ic[j].regval);
	break;

      case CTX_GPR:
	printf("r%ld=%lx ", Ic[j].regaddr, Ic[j].regval);

      }

    }

    printf(") oc(");

    for(j = 0 ; j < OC_NUM; j++) {
      if(Oc[j].regtype == CTX_NONE) break;

      switch( Oc[j].regtype ) {
      case CTX_PSR:
	printf("psr=%lx ", Oc[j].regval);
	break;

      case CTX_Y:
	printf("y=%lx ", Oc[j].regval);
	break;

      case CTX_GPR:
	printf("r%ld=%lx ", Oc[j].regaddr, Oc[j].regval);

      }

    }

    printf(")}, npc=%08lx) ", nPC);


    /* trace possui instrucao de acesso a memoria */

    for(i = 0; i < LdStSz; i++) {
	
      if(LdStInst[i].LdSt == IOM_LOAD) {
	printf("LD[%ld, maddr=%08lx", LdStInst[i].Type, LdStInst[i].MAddr);

	if( LdStInst[i].MValid ) {
	  printf("-V-");

	  if( LdStInst[i].Rdhi.IsReqAsTarget() )
	    printf( " hi(r%ld<- M[]=%lx)",
		    LdStInst[i].Rdhi.GetRegAddr(),
		    LdStInst[i].MValhi );
	
	  if( LdStInst[i].Rdlo.IsReqAsTarget() )
	    printf( " lo(r%ld<- M[]=%lx)",
		    LdStInst[i].Rdlo.GetRegAddr(),
		    LdStInst[i].MVallo );
	}
	else {
	  printf("-I-");

	  if( LdStInst[i].Rdhi.IsReqAsTarget() ) {
	    printf( " hi(r%ld<-M[])", LdStInst[i].Rdhi.GetRegAddr() );
	  }
	
	  if( LdStInst[i].Rdlo.IsReqAsTarget() )
	    printf( " lo(r%ld<-M[])", LdStInst[i].Rdlo.GetRegAddr() );	  
	}

	if( LdStInst[i].LdStChain )
	  printf(" LD<->ST ");

	printf("]");

      }
      else {
	printf("ST[%ld, maddr=%08lx, ", LdStInst[i].Type, LdStInst[i].MAddr);

	if( LdStInst[i].Rdhi.IsReqAsSource() )
	  printf(" hi(M[]<-%lx)", LdStInst[i].MValhi);

	if( LdStInst[i].Rdlo.IsReqAsSource() )
	  printf(" lo(M[]<-%lx)", LdStInst[i].MVallo);

	printf("]");
      }
      
    }

    if( BiccAtEnd() )
      printf(", DI");

}


/* funcoes de basicas controle */

// controle da fila RLU
CMemoTableEntryT* CMemoTableEntryT::Link(CMemoTableEntryT* p)
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

CMemoTableEntryT* CMemoTableEntryT::Unlink()
{
  if(prev != 0) prev->next = next;
  if(next != 0) next->prev = prev;
  next = prev = 0;
  return this;
}

// controle da tabela hash
CMemoTableEntryT* CMemoTableEntryT::HashLink(CMemoTableEntryT* p)
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

CMemoTableEntryT* CMemoTableEntryT::HashUnlink()
{
  if(hashprev != 0) hashprev->hashnext = hashnext;
  if(hashnext != 0) hashnext->hashprev = hashprev;
  hashnext = hashprev = 0;
  return this;
}


/* funcoes de sobreposicao de operadores */

CMemoTableEntryT & CMemoTableEntryT::operator=(CMemoTableEntryT & _it)
{
  int i;

  Id = _it.Id;

  PC = _it.PC;

  for(i = 0; i < _it.BrcSz; i++) {
    Brc[i].btype = _it.Brc[i].btype;
    Brc[i].btaken = _it.Brc[i].btaken;
  }

  BrcSz = _it.BrcSz;

  for(i = 0; i < IC_NUM; i++)
    Ic[i] = _it.Ic[i];

  for(i = 0; i < OC_NUM; i++)
    Oc[i] = _it.Oc[i];

  nPC = _it.nPC;

  Delay = _it.Delay;
  DAddr = _it.DAddr;
  DInst = _it.DInst;

  Tracesz = _it.Tracesz;

  Count = _it.Count;

  /* instrucoes de acesso a memoria */

  LdStSz = _it.LdStSz;
  for(i = 0; i < _it.LdStSz; i++) {
    LdStInst[i].LdSt = _it.LdStInst[i].LdSt;
    LdStInst[i].LdStChain = _it.LdStInst[i].LdStChain;
    LdStInst[i].Type = _it.LdStInst[i].Type;
    LdStInst[i].MAddr = _it.LdStInst[i].MAddr;
    LdStInst[i].MValid = _it.LdStInst[i].MValid;
    LdStInst[i].MValhi = _it.LdStInst[i].MValhi;
    LdStInst[i].MVallo = _it.LdStInst[i].MVallo;
    LdStInst[i].Rdhi = _it.LdStInst[i].Rdhi;
    LdStInst[i].Rdlo = _it.LdStInst[i].Rdlo;
  }

  ////////////////////////////////////////////////////////////////
  //// contabilizacao estatistica das instrucoes que compoem o trace
 
  /* instrucoes de desvios */

  TraceStats.IC_call = _it.TraceStats.IC_call;

  TraceStats.IC_bicc = _it.TraceStats.IC_bicc;

  TraceStats.IC_jmpl = _it.TraceStats.IC_jmpl;

  /* chamadas ao sistema operacional */

  TraceStats.IC_ticc = _it.TraceStats.IC_ticc;

  /* instrucoes de acesso a memoria */

  TraceStats.IC_load = _it.TraceStats.IC_load;

  TraceStats.IC_store = _it.TraceStats.IC_store;

  /* instrucoes logicas e aritimeticas */

  TraceStats.IC_arithmetic = _it.TraceStats.IC_arithmetic;

  TraceStats.IC_logic = _it.TraceStats.IC_logic;

  /* instrucoes de multiplicacao */

  TraceStats.IC_mult = _it.TraceStats.IC_mult;

  /* instrucoes de save/restore */

  TraceStats.IC_save = _it.TraceStats.IC_save;

  TraceStats.IC_restore = _it.TraceStats.IC_restore;

  /* instrucao sethi */

  TraceStats.IC_sethi = _it.TraceStats.IC_sethi;

  /* outras instrucoes */

  TraceStats.IC_others = _it.TraceStats.IC_others;

  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces

  TraceStats.TFN_traces = _it.TraceStats.TFN_traces;

  return *this;
}


////////////////////////////////////////////////////////////////////////
///// CMemoTableT

CMemoTableT::CMemoTableT(uword_t _nentries)
{
  uword_t i;
  if((pMemoTableT = new CMemoTableEntryT [(NEntries = _nentries)]) == 0)
    errmsg(__MEMOTABLET_H, ERR_CANTALLOCMEM, ""); 
  LRUList.p_mru = LRUList.p_lru = 0;
  for(i = 0; i < MEMOTABLET_NSLOTS; i++)
    HashTable[i].p_mru = HashTable[i].p_lru = 0;
  pCurrent = 0;
  Size = 0;
  NextId = 1;
}

CMemoTableT::~CMemoTableT()
{
  delete [] pMemoTableT;
}

CMemoTableEntryT* CMemoTableT::AddNew( uword_t _pc,
				       CContext* _ic,
				       CContext* _oc,
				       uword_t _npc )
{
  CMemoTableEntryT* p;
  uword_t nslot, nslotold;
  int i;

  nslot = (_pc / 4) % MEMOTABLET_NSLOTS;

  if(Size < NEntries) {
    p = & pMemoTableT [Size];
    Size += 1;
  }
  else {
    if(LRUList.p_lru == 0)
      errmsg(__MEMOTABLET_H, ERR_SIMARCHFAIL, "(AddNew - Instr)");

    p = LRUList.p_lru;

    LRUList.p_lru = p->prev;
    if(LRUList.p_lru == 0)
      LRUList.p_mru = 0;
    p->Unlink();

    nslotold = (p->PC / 4) % MEMOTABLET_NSLOTS;

    if(p->hashprev == 0)
      HashTable[nslotold].p_mru = p->hashnext;
    if(p->hashnext == 0)
      HashTable[nslotold].p_lru = p->hashprev;
    p->HashUnlink();
  }

  p->Id = NextId;
  NextId += 1;


  p->PC = _pc;

  for(i = 0; i < IC_NUM; i++)
    p->Ic[i] = _ic[i];

  for(i = 0; i < OC_NUM; i++)
    p->Oc[i] = _oc[i];

  p->nPC = _npc;

  p->Delay = FALSE;
  p->DAddr = 0;
  p->DInst = 0;

  p->Tracesz = 0;

  p->Count = 0;

  /* instrucoes de acesso a memoria */

  p->LdStSz = 0;

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

CMemoTableEntryT* CMemoTableT::AddNew(CMemoTableEntryT & _it)
{
  CMemoTableEntryT* p;
  uword_t nslot, nslotold;
  
  nslot = (_it.PC / 4) % MEMOTABLET_NSLOTS;

  if(Size < NEntries) {
    p = & pMemoTableT [Size];
    Size += 1;
  }
  else {
    if(LRUList.p_lru == 0)
      errmsg(__MEMOTABLET_H, ERR_SIMARCHFAIL, "(AddNew - MemoTableEntry)");

    p = LRUList.p_lru;

    LRUList.p_lru = p->prev;
    if(LRUList.p_lru == 0)
      LRUList.p_mru = 0;
    p->Unlink();

    nslotold = (p->PC / 4) % MEMOTABLET_NSLOTS;

    if(p->hashprev == 0)
      HashTable[nslotold].p_mru = p->hashnext;
    if(p->hashnext == 0)
      HashTable[nslotold].p_lru = p->hashprev;
    p->HashUnlink();
  }

  (*p) = _it;

  p->Id = NextId;
  NextId += 1;

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

bool_t CMemoTableT::AtEnd()
{
  return (LRUList.p_lru == pCurrent);
}

CMemoTableEntryT* CMemoTableT::GetCurrent()
{
  return pCurrent;
}

CMemoTableEntryT* CMemoTableT::GoTop()
{
  return (pCurrent = LRUList.p_mru);
}

CMemoTableEntryT* CMemoTableT::MoveNext()
{
  if((pCurrent != 0) && (pCurrent->next != 0))
    return (pCurrent = pCurrent->next);
  return 0;
}

CMemoTableEntryT* CMemoTableT::FindFirst(uword_t _pc)
{
  uword_t nslot;
  nslot = (_pc / 4) % MEMOTABLET_NSLOTS;

  pCurrent = HashTable[nslot].p_mru;
  while(pCurrent != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
    pCurrent = pCurrent->hashnext;
  }
  return 0;
}

CMemoTableEntryT* CMemoTableT::FindNext(uword_t _pc)
{
  if(pCurrent == 0)
    return 0;

  while((pCurrent = pCurrent->hashnext) != 0) {
    if(pCurrent->PC == _pc)
      return pCurrent;
  }
  return 0;
}

CMemoTableEntryT* CMemoTableT::Update(CMemoTableEntryT* p)
{
  uword_t nslot;
  nslot = (p->PC / 4) % MEMOTABLET_NSLOTS;

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

void CMemoTableT::Invalidate(uword_t _ptr, uword_t _len)
{
  CMemoTableEntryT * p;
  int i;

  p = LRUList.p_mru;
  while(p != NULL) {

    for(i = 0; i < p->LdStSz; i++) {

      if( ((p->LdStInst[i].LdSt & IOM_LOAD) == IOM_LOAD) && !p->LdStInst[i].LdStChain ) {
	// verifica sobreposicao se instrucao de load nao for servida por store do mesmo trace
	// sobreposicao em memoria ocorre quando (Mmax >= Pmin) e (Mmin <= Pmax)

	switch( (p->LdStInst[i]).Type ) {
	case RD_BYTE:
	case RD_UBYTE:
	  if( ((p->LdStInst[i].MAddr + sizeof(byte_t) - 1) >= _ptr) &&
	      (p->LdStInst[i].MAddr <= (_ptr + _len)) )
	    p->LdStInst[i].MValid = FALSE;
	  break;
	  
	case RD_HWORD:
	case RD_UHWORD:
	  if( ((p->LdStInst[i].MAddr + sizeof(hword_t) - 1) >= _ptr) &&
	      (p->LdStInst[i].MAddr <= (_ptr + _len)) )
	    p->LdStInst[i].MValid = FALSE;
	  break;
	  
	case RD_WORD:
	  if( ((p->LdStInst[i].MAddr + sizeof(word_t) - 1) >= _ptr) &&
	      (p->LdStInst[i].MAddr <= (_ptr + _len)) )
	    p->LdStInst[i].MValid = FALSE;
	  break;
	  
	case RD_DWORD:
	  if( ((p->LdStInst[i].MAddr + sizeof(dword_t) - 1) >= _ptr) &&
	      (p->LdStInst[i].MAddr <= (_ptr + _len)) )
	    p->LdStInst[i].MValid = FALSE;
	  break;
	  
	} 

      }

    }

    p = p->next;
  }  

}

void CMemoTableT::UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len)
{
  CMemoTableEntryT * p;
  int i;

  dword_t res;
  uword_t pos, n;

  p = LRUList.p_mru;
  while(p != NULL) {

    for(i = 0; i < p->LdStSz; i++) {

      if( ((p->LdStInst[i].LdSt & IOM_LOAD) == IOM_LOAD) && !p->LdStInst[i].LdStChain ) {
	// verifica sobreposicao se instrucao de load nao for servida por store do mesmo trace
	// sobreposicao em memoria ocorre quando (Mmax >= Pmin) e (Mmin <= Pmax)

	switch( (p->LdStInst[i]).Type ) {
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

	if( ((p->LdStInst[i]).MAddr <= (_maddr + _len - 1)) && 
	    (((p->LdStInst[i]).MAddr + n - 1) >= _maddr) )
	{
	  res = 0;
	  for(uword_t k = 0; k < n; k++)
	  {
	    if( (((p->LdStInst[i]).MAddr + k) >= _maddr) &&
		(((p->LdStInst[i]).MAddr + k) <= (_maddr + _len - 1)) )
	    {
	      pos = (((p->LdStInst[i]).MAddr + k) - _maddr);
	      res = (res << 32) | _ptr[pos];
	    }
	    else
	    {
	      res = (res << 32);
	    }
	  }
	}

	(p->LdStInst[i]).MValhi = (uword_t) ((res >> 32) & 0x00000000FFFFFFFFL);
	if((p->LdStInst[i]).Type == RD_DWORD)
	  (p->LdStInst[i]).MVallo = (uword_t) (res & 0x00000000FFFFFFFFL);

      }

    }

    p = p->next;
  }  

}
