
/*
 * mem.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#ifndef __MEM_H
#define __MEM_H 105

///////////////////////////////////////////////////////////////
///// CMem

class CMem {
private:

  CComponent * Dtm;

  bool_t * ptrMemBrkflag;

  uword_t * ptrMemBrkpt;


public:

  ubyte_t* Mem;

  uword_t MemSize;


  uword_t EntryPtr;

  uword_t ProgSize;


  str_t errtile;


  CMem(uword_t _memsize);

  ~CMem();


  void Init( CComponent * _dtm,
	     bool_t * _membrkflag,
	     uword_t * _membrkpt );


  uword_t GetEntryPtr();


  int Read(uword_t _addr, ubyte_t* _buf, int _size);

  int Write(uword_t _addr, ubyte_t* _buf, int _size);


  ubyte_t* GetMemPtr(uword_t _addr);


  void Load(char* _filename);         // funcao para carregamento da aplicacao


  void Show(uword_t _memaddr);

  void ShowProg(uword_t _progaddr);

};

#endif
