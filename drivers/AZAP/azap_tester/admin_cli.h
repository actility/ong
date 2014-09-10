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
//** File : ./azap_tester/admin_cli.h
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


#ifndef __ADMIN_CLI__H_
#define __ADMIN_CLI__H_


#define CLI_MAX_FD  32

#define CLI_NFOUND  0
#define CLI_PROMPT  1
#define CLI_DISC  2
#define CLI_KEY   3
#define CLI_EMPTY 4
#define CLI_MORE  5

#define     ABS(x)          ((x) > 0 ? (x) : -(x))

// a buffer waiting for "poll write event" on a tcp/socket for CLI
typedef struct  s_lout
{
  struct list_head list;  // the chain list of pending data
  char  *o_buf;   // a buffer
  int o_len;    // lenght of o_buf
} t_lout;


// a CLI connection on admin port
typedef struct
{
  int cl_fd;    // associated socket
  int cl_pos;   // position in cl_cmd
  char  cl_cmd[1000]; // the user input accumulated until '\n' | '\r'
  time_t  cl_lasttcpread; // time of last remote read
  time_t  cl_lasttcpwrite;// time of last remote write
  int cl_nbkey; // number of retry for password
  int cl_okkey; // password ok
  t_lout  cl_lout;  // list of pending data
  char  cl_sensor[256]; // sensor address to sniff ('*' == all)
} t_cli;

int AdmTcpMsgCount(t_cli *cl);
void  AdmPrintSync(int fd, char *fmt, ...);
void  AdmPrint(t_cli *cl, char *fmt, ...);
void  AdmPrintTable(t_cli *cl, int nb, int tab[], char *fmt, char pstart, char pend, char psep);
void  AdmPrintDefTable(t_cli *cl, int nb, int tab[], char *fmt);
void  AdmPrintTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt, char pstart, char pend, char psep);
void  AdmPrintDefTableUS(t_cli *cl, int nb, unsigned short tab[], char *fmt);

char  *AdmAdvance(char *pt);
char  *AdmAccept(char *pt);
char  *AdmAcceptKeep(char *pt, char *dst);

int CB_AdmTcpRequest(void *tb, int fd, void *r1, void *r2, int revents);
int CB_AdmTcpEvent(void *tb, int fd, void *r1, void *r2, int revents);
int CB_AdmTcpListen(void *tb, int fd, void *r1, void *r2, int revents);
void  AdmTcpInit();
void  AdmClockSc(time_t now);


#endif

