
#ifndef _HTTP_CLIENT__H_
#define _HTTP_CLIENT__H_

void httpClientsInit(int nbMaxClients);
void httpClientsUninit();
void httpClientsClockMs();
httpClient_t * createHttpClient(t_cli *cl);
void releaseHttpClient(t_cli *cl);

#endif

