
/*
 * decode.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */


#ifndef __DECODE_H
#define __DECODE_H 145


/////////////////////////////////////////////////////////////
///// CDecode

class CDecode {
 private:

  CIQueue* IQueue;

  CDisq* Disq;

 public:

  void Init( CIQueue* _iqueue,
	     CDisq* _disq );

  void Execute();

};

#endif
