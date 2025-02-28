
/*
 * asm.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/5/99
 */

#ifndef __ASM_H
#define __ASM_H 50

////////////////////////////////////////////////////////
///// CAsm

class CAsm {
 private:

  str_t buf;

 public:

  void Set(uword_t _addr, uword_t _inst);

  void Get(char* s, int len = -1);

};

#endif
