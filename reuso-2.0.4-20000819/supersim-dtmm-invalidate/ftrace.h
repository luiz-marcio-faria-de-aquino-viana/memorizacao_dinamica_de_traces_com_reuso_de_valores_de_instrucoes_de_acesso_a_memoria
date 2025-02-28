
/*
 * ftrace.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 10/12/99
 */

#ifndef __FTRACE_H
#define __FTRACE_H 380

/////////////////////////////////////////////////////////////
///// CFTrace

class CFTrace {
 private:

  FILE* Fp;

  str_t FileName;

 public:

  CFTrace();

  CFTrace(const char* _filename);

  ~CFTrace();

  void Open();

  void Close();

  void SetTraceFile(const char* _filename);

  void InsertItem(const char* _str);

};

#endif
