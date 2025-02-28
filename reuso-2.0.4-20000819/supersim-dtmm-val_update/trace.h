
/*
 * trace.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 10/12/99
 */

#ifndef __TRACE_H
#define __TRACE_H 375

/////////////////////////////////////////////////////////////
///// CTrace

class CTrace {
 private:

  char** pBuffer;

  uword_t NEntries;

  uword_t Size;

 public:

  CTrace(uword_t _nentries);

  ~CTrace();

  uword_t GetBufferSize();

  void InsertItem(char* _str);

  char* GetNthItem(uword_t _idx);

  void Flush();

  void Show(bool_t _dc = FALSE);

};

#endif
