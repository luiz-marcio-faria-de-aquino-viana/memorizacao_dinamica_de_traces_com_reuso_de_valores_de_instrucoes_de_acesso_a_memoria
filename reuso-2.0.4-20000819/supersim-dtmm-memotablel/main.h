
/*
 * main.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/1/99
 */


#ifndef __MAIN_H
#define __MAIN_H 250

////////////////////////////////////////////////////////////
///// CMainApp

class CMainApp {
 private:

  FILE* cstdin;

  CCpu Cpu;


  uword_t RegWin;

  uword_t RegAddr;


  uword_t ShowOptions;


  uword_t ShowMemAddr;

  uword_t ShowFollowPC;

  uword_t ShowProgAddr;

  uword_t ShowRs;

  uword_t ShowIReg;

  uword_t ShowSReg;

  uword_t ShowDtm;


  void CopyrightMsg();


  int GetUserInput(const char* s, int n);


  uword_t SetShowRs(int opt);

  uword_t GetShowRs();


  uword_t SetShowOptions(int opt);

  uword_t GetShowOptions();


  uword_t SetShowDtm(int opt);

 public:

  CMainApp();


  void SetStdin(const char* filename);


  void Run();

  void Show();

};

#endif
