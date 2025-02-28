
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


  long IReuse;           // total de instrucoes reusadas pelo mecanismo

  long ITraceReuse;      // total de instrucoes reusadas por traces


  long ITrace;           // total de traces reusados


  long iTime;            // valor inicial do relogio

  long fTime;            // valor final do relogio


  long Time;             // total de segundos gastos


  //////////////////////////////////////////////////////////////////
  //// contabilizacao dos tipos de instrucoes
  //// (0-instrucoes, 1-instrucoes reusadas, 2-reconhecidas em traces, 3-reusadas em traces)

  /* instrucoes de desvios */

  long IC_call [4];

  long IC_bicc [4];

  long IC_jmpl [4];

  /* chamadas ao sistema operacional */

  long IC_ticc [4];

  /* instrucoes de acesso a memoria */

  long IC_load [4];

  long IC_store [4];

  /* instrucoes logicas e aritimeticas */

  long IC_arithmetic [4];

  long IC_logic [4];

  /* instrucoes de multiplicacao */

  long IC_mult [4];

  /* instrucoes de save/restore */

  long IC_save [4];

  long IC_restore [4];

  /* instrucao sethi */

  long IC_sethi [4];

  /* outras instrucoes */

  long IC_others [4];


  //////////////////////////////////////////////////////////////////
  //// contabilizacao do tamanho dos traces
  //// (0-reconhecidos, 1-reusados)

  long TSZ_traces [2];       // total de traces identificados/reusados

  long TSZ_traces_per_size [TSZ_MAX] [2];   // frequencia de traces por tamanho

  long TSZ_traces_load_per_size [TSZ_MAX] [2];   // frequencia de traces com load por tamanho

  long TSZ_traces_store_per_size [TSZ_MAX] [2];  // frequencia de traces com store por tamanho

  long TSZ_branches_per_trace [TSZ_MAX] [2];     // frequencia de instrucoes de desvio por trace

  long TSZ_traces_instr [2]; // total de instrucoes pertencentes a traces identificados/reusados

  long TSZ_maximo [2];       // tamanho maximo de trace identificado/reusado


  //////////////////////////////////////////////////////////////////
  //// contabilizacao da forma de finalizacao dos traces
  //// (0-reconhecidos, 1-reusados, 2-nao formados)

  long TFN_traces [6] [3];   // TFN_OVERFLOW = 'overflow'
                             // TFN_DELAYINST = 'delay instruction'
                             // TFN_NVALID = 'instrucao nao valida'
                             // TFN_NREDUNDANT = 'instrucao nao redundante'
                             // TFN_TRACE = 'trace redundante'
                             // TFN_LOADSTORE = 'instrucao de acesso a memoria'


  //////////////////////////////////////////////////////////////
  //// contabilizacao dos tempos de processamento de load/store

  long T_total_load;    // tempo total necessario para resolver instrucoes de load

  long T_total_store;   // tempo total necessario para resolver instrucoes de store


  /////////////////////////////////////////////////////////////
  //// contabilizacao do total de instrucoes de load que serviram traces

  long TLD_traces_following_load;          // total de traces servidos por load

  long TLD_traces_following_reused_load;   // total de traces servidos por load reusados

  long TLD_traces_following_trace_load;    // total de traces servidos por traces com load


public:

  CStat();

  void IncCicle();

  void IncICount();

  void IncICount(uword_t _n);

  void IncIReuse(uword_t _n);

  void StartClock();

  void StopClock();

  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _execunit - unidade de execucao
  // _instop - opcode da instrucao
  void AddInstrCount(uword_t _execunit, uword_t _instop);

  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _execunit - unidade de execucao
  // _instop - opcode da instrucao
  // _numinstr - numero de instrucoes (>1 para traces)
  void AddInstrCount(uword_t _execunit, uword_t _instop, long _numinstr);

  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _execunit - unidade de execucao
  // _instop - opcode da instrucao
  // _numinstr - numero de instrucoes (>1 para traces)
  // _tpfaixa - tipo de faixa (0-NORMAL, 1-INST_REUSE, 2-TRACE, 3-TRACE_REUSE)
  void AddInstrCount(uword_t _execunit, uword_t _instop, long _numinstr, int _tpfaixa);

  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _stats - estatistica a ser contabilizada emforma de estrutura 'statistics_t'
  void AddInstrCount(statistics_t * _stats);  

  // AddInstrCount(): contabilizacao dos tipos de instrucoes por faixa
  // _stats - estatistica a ser contabilizada emforma de estrutura 'statistics_t'
  // _tpfaixa - tipo de faixa (0-NORMAL, 1-INST_REUSE, 2-TRACE, 3-TRACE_REUSE)
  void AddInstrCount(statistics_t * _stats, int _tpfaixa);  

  // AddTraceSz():  contabilizacao do tamanho dos traces por faixa
  // _tracesz - tamanho do trace
  // _tpfaixa - tipo de faixa (0-TRACE, 1-TRACE_REUSE)
  // _stats - estatistica a ser contabilizada emforma de estrutura 'statistics_t'
  void AddTraceSz(long _tracesz, int _tpfaixa);

  // AddTraceSz():  contabilizacao do tamanho dos traces por faixa
  // _tracesz - tamanho do trace
  // _tpfaixa - tipo de faixa (0-TRACE, 1-TRACE_REUSE)
  // _stats - estatistica a ser contabilizada emforma de estrutura 'statistics_t'
  void AddTraceSz(long _tracesz, int _tpfaixa, statistics_t * _stats);

  // AddTraceTerm(): contabiliza a forma de finalizacao dos traces
  // _tpterm - tipo de finalizacao (TFN_OVERFLOW, TFN_DELAYINST, TFN_NVALID, TFN_NREDUNDANT)
  // _tpfaixa - tipo de faixa (0-TRACE, 1-TRACE_REUSE, 2-NOT_FORMED)
  void AddTraceTerm(uword_t _tpterm, int _tpfaixa);

  // AddLoadStoreDelay(): contabiliza o tempo de processamento de load/store
  // _instropcode - opcode da instrucao
  // _ncicles - tempo de espera em ciclos
  void AddLoadStoreDelay(long _instropcode, long _ncicles);

  // AddTraceFollowingLoad(): contabiliza o total de instrucoes de load que servem traces
  // _tldtype - indicador de instrucao fonte reusada (TLD_NOLOAD, TLD_INST_LOAD, TLD_REUSED_LOAD, TLD_TRACE_LOAD)
  void AddTraceFollowingLoad(int _tldtype);

  // ShowCDF(): apresenta os resultados estatisticos registrados no formato CDF
  void ShowCDF();

  // Show(): apresenta os resultados estatisticos registrados
  void Show();

  // Reset(): inicializa os contadores estatisticos
  void Reset();

};

#endif
