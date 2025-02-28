
/*
 * dmmu.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/22/99
 */

#ifndef __DMMU_H
#define __DMMU_H 187

/////////////////////////////////////////////////////////////
///// CDMmu

class CDMmu {
 private:

  CMem* Mem;


  databuf_t RdBuf;

  databuf_t WrBuf;


 public:


  void Init( CMem* _mem );


  void MemRead( uword_t _rtype,
	        uword_t _addr,
		uword_t & _hi,
		uword_t & _lo );

  void MemWrite( uword_t _wtype,
		 uword_t _addr,
		 uword_t _hi,
		 uword_t _lo );

};

#endif
