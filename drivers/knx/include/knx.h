#ifndef __KNX__
#define __KNX__

#define _GNU_SOURCE

/*
 *  Overwrite drvcommon define
 */
#define CMN_IEEE_SIZE 128
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <poll.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <endian.h>
#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"
#include "cmnstruct.h"
#include "cmndefine.h"
#include "knx-type.h"
#define XO_APRES_2_1
#include "xo.h"
#include "xo-helpers.h"
#include "dIa.h"
#include "cproto.h"
#include "cmnproto.h"
#include "speccproto.h"
#include "specextern.h"
#include "knx-def.h"
#include "libknx.h"

#ifndef htobe16
# ifndef __BYTE_ORDER
#  error "__BYTE_ORDER undefined"
# endif
# ifndef __LITTLE_ENDIAN
#  error "__LITTLE_ENDIAN undefined"
# endif
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htobe16(x) __bswap_16 (x)
#  define htole16(x) (x)
#  define be16toh(x) __bswap_16 (x)
#  define le16toh(x) (x)

#  define htobe32(x) __bswap_32 (x)
#  define htole32(x) (x)
#  define be32toh(x) __bswap_32 (x)
#  define le32toh(x) (x)

#  define htobe64(x) __bswap_64 (x)
#  define htole64(x) (x)
#  define be64toh(x) __bswap_64 (x)
#  define le64toh(x) (x)
# else
#  define htobe16(x) (x)
#  define htole16(x) __bswap_16 (x)
#  define be16toh(x) (x)
#  define le16toh(x) __bswap_16 (x)

#  define htobe32(x) (x)
#  define htole32(x) __bswap_32 (x)
#  define be32toh(x) (x)
#  define le32toh(x) __bswap_32 (x)

#  define htobe64(x) (x)
#  define htole64(x) __bswap_64 (x)
#  define be64toh(x) (x)
#  define le64toh(x) __bswap_64 (x)
# endif
#endif

void DiaSetDefaultRequestMode(int flags);

/*
 *  Global from drvcommon / supervisor
 */
extern const char *knx_whatStr; 

extern t_cli *TbCli[];

extern  int Operating;
extern  char *rootactPath;
extern	int	TraceDebug;
extern	int	TraceLevel;
extern	int	TraceProto;
extern	int	PseudoOng;
extern	int	WithDia;

extern	int	DiaTraceRequest;

extern	void	*MainIQ;
extern	void	*MainTbPoll;
extern  void    *MdlCfg;
extern	dia_context_t	*DiaCtxt;

extern	int	DiaNbReqPending;
extern	int	DiaMaxReqPending;
extern	int	DiaConsecutiveTimeout;

extern	unsigned int	DiaSendCount;
extern	unsigned int	DiaRecvCount;
extern	unsigned int	DiaRtryCount;
extern	unsigned int	DiaTimeCount;
extern	unsigned int	DiaIpuOk;
extern	unsigned int	DiaNetOk;
extern	unsigned int	DiaIpuTmt;
extern	unsigned int	DiaNetTmt;
extern	unsigned int	DiaIpuErr;
extern	unsigned int	DiaNetErr;
extern	unsigned int	DiaRepDrp;

extern  NetworkList_t NetworkList;
#endif /* __KNX__ */
