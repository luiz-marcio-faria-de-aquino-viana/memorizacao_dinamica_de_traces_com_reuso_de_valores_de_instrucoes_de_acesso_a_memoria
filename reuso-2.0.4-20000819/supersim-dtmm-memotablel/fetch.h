
/*
 * fetch.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/3/99
 */

#ifndef __FETCH_H
#define __FETCH_H 130

////////////////////////////////////////////////////////////////
///// CFetch

class CFetch {
 public:

  CIMmu* IMmu;

  CFetchBuf* FetchBuf;

  CIQueue* IQueue;

  CComponent* Dtm;

  uword_t RegPC;

  void Init( CIMmu* _immu,
	     CFetchBuf* _fetchbuf,
	     CIQueue* _iqueue,
	     CComponent* _dtm);
  
  CFetch();

  void Execute(bool_t *_breakflag, uword_t _breakpt);


  void SetNewPC(uword_t _npc);


  void Reset(uword_t _entryPt);


  void Show();
};

#endif

