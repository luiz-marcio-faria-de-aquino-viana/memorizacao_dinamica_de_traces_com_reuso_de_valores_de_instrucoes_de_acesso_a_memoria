
/*
 * cpu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#ifndef __CPU_H
#define __CPU_H 1000

////////////////////////////////////////////////////////////
///// CCpu

class CCpu {
 public:

  CStat Stat;

  CMem Mem;

  CBtb Btb;

  CPredict Predict;

  CFetchBuf FetchBuf;

  CIMmu IMmu;

  CDMmu DMmu;

  CIQueue IQueue;

  CFetch Fetch;

  CDtm Dtm;

  CDisq Disq;

  CDisq DBDisq;

  CDecode Decode;

  CSReg SReg;

  CIReg IReg;

  CRs RsIu;

  CRs RsBpu;

  CRs RsLsu;

  CMaq Maq;

  CRob Rob;

  CDispatch Dispatch;

  CIu Iu;

  CLsu Lsu;

  CBpu Bpu;

  CComplete Complete;

  CWb Wb;

  CTrap Trap;


  bool_t Breakflag;

  uword_t Breakpt;


  bool_t DtmBrkflag;

  uword_t DtmBrkCnt;


  udword_t Cicles;

  udword_t NSteps;

  bool_t Running;

  str_t FileName;

  bool_t Titled;


  /* ponteiros utilizados para acesso externo aos parametros
   */

  char** Argp;           /* ponteiro do vetor de ponteiros dos argumentos */

  char* Argv[ARGNUM];    /* vetor de ponteiros dos argumentos */

  int* Argc;             /* ponteiro para o contador de argumentos */


  void Tick(bool_t _steprun = FALSE);


  CCpu();


  udword_t GetNumCicles();


  void SetFileName(char* _filename);

  void SetNumSteps(udword_t _nsteps);


  void SetBreakpt(uword_t _memaddr);


  void ShowArgs();


  void Step();

  void Runn();

  void Run();

  void Reset();

  void Load();

};

#endif
