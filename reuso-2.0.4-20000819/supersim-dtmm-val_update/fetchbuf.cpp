
/*
 * fetchbuf.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/3/1999
 */

#include<stdio.h>
#include"all.h"

////////////////////////////////////////////////////////////////////
///// CFetchBufItem

CFetchBufItem::CFetchBufItem()
{
  /* nothing to do */
}

void CFetchBufItem::SetItem( uword_t _seq,
			     uword_t _addr,
			     uword_t _inst,
			     bool_t _bdep,
			     uword_t _bseq,
			     uword_t _blevel,
			     bool_t _dslot )
{
  Seq = _seq;

  Addr = _addr;

  Inst = _inst;

  DSlot = _dslot;

  BDep = _bdep;

  WBDep = _bdep;

  BSeq = _bseq;

  BLevel = _blevel;

  NewPC = 0;

  BInst = FALSE;

  BPred = NTAKEN;

  BAddr = 0;

  BType = __NIL;

  BResult = NTAKEN;

  BDec();

  InstDependentRedundant = FALSE;

  ProbRedundant = FALSE;

  InstRedundant = FALSE;

  TraceRedundant = FALSE;

  InvalidInst = FALSE;

}

void CFetchBufItem::SetNewPC(uword_t _newpc)
{
  NewPC = _newpc;
}

int CFetchBufItem::BDec()
{
  if( T_BA(Inst) ) {
    BInst = TRUE;
    BPred = TAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L) << 2);
    return (BType = __BA);
  }
  else if ( T_BA_ANNUL(Inst) ) {
    BInst = TRUE;
    BPred = TAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L) << 2);
    return (BType = __BA_ANNUL);
  }
  else if ( T_BN(Inst) ) {
    BInst = TRUE;
    BPred = NTAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L) << 2);
    return (BType = __BN);
  }
  else if ( T_BN_ANNUL(Inst) ) {
    BInst = TRUE;
    BPred = NTAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L) << 2);
    return (BType = __BN_ANNUL);
  }
  else if ( T_BICC(Inst) ) {
    BInst = TRUE;
    BPred = NTAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L) << 2);
    return (BType = __BICC);
  }
  else if ( T_BICC_ANNUL(Inst) ) {
    BInst = TRUE;
    BPred = NTAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L) << 2);
    return (BType = __BICC_ANNUL);
  }
  else if ( T_CALL(Inst) ) {
    BInst = TRUE;
    BPred = TAKEN;
    BAddr = Addr + (SIGN_EXTEND((Inst & 0x3FFFFFFFL), 0xE0000000L) << 2);
#if(DEBUG == 0)
printf("\nFETCHBUF::BDEC(addr=%08lx,inst=%08lx (call),baddr=%08lx)", Addr, Inst, BAddr);
#endif
    return (BType = __CALL);
  }
  else if ( T_JUMPL(Inst) ) {
    BInst = TRUE;
    BPred = TAKEN;
    BAddr = Addr + 4;
    return (BType = __JUMPL);
  }
  return RTERR;
}

bool_t CFetchBufItem::IsInvalid()
{
  return InvalidInst;
}

void CFetchBufItem::SetInvalidBit()
{
  InvalidInst = TRUE;
}

int CFetchBufItem::IsBranchInst()
{
  return BInst;
}

int CFetchBufItem::IsPredTaken()
{
  return (BInst && (BPred == TAKEN));
}

void CFetchBufItem::SetBranchPred(bool_t _bpred, uword_t _baddr)
{
  BPred = _bpred;
  BAddr = _baddr;
}

int CFetchBufItem::GetBranchType()
{
  return BType;
}

uword_t CFetchBufItem::DecSeqNum()
{
  return (Seq -= 1);
}

uword_t CFetchBufItem::GetSeqNum()
{
  return Seq;
}

uword_t CFetchBufItem::DecBranchLevel()
{
  return (BLevel -= 1);
}

uword_t CFetchBufItem::GetBranchLevel()
{
  return BLevel;
}

uword_t CFetchBufItem::GetTargetAddr()
{
  return BAddr;
}

int CFetchBufItem::IsDelaySlotInst()
{
  return DSlot;
}

int CFetchBufItem::IsBranchDep()
{
  return BDep;
}

int CFetchBufItem::WasBranchDep()
{
  return WBDep;
}

void CFetchBufItem::ClearBranchDep()
{
  BDep = FALSE;
  BSeq = 0;
}

uword_t CFetchBufItem::DecBranchDepNum()
{
  return (BSeq -= 1);
}

uword_t CFetchBufItem::GetBranchDepNum()
{
  return BSeq;
}

bool_t CFetchBufItem::IsProbablyRedundant()
{
  return ProbRedundant;
}

void CFetchBufItem::SetProbablyRedundant()
{
  ProbRedundant = TRUE;
}

bool_t CFetchBufItem::IsDependentRedundant()
{
  return InstDependentRedundant;
}

void CFetchBufItem::SetDependentRedundant()
{
  InstDependentRedundant = TRUE;
}

bool_t CFetchBufItem::IsRedundant()
{
  return InstRedundant;
}

void CFetchBufItem::SetRedundant()
{
  InstRedundant = TRUE;
}

bool_t CFetchBufItem::IsTraceRedundant()
{
  return TraceRedundant;
}

void CFetchBufItem::SetTraceRedundant()
{
  TraceRedundant = TRUE;
}

CFetchBufItem& CFetchBufItem::operator=(CFetchBufItem& _fb)
{
  Seq = _fb.Seq;

  Addr = _fb.Addr;

  Inst = _fb.Inst;

  DSlot = _fb.DSlot;

  BDep = _fb.BDep;

  WBDep = _fb.WBDep;

  BSeq = _fb.BSeq;

  BLevel = _fb.BLevel;

  NewPC = _fb.NewPC;

  BInst = _fb.BInst;

  BPred = _fb.BPred;

  BAddr = _fb.BAddr;

  BType = _fb.BType;

  InstDependentRedundant = _fb.InstDependentRedundant;

  TraceRedundant = _fb.TraceRedundant;

  ProbRedundant = _fb.ProbRedundant;

  InstRedundant = _fb.InstRedundant;

  InvalidInst = _fb.InvalidInst;

  return *this;
}

////////////////////////////////////////////////////////////////////
///// CFetchBuf

void CFetchBuf::Init(CPredict* _predict)
{
  Predict = _predict;
}

CFetchBuf::CFetchBuf(uword_t _nentries)
  : CQueue(__FETCHBUF_H, _nentries)
{
  if((pFetchBufArray = new CFetchBufItem [ _nentries ]) == 0)
    errmsg(__FETCHBUF_H, ERR_CANTALLOCMEM, "");
  CQueue::Init((ubyte_t*) pFetchBufArray, sizeof(CFetchBufItem));

  SeqNum = 0;
  BLevel = 0;
  LastBLevel = 0;
}

CFetchBuf::~CFetchBuf()
{
  delete [] pFetchBufArray;
}

int CFetchBuf::IsFull()
{
  return ((GetNumEntries() - GetQueueSize()) <= 1);
}

uword_t CFetchBuf::GetCurrSeqNum()
{
  return SeqNum;
}

void CFetchBuf::FreeSeqNum(uword_t _seq)
{
  CFetchBufItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetSeqNum() > _seq)
      p->DecSeqNum();
    if(( p->IsBranchDep() ) && (p->GetBranchDepNum() > _seq))
      p->DecBranchDepNum();
  }
  SeqNum -= 1;
}

void CFetchBuf::FreeBranchLevel(uword_t _blevel)
{
  CFetchBufItem* p;
  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);
    if(p->GetBranchLevel() > _blevel)
      p->DecBranchLevel();
  }
  BLevel -= 1;
}

CFetchBufItem* CFetchBuf::InsertItem( uword_t _addr,
				      uword_t _inst,
				      bool_t _dslot,
				      bool_t _oblevel,
				      bool_t _bdep,
				      uword_t _bseq)
{
  CFetchBufItem* p = (CFetchBufItem*) (CQueue::InsertItem()->pData);

  bool_t bpred;
  uword_t bdest;

  if(_oblevel == TRUE)
    p->SetItem(SeqNum, _addr, _inst, _bdep, _bseq, BLevel - 1, _dslot);
  else
    p->SetItem(SeqNum, _addr, _inst, _bdep, _bseq, BLevel, _dslot);

  SeqNum += 1;

  if( p->IsBranchInst() ) {
    if( ((p->GetBranchType() == __BICC) ||
	 (p->GetBranchType() == __BICC_ANNUL) ||
	 (p->GetBranchType() == __JUMPL)) &&
	(Predict->Evaluate(_addr, & bpred, & bdest) == HIT) )
      p->SetBranchPred(bpred, bdest);
    BLevel += 1;
  }

  return p;
}

CFetchBufItem* CFetchBuf::GetFirstItem()
{
  return ((CFetchBufItem*) (CQueue::GetFirstItem()->pData));
}

CFetchBufItem* CFetchBuf::GetNthItem(int nth)
{
  return ((CFetchBufItem*) (CQueue::GetNthItem(nth)->pData));
}

CFetchBufItem* CFetchBuf::GetLastItem()
{
  return ((CFetchBufItem*) (CQueue::GetLastItem()->pData));
}

void CFetchBuf::RemoveFirstItem()
{
  CQueue::RemoveFirstItem();
}

void CFetchBuf::Swap(uword_t _i1, uword_t _i2)
{
  CFetchBufItem* p1 = (CFetchBufItem*) (CQueue::GetNthItem(_i1)->pData);
  CFetchBufItem* p2 = (CFetchBufItem*) (CQueue::GetNthItem(_i2)->pData);

  uword_t seq;
  uword_t npc;

  seq = p1->Seq;
  p1->Seq = p2->Seq;
  p2->Seq = seq;

  npc = p1->NewPC;
  p1->NewPC = p2->NewPC;
  p2->NewPC = npc;

  CQueue::Swap(_i1, _i2);
}

void CFetchBuf::SetLastBranchLevel(uword_t _lblevel)
{
  if(_lblevel > LastBLevel)
    LastBLevel = _lblevel;
}

uword_t CFetchBuf::GetLastBranchLevel()
{
  return LastBLevel;
}

void CFetchBuf::Reset()
{
  CQueue::Flush();
  SeqNum = 0;
  BLevel = 0;
  LastBLevel = 0;
}

void CFetchBuf::Flush()
{
  CQueue::Flush();
  BLevel = LastBLevel;
}

void CFetchBuf::Flush(uword_t _seq, uword_t _blevel)
{
#if(DEBUG == 0)
  printf("\nFETCHBUF::FLUSH");
  printf("\n\tSEQNUM(%lu)", _seq % MAX_SEQNUM);
  printf("\n\tBLEVEL(%lu)", _blevel);
#endif
  CQueue::Flush();
  BLevel = LastBLevel;
}

void CFetchBuf::Show()
{
  CFetchBufItem *p;
  CAsm it;

  str_t s;

  printf("\nfetch buffer");
  printf("\n============\n");

  if( IsEmpty() ) {
    printf("\nFetch buffer is empty\n");
    return;
  }

  if( IsFull() )
    printf("\nFetch buffer is full\n");

  for(uword_t i = 0; i < GetQueueSize(); i++) {
    p = GetNthItem(i);

    it.Set(p->Addr, p->Inst);
    it.Get(s);

    if( p->IsBranchInst() ) {
      if( p->IsPredTaken() )
	printf( "\n%c i(%02lu) %2lu %2lu <BT> %08lx %08lx %s TARGET=%08lx",
		(p->IsInvalid() ? 'I' : ' '),
		i,
		p->Seq % MAX_SEQNUM,
		p->BLevel,
		p->Addr,
		p->Inst,
		s,
		p->BAddr );
      else
	printf( "\n%c i(%02lu) %2lu %2lu <BN> %08lx %08lx %s TARGET=%08lx",
		(p->IsInvalid() ? 'I' : ' '),
		i,
		p->Seq % MAX_SEQNUM,
		p->BLevel,
		p->Addr,
		p->Inst,
		s,
		p->BAddr );
    }
    else {
      printf(  "\n%c i(%02lu) %2lu %2lu <  > %08lx %08lx %s",
	       (p->IsInvalid() ? 'I' : ' '),	       
	       i,
	       p->Seq % MAX_SEQNUM,
	       p->BLevel,
	       p->Addr,
	       p->Inst,
	       s );
    }

  }

  printf("\n");
}
