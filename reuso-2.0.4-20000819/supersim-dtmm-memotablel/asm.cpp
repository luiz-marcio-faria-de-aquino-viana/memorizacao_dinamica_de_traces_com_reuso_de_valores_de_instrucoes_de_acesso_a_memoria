
/*
 * asm.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/5/99
 */

#include<stdio.h>
#include<string.h>
#include"defs.h"
#include"types.h"
#include"opcode.h"
#include"error.h"
#include"asm.h"

///////////////////////////////////////////////////////
///// CAsm

void CAsm::Set(uword_t _addr, uword_t _inst)
{
  uword_t op;
  uword_t disp30;
  uword_t rd;
  uword_t op2;
  uword_t imm22;
  uword_t a;
  uword_t cond;
  uword_t disp22;
  uword_t op3;
  uword_t rs1;
  uword_t i;
  uword_t asi;
  uword_t rs2;
  uword_t simm13;
  //uword_t opf;

  const char *tmp = 0;

  op     = (_inst >> 30) & 0x03L;
  disp30 = SIGN_EXTEND((_inst & 0x3FFFFFFFL), 0xE0000000L);
  rd     = (_inst >> 25) & 0x1FL;
  op2    = (_inst >> 22) & 0x07L;
  imm22  = _inst & 0x003FFFFFL;
  a      = (_inst >> 29) & 0x01L;
  cond   = (_inst >> 25) & 0x0FL;
  disp22 = SIGN_EXTEND((_inst & 0x003FFFFFL), 0xFFE00000L);
  op3    = (_inst >> 19) & 0x3FL;
  rs1    = (_inst >> 14) & 0x1FL;
  i      = (_inst >> 13) & 0x01L;
  asi    = (_inst >>  5) & 0xFFL;
  rs2    = _inst & 0x0000001FL;
  simm13 = SIGN_EXTEND((_inst & 0x00001FFFL), 0xFFFFF000L);
  //opf    = (_inst >>  5) & 0x1FFL;

  switch(op) 
  {
        case OP_VALUE_1:
        {
                /* instrucao do formato 1 (CALL) */
                sprintf(buf, "call %08lx", _addr + (disp30 << 2));
                break;
        }
        case OP_VALUE_0:
        {
                /* instrucao do formato 2 (UNIMP, Bicc, FBfcc, CBccc, SETHI) */
                switch(op2) 
                {
                        case OP2_VALUE_0:
                        {
                                /* instrucao UNIMP */
                                sprintf(buf, "unimp %08lx", imm22);
                                break;
                        }
                        case OP2_VALUE_2:
                        {
                              /* instrucao Bicc */
                              if(cond == BA) tmp = "ba";
                              else if(cond == BN) tmp = "bn";
                              else if(cond == BNE) tmp = "bne";
                              else if(cond == BE) tmp = "be";
                              else if(cond == BG) tmp = "bg";
                              else if(cond == BLE) tmp = "ble";
                              else if(cond == BGE) tmp = "bge";
                              else if(cond == BL) tmp = "bl";
                              else if(cond == BGU) tmp = "bgu";
                              else if(cond == BLEU) tmp = "bleu";
                              else if(cond == BCC) tmp = "bcc";
                              else if(cond == BCS) tmp = "bcs";
                              else if(cond == BPOS) tmp = "bpos";
                              else if(cond == BNEG) tmp = "bneg";
                              else if(cond == BVC) tmp = "bvc";
                              else if(cond == BVS) tmp = "bvs";
                              if(a == 1)
	                        sprintf(buf, "%s,a %08lx", tmp, _addr + (disp22 << 2));
                              else
	                        sprintf(buf, "%s %08lx", tmp, _addr + (disp22 << 2));
                              break;
                        }
                        case OP2_VALUE_4:
                        {
                                /* instrucao SETHI */
                                sprintf(buf, "sethi %lx, r%ld", imm22, rd);
                                break;
                        }
                        case OP2_VALUE_6:
                        {
                              /* branch on floating-point condition */
                              if(cond == FBA) tmp = "fba";
                              else if(cond == FBN) tmp = "fbn";
                              else if(cond == FBU) tmp = "fbu";
                              else if(cond == FBG) tmp = "fbg";
                              else if(cond == FBUG) tmp = "fbug";
                              else if(cond == FBL) tmp = "fbl";
                              else if(cond == FBUL) tmp = "fbul";
                              else if(cond == FBLG) tmp = "fblg";
                              else if(cond == FBNE) tmp = "fbne";
                              else if(cond == FBE) tmp = "fbe";
                              else if(cond == FBUE) tmp = "fbue";
                              else if(cond == FBGE) tmp = "fbge";
                              else if(cond == FBUGE) tmp = "fbuge";
                              else if(cond == FBLE) tmp = "fble";
                              else if(cond == FBULE) tmp = "fbule";
                              else if(cond == FBO) tmp = "fbo";
                              if(a == 1)
	                        sprintf(buf, "%s,a %08lx", tmp, _addr + (disp22 << 2));
                              else
	                        sprintf(buf, "%s %08lx", tmp, _addr + (disp22 << 2));
                              break;
                        }
                        case OP2_VALUE_7:
                        {
                              /* branch on coprocessor condition */
                              if(cond == CBA) tmp = "cba";
                              else if(cond == CBN) tmp = "cbn";
                              else if(cond == CB3) tmp = "cb3";
                              else if(cond == CB2) tmp = "cb2";
                              else if(cond == CB23) tmp = "cb23";
                              else if(cond == CB1) tmp = "cb1";
                              else if(cond == CB13) tmp = "cb13";
                              else if(cond == CB12) tmp = "cb12";
                              else if(cond == CB123) tmp = "cb123";
                              else if(cond == CB0) tmp = "cb0";
                              else if(cond == CB03) tmp = "cb03";
                              else if(cond == CB02) tmp = "cb02";
                              else if(cond == CB023) tmp = "cb023";
                              else if(cond == CB01) tmp = "cb01";
                              else if(cond == CB013) tmp = "cb013";
                              else if(cond == CB012) tmp = "cb012";
                              if(a == 1)
	                        sprintf(buf, "%s,a %08lx", tmp, _addr + (disp22 << 2));
                              else
	                        sprintf(buf, "%s %08lx", tmp, _addr + (disp22 << 2));
                              break;
                        }
                        case OP2_VALUE_1:
                        case OP2_VALUE_3:
                        case OP2_VALUE_5:
                        {
                              sprintf(buf, "??");
                        }
                }
                break;
        }
        case OP_VALUE_3:
        {
                /* instrucao do formato 3 (Load/Store) */
                switch(op3) 
                {
                        case LDSBA:
                        {
                                if(tmp == 0) tmp = "ldsba";
                        }
                        case LDSHA:
                        {
                                if(tmp == 0) tmp = "ldsha";
                        }
                        case LDUBA:
                        {
                                if(tmp == 0) tmp = "lduba";
                        }
                        case LDUHA:
                        {
                                if(tmp == 0) tmp = "lduha";
                        }
                        case LDA:
                        {
                                if(tmp == 0) tmp = "lda";
                        }
                        case LDDA:
                        {
                                if(tmp == 0) tmp = "ldda";
                                /* load from alternate space */
                                if(i == 0)
	                          sprintf(buf, "%s r%ld + r%ld %03ld, r%ld", tmp, rs1, rs2, asi, rd);
                                else
	                          sprintf(buf, "??");
                                break;
                        }
                        case LDSB:
                        {
                                if(tmp == 0) tmp = "ldsb";
                        }
                        case LDSH:
                        {
                                if(tmp == 0) tmp = "ldsh";
                        }
                        case LDUB:
                        {
                                if(tmp == 0) tmp = "ldub";
                        }
                        case LDUH:
                        {
                                if(tmp == 0) tmp = "lduh";
                        }
                        case LD:
                        {
                                if(tmp == 0) tmp = "ld";
                        }
                        case LDD:
                        {
                                if(tmp == 0) tmp = "ldd";
                                /* load instructions */
                                if(i == 0)
	                                sprintf(buf, "%s r%ld + r%ld, r%ld", tmp, rs1, rs2, rd);
                                else
	                                sprintf(buf, "%s r%ld + %lx, r%ld", tmp, rs1, simm13, rd);
                                break;
                        }
                        case LDF:
                        {
                                if(tmp == 0) tmp = "ldf";
                        }
                        case LDDF:
                        {
                                if(tmp == 0) tmp = "lddf";
                                /* load floating-point register */
                                if(i == 0)
	                                sprintf(buf, "%s r%ld + r%ld, f%ld", tmp, rs1, rs2, rd);
                                else
	                                sprintf(buf, "%s r%ld + %lx, f%ld", tmp, rs1, simm13, rd);
                                break;
                        }
                        case LDFSR:
                        {
                                /* load floating-point state register */
                                if(i == 0)
	                                sprintf(buf, "ld r%ld + r%ld, fsr", rs1, rs2);
                                else
	                                sprintf(buf, "ld r%ld + %lx, fsr", rs1, simm13);
                                break;
                        }
                        case LDC:
                        {
                                if(tmp == 0) tmp = "ldc";
                        }
                        case LDDC:
                        {
                                if(tmp == 0) tmp = "lddc";
                                /* load coprocessor register */
                                if(i == 0)
	                                sprintf(buf, "%s r%ld + r%ld, c%ld", tmp, rs1, rs2, rd);
                                else
	                                sprintf(buf, "%s r%ld + %lx, c%ld", tmp, rs1, simm13, rd);
                                break;
                        }
                        case LDCSR:
                        {
                                /* load coprocessor state register */
                                if(i == 0)
	                                sprintf(buf, "ld r%ld + r%ld, csr", rs1, rs2);
                                else
	                                sprintf(buf, "ld r%ld + %lx, csr", rs1, simm13);
                                break;
                        }
                        case STBA:
                        {
                                if(tmp == 0) tmp = "stba";
                        }
                        case STHA:
                        {
                                if(tmp == 0) tmp = "stha";
                        }
                        case STA:
                        {
                                if(tmp == 0) tmp = "sta";
                        }
                        case STDA:
                        {
                                if(tmp == 0) tmp = "stda";
                                /* store into alternate space */
                                if(i == 0)
	                                sprintf(buf, "%s r%ld, r%ld + r%ld %03ld", tmp, rd, rs1, rs2, asi);
                                else
	                                sprintf(buf, "??");
                                break;
                        }
                        case STB:
                        {
                                if(tmp == 0) tmp = "stb";
                        }
                        case STH:
                        {
                                if(tmp == 0) tmp = "sth";
                        }
                        case ST:
                        {
                                if(tmp == 0) tmp = "st";
                        }
                        case STD:
                        {
                                if(tmp == 0) tmp = "std";
                                /* store instructions */
                                if(i == 0)
	                                sprintf(buf, "%s r%ld, r%ld + r%ld", tmp, rd, rs1, rs2);
                                else
	                                sprintf(buf, "%s r%ld, r%ld + %lx", tmp, rd, rs1, simm13);
                                break;
                        }
                        case STF:
                        {
                                if(tmp == 0) tmp = "stf";
                        }
                        case STDF:
                        {
                                if(tmp == 0) tmp = "stdf";
                                /* store floating-point instructions */
                                if(i == 0)
	                                sprintf(buf, "%s f%ld, r%ld + r%ld", tmp, rd, rs1, rs2);
                                else
	                                sprintf(buf, "%s f%ld, r%ld + %lx", tmp, rd, rs1, simm13);
                                break;
                        }
                        case STFSR:
                        {
                                /* store floating-point state register */
                                if(i == 0)
	                                sprintf(buf, "st fsr, r%ld + r%ld", rs1, rs2);
                                else
	                                sprintf(buf, "st fsr, r%ld + %lx", rs1, simm13);
                                break;
                        }
                        case STC:
                        {
                                if(tmp == 0) tmp = "stc";
                        }
                        case STDC:
                        {
                                if(tmp == 0) tmp = "stdc";
                                /* store coprocessor register */
                                if(i == 0)
	                                sprintf(buf, "%s c%ld, r%ld + r%ld", tmp, rd, rs1, rs2);
                                else
	                                sprintf(buf, "%s c%ld, r%ld + %lx", tmp, rd, rs1, simm13);
                                break;
                        }
                        case STCSR:
                        {
                                /* store coprocessor state register */
                                if(i == 0)
	                                sprintf(buf, "st csr, r%ld + r%ld", rs1, rs2);
                                else
	                                sprintf(buf, "st csr, r%ld + %lx", rs1, simm13);
                                break;
                        }
                        case LDSTUBA:
                        {
                                /* atomic load-store unsigned byte instructions from alternate space */
                                if(i == 0)
	                                sprintf(buf, "ldstuba r%ld + r%ld %03ld, r%ld", rs1, rs2, asi, rd);
                                else
	                                sprintf(buf, "??");
                                break;
                        }
                        case LDSTUB:
                        {
                                /* atomic load-store unsigned byte instructions */
                                if(i == 0)
	                                sprintf(buf, "ldstub r%ld + r%ld, r%ld", rs1, rs2, rd);
                                else
	                                sprintf(buf, "ldstub r%ld + %lx, r%ld", rs1, simm13, rd);
                                break;
                        }
                        case SWAPA:
                        {
                                /* swap register with memory from alternate space */
                                if(i == 0)
	                                sprintf(buf, "swapa r%ld + r%ld %03ld, r%ld", rs1, rs2, asi, rd);
                                else
	                                sprintf(buf, "??");
                                break;
                        }
                        case SWAP:
                        {
                                /* swap register with memory */
                                if(i == 0)
	                                sprintf(buf, "swap r%ld + r%ld, r%ld", rs1, rs2, rd);
                                else
	                                sprintf(buf, "swap r%ld + %lx, r%ld", rs1, simm13, rd);
                                break;
                        }
                        default:
                        {
                                sprintf(buf, "??");
                        }
                }
                break;
        }
        case OP_VALUE_2:
        {
                /* instrucao do formato 3 */
                switch(op3) 
                {
                        case ADD:
                        {
                                if(tmp == 0) tmp = "add";
                        }
                        case ADDCC:
                        {
                                if(tmp == 0) tmp = "addcc";
                        }
                        case ADDX:
                        {
                                if(tmp == 0) tmp = "addx";
                        }
                        case ADDXCC:
                        {
                                if(tmp == 0) tmp = "addxcc";
                        }
                        case TADDCC:
                        {
                                if(tmp == 0) tmp = "taddcc";
                        }
                        case TADDCCTV:
                        {
                                if(tmp == 0) tmp = "taddcctv";
                        }
                        case SUB:
                        {
                                if(tmp == 0) tmp = "sub";
                        }
                        case SUBCC:
                        {
                                if(tmp == 0) tmp = "subcc";
                        }
                        case SUBX:
                        {
                                if(tmp == 0) tmp = "subx";
                        }
                        case SUBXCC:
                        {
                                if(tmp == 0) tmp = "subxcc";
                        }
                        case TSUBCC:
                        {
                                if(tmp == 0) tmp = "tsubcc";
                        }
                        case TSUBCCTV:
                        {
                                if(tmp == 0) tmp = "tsubcctv";
                        }
                        case MULSCC:
                        {
                                if(tmp == 0) tmp = "mulscc";
                        }
                        case AND:
                        {
                                if(tmp == 0) tmp = "and";
                        }
                        case ANDCC:
                        {
                                if(tmp == 0) tmp = "andcc";
                        }
                        case ANDN:
                        {
                                if(tmp == 0) tmp = "andn";
                        }
                        case ANDNCC:
                        {
                                if(tmp == 0) tmp = "andncc";
                        }
                        case OR:
                        {
                                if(tmp == 0) tmp = "or";
                        }
                        case ORCC:
                        {
                                if(tmp == 0) tmp = "orcc";
                        }
                        case ORN:
                        {
                                if(tmp == 0) tmp = "orn";
                        }
                        case ORNCC:
                        {
                                if(tmp == 0) tmp = "orncc";
                        }
                        case XOR:
                        {
                                if(tmp == 0) tmp = "xor";
                        }
                        case XORCC:
                        {
                                if(tmp == 0) tmp = "xorcc";
                        }
                        case XNOR:
                        {
                                if(tmp == 0) tmp = "xnor";
                        }
                        case XNORCC:
                        {
                                if(tmp == 0) tmp = "xnorcc";
                        }
                        case SLL:
                        {
                                if(tmp == 0) tmp = "sll";
                        }
                        case SRL:
                        {
                                if(tmp == 0) tmp = "srl";
                        }
                        case SRA:
                        {
                                if(tmp == 0) tmp = "sra";
                        }
                        case SAVE:
                        {
                                if(tmp == 0) tmp = "save";
                        }
                        case RESTORE:
                        {
                                if(tmp == 0) tmp = "restore";
                                if(i == 0)
	                                sprintf(buf, "%s r%ld, r%ld, r%ld", tmp, rs1, rs2, rd);
                                else
	                                sprintf(buf, "%s r%ld, %lx, r%ld", tmp, rs1, simm13, rd);
                                break;
                        }
                        case JMPL:
                        {
                                if(i == 0)
	                                sprintf(buf, "jmpl r%ld + r%ld, r%ld", rs1, rs2, rd);
                                else
	                                sprintf(buf, "jmpl r%ld + %lx, r%ld", rs1, simm13, rd);
                                break;
                        }
                        case RETT:
                        {
                                if(i == 0)
	                                sprintf(buf, "rett r%ld + r%ld", rs1, rs2);
                                else
	                                sprintf(buf, "rett r%ld + %lx", rs1, simm13);
                                break;
                        }
                        case TICC:
                        {
                              if(cond == TA) tmp = "ta";
                              else if(cond == TN) tmp = "tn";
                              else if(cond == TNE) tmp = "tne";
                              else if(cond == TE) tmp = "te";
                              else if(cond == TG) tmp = "tg";
                              else if(cond == TLE) tmp = "tle";
                              else if(cond == TGE) tmp = "tge";
                              else if(cond == TL) tmp = "tl";
                              else if(cond == TGU) tmp = "tgu";
                              else if(cond == TLEU) tmp = "tleu";
                              else if(cond == TCC) tmp = "tcc";
                              else if(cond == TCS) tmp = "tcs";
                              else if(cond == TPOS) tmp = "tpos";
                              else if(cond == TNEG) tmp = "tneg";
                              else if(cond == TVC) tmp = "tvc";
                              else if(cond == TVS) tmp = "tvs";
                              /* trap on integer condition instruction */
                              if(i == 0)
	                        sprintf(buf, "%s r%ld + r%ld", tmp, rs1, rs2);
                              else
	                        sprintf(buf, "%s r%ld + %lx", tmp, rs1, simm13);
                              break;
                        }
                        case RDY:
                        {
                              /* read Y register instruction */
                              sprintf(buf, "rd y, r%ld", rd);
                              break;
                        }
                        case RDPSR:
                        {
                              /* read processor state register instruction */
                              sprintf(buf, "rd psr, r%ld", rd);
                              break;
                        }
                        case RDWIM:
                        {
                              /* read window invalid mask register instruction */
                              sprintf(buf, "rd win, r%ld", rd);
                              break;
                        }
                        case RDTBR:
                        {
                              /* read trap base register instruction */
                              sprintf(buf, "rd tbr, r%ld", rd);
                              break;
                        }
                        case WRY:
                        {
                              /* write Y register instruction */
                              if(i == 0)
	                        sprintf(buf, "wr r%ld, r%ld, y", rs1, rs2);
                              else
	                        sprintf(buf, "wr r%ld, %lx, y", rs1, simm13);
                              break;
                        }
                        case WRPSR:
                        {
                              /* write processor state register instruction */
                              if(i == 0)
	                        sprintf(buf, "wr r%ld, r%ld, psr", rs1, rs2);
                              else
	                        sprintf(buf, "wr r%ld, %lx, psr", rs1, simm13);
                              break;
                        }
                        case WRWIM:
                        {
                              /* write window invalid mask register instruction */
                              if(i == 0)
	                        sprintf(buf, "wr r%ld, r%ld, win", rs1, rs2);
                              else
	                        sprintf(buf, "wr r%ld, %lx, win", rs1, simm13);
                              break;
                        }
                        case WRTBR:
                        {
                              /* write trap base register instruction */
                              if(i == 0)
	                        sprintf(buf, "wr r%ld, r%ld, tbr", rs1, rs2);
                              else
	                        sprintf(buf, "wr r%ld, %lx, tbr", rs1, simm13);
                              break;
                        }
                        case IFLUSH:
                        {
                              /* instruction cache flush instruction */
                              if(i == 0)
	                        sprintf(buf, "iflush (r%ld + r%ld)", rs1, rs2);
                              else
	                        sprintf(buf, "iflush (r%ld + %lx)", rs1, imm22);
                              break;
                        }
                        case FPop1:
                        {
                              sprintf(buf, "fpop1 ??");
                              break;
                        }
                        case FPop2:
                        {
                              sprintf(buf, "fpop2 ??");
                              break;
                        }
                        default:
                        {
                              sprintf(buf, "??");
                        }
                }
                break;
        }
        default:
        {
                sprintf(buf, "??");
        }
  };

//   printf("ASM: op=%ld,disp30=%ld,rd=%ld,op2=%ld,imm22=%ld,a=%ld,cond=%ld,disp22=%ld,op3=%ld,rs1=%ld,i=%ld,asi=%ld,rs2=%ld,simm13=%ld,opf=%ld\n",
//         op,
//         disp30,
//         rd,
//         op2,
//         imm22,
//         a,
//         cond,
//         disp22,
//         op3,
//         rs1,
//         i,
//         asi,
//         rs2,
//         simm13,
//         opf);

}

void CAsm::Get(char* s, int len)
{
  int i;
  if(len > -1) 
  {
    strncpy(s, buf, len);
    s[len] = '\0';
    for(i = strlen(s); i < len; i++)
      s[i] = ' ';
  }
  else
  {
    strcpy(s, buf);
  }
}
