
/*
 * dmmu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/22/99
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////
///// CDMmu

void CDMmu::Init( CMem* _mem )
{
  Mem = _mem;
}

void CDMmu::MemRead( uword_t _rtype,
		     uword_t _addr,
		     uword_t & _hi,
		     uword_t & _lo )
{
  RdBuf.d = 0;

  switch(_rtype) {
  case RD_BYTE:
    Mem->Read(_addr, (ubyte_t*) & RdBuf.b, sizeof(byte_t));
    _hi = (uword_t) RdBuf.b;
    break;
  case RD_UBYTE:
    Mem->Read(_addr, (ubyte_t*) & RdBuf.ub, sizeof(ubyte_t));
    _hi = (uword_t) RdBuf.ub;
    break;
  case RD_HWORD:
    Mem->Read(_addr, (ubyte_t*) & RdBuf.h, sizeof(hword_t));
    _hi = (uword_t) RdBuf.h;
    break;
  case RD_UHWORD:
    Mem->Read(_addr, (ubyte_t*) & RdBuf.uh, sizeof(uhword_t));
    _hi = (uword_t) RdBuf.uh;
    break;
  case RD_WORD:
    Mem->Read(_addr, (ubyte_t*) & RdBuf.w, sizeof(word_t));
    _hi = (uword_t) RdBuf.w;
    break;
  case RD_DWORD:
    Mem->Read(_addr, (ubyte_t*) & RdBuf.d, sizeof(dword_t));
    _hi = (uword_t) ((RdBuf.d >> 32) & 0xFFFFFFFFL);
    _lo = (uword_t) (RdBuf.d & 0xFFFFFFFFL);
  }

#if(DEBUG == 0)
  if(_addr == 0x61a8)
    printf("\n=================================================================================");
  if(_rtype == RD_DWORD)
    printf("\nDMMU::READ(addr=%lx, hi=%lx, lo=%lx)", _addr, _hi, _lo);
  else
    printf("\nDMMU::READ(addr=%lx, hi=%lx)", _addr, _hi);
#endif
}

void CDMmu::MemWrite( uword_t _wtype,
		      uword_t _addr, 
		      uword_t _hi,
		      uword_t _lo )
{
#if(DEBUG == 0)
  if(_addr == 0x61a8)
    printf("\n=================================================================================");
  if(_wtype == WR_DWORD)
    printf("\nDMMU::WRITE(addr=%lx, hi=%lx, lo=%lx)", _addr, _hi, _lo);
  else
    printf("\nDMMU::WRITE(addr=%lx, hi=%lx)", _addr, _hi);
#endif

  switch(_wtype) {
  case WR_BYTE:
    WrBuf.b = (byte_t) _hi;
    Mem->Write(_addr, (ubyte_t*) & WrBuf.b, sizeof(byte_t));
    break;
  case WR_HWORD:
    WrBuf.h = (hword_t) _hi;
    Mem->Write(_addr, (ubyte_t*) & WrBuf.h, sizeof(hword_t));
    break;
  case WR_WORD:
    WrBuf.w = (word_t) _hi;
    Mem->Write(_addr, (ubyte_t*) & WrBuf.w, sizeof(word_t));
    break;
  case WR_DWORD:
    WrBuf.d = (((dword_t) _hi) << 32) + ((dword_t) _lo);
    Mem->Write(_addr, (ubyte_t*) & WrBuf.d, sizeof(dword_t));
  }
}



