
/*
 * immu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/3/1999
 */

#ifndef __IMMU_H
#define __IMMU_H 115

//////////////////////////////////////////////////////////////
///// CIMmu

class CIMmu {
 public:

  CMem* Mem;

  CFetchBuf* FetchBuf;

  str_t errtile;

  void Init( CMem* _mem,
	     CFetchBuf* _fetchbuf );

  CIMmu();

  void Execute(uword_t _addr, bool_t *_breakflag, uword_t _breakpt);

};

#endif
