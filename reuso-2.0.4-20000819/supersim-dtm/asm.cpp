
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

  char *tmp = 0;

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
          if(cond == BA) tmp = (char*)"ba";
          else if(cond == BN) tmp = (char*)"bn";
          else if(cond == BNE) tmp = (char*)"bne";
          else if(cond == BE) tmp = (char*)"be";
          else if(cond == BG) tmp = (char*)"bg";
          else if(cond == BLE) tmp = (char*)"ble";
          else if(cond == BGE) tmp = (char*)"bge";
          else if(cond == BL) tmp = (char*)"bl";
          else if(cond == BGU) tmp = (char*)"bgu";
          else if(cond == BLEU) tmp = (char*)"bleu";
          else if(cond == BCC) tmp = (char*)"bcc";
          else if(cond == BCS) tmp = (char*)"bcs";
          else if(cond == BPOS) tmp = (char*)"bpos";
          else if(cond == BNEG) tmp = (char*)"bneg";
          else if(cond == BVC) tmp = (char*)"bvc";
          else if(cond == BVS) tmp = (char*)"bvs";
      
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
          if(cond == FBA) tmp = (char*)"fba";
          else if(cond == FBN) tmp = (char*)"fbn";
          else if(cond == FBU) tmp = (char*)"fbu";
          else if(cond == FBG) tmp = (char*)"fbg";
          else if(cond == FBUG) tmp = (char*)"fbug";
          else if(cond == FBL) tmp = (char*)"fbl";
          else if(cond == FBUL) tmp = (char*)"fbul";
          else if(cond == FBLG) tmp = (char*)"fblg";
          else if(cond == FBNE) tmp = (char*)"fbne";
          else if(cond == FBE) tmp = (char*)"fbe";
          else if(cond == FBUE) tmp = (char*)"fbue";
          else if(cond == FBGE) tmp = (char*)"fbge";
          else if(cond == FBUGE) tmp = (char*)"fbuge";
          else if(cond == FBLE) tmp = (char*)"fble";
          else if(cond == FBULE) tmp = (char*)"fbule";
          else if(cond == FBO) tmp = (char*)"fbo";
      
          if(a == 1)
	    sprintf(buf, "%s,a %08lx", tmp, _addr + (disp22 << 2));
          else
	    sprintf(buf, "%s %08lx", tmp, _addr + (disp22 << 2));
          break;
        }
        case OP2_VALUE_7:
        {
          /* branch on coprocessor condition */
          if(cond == CBA) tmp = (char*)"cba";
          else if(cond == CBN) tmp = (char*)"cbn";
          else if(cond == CB3) tmp = (char*)"cb3";
          else if(cond == CB2) tmp = (char*)"cb2";
          else if(cond == CB23) tmp = (char*)"cb23";
          else if(cond == CB1) tmp = (char*)"cb1";
          else if(cond == CB13) tmp = (char*)"cb13";
          else if(cond == CB12) tmp = (char*)"cb12";
          else if(cond == CB123) tmp = (char*)"cb123";
          else if(cond == CB0) tmp = (char*)"cb0";
          else if(cond == CB03) tmp = (char*)"cb03";
          else if(cond == CB02) tmp = (char*)"cb02";
          else if(cond == CB023) tmp = (char*)"cb023";
          else if(cond == CB01) tmp = (char*)"cb01";
          else if(cond == CB013) tmp = (char*)"cb013";
          else if(cond == CB012) tmp = (char*)"cb012";
      
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
          if(tmp == 0) tmp = (char*)"ldsba";
        }
        case LDSHA:
        {
          if(tmp == 0) tmp = (char*)"ldsha";
        }
        case LDUBA:
        {
          if(tmp == 0) tmp = (char*)"lduba";
        }
        case LDUHA:
        {
          if(tmp == 0) tmp = (char*)"lduha";
        }
        case LDA:
        {
          if(tmp == 0) tmp = (char*)"lda";
        }
        case LDDA:
        {
          if(tmp == 0) tmp = (char*)"ldda";

          /* load from alternate space */
          if(i == 0)
	    sprintf(buf, "%s r%ld + r%ld %03ld, r%ld", tmp, rs1, rs2, asi, rd);
          else
	    sprintf(buf, "??");
          break;
        }
        case LDSB:
        {
          if(tmp == 0) tmp = (char*)"ldsb";
        }
        case LDSH:
        {
          if(tmp == 0) tmp = (char*)"ldsh";
        }
        case LDUB:
        {
          if(tmp == 0) tmp = (char*)"ldub";
        }
        case LDUH:
        {
          if(tmp == 0) tmp = (char*)"lduh";
        }
        case LD:
        {
          if(tmp == 0) tmp = (char*)"ld";
        }
        case LDD:
        {
          if(tmp == 0) tmp = (char*)"ldd";
      
          /* load instructions */
          if(i == 0)
	    sprintf(buf, "%s r%ld + r%ld, r%ld", tmp, rs1, rs2, rd);
          else
	    sprintf(buf, "%s r%ld + %lx, r%ld", tmp, rs1, simm13, rd);
          break;
        }
        case LDF:
        {
          if(tmp == 0) tmp = (char*)"ldf";
        }
        case LDDF:
        {
          if(tmp == 0) tmp = (char*)"lddf";
      
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
          if(tmp == 0) tmp = (char*)"ldc";
        }
        case LDDC:
        {
          if(tmp == 0) tmp = (char*)"lddc";
      
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
          if(tmp == 0) tmp = (char*)"stba";
        }
        case STHA:
        {
          if(tmp == 0) tmp = (char*)"stha";
        }
        case STA:
        {
          if(tmp == 0) tmp = (char*)"sta";
        }
        case STDA:
        {
          if(tmp == 0) tmp = (char*)"stda";
      
          /* store into alternate space */
          if(i == 0)
	    sprintf(buf, "%s r%ld, r%ld + r%ld %03ld", tmp, rd, rs1, rs2, asi);
          else
	    sprintf(buf, "??");
          break;
        }
        case STB:
        {
          if(tmp == 0) tmp = (char*)"stb";
        }
        case STH:
        {
          if(tmp == 0) tmp = (char*)"sth";
        }
        case ST:
        {
          if(tmp == 0) tmp = (char*)"st";
        }
        case STD:
        {
          if(tmp == 0) tmp = (char*)"std";
      
          /* store instructions */
          if(i == 0)
	    sprintf(buf, "%s r%ld, r%ld + r%ld", tmp, rd, rs1, rs2);
          else
	    sprintf(buf, "%s r%ld, r%ld + %lx", tmp, rd, rs1, simm13);
          break;
        }
        case STF:
        {
          if(tmp == 0) tmp = (char*)"stf";
        }
        case STDF:
        {
          if(tmp == 0) tmp = (char*)"stdf";

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
          if(tmp == 0) tmp = (char*)"stc";
        }
        case STDC:
        {
          if(tmp == 0) tmp = (char*)"stdc";

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
          if(tmp == 0) tmp = (char*)"add";
        }
        case ADDCC:
        {
          if(tmp == 0) tmp = (char*)"addcc";
        }
        case ADDX:
        {
          if(tmp == 0) tmp = (char*)"addx";
        }
        case ADDXCC:
        {
          if(tmp == 0) tmp = (char*)"addxcc";
        }
        case TADDCC:
        {
          if(tmp == 0) tmp = (char*)"taddcc";
        }
        case TADDCCTV:
        {
          if(tmp == 0) tmp = (char*)"taddcctv";
        }
        case SUB:
        {
          if(tmp == 0) tmp = (char*)"sub";
        }
        case SUBCC:
        {
          if(tmp == 0) tmp = (char*)"subcc";
        }
        case SUBX:
        {
          if(tmp == 0) tmp = (char*)"subx";
        }
        case SUBXCC:
        {
          if(tmp == 0) tmp = (char*)"subxcc";
        }
        case TSUBCC:
        {
          if(tmp == 0) tmp = (char*)"tsubcc";
        }
        case TSUBCCTV:
        {
          if(tmp == 0) tmp = (char*)"tsubcctv";
        }
        case MULSCC:
        {
          if(tmp == 0) tmp = (char*)"mulscc";
        }
        case AND:
        {
          if(tmp == 0) tmp = (char*)"and";
        }
        case ANDCC:
        {
          if(tmp == 0) tmp = (char*)"andcc";
        }
        case ANDN:
        {
          if(tmp == 0) tmp = (char*)"andn";
        }
        case ANDNCC:
        {
          if(tmp == 0) tmp = (char*)"andncc";
        }
        case OR:
        {
          if(tmp == 0) tmp = (char*)"or";
        }
        case ORCC:
        {
          if(tmp == 0) tmp = (char*)"orcc";
        }
        case ORN:
        {
          if(tmp == 0) tmp = (char*)"orn";
        }
        case ORNCC:
        {
          if(tmp == 0) tmp = (char*)"orncc";
        }
        case XOR:
        {
          if(tmp == 0) tmp = (char*)"xor";
        }
        case XORCC:
        {
          if(tmp == 0) tmp = (char*)"xorcc";
        }
        case XNOR:
        {
          if(tmp == 0) tmp = (char*)"xnor";
        }
        case XNORCC:
        {
          if(tmp == 0) tmp = (char*)"xnorcc";
        }
        case SLL:
        {
          if(tmp == 0) tmp = (char*)"sll";
        }
        case SRL:
        {
          if(tmp == 0) tmp = (char*)"srl";
        }
        case SRA:
        {
          if(tmp == 0) tmp = (char*)"sra";
        }
        case SAVE:
        {
          if(tmp == 0) tmp = (char*)"save";
        }
        case RESTORE:
        {
          if(tmp == 0) tmp = (char*)"restore";
      
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
          if(cond == TA) tmp = (char*)"ta";
          else if(cond == TN) tmp = (char*)"tn";
          else if(cond == TNE) tmp = (char*)"tne";
          else if(cond == TE) tmp = (char*)"te";
          else if(cond == TG) tmp = (char*)"tg";
          else if(cond == TLE) tmp = (char*)"tle";
          else if(cond == TGE) tmp = (char*)"tge";
          else if(cond == TL) tmp = (char*)"tl";
          else if(cond == TGU) tmp = (char*)"tgu";
          else if(cond == TLEU) tmp = (char*)"tleu";
          else if(cond == TCC) tmp = (char*)"tcc";
          else if(cond == TCS) tmp = (char*)"tcs";
          else if(cond == TPOS) tmp = (char*)"tpos";
          else if(cond == TNEG) tmp = (char*)"tneg";
          else if(cond == TVC) tmp = (char*)"tvc";
          else if(cond == TVS) tmp = (char*)"tvs";
      
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
}

void CAsm::Get(char* s)
{
  Get(s, -1);
}

void CAsm::Get(char* s, int len)
{
  int i;
  if(len > -1) 
  {
    strncpy(s, buf, len);
    s[len] = '\0';
    for(i = strlen(s); i < len; i++)
    {
      s[i] = ' ';
    }
  }
  else
  {
    strcpy(s, buf);
  }
}

