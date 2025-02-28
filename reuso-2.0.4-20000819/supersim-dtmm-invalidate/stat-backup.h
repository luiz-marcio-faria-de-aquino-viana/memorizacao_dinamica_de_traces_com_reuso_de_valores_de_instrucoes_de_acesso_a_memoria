
/*
 * stat.h
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 3/29/2000
 */

#ifndef __STAT_H
#define __STAT_H 104

///////////////////////////////////////////////////////////////
///// CStat

class CStat {
private:

  long Cicles;           // numero de ciclos executados


  long ICount;           // total de instrucoes completadas


  long iTime;               // valor inicial do relogio

  long fTime;               // valor final do relogio


  long Time;             // total de segundos gastos


public:

  CStat();


  void IncCicle();


  void IncICount();

  void IncICount(uword_t _n);


  void StartClock();

  void StopClock();


  void Show();


  void Reset();

};

#endif
