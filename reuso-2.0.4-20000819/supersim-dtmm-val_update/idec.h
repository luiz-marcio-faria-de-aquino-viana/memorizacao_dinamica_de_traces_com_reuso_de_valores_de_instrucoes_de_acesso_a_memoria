
/*
 * idec.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#ifndef __IDEC_H
#define __IDEC_H 143

/////////////////////////////////////////////////////////////
///// CIDec

class CIDec : public CIQueueItem {
 public:

  bool_t Bad;          // indicador de instrucao invalida


  uword_t Unit;        // tipo de unidade para operacao

  uword_t Opcode;      // codigo de operacao da instrucao


  uword_t RType;       // 0x00 = se instrucao nao faz leitura,
                       // 0x01 = se instrucao le byte,
                       // 0x02 = se instrucao le byte sem sinal,
                       // 0x04 = se instrucao le meia palavra,
                       // 0x08 = se instrucao le meia palavra sem sinal,
                       // 0x10 = se instrucao le palavra,
                       // 0x40 = se instrucao le palavra dupla

  uword_t WType;       // 0x00 = se instrucao nao faz escrita,
                       // 0x01 = se instrucao escreve byte,
                       // 0x04 = se instrucao escreve meia palavra,
                       // 0x10 = se instrucao escreve palavra inteira,
                       // 0x40 = se instrucao escreve palavra dupla,


  struct {

    uword_t op;        // seletor do formatos da instrucoes (1, 2 ou 3)

    uword_t disp30;    // 30 bits - endereco alvo do desvio relativo ao PC

    uword_t rd;        // seletor do registrador destino das instrucoes

    uword_t op2;       // seletor de instrucao (0=UNIMP,2=Bicc,4=SETHI,...)

    uword_t imm22;     // 22 bits - valor imediato usado por SETHI

    uword_t a;         // bit de controle do comportamento do 'delay branch'

    uword_t cond;      // seletor da operacao condicional (ba,bn,bne,...)

    uword_t disp22;    // 22 bits - endereco alvo do desvio relativo ao PC

    uword_t op3;       // seletor de um dos tres tipos do formato 3

    uword_t rs1;       // seletor do registrador fonte da operacao

    uword_t i;         // seletor do segundo operando (0=rs2,1=simm13)

    uword_t asi;       // seletor do espaco de enderecamento em load/store

    uword_t rs2;       // seletor do registrador fonte da operacao

    uword_t simm13;    // 13 bits - valor imediato utilizado nas operacoes

    uword_t opf;       // seletor para as operacoes de ponto flutuante

  } fi;


  CIDecctl Psr;         // requerimento do registrador de estado

  CIDecctl Y;           // requerimento do registrador Y

  CIDecctl Rs1;         // requerimento do primeiro registrador fonte

  CIDecctl Rs2;         // requerimento do segundo registrador fonte

  CIDecctl Rd_hi;       // requerimento da palavra mais significativa do registrador destino

  CIDecctl Rd_lo;       // requerimento da palavra menos significativa do registrador destino


  int IsBadInst();


  int IsLdStInst();


  uword_t GetExecUnit();

  uword_t GetInstOpcode();


  void Set(CIQueueItem iq);

};

#endif
