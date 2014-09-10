
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

/*
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <signal.h>

#define	SZPOLL	10

#if	0
           struct pollfd {
               int   fd;         /* file descriptor */
               short events;     /* requested events */
               short revents;    /* returned events */
           };
#endif

typedef struct pollfd POLLFD;
typedef struct
{
	int	(*fctevent)();
	int	(*fctrequest)();
	void	*ref;
	void	*ref2;
} HAND;

typedef struct polltable
{
	POLLFD	*tbPoll;
	HAND	*handPoll;
	int	szTbPoll;
	int	nbPoll;
} polltable_t;


static int rtl_pollFind(polltable_t *table, int fd);


void	*rtl_pollInit()	/* TS */
{
	polltable_t *table;
	table	= calloc(1,sizeof(polltable_t));
	return table;
}

/*
 *
 *	Ajoute une entree dans la table de polling
 *
 */

int	rtl_pollAdd(void *ptbl,int fd,			/* TS */
		int (*fctevent)(void *,int,void *,void *,int),
		int (*fctrequest)(void *,int,void *,void *,int),
		void * ref,void * ref2)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	POLLFD *poll;
	int	i;

	if(table->nbPoll==table->szTbPoll)
	{
		i=table->szTbPoll;
		if(!i)
		{
			table->szTbPoll=SZPOLL;
			table->handPoll=(HAND *)malloc(table->szTbPoll*sizeof(HAND));
			table->tbPoll=(POLLFD *)malloc(table->szTbPoll*sizeof(POLLFD));
		}
		else
		{
			table->szTbPoll+=SZPOLL;
			table->handPoll=(HAND *)realloc(table->handPoll,table->szTbPoll*sizeof(HAND));
			table->tbPoll=(POLLFD *)realloc(table->tbPoll,table->szTbPoll*sizeof(POLLFD));
		}

		if	(!table->handPoll || !table->tbPoll)
		{
			return	-1;
		}
		for	(poll=table->tbPoll+i; i<table->szTbPoll; i++,poll++)
		{
			poll->fd	= -1;
			poll->events	= 0;
			poll->revents	= 0;
		}
	}

	for	(poll=table->tbPoll,i=0; i<table->szTbPoll; i++,poll++)
	{
		if(poll->fd<0)
		{
			poll->fd	= fd;
			poll->events	= POLLIN;
			poll->revents	= 0;
			table->handPoll[i].fctevent	= fctevent;
			table->handPoll[i].fctrequest	= fctrequest;
			if (fctrequest)
				poll->events	= 0;
			table->handPoll[i].ref	= ref;
			table->handPoll[i].ref2	= ref2;
			table->nbPoll++;
			return i;
		}
	}
	return -1;
}

int	rtl_pollChg(void *ptbl, int fd,int (*fctevent)(void *,int,void *,void *,int), void *ref, void *ref2)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	i	= rtl_pollFind(table,fd);
	if	(i < 0)
		return	-1;

	table->handPoll[i].fctevent	= fctevent;
	table->handPoll[i].ref	= ref;
	table->handPoll[i].ref2	= ref2;
	return 0;
}

/*
 *
 *	recherche d'un file-descriptor ds la table
 *
 */

static int rtl_pollFind(polltable_t *table, int fd)	/* TS */
{
	POLLFD *poll;
	int	i;

	for(i=0,poll=table->tbPoll;i<table->szTbPoll;i++,poll++)
	{
		if(poll->fd==fd)
		{
			return i;
		}
	}
	return -1;
}

/*
 *
 *	Change le mode de polling
 *
 */

int	rtl_pollSetEvt(void *ptbl, int fd,int mode)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if((i=rtl_pollFind(table,fd))>=0)
	{
		table->tbPoll[i].events	= mode & (POLLIN|POLLOUT|POLLPRI);
		return 0;
	}
	return -1;
}

int	rtl_pollSetEvt2(void *ptbl, int pos,int mode)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	if	(pos < 0)
		return	-1;
	table->tbPoll[pos].events	= mode & (POLLIN|POLLOUT|POLLPRI);
	return 0;
}


/*
 *
 *	Retourne le mode de polling
 *
 */

int	rtl_pollGetEvt(void *ptbl, int fd)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if((i=rtl_pollFind(table,fd))>=0)
	{
		return table->tbPoll[i].revents;
	}
	return 0;
}

int	rtl_pollGetMode(void *ptbl, int fd)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if((i=rtl_pollFind(table,fd))>=0)
	{
		return table->tbPoll[i].events;
	}
	return 0;
}


/*
 *
 *	Retire un file-descriptor de la liste
 *
 */

int	rtl_pollRmv(void *ptbl, int fd)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	i;

	if	(fd < 0)
		return	-1;

	i	= rtl_pollFind(table,fd);
	if(i<0)
	{
		return	-1;
	}

	table->tbPoll[i].fd		= -1;
	table->tbPoll[i].events		= 0;
	table->handPoll[i].fctevent	= NULL;
	table->handPoll[i].fctrequest	= NULL;
	table->nbPoll--;
	return	0;
}

/*
 *
 *	Scrutation des file-descriptors et appel des procedures
 *
 */

static int rtl_pollRequest(polltable_t *table)	/* TS */
{
	int	i;
	int	nb=0;
	int	ret;

	for (i=0; i<table->szTbPoll; i++)
	{
//		if (table->handPoll[i].fctrequest && (table->tbPoll[i].fd >= 0))
		if ((table->tbPoll[i].fd >= 0) && 
					table->handPoll[i].fctrequest)
		{
			ret =	table->handPoll[i].fctrequest(table,
					table->tbPoll[i].fd,
					table->handPoll[i].ref,
					table->handPoll[i].ref2,
					table->tbPoll[i].events);
			if (ret>=0)
				table->tbPoll[i].events=ret;
			nb++;
		}
	}
	return nb;
}

static int rtl_pollScan(polltable_t *table)	/* TS */
{
	int	i;
	int	nb=0;

	for	(i=0; i<table->szTbPoll; i++)
	{
//		if	(!table->handPoll[i].fctevent && (table->tbPoll[i].fd >=0))
		if	((table->tbPoll[i].fd >= 0) && 
						!table->handPoll[i].fctevent)
		{
			table->tbPoll[i].fd	= -1;
			table->tbPoll[i].revents 	= 0;
		}
		if	(table->tbPoll[i].revents & (POLLIN|POLLOUT|POLLPRI))
		{
#if	0
			if	((table->tbPoll[i].fd<0) || !table->handPoll[i].fctevent)
			{
			}
			else
			{
				table->handPoll[i].fctevent(table,table->tbPoll[i].fd,table->handPoll[i].ref,table->handPoll[i].ref2,table->tbPoll[i].revents);
			}
#endif
			if	((table->tbPoll[i].fd>=0) && table->handPoll[i].fctevent)
				table->handPoll[i].fctevent(table,table->tbPoll[i].fd,table->handPoll[i].ref,table->handPoll[i].ref2,table->tbPoll[i].revents);


			table->tbPoll[i].revents=0;
			nb++;
		}
	}
	return nb;
}


/*
 *
 *	Attente de donnees sur un des fd
 *
 */

int	rtl_poll(void *ptbl, int timeout)	/* TS */
{
	polltable_t	*table	= (polltable_t *)ptbl;
	int	rc;

	if(table->nbPoll<0)
		return -1 ;

	rtl_pollRequest(table);
	rc	= poll(table->tbPoll,table->szTbPoll,timeout);

	if	(rc == 0)
	{
		return	0;
	}

	if	(rc < 0)
	{
		if	(errno == EINVAL)
		{
			return -EINVAL;
		}
		return -1;
	}
	return rtl_pollScan(table);
}
