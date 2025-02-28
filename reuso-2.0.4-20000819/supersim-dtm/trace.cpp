
/*
 * trace.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 10/11/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

CTrace::CTrace(uword_t _nentries)
{
  int size = (NEntries = _nentries);
  pBuffer = new char*[size];
  if(pBuffer == NULL)
    errmsg(__TRACE_H, ERR_CANTALLOCMEM, "(array creation fail)");

  for(uword_t i = 0; i < NEntries; i++)
    if((pBuffer[i] = new char [STRSZ]) == 0)
      errmsg(__TRACE_H, ERR_CANTALLOCMEM, "(string creation fail)");

  Size = 0;
}

CTrace::~CTrace()
{
  for(uword_t i = 0; i < NEntries; i++)
    delete [] pBuffer[i];
  delete [] pBuffer;
}

uword_t CTrace::GetBufferSize()
{
  return Size;
}

void CTrace::InsertItem(char* _str)
{
  uword_t i;
  char* p;

  if(Size == NEntries) {
    p = pBuffer[0];
    for(i = 1; i < NEntries; i++)
      pBuffer[i - 1] = pBuffer[i];
    pBuffer[(Size -= 1)] = p;
  }

  strncpy(pBuffer[Size], _str, STRSZ);
  pBuffer[Size][STRSZ - 1] = '\0';
  Size += 1;
}

char* CTrace::GetNthItem(uword_t _idx)
{
  if(_idx >= Size)
    errmsg(__TRACE_H, ERR_OUTOFBOUNDS, "");
  return pBuffer[_idx];
}

void CTrace::Flush()
{
  Size = 0;
}

void CTrace::Show()
{
  Show(FALSE);
}

void CTrace::Show(bool_t _dc)
{
  for(uword_t i = 0; i < Size; i++) {
    if(_dc == TRUE)
      printf("%4lu. %s\n", i, pBuffer[i]);
    else
      printf("%s\n", pBuffer[i]);
  }
}
