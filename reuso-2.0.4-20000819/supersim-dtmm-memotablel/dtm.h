
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

  CLsu* Lsu;

  CRob* Rob;


  // variavel que armazena o estado de operacao do DTM

  int Status;    // DTM_DISABLED - recurso desabilitado
                 // DTM_ENABLED - habilitado sem acesso a memoria
                 // DTM_WITH_MEM_ENABLED - habilitado com acesso a memoria


  CMemoTableG MemoTableG;

  CMemoTableT MemoTableT;

  CMemoTableL MemoTableL;


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


  CDtm(uword_t _memotbgsz, uword_t _memotbtsz, uword_t _memotblsz);


  bool_t IsEnabled();

  bool_t IsEnabledWithMem();

  void SetEnabled(int _status);


  bool_t IsValidDtmInst(CRobItem* _robit);


  bool_t EmptyBufferT();


  void ClearBufferT();

  void UpdateBufferT(CRobItem* _robit);


  void BtbUpdate(CMemoTableEntryT* p);


  bool_t BranchMaskOverflow(CRobItem* _robit);

  bool_t MatchBranchPattern(CMemoTableEntryT* p);


  // verifica se existe operacoes de escrita na memoria pendente

  bool_t HaveStorePending();


  // finaliza trace com instrucao de delay slot

  void CompleteBufferT(uword_t _daddr, uword_t _dinst);


  // finaliza trace com instrucao de acesso a memoria

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


  void Invalidate(uword_t _ptr, uword_t _len);

  void UpdateMemValue(uword_t _maddr, ubyte_t * _ptr, uword_t _len);


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
	     CLsu* _lsu,
	     CRob* _rob );


  void ShowMemoTableG();

  void ShowMemoTableT();

  void ShowMemoTableL();


  void Reset();

};

#endif
