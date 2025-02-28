
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

  CStat* Stat;

  CBtb* Btb;

  CFetchBuf* FetchBuf;

  CFetch* Fetch;

  CIQueue* IQueue;

  CDisq* Disq;

  CDispatch* Dispatch;

  CSReg* SReg;

  CIReg* IReg;

  CRs* RsLsu;

  CMaq* Maq;

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


  //////////////////////////////////////////////////////////////////
  //// contabilacao estatistica de traces dependentes de loads
  bool_t DepMap [ IREG_NGREGS + (IREG_WINOFFSET * IREG_NUMWIN) ];

  bool_t DepMapY;

  bool_t DepMapPSR;


  void SetDepMap(CRobItem* _robit);

  int CheckDepMap(CRobItem* _robit);


  CDtm(uword_t _memotbgsz, uword_t _memotabtsz);


  bool_t IsEnabled();

  void SetEnabled();


  bool_t IsValidDtmInst(CRobItem* _robit);


  bool_t EmptyBufferT();


  void ClearBufferT();

  void UpdateBufferT(CRobItem* _robit);


  void BtbUpdate(CMemoTableEntryT* p);


  bool_t BranchMaskOverflow(CRobItem* _robit);

  bool_t MatchBranchPattern(CMemoTableEntryT* p);


  // finaliza trace com instrucao de delay slot

  void CompleteBufferT(uword_t _daddr, uword_t _dinst);


  // finaliza trace com instrucao de escrita em memoria

  void CompleteBufferT(CRobItem* _robit);


  // finaliza trace com instrucoes invalidas ou nao-redundantes

  void CompleteBufferT();


  bool_t ContextOverflow(CRobItem* _robit);


  bool_t VerifyReuse(CDisqItem* _di);


  void ReservRegs(CRsItem* _rsit, CRobItem* _robit, CMemoTableEntryG* _it);

  void ReservRegs(CRsItem* _rsit, CRobItem* _robit, CMemoTableEntryT* _it);


  bool_t Stage_DS1(CFetchBufItem* _fb);

  bool_t Stage_DS2(CDisqItem* _di);

  bool_t Stage_DS3_TraceReuse(CRobItem* _rob);          // atualiza registradores de saida

  bool_t Stage_DS3_TraceConstruction(CRobItem* _rob);


  void Init( CStat* _stat,
	     CBtb* _btb,
	     CFetchBuf* _fetchbuf,
	     CFetch* _fetch,
	     CIQueue* _iqueue,
	     CDisq* _disq,
	     CDispatch* _dispatch,
	     CSReg* _sreg,
	     CIReg* _ireg,
	     CRs* _rslsu,
	     CMaq* _maq,
	     CRob* _rob );


  void ShowMemoTableG();

  void ShowMemoTableT();


  void Reset();

};

#endif


