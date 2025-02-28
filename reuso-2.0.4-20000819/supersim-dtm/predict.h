
/*
 * predict.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/8/99
 */

#ifndef __PREDICT_H
#define __PRECICT_H 135

/////////////////////////////////////////////////////////////
///// CPredict

class CPredict {
 private:

  CBtb* Btb;

 public:

  void Init( CBtb* _btb );

  int Evaluate(uword_t _addr, bool_t* _bpred, uword_t* _baddr);

};

#endif
