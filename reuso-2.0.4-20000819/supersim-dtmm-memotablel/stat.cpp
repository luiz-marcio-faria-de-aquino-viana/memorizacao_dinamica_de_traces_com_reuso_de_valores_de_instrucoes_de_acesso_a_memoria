
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

void CStat::IncIReuse(uword_t _n)
{
  IReuse += _n;
  if(_n > 1) {
    ITraceReuse += _n;
    ITrace += 1;
  }
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

// AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
// _execunit - unidade de execucao
// _instop - opcode da instrucao
// _numinstr - numero de instrucoes (>1 para traces)
// _tpfaixa - tipo de faixa (0-NORMAL, 1-INST_REUSE, 2-TRACE, 3-TRACE_REUSE)
void CStat::AddInstrCount(uword_t _execunit, uword_t _instop, long _numinstr, int _tpfaixa)
{

        switch( _execunit ) 
        {
                case IU:
                {
                        /* instrucoes logicas, aritimeticas e de multiplicacao */
                        switch( _instop ) 
                        {
                                case ADD:
                                case ADDCC:
                                case ADDX:
                                case ADDXCC:
                                case SUB:
                                case SUBCC:
                                case SUBX:
                                case SUBXCC:
                                {
                                        IC_arithmetic[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case MULSCC:
                                {
                                        IC_mult[_tpfaixa] += _numinstr;
                                        break;
                                }
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
                                case SLL:
                                case SRL:
                                case SRA:
                                {
                                        IC_logic[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case SETHI:
                                {
                                        IC_sethi[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case SAVE:
                                {
                                        IC_save[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case RESTORE:
                                {
                                        IC_restore[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case RDPSR:
                                case RDY:
                                case WRPSR:
                                case WRY:
                                {
                                        IC_others[_tpfaixa] += _numinstr;
                                        break;
                                }
                                default:
                                {
                                        IC_others[_tpfaixa] += _numinstr;
                                }
                        }
                        break;
                }
                case BPU:
                {
                        /* instrucoes de desvios e chamadas ao sistema operacional */
                        switch( _instop ) 
                        {
                                case CALL:
                                {
                                        IC_call[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case BICC:
                                {
                                        IC_bicc[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case JMPL:
                                {
                                        IC_jmpl[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case TICC:
                                {
                                        IC_ticc[_tpfaixa] += _numinstr;
                                        break;
                                }
                                default:
                                {
                                        IC_others[_tpfaixa] += _numinstr;
                                }
                                break;
                        }
                }
                case LSU:
                {
                        /* instrucoes de acesso a memoria */
                        switch(_instop) 
                        {
                                case LDSB:
                                case LDSH:
                                case LDUB:
                                case LDUH:
                                case LD:
                                case LDD:
                                {
                                        IC_load[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case STB:
                                case STH:
                                case ST:
                                case STD:
                                {
                                        IC_store[_tpfaixa] += _numinstr;
                                        break;
                                }
                                case LDSTUB:
                                case SWAP:
                                {
                                        IC_others[_tpfaixa] += _numinstr;
                                        break;
                                }
                                default:
                                {
                                        IC_others[_tpfaixa] += _numinstr;
                                }
                        }
                        break;
                }
                default:
                {
                        IC_others[_tpfaixa] += _numinstr;
                }
        }
}

// AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
// _stats - estatistica a ser contabilizada emforma de estrutura 'statistics_t'
// _tpfaixa - tipo de faixa (0-NORMAL, 1-INST_REUSE, 2-TRACE, 3-TRACE_REUSE)
void CStat::AddInstrCount(statistics_t * _stats, int _tpfaixa)
{
  /* instrucoes de desvios */

  IC_call[_tpfaixa] += _stats->IC_call;

  IC_bicc[_tpfaixa] += _stats->IC_bicc;

  IC_jmpl[_tpfaixa] += _stats->IC_jmpl;

  /* chamadas ao sistema operacional */

  IC_ticc[_tpfaixa] += _stats->IC_ticc;

  /* instrucoes de acesso a memoria */

  IC_load[_tpfaixa] += _stats->IC_load;

  IC_store[_tpfaixa] += _stats->IC_store;

  /* instrucoes logicas e aritimeticas */

  IC_arithmetic[_tpfaixa] += _stats->IC_arithmetic;

  IC_logic[_tpfaixa] += _stats->IC_logic;

  /* instrucoes de multiplicacao */

  IC_mult[_tpfaixa] += _stats->IC_mult;

  /* instrucoes de save/restore */

  IC_save[_tpfaixa] += _stats->IC_save;

  IC_restore[_tpfaixa] += _stats->IC_restore;

  /* instrucao sethi */

  IC_sethi[_tpfaixa] += _stats->IC_sethi;

  /* outras instrucoes */

  IC_others[_tpfaixa] += _stats->IC_others;

}

// AddTraceSz():  contabilizacao do tamanho dos traces por faixa e freq. de load/store e desvios
// _tracesz - tamanho do trace
// _tpfaixa - tipo de faixa (0-TRACE, 1-TRACE_REUSE)
// _stats - estatistica a ser contabilizada emforma de estrutura 'statistics_t'
void CStat::AddTraceSz(long _tracesz, int _tpfaixa, statistics_t * _stats)
{

  long num_branches;

  /* total de traces */

  TSZ_traces[_tpfaixa] += 1;

  /* frequencia de traces por tamanho */

  if((_tracesz - 2) < TSZ_MAX) {
    TSZ_traces_per_size[_tracesz - 2][_tpfaixa] += 1;

    /* frequencia de traces com instrucoes de load/store */

    if(_stats != NULL) {

      if(_stats->IC_load > 0)
	TSZ_traces_load_per_size[_tracesz - 2][_tpfaixa] += 1;
      
      if(_stats->IC_store > 0)
	TSZ_traces_store_per_size[_tracesz - 2][_tpfaixa] += 1;

    }  

  }

  /* total de instrucoes nos traces */

  TSZ_traces_instr[_tpfaixa] += _tracesz;

  /* obtem o tamanho maximo do trace */

  if(_tracesz > TSZ_maximo[_tpfaixa])
    TSZ_maximo[_tpfaixa] = _tracesz;

  /* frequencia de desvios no trace */

  if(_stats != NULL) {

    num_branches = 0;

    num_branches += _stats->IC_call;
    num_branches += _stats->IC_bicc;
    num_branches += _stats->IC_jmpl;
    
    if(num_branches < TSZ_MAX)
      TSZ_branches_per_trace[num_branches][_tpfaixa] += 1;
  
  }

}

// AddTraceTerm(): contabiliza a forma de finalizacao dos traces
// _tpterm - tipo de finalizacao (TFN_OVERFLOW, TFN_DELAYINST, TFN_NVALID, TFN_NREDUNDANT, TFN_TRACE, TFN_LOADSTORE)
// _tpfaixa - tipo de faixa (0-TRACE, 1-TRACE_REUSE, 2-NOT_FORMED)
void CStat::AddTraceTerm(uword_t _tpterm, int _tpfaixa)
{
  TFN_traces[_tpterm][_tpfaixa] += 1;
}

// AddLoadStoreDelay(): contabiliza o tempo de processamento de load/store
// _instropcode - opcode da instrucao
// _ncicles - tempo de espera em ciclos
void CStat::AddLoadStoreDelay(long _instropcode, long _ncicles)
{
  T_total_load += 1;
  T_total_store += 1;
}

// AddTraceFollowingLoad(): contabiliza o total de instrucoes de load que servem traces
// _tldtype - indicador de instrucao fonte reusada (TLD_NOLOAD, TLD_INST_LOAD, TLD_REUSED_LOAD, TLD_TRACE_LOAD)
void CStat::AddTraceFollowingLoad(int _tldtype)
{
  if(_tldtype == TLD_INST_LOAD)
    TLD_traces_following_load += 1;
  else if(_tldtype == TLD_REUSED_LOAD)
    TLD_traces_following_reused_load += 1;
  else if(_tldtype == TLD_TRACE_LOAD)
    TLD_traces_following_trace_load += 1;
  else {
    /* TLD_NOLOAD: nothing todo! */
  }
}

void CStat::ShowCDF()
{
  printf("=> contabilizacao estatistica basica\n");
  printf("Cicles,ICount,IPCmedio,IInstrReuse,ITraceReuse,Ntraces,ITotalReuse,PPInstrReuse,PPTraceReuse,PPTotalReuse,Time\n");
  printf("%ld,%ld,%0.4f,%ld,%ld,%ld,%ld,%0.4f,%0.4f,%0.4f,%ld\n",
	 Cicles,
	 ICount,
	 ((Cicles  > 0) ? ((double) ICount / Cicles) : 0),
	 IReuse - ITraceReuse,
	 ITraceReuse,
	 ITrace,
	 IReuse,
	 ((ICount > 0) ? ((double) (IReuse - ITraceReuse) / ICount * 100.0) : 0),
	 ((ICount > 0) ? ((double) ITraceReuse / ICount * 100.0) : 0),
	 ((ICount > 0) ? ((double) IReuse / ICount * 100.0) : 0),
	 Time );
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// tabelas de descricao de faixas
  
  const char *tpFaixa [5] = { "IC_NORMAL", "IC_INST_REUSE", "IC_TRACE", "IC_TRACE_REUSE", "IC_NOT_FORMED" };
  const char *tpTerm [6] = { "TFN_OVERFLOW", "TFN_DELAYINST", "TFN_NVALID", "TFN_NREDUNDANT", "TFN_TRACE", "TFN_LOADSTORE" };

  //////////////////////////////////////////////////////////////////
  //// contabilizacao dos tipos de instrucoes

  printf("=> contabilizacao das instrucoes\n");
  printf("TPfaixa,IC_call,IC_bicc,IC_jmpl,IC_ticc,IC_load,IC_store,IC_arithmetic,IC_logic,IC_mult,IC_save,IC_restore,IC_sethi,IC_others\n");
  for(int i = 0; i < 4; i++) {
    printf("%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld\n",
	   tpFaixa[i],
	   IC_call[i],
	   IC_bicc[i],
	   IC_jmpl[i],
	   IC_ticc[i],
	   IC_load[i],
	   IC_store[i],
	   IC_arithmetic[i],
	   IC_logic[i],
	   IC_mult[i],
	   IC_save[i],
	   IC_restore[i],
	   IC_sethi[i],
	   IC_others[i] );
  }
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// contabilizacao do tamanho dos traces

  printf("=> contabilizacao do tamanho dos traces\n");
  printf("TPfaixa,TSZ_traces,TSZ_traces_instr,TSZ_medio,TSZ_maximo\n");
  for(int i = 0; i < 2; i++) {
    printf("%s,%ld,%ld,%0.4f,%ld\n",
	   tpFaixa[i + 2],
	   TSZ_traces[i],
	   TSZ_traces_instr[i],
	   ((TSZ_traces[i] > 0) ? ((double) TSZ_traces_instr[i] / TSZ_traces[i]) : 0),
	   TSZ_maximo[i]);
  }
  printf("\n");

  printf("=> frequencia dos traces por tamanho\n");
  printf("TPfaixa");
  for(int j = 0; j < TSZ_MAX; j++)
    printf(",%d", j + 2);
  printf("\n");
  for(int i = 0; i < 2; i++) {
    printf("%s", tpFaixa[i + 2]);
    for(int j = 0; j < TSZ_MAX; j++)
      printf(",%ld", TSZ_traces_per_size[j][i]);
    printf("\n");
  }
  printf("\n");

  printf("=> frequencia de traces contendo load por tamanho\n");
  printf("TPfaixa");
  for(int j = 0; j < TSZ_MAX; j++)
    printf(",%d", j + 2);
  printf("\n");
  for(int i = 0; i < 2; i++) {
    printf("%s", tpFaixa[i + 2]);
    for(int j = 0; j < TSZ_MAX; j++)
      printf(",%ld", TSZ_traces_load_per_size[j][i]);
    printf("\n");
  }
  printf("\n");

  printf("=> frequencia de traces contendo store por tamanho\n");
  printf("TPfaixa");
  for(int j = 0; j < TSZ_MAX; j++)
    printf(",%d", j + 2);
  printf("\n");
  for(int i = 0; i < 2; i++) {
    printf("%s", tpFaixa[i + 2]);
    for(int j = 0; j < TSZ_MAX; j++)
      printf(",%ld", TSZ_traces_store_per_size[j][i]);
    printf("\n");
  }
  printf("\n");

  printf("=> contabilizacao do total de desvios por trace\n");
  printf("TPfaixa");
  for(int j = 0; j < TSZ_MAX; j++)
    printf(",%d", j);
  printf("\n");
  for(int i = 0; i < 2; i++) {
    printf("%s", tpFaixa[i + 2]);
    for(int j = 0; j < TSZ_MAX; j++)
      printf(",%ld", TSZ_branches_per_trace[j][i]);
    printf("\n");
  }
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces
  
  printf("=> contabilizacao da forma de finalizacao dos traces\n");

  printf("TPfaixa");
  for(int j = 0; j < 6; j++)
    printf(",%s", tpTerm[j]);
  printf("\n");

  for(int i = 0; i < 3; i++) {
    printf("%s", tpFaixa[i + 2]);
    for(int j = 0; j < 6; j++)
      printf(",%ld", TFN_traces[j][i]);
    printf("\n");
  }

  printf("\n");

  /////////////////////////////////////////////////////////////
  //// contabilizacao do total de instrucoes de load que serviram traces

  printf("=> contabilizacao do total de instrucoes de load que serviram traces\n");
  printf("TLD_load,TLD_pp_load,TLD_reused_load,TLD_pp_reused_load,TLD_trace_load,TLD_pp_trace_load\n");
  printf("%ld,%0.4f,%ld,%0.4f,%ld,%0.4f\n",
	 TLD_traces_following_load, 
	 ((ITrace > 0) ? (((double) TLD_traces_following_load / ITrace) * 100.0) : 0),
	 TLD_traces_following_reused_load, 
	 ((ITrace > 0) ? (((double) TLD_traces_following_reused_load / ITrace) * 100.0) : 0),
	 TLD_traces_following_trace_load, 
	 ((ITrace > 0) ? (((double) TLD_traces_following_trace_load / ITrace) * 100.0) : 0));
  printf("\n");

}

// Show(): apresenta os resultados estatisticos registrados
void CStat::Show()
{

  //////////////////////////////////////////////////////////////////
  //// apresentacao dos resultados estatisticos

  printf("\n");

  printf("Statistics\n");
  printf("==========\n\n");

  printf("N. cicles = %ld\n", Cicles);
  printf("N. instructions = %ld\n", ICount);
  printf("IPC medio = %0.4f\n\n", (Cicles > 0) ? ((double) ICount / Cicles) : 0);
  printf("N. reused inst. - simples = %ld\n", IReuse - ITraceReuse);
  printf("N. reused inst. - traces = %ld\n", ITraceReuse);
  printf("N. traces = %ld\n", ITrace);
  printf("Total reuse = %ld\n\n", IReuse);
  printf("Reuse - simples (%%) = %0.4f\n", (ICount > 0) ? ((double) (IReuse - ITraceReuse) / ICount * 100.0) : 0);
  printf("Reuse - traces (%%) = %0.4f\n", (ICount > 0) ? ((double) ITraceReuse / ICount * 100.0) : 0);
  printf("Total Reuse (%%) = %0.4f\n", (ICount > 0) ? ((double) IReuse / ICount * 100.0) : 0);
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// tabelas de descricao de faixas
  
  const char *tpFaixa [5] = { "IC_NORMAL", "IC_INST_REUSE", "IC_TRACE", "IC_TRACE_REUSE", "IC_NOT_FORMED" };
  const char *tpTerm [6] = { "TFN_OVERFLOW", "TFN_DELAYINST", "TFN_NVALID", "TFN_NREDUNDANT", "TFN_TRACE", "TFN_LOADSTORE" };

  //////////////////////////////////////////////////////////////////
  //// contabilizacao dos tipos de instrucoes

  printf("=> contabilizacao das instrucoes\n");
  for(int i = 0; i < 4; i++) {
    printf("(%d) faixa = %s\n", i + 1, tpFaixa[i]);
    printf("  - instrucoes de desvios\n");
    printf("    call = %ld\n", IC_call[i]);
    printf("    bicc = %ld\n", IC_bicc[i]);
    printf("    jmpl = %ld\n", IC_jmpl[i]);
    printf("  - chamadas ao sistema operacional\n");
    printf("    ticc = %ld\n", IC_ticc[i]);
    printf("  - instrucoes de acesso a memoria\n");
    printf("    load = %ld\n", IC_load[i]);
    printf("    store = %ld\n", IC_store[i]);
    printf("  - instrucoes logica e arithmeticas\n");
    printf("    arithmetic = %ld\n", IC_arithmetic[i]);
    printf("    logic = %ld\n", IC_logic[i]);
    printf("  - instrucoes de multiplicacao\n");
    printf("    mult = %ld\n", IC_mult[i]);
    printf("  - instrucoes de save e restore\n");
    printf("    save = %ld\n", IC_save[i]);
    printf("    restore = %ld\n", IC_restore[i]);
    printf("  - instrucao sethi\n");
    printf("    sethi = %ld\n", IC_sethi[i]);
    printf("  - outras instrucoes\n");
    printf("    others = %ld\n", IC_others[i]);
  }
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// contabilizacao do tamanho dos traces

  printf("=> contabilizacao do tamanho dos traces\n");
  for(int i = 0; i < 2; i++) {
    printf("(%d) faixa = %s\n", i + 1, tpFaixa[i + 2]);
    printf("  - total de traces = %ld\n", TSZ_traces[i]);
    printf("  - total de instrucoes nos traces = %ld\n", TSZ_traces_instr[i]);
    printf("  - tamanho medio dos traces = %0.4f\n", 
	   (TSZ_traces[i] > 0) ? ((double) TSZ_traces_instr[i] / TSZ_traces[i]) : 0);
    printf("  - tamanho maximo dos traces = %ld\n", TSZ_maximo[i]);
    printf("  - frequencia dos traces por tamanho\n");
    for(int j = 0; j < TSZ_MAX; j++)
      printf("    %d instrucoes = %ld\n", j + 2, TSZ_traces_per_size[j][i]);
    printf("  - frequencia de traces contendo load por tamanho\n");
    for(int j = 0; j < TSZ_MAX; j++)
      printf("    %d instrucoes = %ld\n", j + 2, TSZ_traces_load_per_size[j][i]);
    printf("  - frequencia de traces contendo store por tamanho\n");
    for(int j = 0; j < TSZ_MAX; j++)
      printf("    %d instrucoes = %ld\n", j + 2, TSZ_traces_store_per_size[j][i]);
  }
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// contabilizacao do total de desvios por trace

  printf("=> contabilizacao do total de desvios por trace\n");
  for(int i = 0; i < 2; i++) {
    printf("(%d) faixa = %s\n", i + 1, tpFaixa[i + 2]);
    printf("  - frequencia dos desvios por trace\n");
    for(int j = 0; j < TSZ_MAX; j++)
      printf("    %d desvios = %ld\n", j, TSZ_branches_per_trace[j][i]);
  }
  printf("\n");

  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces
  
  printf("=> contabilizacao da forma de finalizacao dos traces\n");
  for(int i = 0; i < 3; i++) {
    printf("(%d) faixa = %s\n", i + 1, tpFaixa[i + 2]);
    for(int j = 0; j < 6; j++)
      printf("  - traces finalizados com '%s' = %ld\n", tpTerm[j], TFN_traces[j][i]);
  }
  printf("\n");

  /////////////////////////////////////////////////////////////
  //// contabilizacao do total de instrucoes de load que serviram traces

  printf("=> contabilizacao do total de instrucoes de load que serviram traces\n");
  printf("  - total/percental traces servidos por loads = %ld / %0.4f %%\n",
	 TLD_traces_following_load, 
	 (ITrace > 0) ? (((double) TLD_traces_following_load / ITrace) * 100.0) : 0);
  printf("  - total/percental traces servidos por loads reusados = %ld / %0.4f %%\n",
	 TLD_traces_following_reused_load, 
	 (ITrace > 0) ? (((double) TLD_traces_following_reused_load / ITrace) * 100.0) : 0);
  printf("  - total/percental traces servidos por traces com loads = %ld / %0.4f %%\n",
	 TLD_traces_following_trace_load, 
	 (ITrace > 0) ? (((double) TLD_traces_following_trace_load / ITrace) * 100.0) : 0);
  printf("\n");


  printf("Simulation time (sec) = %ld\n\n", Time);

  ShowCDF();
 
}

// Reset(): inicializa os contadores estatisticos
void CStat::Reset()
{

  Cicles = 0;

  ICount = 0;


  IReuse = 0; ITrace = 0;

  ITraceReuse = 0;


  iTime = 0; fTime = 0;

  Time = 0;


  for(int i = 0; i < 4; i++) {
  
    //////////////////////////////////////////////////////////////////
    //// contabilizacao dos tipos de instrucoes
    //// (0-instrucoes, 1-instrucoes reusadas, 2-reconhecidas em traces, 3-reusadas em traces)
    
    /* instrucoes de desvios */

    IC_call[i] = 0;

    IC_bicc [i] = 0;

    IC_jmpl [i] = 0;

    /* chamadas ao sistema operacional */

    IC_ticc[i] = 0;

    /* instrucoes de acesso a memoria */

    IC_load[i] = 0;

    IC_store[i] = 0;

    /* instrucoes logicas e aritimeticas */

    IC_arithmetic[i] = 0;

    IC_logic[i] = 0;

    /* instrucoes de multiplicacao */

    IC_mult[i] = 0;

    /* instrucoes de save/restore */

    IC_save[i] = 0;

    IC_restore[i] = 0;

    /* instrucao sethi */

    IC_sethi [i] = 0;

    /* outras instrucoes */

    IC_others[i] = 0;
  
  }

  for(int i = 0; i < 2; i++) {

    //////////////////////////////////////////////////////////////////
    //// contabilizacao do tamanho dos traces
    //// (0-reconhecidos, 1-reusados)

    TSZ_traces[i] = 0;       // total de traces identificados/reusados

    for(int j = 0; j < TSZ_MAX; j++) {
      TSZ_traces_per_size[j][i] = 0;        // frequencia de traces por tamanho
      TSZ_traces_load_per_size[j][i] = 0;   // frequencia de traces com load por tamanho
      TSZ_traces_store_per_size[j][i] = 0;  // frequencia de traces com store por tamanho
      TSZ_branches_per_trace[j][i] = 0;     // frequencia de instrucoes de desvio por trace
    }

    TSZ_traces_instr[i] = 0; // total de instrucoes pertencentes a traces identificados/reusados

    TSZ_maximo[i] = 0;       // tamanho maximo de trace identificado/reusado

  }

  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces
  //// (0-reconhecidos, 1-reusados)

  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 4; j++)
      TFN_traces[j][i] = 0;   // traces finalizados 'overflow' identificados/reusados
  }

  //////////////////////////////////////////////////////////////
  //// contabilizacao dos tempos de processamento de load/store
  
  T_total_load = 0;    // tempo total necessario para resolver instrucoes de load

  T_total_store = 0;   // tempo total necessario para resolver instrucoes de store


  /////////////////////////////////////////////////////////////
  //// contabilizacao do total de instrucoes de load que serviram traces

  TLD_traces_following_load = 0;       // total de traces servidos por load

}
