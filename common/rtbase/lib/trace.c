
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
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <errno.h>

#include "rtlbase.h"

#define	ABS(x)		((x) > 0 ? (x) : -(x))


static	int	TraceLevel	= 0;
static	int	TraceRotate	= 0;
static	char	TraceFiles[512];

static	int	SZMAX	= 10 * 1000 * 1000;	// 10M

static	pthread_mutex_t	CS;
static	char	CSDone	= 0;

static	void	InitCS(void)
{

	if	( CSDone )
		return;
	if (pthread_mutex_init(&CS,NULL))
	{
		fprintf(stderr,"'%s' %d error while initializing mutex '%s' !!!\n",
			__FILE__,__LINE__,RTL_STRERRNO);
		return;
	}
	CSDone	= 1;
}

static	void	*GetCS(void)
{
	if (!CSDone)
		return	NULL;
	return	&CS;
}

static	void	EnterCS(void)
{
	if (!CSDone)
		return;
	if (pthread_mutex_lock(&CS))
	{
		fprintf(stderr,"'%s' %d error while locking mutex '%s' !!!\n",
			__FILE__,__LINE__,RTL_STRERRNO);
	}


}

static	void	LeaveCS(void)
{
	if (!CSDone)
		return;
	pthread_mutex_unlock(&CS);
}

void	rtl_traceunlock(void)
{
	LeaveCS();
}

int	rtl_tracemutex()
{
	InitCS();
	return	1;
}

void	rtl_tracelevel(int level)
{
	TraceLevel	= level;
}

void	rtl_tracesizemax(int sz)
{
	SZMAX	= sz;
}

static	int	SzMax(FILE *f)
{
	int	szMax;
	struct stat st;

	szMax	= 1;
	if (f)
	{
		if	( fstat(fileno(f),&st) == 0 )
		{
			if	( st.st_size < SZMAX )
				szMax	= 0;
		}
	}
	return	szMax;
}

int	rtl_tracefilesize(FILE *f)
{
	return SzMax(f);
}

static	FILE	*HistoLog(char *logFiles)
{
	char	tmp[1024];

	static	int	day_open = -1;
	static	FILE* f = NULL;
	static	int	rotatecount = 1;
	time_t	gmtTime;
	struct tm locTime;
	int	day_full = 0;

#define WDAY(d)	(d?d:7)

	if (!TraceRotate || !logFiles)
		return	stderr;

	time(&gmtTime);
	locTime = *localtime(&gmtTime);

#if	0	// TODO : fichier du jour plein
#endif
	if (f && (day_open == WDAY(locTime.tm_wday)) && SzMax(f))
	{
		day_full = 1;
	}

	if (!f || day_full || (day_open != WDAY(locTime.tm_wday)))
	{
		char *ext;
		char *mode="w+";
		struct stat st;
		struct tm* flocTime;
		int append;
#if	0
		if (f)
		{
			fclose(f);
			f = NULL;
		}
#endif
		if (day_open != WDAY(locTime.tm_wday))
			rotatecount	= 1;
		day_open = WDAY(locTime.tm_wday);
		sprintf(tmp, "%s", logFiles);
		ext = strrchr(tmp, '.');
		if (ext)
			*ext = '\0';
		ext = strrchr(logFiles, '.');
		if (!ext)
			ext = ".log";
		sprintf(tmp, "%s_%02d%s", tmp, day_open, ext);

		mode = "w+";
		if (day_full)
		{
			unlink(tmp);
			rotatecount++;
		}
		append = 0;
		if (stat(tmp, &st) == 0)
		{
			flocTime = localtime(&st.st_mtime);
			if (locTime.tm_yday == flocTime->tm_yday && locTime.tm_year == flocTime->tm_year)
			{
				mode = "a+";
				append	= 1;
			}
		}
#if	0
		f = fopen(tmp, mode);
		if (!f)
		{
			char	dir[512];
			rtl_dirname(tmp, dir);
			rtl_mkdirp(dir);
			f = fopen(tmp, mode);
		}
#endif
		f = f ? freopen(tmp, mode, f) : fopen(tmp, mode);
		if (!f)
		{
			char	dir[512];
			rtl_dirname(tmp, dir);
			rtl_mkdirp(dir);
			f = f ? freopen(tmp, mode, f) : fopen(tmp, mode);
		}
		if (day_full)
		{
			ext	= "rotate";
		}
		else
		{
			if (!append)
				ext = "start";
			else
				ext = "restart";
		}
		if (f)
		{
			unlink(logFiles);
			link(tmp,logFiles);
		}
		if (f && !SzMax(f))
		{
fprintf(f,"################################################################\n");
fprintf(f,"### %s(%d) : %04d%02d%02d ", ext,rotatecount,
		locTime.tm_year+1900,locTime.tm_mon+1,locTime.tm_mday);
fprintf(f," %02d:%02d:%02d\n",
		locTime.tm_hour,locTime.tm_min,locTime.tm_sec);
fprintf(f,"### pid=%d szmax=%d\n",getpid(),SZMAX);
fprintf(f,"################################################################\n");
			fflush(f);
		}
	}
	if	( !f )
		return	stderr;
	return	f;
}

FILE *rtl_tracecurrentfile()
{
	return	HistoLog(TraceFiles);
}

void	rtl_tracerotate(char *logFiles)	/* !TS */
{
	TraceRotate	= 1;
	strcpy	(TraceFiles,logFiles);
	HistoLog(logFiles);
}

void	rtl_vtrace(int t,int withTime,char *fmt, va_list listArg) /* TS */
{
	char	tmp[64];

	EnterCS();
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		FILE	*f;

		f	= HistoLog(TraceFiles);
		if	( SzMax(f) )
		{
			LeaveCS();
			return;
		}
		if	(withTime)
			fprintf (f, "%s", rtl_hhmmssms(tmp));
			
		fprintf (f, " (%04d) ", rtl_pseudothreadid());

		vfprintf (f, fmt, listArg);
		fflush	(f) ;
	}
	LeaveCS();
}

void	rtl_tracedbg(int t,char *file,int line,char *fmt,...)	/* TS */
{
	char	tmp[64];

	EnterCS();
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;
		FILE	*f;

		f	= HistoLog(TraceFiles);
		if	( SzMax(f) )
		{
			LeaveCS();
			return;
		}
		fprintf (f, "%s", rtl_hhmmssms(tmp));
			
		fprintf (f, " (%04d) ", rtl_pseudothreadid());

		if	(file && *file)
		{
			fprintf (f, "[%s:%d] ", file, line);
		}

		va_start(listArg,fmt);
		vfprintf (f, fmt, listArg);
		va_end(listArg);
		fflush	(f) ;
	}
	LeaveCS();
}

void	rtl_trace(int t,char *fmt,...)	/* TS */
{
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;

		va_start(listArg,fmt);
		rtl_vtrace (t, 1, fmt, listArg);
		va_end(listArg);
	}
}


void	rtl_tracenotime(int t,char *fmt,...)	/* TS */
{
	if	( TraceLevel >= 0 && t <= TraceLevel )
	{
		va_list	listArg;

		va_start(listArg,fmt);
		rtl_vtrace (t, 0, fmt, listArg);
		va_end(listArg);
	}
}
