#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <curl/curl.h>
#include <poll.h>
#include <pthread.h>
#include <jni.h>

#include "rtlbase.h"

#include "common.h"
#include "header_t.h"                                                                               
#include "response_t.h"
#include "transaction_t.h"
#include "transactionsList_t.h"
#include "freeTransactionsList_t.h"
#include "httpClient_t.h"
#include "http_clients.h"



static int g_nbClients = 0;
static int g_nbMaxClients = 0;
static httpClient_t ** g_clients = NULL;
static pthread_t pollingThread;

static void httpClientsPollingThread(void *param1);

void httpClientsInit(int nbMaxClients)
{
  curl_global_init(CURL_GLOBAL_ALL);
  g_nbMaxClients = nbMaxClients;
  g_clients = malloc(g_nbMaxClients * sizeof(httpClient_t *));
  memset(g_clients, 0, g_nbMaxClients * sizeof(httpClient_t *));

  pthread_create (&pollingThread, NULL, (void *) &httpClientsPollingThread, NULL);
  pthread_detach(pollingThread);
}

void httpClientsUninit()
{
  int i;
  for(i = 0; i < g_nbMaxClients; i++)
  {
    if (g_clients[i])
    {
      g_clients[i]->free(g_clients[i]); 
    }
  }
  free(g_clients);
}

httpClient_t * createHttpClient(void *userContext, PF_HTTP_RESPONSE_CB httpResponseCb)
{
  httpClient_t *res = NULL;
  int i;
  if (g_nbClients < g_nbMaxClients)
  {
    res = new_httpClient_t(1, httpResponseCb, userContext);
    res->setMaxSocketsPerHost(res, 10);
    res->setMaxTotalSockets(res, 50);
    for(i = 0; i < g_nbMaxClients; i++)
    {
      if (NULL == g_clients[i]) break;
    }
    g_clients[i] = res;
    g_nbClients ++;
  }
  return res;
}

void releaseHttpClient(void *userContext)
{
  int i;
  for(i = 0; i < g_nbMaxClients; i++)
  {
    if (userContext == g_clients[i]->issuerHandle)
    {
      g_clients[i] = NULL;  
      g_nbClients --;
      break;
    }
  }
}

static void httpClientsPollingThread(void *param1)
{
  (void)param1;
  int i;
  httpClient_t *client;
  int nbClients;

  while(1)
  {
    i = 0;
    nbClients = g_nbClients;

    do
    {
      client = g_clients[i];
      if (client)
      {
        client->processMessages(client);
        client->trackTimedOutMessages(client);                                                      
      }
      i++;
    } while ((client) && (i < nbClients));
  }

} 


