
/*
 * rob.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/11/99
 */

#ifndef __ROB_H
#define __ROB_H 200

/////////////////////////////////////////////////////////////
///// CRobctl

class CRobctl : public CIDecctl {
 private:

  bool_t Ready;     // indicador de dado disponivel

  uword_t SrcVal;   // valor fonte do registrador

  uword_t Val;      // valor destino do registrador


 public:

  CRobctl();


  int IsReady();


  void UpdateSrc(uword_t _val);

  void Update(uword_t _val);

  uword_t GetSrcValue();

  uword_t GetValue();

  void Set(CIDecctl _it);

};


/////////////////////////////////////////////////////////////
///// CRobItem

class CRobItem : public CDisqItem {
 public:

  bool_t Done;          // controla se instrucao foi completada


  uword_t SourceWin;      // janela de registradores fonte das operacoes

  uword_t TargetWin;      // janela de registradores destino das operacoes


  CRobctl Psr;          // controle do registrador de estado

  CRobctl Y;            // controle do registrador Y

  CRobctl Rs1;          // controle do primeiro registrador fonte

  CRobctl Rs2;          // controle do segundo registrador fonte

  CRobctl Rd_hi;        // controle da palavra mais significativa do registrador destino

  CRobctl Rd_lo;        // controle da palavra menos significativa do registrador destino

  bool_t Trapsig;

  uword_t Trapnum;

  bool_t MValidVal;     // sinalizador de valor de memoria valido

  uword_t MValhi;

  uword_t MVallo;

  uword_t MAddr;        // endereco efetivo de acesso a memoria


  int IsReady();
 

  void SetSourceWin(uword_t _cwin);

  uword_t GetSourceWin();


  void SetTargetWin(uword_t _cwin);

  uword_t GetTargetWin();


  void UpdateSrc( CRsItem* _rsit );

  void Update( uword_t _psr_val,
	       uword_t _y_val,
	       uword_t _rs1_val,
	       uword_t _rs2_val,
	       uword_t _rd_lo_val,
	       uword_t _rd_hi_val,
	       bool_t _trapsig = FALSE,
	       uword_t _trapnum = 0,
	       bool_t _chgpc = FALSE,
	       uword_t _newpc = 0,
	       CMemoTableEntryT* _tracedata = 0,
	       bool_t _bres = NTAKEN,
	       uword_t _maddr = 0 );


  void InvalidateMem(uword_t _maddr, uword_t _nbytes);

  void UpdateMem(uword_t _maddr, ubyte_t * _val, uword_t _nbytes);

  void SetItem(CDisqItem _di);

};


/////////////////////////////////////////////////////////////
///// CRob

class CRob : public CQueue {
 private:

  CRobItem* pRobArray;

 public:


  CRob(uword_t _nentries);

  ~CRob();


  void FreeSeqNum(uword_t _seq);

  void FreeBranchLevel(uword_t _blevel);


  bool_t BranchNotSolved(uword_t _seq);


  CRobItem* InsertItem(CDisqItem* _di);

  CRobItem* GetNthItem(uword_t _idx);

  CRobItem* GetFirstItem();

  void RemoveFirstItem();

  CRobItem* GetReadyItem();

  CRobItem* GetLastItem();

  void RemoveReadyItem();

  void UpdateSrc( uword_t _seq,
		  CRsItem* _rsit );

  CRobItem* Update( uword_t _seq,
		    uword_t _psr_val,
		    uword_t _y_val,
		    uword_t _rs1_val,
		    uword_t _rs2_val,
		    uword_t _rd_hi_val,
		    uword_t _rd_lo_val,
		    bool_t _trapsig = FALSE,
		    uword_t _trapnum = 0,
		    bool_t _chgpc = FALSE,
		    uword_t _newpc = 0,
		    CMemoTableEntryT* _tracedata = 0,
		    bool_t _bres = NTAKEN,
		    uword_t _maddr = 0 );

  void Flush(uword_t _blevel);


  void InvMemVal(uword_t _seq, uword_t _wtype, uword_t _maddr);

  void UpdMemVal(uword_t _seq, uword_t _wtype, uword_t _maddr, uword_t _res1, uword_t _res2);


  void Reset();

  void Show();

};

#endif
