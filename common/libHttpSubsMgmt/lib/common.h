
#ifndef __HSM_COMMON__H__
#define __HSM_COMMON__H__

#define BUF_SIZE_S 128
#define BUF_SIZE_M 256
#define BUF_SIZE_L 1024

#define CONTENT_TYPE_VALUE "application/xml"
#define SUBS_STR_FORMAT "%lu-%s-%d"
#define SUBS_QPARAM_ID "id"

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

typedef enum _SUBS_PENDING_STATE
{
  SUBS_ST_INIT = 0,
  SUBS_ST_WAITING_SUBSCRIBE_RESP,
  SUBS_ST_WAITING_RESUBSCRIBE_RESP,
  SUBS_ST_WAITING_UNSUBSCRIBE_RESP,
  SUBS_ST_ACTIVE,
  SUBS_ST_EXPIRED,

} SUBS_PENDING_STATE;

char *xoSerializeAndFree(void *xo);
char *stateStr(SUBS_PENDING_STATE state);
char *statusCodeStr(SOH_STATUS_CODE statusCode);
char *buildErrInfoContent(SOH_STATUS_CODE statusCode, char *addInfo);

#endif

