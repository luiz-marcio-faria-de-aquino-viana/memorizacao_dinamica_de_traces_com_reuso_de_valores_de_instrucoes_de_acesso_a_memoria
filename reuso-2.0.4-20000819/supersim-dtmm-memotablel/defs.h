
/*
 * defs.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#ifndef __DEFS_H
#define __DEFS_H 100

#define DEBUG 22           /* 23=desabilitado */

/* #define OLDGCC 1 */

/* #define NO_TRACE_REUSE  1 */

/* definicao do numero de instrucoes limite para apresentacao estatistica
 */
#define STAT_IC_STEP_LIMIT 100000                /* padrao = 10000000 */

/* definicao do numero maximo de traps permitida no modo de depuracao
 */
#define MAX_TRAPS  100000

/* definicao do valor maximo para apresentacao do numero de sequencia
 */
#define MAX_SEQNUM 10000             // com o objetivo de facilitar a depuracao

/* definicao do formato de dados que e tratado pela maquina hospedeira
 */
#define BIGENDIAN 1      // se habilitado maquina e big senao e little

/* definicao de macros para conversao entre little-endian e big-endian
 */
#define LB_HWORD(x)  ( ((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF) )
#define LB_WORD(x)   ( ((x << 24) & 0xFF000000L) | ((x << 8) & 0x00FF0000L) | ((x >> 8) & 0x0000FF00L) | ((x >> 24) & 0x000000FFL) )
#define LB_DWORD(x)  ( ((x << 56) & 0xFF00000000000000LL) | ((x << 40) & 0x00FF000000000000LL) | ((x << 24) & 0x0000FF0000000000LL) | ((x << 8) & 0x000000FF00000000LL) | ((x >> 8) & 0x00000000FF000000LL) | ((x >> 24) & 0x0000000000FF0000LL) | ((x >> 40) & 0x000000000000FF00LL) | ((x >> 56) & 0x00000000000000FFLL) )

/* definicao da extensao do arquivo de trace
 */
#define ITRACE_FILE_EXT ".i.log"
#define BTRACE_FILE_EXT ".b.log"
#define STRACE_FILE_EXT ".s.log.gz"

/* definicao do programa de compressao de dados
 */
#define COMPRESS  "gzip"

/* definicao do tamanho do campo para representacao dos mneumonicos
 */
#define MNMONIC_LEN 25

/* definicao das constantes basicas verdadeiro e falso
 */
#define FALSE 0
#define TRUE  1

/* definicao das constantes de retorno das funcoes
 */
#define RTERR  0
#define RTNORM 1

/* definicao do tamanho maximo de uma cadeia
 */
#define STRSZ 256

/* definicao do numero de entradas no buffer de trace simsparc
 */
#define STRACE_BUFSZ 4096


/* definicao das opcoes de entrada de usuario disponiveis
 */

/* macro para transformar letras minusculas em maiusculas */

#define UPPER(c) (((c >= 'a') && (c <= 'z')) ? c - 'a' + 'A' : c)

/* definicao da macro de comparacao de bits
 */
#define BITTEST(A, B) ((A & B) != 0)

/* opcoes do menu principal */

#define OPT_QUIT        0
#define OPT_LOAD        1
#define OPT_ARGS        2
#define OPT_RESET       3
#define OPT_RUN         4
#define OPT_RUNN        5
#define OPT_STEP        6
#define OPT_SHOW        7
#define OPT_WATCH       8
#define OPT_REDISPLAY   9
#define OPT_BREAKPT    10
#define OPT_TRACE      11
#define OPT_RSTSTAT    12
#define OPT_DTMCTRL    13

/* opcoes do submenu de visualizacao */

#define OPT_SHOW_DISABLE   0
#define OPT_SHOW_PROGRAM   1
#define OPT_SHOW_ARGS      2
#define OPT_SHOW_MEM       3
#define OPT_SHOW_BTB       4
#define OPT_SHOW_FETCH     5
#define OPT_SHOW_IQUEUE    6
#define OPT_SHOW_DISQ      7
#define OPT_SHOW_DBDISQ    8
#define OPT_SHOW_IREG      9
#define OPT_SHOW_SREG     10
#define OPT_SHOW_RS       11
#define OPT_SHOW_IU       12
#define OPT_SHOW_LSU      13
#define OPT_SHOW_BPU      14
#define OPT_SHOW_MAQ      15
#define OPT_SHOW_ROB      16
#define OPT_SHOW_CTRACE   17
#define OPT_SHOW_STAT     18
#define OPT_SHOW_DTM      19

/* opcoes do submenu show program */

#define OPT_SHOW_PROGRAM_BASEADDR 0
#define OPT_SHOW_PROGRAM_FOLLOWPC 1

/* opcoes do submenu show rs */

#define OPT_SHOW_RS_DISABLE 0
#define OPT_SHOW_RS_IU      1
#define OPT_SHOW_RS_BPU     2
#define OPT_SHOW_RS_LSU     3

/* opcoes do submenu break pt */

#define OPT_BREAKPT_DISABLE 0
#define OPT_BREAKPT_PROGRAM 1
#define OPT_BREAKPT_MEMORY  2
#define OPT_BREAKPT_DTM     3

/* opcoes do submenu show dtm */

#define OPT_SHOW_DTM_DISABLE 0
#define OPT_SHOW_DTM_G       1
#define OPT_SHOW_DTM_T       2
#define OPT_SHOW_DTM_L       3

/* opcoes do submenu show ireg */

#define OPT_SHOW_IREG_ALL   0
#define OPT_SHOW_IREG_BUSY  1

/* opcoes do submenu show ireg */

#define OPT_SHOW_SREG_ALL   0
#define OPT_SHOW_SREG_BUSY  1

/* opcoes do submenu trace */

#define OPT_TRACE_DISABLE          0
#define OPT_TRACE_INSTRUCTIONS     1
#define OPT_TRACE_BASICBLOCKS      2
#define OPT_TRACE_SIMSPARC         3

/* opcoes do submenu dtm */

#define OPT_DTMCTRL_DISABLE     0
#define OPT_DTMCTRL_DTM         1
#define OPT_DTMCTRL_DTMM        2

/* definicao do resultado de operacoes de desvio (tomado/nao tomado)
 */
#define NTAKEN  0
#define TAKEN   1

/* definicao da macro de extensao de sinal
 */
#define SIGN_EXTEND(X, MASK) ( !(X & MASK) ? X : X | MASK )

/* definicao dos possiveis resultados obtidos nos acessos a btb
 */
#define MISS 0
#define HIT  1

/* definicao dos parametros da btb
 */
#define BTB_NSETS       128      // numero de conjuntos na tabela
#define BTB_NENTRIES      8      // numero de entradas por conjunto

#define BTB_SATMAX        4      // valor maximo para o contador saturado
#define BTB_SATLIM        2      // linha limite tomada como base na previsao

#define BTB_SATINI(PRED)  ((PRED == TAKEN) ? BTB_SATLIM : BTB_SATLIM - 1)

/* definicao dos parametros basicos de pilha e argumentos
 */
#define INISP               0x8000000

#define ARGPTR              0x8000400
#define ARGNUM              16
#define ARGLEN              64

/* definicao dos parametros da memoria principal em bytes
 */
#define MEM_SIZE            0x8001000

/* definicao dos parametros do estagio de busca
 */
#define FETCH_WIDTH                (4+1)    // efetivos + reserva (delay slot)

/* definicao dos parametros da fila de instrucoes
 */
#define IQUEUE_SIZE                4

/* definicao dos parametros da fila de despacho
 */
#define DISQ_SIZE                  4

/* definicao dos parametros da fila de instrucoes dependentes de desvio
 */
#define DBDISQ_SIZE                4

/* definicao dos parametros da unidade de decodificacao
 */
#define DECODE_WIDTH               4

/* definicao dos parametros da unidade de despacho
 */
#define DISPATCH_WIDTH             4

/* definicao dos parametros da unidade de complete
 */
#define COMPLETE_WIDTH             4

/* definicao dos parametros da unidade de write-back
 */
#define WB_WIDTH                   4

/* definicao dos parametros do bancos de registradores de proposito gerais
 */
#define IREG_NGREGS                8
#define IREG_NUMWIN               32
#define IREG_WINSIZE              24
#define IREG_WINOFFSET            16

#define IREG_GETABSADDR(cwp,r)    ((r < IREG_NGREGS) ? r : (IREG_WINOFFSET * cwp) + r)

/* definicao dos parametros das filas das unidades de reserva
 */
#define RSIU_SIZE                  16      // unidade de inteiros
#define RSBPU_SIZE                 16      // unidade de desvios
#define RSLSU_SIZE                 16      // unidade de acesso a memoria

/* definicao dos parametros da fila de acesso a memoria
 */
#define MAQ_SIZE                   16

/* definicao dos parametros do buffer de reordenacao
 */
#define ROB_SIZE                   16

/* definicao dos parametros dos traces
 */
#define EXECTRACE_SIZE            12
#define DISPTRACE_SIZE            12


/* numero de unidades funcionais de cada tipo simuladas
 */
#define NUM_IU                    3
#define NUM_LSU                   2
#define NUM_BPU                   2


/* definicao dos parametros do consumidor
 */

/* taxa de consumo das instrucoes */

#define CONSUMER_MINRATE 1.0
#define CONSUMER_MAXRATE 8.0

#define CONSUMER_RATE  ( CONSUMER_MINRATE + ((CONSUMER_MAXRATE - CONSUMER_MINRATE + 1) * rand() / (RAND_MAX + 1.0)) )

/* taxa de acerto das previsoes */

#define PREDICTION_MATCH_RATE 0.1

#define PREDICTION_MATCH ( ((rand() / (RAND_MAX + 1.0)) > PREDICTION_MATCH_RATE) ? TRUE : FALSE )

/* definicao dos metodos de requerimento dos registradores
 */
#define REQ_NONE    0
#define REQ_SOURCE  1 
#define REQ_TARGET  2

/* definicao dos tipos de acesso a memoria
 */

/* acessos de leitura */

#define RD_NONE     0x00
#define RD_BYTE     0x01
#define RD_UBYTE    0x02
#define RD_HWORD    0x04
#define RD_UHWORD   0x08
#define RD_WORD     0x10
#define RD_DWORD    0x40

/* acessos de escrita */

#define WR_NONE     0x00
#define WR_BYTE     0x01
#define WR_HWORD    0x04
#define WR_WORD     0x10
#define WR_DWORD    0x40

/* definicao dos tipos de instrucoes de desvio
 */
#define __NIL          0
#define __BA           1
#define __BA_ANNUL     2
#define __BN           3
#define __BN_ANNUL     4
#define __BICC         5
#define __BICC_ANNUL   6
#define __FBFCC        7
#define __FBFCC_ANNUL  8
#define __CBCC         9
#define __CBCC_ANNUL  10
#define __CALL        11
#define __JUMPL       12
#define __RETT        13

/* BA e BN bit teste */

#define T_BA(x)       ((x & 0xFFC00000) == 0x10800000)
#define T_BA_ANNUL(x) ((x & 0xFFC00000) == 0x30800000)

#define T_BN(x)       ((x & 0xFFC00000) == 0x00800000)
#define T_BN_ANNUL(x) ((x & 0xFFC00000) == 0x20800000)

/* Bicc, FBfcc e CBcc bit teste */

#define T_BICC(x)  ((x & 0xE1C00000) == 0x00800000)
#define T_BICC_ANNUL(x)  ((x & 0xE1C00000) == 0x20800000)

#define T_FBFCC(x) ((x & 0xE1C00000) == 0x01800000)
#define T_FBFCC_ANNUL(x) ((x & 0xE1C00000) == 0x21800000)

#define T_CBCC(x)  ((x & 0xE1C00000) == 0x01C00000)
#define T_CBCC_ANNUL(x)  ((x & 0xE1C00000) == 0x21C00000)

/* CALL bit teste */

#define T_CALL(x)  ((x & 0xC0000000) == 0x40000000)

/* JUMPL e RETT bit teste */

#define T_JUMPL(x) ((x & 0xC1F80000) == 0x81C00000)
#define T_RETT(x)  ((x & 0xC1F80000) == 0x81C80000)

/* definicao das macros de acesso aos codigos de condicao
 */
#define ICC_C(X) ((X >> 20) & 0x01)
#define ICC_V(X) ((X >> 21) & 0x01)
#define ICC_Z(X) ((X >> 22) & 0x01)
#define ICC_N(X) ((X >> 23) & 0x01)

#define ICC(N, Z, V, C) ((N << 23) | (Z << 22) | (V << 21) | (C << 20))

/* definicao das macros de verificacao de sinais
 */
#define BSIGN(X) ((X >>  7) & (byte_t)  0x01)
#define HSIGN(X) ((X >> 15) & (hword_t) 0x01)
#define WSIGN(X) ((X >> 31) & (word_t)  0x01)
#define DSIGN(X) ((X >> 63) & (dword_t) 0x01)

/* definicao dos estados de montagem dos traces de bloco basicos (depuracao)
 */
#define BB_BEGIN    0
#define BB_BODY     1
#define BB_END      2

/* definicao das mascaras de controle dos traces
 */
#define WTRACE_INSTRUCTIONS  1
#define WTRACE_BASICBLOCKS   2
#define WTRACE_SIMSPARC      4

/* definicao dos tipos de registradores alvos do contexto da instrucao reusada
 */
#define CTX_NONE         0
#define CTX_PSR          1
#define CTX_Y            2
#define CTX_GPR          3

/* definicao dos estados de operacao do DTM na arquitetura
 */
#define DTM_DISABLED            0
#define DTM_ENABLED             1
#define DTM_WITH_MEM_ENABLED    2

/* definicao dos parametros do DTM
 */

/* definicao dos tipos de identificacao dinamica das classes
 */
#define MEMOTABLEG              1
#define MEMOTABLEL              2
#define MEMOTABLET              4

/* MEMO_TABLE_G */
#define MEMOTABLEG_SIZE 4672

#define MEMOTABLEG_NSLOTS 1024

/* MEMO_TABLE_T */
#define MEMOTABLET_SIZE  512

#define MEMOTABLET_NSLOTS 128

#define BRC_NUM 10

#define LDST_NUM 1

#define IC_NUM 7

#define OC_NUM 7

/* MEMO_TABLE_L */
#define MEMOTABLEL_SIZE 512

#define MEMOTABLEL_NSLOTS 128

/* definicao dos indicadores de operacoes de leitura/escrita a memoria
 */
#define IOM_NONE        0x00
#define IOM_LOAD        0x01
#define IOM_STORE       0x02

/* definicao das faixas de contadores de instrucoes
 */

/* tipos de faixas */
#define IC_NORMAL       0
#define IC_INST_REUSE   1
#define IC_TRACE        2
#define IC_TRACE_REUSE  3

/* definicao das faixas de contadores de tamanhos de traces
 */

/* tamanho maximo de traces */
#define TSZ_MAX         20

/* tipos de faixas de traces */
#define TSZ_TRACE        0
#define TSZ_TRACE_REUSE  1
#define TSZ_NOT_FORMED   2

/* definicao das faixas de contadores de finalizacao dos traces
 */
#define TFN_OVERFLOW    0
#define TFN_DELAYINST   1
#define TFN_NVALID      2
#define TFN_NREDUNDANT  3
#define TFN_TRACE       4
#define TFN_LOADSTORE   5

/* definicao da classificacao de dependencia dos dados
 */
#define TLD_NOLOAD         0
#define TLD_INST_LOAD      1
#define TLD_REUSED_LOAD    2
#define TLD_TRACE_LOAD     3

#endif

