
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

#ifndef _CMNSTRUCT_H_
#define _CMNSTRUCT_H_

#ifndef CMN_IEEE_SIZE
#define CMN_IEEE_SIZE 32
#endif

// common sensor/node/device ... members
// must be included in the specific structure sensor
typedef	struct
{
	// pointer to the specif structure encapsulating this one
	void		*cmn_self;	// == &TbSensor[cmn_num]

	// dia infos
	int		cmn_num;	// indice of sensor in the sensor table
	int		cmn_serial;	// serial for the sensor (DoSerial())
	char		cmn_ieee[CMN_IEEE_SIZE];	// $d_ieee

	// dia counters
	unsigned char	cmn_dia_ipuok;	// network declared
	unsigned char	cmn_dia_netok;	// network declared
	unsigned char	cmn_dia_devok;	// device declared
	unsigned char	cmn_dia_appok;	// application.1 declared
	unsigned char	cmn_dia_datok;	// data.1 declared
	unsigned char	cmn_dia_metok;	// method.1 declared
	unsigned char	cmn_dia_accok;	// access declared
	unsigned int	cmn_dia_repok;	// reports declared
	unsigned int	cmn_dia_reperr;	// reports error
}	t_cmn_sensor, t_cmn_context;

// a buffer waiting for "poll write event" on a tcp/socket for CLI
typedef	struct	s_lout
{
	struct list_head list;	// the chain list of pending data
	char	*o_buf;		// a buffer
	int	o_len;		// lenght of o_buf
}	t_lout;

// a CLI connection on admin port
typedef	struct
{
	int	cl_fd;		// associated socket
	int	cl_pos;		// position in cl_cmd
	char	cl_cmd[1000];	// the user input accumulated until '\n' | '\r'
	time_t	cl_lasttcpread;	// time of last remote read
	time_t	cl_lasttcpwrite;// time of last remote write
	int	cl_nbkey;	// number of retry for password
	int	cl_okkey;	// password ok
	t_lout	cl_lout;	// list of pending data
	char	cl_sensor[256];	// sensor address to sniff ('*' == all)
}	t_cli;


// a DIA request
typedef	struct
{
	void	*pr_dia;	// libdIa handle (can be NULL for tests)
	char	*pr_scode;	// response code
	void	*pr_ctt;	// content
	int	pr_len;		// content lenght
	char	*pr_cttType;	// content type
	void	*pr_optHeader;	// not used
	int	pr_tid;		// transaction ID
	int	pr_error;	// libdIa internal error
	int	pr_timeout;	// timedout by DiaClientClockSc()
	char	*pr_targetId;	// targetId for this request
}	t_dia_rspparam;

// a DIA request
typedef	struct	s_dia_req
{
	char	rq_type;	// server / client ; 0 == client side
	int	rq_flags;
	int	rq_waitRsp;	// free,init,currently waiting for a response
	int	rq_waitMax;	// max time waiting for a response
	int	rq_retry;
	void	*rq_cli;	// possible associated CLI for tests
	time_t	rq_initAt;	// request init at 
	time_t	rq_sendAt;	// request sent at 
	char	*rq_targetId;	// targetId for this request
	int	rq_plcDup;	// duplicate request to PLC/IEC
	char	*rq_buf;	// the buffer waiting to be sent
	int	rq_sz;		// size of rq_buf
	char	*rq_cttType;	// content type
	int	rq_tid;		// transaction ID returned by libdIa
	char	*rq_name;	// request name (the function name in dia???.c)
	int	rq_dev;		// sensor num associated to this request
	int	rq_serial;	// sensor serial associated to this request
	int	rq_app;		// appli num associated to this request
	int	rq_access;	// access num associated to this request
	int	rq_cluster;	// cluster num associated to this request
	int	rq_attribut;	// attribut num associated to this request
	int	rq_member;	// member num associated to this request
	t_cmn_sensor *(*rq_cbserial)(int dev,int serial);

	// the function to call when a response is received or timedout
	void	(*rq_cb)(struct s_dia_req *req,t_dia_rspparam *par,...);

	char	*rq_scode;	// response code
	t_dia_rspparam *rq_par;
}	t_dia_req;

/*ML-20121001-retargeting-response+*/
typedef enum
{
  DIA_RETRIEVE = 0,
  DIA_CREATE,
  DIA_UPDATE,
  DIA_DELETE,
} DIA_REQ_TYPE;
// a DIA incoming request
typedef struct  s_dia_in_req
{
  int rq_waitRsp; // currently waiting for a response
  time_t  rq_sendAt;  // request sent at
  int rq_tid;
  char *peer;
  void *dia;
  char * targetID;
  DIA_REQ_TYPE reqType;
} t_dia_in_req;
/*ML-20121001-retargeting-response-*/

// a variable
typedef	struct
{
	char	*v_name;	// variable name
	char	*v_default;	// default value
	char	*(*v_fct)();	// driven by a function (w_time/GetTime())
	char	*v_value;	// current value (strdup/free)
}	t_var;

#endif
