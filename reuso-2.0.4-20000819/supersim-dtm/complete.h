
/*
 * complete.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/5/99
 */

#ifndef __COMPLETE_H
#define __COMPLETE_H 195

//////////////////////////////////////////////////////////
///// CComplete

class CComplete {
 private:

  CIReg* IReg;

  CSReg* SReg;

  CRs* RsIu;

  CRs* RsBpu;

  CRs* RsLsu;

  CIu* Iu;

  CBpu* Bpu;

  CLsu* Lsu;

  CRob* Rob;

 public:

  void Init( CIReg* _ireg,
	     CSReg* _sreg,
	     CRs* _rsiu,
	     CRs* _rsbpu,
	     CRs* _rslsu,
	     CIu* _iu,
	     CBpu* _bpu,
	     CLsu* _lsu,
	     CRob* _rob );

  void Step(CResBuf* _rb);

  void Execute();

};

#endif
