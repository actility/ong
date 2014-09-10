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

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>   /* File control definitions */
#include <errno.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <sys/ioctl.h> /* POSIX terminal control definitions */
#include <poll.h>

// FFS : Use endian.h
#define bswap16(x)  ( ((x & 0xff00) >> 8) | ((x & 0x00ff) << 8) )

#include <rtlbase.h>
#include <rtllist.h>
#include <rtlimsg.h>

#include "manufacturer.h"
#include "wmbus.h"
#include "processor.h"

extern char *rootactPath;

static int fdDongle = -1;
static int nextAttempt = 0; // start immediately
static int lastNextAttempt = 1;
extern void *MainTbPoll;
extern void *MainIQ;
static t_imsg *dongleTimeout = NULL;

typedef struct _IncommingDataContext
{
  unsigned char buffer[WMBUS_RX_BUFFER_SIZE];
  unsigned int len;
} IncommingDataContext_t;
static IncommingDataContext_t ctx =
{
  .len = 0
};



typedef enum _DongleState
{
  DONGLE_NOT_CONNECTED,
  DONGLE_CONFIGURING_FORMAT,
  DONGLE_CONFIGURING_ID,
  DONGLE_CONFIGURING_MODE,
  DONGLE_READY
} DongleState_t;
DongleState_t dongleState = DONGLE_NOT_CONNECTED;


void wmbusDongleError(void);
int wmbusDongleStop(void);
int wmbusDongleStart(void);
void wmbusDongleClockSc(time_t now);


/*
 *  wMBus commands
 */
unsigned char cmdRequestID[] = {0xff, 0x0b, 0x00, 0xf4};
unsigned char cmdDisableTransparentFormat[] = {0xff, 0x09, 0x03, 0x05, 0x01, 0x01, 0xf0};
unsigned char cmdPresetMode[] = {0xff, 0x09, 0x03, 0x46, 0x01, 0x00 /* value */, 0x00 /* chk */};

/*
 *  XOR on all byte (checksum include) must be 0x00
 */
static
int
verify_checksum(unsigned char *buffer, int size)
{
  int i;
  unsigned char cs = 0;

  for (i = 0; i < size; i++)
  {
    cs ^= buffer[i];
  }

  return cs;
}


/*
 *  Patch the var cmdPresetMode to the right wireless mode
 */
int
buildPresetCommand (char *sMode)
{
  int mode;

  if (!strcmp("S1", sMode))
  {
    mode = 0x01;
    SetVar("w_rfch", "11");
  }
  else if (!strcmp("S1-m", sMode))
  {
    mode = 0x02;
    SetVar("w_rfch", "11");
  }
  else if (!strcmp("S2", sMode))
  {
    mode = 0x03;
    SetVar("w_rfch", "11");
  }
  else if (!strcmp("T1-meter", sMode))
  {
    mode = 0x05;
    SetVar("w_rfch", "12");
  }
  else if (!strcmp("T1-other", sMode))
  {
    mode = 0x06;
    SetVar("w_rfch", "11");
  }
  else if (!strcmp("T2-meter", sMode))
  {
    mode = 0x07;
    SetVar("w_rfch", "12");
  }
  else if (!strcmp("T2-other", sMode))
  {
    mode = 0x08;
    SetVar("w_rfch", "11");
  }
  else if (!strcmp("R2-meter", sMode))
  {
    mode = 0x0A;
    SetVar("w_rfch", "6");
  }
  else if (!strcmp("R2-other", sMode))
  {
    mode = 0x0B;
    SetVar("w_rfch", "6");
  }
  else
  {
    RTL_TRDBG(TRACE_ERROR, "Unknow mode '%s'\n", sMode);
    return -1;
  }

  //SetVar("w_mode", sMode);
  SetVar("w_mid", "AMB");

  RTL_TRDBG(TRACE_IMPL, "Select Preset : %d\n", mode);
  cmdPresetMode[5] = mode;
  cmdPresetMode[6] = verify_checksum(cmdPresetMode, 6);

  return 0;
}

/*!
 *
 * @brief Call back function for rtlPoll to treat the detected events.
 * @param tb the table of all polled file descriptors
 * @param fd the file descriptor on which events were detected
 * @param r1 user's data placeholder #1
 * @param r2 user's data placeholder #2
 * @param revents the current detected events
 * @return -1 in case of error detection, 0 otherwise
 *
 */
static
int
wmbusDongleRequestPollCb(void *tb, int fd, void *r1, void *r2, int revents)
{
  char /*var_key[256],*/ var_val[256];
  int i, ret, dongleSn;

  if (revents & POLLIN)
  {

    ret = read (fdDongle, ctx.buffer + ctx.len, WMBUS_RX_BUFFER_SIZE - ctx.len);
    if (ret < 0)
    {
      if (ret == -EAGAIN)
      {
        return 0;
      }

      RTL_TRDBG(TRACE_ERROR, "Can't read fd=%d, error is %s\n", fdDongle, strerror(errno));
      return 0;
    }

    ctx.len += ret;

    /* Each frame start by 0xFF */
    do
    {
      if (ctx.buffer[0] == 0xFF)
      {
        break;
      }

      RTL_TRDBG(TRACE_INFO, "wMBus framing error, ctx.len=%d\n", ctx.len);
      //DUMP_MEMORY(ctx.buffer, ctx.len);

      for (i = 1; i < ctx.len; i++)
      {
        if (ctx.buffer[i] == 0xFF)
        {
          ctx.len -= i;
          memcpy(ctx.buffer, ctx.buffer + i, ctx.len);
          break;
        }
      }

      // No 0xFF found, drop all data
      if (i == ctx.len)
      {
        ctx.len = 0;
      }
    }
    while (ctx.len > 0);

    /* Wait a full header */
    if (ctx.len > 3)
    {
      int frameSize = 3 /* <0xFF> <cmd> <len> */ + ctx.buffer[2] /* payload size */ + 1 /* CS XOR */;
      if (ctx.len >= frameSize)
      {
        // Trame Recu

        if (verify_checksum(ctx.buffer, frameSize) == 0)
        {

          switch (ctx.buffer[1])
          {
            case 0x03:
              if (dongleState != DONGLE_READY)
              {
                RTL_TRDBG(TRACE_IMPL, "Data receive before the end of confiuration (ignored).\n");
                break;
              }
              wmbusProcessor(&(ctx.buffer[3]), ctx.buffer[2]);
              break;

              // Hand check for Disable Transparent Format,
              // Request the serial ID of the dongle
            case 0x89:
              if (ctx.buffer[3] != 0x00)
              {
                RTL_TRDBG(TRACE_ERROR, "Command failed !\n");
                return -1;
              }

              if (dongleState == DONGLE_CONFIGURING_FORMAT)
              {
                RTL_TRDBG(TRACE_IMPL, "Transparent format disabled\n");
                dongleState = DONGLE_CONFIGURING_MODE;
                ret = buildPresetCommand(GetVar("w_mode"));
                if (ret < 0)
                {
                  RTL_TRDBG(TRACE_ERROR, "Can't build preset command for mode '%s'.\n",
                      GetVar("w_mode"));
                  return -1;
                }
                ret = write (fdDongle, cmdPresetMode, sizeof(cmdPresetMode));
                if (ret != sizeof(cmdPresetMode))
                {
                  RTL_TRDBG(TRACE_ERROR, "Can't change dongle mode.\n");
                  return -1;
                }
              }
              else if (dongleState == DONGLE_CONFIGURING_MODE)
              {
                RTL_TRDBG(TRACE_IMPL, "Mode OK\n");
                dongleState = DONGLE_CONFIGURING_ID;
                ret = write (fdDongle, cmdRequestID, sizeof(cmdRequestID));
                if (ret != sizeof(cmdRequestID))
                {
                  RTL_TRDBG(TRACE_ERROR, "Can't ask dongle serial ID !\n");
                  return -1;
                }
              }
              break;

            case 0x8B:
              RTL_TRDBG(TRACE_IMPL, "Serial number received - wMBus dongle ready\n");
              //DUMP_MEMORY(ctx.buffer, frameSize);
              memcpy(&dongleSn, &(ctx.buffer[3]), sizeof(int));
              snprintf(var_val, 256, "%X", dongleSn);
              SetVar("w_netid", var_val);
              rtl_imsgRemove(MainIQ, dongleTimeout);
              //rtl_imsgAdd(
              //  MainIQ,
              //  rtl_imsgAlloc(IM_DRIVER, IM_DONGLE_READY, NULL, 0)
              //);
              dongleState = DONGLE_READY;
              break;

            default:
              wmbusDongleError();
              return -1;
          }

          // Shift data
          if (ctx.len > frameSize)
          {
            ctx.len -= frameSize;
            memcpy(ctx.buffer, ctx.buffer + frameSize, ctx.len);
          }
          else
          {
            ctx.len = 0;
          }
        }
        else
        {
          RTL_TRDBG(TRACE_IMPL, "Checksum KO\n");

          // Checksum KO
          ctx.len -= 1;
          memcpy(ctx.buffer, ctx.buffer + 1, ctx.len);
        }
      }
    }

    /* End of POLLIN */
  }

  return 0;
}

static
int
wmbusDongleEventPollCallback(void *tb, int fd, void *r1, void *r2, int revents)
{
  int res = POLLIN;   /* we always want to read */

  // FFS, need to write ?

  return res;
}

int
wmbusDongleReady(void)
{
  return (dongleState == DONGLE_READY);
}

int
wmbusDongleStop(void)
{
  if (fdDongle != -1)
  {
    RTL_TRDBG(TRACE_IMPL, "Close UART, fd=%d\n", fdDongle);
    rtl_pollRmv(MainTbPoll, fdDongle);
    close(fdDongle);
    fdDongle = -1;
  }
  dongleState = DONGLE_NOT_CONNECTED;

  return 0;
}

void wmbusDongleError(void)
{
  RTL_TRDBG(TRACE_IMPL, "Error on wMBus dongle\n");
  wmbusDongleStop();
}



int
wmbusDongleInit(const char *device)
{
  int ret;
  struct termios conf;

  // Start the timeout
  rtl_imsgAddDelayed(
    MainIQ,
    dongleTimeout = rtl_imsgAlloc(IM_DRIVER, IM_DONGLE_CONFIGURATION_TIMEOUT, NULL, 0),
    10000  /* ms */
  );
  ctx.len = 0;

  // Open port
  fdDongle = open (device, O_RDWR | O_NOCTTY);
  if (fdDongle < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "Can't open \"%s\"\n", device);

    return -1;
  }

  tcgetattr(fdDongle, &conf);

  /* Set speed */
  ret = cfsetospeed (&conf, B9600);
  if (ret != 0)
  {
    RTL_TRDBG(TRACE_ERROR, "Error on cfsetospeed !\n");
    return -1;
  }

  ret = cfsetispeed (&conf, B9600);
  if (ret != 0)
  {
    RTL_TRDBG(TRACE_ERROR, "Error on cfsetispeed !\n");
    return -1;
  }

  conf.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);

  /* Set data length */
  conf.c_cflag &= ~CSIZE;
  conf.c_cflag |= CS8;

  /* Set parity */
  conf.c_cflag &= ~PARENB;

  /* Set bit stop length */
  conf.c_cflag &= ~CSTOPB;

  /* Config cts-dts */
  conf.c_cflag &= ~CRTSCTS;

  /* config Xon-Xoff */
  conf.c_iflag &= ~(IXON | IXOFF | IXANY);

  /* Canonique read */
  conf.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  conf.c_oflag &= ~OPOST;
  conf.c_cc[VMIN] = 255;
  conf.c_cc[VTIME] = 1;

  /* Enable reception */
  conf.c_cflag |= (CREAD | CLOCAL);

  /* Clean uart buffer */
  ret = tcflush (fdDongle, TCIOFLUSH);
  if (ret != 0)
  {
    RTL_TRDBG(TRACE_ERROR, "Error on tcflush !\n");
    return -1;
  }

  /* Apply */
  ret = tcsetattr (fdDongle, TCSANOW, &conf);
  if (ret != 0)
  {
    RTL_TRDBG(TRACE_ERROR, "Can't apply uart configuration !\n");
    return -1;
  }

  // Disable Transparent mode for data reception
  dongleState = DONGLE_CONFIGURING_FORMAT;
  char cmdDisableTransparentFormat[] = {0xff, 0x09, 0x03, 0x05, 0x01, 0x01, 0xf0};
  ret = write (fdDongle, cmdDisableTransparentFormat, sizeof(cmdDisableTransparentFormat));
  if (ret != sizeof(cmdDisableTransparentFormat))
  {
    RTL_TRDBG(TRACE_ERROR, "Can't disable transparent mode for data reception !\n");
    return -1;
  }

  return 0;
}


int
wmbusDongleStart(void)
{
  int ret;

  RTL_TRDBG(TRACE_ERROR, "wmbusDongleStart: starting with device '%s'\n", 
    GetVar("w_device_tty"));

  ret = wmbusDongleInit(GetVar("w_device_tty"));
  if (ret < 0)
  {
    RTL_TRDBG(TRACE_ERROR, "Can't initialize device '%s'\n", GetVar("w_device_tty"));
    return -1;
  }

  RTL_TRDBG(TRACE_API, "Uart found, add fd=%d into poll\n", fdDongle);
  rtl_pollAdd(MainTbPoll, fdDongle, wmbusDongleRequestPollCb, wmbusDongleEventPollCallback, NULL, NULL);

  return 0;
}

void
wmbusDongleWrite(char *str, int len)
{
  /*
      int i;
      unsigned chk = 0;

      char prefix[3];
      wmBusHeader_t header;

      prefix[0] = 0xFF;
      prefix[1] = 0x00;
      prefix[2] = 3 + sizeof(wmBusHeader_t) + len + 1;
      write (fdDongle, prefix, 3);

      header.c = 0x53;        // SND-UD
      header.man = 0x103B;
      header.sn[0] = 0x01;
      header.sn[1] = 0x23;
      header.sn[2] = 0x45;
      header.sn[3] = 0x67;
      header.version = 0x02;
      header.type = 0x00;
      write (fdDongle, &header, sizeof(wmBusHeader_t));

      write (fdDongle, str, len);

      chk  = prefix[0] ^ prefix[1] ^ prefix[2];

      for (i=0; i<sizeof(wmBusHeader_t); i++)
          chk ^= *(((char *)&header) + i);

      for (i=0; i<len; i++)
          chk ^= str[i];

      write (fdDongle, &chk, 1);
      */
}

static 
void
dongleStartSucceeds()
{
  nextAttempt = 1;
  lastNextAttempt = nextAttempt;
}

static 
void
dongleStartFails()
{
  nextAttempt = lastNextAttempt * 2;
  if (nextAttempt > 64) nextAttempt = 64; // max time to wait for next attempt
  lastNextAttempt = nextAttempt;
}

static 
void
dongleDetectionFails()
{
  nextAttempt = 64; // max time to wait for next attempt
  lastNextAttempt = nextAttempt;
}


void
wmbusDongleClockSc(time_t now)
{
  (void)now;
  char szCmd[128];
  char szRead[128];
  char *pPtr;
  FILE *prgm = NULL;
  size_t lg;

  if (dongleState == DONGLE_NOT_CONNECTED)
  {
    if (nextAttempt > 1)
    {
      nextAttempt--;
      return;
    }
    RTL_TRDBG(TRACE_API, "Trying to reconnect to the dongle\n");
    // first, try to detect the dongle
    sprintf(szCmd, "%s/%s %s %s 2>&1", rootactPath, GetVar("w_tty_mapper"), 
      GetVar("w_tty_prefix"), GetVar("w_dongle_product")); 
    // this is a blocking operation, but without dongle there is nothing to do else...
    prgm = popen(szCmd, "r");
    if (! prgm)
    {
      RTL_TRDBG(TRACE_ERROR, "Fail to run \"%s\" (errno:%d) (err:%s)\n", szCmd, 
        errno, strerror(errno));
      dongleDetectionFails();
      return;
    }
    memset(szRead, 0, sizeof(char) * sizeof(szRead));
    lg = fread(szRead, sizeof(char), sizeof(szRead) - 1, prgm);
    if ((pPtr = strchr(szRead, '\r')) || 
        (pPtr = strchr(szRead, '\n')) || 
        (pPtr = strchr(szRead, EOF)))
    {
      *pPtr = 0;
      if (0 == strncmp(szRead, "/dev/", strlen("/dev/")))
      {
        SetVar("w_device_tty", szRead);
        if (0 != wmbusDongleStart())
        {
          dongleStartFails();
        }
        else
        {
          // success
          dongleStartSucceeds();
        }
      }
      else
      {
        dongleStartFails();
        RTL_TRDBG(TRACE_ERROR, "dongle not found (lg:%d) (output:%s)\n", lg, szRead);
      }
    }
    else
    {
      dongleDetectionFails();
      RTL_TRDBG(TRACE_ERROR, "Fail to interpret cmd output (lg:%d) (output:%s)\n", lg, 
        szRead);
    }
    pclose(prgm);
  }
}


