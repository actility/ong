
#ifndef _CLT_TRANSACTION_T__H_
#define _CLT_TRANSACTION_T__H_


typedef struct cltTransaction_t
{
  void (* free) (struct cltTransaction_t *);
  void (* reset) (struct cltTransaction_t *);
  void (* setReqUrl) (struct cltTransaction_t *, char *url);
  void (* addReqHeader) (struct cltTransaction_t *, char *name, char *value);
  void (* setCompletionParam) (struct cltTransaction_t *, void *callback, void *issuerData);
  void (* setReqMethod) (struct cltTransaction_t *, SOH_METHOD method);
  void (* setReqBody) (struct cltTransaction_t *, unsigned char *body, size_t len);
  void (* setReqProxy) (struct cltTransaction_t *, char *proxy);
  void (* setReqRequestingEntity) (struct cltTransaction_t *, char *reqEntity);
  int (* sendReq) (struct cltTransaction_t *);
  void (* invokeRespCompletion) (struct cltTransaction_t *);
  char *(* dumpInfo) (struct cltTransaction_t *, char *dest);

  char transId[MAX_BUF_SIZE_XS];
  CURL *curlEasyHandle;
  unsigned char *reqBody;
  size_t reqBodyLen;
  size_t readReqBodyLen;
  short contenLenHeaderAdded;
  response_t *response;
  
  struct httpClient_t *client;
  struct curl_slist *reqHeaderlist;
  struct list_head chainLink;

  void *issuerData;
  SOH_METHOD method;
  union {
    PF_SOH_CREATE_RESPONSE_CB createRespCb;
    PF_SOH_RETRIEVE_RESPONSE_CB retrieveRespCb;
    PF_SOH_UPDATE_RESPONSE_CB updateRespCb;
    PF_SOH_DELETE_RESPONSE_CB deleteRespCb;
  } callbacks; 
 
} cltTransaction_t;

cltTransaction_t *new_cltTransaction_t(struct httpClient_t *client);
void cltTransaction_t_newFree(cltTransaction_t *This);

void cltTransaction_t_reset (cltTransaction_t *This);
void cltTransaction_t_setReqUrl (cltTransaction_t *This, char *url);
void cltTransaction_t_addReqHeader (cltTransaction_t *This, char *name, char *value);
void cltTransaction_t_setCompletionParam(cltTransaction_t *This, void *callback, void *issuerData);
void cltTransaction_t_setReqMethod (cltTransaction_t *This, SOH_METHOD method);
void cltTransaction_t_setReqBody (cltTransaction_t *This, unsigned char *body, size_t len);
void cltTransaction_t_setReqProxy (cltTransaction_t *This, char *proxy);
void cltTransaction_t_setReqRequestingEntity(cltTransaction_t *This, char *reqEntity);
int cltTransaction_t_sendReq (cltTransaction_t *This);
void cltTransaction_t_invokeRespCompletion(cltTransaction_t *This);
char *cltTransaction_t_dumpInfo (cltTransaction_t *This, char *dest);

#endif

