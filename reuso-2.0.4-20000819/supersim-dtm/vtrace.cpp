
/*
 * vtrace.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 10/11/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

extern str_t errtile;

CVTrace::CVTrace(int _bufsz)
{
  if((Buf = new vtbuf_t[(BufSz = _bufsz)]) == 0)
    errmsg(__VTRACE_H, ERR_CANTALLOCMEM, "(trace buffer creation fail)");
  (*FileName) = '\0';
  Fp = 0;
  Num = 0;
}

CVTrace::CVTrace(int _bufsz, char* _filename)
{
  if((Buf == new vtbuf_t[(BufSz = _bufsz)]) == 0)
    errmsg(__VTRACE_H, ERR_CANTALLOCMEM, "(trace buffer creation fail)");
  strcpy(FileName, _filename);
  Fp = 0;
  Num = 0;
}

void CVTrace::Open()
{
  str_t cmdLine;
  if(Fp == 0) {
    sprintf(cmdLine, "%s > %s", COMPRESS, FileName); 
    if((Fp = popen(cmdLine, "w")) == 0) {
      sprintf(errtile, "(=%s)", FileName);
      errmsg(__FTRACE_H, ERR_CANTOPENTRACEFILE, errtile);
    }
    printf("\nTrace enabled.");
  }
  else
    printf("\nERR: This trace already is enable.");
}

void CVTrace::Close()
{
  if(Fp != 0) {
    if(Num > 0)
      fwrite(Buf, sizeof(vtbuf_t), Num, Fp);
    pclose(Fp);
    Fp = 0;
    Num = 0;
    printf("\nTrace disabled.");
  }
  else
    printf("\nERR: This trace already is disable.");
}

CVTrace::~CVTrace()
{
  if(Fp != 0) {
    if(Num > 0)
      fwrite(Buf, sizeof(vtbuf_t), Num, Fp);
    pclose(Fp);
  }
}

void CVTrace::SetTraceFile(char* _filename)
{
  if(Fp != 0) {
    if(Num > 0)
      fwrite(Buf, sizeof(vtbuf_t), Num, Fp);
    pclose(Fp);
    Fp = 0;
    Num = 0;
  }

  strcpy(FileName, _filename);
}

void CVTrace::InsertItem(uword_t _addr, ubyte_t _rd, uword_t _res, uword_t _n)
{
  if(Fp == 0) {
    sprintf(errtile, "(=%s)", FileName);
    errmsg(__FTRACE_H, ERR_TRACEFILENOTOPEN, errtile);
  }

  if(Num >= BufSz) {
    fwrite(Buf, sizeof(vtbuf_t), Num, Fp);
    Num = 0;
  }

  Buf[Num].addr = _addr;
  Buf[Num].rd = _rd;
  Buf[Num].res = _res;
  Buf[Num].n = _n;

  Num += 1;

}
