
#ifndef _COMMON__H_
#define _COMMON__H_

#define MAX_BUF_SIZE_S 127
#define MAX_BUF_SIZE_B 255
#define MAX_BUF_SIZE_H 1023
#define MAX_BUF_SIZE_VL 2047

#define METHOD_GET "GET"
#define METHOD_POST "POST"
#define METHOD_PUT "PUT"
#define METHOD_DELETE "DELETE"

#define HEADER_CONTENT_LENGTH "Content-Length"

#define DEFAULT_HTTPCLIENT_MAX_CONNECTS 5
#define DEFAULT_HTTPCLIENT_PIPELINE 0L
#define DEFAULT_TRANSACTION_CONNECT_TIMEOUT 10000L
#define DEFAULT_TRANSACTION_TIMEOUT 10000L

#define LOG(l,...) \
{ \
  extern int g_traceLevel;\
  char *pPtr;\
  if (g_traceLevel >= (l))\
  {\
    pPtr = strrchr(__FILE__, '/');\
    pPtr++;\
    trace((l), pPtr, __LINE__, __VA_ARGS__);\
  }\
}

// trace levels
enum
{
  TRACE_FATAL = 0,
  TRACE_ERROR = 1,
  TRACE_WARN = 2,
  TRACE_INFO = 3,
  TRACE_DEBUG = 4,
  TRACE_TRACE = 5,
};

void trace(int level, char *file, int line, char *msg, ...);

#endif
