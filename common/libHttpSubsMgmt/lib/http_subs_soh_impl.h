
#ifndef __HTTP_SUBS_SOH_IMPL__H__
#define __HTTP_SUBS_SOH_IMPL__H__

SOH_RCODE httpSubsSohCreateCb(char *reqEntity, char *targetID, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

SOH_RCODE httpSubsSohRetrieveCb(char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid);

SOH_RCODE httpSubsSohUpdateCb(char *reqEntity, char *targetID, unsigned char *content,
  size_t len, char *contentType, sohHeader_t **optHeaders, char *tid);

SOH_RCODE httpSubsSohDeleteCb(char *reqEntity, char *targetID,
  sohHeader_t **optHeaders, char *tid);

#endif

