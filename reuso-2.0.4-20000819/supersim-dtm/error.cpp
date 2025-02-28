
/*
 * error.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#include<stdio.h>
#include<stdlib.h>
#include"defs.h"
#include"types.h"
#include"error.h"

/* definicao da variavel global para complemento de informacoes de error
 */

str_t errtile;


/* implementacao das funcoes de erro da aplicacao
 */

void errmsg(int _foo, const char* _err, const char* _msg)
{
  printf("\n\nERR(%d): %s. %s\n", _foo, _err, _msg);
  exit(_foo);
}

void warnmsg(int _foo, const char* _err, const char* _msg)
{
#if(DEBUG == 0)
  printf("\n\nWARN(%d): %s. %s\n", _foo, _err, _msg);
#endif
}
