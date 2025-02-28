
/*
 * dispatch.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/24/99
 */

///////////////////////////////////////////////////////////
///// CDispatch

class CDispatch {
 private:

  CComponent* Dtm;

  CDisq* Disq;

  CDisq* DBDisq;

  CSReg* SReg;

  CIReg* IReg;

  CRs* RsLsu;

  CRs* RsBpu;

  CRs* RsIu;

  CMaq* Maq;

  CRob* Rob;


  uword_t MapPsr;

  uword_t MapY;

  uword_t MapR [ DISPATCH_WIDTH ];


  void ClearMap();

  int HaveDependence(CDisqItem* _di);


 public:

  CDispatch();

  void Init( CComponent* _dtm,
	     CDisq* _disq,
	     CDisq* _dbdisq,
	     CSReg* _sreg,
	     CIReg* _ireg,
	     CRs* _rsiu, 
	     CRs* _rsbpu,
	     CRs* _rslsu,
	     CMaq* _maq,
	     CRob* _rob );


  void Reserv(CRsItem* _rsit, CRobItem* _robit);

  void Execute(bool_t* _dtmbrkflag, uword_t* _dtmbrkcnt);

  void Reset();

};
