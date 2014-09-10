
#ifndef _HTTP_CLIENT_T__H_
#define _HTTP_CLIENT_T__H_

typedef void (*PF_HTTP_RESPONSE_CB) (void *issuerHandle, cltTransaction_t *transaction);

typedef struct httpClient_t
{
  char *(* createReq) (struct httpClient_t *);
  cltTransaction_t *(* getTransaction) (struct httpClient_t *, char *transId);
  void (* removeReq) (struct httpClient_t *, char *transId);
  int (* sendReq) (struct httpClient_t *, char *transId, CURL *eh);
  void (* dumpInfo) (struct httpClient_t *, PF_DUMP_INFO_CB pFunc, void *issuerHandle);
  void (* setMaxTotalSockets) (struct httpClient_t *, int maxTotalSockets);
  void (* setMaxSocketsPerHost) (struct httpClient_t *, int maxSocketsPerHost);
  void (* setProxyUserPwd) (struct httpClient_t *, char *proxyUserPwd);
  void (* setProxyHostPort) (struct httpClient_t *, char *proxyHostPort);
  void (* setTcpNoDelay) (struct httpClient_t *, int tcpNoDelay);
  void (* setRequestTimeout) (struct httpClient_t *, long requestTimeout);
  void (* setConnectionTimeout) (struct httpClient_t *, long connectionTimeout);
  void (* invokeRespCompletion) (struct httpClient_t *, CURLcode result, CURL *easyHandle);
  void (* processMessages) (struct httpClient_t *);
  void (* trackTimedOutMessages) (struct httpClient_t *);
  void (* lock) (struct httpClient_t *);
  void (* unlock) (struct httpClient_t *);
  void (* free) (struct httpClient_t *);

  short curlDebugEnable;  
  CURLM *curlMultiHandle;
  cltTransactionsList_t *transactions;
  freeCltTransactionsList_t *freeTransactions;
  pthread_mutex_t mutex;
  char *proxyHostPort;
  char *proxyUserPwd;
  int tcpNoDelay;
  long requestTimeout;
  long connectionTimeout;

} httpClient_t;

httpClient_t *new_httpClient_t(short curlDebugEnable);
void httpClient_t_newFree(httpClient_t *This);

char *httpClient_t_createReq (httpClient_t *This);
cltTransaction_t *httpClient_t_getTransaction(httpClient_t *This, char *transId);
void httpClient_t_removeReq (httpClient_t *This, char *transId);
int httpClient_t_sendReq(httpClient_t *This, char *transId, CURL *eh);
void httpClient_t_setMaxTotalSockets (httpClient_t *This, int maxTotalSockets);
void httpClient_t_setMaxSocketsPerHost (httpClient_t *This, int maxSocketsPerHost);
void httpClient_t_setProxyUserPwd (httpClient_t *This, char *proxyUserPwd);
void httpClient_t_setProxyHostPort (httpClient_t *This, char *proxyHostPort);
void httpClient_t_setTcpNoDelay (httpClient_t *This, int tcpNoDelay);
void httpClient_t_setRequestTimeout (httpClient_t *This, long requestTimeout);
void httpClient_t_setConnectionTimeout (httpClient_t *This, long connectionTimeout);
void httpClient_t_dumpInfo(httpClient_t *This, PF_DUMP_INFO_CB pFunc, void *issuerHandle);
void httpClient_t_invokeRespCompletion(httpClient_t *, CURLcode result, CURL *easyHandle);
void httpClient_t_processMessages (httpClient_t *This);
void httpClient_t_trackTimedOutMessages (httpClient_t *This);
void httpClient_t_lock (httpClient_t *This);
void httpClient_t_unlock (httpClient_t *This);

#endif


