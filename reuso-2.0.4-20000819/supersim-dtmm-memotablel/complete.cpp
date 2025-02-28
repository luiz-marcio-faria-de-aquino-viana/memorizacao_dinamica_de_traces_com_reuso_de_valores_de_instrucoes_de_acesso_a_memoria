
/*
 * complete.cpp
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/5/99
 */

#include<stdio.h>
#include"all.h"

/////////////////////////////////////////////////////////////
///// CComplete

void CComplete::Init( CIReg* _ireg,
		      CSReg* _sreg,
		      CRs* _rsiu,
		      CRs* _rsbpu,
		      CRs* _rslsu,
		      CIu* _iu,
		      CBpu* _bpu,
		      CLsu* _lsu,
		      CRob* _rob )
{
  IReg = _ireg;
  SReg = _sreg;
  RsIu = _rsiu;
  RsBpu = _rsbpu;
  RsLsu = _rslsu;
  Iu = _iu;
  Bpu = _bpu;
  Lsu = _lsu;
  Rob = _rob;
}

void CComplete::Step(CResBuf* _rb)
{
  uword_t rd_hi, val_hi;
  uword_t rd_lo, val_lo;

  int i;

  rd_hi = rd_lo = 0;
  val_hi = val_lo = 0;

  CRsItem* rsit = _rb->Rsit;
  CMemoTableEntryT* q = NULL;

  if( rsit->IsTraceRedundant() ) {
    q = & (_rb->Rsit)->TraceData;

    /* atualiza registradores destino para contexto de saida do trace */

    /*
     *for(i = 0; i < OC_NUM; i++) {
     *  switch( q->Oc[i].regtype ) {
     *  case CTX_PSR:
     *    (SReg->Psr).Update(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  RsIu->UpdatePsr(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  RsLsu->UpdatePsr(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  RsBpu->UpdatePsr(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  break;
     *
     *  case CTX_Y:
     *	  (SReg->Y).Update(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  RsIu->UpdateY(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  RsLsu->UpdateY(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  RsBpu->UpdateY(rsit->GetSeqNum(), q->Oc[i].regval);
     *	  break;
     *
     *  case CTX_GPR:
     *	  IReg->Update( rsit->GetSeqNum(),
     *		        rsit->GetTargetWin(),
     *		        q->Oc[i].regaddr,
     *		        q->Oc[i].regval );
     *
     *	  RsIu->UpdateIReg( rsit->GetSeqNum(),
     *			    rsit->GetTargetWin(),
     *			    q->Oc[i].regaddr,
     *			    q->Oc[i].regval );
     *
     *	  RsLsu->UpdateIReg( rsit->GetSeqNum(),
     *			     rsit->GetTargetWin(),
     *			     q->Oc[i].regaddr,
     *			     q->Oc[i].regval );
     *
     *	  RsBpu->UpdateIReg( rsit->GetSeqNum(),
     *			     rsit->GetTargetWin(),
     *			     q->Oc[i].regaddr,
     *			     q->Oc[i].regval );
     *
     *  };
     *}
     */

    /* atualiza registradores destino para instrucoes de acesso a memoria do trace */

    for(i = 0; i < q->LdStSz; i++) {
      if( (q->LdStInst[i].LdSt & IOM_LOAD) != 0 ) {
	// instrucoes de leitura - apenas traces com valores invalidados acessam a memoria

	if( (q->LdStInst[i]).Rdhi.IsReqAsTarget() ) {
	  rd_hi = (q->LdStInst[i]).Rdhi.GetRegAddr();
	  val_hi = (q->LdStInst[i]).MValhi;
	  IReg->Update( rsit->GetSeqNum(),
			rsit->GetTargetWin(),
			rd_hi,
			val_hi );
	}
      
	if( (q->LdStInst[i]).Rdlo.IsReqAsTarget() ) {
	  rd_lo = (q->LdStInst[i]).Rdlo.GetRegAddr();
	  val_lo = (q->LdStInst[i]).MVallo;
	  IReg->Update( rsit->GetSeqNum(),
			rsit->GetTargetWin(),
			rd_lo,
			val_lo );
	}
	
	RsIu->Update( rsit->GetSeqNum(),
		      rsit->GetTargetWin(),
		      (q->LdStInst[i]).Rdhi.GetRegAddr(),
		      (q->LdStInst[i]).MValhi,
		      (q->LdStInst[i]).Rdlo.GetRegAddr(),
		      (q->LdStInst[i]).MVallo,
		      0,
		      0 );
    
	RsLsu->Update( rsit->GetSeqNum(),
		       rsit->GetTargetWin(),
		       (q->LdStInst[i]).Rdhi.GetRegAddr(),
		       (q->LdStInst[i]).MValhi,
		       (q->LdStInst[i]).Rdlo.GetRegAddr(),
		       (q->LdStInst[i]).MVallo,
		       0,
		       0 );
    
	RsBpu->Update( rsit->GetSeqNum(),
		       rsit->GetTargetWin(),
		       (q->LdStInst[i]).Rdhi.GetRegAddr(),
		       (q->LdStInst[i]).MValhi,
		       (q->LdStInst[i]).Rdlo.GetRegAddr(),
		       (q->LdStInst[i]).MVallo,
		       0,
		       0 );
      
      }

    }

    Rob->UpdateSrc(rsit->GetSeqNum(), rsit);     // atualiza valores fonte para entrada correspondente no ROB
	
    Rob->Update( rsit->GetSeqNum(),              // atualiza valores destino para entrada correspondente no ROB
		 _rb->Psr,
		 _rb->Y,
		 _rb->Result1,
		 _rb->Result2,
		 _rb->Result1,
		 _rb->Result2,
		 _rb->Trapsig,
		 _rb->Trapnum,
		 _rb->ChgPC,
		 _rb->NewPC,
		 0,
		 rsit->BResult,
		 _rb->MAddr );

  }
  else {

    if( (rsit->Rs1).IsReqAsTarget() ) {
      rd_hi = (rsit->Rs1).GetRegAddr();
      val_hi = _rb->Result1;
      rd_lo = rd_hi;
      IReg->Update( rsit->GetSeqNum(),
		    rsit->GetTargetWin(),
		    rd_hi,
		    val_hi );
    }
    
    if( (rsit->Rd_hi).IsReqAsTarget() ) {
      rd_hi = (rsit->Rd_hi).GetRegAddr();
      val_hi = _rb->Result1;
      rd_lo = rd_hi;
      IReg->Update( rsit->GetSeqNum(),
		    rsit->GetTargetWin(),
		    rd_hi,
		    val_hi );
    }
    
    if( (rsit->Rs2).IsReqAsTarget() ) {
      rd_lo = (rsit->Rs2).GetRegAddr();
      val_lo = _rb->Result2;
      IReg->Update( rsit->GetSeqNum(),
		    rsit->GetTargetWin(),
		    rd_lo,
		    val_lo );
    }
    
    if( (rsit->Rd_lo).IsReqAsTarget() ) {
      rd_lo = (rsit->Rd_lo).GetRegAddr();
      val_lo = _rb->Result2;
      IReg->Update( rsit->GetSeqNum(),
		    rsit->GetTargetWin(),
		    rd_lo,
		    val_lo );
    }
    
    if( (rsit->Psr).IsReqAsTarget() )
      (SReg->Psr).Update( rsit->GetSeqNum(),
			  _rb->Psr );
    
    if( (rsit->Y).IsReqAsTarget() )
      (SReg->Y).Update( rsit->GetSeqNum(),
			_rb->Y );
    
    RsIu->Update( rsit->GetSeqNum(),
		  rsit->GetTargetWin(),
		  rd_hi,
		  val_hi,
		  rd_lo,
		  val_lo,
		  _rb->Psr,
		  _rb->Y );
    
    RsLsu->Update( rsit->GetSeqNum(),
		   rsit->GetTargetWin(),
		   rd_hi,
		   val_hi,
		   rd_lo,
		   val_lo,
		   _rb->Psr,
		   _rb->Y );
    
    RsBpu->Update( rsit->GetSeqNum(),
		   rsit->GetTargetWin(),
		   rd_hi,
		   val_hi,
		   rd_lo,
		   val_lo,
		   _rb->Psr,
		   _rb->Y );
    
    Rob->UpdateSrc( rsit->GetSeqNum(),
		    rsit );  
    
    Rob->Update( rsit->GetSeqNum(),
		 _rb->Psr,
		 _rb->Y,
		 _rb->Result1,
		 _rb->Result2,
		 _rb->Result1,
		 _rb->Result2,
		 _rb->Trapsig,
		 _rb->Trapnum,
		 _rb->ChgPC,
		 _rb->NewPC,
		 0,
		 rsit->BResult,
		 _rb->MAddr );
  
  }

}

void CComplete::Execute()
{
  CResBuf* rb;
  uword_t i;

  for(i = 0; i < Iu->GetNumUnits(); i++) {
    if( (rb = Iu->GetNthResBuf(i))->IsValid() ) {
#if(DEBUG == 0)
      printf( "\nCOMPLETE(iu)::EXECUTE(%lu) [%lu, %lx, %lx]",
	      i,
	      (rb->Rsit)->Seq % MAX_SEQNUM,
	      (rb->Rsit)->Addr,
	      (rb->Rsit)->Inst );
#endif
      Step(rb);
      RsIu->RemoveItem((rb->Rsit)->GetSeqNum());
      rb->Reset();
    }
  }

  for(i = 0; i < Lsu->GetNumUnits(); i++) {
    if( (rb = Lsu->GetNthResBuf(i))->IsValid() ) {
#if(DEBUG == 0)
      printf( "\nCOMPLETE(lsu)::EXECUTE(%lu) [%lu, %lx, %lx]",
	      i,
	      (rb->Rsit)->Seq % MAX_SEQNUM,
	      (rb->Rsit)->Addr,
	      (rb->Rsit)->Inst );
#endif
      Step(rb);
      RsLsu->RemoveItem((rb->Rsit)->GetSeqNum());
      rb->Reset();
    }
  }

  for(i = 0; i < Bpu->GetNumUnits(); i++) {
    if( (rb = Bpu->GetNthResBuf(i))->IsValid() ) {
#if(DEBUG == 0)
      printf( "\nCOMPLETE(bpu)::EXECUTE(%lu) [%lu, %lx, %lx]",
	      i,
	      (rb->Rsit)->Seq % MAX_SEQNUM,
	      (rb->Rsit)->Addr,
	      (rb->Rsit)->Inst );
#endif
      Step(rb);
      RsBpu->RemoveItem((rb->Rsit)->GetSeqNum());
      rb->Reset();
    }
  }
}
