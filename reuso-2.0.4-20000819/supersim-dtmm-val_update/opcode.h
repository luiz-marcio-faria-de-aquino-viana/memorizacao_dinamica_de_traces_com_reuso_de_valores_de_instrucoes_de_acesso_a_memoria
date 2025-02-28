
/*
 * opcode.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/10/99
 */

#ifndef __OPCODE_H
#define __OPCODE_H 104


/* definicao dos endereco dos atributos */

#define R0   0
#define R1   1
#define R2   2
#define R3   3
#define R4   4
#define R5   5
#define R6   6
#define R7   7
#define R8   8
#define R9   9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15
#define R16 16
#define R17 17
#define R18 18
#define R19 19
#define R20 20
#define R21 21
#define R22 22
#define R23 23
#define R24 24
#define R25 25
#define R26 26
#define R27 27
#define R28 28
#define R29 29
#define R30 30
#define R31 31

/* registradores globais
 */
#define G0  R0
#define G1  R1
#define G2  R2
#define G3  R3
#define G4  R4
#define G5  R5
#define G6  R6
#define G7  R7

/* registradores de saida
 */
#define O0  R8
#define O1  R9
#define O2  R10
#define O3  R11
#define O4  R12
#define O5  R13
#define O6  R14
#define O7  R15

/* registradores locais
 */
#define L0  R16
#define L1  R17
#define L2  R18
#define L3  R19
#define L4  R20
#define L5  R21
#define L6  R22
#define L7  R23

/* registradores de entrada
 */
#define I0  R24
#define I1  R25
#define I2  R26
#define I3  R27
#define I4  R28
#define I5  R29
#define I6  R30
#define I7  R31

/* pseudo registradores
 */
#define	SP  R14
#define	FP  R30
#define	RET R31

/* definicao dos identificadores das unidades de execucao
 */
#define NONE 0
#define IU   1
#define BPU  2
#define LSU  3

/* definicao dos codigos de operacao das instrucoes
 */

/* definicao dos valores assumidos pelo campo op */

#define OP_VALUE_0 0         // formato 2 (Bicc, FBfcc, CBccc, SETHI)
#define OP_VALUE_1 1         // formato 1 (Call)
#define OP_VALUE_2 2         // formato 3
#define OP_VALUE_3 3         // formato 3 (load-store)

#define CALL (OP_VALUE_1 << 9)


/* definicao dos valores assumidos pelo campo op2 (op=0) */

#define OP2_VALUE_0 0        // UNIMP
#define OP2_VALUE_1 1        // invalid
#define OP2_VALUE_2 2        // Bicc
#define OP2_VALUE_3 3        // invalid
#define OP2_VALUE_4 4        // SETHI
#define OP2_VALUE_5 5        // invalid
#define OP2_VALUE_6 6        // FBfcc
#define OP2_VALUE_7 7        // CBccc

#define UNIMP (OP2_VALUE_0 << 6)
#define BICC  (OP2_VALUE_2 << 6)
#define SETHI (OP2_VALUE_4 << 6)
#define FBFCC (OP2_VALUE_6 << 6)
#define CBCCC (OP2_VALUE_7 << 6)


/* definicao dos valores assumidos pelo campo op3 (op=3) */

/* load integer instructions */

#define LDSB      0x09
#define LDSBA     0x19
#define LDSH      0x0A
#define LDSHA     0x1A
#define LDUB      0x01
#define LDUBA     0x11
#define LDUH      0x02
#define LDUHA     0x12
#define LD        0x00
#define LDA       0x10
#define LDD       0x03
#define LDDA      0x13

/* load floating-point instructions */

#define LDF       0x20
#define LDDF      0x23
#define LDFSR     0x21

/* load coprocessor instructions */

#define LDC       0x30
#define LDDC      0x33
#define LDCSR     0x31

/* store integer instructions */

#define STB       0x05
#define STBA      0x15
#define STH       0x06
#define STHA      0x16
#define ST        0x04
#define STA       0x14
#define STD       0x07
#define STDA      0x17

/* store floating-point instruction */

#define STF       0x24
#define STDF      0x27
#define STFSR     0x25
#define STDFQ     0x26

/* store coprocessor instructions */

#define STC       0x34
#define STDC      0x37
#define STCSR     0x35
#define STDCQ     0x36

/* atomic load-store unsigned byte instructions */

#define LDSTUB    0x0D
#define LDSTUBA   0x1D

/* swap register with memory */

#define SWAP      0x0F
#define SWAPA     0x1F

/* definicao dos valores assumidos pelo campo op3 (op=2) */

/* add instructions */

#define ADD       0x00
#define ADDCC     0x10
#define ADDX      0x08
#define ADDXCC    0x18

/* tagged add instructions */

#define TADDCC    0x20
#define TADDCCTV  0x22

/* subtract instructions */

#define SUB       0x04
#define SUBCC     0x14
#define SUBX      0x0C
#define SUBXCC    0x1C

/* tagged subtract instructions */

#define TSUBCC    0x21
#define TSUBCCTV  0x23

/* multiply step instruction */ 

#define MULSCC    0x24

/* logical instructions */ 

#define AND       0x01
#define ANDCC     0x11
#define ANDN      0x05
#define ANDNCC    0x15
#define OR        0x02
#define ORCC      0x12
#define ORN       0x06
#define ORNCC     0x16
#define XOR       0x03
#define XORCC     0x13
#define XNOR      0x07
#define XNORCC    0x17

/* shift instructions */

#define SLL       0x25
#define SRL       0x26
#define SRA       0x27

/* SAVE abd RESTORE instructions */ 

#define SAVE      0x3C
#define RESTORE   0x3D

/* jump and link instruction */

#define JMPL     0x38

/* return from trap instruction */

#define RETT      0x39

/* trap on integer condition instruction */

#define TICC      0x3A

/* read state register instructions */

#define RDY       0x28
#define RDPSR     0x29
#define RDWIM     0x2A
#define RDTBR     0x2B

/* write state register instructions */

#define WRY       0x30
#define WRPSR     0x31
#define WRWIM     0x32
#define WRTBR     0x33

/* instruction cache flush instruction */

#define IFLUSH    0x3B

/* floating-point operate (FPop) instructions */

#define FPop1     0x34
#define FPop2     0x35

/* coprocessor operate instructions */

#define CPop1     0x36
#define CPop2     0x37

/* definicao dos valores assumidos pelo campo cond (Bicc, FBfcc, CBccc, Ticc) */

#define BA        0x08
#define BN        0x00
#define BNE       0x09
#define BE        0x01
#define BG        0x0A
#define BLE       0x02
#define BGE       0x0B
#define BL        0x03
#define BGU       0x0C
#define BLEU      0x04
#define BCC       0x0D
#define BCS       0x05
#define BPOS      0x0E
#define BNEG      0x06
#define BVC       0x0F
#define BVS       0x07

/* floating-point branch on condition instructions */

#define FBA       0x08
#define FBN       0x00
#define FBU       0x07
#define FBG       0x06
#define FBUG      0x05
#define FBL       0x04
#define FBUL      0x03
#define FBLG      0x02
#define FBNE      0x01
#define FBE       0x09
#define FBUE      0x0A
#define FBGE      0x0B
#define FBUGE     0x0C
#define FBLE      0x0D
#define FBULE     0x0E
#define FBO       0x0F

/* coprocessor branch on condition instructions */

#define CBA       0x08
#define CBN       0x00
#define CB3       0x07
#define CB2       0x06
#define CB23      0x05
#define CB1       0x04
#define CB13      0x03
#define CB12      0x02
#define CB123     0x01
#define CB0       0x09
#define CB03      0x0A
#define CB02      0x0B
#define CB023     0x0C
#define CB01      0x0D
#define CB013     0x0E
#define CB012     0x0F

/* trap on integer condition instruction */

#define TA        0x08
#define TN        0x00
#define TNE       0x09
#define TE        0x01
#define TG        0x0A
#define TLE       0x02
#define TGE       0x0B
#define TL        0x03
#define TGU       0x0C
#define TLEU      0x04
#define TCC       0x0D
#define TCS       0x05
#define TPOS      0x0E
#define TNEG      0x06
#define TVC       0x0F
#define TVS       0x07

/* definicao dos valores assumidos pelo campo opf (FPop1, FPop2) */

/* convert integer to floating-point instructions */

#define FITOS     0x0C4
#define FITOD     0x0C8
#define FITOX     0x0CC

/* convert floating-point to integer */

#define FSTOI     0x0D1
#define FDTOI     0x0D2
#define FXTOI     0x0D3

/* convert between floating-point formats instructions */

#define FSTOD     0x0C9
#define FSTOX     0x0CD
#define FDTOS     0x0C6
#define FDTOX     0x0CE
#define FXTOS     0x0C7
#define FXTOD     0x0CB

/* floating-point move instructions */

#define FMOVS     0x001
#define FNEGS     0x005
#define FABSS     0x009

/* floating-point square root instructions */

#define FSQRTS    0x029
#define FSQRTD    0x02A
#define FSQRTX    0x02B

/* floating-point add and subtract instructions */

#define FADDS     0x041
#define FADDD     0x042
#define FADDX     0x043
#define FSUBS     0x045
#define FSUBD     0x046
#define FSUBX     0x047

/* floating-point multiply and divide instructions */

#define FMULS     0x049
#define FMULD     0x04A
#define FMULX     0x04B
#define FDIVS     0x04D
#define FDIVD     0x04E
#define FDIVX     0x04F

/* floating-point compare instructions */

#define FCMPS     0x051
#define FCMPD     0x052
#define FCMPX     0x053
#define FCMPES    0x055
#define FCMPED    0x056
#define FCMPEX    0x057

#endif
