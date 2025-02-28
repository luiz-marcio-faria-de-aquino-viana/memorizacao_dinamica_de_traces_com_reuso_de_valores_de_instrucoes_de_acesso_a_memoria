
/*
 * dtm.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 5/17/2000
 */

#ifndef __DTM_H
#define __DTM_H 400

////////////////////////////////////////////////////////////////
///// CDtm

class CDtm : public CComponent {
 public:

  CBtb* Btb;

  CFetchBuf* FetchBuf;

  CFetch* Fetch;

  CIQueue* IQueue;

  CDisq* Disq;

  CSReg* SReg;

  CIReg* IReg;

  CRob* Rob;


  bool_t Enabled;


  CMemoTableG MemoTableG;

  CMemoTableT MemoTableT;


  bool_t Ic_PsrMap;

  bool_t Ic_YMap;

  bool_t Ic_IRegMap [ IREG_NGREGS + IREG_WINSIZE];


  bool_t Oc_PsrMap;

  bool_t Oc_YMap;

  bool_t Oc_IRegMap [ IREG_NGREGS + IREG_WINSIZE ];


  CMemoTableEntryT BufferT;
  uword_t PC;


  CDtm(uword_t _memotbgsz, uword_t _memotabtsz);


  bool_t IsEnabled();

  void SetEnabled();


  bool_t IsValidDtmInst(CRobItem* _robit);


  bool_t EmptyBufferT();


  void ClearBufferT();

  void UpdateBufferT(CRobItem* _robit);


  bool_t BranchMaskOverflow(CRobItem* _robit);

  bool_t MatchBranchPattern(CMemoTableEntryT* p);


  void CompleteBufferT(uword_t _daddr, uword_t _dinst);

  void CompleteBufferT();


  bool_t ContextOverflow(CRobItem* _robit);


  bool_t VerifyReuse(CDisqItem* _di);


  bool_t Stage_DS1(CFetchBufItem* _fb);

  bool_t Stage_DS2(CDisqItem* _di);

  bool_t Stage_DS3_TraceReuse(CRobItem* _rob);          // atualiza registradores de saida

  bool_t Stage_DS3_TraceConstruction(CRobItem* _rob);


  void Init( CBtb* _btb,
	     CFetchBuf* _fetchbuf,
	     CFetch* _fetch,
	     CIQueue* _iqueue,
	     CDisq* _disq,
	     CSReg* _sreg,
	     CIReg* _ireg,
	     CRob* _rob );


  void ShowMemoTableG();

  void ShowMemoTableT();

};

#endif


