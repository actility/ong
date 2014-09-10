
#ifndef _SRV_TRANSACTION_T__H_
#define _SRV_TRANSACTION_T__H_

#define MAX_HEADERS 10

typedef struct srvTransaction_t
{
  void (* free) (struct srvTransaction_t *);
  void (* setReqUrl) (struct srvTransaction_t *, const char *url);
  void (* setReqContentType) (struct srvTransaction_t *, const char *contentType);
  void (* setReqEntity) (struct srvTransaction_t *, char *reqEntity);
  void (* addReqHeader) (struct srvTransaction_t *, const char *name, 
    const char *value);
  void (* setReqMethod) (struct srvTransaction_t *, SOH_METHOD method);
  void (* setReqBody) (struct srvTransaction_t *, const char *body, size_t len);
  int (* sendResp) (struct srvTransaction_t *, SOH_STATUS_CODE statusCode, 
    char *resourceURI, unsigned char *content, size_t len, char *contentType, 
    sohHeader_t **optHeaders);
  char *(* dumpInfo) (struct srvTransaction_t *, char *dest);

  char transId[MAX_BUF_SIZE_XS];
  time_t start;
  time_t expired;
  char *reqUrl;
  char *reqContentType;
  char *reqEntity;
  struct MHD_Connection *mhdConnection;
  unsigned char *reqBody;
  size_t reqBodyLen;
  size_t readReqBodyLen;
  sohHeader_t *headers[MAX_HEADERS+1];
  
  struct list_head chainLink;

  SOH_METHOD method;
 
} srvTransaction_t;

srvTransaction_t *new_srvTransaction_t(struct MHD_Connection *con);
void srvTransaction_t_newFree(srvTransaction_t *This);

void srvTransaction_t_setReqUrl (srvTransaction_t *This, const char *url);
void srvTransaction_t_addReqHeader (srvTransaction_t *This, const char *name,
  const char *value);
void srvTransaction_t_setReqContentType (srvTransaction_t *This, const char *contentType);
void srvTransaction_t_setReqEntity (srvTransaction_t *This, char *reqEntity);
void srvTransaction_t_setReqMethod (srvTransaction_t *This, SOH_METHOD method);
void srvTransaction_t_setReqBody (srvTransaction_t *This, const char *body, size_t len);
int srvTransaction_t_sendResp (srvTransaction_t *This, SOH_STATUS_CODE statusCode, 
  char *resourceURI, unsigned char *content, size_t len, char *contentType, 
  sohHeader_t **optHeaders);
char *srvTransaction_t_dumpInfo (srvTransaction_t *This, char *dest);

#endif

