
/*
 * resbuf.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 5/19/99
 */

#ifndef __RESBUF_H
#define __RESBUF_H 173

//////////////////////////////////////////////////////
///// CResBuf

class CResBuf {
 private:

  uword_t Valid;

 public:

  CRsItem* Rsit;


  uword_t Address;

  uword_t Result1;

  uword_t Result2;

  uword_t Y;

  uword_t Psr;


  uword_t MAddr;


  bool_t Trapsig;

  uword_t Trapnum;


  bool_t ChgPC;

  uword_t NewPC;


  CResBuf();


  int IsValid();

  void Set( CRsItem* _rsit,
	    uword_t _address,
	    uword_t _result1,
	    uword_t _result2,
	    uword_t _y,
	    uword_t _psr,
	    bool_t _trapsig = FALSE,
	    uword_t _trapnum = 0,
	    bool_t _chgpc = FALSE,
	    uword_t _newpc = 0,
	    uword_t _maddr = 0 );

  void Reset();

};

#endif
