
/*
 * iu.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/17/99
 */

#include<stdio.h>
#include"all.h"

////////////////////////////////////////////////////////////
///// CIu

void CIu::Init(CRs* _rsiu)
{
  RsIu = _rsiu;
}

CIu::CIu(uword_t _nentries)
{
  if((ResBuf = new CResBuf [(NEntries = _nentries)]) == 0)
    errmsg(__IU_H, ERR_CANTALLOCMEM, "");
}

CIu::~CIu()
{
  delete [] ResBuf;
}

uword_t CIu::GetNumUnits()
{
  return NEntries;
}

CResBuf* CIu::GetNthResBuf(uword_t _idx)
{
  if(_idx < NEntries)
    return & ResBuf[_idx];
  return 0;
}

void CIu::Step(CResBuf* _rb)
{
  CRsItem* rsit = 0;
  uword_t i;
  
  uword_t mask;

  _rb->Reset();

  if( RsIu->IsEmpty() || ((rsit = RsIu->GetReadyItem()) == 0) ) return;

#if(DEBUG == 0)
  printf( "\n\tIU::STEP(seq=%lu, addr=%lx, inst=%lx)",
	  rsit->GetSeqNum() % MAX_SEQNUM,
	  rsit->Addr,
	  rsit->Inst );
#endif

  rsit->SetBusy();

  switch(rsit->GetInstOpcode()) {
  case ADD:
  case ADDCC:
  case ADDX:
  case ADDXCC:
    Operand1 = (rsit->Rs1).GetValue();
    if((rsit->fi).i == 0)
      Operand2 = (rsit->Rs2).GetValue();
    else
      Operand2 = (rsit->fi).simm13;
      
    if( (rsit->GetInstOpcode() == ADD) || (rsit->GetInstOpcode() == ADDCC) )
      Result1 = Operand1 + Operand2;
    else {
      Result1 = Operand1 + Operand2;
      if( ICC_C((rsit->Psr).GetValue()) )
	Result1 = Result1 + 1;
    }

    if( (rsit->GetInstOpcode() == ADDCC) || (rsit->GetInstOpcode() == ADDXCC) ) {
      Icc.n = WSIGN(Result1);
      Icc.z = (Result1 == 0);
      Icc.v = ( (WSIGN(Operand1) && WSIGN(Operand2) && !WSIGN(Result1)) ||
		(!WSIGN(Operand1) && !WSIGN(Operand2) && WSIGN(Result1)) );
      Icc.c = ( (WSIGN(Operand1) && WSIGN(Operand2)) ||
		(!WSIGN(Result1) && (WSIGN(Operand1) || WSIGN(Operand2))) );
    }

    if((rsit->fi).rd == 0)
      Result1 = 0;
    break;

  case SUB:
  case SUBCC:
  case SUBX:
  case SUBXCC:
    Operand1 = (rsit->Rs1).GetValue();
    if((rsit->fi).i == 0)
      Operand2 = (rsit->Rs2).GetValue();
    else
      Operand2 = (rsit->fi).simm13;
      
    if( (rsit->GetInstOpcode() == SUB) || (rsit->GetInstOpcode() == SUBCC) )
      Result1 = Operand1 - Operand2;
    else {
      Result1 = Operand1 - Operand2;
      if( ICC_C((rsit->Psr).GetValue()) )
	Result1 = Result1 - 1;
    }

    if( (rsit->GetInstOpcode() == SUBCC) || (rsit->GetInstOpcode() == SUBXCC) ) {
      Icc.n = WSIGN(Result1);
      Icc.z = (Result1 == 0);
      Icc.v = ( (WSIGN(Operand1) && !WSIGN(Operand2) && !WSIGN(Result1)) ||
		(!WSIGN(Operand1) && WSIGN(Operand2) && WSIGN(Result1)) );
      Icc.c = ( (!WSIGN(Operand1) && WSIGN(Operand2)) ||
		(WSIGN(Result1) && (!WSIGN(Operand1) || WSIGN(Operand2))) );
    }

    if((rsit->fi).rd == 0)
      Result1 = 0;

    break;

  case MULSCC:

    Operand1 = (rsit->Rs1).GetValue();
    Operand1 = (Operand1 >> 1) & 0x7FFFFFFFL;			
    if( (ICC_N((rsit->Psr).GetValue()) ^ ICC_V((rsit->Psr).GetValue())) == 1 )
      Operand1 = Operand1 | 0x80000000L;

    if( ((rsit->Y).GetValue() & 0x01L) == 0 )
      Operand2 = 0;
    else if( (rsit->fi).i == 0 )
      Operand2 = (rsit->Rs2).GetValue();
    else if( ((rsit->fi).simm13 & 0x1000) == 0 )		
      Operand2 = (0x00000FFFL & (rsit->fi).simm13);
    else
      Operand2 = (0xFFFFF000L | (rsit->fi).simm13);

    Result1 = Operand1 + Operand2;

    Y = ((rsit->Y).GetValue() >> 1) & 0x7fffffffL;
    if( ((rsit->Rs1).GetValue() & 0x01L) == 1 )
      Y = Y | 0x80000000L;

    if( WSIGN(Result1) == 1 )
      Icc.n = 1;
    else
      Icc.n = 0;

    if( Result1 == 0 )
      Icc.z = 1;
    else
      Icc.z = 0;

    if( (  WSIGN(Operand1) &  WSIGN(Operand2) & ~WSIGN(Result1) ) |
	( ~WSIGN(Operand1) & ~WSIGN(Operand2) & WSIGN(Result1)) )
      Icc.v = 1;
    else
      Icc.v = 0;

    if( ( WSIGN(Operand1) & WSIGN(Operand2) ) |
	( ~WSIGN(Result1) & ( WSIGN(Operand1) | WSIGN(Operand2) ) ) )
      Icc.c = 0;
    else
      Icc.c = 1;

    if((rsit->fi).rd == 0)
      Result1 = 0;

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
    Operand1 = (rsit->Rs1).GetValue();
    if((rsit->fi).i == 0)
      Operand2 = (rsit->Rs2).GetValue();
    else
      Operand2 = (rsit->fi).simm13;
    
    if( (rsit->GetInstOpcode() == AND) || (rsit->GetInstOpcode() == ANDCC) )
      Result1 = Operand1 & Operand2;
    if( (rsit->GetInstOpcode() == ANDN) || (rsit->GetInstOpcode() == ANDNCC) )
      Result1 = Operand1 & ~Operand2;
    if( (rsit->GetInstOpcode() == OR) || (rsit->GetInstOpcode() == ORCC) )
      Result1 = Operand1 | Operand2;
    if( (rsit->GetInstOpcode() == ORN) || (rsit->GetInstOpcode() == ORNCC) )
      Result1 = Operand1 | ~Operand2;
    if( (rsit->GetInstOpcode() == XOR) || (rsit->GetInstOpcode() == XORCC) )
      Result1 = Operand1 ^ Operand2;
    if( (rsit->GetInstOpcode() == XNOR) || (rsit->GetInstOpcode() == XNORCC) )
      Result1 = Operand1 ^ ~Operand2;

    if( (rsit->GetInstOpcode() == ANDCC) || (rsit->GetInstOpcode() == ANDNCC) ||
	(rsit->GetInstOpcode() == ORCC) || (rsit->GetInstOpcode() == ORNCC) ||
	(rsit->GetInstOpcode() == XORCC) || (rsit->GetInstOpcode() == XNORCC) ) {
      Icc.n = WSIGN(Result1);
      Icc.z = (Result1 == 0);
      Icc.v = 0;
      Icc.c = 0;
    }

    if((rsit->fi).rd == 0)
      Result1 = 0;
    break;

  case SLL:
  case SRL:
  case SRA:
    Operand1 = (rsit->Rs1).GetValue();
    if((rsit->fi).i == 0)
      Shift_count = (rsit->Rs2).GetValue() & 0x1FL;
    else
      Shift_count = (rsit->fi).simm13 & 0x1FL;
      
    if(rsit->GetInstOpcode() == SLL) {
      Result1 = Operand1 << Shift_count;
    }
    else if(rsit->GetInstOpcode() == SRL) {
      Result1 = Operand1 >> Shift_count;
    }
    else if(rsit->GetInstOpcode() == SRA) {
      Result1 = Operand1 >> Shift_count;
      if ((Operand1 & 0x80000000L) != 0) {
	mask = 0;
	for (i = 0; i < Shift_count; i++)
	  mask = (mask >> 1) | 0x80000000L;
	Result1 = Result1 | mask;
      }
    }

    if((rsit->fi).rd == 0)
      Result1 = 0;
    break;

  case SETHI:
    if((rsit->fi).rd != 0)
      Result1 = (rsit->fi).imm22 << 10;
    else
      Result1 = 0;
    break;

  case SAVE:
  case RESTORE:
    Operand1 = (rsit->Rs1).GetValue();
    if((rsit->fi).i == 0)
      Operand2 = (rsit->Rs2).GetValue();
    else
      Operand2 = (rsit->fi).simm13;
    
    Result1 = Operand1 + Operand2;
    
    if((rsit->fi).rd == 0)
      Result1 = 0;
    break;

  case RDPSR:
  case RDY:
    if(rsit->GetInstOpcode() == RDPSR)
      Result1 = (rsit->Psr).GetValue();
    else
      Result1 = (rsit->Y).GetValue();
    
    if((rsit->fi).rd == 0)
      Result1 = 0;
    break;

  case WRPSR:
    Psr = (rsit->Rs1).GetValue();
    break;

  case WRY:
    Y = (rsit->Rs1).GetValue();

  }

  if(rsit->GetInstOpcode() != WRPSR)
    Psr = ICC(Icc.n, Icc.z, Icc.v, Icc.c);

  _rb->Set( rsit,
	    0,
	    Result1,
	    0,
	    Y,
	    Psr );
}

void CIu::Execute()
{
  for(uword_t i = 0; i < NEntries; i++) {
#if(DEBUG == 0)
    printf( "\nIU::EXECUTE(%lu)", i);
#endif
    Step(& ResBuf[i]);
  }
}

void CIu::Flush(uword_t _blevel)
{
  for(uword_t i = 0; i < NEntries; i++) {
    if( ResBuf[i].IsValid() && ((ResBuf[i].Rsit)->GetBranchLevel() > _blevel) )
      ResBuf[i].Reset();
  }
}

void CIu::Reset()
{
  for(uword_t i = 0; i < NEntries; i++)
    ResBuf[i].Reset();
}

void CIu::Show()
{
  CResBuf* rb;

  CRsItem* p;
  CAsm it;

  str_t s1;

  printf("\nIU");
  printf("\n==\n");

  for(uword_t i = 0; i < GetNumUnits(); i++) {
    if( !(rb = & ResBuf[i])->IsValid() ) {
      printf("\n%2lu) *** NOT_VALID ***", i);
      continue;
    }

    p = rb->Rsit;

    it.Set(p->Addr, p->Inst);
    it.Get(s1, 22);

    printf("\n%2lu) ", i);

    printf(  "%2lu %2lu %08lx %08lx %s ", p->Seq % MAX_SEQNUM, p->BLevel, p->Addr, p->Inst, s1);

    printf("[ ");

    if( (p->Psr).IsReqAsTarget() ) {
      if( (p->Psr).IsReady() )
	printf("psr=%lu ", (p->Psr).GetValue());
      else
	printf("(psr=?) ");
    }

    if( (p->Y).IsReqAsTarget() ) {
      if( (p->Y).IsReady() )
	printf("y=%lu ", (p->Y).GetValue());
      else
	printf("(y=?) ");
    }

    if( (p->Rd_lo).IsReqAsTarget() ) {
      if( (p->Rd_lo).IsReady() )
	printf("rd%lu=%lu ", (p->Rd_lo).GetRegAddr(), (p->Rd_lo).GetValue());
      else
	printf("(rd%lu) ", (p->Rd_lo).GetRegAddr());
    }
    
    if( (p->Rd_hi).IsReqAsTarget() ) {
      if( (p->Rd_hi).IsReady() )
	printf("rd%lu=%lu ", (p->Rd_hi).GetRegAddr(), (p->Rd_hi).GetValue());
      else
	printf("(rd%lu) ", (p->Rd_hi).GetRegAddr());
    }

    printf("]");

  }
  printf("\n");
}
