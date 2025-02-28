
/*
 * error.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 4/1/99
 */

#ifndef __ERROR_H
#define __ERROR_H 102

/* definicao das mensagens de erro da aplicacao
 */

#define ERR_INVALIDPARAM       "Parametro invalido"
#define ERR_INSUFICIENTPARAM   "Parametros insuficientes"
#define ERR_CANTOPENFILE       "Nao foi possivel abrir o arquivo"
#define ERR_FILETRUNCATED      "Arquivo truncado"
#define ERR_LOADOUTOFMEMORY    "Falta de memoria para carregar o arquivo"
#define ERR_ACCESSOUTOFMEM     "Acesso fora da regiao de memoria"
#define ERR_CANTALLOCMEM       "Nao foi possivel alocar memoria"
#define ERR_INVREGISTER        "Registrador invalido"
#define ERR_SYSCALLREQOUTOFMEM "Syscall acessa regiao de memoria invalida"
#define ERR_BRANCHAFTERBRANCH  "Instrucoes de desvio consecutivas"
#define ERR_QUEUEFULL          "Fila cheia"
#define ERR_QUEUEEMPTY         "Fila vazia"
#define ERR_WINUNDERFLOW       "Underflow -- uso indevido da janela de registradores"
#define ERR_WINOVERFLOW        "Overflow -- uso indevido da janela de registradores"
#define ERR_REGISTERUPDATEFAIL "Falha na atualizacao do registrador destino"
#define ERR_OUTOFBOUNDS        "Indice fora dos limites"
#define ERR_CANTFINDSCRIPT     "Nao foi possivel abrir o arquivo de script"
#define ERR_ROBINVVALUE        "Valor incompativel de nivel de desvio no reorder buffer"
#define ERR_NOTIMPLEMENTED     "Instrucao nao implementada no topo do buffer de reordenacao"
#define ERR_CANTOPENTRACEFILE  "Nao foi possivel abrir o arquivo de saida do trace"
#define ERR_TRACEFILENOTOPEN   "Arquivo de saida do trace nao esta aberto"
#define ERR_SIMARCHFAIL        "Falha arquitetural do simulador"

/* definicao das mensagens de warning da aplicacao
 */

#define WARN_NOTIMPLEMENTED        "Instrucao nao implementada"
#define WARN_INVINSTRUCTION        "Codigo de instrucao invalido"
#define WARN_SYSCALLNOTIMPLEMENTED "Syscall nao implementada"
#define WARN_KILLSYSCALL           "Kill syscall"
#define WARN_EXITSYSCALL           "Exit syscall"
#define WARN_PIPESYSCALL           "Pipe syscall"
#define WARN_DUPSYSCALL            "Dup syscall"
#define WARN_DUP2SYSCALL           "Dup2 syscall"
#define WARN_GETCNTXTSYSCALL       "Getcntxt syscall"
#define WARN_SBRKSYSCALL           "Sbrk syscall"
#define WARN_UTIMESYSCALL          "Utime syscall"
#define WARN_FCHDIRSYSCALL         "Fchdir syscall"
#define WARN_FCHROOTSYSCALL        "Fchroot syscall"
#define WARN_USTATSYSCALL          "Ustat syscall"
#define WARN_SIGSTACKSYSCALL       "Sigstack syscall"

/* declaracao da funcao de erro da aplicacao
 */

void errmsg(int _foo, const char* _err, const char* _msg);

void warnmsg(int _foo, const char* _err, const char* _msg);

#endif
