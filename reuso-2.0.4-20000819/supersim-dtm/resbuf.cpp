
/*
 * resbuf.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/19/99
 */

#include<stdio.h>
#include"all.h"

///////////////////////////////////////////////////////////////////////
///// CResBuf

CResBuf::CResBuf()
{
  Reset();
}

int CResBuf::IsValid()
{
  return Valid;
}

void CResBuf::Set( 
    CRsItem* _rsit,
    uword_t _address,
    uword_t _result1,
    uword_t _result2,
    uword_t _y,
    uword_t _psr)
{
  Set( 
    _rsit,
    _address,
    _result1,
    _result2,
    _y,
    _psr,
    FALSE,
    0,
    FALSE,
    0,
    0);
}

void CResBuf::Set( 
    CRsItem* _rsit,
    uword_t _address,
    uword_t _result1,
    uword_t _result2,
    uword_t _y,
    uword_t _psr,
    bool_t _trapsig)
{
  Set( 
    _rsit,
    _address,
    _result1,
    _result2,
    _y,
    _psr,
    _trapsig,
    0,
    FALSE,
    0,
    0);
}

void CResBuf::Set( CRsItem* _rsit,
    uword_t _address,
    uword_t _result1,
    uword_t _result2,
    uword_t _y,
    uword_t _psr,
    bool_t _trapsig,
    uword_t _trapnum)
{
  Set( 
    _rsit,
    _address,
    _result1,
    _result2,
    _y,
    _psr,
    _trapsig,
    _trapnum,
    FALSE,
    0,
    0);
}

void CResBuf::Set( 
    CRsItem* _rsit,
		uword_t _address,
		uword_t _result1,
		uword_t _result2,
		uword_t _y,
		uword_t _psr,
		bool_t _trapsig,
		uword_t _trapnum,
		bool_t _chgpc)
{
  Set( 
    _rsit,
    _address,
    _result1,
    _result2,
    _y,
    _psr,
    _trapsig,
    _trapnum,
    _chgpc,
    0,
    0 );
}

void CResBuf::Set( CRsItem* _rsit,
    uword_t _address,
    uword_t _result1,
    uword_t _result2,
    uword_t _y,
    uword_t _psr,
    bool_t _trapsig,
    uword_t _trapnum,
    bool_t _chgpc,
    uword_t _newpc)
{
  Set( 
    _rsit,
        _address,
        _result1,
        _result2,
        _y,
        _psr,
        _trapsig,
        _trapnum,
        _chgpc,
        _newpc,
        0);
}

void CResBuf::Set( CRsItem* _rsit,
    uword_t _address,
    uword_t _result1,
    uword_t _result2,
    uword_t _y,
    uword_t _psr,
    bool_t _trapsig = FALSE,
    uword_t _trapnum = 0,
    bool_t _chgpc = FALSE,
    uword_t _newpc = 0,
    uword_t _maddr = 0 )
{
  Valid = TRUE;
  Rsit = _rsit;
  Address = _address;
  Result1 = _result1;
  Result2 = _result2;
  Y = _y;
  Psr = _psr;
  Trapsig = _trapsig;
  Trapnum = _trapnum;
  ChgPC = _chgpc;
  NewPC = _newpc;
  MAddr = _maddr;
}

void CResBuf::Reset()
{
  Valid = FALSE;
}

