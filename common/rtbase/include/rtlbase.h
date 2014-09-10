
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

#ifndef	RTL_BASE_H
#define	RTL_BASE_H

#include	<time.h>
#include	<sys/time.h>


#define	RTL_STRERRNO	strerror(errno)

/*
*/

#define	RTL_TRDBG(lev,...) \
{\
	extern int TraceDebug;\
	extern int TraceLevel;\
	if (TraceLevel >= (lev))\
	{\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
	}\
}



#ifndef	RTL_BASE_TYPE
#define	RTL_BASE_TYPE
#define	int32	int
#define	uint32	unsigned int
#endif

#ifndef	RTL_LIST_HEAD_STRUCT
#define	RTL_LIST_HEAD_STRUCT
struct list_head {
	struct list_head *next, *prev;
};
#endif

int	rtl_init();
char	*rtl_version();

void	*rtl_openDir(char *path);
char	*rtl_readDir(void *dir);
char	*rtl_readAbsDir(void *dir);
void	rtl_closeDir(void *dir);
int	rtl_mkdirp(char *path);
char	*rtl_basename(char *fullName);
char	*rtl_suffixname(char *fullName);
char	*rtl_dirname(char *fullName,char *pathName);
void	rtl_checkPathName(char *path);
void	rtl_catPathNames(char *left,char *right);
void	rtl_absToRelPath(char *path,char *pathRef,char *pathRet);

void	*rtl_pollInit();
int	rtl_pollAdd(void *tbl,int fd,
		int (*fevt)(void *,int,void *,void *,int),
		int (*freq)(void *,int,void *,void *,int),
		void *ref,void *ref2);
int	rtl_pollSetEvt(void *tbl, int fd,int mode);
int	rtl_pollSetEvt2(void *tbl, int pos,int mode);
int	rtl_pollGetEvt(void *tbl, int fd);
int	rtl_pollGetMode(void *tbl, int fd);
int	rtl_pollRmv(void *tbl, int fd);
int	rtl_poll(void *tbl, int timeout);


unsigned long rtl_vsize(int pid);
unsigned int rtl_cpuloadavg();
int	rtl_pseudothreadid();
void	rtl_enablecore(int flg);
void	rtl_docore();
char	*rtl_aaaammjj(time_t t,char *dst);
char	*rtl_hhmmss(time_t t,char *dst);
char	*rtl_aaaammjjhhmmss(time_t t,char *dst);
char	*rtl_hhmmssms(char *dst);

int	rtl_tracemutex();
void	rtl_traceunlock(void);
void	rtl_tracelevel(int level);
void	rtl_tracesizemax(int sz);
int	rtl_tracefilesize(FILE *f);
FILE	*rtl_tracecurrentfile();
void	rtl_tracerotate(char *logFiles);
void	rtl_trace(int t,char *fmt,...);
void	rtl_tracenotime(int t,char *fmt,...);
void	rtl_tracedbg(int t,char *file,int line,char *fmt,...);


void	rtl_timespec(struct timespec *tv);
time_t	rtl_time(time_t *part);			// == time(2)
time_t	rtl_tmms();
void	rtl_timebase(struct timespec *tv);
void	rtl_timebaseVal(time_t *sec,time_t *ns);
#if	0
time_t	rtl_timems(time_t *base,time_t *etms,time_t *dtms);
time_t	rtl_timecs(time_t *base,time_t *etcs,time_t *dtcs);
#endif

void	rtl_timespecmono(struct timespec *tv);
time_t	rtl_timemono(time_t *part);
time_t	rtl_tmmsmono();
void	rtl_timebasemono(struct timespec *tv);
void	rtl_timebasemonoVal(time_t *sec,time_t *ns);
int	rtl_timespeccmp(struct timespec *tv1, struct timespec *tv2);
void	rtl_timespecaddms(struct timespec *tv,int ms);

#define	RTL_TIMESPECMP(t1,t2)\
	((t1)->tv_sec > (t2)->tv_sec ? +1000000000 :\
	((t1)->tv_sec < (t2)->tv_sec ? -1000000000 :\
			(t1)->tv_nsec - (t2)->tv_nsec))


char *rtl_binToStr(unsigned char *bin,int lg,char dst[],int max);
unsigned char	*rtl_strToBin(char *str,unsigned char bin[],int *lg);

char	*rtl_evalPath(char *in,char *out,int lgmax,char *(*fctvar)(char *v));
char	*rtl_evalPath2(char *in,char *out,int lgmax,char *(*fctvar)(void *user,char *v),void *user);


long rtl_base64_encode (char *to, const char *from, unsigned int len);
long rtl_base64_decode (char *to, const char *from, unsigned int len);



void rtl_gettimeofday_to_iso8601date(struct timeval *tv, struct timezone *tz, char *buf);
void rtl_getCurrentIso8601date(char *buf);

/* Parse given INI-style file. May have [section]s, name=value pairs
   (whitespace stripped), and comments starting with ';' (semicolon). Section
   is "" if name=value pair parsed before any section heading. name:value
   pairs are also supported as a concession to Python's ConfigParser.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error (doesn't
   stop on first error), -1 on file open error, or -2 on memory allocation
   error (only when INI_USE_STACK is zero).
*/
int rtl_iniParse(const char* filename,
              int (*handler)(void* user, const char* section,
                             const char* name, const char* value),
              void* user);

/* Same as ini_parse(), but takes a FILE* instead of filename. This doesn't
   close the file when it's finished -- the caller must do that. */
int rtl_iniParseFile(FILE* file,
                   int (*handler)(void* user, const char* section,
                                  const char* name, const char* value),
                   void* user);

char *rtl_iniStr(void *ht,char *sec,int index,char *var,char *def);
int rtl_iniInt(void *ht,char *sec,int index,char *var,int def);
int rtl_iniLoadCB(void *user,const char *section,const char *name,const char *value);

#endif
