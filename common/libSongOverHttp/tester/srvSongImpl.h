
#ifndef __SRV_SOH_IMPL__H_
#define __SRV_SOH_IMPL__H_

SOH_RCODE testerSohCreateCb (char *reqEntity, char *targetID,
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);
SOH_RCODE testerSohRetrieveCb (char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid);
SOH_RCODE testerSohUpdateCb (char *reqEntity, char *targetID,
  unsigned char *content, size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);
SOH_RCODE testerSohDeleteCb (char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid);

#endif

