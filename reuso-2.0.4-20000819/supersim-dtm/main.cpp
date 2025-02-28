
/*
 * main.h
 * Copyright (C) 1999 by Luiz Marcio F A Viana, 6/1/99
 */

#include<stdio.h>
#include<string.h>
#include"all.h"

extern str_t errtile;

////////////////////////////////////////////////////////////
///// CMainApp

void CMainApp::CopyrightMsg()
{
  printf("\n\nSuperSIM (R) 1.0 - Instruction Decode (ID) Analise");
  printf("\nCopyright (C) 1999 by Luiz Marcio F A Viana, 6/1/99");
  printf("\n\n");
}

CMainApp::CMainApp()
{
  CopyrightMsg();
  ShowOptions = OPT_SHOW_DISABLE;
  cstdin = stdin;
}

int CMainApp::GetUserInput(char* s, int n)
{
  int c;
  printf("\n%s: ", s);
  do {
    c = fgetc(cstdin);
    c = UPPER(c);
#if(DEBUG == 0)
    printf("\nMAINAPP::PRESSED(%d)", c);
#endif
    if((c == -1) && (cstdin != stdin)) {
	fclose(cstdin);
	cstdin = stdin;
    }
    else {
      if( (c != 10) && ((c < 'A') || (c > ('A' + n))) ) {
	printf("\nERR: Invalid option.\n");
	printf("\n%s:", s);
      }
    }
  } while( (c < 'A') || (c > ('A' + n)) );
  return (c - 'A');
}

uword_t CMainApp::SetShowRs(int opt)
{
  return ( ShowRs = (opt == OPT_SHOW_DISABLE) ? 0 : ShowRs | (1 << (opt - 1)) );
}

uword_t CMainApp::GetShowRs()
{
  return ShowRs;
}

uword_t CMainApp::SetShowOptions(int opt)
{
  return ( ShowOptions = (opt == OPT_SHOW_DISABLE) ? 0 : ShowOptions | (1 << (opt - 1)) );
}

void CMainApp::SetStdin(char* filename)
{
  if((cstdin = fopen(filename, "r")) == 0) {
    sprintf(errtile, "(=%s)", filename);
    errmsg(__MAIN_H, ERR_CANTFINDSCRIPT, errtile);
  }
}

uword_t CMainApp::GetShowOptions()
{
  return ShowOptions;
}

void CMainApp::Run()
{
  int opt1, opt2;

  str_t s;
  int n;

  int tmp_args;

  opt1 = -1;
  while(opt1 != OPT_QUIT) {
    opt1 = GetUserInput((char*)"A)Quit B)Load C)Args D)Reset E)Run F)Runn G)Step H)Show I)Watch J)Re-display K)Breakpt L)Trace M)Reset Stat N)Dtm Ctrl", OPT_DTMCTRL);
    switch(opt1) {
      case OPT_LOAD: {
        printf("\nFile to load: ");
        fscanf(cstdin, "%s", s);
        Cpu.SetFileName(s);
        Cpu.Load();
        Cpu.Wb.SetTraceFile(s);
        break;
      }
      case OPT_ARGS: {
        strcpy(Cpu.Argv[0], Cpu.FileName);
        tmp_args = 1;
        while(tmp_args < ARGNUM - 1) {	
          printf("\nArg[%i]: ", tmp_args);
          fgets(s, ARGLEN, cstdin);
          s[ARGLEN - 1] = '\0';
          if(s[strlen(s) - 1] == '\n')
            s[strlen(s) - 1] = '\0';
          if(s[0] == '.') break;
          if(s[0] != '\0') {
            strcpy(Cpu.Argv[tmp_args], s);
            tmp_args += 1;
          }
        }
#ifdef BIGENDIAN
        (* Cpu.Argc) = LB_WORD(tmp_args);
#else
        (* Cpu.Argc) = tmp_args;
#endif
        break;
      }
      case OPT_RESET: {
        Cpu.Reset();
        printf("\nReset done.");
        break;
      }
      case OPT_RUN: {
        printf("\nRunning...");
        Cpu.Run();
        Show();
        printf("\nExecution completed.");
        break;
      }
      case OPT_RUNN: {
        printf("\nInput number of cicles to execute: ");
        fscanf(cstdin, "%i", & n);
        printf("\nRunning...");
        Cpu.SetNumSteps(n);
        Cpu.Runn();
        Show();
        printf("\nExecution completed.");
        break;
      }
      case OPT_STEP: {
        Cpu.Step();
        printf("\n(*%lu)\n", (uword_t) Cpu.GetNumCicles());
        Show();
        break;
      }
      case OPT_REDISPLAY: {
        printf("\n(*%lu)\n", (uword_t) Cpu.GetNumCicles());
        Show();
        break;
      }
      case OPT_SHOW: {
        opt2 = GetUserInput((char*)" >> A)Disable B)Program C)Args D)Mem E)BTB F)Fetch G)IQueue H)Disq I)DBDisq\n    J)IReg K)SReg L)Rs M)Iu N)Lsu O)Bpu P)Maq Q)Rob R)Comp.Trace S)Stat T)Dtm", OPT_SHOW_DTM);
        SetShowOptions(opt2);
        switch(opt2) {
          case OPT_SHOW_MEM: {
            printf("\nMemory address: ");
            fscanf(cstdin, "%li", & ShowMemAddr);
            break;
          }
          case OPT_SHOW_PROGRAM: {
            ShowFollowPC = GetUserInput((char*)" >> A)Base Address B)Follow PC", OPT_SHOW_PROGRAM_FOLLOWPC);
            if(ShowFollowPC == OPT_SHOW_PROGRAM_BASEADDR) {
              printf("\nBase program address: ");
              fscanf(cstdin, "%li", & ShowProgAddr);
            }
          	break;
          }
          case OPT_SHOW_RS: {
            SetShowRs( GetUserInput((char*)" >> A)DISABLE B)IU C)BPU D)LSU", OPT_SHOW_RS_LSU) );
            break;
          }
          case OPT_SHOW_IREG: {
            ShowIReg = GetUserInput((char*)" >> A)ALL B)BUSY ONLY", OPT_SHOW_IREG_BUSY);
            break;
          }
          case OPT_SHOW_SREG: {
            ShowSReg = GetUserInput((char*)" >> A)ALL B)BUSY ONLY", OPT_SHOW_SREG_BUSY);
            break;
          }
          case OPT_SHOW_DTM: {
          	ShowDtm = GetUserInput((char*)" >> A)DISABLE B)MEMO_TABLE_G C)MEMO_TABLE_T", OPT_SHOW_DTM_T);
          }
        }
        break;
      }
      case OPT_WATCH: {
        printf("\nCwp <%lu> = ", (Cpu.IReg).GetCurrentWin());
        fscanf(cstdin, "%li", & RegWin);
        printf("\nReg address = ");
        fscanf(cstdin, "%li", & RegAddr);
        (Cpu.IReg).Watch(RegWin, RegAddr);
        break;
      }
      case OPT_BREAKPT: {
        printf("\nMemory address: ");
        fscanf(cstdin, "%i", & n);      
        Cpu.SetBreakpt(n);
        break;
      }
      case OPT_TRACE: {
        switch(GetUserInput((char*)" >> A)Disable B)Instructions C)Basic Blocks D) SIMSparc", OPT_TRACE_SIMSPARC)) {
          case OPT_TRACE_DISABLE: {
            Cpu.Wb.DisableTrace(WTRACE_INSTRUCTIONS | WTRACE_BASICBLOCKS | WTRACE_SIMSPARC);
            break;
          }
          case OPT_TRACE_INSTRUCTIONS: {
            Cpu.Wb.EnableTrace(WTRACE_INSTRUCTIONS);
            break;
          }
          case OPT_TRACE_BASICBLOCKS: {
            Cpu.Wb.EnableTrace(WTRACE_BASICBLOCKS);
            break;
          }
          case OPT_TRACE_SIMSPARC: {
            Cpu.Wb.EnableTrace(WTRACE_SIMSPARC);
          }
        }
        break;
      }
      case OPT_RSTSTAT: {
        (Cpu.Stat).Reset();
        break;
      }
      case OPT_DTMCTRL: {
        if( !Cpu.Dtm.IsEnabled() ) {
          Cpu.Dtm.SetEnabled();
          printf("\nDTM is enable now.");
        }
        else {
          printf("DTM was already enabled.");
        }
      }
    }
  }
  printf("\n");
}

void CMainApp::Show()
{
  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_PROGRAM - 1)) ) {
    if(ShowFollowPC)
      (Cpu.Mem).ShowProg((Cpu.Fetch).RegPC);
    else
      (Cpu.Mem).ShowProg(ShowProgAddr);
  }

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_ARGS - 1)) )
    Cpu.ShowArgs();
  
  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_MEM - 1)) )
    (Cpu.Mem).Show(ShowMemAddr);
  
  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_BTB - 1)) )
    (Cpu.Btb).Show();
  
  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_FETCH - 1)) ) {
    (Cpu.FetchBuf).Show();
    (Cpu.Fetch).Show();
  }
  
  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_IQUEUE - 1)) )
    (Cpu.IQueue).Show();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_DISQ - 1)) ) {
    printf("\nDispatch queue");
    printf("\n==============\n");
    (Cpu.Disq).Show();
  }

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_DBDISQ - 1)) ) {
    printf("\nBranch dependence instruction queue");
    printf("\n===================================\n");
    (Cpu.DBDisq).Show();
  }

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_IREG - 1)) ) {
    if(ShowIReg == OPT_SHOW_IREG_ALL)
      (Cpu.IReg).Show();
    else
      (Cpu.IReg).Show2();
  }

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_SREG - 1)) ) {
    if(ShowSReg == OPT_SHOW_SREG_ALL)
      (Cpu.SReg).Show();
    else
      (Cpu.SReg).Show2();
  }

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_RS - 1)) ) {
    if( BITTEST(ShowRs, 1 << (OPT_SHOW_RS_IU - 1)) ) {
      printf("\n\n[ IU ]\n");
      (Cpu.RsIu).Show();
    }

    if( BITTEST(ShowRs, 1 << (OPT_SHOW_RS_BPU - 1)) ) {
      printf("\n\n[ BPU ]\n");
      (Cpu.RsBpu).Show();
    }

    if( BITTEST(ShowRs, 1 << (OPT_SHOW_RS_LSU - 1)) ) {
      printf("\n\n[ LSU ]\n");
      (Cpu.RsLsu).Show();
    }

  }

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_IU - 1)) )
    (Cpu.Iu).Show();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_LSU - 1)) )
    (Cpu.Lsu).Show();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_BPU - 1)) )
    (Cpu.Bpu).Show();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_MAQ - 1)) )
    (Cpu.Maq).Show();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_ROB - 1)) )
    (Cpu.Rob).Show();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_CTRACE - 1)) )
    (Cpu.Wb).ShowTrace();

  if( BITTEST(ShowOptions, 1 << (OPT_SHOW_STAT - 1)) )
    (Cpu.Stat).Show();

  if( BITTEST(ShowDtm, 1 << (OPT_SHOW_DTM_G - 1)) )
    (Cpu.Dtm).ShowMemoTableG();

  if( BITTEST(ShowDtm, 1 << (OPT_SHOW_DTM_T - 1)) )
    (Cpu.Dtm).ShowMemoTableT();
}

/* funcao principal */

int main(int argc, char* argv[])
{
#if(DEBUG == 0)
  printf("\n[ MAIN::PASS_1 ]");
#endif
  CMainApp app;
#if(DEBUG == 0)
  printf("\n[ MAIN::PASS_2 ]");
#endif
  if(argc > 1)
    app.SetStdin(argv[1]);
  app.Run();
  return(0);
}
