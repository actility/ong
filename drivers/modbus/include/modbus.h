#ifndef __MODBUS__
#define __MODBUS__

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
#include <errno.h>
#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"
#include "cmnstruct.h"
#include "cmndefine.h"
#include "modbus-def.h"
#include "modbus-type.h"
#define XO_APRES_2_1
#include "xo.h"
#include "xo-helpers.h"
#include "dIa.h"
#include "cproto.h"
#include "cmnproto.h"
#include "speccproto.h"
#include "specextern.h"
#include "libmodbus-acy.h"
#include "cproto.h"

/*
 *  Global from drvcommon / supervisor
 */
extern const char *modbus_whatStr; 

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

extern struct list_head NetworkInternalList;

#endif /* __MODBUS__ */
