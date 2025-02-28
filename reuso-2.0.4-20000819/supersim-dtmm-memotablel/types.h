
/*
 * types.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/5/99
 */

#ifndef __TYPES_H
#define __TYPES_H 103

/* definicao do tipo de dado cadeia
 */
typedef char str_t [ STRSZ ];

/* definicao do tipo de dados utilizados pela aplicacao
 */
typedef char bool_t;

typedef char               byte_t;
typedef unsigned char      ubyte_t;
typedef short              hword_t;
typedef unsigned short     uhword_t;
typedef long               word_t;
typedef unsigned long      uword_t;
typedef long long          dword_t;
typedef unsigned long long udword_t;

/* definicao do tipo de dados buffer da aplicacao
 */
typedef union {
  byte_t   b;
  ubyte_t  ub;
  hword_t  h;
  uhword_t uh;
  word_t   w;
  dword_t  d;
} databuf_t;

typedef struct {
  ubyte_t n;
  ubyte_t z;
  ubyte_t v;
  ubyte_t c;
} icc_t;

/* definicao do cabecalho dos arquivos a.out
 */
typedef struct {
  ubyte_t  DynToolVer;
  ubyte_t  MachineType;
  uhword_t MagicNumber;
  uword_t  TextSize;
  uword_t  DataSize;
  uword_t  BssSize;
  uword_t  SymbolTabSize;
  uword_t  EntryPoint;
  uword_t  TextRelSize;
  uword_t  DataRelSize; 
} fheader_t;

/* definicao da posicao dos principais dados do cabecalho dos arquivos a.out
 */
#define OF_TEXTSIZE   4
#define OF_DATASIZE   8
#define OF_ENTRYPTR  20

/* definicao da estrutura o_stat
 */

/* SVID stat struct (SVr4.x) */

typedef struct statbuf {
  hword_t    _dev;
  uword_t    _ino;
  uhword_t   _mode;
  hword_t    _nlink;
  uhword_t   _uid;
  uhword_t   _gid;
  hword_t    _rdev;
  word_t     _size;
  word_t     _atime;
  word_t     _spare1;
  word_t     _mtime;
  word_t     _spare2;
  word_t     _ctime;
  word_t     _spare3;
  word_t     _blksize;
  word_t     _blocks;
  word_t     _spare4[2];
} statbuf_t;

/* definicao da entrada do buffer para trace simsparc
 */
typedef struct {
  uword_t addr;
  ubyte_t rd;
  uword_t res;
  uword_t n;
} vtbuf_t;

/* definicao da estrutura estatistica basica
 */
typedef struct {
  /* instrucoes de desvios */

  long IC_call;

  long IC_bicc;

  long IC_jmpl;

  /* chamadas ao sistema operacional */

  long IC_ticc;

  /* instrucoes de acesso a memoria */

  long IC_load;

  long IC_store;

  /* instrucoes logicas e aritimeticas */

  long IC_arithmetic;

  long IC_logic;

  /* instrucoes de multiplicacao */

  long IC_mult;

  /* instrucoes de save/restore */

  long IC_save;

  long IC_restore;

  /* instrucao sethi */

  long IC_sethi;

  /* outras instrucoes */

  long IC_others;

  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces

  long TFN_traces;   // TFN_OVERFLOW = 'overflow'
                     // TFN_DELAYINST = 'delay instruction'
                     // TFN_NVALID = 'instrucao nao valida'
                     // TFN_NREDUNDANT = 'instrucao nao redundante'
                     // TFN_TRACE = 'trace redundante'
                     // TFN_LOADSTORE = 'instrucao de acesso a memoria'

} statistics_t;

#endif

