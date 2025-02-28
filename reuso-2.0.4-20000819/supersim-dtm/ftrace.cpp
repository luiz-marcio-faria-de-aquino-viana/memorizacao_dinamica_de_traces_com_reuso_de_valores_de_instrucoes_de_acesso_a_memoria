
/*
 * ftrace.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 10/11/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

extern str_t errtile;

CFTrace::CFTrace()
{
  (*FileName) = '\0';
  Fp = 0;
}

CFTrace::CFTrace(char* _filename)
{
  strcpy(FileName, _filename);
  Fp = 0;
}

void CFTrace::Open()
{
  if(Fp == 0) {
    if((Fp = fopen(FileName, "w")) == 0) {
      sprintf(errtile, "(=%s)", FileName);
      errmsg(__FTRACE_H, ERR_CANTOPENTRACEFILE, errtile);
    }
    printf("\nTrace enabled.");
  }
  else
    printf("\nERR: This trace already is enable.");
}

void CFTrace::Close()
{
  if(Fp != 0) {
    fclose(Fp);
    Fp = 0;
    printf("\nTrace disabled.");
  }
  else
    printf("\nERR: This trace already is disable.");
}

CFTrace::~CFTrace()
{
  if(Fp != 0)
    fclose(Fp);
}

void CFTrace::SetTraceFile(char* _filename)
{
  if(Fp != 0)
    fclose(Fp);
  Fp = 0;

  strcpy(FileName, _filename);
}

void CFTrace::InsertItem(char* _str)
{
  if(Fp == 0) {
    sprintf(errtile, "(=%s)", FileName);
    errmsg(__FTRACE_H, ERR_TRACEFILENOTOPEN, errtile);
  }
  fprintf(Fp, "%s\n", _str);
}

