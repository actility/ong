/*
 * Copyright (C) Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 */

//************************************************************************
//************************************************************************
//**
//** File : ./azap_tester/admin_cli.c
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************

#include <stdio.h>      /************************************/
#include <stdlib.h>     /*     run in main thread           */
#include <string.h>     /************************************/
#include <stdarg.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>
#include <string.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>


#include "rtlbase.h"
#include "rtllist.h"
#include "rtlimsg.h"

#include "admin_cli.h"

#include "azap_types.h"
#include "azap_api.h"
#include "azap_api_zcl.h"
#include "azap_zcl_attr_val.h"
#include "tester_req_context.h"


#define FD_MAX    CLI_MAX_FD
#define TCP_FRAME_SZ  1024


extern void *MainTbPoll;
extern azap_uint8 azapPort;

t_cli *TbCli[FD_MAX];

static  int FdListen = -1;
static  int TcpKeepAlive  = 1;

static char *checkHexIntArg(t_cli *cl, char *buf, char *argname, int min, int max,
    int *val);
static char *checkIntArg(t_cli *cl, char *buf, char *argname, int min, int max,
    int *val);
static char *checkStrArg(t_cli *cl, char *buf, char *argname, char *possibleVal,
    char *val);

/**********************************************************/
/* HELP MSG */
static void AdmHelp(t_cli *cl)
{
  AdmPrint(cl, "help, ?     : display this help message\n");
  AdmPrint(cl, "exit, quit  : end the current CLI session\n");
  AdmPrint(cl, "core        : do generate a core file of the current process\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, " ================ ZDP ==================\n");
  AdmPrint(cl, "permitjoin <duration>\n");
  AdmPrint(cl, "ieeeAddrReq <addr>\n");
  AdmPrint(cl, "mgmtRtgReq <addr>\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, " ================ ZCL ==================\n");
  AdmPrint(cl, "IDENTIFY CLUSTER\n");
  AdmPrint(cl, "zclIdentify <addr> <epId> <duration>\n");
  AdmPrint(cl, "      send ZCL Identify command, asking for flashing <duration> seconds\n");
  AdmPrint(cl, "zclIdentifyQuery <addr> <epId>\n");
  AdmPrint(cl, "      send ZCL IdentifyQuery command\n");
  AdmPrint(cl, "\n");
  AdmPrint(cl, "ON OFF CLUSTER\n");
  AdmPrint(cl, "zclToggle <addr> <epId>: send ZCL Toggle command\n");
  AdmPrint(cl, "      send ZCL Toggle command\n");
}

/**********************************************************/
/* SUPPORTED COMMANDS */
static  int AdmCommand(t_cli *cl, char *buf)
{
  int addr;
  int epId;
  int duration;
  azap_byte payload[255];
  azap_uint8 len;
  azap_byte *pPayloadPtr;
  azapZclAttrVal_t *zclparam;
  testerReqContext_t *context;
  if  (!strlen(buf))
  {
    return  CLI_PROMPT;
  }

  if ((0 == strcmp(buf, "help")) || (0 == strcmp(buf, "?")))
  {
    AdmHelp(cl);
    return CLI_PROMPT;
  }
  if ((0 == strcmp(buf, "quit")) || (0 == strcmp(buf, "exit")))
  {
    return CLI_DISC;
  }
  if (0 == strcmp(buf, "core"))
  {
    rtl_docore();
    return CLI_PROMPT;
  }
  //////////////////////////////// ZCL /////////////////////////////////////////
  if (0 == strncmp(buf, "permitjoin", strlen("permitjoin")))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "permitjoin", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "duration", 0, 255, &duration)))
    {
      return CLI_PROMPT;
    }

    context = new_testerReqContext_t(TSTER_REQ_FROM_CLI);
    context->issuerData = cl;

    if (!azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, context, 0,
        duration, true, true))
    {
      context->free(context);
      AdmPrint(cl, "Failed to send azapZdpMgmtPermitJoinReq\n");
    }
    azapZdpMgmtPermitJoinReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, 0xFFFF, duration,
        true, true);
    return CLI_PROMPT;
  }

  if (0 == strncmp(buf, "ieeeAddrReq", strlen("ieeeAddrReq")))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "ieeeAddrReq", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkHexIntArg(cl, buf, "addr", 0x00000000, 0x0000FFFF, &addr)))
    {
      return CLI_PROMPT;
    }

    context = new_testerReqContext_t(TSTER_REQ_FROM_CLI);
    context->issuerData = cl;

    if (!azapZdpIeeeAddrReq(azapPort, AZAP_REQ_TTL_START_AT_API, context, addr, addr, 1, 0))
    {
      context->free(context);
      AdmPrint(cl, "Failed to send azapZdpIeeeAddrReq\n");
    }
    return CLI_PROMPT;
  }

  if (0 == strncmp(buf, "mgmtRtgReq", strlen("mgmtRtgReq")))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "mgmtRtgReq", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkHexIntArg(cl, buf, "addr", 0x00000000, 0x0000FFFF, &addr)))
    {
      return CLI_PROMPT;
    }

    context = new_testerReqContext_t(TSTER_REQ_FROM_CLI);
    context->issuerData = cl;

    if (!azapZdpMgmtRtgReq(azapPort, AZAP_REQ_TTL_START_AT_API, context, addr, 0))
    {
      context->free(context);
      AdmPrint(cl, "Failed to send azapZdpMgmtRtgReq\n");
    }
    return CLI_PROMPT;
  }


  //////////////////////////////// ZDP /////////////////////////////////////////
  if (0 == strncmp(buf, "zclIdentifyQuery", strlen("zclIdentifyQuery")))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "zclIdentifyQuery", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkHexIntArg(cl, buf, "addr", 0x00000000, 0x0000FFFF, &addr)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "epId", 1, 240, &epId)))
    {
      return CLI_PROMPT;
    }

    context = new_testerReqContext_t(TSTER_REQ_FROM_CLI);
    context->issuerData = cl;

    if (!azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, context, addr,
        1, epId, ZCL_CLUSTER_ID_GEN_IDENTIFY,
        ZCL_FRAME_CLIENT_SERVER_DIR, false, COMMAND_IDENTIFY_QUERY,
        COMMAND_IDENTIFY_QUERY_RSP, NULL, 0))
    {
      context->free(context);
      AdmPrint(cl, "Failed to send azapZclOutgoingCmdReq\n");
    }
    return CLI_PROMPT;
  }

  if (0 == strncmp(buf, "zclIdentify", strlen("zclIdentify")))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "zclIdentify", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkHexIntArg(cl, buf, "addr", 0x00000000, 0x0000FFFF, &addr)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "epId", 1, 240, &epId)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "duration", 1, 255, &duration)))
    {
      return CLI_PROMPT;
    }

    len = 2;
    pPayloadPtr = payload;
    zclparam = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT16);
    zclparam->setIntegerValue(zclparam, duration);
    zclparam->serialize(zclparam, &pPayloadPtr, &len);
    zclparam->free(zclparam);

    if (!azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, addr,
        1, epId, ZCL_CLUSTER_ID_GEN_IDENTIFY,
        ZCL_FRAME_CLIENT_SERVER_DIR, false, COMMAND_IDENTIFY,
        0xFF, payload, 2))
    {
      AdmPrint(cl, "Failed to send azapZclOutgoingCmdReq\n");
    }
    return CLI_PROMPT;
  }

  //////////////////////////////// ZCL /////////////////////////////////////////
  if (0 == strncmp(buf, "zclIdentify", strlen("zclIdentify")))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "zclIdentify", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkHexIntArg(cl, buf, "addr", 0x00000000, 0x0000FFFF, &addr)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "epId", 1, 240, &epId)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "duration", 1, 255, &duration)))
    {
      return CLI_PROMPT;
    }

    len = 2;
    pPayloadPtr = payload;
    zclparam = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT16);
    zclparam->setIntegerValue(zclparam, duration);
    zclparam->serialize(zclparam, &pPayloadPtr, &len);
    zclparam->free(zclparam);

    if (!azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, addr,
        1, epId, ZCL_CLUSTER_ID_GEN_IDENTIFY,
        ZCL_FRAME_CLIENT_SERVER_DIR, false, COMMAND_IDENTIFY,
        0xFF, payload, 2))
    {
      AdmPrint(cl, "Failed to send azapZclOutgoingCmdReq\n");
    }
    return CLI_PROMPT;
  }

  if (0 == strncmp(buf, "zclToggle", 9))
  {
    if (!(buf = checkStrArg(cl, buf, "command", "zclToggle", NULL)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkHexIntArg(cl, buf, "addr", 0x00000000, 0x0000FFFF, &addr)))
    {
      return CLI_PROMPT;
    }
    if (!(buf = checkIntArg(cl, buf, "epId", 1, 240, &epId)))
    {
      return CLI_PROMPT;
    }
    if (!azapZclOutgoingCmdReq(azapPort, AZAP_REQ_TTL_START_AT_API, NULL, addr,
        1, epId, ZCL_CLUSTER_ID_GEN_ON_OFF,
        ZCL_FRAME_CLIENT_SERVER_DIR, false, COMMAND_TOGGLE, 0xFF, NULL, 0))
    {
      AdmPrint(cl, "Failed to send azapZclOutgoingCmdReq\n");
    }
    return CLI_PROMPT;
  }

  return CLI_NFOUND;
}

/**********************************************************/
static char *checkHexIntArg(t_cli *cl, char *buf, char *argname, int min, int max, int *val)
{
  char arg[32];
  arg[0]  = '\0';
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (!*arg)
  {
    AdmPrint(cl, "Missing argument\n");
    return NULL;
  }

  if (!val)
  {
    return buf;
  }

  if (! sscanf(arg, "0x%x", val))
  {
    AdmPrint(cl, "Wrong argument format (%s=%s)\n", argname, arg);
    return NULL;
  }
  if ((*val > max) || (*val < min))
  {
    AdmPrint(cl, "Wrong argument value (%s=%s)\n", argname, arg);
    return NULL;
  }
  return buf;
}

static char *checkIntArg(t_cli *cl, char *buf, char *argname, int min, int max, int *val)
{
  char arg[32];
  arg[0]  = '\0';
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  if (strlen(arg) == 0)
  {
    AdmPrint(cl, "Missing argument\n");
    return NULL;
  }
  if (!val)
  {
    return buf;
  }
  *val = atoi(arg);
  if ((*arg < '0') || (*arg > '9') || (*val > max) || (*val < min))
  {
    AdmPrint(cl, "Wrong argument (%s=%s)\n", argname, arg);
    return NULL;
  }
  return buf;
}


static char *checkStrArg(t_cli *cl, char *buf, char *argname, char *possibleVal, char *val)
{
  char arg[32];
  arg[0]  = '\0';
  buf = AdmAdvance(buf);
  buf = AdmAcceptKeep(buf, arg);
  char *b, *e;
  char szTemp[32];
  int found = 0;
  if (strlen(arg) == 0)
  {
    AdmPrint(cl, "Missing argument\n");
    return NULL;
  }
  if ((possibleVal) && (*possibleVal))
  {
    b = possibleVal;
    while ((*b) && (!found))
    {
      e = strchr(b, '|');
      if (e)
      {
        strncpy(szTemp, b, e - b);
        szTemp[e - b] = '\0';
        e++;
      }
      else
      {
        e = b + strlen(b);
      }
      if (0 == strcmp(b, arg))
      {
        found = 1;
      }
      b = e;
    }
    if (!found)
    {
      AdmPrint(cl, "Wrong argument (%s=%s)\n", argname, arg);
      return NULL;
    }
  }
  if (!val)
  {
    return buf;
  }
  sprintf(val, arg);
  return buf;
}
/**********************************************************/
static char *GetTelnetKey()
{
  return "";
}

static char *GetTelnetPort()
{
  return "2002";
}

/*!
 *
 * @brief set keep alive attributs on a fd socket
 * Actualy we dont change system attributs, we just set keep alive mode.
 * @param fd the socket fd
 * @return void
 *
 */
static  void  AdmKeepAlive(int fd)
{
  if  ( TcpKeepAlive > 0 )
  {
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
        (char *)&TcpKeepAlive, sizeof(TcpKeepAlive));
#if 0
    if  ( TcpKeepIdle > 0 )
    {
      setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE,
          (char *)&TcpKeepIdle, sizeof(TcpKeepIdle));
    }
    if  ( TcpKeepIntvl > 0 )
    {
      setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL,
          (char *)&TcpKeepIntvl, sizeof(TcpKeepIntvl));
    }
    if  ( TcpKeepCnt > 0 )
    {
      setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT,
          (char *)&TcpKeepCnt, sizeof(TcpKeepCnt));
    }
#endif
  }
}



/*!
 *
 * @brief Call back function when we need to store data waiting to be sent
 * @param cl the destination connection
 * @param buf the data to store
 * @param len the lenght of data
 * @return void
 *
 */
static  void  AdmAddBuf(t_cli *cl, char *buf, int len)
{
  //int fd;
  t_lout  *out;

  //fd  = cl->cl_fd;

  RTL_TRDBG(9, "Adm add buffer lg=%d\n", len);
  if  (!buf || len <= 0)
  {
    return;
  }

  // buf deja decouper buf en morceaux plus petits que TCP_FRAME_SZ
  out = (t_lout *)malloc(sizeof(t_lout));
  if  ( !out )
  {
    return;
  }

  out->o_len  = len;
  out->o_buf  = (char *)malloc(len);
  if  ( !out->o_buf )
  {
    return;
  }
  memcpy  (out->o_buf, buf, len);

  list_add_tail( &(out->list), &(cl->cl_lout.list) );
}

/*!
 *
 * @brief get the number of waiting buffers on a connection
 * @param cl the destination connection
 * @return the number of waiting buffers
 *
 */
int AdmTcpMsgCount(t_cli *cl)
{
  int nb  = 0;
  t_lout *msg;

  list_for_each_entry(msg, &cl->cl_lout.list, list)
  {
    nb++;
  }
  return  nb;
}


/*!
 *
 * @brief Call back function when write permission is detected for a connection
 * Concat waiting data while total size is less than TCP_FRAME_SZ. We assume
 * that we can write TCP_FRAME_SZ bytes without blocking if POLLOUT is detected.
 * @param cli the connection on which write permission is detected
 * @return void
 *
 */
static  void  AdmAsyncWrite(t_cli *cl)
{
  int fd;
  t_lout  *lst;
  t_lout  *msg;
  int msgsz;
  int nb;
  int sz;
  char  out[TCP_FRAME_SZ];

  fd  = cl->cl_fd;
  lst = &cl->cl_lout;

  if  ( list_empty(&lst->list) )
  {
    return;
  }

  nb  = 0;
  sz  = 0;
  while ( !list_empty(&lst->list) )
  {
    msg = list_entry(lst->list.next, t_lout, list);
    msgsz = msg->o_len;
    if  ( sz + msgsz > sizeof(out) )
    {
      RTL_TRDBG(2, "defragmentation sz=%d no more than %d\n",
          sz + msgsz, sizeof(out));
      break;
    }
    list_del(&msg->list);
    memcpy  (out + sz, msg->o_buf, msgsz);
    sz  = sz + msgsz;
    nb++;
    free(msg->o_buf);
    free(msg);
  }

  if ( nb <= 0 )
  {
    RTL_TRDBG(0, "bad defragmentation nb=%d sz=%d\n", nb, sz);
    return;
  }

  if ( nb > 0 && sz > 0 )
  {
    time(&cl->cl_lasttcpwrite);
    RTL_TRDBG(9, "write adm/tcp nb=%d sz=%d\n", nb, sz);
    if (write(fd, out, sz) != sz)
    {
      RTL_TRDBG(0, "AdmAsyncWrite() write error\n");
    }
  }
}


char  *AdmAdvance(char *pt)
{
  while ( *pt && ( *pt == ' ' || *pt == '\t' ) )
  {
    pt++;
  }
  return  pt;
}

char  *AdmAccept(char *pt)
{
  while ( *pt && ( *pt != ' ' && *pt != '\t' ) )
  {
    pt++;
  }
  return  pt;
}

char  *AdmAcceptKeep(char *pt, char *dst)
{
  *dst  = '\0';
  while ( *pt && ( *pt != ' ' && *pt != '\t' ) )
  {
    *dst++  = *pt++;
  }
  *dst  = '\0';
  return  pt;
}



/*!
 *
 * @brief write synchronously data on cli connection
 * Format is printf like. Usage of this function must be limited, because it
 * can block until remote client reads data. Max data can not exceed 5KB.
 * @param fd the destination file descriptor
 * @param fmt ... printf(3)
 * @return void
 *
 */
void  AdmPrintSync(int fd, char *fmt, ...)
{
  va_list listArg;
  char  tmp[5 * 1024] ;
  int lg ;

  va_start(listArg, fmt);
  vsprintf(tmp, fmt, listArg);
  va_end(listArg);

  lg  = strlen(tmp);
  if  ( tmp[lg - 1] == '\n' )
  {
    tmp[lg - 1] = '\r';
    tmp[lg]   = '\n';
    lg++;
  }

  if (write(fd, tmp, lg) != lg)
  {
    RTL_TRDBG(0, "AdmPrintSync() write error\n");
  }
}

/*!
 *
 * @brief write asynchronously data on cli connection
 * Format is printf like. Max data can not exceed 5KB. If parameter fmt is
 * greater than 512B, we assume this is not a printf format but data itself.
 * @param cl the destination connection
 * @param fmt ... printf(3)
 * @return void
 *
 */
void  AdmPrint(t_cli *cl, char *fmt, ...)
{
  va_list listArg;
  char  tmp[5 * 1024] ;
  int lg ;
  int out;
  int dep;

  if  ( (lg = strlen(fmt)) < 512 )
  {
    va_start(listArg, fmt);
    vsprintf(tmp, fmt, listArg);
    va_end(listArg);

    lg  = strlen(tmp);
    if  ( tmp[lg - 1] == '\n' )
    {
      tmp[lg - 1] = '\r';
      tmp[lg]   = '\n';
      lg++;
    }
  }
  else
  {
    // if fmt is more than 512 we assume there is no a format
    if  ( lg > sizeof(tmp) )
    {
      lg  = sizeof(tmp);
    }
    memcpy  (tmp, fmt, lg);
  }


  dep = 0;
  do
  {
    if  ( lg > TCP_FRAME_SZ / 2 )
    {
      out = TCP_FRAME_SZ / 2;
    }
    else
    {
      out = lg;
    }
    AdmAddBuf(cl, tmp + dep, out);
    dep = dep + out;
    lg  = lg - out;
  }
  while ( lg > 0 );

}


void  AdmPrintTable(t_cli *cl, int nb, int tab[], char *fmt, char pstart, char pend, char psep)
{
  int n;

  for (n = 0 ; n < nb ; n++)
  {
    if  (n == 0)
    {
      AdmPrint(cl, "%c", pstart);
    }
    AdmPrint(cl, fmt, tab[n]);
    if  (n == nb - 1)
    {
      AdmPrint(cl, "%c", pend);
    }
    else
    {
      AdmPrint(cl, "%c", psep);
    }
  }

}

void  AdmPrintDefTable(t_cli *cl, int nb, int tab[], char *fmt)
{
  return  AdmPrintTable(cl, nb, tab, fmt, '[', ']', ' ');
}

void  AdmPrintTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt, char pstart, char pend, char psep)
{
  int n;

  for (n = 0 ; n < nb ; n++)
  {
    if  (n == 0)
    {
      AdmPrint(cl, "%c", pstart);
    }
    AdmPrint(cl, fmt, tab[n]);
    if  (n == nb - 1)
    {
      AdmPrint(cl, "%c", pend);
    }
    else
    {
      AdmPrint(cl, "%c", psep);
    }
  }

}

void  AdmPrintDefTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt)
{
  return  AdmPrintTableUS(cl, nb, tab, fmt, '[', ']', ' ');
}

/*!
 *
 * @brief allocate and initialize a new t_cli structure
 * @param fd the socket file descriptor associated to this connection
 * @return the allocated structure or NULL
 *
 */
static  t_cli *CliNew(int fd)
{
  t_cli *cl;

  cl  = (t_cli *)malloc(sizeof(t_cli));
  if  (!cl)
  {
    RTL_TRDBG(0, "malloc failure\n");
    return  NULL;
  }
  memset  (cl, 0, sizeof(t_cli));

  cl->cl_fd = fd;
  time(&cl->cl_lasttcpread);
  INIT_LIST_HEAD(&cl->cl_lout.list);

  RTL_TRDBG(1, "new cli %x %d\n", cl, cl->cl_fd);

  return  cl;
}

/*!
 *
 * @brief deallocate a cli connection
 *  - close the socket fd
 *  - remove the fd from the poll table
 *  - remove all pending messages
 *  - adjust the sniffers count
 * @param cli the connection
 * @return void
 *
 */
static  void  CliFree(t_cli *cl)
{
  int nb;

  if  (!cl)
  {
    return;
  }
  RTL_TRDBG(1, "free cli %x fd=%d\n", cl, cl->cl_fd);

  if (cl->cl_fd > 0 && cl->cl_fd < FD_MAX)
  {
    TbCli[cl->cl_fd] = NULL;
    close(cl->cl_fd);
    rtl_pollRmv(MainTbPoll, cl->cl_fd) ;
  }

  // clear & free outgoing message list
  nb  = 0;
  while ( !list_empty(&cl->cl_lout.list) )
  {
    t_lout *msg;

    msg = list_entry(cl->cl_lout.list.next, t_lout, list);
    list_del(&msg->list);
    free(msg->o_buf);
    free(msg);
    nb++;
  }
  free(cl);

  RTL_TRDBG(1, "adm connection is now cleared (wmsg=%d)\n", nb);
}


static  int AdmPassword(t_cli *cl, char *buf)
{
  if  ( strcmp(GetTelnetKey(), buf) )
  {
    cl->cl_nbkey++;
    if  ( cl->cl_nbkey >= 3 )
    {
      return  CLI_DISC;
    }
    return  CLI_KEY;
  }
  cl->cl_okkey  = 1;
  cl->cl_nbkey  = 0;
  return  CLI_PROMPT;
}


/*!
 *
 * @brief main loop for a connection
 *  - store user input until we get a '\r' or a '\n'
 *  - get password if any
 *  - search command in built in commands
 *  - call specific project commands
 *  - prompt user for a new command
 * @param cl the current connection
 * @param buf user input
 * @param lg lenght of user input
 * @return void
 *
 */
static  void  AdmLoopCmd(t_cli *cl, char *buf, int lg)
{
  char  *ptn;
  char  *ptr;
  char  *pt;
  int ret;
  int stop;


  strcat  (cl->cl_cmd, buf);
  cl->cl_pos  += lg;

  ptr = strrchr(cl->cl_cmd, '\r');
  ptn = strrchr(cl->cl_cmd, '\n');
  if  ( !ptr && !ptn )
  {
    return;
  }

  if  ( ptr )
  {
    *ptr  = '\0';
  }
  if  ( ptn )
  {
    *ptn  = '\0';
  }

  RTL_TRDBG(5, "adm get command '%s'\n", cl->cl_cmd);
  stop  = 0;
  pt  = cl->cl_cmd;
  while (!stop)
  {
    while ( *pt == '\n' || *pt == '\r' || *pt == '\t' )
    {
      pt++;
    }
    lg  = 0;
    while ( *pt )
    {
      if  ( *pt == '\n' || *pt == '\r' || *pt == '\t' )
      {
        pt++;
        break;
      }
      buf[lg] = *pt;
      pt++;
      lg++;
    }
    buf[lg] = '\0';
    if (strlen(buf))
    {
      RTL_TRDBG(1, "adm treat '%s'\n", buf);
    }

    if  ( !cl->cl_okkey && strlen(GetTelnetKey()) )
    {
      ret = AdmPassword(cl, buf);
    }
    else
    {
      ret = AdmCommand(cl, buf);
    }
    switch  (ret)
    {
      case  CLI_NFOUND  :
        AdmPrint(cl, "command not found\r\n");
        AdmPrint(cl, ">>");
        break;
      case  CLI_PROMPT  :
        AdmPrint(cl, ">>");
        break;
      case  CLI_DISC  :
        CliFree(cl);
        stop  = 1;
        cl  = NULL;
        break;
      case  CLI_KEY :
        AdmPrint(cl, "key:");
        break;
      case  CLI_EMPTY :
        AdmPrint(cl, ">>");
        break;
      case  CLI_MORE :
        AdmPrint(cl, "++");
        break;
    }
    if  ( !*pt )
    {
      break;
    }
  }
  if  (cl)
  {
    cl->cl_pos  = 0;
    memset  (cl->cl_cmd, 0, sizeof(cl->cl_cmd));
  }
}

/*!
 *
 * @brief Call back function for rtlPoll to get the requested events.
 * This call back function is raised when rtlpoll needs to set the appropriate
 * POLL flags for the provided file descriptor.
 * The question is: on the CLI session matching the provided file descriptor,
 * do we want to read ? and do we want to write ?
 * We always want to raise the POLLIN flag, and we eventually want to raise
 * POLLOUT flag if we have something queued, waiting for write permission.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events need to be detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current requested events
 * @return -1 in case of error detection, 0 otherwise
 *
 */

int CB_AdmTcpRequest(void *tb, int fd, void *r1, void *r2, int revents)
{
  t_cli *cl;
  int empty;

  if  (fd >= FD_MAX || (cl = TbCli[fd]) == NULL)
  {
    RTL_TRDBG(0, "adm no cli for fd=%d\n", fd);
    rtl_pollRmv(MainTbPoll, fd);
    return  -1;
  }

  empty = list_empty(&cl->cl_lout.list);
  RTL_TRDBG(9, "callback request adm/tcp fd=%d empty=%d e=%d\n",
      fd, empty, revents);

  // if we have something to write ask for POLLOUT event and POLLIN
  // or just POLLOUT if we want to slow down remote writer
  if  ( !empty )
  {
    return  POLLIN | POLLOUT;
  }

  return  POLLIN;
}

/*!
 *
 * @brief Call back function for rtlPoll to treat the detected events.
 * This call back function is raised when rtlpoll detects events on the
 * the provided file descriptor. Here the file descriptor is a socket on a
 * established connection.
 * If POLLOUT is detected we try to write data if any.
 * If POLLIN is detected we try to read data
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current detected events
 * @return -1 in case of error detection, 0 otherwise
 *
 */
int CB_AdmTcpEvent(void *tb, int fd, void *r1, void *r2, int revents)
{
  t_cli *cl;
  int lg;
  char  buf[500];

  RTL_TRDBG(9, "callback event adm/tcp fd=%d e=%d r1=%lx r2=%lx\n",
      fd, revents, r1, r2);

  if  (fd >= FD_MAX || (cl = TbCli[fd]) == NULL)
  {
    RTL_TRDBG(0, "adm no cli for fd=%d\n", fd);
    rtl_pollRmv(MainTbPoll, fd);
    return  -1;
  }

  if ((revents & POLLOUT) == POLLOUT)
  {
    RTL_TRDBG(9, "event write adm/tcp fd=%d\n", fd);
    AdmAsyncWrite(cl);
  }

  if ((revents & POLLIN) != POLLIN)
  {
    return  0;
  }
  lg  = read(fd, buf, sizeof(buf));
  if  ( lg <= 0 )
  {
    RTL_TRDBG(1, "Adm connection ended by remote\n");
    CliFree(cl);
    return  0;
  }
  buf[lg] = '\0';
  RTL_TRDBG(5, "event read adm/tcp fd=%d lg=%d\n", fd, lg);
  time(&cl->cl_lasttcpread);


  if  ( cl->cl_pos + lg >= sizeof(cl->cl_cmd) )
  {
    RTL_TRDBG(1, "command too long => reset\n");
    CliFree(cl);
    return  0;
  }
  AdmLoopCmd(cl, buf, lg);


  return  0;
}

static  void  SetCB(int fdnew)
{
  rtl_pollAdd(MainTbPoll, fdnew, CB_AdmTcpEvent, CB_AdmTcpRequest, NULL, NULL);
}

/*!
 *
 * @brief Call back function for rtlPoll to treat the detected events.
 * This call back function is raised when rtlpoll detects events on the
 * the provided file descriptor. Here the file descriptor is a socket waiting
 * for connections (a listener socket).
 * If POLLIN is detected we :
 *  - accept the connection with a new socket
 *  - create a new t_cli structure
 *  - add the new socket in the table of polled descriptors.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current detected events
 * @return -1 in case of error detection, 0 otherwise
 *
 */
int CB_AdmTcpListen(void *tb, int fd, void *r1, void *r2, int revents)
{
  t_cli *cl;
  int fdnew;
  struct sockaddr_in6 from;
  socklen_t lenaddr;

  RTL_TRDBG(1, "callback event adm/tcp fd=%d FD=%d e=%d r1=%lx r2=%lx\n",
      fd, FdListen, revents, r1, r2);

  if (fd != FdListen)
  {
    RTL_TRDBG(0, "fd=%d != myfd=%d\n", fd, FdListen);
    rtl_pollRmv(MainTbPoll, fd);
    return  -1;
  }

  if ((revents & POLLIN) != POLLIN)
  {
    return  0;
  }

  lenaddr = sizeof(from) ;
  fdnew = accept(FdListen, (struct sockaddr *)&from, &lenaddr);
  if  ( fdnew < 0 )
  {
    return  0;
  }

  if  (fdnew > FD_MAX)
  {
    AdmPrintSync(fdnew, "Adm does not support fd > %d\r\n", FD_MAX);
    close(fdnew);
    RTL_TRDBG(0, "Adm does not support fd > %d\r\n", FD_MAX);
    return  0;
  }
  if  (TbCli[fdnew] != NULL)
  {
    close(fdnew);
    RTL_TRDBG(0, "Amd fd=%d not free !!!\r\n", fdnew);
    return  0;
  }

  cl  = CliNew(fdnew);
  if  (!cl)
  {
    close(fdnew);
    return  0;
  }
  TbCli[fdnew]  = cl;

  AdmKeepAlive(fdnew);

  SetCB(fdnew);

  AdmPrint(cl, "Welcome to AZAP tester CLI\n");
  if  (strlen(GetTelnetKey()))
  {
    AdmPrint(cl, "key:");
  }
  else
  {
    AdmPrint(cl, ">>");
  }

  return  0;
}

static  void  AdmTcpBind()
{
  int flag;

  struct addrinfo hints;
  struct addrinfo *result, *rp;


  char  *strport  = GetTelnetPort();

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
  hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
  hints.ai_flags = AI_PASSIVE;    /* For wildcard IP address */

  if (getaddrinfo(NULL, strport, &hints, &result))
  {
    RTL_TRDBG(0, "adm/tcp cannot getaddrinfo port=%s\n",
        strport);
    return;
  }
  for (rp = result; rp != NULL; rp = rp->ai_next)
  {
    FdListen = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
    if  (FdListen < 0)
    {
      continue;
    }
    flag  = 1;
    if  ( setsockopt(FdListen, SOL_SOCKET, SO_REUSEADDR,
        (char *)&flag, sizeof(flag)) < 0)
    {
      close (FdListen) ;
      continue;
    }
    if  (bind(FdListen, rp->ai_addr, rp->ai_addrlen) == 0)
    {
      RTL_TRDBG(0, "ok bind adm/tcp\n");
      break;
    }
  }

  if (rp == NULL)
  {
    FdListen  = -1;
    RTL_TRDBG(0, "cannot bind adm/tcp '%s'+%s\n", "*", strport);
    return;
  }

  if (listen(FdListen, 2) < 0)
  {
    close(FdListen);
    FdListen  = -1;
    RTL_TRDBG(0, "cannot listen adm/tcp '%s'+%s\n", "*", strport);
    return;
  }

  rtl_pollAdd(MainTbPoll, FdListen, CB_AdmTcpListen, NULL, NULL, NULL);
}


/*!
 *
 * @brief initialize admin command line interface
 *  - bind listener socket on port $s_telnetport
 * @return void
 *
 */
void  AdmTcpInit()
{
  AdmTcpBind();
}

/*!
 *
 * @brief clock for connections, must be called by main loop each second
 *  - disconnect clients which do not read messages
 *  - disconnect clients which do not send messages
 * @param now current time in second (time(2))
 * @return void
 *
 */
void  AdmClockSc(time_t now)
{
  t_cli *wh;
  int i = 0;
  time_t  diff;
  time_t  delta;
  time_t  timer = 600;

  delta = timer - (timer / 10);
  for (i = 0 ; i < FD_MAX ; i++)
  {
    wh  = TbCli[i];
    if  (!wh)
    {
      continue;
    }

    // remote client does not read its messages
    diff  = ABS(now - wh->cl_lasttcpwrite);
    if  (diff >= 60 && AdmTcpMsgCount(wh) > 0)
    {
      RTL_TRDBG(1, "waiting msg timer for [%03d]\n", i);
      CliFree(wh);
      continue;
    }

    // remote client does not send messages
    diff  = ABS(now - wh->cl_lasttcpread);
    if  (diff >= timer)
    {
      RTL_TRDBG(1, "Inactivity timer for [%03d]\n", i);
      CliFree(wh);
      continue;
    }
    if  ( diff >= delta )
    {
      AdmPrint(wh,
          "you'll be disconnected in %d sec!!!\n",
          timer - diff);
    }
  }
}

