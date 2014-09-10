
#ifndef __CLT_SOH_IMPL__H__
#define __CLT_SOH_IMPL__H__

void testerCreateResponseCb (SOH_STATUS_CODE statusCode,
  char *resourceURI, unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);
void testerRetrieveResponseCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);
void testerUpdateResponseCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);
void testerDeleteResponseCb (SOH_STATUS_CODE statusCode,
  unsigned char *content, size_t len, char *contentType,
  sohHeader_t **optHeaders, char *tid, void *issuerData);

#endif

