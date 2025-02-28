
/*
 * wb.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/5/99
 */

#ifndef __WB_H
#define __WB_H 205

//////////////////////////////////////////////////////////
///// CWb

class CWb {
 private:

  CStat* Stat;

  CFetchBuf* FetchBuf;

  CFetch* Fetch;

  CIQueue* IQueue;

  CComponent* Dtm;

  CDisq* Disq;

  CDisq* DBDisq;

  CDispatch* Dispatch;

  CIReg* IReg;

  CSReg* SReg;

  CRs* RsIu;

  CRs* RsLsu;

  CRs* RsBpu;

  CIu* Iu;

  CLsu* Lsu;

  CBpu* Bpu;

  CMaq* Maq;

  CRob* Rob;

  CTrap* Trap;


  CTrace Trace;


  CFTrace ITrace;

  CFTrace BTrace;


  CVTrace STrace;


  int BB_State;


  int WTrace;

 public:

#if(DEBUG == 0)
  int NTraps;
#endif

  void Init( CStat* _stat,
	     CFetchBuf* _fetchbuf,
	     CFetch* _fetch,
	     CIQueue* _iqueue,
	     CComponent* Dtm,
	     CDisq* _disq,
	     CDisq* _dbdisq,
	     CDispatch* _dispatch,
	     CIReg* _ireg,
	     CSReg* _sreg,
	     CRs* _rsiu,
	     CRs* _rsbpu,
	     CRs* _rslsu,
	     CMaq* _maq,
	     CRob* _rob,
	     CIu* _iu,
	     CLsu* _lsu,
	     CBpu* _bpu,
	     CTrap* _trap );


  CWb();


  void Execute();

  void Execute(udword_t* _nsteps);


  void WriteTrace(CRobItem* _it);


  void WriteBTrace(CRobItem* _it, char* _s);

  void WriteSTrace(CRobItem* _it);


  void ShowTrace();


  void SetTraceFile(char* _filename);


  void EnableTrace(int _wtmask);

  void DisableTrace(int _wtmask);

};

#endif
