#ifndef _TRANSACTION_T__H_
#define _TRANSACTION_T__H_

typedef struct transaction_t
{
  void (*free)(struct transaction_t *);
  void (*reset)(struct transaction_t *);
  void (*setTransId)(struct transaction_t *, char *transId);
  void (*setJTransaction)(struct transaction_t *, jobject jTransaction);
  void (*setReqUrl)(struct transaction_t *, char *url);
  void (*addReqHeader)(struct transaction_t *, char *name, char *value);
  void (*setReqMethod)(struct transaction_t *, char *method);
  void (*setReqBody)(struct transaction_t *, unsigned char *body, size_t len);
  void (*setReqProxy)(struct transaction_t *, char *proxy);
  int (*sendReq)(struct transaction_t *);
  char *(*dumpInfo)(struct transaction_t *, char *dest);

  jobject jTransaction;
  char *transId;
  CURL *curlEasyHandle;
  unsigned char *reqBody;
  size_t reqBodyLen;
  size_t readReqBodyLen;
  short contenLenHeaderAdded;
  response_t * response;

  struct httpClient_t *client;
  struct curl_slist *reqHeaderlist;
  struct list_head chainLink;

} transaction_t;

transaction_t *new_transaction_t(struct httpClient_t *client);
void transaction_t_newFree(transaction_t *This);

void transaction_t_reset(transaction_t *This);
void transaction_t_setTransId(transaction_t *This, char *transId);
void transaction_t_setJTransaction(transaction_t *This, jobject jTransaction);
void transaction_t_setReqUrl(transaction_t *This, char *url);
void transaction_t_addReqHeader(transaction_t *This, char *name, char *value);
void transaction_t_setReqMethod(transaction_t *This, char *method);
void transaction_t_setReqBody(transaction_t *This, unsigned char *body, size_t len);
void transaction_t_setReqProxy(transaction_t *This, char *proxy);
int transaction_t_sendReq(transaction_t *This);
char *transaction_t_dumpInfo(transaction_t *This, char *dest);

#endif

