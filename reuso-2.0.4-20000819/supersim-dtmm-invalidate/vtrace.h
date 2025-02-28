
/*
 * vtrace.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 10/12/99
 */

#ifndef __VTRACE_H
#define __VTRACE_H 378

/////////////////////////////////////////////////////////////
///// CVTrace

class CVTrace {
 private:

  str_t FileName;

  FILE* Fp;

  vtbuf_t* Buf;

  int BufSz;

  int Num;

 public:

  CVTrace(int _bufsz);

  CVTrace(int _bufsz, char* _filename);

  ~CVTrace();

  void Open();

  void Close();

  void SetTraceFile(char* _filename);

  void InsertItem(uword_t _addr, ubyte_t _rd, uword_t _res, uword_t _n);

};

#endif
