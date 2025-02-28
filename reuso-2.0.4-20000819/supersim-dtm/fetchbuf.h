
/*
 * fetchbuf.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 9/3/1999
 */

#ifndef __FETCHBUF_H
#define __FETCHBUF_H 137

//////////////////////////////////////////////////////////////////
///// CFetchBufItem

class CFetchBufItem {
 public:

  /* estrutura de dados comuns a todas as instrucoes */

  uword_t Seq;         // numero de sequencia

  uword_t Addr;        // endereco da instrucao

  uword_t Inst;        // codigo da instrucao

  uword_t DSlot;       // (TRUE=instrucao no delay slot)

  uword_t BDep;        // (TRUE=instrucao no delay slot com dependencia de desvio)

  uword_t WBDep;       // (TRUE=instrucao no delay slot que foi dependente de desvio)

  uword_t BSeq;        // numero de sequencia do desvio que produz a dependencia

  uword_t BLevel;      // profundidade de execucao especulativa

  uword_t NewPC;       // endereco da instrucao seguinte

  /* estrutura de dados das instrucoes de desvio */

  bool_t BInst;        // (TRUE=instrucao de desvio)

  bool_t BPred;        // previsao para o desvio (TAKEN=tomado, NTAKEN=nao tomado)

  uword_t BAddr;       // endereco da instrucao alvo do desvio

  uword_t BType;       // tipo de instrucao de desvio (__CALL, __BA, __BN, ...)

  bool_t BResult;     // resultado do desvio (TAKEN=tomado, NTAKEN=nao tomado)

  /* controle de invalidacoes das instrucoes */

  bool_t InvalidInst;   // informa se a instrucao do pipe foi invalidada

  bool_t InstDependentRedundant;   // indicador de instrucao dependente e redundante

  bool_t ProbRedundant;   // indicador de instrucao provavel redundante

  bool_t InstRedundant;   // indicador de instrucao redundante 

  bool_t TraceRedundant;  // indicador de trace redundante

  CFetchBufItem();

  /* membros comuns a todas as instrucoes */

  void SetItem( uword_t _seq,
	        uword_t _addr,
	        uword_t _inst,
	        bool_t _bdep,
	        uword_t _bseq,
	        uword_t _blevel,
		bool_t _dslot );  

  void SetNewPC(uword_t _newpc);

  int BDec();

  /* controle de instrucoes invalidadas no pipe */

  bool_t IsInvalid();

  void SetInvalidBit();

  /* membros comuns as instrucoes de desvios */

  int IsBranchInst();

  int IsPredTaken();

  void SetBranchPred(bool_t _bpred, uword_t _baddr);

  int GetBranchType();

  uword_t DecSeqNum();

  uword_t GetSeqNum();

  uword_t DecBranchLevel();

  uword_t GetBranchLevel();

  uword_t GetTargetAddr();

  int IsDelaySlotInst();

  int IsBranchDep();

  int WasBranchDep();

  void ClearBranchDep();

  uword_t DecBranchDepNum();

  uword_t GetBranchDepNum();

  bool_t IsTraceRedundant();

  void SetTraceRedundant();

  bool_t IsProbablyRedundant();

  void SetProbablyRedundant();

  bool_t IsDependentRedundant();

  void SetDependentRedundant();

  bool_t IsRedundant();

  void SetRedundant();

  CFetchBufItem& operator=(CFetchBufItem& _fb);

};

//////////////////////////////////////////////////////////////////
///// CFetchBuf

class CFetchBuf : public CQueue {
 private:

  CFetchBufItem* pFetchBufArray;

 public:

  CPredict* Predict;

  uword_t SeqNum;

  uword_t BLevel;     /* nivel de desvio corrente */

  uword_t LastBLevel; /* ultimo nivel de desvio executado */

  void Init( CPredict* _predict );

  CFetchBuf(uword_t _nentries);

  ~CFetchBuf();

  int IsFull();

  uword_t GetCurrSeqNum();

  void FreeSeqNum(uword_t _seq);

  void FreeBranchLevel(uword_t _blevel);

  CFetchBufItem* InsertItem( uword_t _addr,
			     uword_t _inst,
			     bool_t _dslot);

  CFetchBufItem* InsertItem( uword_t _addr,
			     uword_t _inst,
			     bool_t _dslot,
			     bool_t _oblevel);

  CFetchBufItem* InsertItem( uword_t _addr,
			     uword_t _inst,
			     bool_t _dslot,
			     bool_t _oblevel,
			     bool_t _bdep);

  CFetchBufItem* InsertItem( uword_t _addr,
			     uword_t _inst,
			     bool_t _dslot,
			     bool_t _oblevel,
			     bool_t _bdep,
			     uword_t _bseq);

  CFetchBufItem* GetFirstItem();

  CFetchBufItem* GetNthItem(int nth);

  CFetchBufItem* GetLastItem();

  void RemoveFirstItem();

  void Swap(uword_t _i1, uword_t _i2);

  void SetLastBranchLevel(uword_t _lblevel);

  uword_t GetLastBranchLevel();

  void Reset();

  void Flush();

  void Flush(uword_t _seq, uword_t _blevel);

  void Show();
};

#endif

