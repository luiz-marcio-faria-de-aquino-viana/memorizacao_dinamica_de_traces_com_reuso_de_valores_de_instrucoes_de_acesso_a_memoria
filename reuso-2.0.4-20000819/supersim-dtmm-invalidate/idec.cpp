
/*
 * idec.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/9/99
 */

#include<stdio.h>
#include"all.h"

extern str_t errtile;

//////////////////////////////////////////////////////////////
///// CIDec

int CIDec::IsBadInst()
{
  return Bad;
}

int CIDec::IsLdStInst()
{
  return ( ((RType != 0) || (WType != 0)) ? TRUE : FALSE );
}

uword_t CIDec::GetExecUnit()
{
  return Unit;
}

uword_t CIDec::GetInstOpcode()
{
  return Opcode;
}

void CIDec::Set(CIQueueItem iq)
{

  *((CIQueueItem*) this) = iq;

  Bad = FALSE;

  Unit = NONE;

  RType = RD_NONE;
  WType = WR_NONE;

  Psr.Reset();

  Y.Reset();

  Rs1.Reset();
  Rs2.Reset();

  Rd_hi.Reset();
  Rd_lo.Reset();


  fi.op     = (Inst >> 30) & 0x03L;
  fi.disp30 = SIGN_EXTEND((Inst & 0x3FFFFFFFL), 0xE0000000L);
  fi.rd     = (Inst >> 25) & 0x1FL;
  fi.op2    = (Inst >> 22) & 0x07L;
  fi.imm22  = Inst & 0x003FFFFFL;
  fi.a      = (Inst >> 29) & 0x01L;
  fi.cond   = (Inst >> 25) & 0x0FL;
  fi.disp22 = SIGN_EXTEND((Inst & 0x003FFFFFL), 0xFFE00000L);
  fi.op3    = (Inst >> 19) & 0x3FL;
  fi.rs1    = (Inst >> 14) & 0x1FL;
  fi.i      = (Inst >> 13) & 0x01L;
  fi.asi    = (Inst >>  5) & 0xFFL;
  fi.rs2    = Inst & 0x0000001FL;
  fi.simm13 = SIGN_EXTEND((Inst & 0x00001FFFL), 0xFFFFF000L);
  fi.opf    = (Inst >>  5) & 0x1FFL;


  switch(fi.op) {

  case OP_VALUE_1:
    /* instrucao do formato 1 (CALL) */

    Unit = BPU;
    Opcode = CALL;
    Rd_hi.Set(REQ_TARGET, R15);
    break;


  case OP_VALUE_0:
    /* instrucao do formato 2 (UNIMP, Bicc, FBfcc, CBccc, SETHI) */

    switch(fi.op2) {

    case OP2_VALUE_0:
      /* instrucao UNIMP */
      sprintf(errtile, "(@%08lX, unimp=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case OP2_VALUE_2:
      /* instrucao Bicc */
      Unit = BPU;
      Opcode = BICC;
      if((fi.cond != BA) && (fi.cond != BN))
	Psr.Set(REQ_SOURCE);
      break;

    case OP2_VALUE_4:
      /* instrucao SETHI */
      Unit = IU;
      Opcode = SETHI;
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case OP2_VALUE_6:
      /* branch on floating-point condition */
      sprintf(errtile, "(@%08lX, fbfcc=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case OP2_VALUE_7:
      /* branch on coprocessor condition */
      sprintf(errtile, "(@%08lX, cpccc=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      break;

    case OP2_VALUE_1:
    case OP2_VALUE_3:
    case OP2_VALUE_5:
      sprintf(errtile, "(@%08lX, i=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_INVINSTRUCTION, errtile);
      Bad = TRUE;

    }
    break;


  case OP_VALUE_3:
    /* instrucao do formato 3 (Load/Store) */
    switch(fi.op3) {
    case LDSBA:
    case LDSHA:
    case LDUBA:
    case LDUHA:
    case LDA:
    case LDDA:
      /* load integer instructions from alternate space */
      sprintf(errtile, "(@%08lX, lda=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case LDSB:
      /* load integer instructions (byte) */
      Unit = LSU;
      Opcode = fi.op3;
      RType = RD_BYTE;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case LDSH:
      /* load integer instructions (half word) */
      Unit = LSU;
      Opcode = fi.op3;
      RType = RD_HWORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case LDUB:
      /* load integer instructions (unsigned byte) */
      Unit = LSU;
      Opcode = fi.op3;
      RType = RD_UBYTE;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case LDUH:
      /* load integer instructions (unsigned half word) */
      Unit = LSU;
      Opcode = fi.op3;
      RType = RD_UHWORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case LD:
      /* load integer instructions (word) */
      Unit = LSU;
      Opcode = fi.op3;
      RType = RD_WORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case LDD:
      /* load integer instructions (double word) */
      Unit = LSU;
      Opcode = fi.op3;
      RType = RD_DWORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_TARGET, fi.rd & 0x1E);
      Rd_lo.Set(REQ_TARGET, (fi.rd & 0x1E) + 1);
      break;

    case LDF:
    case LDDF:
    case LDFSR:
      /* load floating-point instructions */
      sprintf(errtile, "(@%08lX, ldf=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case LDC:
    case LDDC:
    case LDCSR:
      /* load coprocessor instructions */
      sprintf(errtile, "(@%08lX, ldc=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case STBA:
    case STHA:
    case STA:
    case STDA:
      /* store integer instructions from alternate space */
      sprintf(errtile, "(@%08lX, sta=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case STB:
      /* store integer instructions (byte) */
      Unit = LSU;
      Opcode = fi.op3;
      WType = WR_BYTE;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_SOURCE, fi.rd);
      break;


    case STH:
      /* store integer instructions (hword) */
      Unit = LSU;
      Opcode = fi.op3;
      WType = WR_HWORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_SOURCE, fi.rd);
      break;

    case ST:
      /* store integer instructions (word) */
      Unit = LSU;
      Opcode = fi.op3;
      WType = WR_WORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_SOURCE, fi.rd);
      break;

    case STD:
      /* store integer instructions (dword) */
      Unit = LSU;
      Opcode = fi.op3;
      WType = WR_DWORD;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Rd_hi.Set(REQ_SOURCE, fi.rd & 0x001E);
      Rd_lo.Set(REQ_SOURCE, (fi.rd & 0x001E) + 1);
      break;

    case STF:
    case STDF:
    case STFSR:
    case STDFQ:
      /* store floating-point instructions */
      sprintf(errtile, "(@%08lX, stf=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case STC:
    case STDC:
    case STCSR:
    case STDCQ:
      /* store coprocessor instructions */
      sprintf(errtile, "(@%08lX, stc=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case LDSTUBA:
      /* atomic load-store unsigned byte instructions from alternate space */
      sprintf(errtile, "(@%08lX, ldstuba=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case LDSTUB:
      /* atomic load-store unsigned byte instructions */
      sprintf(errtile, "(@%08lX, ldstub=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case SWAPA:
      /* swap register with memory from alternate space */
      sprintf(errtile, "(@%08lX, swapa=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case SWAP:
      /* swap register with memory */
      sprintf(errtile, "(@%08lX, swap=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    default:
      sprintf(errtile, "(@%08lX, i=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_INVINSTRUCTION, errtile);
      Bad = TRUE;
    }
    break;


  case OP_VALUE_2:
    /* instrucao do formato 3 */
    switch(fi.op3) {
    case ADD:
    case ADDCC:
    case ADDX:
    case ADDXCC:
      /* add instructions */
      Unit = IU;
      Opcode = fi.op3;

      if((fi.op3 == ADDX) || (fi.op3 == ADDXCC))
	Psr.Set(REQ_SOURCE);

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      if((fi.op3 == ADDCC) || (fi.op3 == ADDXCC))
	Psr.Set(REQ_TARGET);

      break;

    case TADDCC:
      /* tagged add instructions */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      Psr.Set(REQ_TARGET);

      break;

    case TADDCCTV:
      /* tagged add instructions and trap on overflow */
      sprintf(errtile, "(@%08lX, taddcctv=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case SUB:
    case SUBCC:
    case SUBX:
    case SUBXCC:
      /* subtract instructions */
      Unit = IU;
      Opcode = fi.op3;

      if((fi.op3 == SUBX) || (fi.op3 == SUBXCC))
	Psr.Set(REQ_SOURCE);

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);


      Rd_hi.Set(REQ_TARGET, fi.rd);

      if((fi.op3 == SUBCC) || (fi.op3 == SUBXCC))
	Psr.Set(REQ_TARGET);

      break;

    case TSUBCC:
      /* tagged subtract instructions */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      Psr.Set(REQ_TARGET);

      break;

    case TSUBCCTV:
      /* tagged subtract instructions and trap on overflow */
      sprintf(errtile, "(@%08lX, tsubcctv=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case MULSCC:
      /* multiply step instruction */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Y.Set(REQ_SOURCE);

      Psr.Set(REQ_SOURCE);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      Y.Set(REQ_TARGET);

      Psr.Set(REQ_TARGET);

      break;

    case AND:
    case ANDCC:
    case ANDN:
    case ANDNCC:
    case OR:
    case ORCC:
    case ORN:
    case ORNCC:
    case XOR:
    case XORCC:
    case XNOR:
    case XNORCC:
      /* logical instructions */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      if( (fi.op3 == ANDCC) || (fi.op3 == ANDNCC) ||
	  (fi.op3 == ORCC) || (fi.op3 == ORNCC) ||
	  (fi.op3 == XORCC) || (fi.op3 == XNORCC) )
	Psr.Set(REQ_TARGET);

      break;

    case SLL:
    case SRL:
    case SRA:
      /* shift instructions */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      break;

    case SAVE:
      /* SAVE instruction */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      break;

    case RESTORE:
      /* RESTORE instruction */
      Unit = IU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      break;

    case JMPL:
      /* jump and link instruction */
      Unit = BPU;
      Opcode = fi.op3;

      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);

      Rd_hi.Set(REQ_TARGET, fi.rd);

      break;

    case RETT:
      /* return from trap instruction */
      sprintf(errtile, "(@%08lX, rett=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case TICC:
      /* trap on integer condition instruction */
      Unit = BPU;
      Opcode = TICC;
      if(fi.cond != TA) {
	sprintf(errtile, "(@%08lX, ticc=%08lX)", Addr, Inst);
	warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
	Bad = TRUE;
      }
      break;

    case RDY:
      /* read Y register instruction */
      Unit = IU;
      Opcode = fi.op3;
      Y.Set(REQ_SOURCE);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case RDPSR:
      /* read processor state register instruction */
      Unit = IU;
      Opcode = fi.op3;
      Psr.Set(REQ_SOURCE);
      Rd_hi.Set(REQ_TARGET, fi.rd);
      break;

    case RDWIM:
      /* read window invalid mask register instruction */
      sprintf(errtile, "(@%08lX, rdwim=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case RDTBR:
      /* read trap base register instruction */
      sprintf(errtile, "(@%08lX, rdtbr=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case WRY:
      /* write Y register instruction */
      Unit = IU;
      Opcode = fi.op3;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Y.Set(REQ_TARGET);
      break;

    case WRPSR:
      /* write processor state register instruction */
      Unit = IU;
      Opcode = fi.op3;
      Rs1.Set(REQ_SOURCE, fi.rs1);
      if(fi.i == 0)
	Rs2.Set(REQ_SOURCE, fi.rs2);
      Psr.Set(REQ_TARGET);
      break;

    case WRWIM:
      /* write window invalid mask register instruction */
      sprintf(errtile, "(@%08lX, wrwim=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case WRTBR:
      /* write trap base register instruction */
      sprintf(errtile, "(@%08lX, wrtbr=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case IFLUSH:
      /* instruction cache flush instruction */
      sprintf(errtile, "(@%08lX, iflush=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    case FPop1:
    case FPop2:
      /* floating-point operate (FPop) instructions */
      sprintf(errtile, "(@%08lX, fpop=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_NOTIMPLEMENTED, errtile);
      Bad = TRUE;
      break;

    default:
      sprintf(errtile, "(@%08lX, i=%08lX)", Addr, Inst);
      warnmsg(__IDEC_H, WARN_INVINSTRUCTION, errtile);
      Bad = TRUE;
    }

  };

}
