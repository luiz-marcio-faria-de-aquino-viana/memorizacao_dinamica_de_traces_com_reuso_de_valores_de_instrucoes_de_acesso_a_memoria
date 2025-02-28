
/*
 * mem.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

///////////////////////////////////////////////////////////////
///// CMem

CMem::CMem(uword_t _memsize)
{
  if((Mem = new ubyte_t [(MemSize = _memsize)]) == 0)
    errmsg(__MEM_H, ERR_CANTALLOCMEM, "");
#if(DEBUG == 0)
  printf("\n[ MEM::PASS_1 ]");
#endif

  ptrMemBrkpt = NULL;
  ptrMemBrkflag = NULL;
}

CMem::~CMem()
{
  delete [] Mem;
}

void CMem::Init( CComponent * _dtm,
		 bool_t * _membrkflag,
		 uword_t * _membrkpt )
{
  Dtm = _dtm;
  ptrMemBrkflag = _membrkflag;
  ptrMemBrkpt = _membrkpt;
}

uword_t CMem::GetEntryPtr()
{
  return EntryPtr;
}

int CMem::Read(uword_t _addr, ubyte_t* _buf, int _size)
{

#if(DEBUG == 0)
  int i;
  printf("\nMEM::READ(memsize=%lx, addr=%lx, size=%d) ", MemSize, _addr, _size);  
  for(i = 0; i < _size; i++)
    printf(" %02x", _buf[i]);
#endif

  if( (ptrMemBrkpt != NULL) && (ptrMemBrkflag != NULL) ) {
    if((*ptrMemBrkpt) > 0) {
      if( (_addr <= (*ptrMemBrkpt)) && ((_addr + _size) >= (*ptrMemBrkpt)) )
	(*ptrMemBrkflag) = TRUE;
    }
  }

  if((_addr + _size) >= MemSize) {
    warnmsg(__MEM_H, ERR_ACCESSOUTOFMEM, "");
  }
  else {
#ifdef BIGENDIAN
    int j;
    for(j = 0; j < _size; j++)
      _buf[j] = Mem[_addr + (_size - j - 1)];
#else
    memcpy(_buf, & Mem[_addr], _size);
#endif
  }
  return RTNORM;
}

int CMem::Write(uword_t _addr, ubyte_t* _buf, int _size)
{

#if(DEBUG == 0)
  int i;
  printf("\nMEM::WRITE(memsize=%lx, addr=%lx, size=%d) ", MemSize, _addr, _size);  
  for(i = 0; i < _size; i++)
    printf(" %02x", _buf[i]);
#endif

  if( (ptrMemBrkpt != NULL) && (ptrMemBrkflag != NULL) ) {
    if((*ptrMemBrkpt) > 0) {
      if( (_addr <= (*ptrMemBrkpt)) && ((_addr + _size) >= (*ptrMemBrkpt)) )
	(*ptrMemBrkflag) = TRUE;
    }
  }

  if((_addr + _size) >= MemSize)
    warnmsg(__MEM_H, ERR_ACCESSOUTOFMEM, "");
  else {
#ifdef BIGENDIAN
    int j;
    for(j = 0; j < _size; j++)
      Mem[_addr + (_size - j - 1)] = _buf[j];
#else
    memcpy(& Mem[_addr], _buf, _size);
#endif
    ((CDtm*) Dtm)->Invalidate(_addr, _size);
  }
  return RTNORM;
}

ubyte_t* CMem::GetMemPtr(uword_t _addr)
{
  if(_addr >= MemSize)
    errmsg(__MEM_H, ERR_ACCESSOUTOFMEM, "");
  printf("\nMemory PTR M(%08lx): %p\n", _addr, & Mem[_addr]);
  return & Mem[_addr];
}

void CMem::Load(char* _filename)
{
  FILE* fp;
  ubyte_t tmpFileHeader [ sizeof(fheader_t) ];

  fheader_t fileHeader;        // cabecalho do arquivo carregado

  if((fp = fopen(_filename, "rb")) == 0) {
    sprintf(errtile, "(=%s)", _filename);
    errmsg(__MEM_H, ERR_CANTOPENFILE, errtile);
  }

  if(fread(& tmpFileHeader, sizeof(fheader_t), 1, fp) < 1) {
    sprintf(errtile, "(=%s)", _filename);
    errmsg(__MEM_H, ERR_FILETRUNCATED, errtile);
  }

#ifdef BIGENDIAN
  fileHeader.TextSize = LB_WORD( (* (uword_t*) (& tmpFileHeader[OF_TEXTSIZE])) );
  fileHeader.DataSize = LB_WORD( (* (uword_t*) (& tmpFileHeader[OF_DATASIZE])) );
  fileHeader.EntryPoint = LB_WORD( (* (uword_t*) (& tmpFileHeader[OF_ENTRYPTR])) );
#else
  fileHeader.TextSize = (* (uword_t*) (& tmpFileHeader[OF_TEXTSIZE]));
  fileHeader.DataSize = (* (uword_t*) (& tmpFileHeader[OF_DATASIZE]));
  fileHeader.EntryPoint = (* (uword_t*) (& tmpFileHeader[OF_ENTRYPTR]));
#endif

#if(DEBUG == 0)
  printf("\nMEM::FILEHEADER_TEXTSIZE(%08lx %lu)", fileHeader.TextSize, fileHeader.TextSize);
  printf("\nMEM::FILEHEADER_DATASIZE(%08lx %lu)", fileHeader.DataSize, fileHeader.DataSize);
  printf("\nMEM::FILEHEADER_ENTRYPTR(%08lx)", fileHeader.EntryPoint);
#endif

  ProgSize = (fileHeader.TextSize - sizeof(fheader_t)) + fileHeader.DataSize;
  EntryPtr = fileHeader.EntryPoint;

  if((ProgSize + EntryPtr) - 1 > MemSize) {
    sprintf(errtile, "(=%s)", _filename);
    errmsg(__MEM_H, ERR_LOADOUTOFMEMORY, errtile);
  }

  while(fread(& Mem[EntryPtr], sizeof(ubyte_t), ProgSize, fp) < ProgSize) {
    sprintf(errtile, "(=%s)", _filename);
    errmsg(__MEM_H, ERR_FILETRUNCATED, _filename);
  }

  fclose(fp);

}

void CMem::Show(uword_t _memaddr)
{
  uword_t baseaddr = (_memaddr / 16) * 16;

  printf("\nMemory");
  printf("\n======\n");

  for(uword_t ln = 0; ln < 20; ln++) {
    printf("\n%08lx   ", baseaddr);
    for(uword_t i = 0; i < 16; i++)
      if((baseaddr + i) < MemSize) {
	printf("%02x ", (int) Mem[baseaddr + i]);
	if(((i + 1) % 4) == 0)
	  printf("  ");
      }
    baseaddr += 16;
  }
  printf("\n");
}

void CMem::ShowProg(uword_t _baseaddr)
{
  uword_t inst, addr;
  CAsm it;

  str_t s;

  addr = (_baseaddr / 4) * 4;

  printf("\nProgram");
  printf("\n=======\n");
  
  for(uword_t ln = 0; ln < 24; ln++) {
    Read(addr, (ubyte_t*) & inst, sizeof(uword_t));

    it.Set(addr, inst);
    it.Get(s);

    if(ln == 0)
      printf("\n ==> %08lx  %08lx  %s", addr, inst, s);
    else
      printf("\n     %08lx  %08lx  %s", addr, inst, s);

    addr += 4;
  }
  printf("\n");
}
