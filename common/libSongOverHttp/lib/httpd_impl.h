
#ifndef __SOH_HTTPD_IMPL__H__
#define __SOH_HTTPD_IMPL__H__

#define POSTBUFFERSIZE  1024
#define MAXCLIENTS      20

extern const char c_errorMsgTemplate[]; 

int sendStaticPage (struct MHD_Connection *connection, const char *page,
  int status_code);
void requestCompleted (void *cls, struct MHD_Connection *connection,                               
  void **conCls, enum MHD_RequestTerminationCode toe);
int answerToConnection (void *cls, struct MHD_Connection *connection,
  const char *url, const char *method, const char *version, const char *upload_data,                
  size_t *uploadDataSize, void **conCls);
void dumpHttpdStats(PF_DUMP_INFO_CB pFunc, void *issuerHandle);

void httpdClockMs();

void httpdInit(PF_SOH_CREATE_REQUEST_CB createCb, PF_SOH_RETRIEVE_REQUEST_CB retrieveCb,
  PF_SOH_UPDATE_REQUEST_CB updateCb, PF_SOH_DELETE_REQUEST_CB deleteCb);
void httpdUninit();

#endif

