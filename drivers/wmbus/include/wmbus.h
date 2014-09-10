#ifndef _WMBUS_H_
#define _WMBUS_H_

#include <stdio.h>
#include <time.h>

#include "rtlbase.h"
#include "cmnstruct.h"
#include "cmndefine.h"
#include "cmnproto.h"


// rtl_trace levels
enum
{
  TRACE_ERROR = 0,
  TRACE_API = 1,
  TRACE_INFO = 2,
  TRACE_IMPL = 5,
  TRACE_DETAIL = 6,
  TRACE_FULL = 10,
};


#ifndef AW_DATA_DIR
#define	AW_DATA_DIR		"AW_DATA_DIR"
#endif

//#define DRIVER_DATA_DIR AW_DATA_DIR

#define IM_DRIVER                           8000
#define IM_DRIVER_IPU_CREATED               (IM_DRIVER + 1)
#define IM_DRIVER_NET_CREATED               (IM_DRIVER + 2)
#define IM_DRIVER_DEV_CREATED               (IM_DRIVER + 3)
#define IM_DRIVER_APP_CREATED               (IM_DRIVER + 4)
#define IM_DONGLE_CONFIGURATION_TIMEOUT     (IM_DRIVER + 100)
#define IM_DONGLE_READY                     (IM_DRIVER + 101)


#define WMBUS_REPORT_APP_CODE               0x72
#define WMBUS_RX_BUFFER_SIZE                512

#define DUMP_MEMORY(buffer, size)     do {  unsigned int i = 0;                        \
                                            unsigned char *pbuffer = (unsigned char *) buffer;        \
                                            printf("Zone memoire %p sur %u octets\n", buffer, size);     \
                                            do {    printf("%02hhX ", (unsigned char) pbuffer[i++]);    \
                                                if((i % 16) == 0) { printf("\n"); }            \
                                            } while( i < size );                        \
                                            printf("\n");                            \
                                        } while(0)
                                        

typedef int (*fn_preprocessor) (char *input, int inputLen, char *output, int outputMaxLen);
typedef int (*fn_cmd) (char *output, int outputMaxLen, int application, int cluster, int command, unsigned long long int cp_addr);


typedef struct _wmBusHeader wmBusHeader_t;
struct _wmBusHeader {
    unsigned char c;
    unsigned short man;         // FFS : Ensure byte order
    unsigned char sn[4];
    unsigned char version;
    unsigned char type;
} __attribute__((__packed__));

#define HEADER_TO_IEEE(x) (unsigned long long int)( \
    ((unsigned long long int)x->man << 40) + \
    ((unsigned long long int)x->version << 32) + \
    ((unsigned long long int)x->sn[3] << 24) + \
    ((unsigned long long int)x->sn[2] << 16) + \
    ((unsigned long long int)x->sn[1] << 8) + \
    (unsigned long long int)x->sn[0])
#define IEEE_TO_SN(x)   (x & 0xFFFFFFFFLL)
#define IEEE_TO_VER(x)   ((x & 0xFF00000000LL) >> 32)
#define IEEE_TO_MID(x)   ((x & 0xFFFF0000000000LL) >> 40)

#endif /* _WMBUS_H_ */
