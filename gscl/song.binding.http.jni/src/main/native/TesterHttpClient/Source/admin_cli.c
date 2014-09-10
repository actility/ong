
#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <pthread.h>
#include <jni.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <curl/curl.h>

#include "rtlbase.h"
#include "rtllist.h"
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

#define FD_MAX    CLI_MAX_FD
#define TCP_FRAME_SZ  1024

static char *g_defaultMethod = NULL;
static char *g_defaultHeaderName1 = NULL;
static char *g_defaultHeaderValue1 = NULL;
static unsigned char *g_defaultContent = NULL;
static int g_defaultContentLen = 0;
static int g_urlRInThreadLen = 0;
char **g_szUrlRInThread = NULL;

const char c_szUrlRInThread[] = "http://10.10.12.65:8181/songBindingHttpTester/myServlet";
const char c_defaultMethod[] = "POST";
const char c_defaultHeaderName1[] = "Authorization";
const char c_defaultHeaderValue1[] = "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm0vYXBwbGljYXRpb25zL1NZU1RFTTo=";

const unsigned char c_defaultContent[] = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?> <m2m:application xmlns:m2m=\"http://uri.etsi.org/m2m\" appId=\"APP_00137a00000080ea.1\"> <m2m:expirationTime>9999-12-31T09:59:59.999+01:00</m2m:expirationTime> <m2m:accessRightID>/m2m/accessRights/APP_00137a00000080ea.1_AR/</m2m:accessRightID> <m2m:searchStrings> <m2m:searchString>ETSI.ObjectType/ETSI.AN_APP</m2m:searchString> <m2m:searchString>ETSI.ObjectTechnology/ZigBee.ZigBee_1_0</m2m:searchString> <m2m:searchString>ETSI.InterworkingProxyID/IPU_ZigBee</m2m:searchString> <m2m:searchString>ETSI.NetworkID/NW_00124b00015a68e4</m2m:searchString> <m2m:searchString>ETSI.NodeID/DEV_00137a00000080ea</m2m:searchString> <m2m:searchString>ZigBee.ProfileID/0x0104</m2m:searchString> <m2m:searchString>ZigBee.DeviceID/0x0009</m2m:searchString> <m2m:searchString>ZigBee.ClusterID/server/0x0000</m2m:searchString> <m2m:searchString>ETSI.InterfaceID/Basic.Srv</m2m:searchString> <m2m:searchString>ZigBee.ClusterID/server/0x0003</m2m:searchString> <m2m:searchString>ETSI.InterfaceID/Identify.Srv</m2m:searchString> <m2m:searchString>ZigBee.ClusterID/server/0x0006</m2m:searchString> <m2m:searchString>ETSI.InterfaceID/OnOff.Srv</m2m:searchString> <m2m:searchString>ZigBee.ClusterID/server/0x0702</m2m:searchString> <m2m:searchString>ETSI.InterfaceID/SimpleMetering.Srv</m2m:searchString> </m2m:searchStrings> <m2m:creationTime>2014-04-11T15:02:27.374+02:00</m2m:creationTime> <m2m:lastModifiedTime>2014-04-11T15:02:27.374+02:00</m2m:lastModifiedTime> <m2m:aPoC>coap://[::1]:5685/zigbee/APP_00137a00000080ea.1</m2m:aPoC> <m2m:aPoCPaths> <m2m:aPoCPath> <m2m:path>/m2m/applications/APP_00137a00000080ea.1/retargeting1</m2m:path> <m2m:accessRightID>/m2m/accessRights/Locadmin_AR2</m2m:accessRightID> </m2m:aPoCPath> <m2m:aPoCPath> <m2m:path>/m2m/applications/APP_00137a00000080ea.1/retargeting2</m2m:path> <m2m:accessRightID>/m2m/accessRights/APP_00137a00000080ea.1_AR2</m2m:accessRightID> </m2m:aPoCPath> <m2m:aPoCPath> <m2m:path>/m2m/applications/APP_00137a00000080ea.1/retargeting3</m2m:path> <m2m:accessRightID>/m2m/accessRights/APP_00137a00000080ea.1_AR3</m2m:accessRightID> </m2m:aPoCPath> </m2m:aPoCPaths> <m2m:containersReference>/m2m/applications/APP_00137a00000080ea.1/containers/</m2m:containersReference> <m2m:groupsReference>/m2m/applications/APP_00137a00000080ea.1/groups/</m2m:groupsReference> <m2m:accessRightsReference>/m2m/applications/APP_00137a00000080ea.1/accessRights/</m2m:accessRightsReference> <m2m:subscriptionsReference>/m2m/applications/APP_00137a00000080ea.1/subscriptions/</m2m:subscriptionsReference> <m2m:notificationChannelsReference>/m2m/applications/APP_00137a00000080ea.1/notificationChannels/</m2m:notificationChannelsReference> </m2m:application>";


const char *c_szUrlTimeout = "http://10.10.12.216:9999/"; // timeout
const char *c_szUrlConnectionFailure = "http://10.10.12.216:6666/"; // connection error
 
extern void *MainTbPoll;

t_cli *TbCli[FD_MAX];

static  int FdListen = -1;
static  int TcpKeepAlive  = 1;


static void doNewReqCmd(t_cli *cl, char *buf);
static void doSetUrlCmd(t_cli *cl, char *buf);
static void doAddHeaderCmd(t_cli *cl, char *buf);
static void doSendReqCmd(t_cli *cl, char *buf);
static void doR1Cmd(t_cli *cl, char *buf);
static void doR2Cmd(t_cli *cl, char *buf);
static void doR3Cmd(t_cli *cl, char *buf);
static void doR4Cmd(t_cli *cl, char *buf);
static void doR100Cmd(t_cli *cl, char *buf);
static void doReqsCmd(t_cli *cl, char *buf);
static void doStatusCmd(t_cli *cl, char *buf);
static void doReleaseCmd(t_cli *cl, char *buf);
static void doRInThreadCmd(t_cli *cl, char *buf);
static void doShowRITCmd(t_cli *cl, char *buf);
static void doSetRITMethodCmd(t_cli *cl, char *buf);
static void doSetRITHeaderCmd(t_cli *cl, char *buf);
static void doSetRITContentCmd(t_cli *cl, char *buf);
static void doAddRITUrlCmd(t_cli *cl, char *buf);
static void doRemoveRITUrlCmd(t_cli *cl, char *buf);
static void doRInThreadVerboseCmd(t_cli *cl, char *buf);
static void doRInThreadStatsCmd(t_cli *cl, char *buf);
static void localSendReq(t_cli * cl, unsigned long nbReq, char *url);


///////////////////////////////////////////////////////////////////////////////

static void AdmHelp(t_cli *cl)
{
  AdmPrint(cl, "help, ?     : display this help message\n");
  AdmPrint(cl, "exit, quit  : end the current CLI session\n");
  AdmPrint(cl, "core        : do generate a core file of the current process\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "newReq <method> <transId>: create a new request, with provided transId.\n");
  AdmPrint(cl, "setUrl <transId> <url>: set the URL.\n");
  AdmPrint(cl, "addHeader <transId> <name> <value>: add a header.\n");
  AdmPrint(cl, "sendReq <transId>: send the request.\n");
  AdmPrint(cl, "status: dump status for each pending transaction.\n");
  AdmPrint(cl, "r1: create and send a request with URL http://www.example.com/.\n");
  AdmPrint(cl, "r2: create and send a request with URL http://rms.poc1.actility.com:8080/m2m.\n");
  AdmPrint(cl, "r3: create and send a request with URL http://10.10.12.216:9090/.\n");
  AdmPrint(cl, "r4: create and send a request as done in rInThread.\n");
  AdmPrint(cl, "r100 [<url>]: create and send 100 requests. Optionally one can provides\n"
    "              an URL, otherwise http://www.example.com/ is assumed.\n");
  AdmPrint(cl, "reqs: create and send requests with various URLs.\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "rInThread: start a thread, in which we create and send requests\n"
               "           with various URLs, during a long period of time.\n");
  AdmPrint(cl, "showRIT: display params used for rInThread command\n");
  AdmPrint(cl, "setRITMethod <method>: set the method param used for rInThread command\n");
  AdmPrint(cl, "setRITHeader <name> <value>: set the header param used for rInThread command\n");
  AdmPrint(cl, "setRITContent <file>: load the request content from a file "
               "                      and use it as content for rInThread command\n");
  AdmPrint(cl, "addRITUrl <url>: add url\n");
  AdmPrint(cl, "removeRITUrl <index>: remove url at index <index> (first one is 0)\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "verbose <0|1>: disable/enable verbose mode while running rInThread command.\n");
  AdmPrint(cl, "stats: display statistics regarding rInThread run.\n");
  AdmPrint(cl, "release <transId>: release the resources for a transaction.\n");
  AdmPrint(cl, "\n");
}



/**********************************************************/
/* SUPPORTED COMMANDS */
static  int AdmCommand(t_cli *cl, char *buf)
{

  if  (!strlen(buf))
  {
    return  CLI_PROMPT;
  }

  if ((0 == strcmp(buf, "help")) || (0 == strcmp(buf, "?")))
  {
    AdmHelp(cl);
    return CLI_PROMPT;
  }
  if ((0 == strcmp(buf, "quit")) || (0 == strcmp(buf, "exit")))
  {
    return CLI_DISC;
  }
  if (0 == strcmp(buf, "core"))
  {
    rtl_docore();
    return CLI_PROMPT;
  }
  /////////////////////////// Tester specific commands ////////////////////////
  if (0 == strncmp(buf, "newReq ", strlen("newReq ")))
  {
    doNewReqCmd(cl, buf);
    return CLI_PROMPT;
  }
  if (0 == strncmp(buf, "setUrl ", strlen("setUrl ")))
  {
    doSetUrlCmd(cl, buf);
    return CLI_PROMPT;
  }
  if (0 == strncmp(buf, "addHeader ", strlen("addHeader ")))
  {
    doAddHeaderCmd(cl, buf);
    return CLI_PROMPT;
  }
  if (0 == strncmp(buf, "sendReq ", strlen("sendReq ")))
  {
    doSendReqCmd(cl, buf);
    return CLI_PROMPT;
  }
  if (0 == strncmp(buf, "release ", strlen("release ")))
  {
    doReleaseCmd(cl, buf);
    return CLI_PROMPT;
  }
  if (0 == strcmp(buf, "r1"))
  {
    doR1Cmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "r2"))
  {
    doR2Cmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "r3"))
  {
    doR3Cmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "r4"))
  {
    doR4Cmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "rInThread"))
  {
    doRInThreadCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "showRIT"))
  {
    doShowRITCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strncmp(buf, "setRITMethod ", strlen("setRITMethod ")))
  {
    doSetRITMethodCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strncmp(buf, "setRITHeader ", strlen("setRITHeader ")))
  {
    doSetRITHeaderCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strncmp(buf, "setRITContent ", strlen("setRITContent ")))
  {
    doSetRITContentCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strncmp(buf, "addRITUrl ", strlen("addRITUrl ")))
  {
    doAddRITUrlCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strncmp(buf, "removeRITUrl ", strlen("removeRITUrl ")))
  {
    doRemoveRITUrlCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strncmp(buf, "verbose ", strlen("verbose ")))
  {
    doRInThreadVerboseCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "stats"))
  {
    doRInThreadStatsCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if ((0 == strcmp(buf, "r100")) || (0 == strncmp(buf, "r100 ", strlen("r100 "))))
  {
    doR100Cmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "reqs"))
  {
    doReqsCmd(cl, buf);
    return CLI_PROMPT;
  } 
  if (0 == strcmp(buf, "status"))
  {
    doStatusCmd(cl, buf);
    return CLI_PROMPT;
  } 
  return CLI_NFOUND;
}

// newReq <method> <transId>: create a new request, with provided transId.
static void doNewReqCmd(t_cli *cl, char *buf)
{
  char method[32];
  char transId[32];

  buf = AdmAccept(buf); // newReq  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, method);
  if (strcmp(method, "GET")
     && strcmp(method, "PUT")
     && strcmp(method, "POST")
     && strcmp(method, "DELETE"))
  {
    AdmPrint(cl, "unsupported method (%s)\n", method);
    return;  
  }

  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, transId);
  if (!strcmp(transId, ""))
  {
    AdmPrint(cl, "transactionId is missing; aborted\n");
    return;
  }
  if (cl->cl_client->getTransaction(cl->cl_client, transId))
  {
    AdmPrint(cl, "transactionId already exists (%s); aborted\n", transId);
    return;
  }
  cl->cl_client->createReq(cl->cl_client, transId);

}

// setUrl <transId> <url>: set the URL.
static void doSetUrlCmd(t_cli *cl, char *buf)
{
  char transId[32];
  char url[255];
  transaction_t *trans;

  buf = AdmAccept(buf); // setUrl  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  if (! trans)
  {
    AdmPrint(cl, "provided transactionId does not exist (%s); aborted\n", transId);
    return;
  }
  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, url);
  if (!strcmp(url, ""))
  {
    AdmPrint(cl, "url is missing; aborted\n");
    return;
  }
  trans->setReqUrl(trans, url);
}

// addHeader <transId> <name> <value>: add a header.
static void doAddHeaderCmd(t_cli *cl, char *buf)
{
  char transId[32];
  char name[127];
  char value[255];
  transaction_t *trans;

  buf = AdmAccept(buf); // addHeader  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  if (! trans)
  {
    AdmPrint(cl, "provided transactionId does not exist (%s); aborted\n", transId);
    return;
  }
  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, name);
  if (!strcmp(name, ""))
  {
    AdmPrint(cl, "header name is missing; aborted\n");
    return;
  }

  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, value);
  if (!strcmp(value, ""))
  {
    AdmPrint(cl, "header value is missing; aborted\n");
    return;
  }
  trans->addReqHeader(trans, name, value);
}

// sendReq <transId>: send the request.
static void doSendReqCmd(t_cli *cl, char *buf)
{
  char transId[32];
  transaction_t *trans;

  buf = AdmAccept(buf); // sendReq  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  if (! trans)
  {
    AdmPrint(cl, "provided transactionId does not exist (%s); aborted\n", transId);
    return;
  }
  
  trans->sendReq(trans);
}

// release <transId>: send the request.
static void doReleaseCmd(t_cli *cl, char *buf)
{
  char transId[32];

  buf = AdmAccept(buf); // sendReq  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, transId);
  cl->cl_client->removeReq(cl->cl_client, transId);
}

static void doR1Cmd(t_cli *cl, char *buf)
{
  transaction_t *trans;
  char transId[32];
  const char c_szUrl[] = "http://www.example.com/";

  sprintf(transId, "req-%ld", time(NULL));
  cl->cl_client->createReq(cl->cl_client, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  trans->setReqUrl(trans, (char *)c_szUrl);
  trans->sendReq(trans);
  AdmPrint(cl, "Sent request to %s (transId:%s)\n", c_szUrl, transId);
}

static void doR2Cmd(t_cli *cl, char *buf)
{
  transaction_t *trans;
  char transId[32];
  const char c_szUrl[] = "http://rms.poc1.actility.com:8080/m2m";

  sprintf(transId, "req-%ld", time(NULL));
  cl->cl_client->createReq(cl->cl_client, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  trans->setReqUrl(trans, (char *)c_szUrl);
  trans->addReqHeader(trans, "Authorization", "Basic aHR0cCUzYS8vcm1zLnBvYzEuYWN0aWxpdHkuY29tJTNhODA4MC9tMm0vYXBwbGljYXRpb25zL1NVUFBPUlQ6");
  trans->sendReq(trans);
  AdmPrint(cl, "Sent request to %s (transId:%s)\n", c_szUrl, transId);
}

static void doR3Cmd(t_cli *cl, char *buf)
{
  transaction_t *trans;
  char transId[32];
  const char c_szUrl[] = "http://10.10.12.216:9090/";

  sprintf(transId, "req-%ld", time(NULL));
  cl->cl_client->createReq(cl->cl_client, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  trans->setReqUrl(trans, (char *)c_szUrl);
  trans->sendReq(trans);
  AdmPrint(cl, "Sent request to %s (transId:%s)\n", c_szUrl, transId);
}

static void doR4Cmd(t_cli *cl, char *buf)
{
  static unsigned long nbReqR4 = 0;
  int i = 0;

  if (g_urlRInThreadLen <= 0)
  {
    AdmPrint(cl, "No URL set; aborted\n");
    return;
  }
  i = nbReqR4 % ( g_urlRInThreadLen - 1 );

  localSendReq(cl, nbReqR4, g_szUrlRInThread[i]);
  nbReqR4++;
}


static void doR100Cmd(t_cli *cl, char *buf)
{
  transaction_t *trans;
  char transId[32];
  char url[255];
  const char c_szUrl[] = "http://www.example.com/";
  time_t now = time(NULL);
  int i;

  memset(url, 0, sizeof(url));

  buf = AdmAccept(buf); // r100  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, url);

  if (! url[0]) 
  {
    sprintf(url, c_szUrl);
  }

  for (i = 0; i < 100; i++)
  {
    sprintf(transId, "req-%ld-%02d", now, i);
    cl->cl_client->createReq(cl->cl_client, transId);
    trans = cl->cl_client->getTransaction(cl->cl_client, transId);
    trans->setReqUrl(trans, (char *)url);
    trans->sendReq(trans);
    AdmPrint(cl, "Sent request to %s (transId:%s)\n", url, transId);
  }
}

static void doReqsCmd(t_cli *cl, char *buf)
{
  transaction_t *trans;
  char transId[32];
  const char *c_szUrl[] = {
    "http://www.example.com/",
    "http://www.google.com/",
    "http://www.example.com/other/",
    "http://www.actility.com/",
    "http://www.example.com/docs/",
    NULL,
  };
  time_t now = time(NULL);
  int i = 0;

  while (c_szUrl[i] != NULL)
  {
    sprintf(transId, "req-%ld-%02d", now, i);
    cl->cl_client->createReq(cl->cl_client, transId);
    trans = cl->cl_client->getTransaction(cl->cl_client, transId);
    trans->setReqUrl(trans, (char *)c_szUrl[i]);
    trans->sendReq(trans);
    AdmPrint(cl, "Sent request to %s (transId:%s)\n", c_szUrl[i], transId);
    i++;
  }
}


static void doStatusCmd(t_cli *cl, char *buf)
{
  cl->cl_client->dumpInfo(cl->cl_client, (PF_DUMP_INFO_CB)AdmPrint, cl);
}


static void localSendReq(t_cli * cl, unsigned long nbReq, char *url)
{
  transaction_t *trans;
  char transId[32];

  sprintf(transId, "req-%lu", nbReq);
  cl->cl_client->createReq(cl->cl_client, transId);
  trans = cl->cl_client->getTransaction(cl->cl_client, transId);
  trans->setReqUrl(trans, url);
  trans->setReqMethod(trans, (char *)g_defaultMethod);
  trans->addReqHeader(trans, (char *)g_defaultHeaderName1, (char *)g_defaultHeaderValue1);
  if (g_defaultContent && (g_defaultContentLen > 0))
  {
    trans->setReqBody(trans, (unsigned char *)g_defaultContent, 
      g_defaultContentLen*sizeof(unsigned char));
  }

  if (trans->sendReq(trans))
  {
    pthread_mutex_lock(&cl->stats.mutex);
    cl->stats.sentReqOK ++;
    pthread_mutex_unlock(&cl->stats.mutex);
  }
  else
  {
    pthread_mutex_lock(&cl->stats.mutex);
    cl->stats.sentReqErr ++;
    pthread_mutex_unlock(&cl->stats.mutex);
  }
  if (cl->verbose)
  {
    AdmPrint(cl, "-> req to %s (transId:%s)\n", url, transId);
  }
}

static void *runContinuousLoad(void *data)
{
  t_cli * cl = (t_cli *)data;
  int i;
  unsigned long nbReq = 0;

  rtl_timemono(&cl->stats.startTimeMono);
  cl->tidRInThreadRunning = 1;
  while (cl->tidRInThreadRunning)
  {
    i = 0;
    
    for(i = 0; i < g_urlRInThreadLen; i++)
    {
      localSendReq(cl, nbReq, g_szUrlRInThread[i]);
      nbReq++;
    }

//    if (! (nbReq % 117))
//    {
//      localSendReq(cl, nbReq, (char *)c_szUrlTimeout);
//      i++;
//      nbReq++;
//    }    
//
//    if (! (nbReq % 29))
//    {
//      localSendReq(cl, nbReq, (char *)c_szUrlConnectionFailure);
//      i++;
//      nbReq++;
//    }    

    usleep(10000);
  }
 
  return NULL;
}

static void doRInThreadCmd(t_cli *cl, char *buf)
{
  if (g_urlRInThreadLen <= 0)
  {
    AdmPrint(cl, "No URL set; aborted\n");
    return;
  }
  pthread_create(&(cl->tidRInThread), NULL, /* default attributes please */ 
    runContinuousLoad, (void *)cl);
}

static void doRInThreadVerboseCmd(t_cli *cl, char *buf)
{
  char verbose[32];

  buf = AdmAccept(buf); // "verbose"
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, verbose);

  cl->verbose = atoi(verbose);
}

static void doRInThreadStatsCmd(t_cli *cl, char *buf)
{
  int i;
  time_t now;

  pthread_mutex_lock(&cl->stats.mutex);
  rtl_timemono(&now);
  AdmPrint(cl, "Running for %lu seconds\n", now - cl->stats.startTimeMono);
  AdmPrint(cl, "sentReqOK: %lu\n", cl->stats.sentReqOK);
  AdmPrint(cl, "sentReqErr: %lu\n", cl->stats.sentReqErr);
  AdmPrint(cl, "rcvRspOK: %lu\n", cl->stats.rcvRspOK);
  AdmPrint(cl, "rcvRspErr: %lu\n", cl->stats.sentReqErr);
  for (i = 0; i < ( sizeof(cl->stats.statusCodes) / sizeof(unsigned long) ); i++)
  {
    if (cl->stats.statusCodes[i])
    {
      AdmPrint(cl, "Status %d: %lu\n", i, cl->stats.statusCodes[i]);
    }
  }
  pthread_mutex_unlock(&cl->stats.mutex);
}

// showRIT: display RIT params
static void doShowRITCmd(t_cli *cl, char *buf)
{
  int i;
  AdmPrint(cl, "rInThread params:\n");
  AdmPrint(cl, "method: %s\n", g_defaultMethod);
  AdmPrint(cl, "URLs: [");
  for (i = 0; i < g_urlRInThreadLen; i++)
  {
    AdmPrint(cl, "%s ", g_szUrlRInThread[i]);
  }
  AdmPrint(cl, "]\n");
  AdmPrint(cl, "header name: %s\n", g_defaultHeaderName1);
  AdmPrint(cl, "header value: %s\n", g_defaultHeaderValue1);
  AdmPrint(cl, "content len: %d\n", g_defaultContentLen);
  AdmPrint(cl, "\n");
}

// setRITMethod <method>: change RIT method.
static void doSetRITMethodCmd(t_cli *cl, char *buf)
{
  char name[127];

  buf = AdmAccept(buf); // setRITMethod  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, name);
  if (!strcmp(name, ""))
  {
    AdmPrint(cl, "method name is missing; aborted\n");
    return;
  }

  if (strcmp(name, "GET") && strcmp(name, "PUT") && strcmp(name, "POST") && strcmp(name, "DELETE"))
  {
    AdmPrint(cl, "method not allowed; aborted (only GET, PUT, POST or DELETE are allowed)\n");
    return;
  }

  if (g_defaultMethod) free(g_defaultMethod);
  g_defaultMethod = strdup(name);
}

// setRITHeader <name> <value>: change RIT header.
static void doSetRITHeaderCmd(t_cli *cl, char *buf)
{
  char name[127];
  char value[255];

  buf = AdmAccept(buf); // setRITHeader  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, name);
  if (!strcmp(name, ""))
  {
    AdmPrint(cl, "header name is missing; aborted\n");
    return;
  }

  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, value);
  if (!strcmp(value, ""))
  {
    AdmPrint(cl, "header value is missing; aborted\n");
    return;
  }

  if (g_defaultHeaderName1) free(g_defaultHeaderName1);
  g_defaultHeaderName1 = strdup(name);

  if (g_defaultHeaderValue1) free(g_defaultHeaderValue1);
  g_defaultHeaderValue1 = strdup(value);
}

// setRITContent <file>: change RIT content.
static void doSetRITContentCmd(t_cli *cl, char *buf)
{
  unsigned char *tmpContent = NULL;
  int tmpContentLen = 0;
  char file[255];
  FILE * fd;
  struct stat fileStat;

  buf = AdmAccept(buf); // setRITContent  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, file);
  if (!strcmp(file, ""))
  {
    AdmPrint(cl, "file name is missing; aborted\n");
    return;
  }

  if (NULL == (fd = fopen(file, "r")))
  {
    AdmPrint(cl, "unable to open %s; aborted (errno:%d) (err:%s)\n", file, errno, strerror(errno));
    return;
  }

  if (-1 == (stat(file, &fileStat)))
  {
    AdmPrint(cl, "unable to stat %s; aborted (errno:%d) (err:%s)\n", file, errno, strerror(errno));
    return;
  }

  if (0 == fileStat.st_size)
  {
    AdmPrint(cl, "WARNING: file %s is empty !\n", file);
    tmpContent = NULL;
    tmpContentLen = 0;
  }
  else
  {
    tmpContent = malloc(fileStat.st_size);
    tmpContentLen = fread(tmpContent, sizeof(unsigned char), fileStat.st_size, fd);
    if (tmpContentLen != fileStat.st_size)
    {
      AdmPrint(cl, "failed to read file content: aborted (expected size:%d) (read:%d) "
        "(errno:%d) (err:%s)\n", fileStat.st_size, tmpContentLen, errno, strerror(errno));
      return;
    }
  }

  if (g_defaultContent) free(g_defaultContent);
  g_defaultContent = tmpContent;
  g_defaultContentLen = tmpContentLen;
}

// addRITUrl <url>
static void doAddRITUrlCmd(t_cli *cl, char *buf)
{
  char **tmpUrl = NULL;
  char url[255];
  int i;

  buf = AdmAccept(buf); // addRITUrl  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, url);
  if (!strcmp(url, ""))
  {
    AdmPrint(cl, "url is missing; aborted\n");
    return;
  }
  
  tmpUrl = malloc((g_urlRInThreadLen + 1) * sizeof(char *));
  for (i = 0; i < g_urlRInThreadLen; i++)
  {
    tmpUrl[i] = g_szUrlRInThread[i];
  }
  tmpUrl[g_urlRInThreadLen] = strdup(url);

  if (g_szUrlRInThread);
  {
    free(g_szUrlRInThread);
  }
  g_urlRInThreadLen++;
  g_szUrlRInThread = tmpUrl;
}

// removeRITUrl <index>
static void doRemoveRITUrlCmd(t_cli *cl, char *buf)
{
  char **tmpUrl = NULL;
  char indexStr[127];
  int index;
  int i;
  int j;

  buf = AdmAccept(buf); // removeRITUrl  
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, indexStr);
  if (!strcmp(indexStr, ""))
  {
    AdmPrint(cl, "index is missing; aborted\n");
    return;
  }
  index = atoi(indexStr);
  if ((index < 0) || (index >= g_urlRInThreadLen))
  {
    AdmPrint(cl, "index is out of bounds; aborted\n");
    return;
  }
  
  tmpUrl = malloc((g_urlRInThreadLen - 1) * sizeof(char *));
  j = 0;
  for (i = 0; i < g_urlRInThreadLen; i++)
  {
    if (i != index)
    {
      tmpUrl[j] = g_szUrlRInThread[i];
      j++;
    }
    else
    {
      free(g_szUrlRInThread[i]);
    }
  }

  free(g_szUrlRInThread);
  g_urlRInThreadLen--;
  g_szUrlRInThread = tmpUrl;
}

/*****************************************************************************/
/* CLI stuff */

static char *GetTelnetKey()
{
  return "";
}

static char *GetTelnetPort()
{
  return "2002";
}

static  void  AdmKeepAlive(int fd)
{
  if  ( TcpKeepAlive > 0 )
  {
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
        (char *)&TcpKeepAlive, sizeof(TcpKeepAlive));
#if 0
    if  ( TcpKeepIdle > 0 )
    {
      setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,
          (char *)&TcpKeepIdle, sizeof(TcpKeepIdle));
    }
    if  ( TcpKeepIntvl > 0 )
    {
      setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL,
          (char *)&TcpKeepIntvl, sizeof(TcpKeepIntvl));
    }
    if  ( TcpKeepCnt > 0 )
    {
      setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,
          (char *)&TcpKeepCnt, sizeof(TcpKeepCnt));
    }
#endif
  }
}

static  void  AdmAddBuf(t_cli *cl, char *buf, int len)
{
  int fd;
  t_lout  *out;

  fd  = cl->cl_fd;

  RTL_TRDBG(9, "Adm add buffer lg=%d\n", len);
  if  (!buf || len <= 0)
  {
    return;
  }

  // buf deja decouper buf en morceaux plus petits que TCP_FRAME_SZ
  out = (t_lout *)malloc(sizeof(t_lout));
  if  ( !out )
  {
    return;
  }

  out->o_len  = len;
  out->o_buf  = (char *)malloc(len);
  if  ( !out->o_buf )
  {
    return;
  }
  memcpy  (out->o_buf, buf, len);

  list_add_tail( &(out->list), &(cl->cl_lout.list) );
}


int AdmTcpMsgCount(t_cli *cl)
{
  int nb  = 0;
  t_lout *msg;

  list_for_each_entry(msg, &cl->cl_lout.list, list)
  {
    nb++;
  }
  return  nb;
}

static  void  AdmAsyncWrite(t_cli *cl)
{
  int fd;
  t_lout  *lst;
  t_lout  *msg;
  int msgsz;
  int nb;
  int sz;
  char  out[TCP_FRAME_SZ];

  fd  = cl->cl_fd;
  lst = &cl->cl_lout;

  if  ( list_empty(&lst->list) )
  {
    return;
  }

  nb  = 0;
  sz  = 0;
  while ( !list_empty(&lst->list) )
  {
    msg = list_entry(lst->list.next, t_lout, list);
    msgsz = msg->o_len;
    if  ( sz + msgsz > sizeof(out) )
    {
      RTL_TRDBG(2, "defragmentation sz=%d no more than %d\n",
          sz + msgsz, sizeof(out));
      break;
    }
    list_del(&msg->list);
    memcpy  (out + sz, msg->o_buf, msgsz);
    sz  = sz + msgsz;
    nb++;
    free(msg->o_buf);
    free(msg);
  }

  if ( nb <= 0 )
  {
    RTL_TRDBG(0, "bad defragmentation nb=%d sz=%d\n", nb, sz);
    return;
  }

  if ( nb > 0 && sz > 0 )
  {
    rtl_timemono(&cl->cl_lasttcpwrite);
    RTL_TRDBG(9, "write adm/tcp nb=%d sz=%d\n", nb, sz);
    if (write(fd, out, sz) != sz)
    {
      RTL_TRDBG(0, "AdmAsyncWrite() write error\n");
    }
  }
}


char  *AdmAdvance(char *pt)
{
  while ( *pt && ( *pt == ' ' || *pt == '\t' ) )
  {
    pt++;
  }
  return  pt;
}

char  *AdmAccept(char *pt)
{
  while ( *pt && ( *pt != ' ' && *pt != '\t' ) )
  {
    pt++;
  }
  return  pt;
}

char  *AdmAcceptKeep(char *pt, char *dst)
{
  *dst  = '\0';
  while ( *pt && ( *pt != ' ' && *pt != '\t' ) )
  {
    *dst++  = *pt++;
  }
  *dst  = '\0';
  return  pt;
}


void  AdmPrintSync(int fd, char *fmt, ...)
{
  va_list listArg;
  char  tmp[5 * 1024] ;
  int lg ;

  va_start(listArg, fmt);
  vsprintf(tmp, fmt, listArg);
  va_end(listArg);

  lg  = strlen(tmp);
  if  ( tmp[lg - 1] == '\n' )
  {
    tmp[lg - 1] = '\r';
    tmp[lg]   = '\n';
    lg++;
  }

  if (write(fd, tmp, lg) != lg)
  {
    RTL_TRDBG(0, "AdmPrintSync() write error\n");
  }
}

void  AdmPrint(t_cli *cl, char *fmt, ...)
{
  va_list listArg;
  char  tmp[5 * 1024] ;
  int lg ;
  int out;
  int dep;

  if  ( (lg = strlen(fmt)) < 512 )
  {
    va_start(listArg, fmt);
    vsprintf(tmp, fmt, listArg);
    va_end(listArg);

    lg  = strlen(tmp);
    if  ( tmp[lg - 1] == '\n' )
    {
      tmp[lg - 1] = '\r';
      tmp[lg]   = '\n';
      lg++;
    }
  }
  else
  {
    // if fmt is more than 512 we assume there is no a format
    if  ( lg > sizeof(tmp) )
    {
      lg  = sizeof(tmp);
    }
    memcpy  (tmp, fmt, lg);
  }


  dep = 0;
  do
  {
    if  ( lg > TCP_FRAME_SZ / 2 )
    {
      out = TCP_FRAME_SZ / 2;
    }
    else
    {
      out = lg;
    }
    AdmAddBuf(cl, tmp + dep, out);
    dep = dep + out;
    lg  = lg - out;
  }
  while ( lg > 0 );

}


void  AdmPrintTable(t_cli *cl, int nb, int tab[], char *fmt, char pstart, char pend, char psep)
{
  int n;

  for (n = 0 ; n < nb ; n++)
  {
    if  (n == 0)
    {
      AdmPrint(cl, "%c", pstart);
    }
    AdmPrint(cl, fmt, tab[n]);
    if  (n == nb - 1)
    {
      AdmPrint(cl, "%c", pend);
    }
    else
    {
      AdmPrint(cl, "%c", psep);
    }
  }

}



void  AdmPrintDefTable(t_cli *cl, int nb, int tab[], char *fmt)
{
  return  AdmPrintTable(cl, nb, tab, fmt, '[', ']', ' ');
}

void  AdmPrintTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt, char pstart, char pend, char psep)
{
  int n;

  for (n = 0 ; n < nb ; n++)
  {
    if  (n == 0)
    {
      AdmPrint(cl, "%c", pstart);
    }
    AdmPrint(cl, fmt, tab[n]);
    if  (n == nb - 1)
    {
      AdmPrint(cl, "%c", pend);
    }
    else
    {
      AdmPrint(cl, "%c", psep);
    }
  }

}

void  AdmPrintDefTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt)
{
  return  AdmPrintTableUS(cl, nb, tab, fmt, '[', ']', ' ');
}


static  t_cli *CliNew(int fd)
{
  t_cli *cl;
  int i;

  cl  = (t_cli *)malloc(sizeof(t_cli));
  if  (!cl)
  {
    RTL_TRDBG(0, "malloc failure\n");
    return  NULL;
  }
  memset  (cl, 0, sizeof(t_cli));

  if (g_defaultMethod) free(g_defaultMethod);
  g_defaultMethod = strdup(c_defaultMethod);

  if (g_defaultHeaderName1) free(g_defaultHeaderName1);
  g_defaultHeaderName1 = strdup(c_defaultHeaderName1);

  if (g_defaultHeaderValue1) free(g_defaultHeaderValue1);
  g_defaultHeaderValue1 = strdup(c_defaultHeaderValue1);

  if (g_defaultContent) free(g_defaultContent);
  g_defaultContentLen = sizeof(c_defaultContent);
  g_defaultContent = malloc(g_defaultContentLen * sizeof(unsigned char));
  memcpy(g_defaultContent, c_defaultContent, g_defaultContentLen);

  if (g_urlRInThreadLen > 0)
  {
    for (i = 0; i < g_urlRInThreadLen; i++)
    {
      free(g_szUrlRInThread[i]);
    }
    free(g_szUrlRInThread);
  }
  g_urlRInThreadLen = 1;
  g_szUrlRInThread = malloc(g_urlRInThreadLen * sizeof(char *));
  g_szUrlRInThread[0] = strdup(c_szUrlRInThread);

  cl->cl_fd = fd;
  rtl_timemono(&cl->cl_lasttcpread);
  INIT_LIST_HEAD(&cl->cl_lout.list);

  cl->cl_client = createHttpClient(cl);
  cl->tidRInThreadRunning = 0;
  pthread_mutex_init(&cl->stats.mutex, NULL);

  RTL_TRDBG(1, "new cli %x %d\n", cl, cl->cl_fd);

  return  cl;
}


static  void  CliFree(t_cli *cl)
{
  int nb;

  if  (!cl)
  {
    return;
  }
  RTL_TRDBG(1, "free cli %x fd=%d\n", cl, cl->cl_fd);

  releaseHttpClient(cl);

  if (cl->cl_fd > 0 && cl->cl_fd < FD_MAX)
  {
    TbCli[cl->cl_fd] = NULL;
    close(cl->cl_fd);
    rtl_pollRmv(MainTbPoll, cl->cl_fd) ;
  }

  // clear & free outgoing message list
  nb  = 0;
  while ( !list_empty(&cl->cl_lout.list) )
  {
    t_lout *msg;

    msg = list_entry(cl->cl_lout.list.next, t_lout, list);
    list_del(&msg->list);
    free(msg->o_buf);
    free(msg);
    nb++;
  }
  cl->cl_client->free(cl->cl_client);
  if (cl->tidRInThreadRunning)
  {
    cl->tidRInThreadRunning = 0;
    pthread_join(cl->tidRInThread, NULL);
  }
  free(cl);

  RTL_TRDBG(1, "adm connection is now cleared (wmsg=%d)\n", nb);
}

static  int AdmPassword(t_cli *cl, char *buf)
{
  if  ( strcmp(GetTelnetKey(), buf) )
  {
    cl->cl_nbkey++;
    if  ( cl->cl_nbkey >= 3 )
    {
      return  CLI_DISC;
    }
    return  CLI_KEY;
  }
  cl->cl_okkey  = 1;
  cl->cl_nbkey  = 0;
  return  CLI_PROMPT;
}

static  void  AdmLoopCmd(t_cli *cl, char *buf, int lg)
{
  char  *ptn;
  char  *ptr;
  char  *pt;
  int ret;
  int stop;


  strcat  (cl->cl_cmd, buf);
  cl->cl_pos  += lg;

  ptr = strrchr(cl->cl_cmd, '\r');
  ptn = strrchr(cl->cl_cmd, '\n');
  if  ( !ptr && !ptn )
  {
    return;
  }

  if  ( ptr )
  {
    *ptr  = '\0';
  }
  if  ( ptn )
  {
    *ptn  = '\0';
  }

  RTL_TRDBG(5, "adm get command '%s'\n", cl->cl_cmd);
  stop  = 0;
  pt  = cl->cl_cmd;
  while (!stop)
  {
    while ( *pt == '\n' || *pt == '\r' || *pt == '\t' )
    {
      pt++;
    }
    lg  = 0;
    while ( *pt )
    {
      if  ( *pt == '\n' || *pt == '\r' || *pt == '\t' )
      {
        pt++;
        break;
      }
      buf[lg] = *pt;
      pt++;
      lg++;
    }
    buf[lg] = '\0';
    if (strlen(buf))
    {
      RTL_TRDBG(1, "adm treat '%s'\n", buf);
    }

    if  ( !cl->cl_okkey && strlen(GetTelnetKey()) )
    {
      ret = AdmPassword(cl, buf);
    }
    else
    {
      ret = AdmCommand(cl, buf);
    }
    switch  (ret)
    {
      case  CLI_NFOUND  :
        AdmPrint(cl, "command not found\r\n");
        AdmPrint(cl, ">>");
        break;
      case  CLI_PROMPT  :
        AdmPrint(cl, ">>");
        break;
      case  CLI_DISC  :
        CliFree(cl);
        stop  = 1;
        cl  = NULL;
        break;
      case  CLI_KEY :
        AdmPrint(cl, "key:");
        break;
      case  CLI_EMPTY :
        AdmPrint(cl, ">>");
        break;
      case  CLI_MORE :
        AdmPrint(cl, "++");
        break;
    }
    if  ( !*pt )
    {
      break;
    }
  }
  if  (cl)
  {
    cl->cl_pos  = 0;
    memset  (cl->cl_cmd, 0, sizeof(cl->cl_cmd));
  }
}

int CB_AdmTcpRequest(void *tb, int fd, void *r1, void *r2, int revents)
{
  t_cli *cl;
  int empty;

  if  (fd >= FD_MAX || (cl = TbCli[fd]) == NULL)
  {
    RTL_TRDBG(0, "adm no cli for fd=%d\n", fd);
    rtl_pollRmv(MainTbPoll, fd);
    return  -1;
  }

  empty = list_empty(&cl->cl_lout.list);
  RTL_TRDBG(TRACE_DEBUG, "callback request adm/tcp fd=%d empty=%d e=%d\n",
      fd, empty, revents);

  // if we have something to write ask for POLLOUT event and POLLIN
  // or just POLLOUT if we want to slow down remote writer
  if  ( !empty )
  {
    return  POLLIN | POLLOUT;
  }

  return  POLLIN;
}

int CB_AdmTcpEvent(void *tb, int fd, void *r1, void *r2, int revents)
{
  t_cli *cl;
  int lg;
  char  buf[500];

  RTL_TRDBG(9, "callback event adm/tcp fd=%d e=%d r1=%lx r2=%lx\n",
      fd, revents, r1, r2);

  if  (fd >= FD_MAX || (cl = TbCli[fd]) == NULL)
  {
    RTL_TRDBG(0, "adm no cli for fd=%d\n", fd);
    rtl_pollRmv(MainTbPoll, fd);
    return  -1;
  }

  if ((revents & POLLOUT) == POLLOUT)
  {
    RTL_TRDBG(9, "event write adm/tcp fd=%d\n", fd);
    AdmAsyncWrite(cl);
  }

  if ((revents & POLLIN) != POLLIN)
  {
    return  0;
  }
  lg  = read(fd, buf, sizeof(buf));
  if  ( lg <= 0 )
  {
    RTL_TRDBG(1, "Adm connection ended by remote\n");
    CliFree(cl);
    return  0;
  }
  buf[lg] = '\0';
  RTL_TRDBG(5, "event read adm/tcp fd=%d lg=%d\n", fd, lg);
  rtl_timemono(&cl->cl_lasttcpread);



  if  ( cl->cl_pos + lg >= sizeof(cl->cl_cmd) )
  {
    RTL_TRDBG(1, "command too long => reset\n");
    CliFree(cl);
    return  0;
  }
  AdmLoopCmd(cl, buf, lg);


  return  0;
}

static  void  SetCB(int fdnew)
{
  rtl_pollAdd(MainTbPoll, fdnew, CB_AdmTcpEvent, CB_AdmTcpRequest, NULL, NULL);
}

int CB_AdmTcpListen(void *tb, int fd, void *r1, void *r2, int revents)
{
  t_cli *cl;
  int fdnew;
  struct sockaddr_in6 from;
  socklen_t lenaddr;

  RTL_TRDBG(1, "callback event adm/tcp fd=%d FD=%d e=%d r1=%lx r2=%lx\n",
      fd, FdListen, revents, r1, r2);

  if (fd != FdListen)
  {
    RTL_TRDBG(0, "fd=%d != myfd=%d\n", fd, FdListen);
    rtl_pollRmv(MainTbPoll, fd);
    return  -1;
  }

  if ((revents & POLLIN) != POLLIN)
  {
    return  0;
  }

  lenaddr = sizeof(from) ;
  fdnew = accept(FdListen, (struct sockaddr *)&from, &lenaddr);
  if  ( fdnew < 0 )
  {
    return  0;
  }

  if  (fdnew > FD_MAX)
  {
    AdmPrintSync(fdnew, "Adm does not support fd > %d\r\n", FD_MAX);
    close(fdnew);
    RTL_TRDBG(0, "Adm does not support fd > %d\r\n", FD_MAX);
    return  0;
  }
  if  (TbCli[fdnew] != NULL)
  {
    close(fdnew);
    RTL_TRDBG(0, "Amd fd=%d not free !!!\r\n", fdnew);
    return  0;
  }

  cl  = CliNew(fdnew);
  if  (!cl)
  {
    close(fdnew);
    return  0;
  }
  TbCli[fdnew]  = cl;

  AdmKeepAlive(fdnew);

  SetCB(fdnew);

  AdmPrint(cl, "Welcome to song.binding.http.jni (native part) tester CLI\n");
  if  (strlen(GetTelnetKey()))
  {
    AdmPrint(cl, "key:");
  }
  else
  {
    AdmPrint(cl, ">>");
  }

  return  0;
}

static  void  AdmTcpBind()
{
  int flag;

  struct addrinfo hints;
  struct addrinfo *result, *rp;


  char  *strport  = GetTelnetPort();

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */

  if (getaddrinfo(NULL, strport, &hints, &result))
  {
    RTL_TRDBG(0, "adm/tcp cannot getaddrinfo port=%s\n",
        strport);
    return;
  }
  for (rp = result; rp != NULL; rp = rp->ai_next)
  {
    FdListen = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
    if  (FdListen < 0)
    {
      continue;
    }
    flag  = 1;
    if  ( setsockopt(FdListen, SOL_SOCKET, SO_REUSEADDR,
        (char *)&flag, sizeof(flag)) < 0)
    {
      close (FdListen) ;
      continue;
    }
    if  (bind(FdListen, rp->ai_addr, rp->ai_addrlen) == 0)
    {
      RTL_TRDBG(0, "ok bind adm/tcp\n");
      break;
    }
  }
  if (rp == NULL)
  {
    FdListen  = -1;
    RTL_TRDBG(0, "cannot bind adm/tcp '%s'+%s\n", "*", strport);
    return;
  }

  if (listen(FdListen, 2) < 0)
  {
    close(FdListen);
    FdListen  = -1;
    RTL_TRDBG(0, "cannot listen adm/tcp '%s'+%s\n", "*", strport);
    return;
  }

  rtl_pollAdd(MainTbPoll, FdListen, CB_AdmTcpListen, NULL, NULL, NULL);
}

void  AdmTcpInit()
{
  AdmTcpBind();
}

void  AdmClockSc(time_t now)
{
  t_cli *wh;
  int i = 0;
  time_t  diff;
  time_t  delta;
  time_t  timer = 600;

  delta = timer - (timer / 10);
  for (i = 0 ; i < FD_MAX ; i++)
  {
    wh  = TbCli[i];
    if  (!wh)
    {
      continue;
    }

    // remote client does not read its messages
    diff  = ABS(now - wh->cl_lasttcpwrite);
    if  (diff >= 60 && AdmTcpMsgCount(wh) > 0)
    {
      RTL_TRDBG(1, "waiting msg timer for [%03d]\n", i);
      CliFree(wh);
      continue;
    }

    // remote client does not send messages
    diff  = ABS(now - wh->cl_lasttcpread);
    if  (diff >= timer)
    {
      RTL_TRDBG(1, "Inactivity timer for [%03d]\n", i);
      CliFree(wh);
      continue;
    }
    if  ( diff >= delta )
    {
      AdmPrint(wh,
          "you'll be disconnected in %d sec!!!\n",
          timer - diff);
    }
  }
}









