
/*
 * btb.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#ifndef __BTB_H
#define __BTB_H 132

//////////////////////////////////////////////////////////////////
///// CBtbEntry

class CBtbEntry {
 public:

  bool_t Valid;         // indicador de entrada valida

  uword_t Tag;          // campo identificador do endereco da instrucao

  uword_t Count;        // contador para o algoritmo LRU implementado

  uword_t SatCount;     // contador saturado para efetuar previsao

  uword_t Dest;         // endereco de destino do desvio


  CBtbEntry();


  int IsValid();

  void SetValid();

  void SetNotValid();


  uword_t GetLifeTime();

  void IncLifeTime();


  uword_t GetTagValue();

  uword_t GetSatCount();

  uword_t GetTargetAddr();

  void SetItem(uword_t _tag, bool_t _bpred, uword_t _baddr, bool_t _rstSat = FALSE);

};

//////////////////////////////////////////////////////////////////
///// CBtb

class CBtb {
 private:

  /* estrutura de controle da BTB */

  CBtbEntry** Btb;

  uword_t NSets;                  // numero maximo de conjuntos

  uword_t NEntries;               // numero de entradas por conjunto


  void AddEntry(uword_t _slot, uword_t _tag, bool_t* _bpred, uword_t* _baddr);

  void UpdateEntry(uword_t _slot, uword_t _entry, bool_t* _bpred, uword_t* _baddr);


 public:


  CBtb(uword_t _nsets, uword_t _nentries);

  ~CBtb();


  int Evaluate(uword_t _addr, bool_t* _bpred, uword_t* _baddr);

  int Update(uword_t _addr, bool_t* _bpred, uword_t* _baddr);


  void Reset();


  void Show();

};

#endif
