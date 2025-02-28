
/*
 * rsctl.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/13/99
 */

#ifndef __RSCTL_H
#define __RSCTL_H 171

///////////////////////////////////////////////////////////
///// CRsctl

class CRsctl : public CIDecctl {
 private:

  bool_t Ready;     // indicador de dado disponivel


  uword_t Dep;      // numero de sequencia da instrucao precedente na dependencia

  uword_t Val;      // valor do registrador requerido


 public:

  CRsctl();


  int IsReady();


  void SetDep(uword_t _seq);

  void SetDep(uword_t _r, uword_t _seq);

  uword_t GetDep();

  void DecDep(uword_t _seq);

  void ClearDep(uword_t _val = 0);


  void SetValue(uword_t _val);

  void SetValue(uword_t _r, uword_t _val);

  uword_t GetValue();


  int Update(uword_t _seq, uword_t _val);

  int Update(uword_t _seq, uword_t _srcwin, uword_t _cwp, uword_t _reg, uword_t _val);


  void Set(CIDecctl _it);

};

#endif
