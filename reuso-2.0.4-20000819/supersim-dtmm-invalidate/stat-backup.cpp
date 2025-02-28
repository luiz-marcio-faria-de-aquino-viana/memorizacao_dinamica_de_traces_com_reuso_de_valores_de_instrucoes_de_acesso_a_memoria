
/*
 * stat.cpp
 * Copyright (C) 2000 by Luiz Marcio F A Viana, 3/29/2000
 */

#include<stdio.h>
#include<time.h>
#include<string.h>
#include"all.h"

///////////////////////////////////////////////////////////////
///// CStat

CStat::CStat()
{
  Reset();
}

void CStat::IncCicle()
{
  Cicles += 1;
}

void CStat::IncICount()
{
  ICount += 1;
  if((ICount % STAT_IC_STEP_LIMIT) == 0)
    printf("\n%ld instructions executed in %ld ciles (IPC = %f)... ", ICount, Cicles, (double) ICount / Cicles);
#if(DEBUG == 0)
  fprintf(stderr, "\nSTAT::INC_ICOUNT(n=1, ni=%ld, cicles=%ld)", ICount, Cicles);
#endif
}

void CStat::IncICount(uword_t _n)
{
  if(((ICount % STAT_IC_STEP_LIMIT) + _n) >= STAT_IC_STEP_LIMIT)
    printf("\n%ld instructions executed in %ld ciles (IPC = %f)... ", ICount + _n, Cicles, (double) (ICount + _n) / Cicles);
  ICount += _n;
#if(DEBUG == 0)
  fprintf(stderr, "\nSTAT::INC_ICOUNT(n=%ld, ni=%ld, cicles=%ld)", _n, ICount, Cicles);
#endif
}

void CStat::StartClock()
{
  iTime = time(0);
}

void CStat::StopClock()
{
  fTime = time(0);
  Time = Time + (fTime - iTime);
}

void CStat::Show()
{
  printf("\n");

  printf("Statistics\n");
  printf("==========\n");

  printf("\n");

  printf("Number of cicles       = %16ld\n\n", Cicles);
  printf("Number of instructions = %16ld\n\n", ICount);
  printf("IPC medio              = %16.4f\n\n", (double) ICount / Cicles);

  printf("\n");

  printf("Simulation time        = %16ld\n\n", Time);
 
}

void CStat::Reset()
{
  Cicles = 0;

  ICount = 0;

  iTime = 0;
  fTime = 0;

  Time = 0;
}
