
/*
 * idecctl.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#ifndef __IDECCTL_H
#define __IDECCTL_H 144

/////////////////////////////////////////////////////////////
///// CIDecctl

class CIDecctl {
 protected:

  ubyte_t Req;     // (0=nao requerido, 1=requerido como fonte,2=requerido como destino)

  uword_t Reg;     // endereco do registrador requerido

  
 public:


  CIDecctl();


  void Reset();

  void Set(ubyte_t _req, uword_t _reg = 0);


  bool_t IsReqAsSource();

  bool_t IsReqAsTarget();


  uword_t GetRegAddr();

  CIDecctl & operator=(CIDecctl & _it);

};

#endif
