#ifndef _RESPONSE_T__H_
#define _RESPONSE_T__H_

#define STATUS_CODE_PARSE_ERR 1
#define STATUS_CODE_CONNECT_FAILURE 2

typedef struct response_t
{
  void (*free)(struct response_t *);
  header_t *(*getHeader)(struct response_t *, char *);
  header_t *(*getNthHeader)(struct response_t *, int);
  int (*parseHeader)(struct response_t *, char *);
  int (*removeHeader)(struct response_t *, char *);
  void (*addToContent)(struct response_t *, unsigned char *, size_t);
  int (*parseStatusLine)(struct response_t *, char *, size_t);
  int (*getMappedStatusCode)(struct response_t *);

  /* from the status line */
  char *protocol;
  int statusCode;
  char *reasonPhrase;
  /* from the headers */
  struct list_head headers;
  /* from the body */
  unsigned char *content;
  size_t contentLength;

} response_t;

response_t *new_response_t();
void response_t_newFree(response_t *This);

header_t *response_t_getHeader(response_t *This, char *name);
header_t *response_t_getNthHeader(response_t *This, int index);
int response_t_parseHeader(response_t *This, char *headerLine);
int response_t_removeHeader(response_t *This, char *name);
int response_t_parseStatusLine(response_t *This, char *statusLine, size_t statusLineLen);
void response_t_addToContent(response_t *This, unsigned char *content, size_t contentLen);
int response_t_getMappedStatusCode(response_t *This);

#endif

