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
#include "header_t.h"                                                                               
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"
#include "http_clients.h"

static int g_nbLoops = 1000;

extern int g_traceLevel;

unsigned char notifyContent[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<m2m:notify xmlns:m2m=\"http://uri.etsi.org/m2m\" xmlns:xmime=\"http://www.w3.org/2005/05/xmlmime\">"
  "<m2m:statusCode>STATUS_OK</m2m:statusCode><representation xmime:contentType=\"application/xml; charset=utf-8\">PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4KPG0ybTphcHBsaWNhdGlv\r\n"
  "bnMgeG1sbnM6bTJtPSJodHRwOi8vdXJpLmV0c2kub3JnL20ybSI+PG0ybTphY2Nlc3NSaWdo\r\n"
  "dElEPi9tMm0vYWNjZXNzUmlnaHRzL0xvY2FkbWluX0FSLzwvbTJtOmFjY2Vzc1JpZ2h0SUQ+\r\n"
  "PG0ybTpjcmVhdGlvblRpbWU+MjAxNC0wNS0xNFQxNjozNTozNS4yNDgrMDI6MDA8L20ybTpj\r\n"
  "cmVhdGlvblRpbWU+PG0ybTpsYXN0TW9kaWZpZWRUaW1lPjIwMTQtMDUtMTRUMTY6MzU6NDIu\r\n"
  "OTk4KzAyOjAwPC9tMm06bGFzdE1vZGlmaWVkVGltZT48bTJtOmFwcGxpY2F0aW9uQ29sbGVj\r\n"
  "dGlvbj48bTJtOm5hbWVkUmVmZXJlbmNlIGlkPSJJRUNfNjExMzFfVk0iPklFQ182MTEzMV9W\r\n"
  "TS88L20ybTpuYW1lZFJlZmVyZW5jZT48bTJtOm5hbWVkUmVmZXJlbmNlIGlkPSJJUFVfS05Y\r\n"
  "Ij5JUFVfS05YLzwvbTJtOm5hbWVkUmVmZXJlbmNlPjxtMm06bmFtZWRSZWZlcmVuY2UgaWQ9\r\n"
  "IlNZU1RFTSI+U1lTVEVNLzwvbTJtOm5hbWVkUmVmZXJlbmNlPjwvbTJtOmFwcGxpY2F0aW9u\r\n"
  "Q29sbGVjdGlvbj48bTJtOmFwcGxpY2F0aW9uQW5uY0NvbGxlY3Rpb24vPjxtMm06c3Vic2Ny\r\n"
  "aXB0aW9uc1JlZmVyZW5jZT4vbTJtL2FwcGxpY2F0aW9ucy9zdWJzY3JpcHRpb25zLzwvbTJt\r\n"
  "OnN1YnNjcmlwdGlvbnNSZWZlcmVuY2U+PG0ybTptZ210T2Jqc1JlZmVyZW5jZT4vbTJtL2Fw\r\n"
  "cGxpY2F0aW9ucy9tZ210T2Jqcy88L20ybTptZ210T2Jqc1JlZmVyZW5jZT48L20ybTphcHBs\r\n"
  "aWNhdGlvbnM+Cg==</representation><subscriptionReference>http://testerman-ong1.actility.com:8080/m2m/applications/subscriptions/SCL_SUBSCRIBE_APPLICATIONS_003</subscriptionReference></m2m:notify>\n";
 
unsigned char subscribCreateContent[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
  "<m2m:subscription xmlns:m2m=\"http://uri.etsi.org/m2m\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" m2m:id=\"SCL_SUBSCRIBE_APPLICATIONS_003\"><m2m:expirationTime>9999-12-31T09:59:59.999+01:00</m2m:expirationTime><m2m:creationTime>2014-05-16T14:05:43.475+02:00</m2m:creationTime><m2m:lastModifiedTime>2014-05-16T14:05:43.475+02:00</m2m:lastModifiedTime><m2m:subscriptionType>ASYNCHRONOUS</m2m:subscriptionType><m2m:contact>http://10.10.12.216:3568</m2m:contact></m2m:subscription>";

typedef struct userContext_t
{
  httpClient_t *client;  
} userContext_t;

static void httpResponseCb(void *issuerHandle, transaction_t *transaction)
{
  char *urlInfo = NULL;
  long response;
  header_t *h;
  int i = 0;

  userContext_t *userCtxt = (userContext_t *)issuerHandle;

  curl_easy_getinfo(transaction->curlEasyHandle, CURLINFO_RESPONSE_CODE, &response);
  curl_easy_getinfo(transaction->curlEasyHandle, CURLINFO_EFFECTIVE_URL, &urlInfo);
  LOG(TRACE_DEBUG, "httpResponseCb (transId:%s) (content:%0.8p) (len:%ld) "
    "(url:%s)\n", transaction->transId, transaction->response->content,
    transaction->response->contentLength, urlInfo);

  printf("===============================================\n");
  printf("===> Response received for %s (url:%s):\n", transaction->transId, 
    urlInfo);
  printf("> %s %d %s\n", transaction->response->protocol, 
    transaction->response->statusCode, transaction->response->reasonPhrase);
  while ((h = transaction->response->getNthHeader(transaction->response, i)))
  {
    printf("> %s: %s\n", h->name, h->values);
    i++;
  }
  printf("\n");
  if (transaction->response->content)
  { 
    printf((char *)transaction->response->content);
  printf("\n");
  }
  printf("===============================================\n");
 
  userCtxt->client->removeReq(userCtxt->client, transaction->transId); 
}

static void firstClientTask(void *param1)
{
  int i;
  char transId[255];
  userContext_t *uc = (userContext_t *)param1;
  httpClient_t *client = uc->client; 

  for (i = 0; i < g_nbLoops; i++)
  { 
    char transIdPrefix[] = "firtClientTask";
    sprintf(transId, "%s-%d", transIdPrefix, i);
    client->createReq(client, transId);
    transaction_t *trans = client->getTransaction(client, transId);
    trans->setJTransaction(trans, NULL);
    trans->setReqMethod(trans, "POST");
    trans->setReqUrl(trans, "http://10.10.12.216:9090");
    trans->addReqHeader(trans, "Authorization", "Basic aHR0cCUzQS8vdGVzdGVybWFuLW9uZzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm06");
    trans->addReqHeader(trans, "Content-Length", "1475");
    trans->addReqHeader(trans, "User-Agent", "ONG/3.2.7");
    trans->addReqHeader(trans, "X-Acy-Trans-Id", transId);
    trans->addReqHeader(trans, "Content-Type", "application/xml; charset=utf-8");
    trans->setReqBody(trans, notifyContent, 1475);
    trans->sendReq(trans);

    printf("<<< %s SENT\n", transId);
    usleep(10000);
  }
  printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% end of first task\n");
}

static void secondClientTask(void *param1)
{
  int i;
  char transId[255];
  userContext_t *uc = (userContext_t *)param1;
  httpClient_t *client = uc->client; 

  for (i = 0; i < g_nbLoops; i++)
  { 
    char transIdPrefix[] = "secondClientTask";
    sprintf(transId, "%s-%d", transIdPrefix, i);
    client->createReq(client, transId);
    transaction_t *trans = client->getTransaction(client, transId);
    trans->setJTransaction(trans, NULL);
    trans->setReqMethod(trans, "POST");
    trans->setReqUrl(trans, "http://10.10.12.216:9090");
    trans->addReqHeader(trans, "Authorization", "Basic aHR0cCUzQS8vdGVzdGVybWFuLW9uZzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm06");
    trans->addReqHeader(trans, "Content-Length", "1475");
    trans->addReqHeader(trans, "User-Agent", "ONG/3.2.7");
    trans->addReqHeader(trans, "X-Acy-Trans-Id", transId);
    trans->addReqHeader(trans, "Content-Type", "application/xml; charset=utf-8");
    trans->setReqBody(trans, notifyContent, 1475);
    trans->sendReq(trans);
  
    printf("<<< %s SENT\n", transId);

    usleep(10000);
  }

  printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% end of second task\n");
}

static void thirdClientTask(void *param1)
{
  int i;
  char transId[255];
  userContext_t *uc = (userContext_t *)param1;
  httpClient_t *client = uc->client; 

  for (i = 0; i < g_nbLoops; i++)
  { 
    char transIdPrefix[] = "thirdClientTask";
    sprintf(transId, "%s-%d", transIdPrefix, i);
    client->createReq(client, transId);
    transaction_t *trans = client->getTransaction(client, transId);
    trans->setJTransaction(trans, NULL);
    trans->setReqMethod(trans, "POST");
    trans->setReqUrl(trans, "http://10.10.12.216:9090");
    trans->addReqHeader(trans, "Authorization", "Basic aHR0cCUzQS8vdGVzdGVybWFuLW9uZzEuYWN0aWxpdHkuY29tJTNBODA4MC9tMm06");
    trans->addReqHeader(trans, "Content-Length", "1475");
    trans->addReqHeader(trans, "User-Agent", "ONG/3.2.7");
    trans->addReqHeader(trans, "X-Acy-Trans-Id", transId);
    trans->addReqHeader(trans, "Content-Type", "application/xml; charset=utf-8");
    trans->setReqBody(trans, notifyContent, 1475);
    trans->sendReq(trans);
  
    printf("<<< %s SENT\n", transId);

    usleep(10000);
  }

  printf("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% end of second task\n");
}

int main(int argc, char *argv[])
{
  static pthread_t firstClientThread;
  static pthread_t secondClientThread;
  static pthread_t thirdClientThread;
  g_traceLevel = 5;
  
  g_nbLoops = 10000;

  httpClientsInit(5);

  userContext_t uc1;
  httpClient_t *client1 = createHttpClient(&uc1, httpResponseCb);
  uc1.client = client1;

  userContext_t uc2;
  httpClient_t *client2 = createHttpClient(&uc2, httpResponseCb);
  uc2.client = client2;

  pthread_create (&firstClientThread, NULL, (void *) &firstClientTask, &uc1);
  pthread_create (&secondClientThread, NULL, (void *) &secondClientTask, &uc1);
  pthread_create (&thirdClientThread, NULL, (void *) &thirdClientTask, &uc2);

  pthread_join(firstClientThread, NULL);
  pthread_join(secondClientThread, NULL);
  pthread_join(thirdClientThread, NULL);


  sleep(10);

  httpClientsUninit();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");


  return 0;
}






