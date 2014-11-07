#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <curl/curl.h>
#include <jni.h>

#include "rtlbase.h"
#include "rtlimsg.h"

#include "common.h"
#include "admin_cli.h"
#include "header_t.h"                                                                               
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"
#include "http_clients.h"

#define ABS(x)    ((x) > 0 ? (x) : -(x))
#define MSTIC   100
#define        IM_DEF                  1000
#define        IM_TIMER_GEN            1001
#define        IM_TIMER_GEN_V          10000   // ms


int TraceLevel = 5;
int TraceDebug = 1;
int TraceProto = 0;
int PseudoOng = 0;
int SoftwareSensor  = 0;
int WithDia = 1;

void *MainTbPoll = NULL;
void *MainIQ = NULL;

void DoClockMs()
{
#if  0
  LOG(TRACE_INFO, "DoClockMs()\n");
#endif
  httpClientsClockMs();
}

void  DoClockSc()
{
  time_t now = 0;

  LOG(TRACE_INFO, "DoClockSc()");

  rtl_timemono(&now);
  AdmClockSc(now);
}

void  DoInternalEvent(t_imsg *imsg)
{
  LOG(TRACE_INFO, "receive event cl=%d ty=%d", imsg->im_class, imsg->im_type);
  switch (imsg->im_class)
  {
//  case <msg class> :
//      doSomething();
//    break;
    default:
      break;
  }
}

void  DoInternalTimer(t_imsg *imsg)
{
  LOG(TRACE_INFO, "receive timer cl=%d ty=%d vsize=%ld",
      imsg->im_class, imsg->im_type, rtl_vsize(getpid()));

  switch (imsg->im_class)
  {
//  case <msg class> :
//    doSomething();
//  break;
//
    default:
      break;
  }
}

void  MainLoop()
{
  time_t lasttimems = 0;
  time_t lasttimesc = 0;
  time_t now = 0;
  int ret = 0;

  t_imsg  *msg;

  while  (1)
  {
    // internal events
    while ((msg = rtl_imsgGet(MainIQ, IMSG_MSG)) != NULL)
    {
      DoInternalEvent(msg);
      rtl_imsgFree(msg);
    }

    // external events
    ret  = rtl_poll(MainTbPoll, MSTIC);

    // clocks
    now  = rtl_tmms();
    if  (ABS(now - lasttimems) >= 100)
    {
      DoClockMs();
      lasttimems  = now;
    }
    if  (ABS(now - lasttimesc) >= 1000)
    {
      DoClockSc();
      lasttimesc  = now;
    }

    // internal timer
    while ((msg = rtl_imsgGet(MainIQ, IMSG_TIMER)) != NULL)
    {
      DoInternalTimer(msg);
      rtl_imsgFree(msg);
    }
  }
}




void usage(char *prg, char *fmt)
{
  printf("usage: %s %s\n", prg, fmt);
  printf(" where options are:\n");
  printf(" -h          print this help message\n");
  printf(" -t <level>  run with trace level <level>\n");
  printf(" -d <level>  run with debug level <level>\n");
  printf("\n");
}



int DoArg(int argc, char *argv[])
{
  extern char *optarg;
  extern int optind;
  int  c;

  char *fmtgetopt  = "ht:d:or";

  int  i;

  for  (i = 1 ; i < argc ; i++)
  {
    if  (strcmp(argv[i], "--version") == 0)
    {
      printf("%s\n", rtl_version());
      exit(0);
    }
    if (strcmp(argv[i], "--help") == 0)
    {
      usage(argv[0], fmtgetopt);
      exit(0);
    }
  }

  while ((c = getopt (argc, argv, fmtgetopt)) != -1)
  {
    switch (c)
    {
      case  'h' :
        usage(argv[0], fmtgetopt);
        exit(0);
        break;
      case  't' :
        TraceLevel  = atoi(optarg);
        break;
      case  'd' :
        TraceDebug  = atoi(optarg);
        break;

      default :
        break;
    }
  }

  return  argc;
}


void  SetOption(char *name, char *value)
{
  if  (strcmp(name, "tracelevel") == 0)
  {
    TraceLevel  = atoi(value);
    return;
  }
  if  (strcmp(name, "tracedebug") == 0)
  {
    TraceDebug  = atoi(value);
    return;
  }

  LOG(TRACE_ERROR, "ERROR parameter/option '%s' not found\n", name);
}

int main(int argc, char *argv[])
{

  signal(SIGPIPE, SIG_IGN);

  rtl_init();

  DoArg(argc, argv);

  rtl_tracemutex();
  rtl_tracelevel(TraceLevel);
  rtl_tracesizemax(1024 * 500);
  //if (0 == logOnStdOutput)
  {
  rtl_tracerotate("./TRACE.log");
  }
  LOG(TRACE_ERROR, "start %s/main th=%lx pid=%d\n", argv[0], (long)pthread_self(), getpid());
  LOG(TRACE_ERROR, "%s\n", rtl_version());

  MainTbPoll = rtl_pollInit();
  MainIQ = rtl_imsgInitIq();

  rtl_tracelevel(TraceLevel);

  LOG(TRACE_ERROR, "TraceLevel=%d\n", TraceLevel);
  LOG(TRACE_ERROR, "TraceDebug=%d\n", TraceDebug);

  AdmTcpInit();
  httpClientsInit(5);

  MainLoop();

  httpClientsUninit();
  LOG(TRACE_ERROR, "end !!! %s/main th=%lx\n", argv[0], (long)pthread_self());
  exit(1);
}






