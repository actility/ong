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

#include "modbus-acy.h"

int TraceLevel = 3; //2;
int TraceDebug = 3; //1;
int TraceProto = 3; //0;
int PseudoOng = 0;
int WithDia = 1;
int Operating = 1;
int DiaTraceRequest = 0;

int ActiveSniffer = 0;
void *MainTbPoll;
void *MainIQ;
void *MdlCfg;

unsigned int  DiaIpuOk;
unsigned int  DiaNetOk;
unsigned int  DiaIpuTmt;
unsigned int  DiaNetTmt;
unsigned int  DiaIpuErr;
unsigned int  DiaNetErr;
unsigned int  DiaRepDrp;

char *rootactPath = NULL;

LIST_HEAD(NetworkInternalList);

char *
GetEnvVarName() {
  return NULL;
}

/*!
  *
  * @brief calculate the next serial number which can be used to identity any 
  * object/structure/messsage. 0 is not a valid serial number. Mainly used for
  * t_sensor structure to identity sensors in conjonction with a sensor number.
  * @return [1..INT_MAX]
  * 
  */
int DoSerial()
{
  static int Serial = INT_MAX;

  if (Serial == INT_MAX) {
    Serial = 1;
  } else {
    Serial++;
  }

  return Serial;
}

/*!
  *
  * @brief called by main loop every 100 ms.
  * @return void
  * 
  */
void DoClockMs()
{
  modbusReaderClockMs();
}

/*!
  *
  * @brief called by main loop every 1 second.
  * @return void
  * 
  */
void DoClockSc()
{
   static unsigned int nbclock = 0;
   time_t now = 0;

   nbclock++;
   //time(&now);
   rtl_timemono(&now);

   AdmClockSc(now);
   DiaClockSc(now);
   ModbusSensorClockSc(now);
}

/*!
  *
  * @brief called by main loop to dispatch an internal message
  * @return void
  * 
  */
void
DoInternalEvent(t_imsg *imsg)
{
  RTL_TRDBG(TRACE_IMPL, "receive event cl=%d ty=%d\n", imsg->im_class, imsg->im_type);
}

/*!
  *
  * @brief called by main loop to dispatch an internal timer
  * @return void
  * 
  */
void
DoInternalTimer(t_imsg *imsg)
{
  RTL_TRDBG(TRACE_IMPL, "receive timer cl=%d ty=%d vsize=%ld\n", imsg->im_class, imsg->im_type, rtl_vsize(getpid()));

  switch(imsg->im_class) {
    case IM_DEF :
      switch(imsg->im_type) {
        case IM_TIMER_GEN:
          rtl_imsgAdd(
          MainIQ,
          rtl_timerAlloc(IM_DEF, IM_TIMER_GEN, IM_TIMER_GEN_V, NULL, 0)
          );
          break;
      }
      break;
  }
}

