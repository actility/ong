
#ifndef _HTTP_CLIENT__H_
#define _HTTP_CLIENT__H_

void httpClientsInit(int nbMaxClients);
void httpClientsUninit();
void httpClientsClockMs();
httpClient_t * createHttpClient(void *userContext, PF_HTTP_RESPONSE_CB httpResponseCb);
void releaseHttpClient(void *userContext);

#endif

