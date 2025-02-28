
/*
 * btb.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#include<stdio.h>
#include"all.h"


///////////////////////////////////////////////////////////////
///// CBtbEntry

CBtbEntry::CBtbEntry()
{
  Valid = FALSE;
}

int CBtbEntry::IsValid()
{
  return Valid;
}

void CBtbEntry::SetValid()
{
  Valid = TRUE;
}

void CBtbEntry::SetNotValid()
{
  Valid = FALSE;
}

uword_t CBtbEntry::GetLifeTime()
{
  return Count;
}

void CBtbEntry::IncLifeTime()
{
  Count += 1;
}

uword_t CBtbEntry::GetTagValue()
{
  return Tag;
}

uword_t CBtbEntry::GetSatCount()
{
  return SatCount;
}

uword_t CBtbEntry::GetTargetAddr()
{
  return Dest;
}

void CBtbEntry::SetItem(uword_t _tag, bool_t _bpred, uword_t _baddr, bool_t _rstSat)
{
  Valid = TRUE;

  Tag = _tag;

  Count = 0;

  if(_rstSat == TRUE) {
    SatCount = BTB_SATINI(_bpred);
    Dest = _baddr;
    return;
  }

  ///////////////////////////////////////////////
  // MODIFICADO EM 25/05/2020 - POR LUIZ MARCIO FARIA DE AQUINO VIANA
  //if(_bpred == TAKEN)
  //  if(SatCount < BTB_SATMAX) SatCount += 1;
  //else
  //  if(SatCount > 0) SatCount -= 1;
  if(_bpred == TAKEN)
  {
    if(SatCount < BTB_SATMAX) SatCount += 1;
  }
  else
  {
    if(SatCount > 0) SatCount -= 1;
  }
  // FIM MODIFICACAO

  if( ((SatCount >= BTB_SATLIM) && (_bpred == TAKEN)) ||
      ((SatCount < BTB_SATLIM) && (_bpred == NTAKEN)) )
    Dest = _baddr;
}

///////////////////////////////////////////////////////////////
///// CBtb

CBtb::CBtb(uword_t _nsets, uword_t _nentries)
{
  if((Btb = new CBtbEntry*[(NSets = _nsets)]) == 0)
    errmsg(__BTB_H, ERR_CANTALLOCMEM, "");

  for(uword_t i = 0; i < NSets; i++)
    if((Btb[i] = new CBtbEntry [(NEntries = _nentries)]) == 0)
      errmsg(__BTB_H, ERR_CANTALLOCMEM, "");
}

CBtb::~CBtb()
{
  for(uword_t i = 0; i < NEntries; i++)
    delete [] Btb[i];
  delete [] Btb;
}

void CBtb::AddEntry(uword_t _slot, uword_t _tag, bool_t* _bpred, uword_t* _baddr)
{
  uword_t centry;
  uword_t i;

  centry = 0;
  for(i = 0; i < NEntries; i++) {
    if( !Btb[_slot][i].IsValid() ) {
      centry = i;
      break;
    }
    if(Btb[_slot][i].GetLifeTime() >= Btb[_slot][centry].GetLifeTime()) centry = i;
    Btb[_slot][i].IncLifeTime();
  }

  Btb[_slot][centry].SetItem(_tag, (*_bpred), (*_baddr), TRUE);
}

void CBtb::UpdateEntry(uword_t _slot, uword_t _entry, bool_t* _bpred, uword_t* _baddr)
{
  uword_t lifeTime;
  uword_t i;

  lifeTime = Btb[_slot][_entry].GetLifeTime();

  for(i = 0; i < NEntries; i++) {
    if( !Btb[_slot][i].IsValid() ) break;

    if(Btb[_slot][i].GetLifeTime() < lifeTime)
      Btb[_slot][i].IncLifeTime();
  }

  Btb[_slot][_entry].SetItem(Btb[_slot][_entry].GetTagValue(), (*_bpred), (*_baddr));
}

int CBtb::Evaluate(uword_t _addr, bool_t* _bpred, uword_t* _baddr)
{
  uword_t slot = (_addr / 4) % NSets;
  uword_t tag = (_addr / 4) / NSets;

  for(uword_t i = 0; i < NEntries; i++) {
    if( !Btb[slot][i].IsValid() ) return MISS;

    if(Btb[slot][i].GetTagValue() == tag) {
      (*_bpred) = (Btb[slot][i].GetSatCount() >= BTB_SATLIM) ? TAKEN : NTAKEN;
      (*_baddr) = Btb[slot][i].GetTargetAddr();
      return HIT;
    }
  }
  return MISS;
}

int CBtb::Update(uword_t _addr, bool_t* _bpred, uword_t* _baddr)
{
  uword_t slot = (_addr / 4) % NSets;
  uword_t tag = (_addr / 4) / NSets;

  for(uword_t i = 0; i < NEntries; i++) {
    if( !Btb[slot][i].IsValid() ) break;

    if(Btb[slot][i].GetTagValue() == tag) {
#if(DEBUG == 0)
      printf("\nBTB::UPDATEENTRY(%lu, %08lx)", slot, tag);
#endif
      UpdateEntry(slot, i, _bpred, _baddr);
      return HIT;
    }
  }

#if(DEBUG == 0)
  printf("\nBTB::ADDENTRY(%lu, %08lx)", slot, tag);
#endif
  AddEntry(slot, tag, _bpred, _baddr);
  return MISS;
}

void CBtb::Reset()
{
  uword_t i, j;

  for(i = 0; i < NSets; i++)
    for(j = 0; j < NEntries; j++)
      Btb[i][j].SetNotValid();
}

void CBtb::Show()
{
  CBtbEntry* p;

  uword_t i, j;
  bool_t flg;

  printf("\nBTB  (obs: <address,tag,lifetime,satcount,target>");
  printf("\n=================================================\n");

  flg = FALSE;
  for(i = 0; i < NSets; i++) {
    if( (p = Btb[i])->IsValid() ) {
      printf("\n[%4lu] ", i);
      flg = TRUE;
      for(j = 0; j < NEntries; j++) {
	if( p->IsValid() )
	  printf(" <%08lx,%08lx,%02lu,%02lu,%08lx>", ((p->Tag * NSets) + i) * 4, p->Tag, p->Count, p->SatCount, p->Dest);
	p++;
      }
    }
  }
  if(flg == FALSE)
    printf("\nBranch Target Buffer is empty");
  printf("\n");
}
