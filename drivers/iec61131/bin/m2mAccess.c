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

#define _GNU_SOURCE

#include <stdio.h>
#include <stddef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <math.h>

#include <mxml.h>
#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>
#include <dIa.h>

#include "iec_debug.h"
#include "iec_config.h"
#include "plc.h"
#include "literals.h"
#include "timeoper.h"
#include "iec_std_lib.h"
#include "vasprintf.h"
#include "tools.h"
#include "operating.h"
#include "xpath.h"
#include "bucket.h"
#include "cli.h"
#include "mxml-reduce.h"


#ifndef __NO_TRACE__
#define M2M_TRDBG(lev,...) \
{\
	extern int TraceDebug;\
	extern int TraceLevel;\
	if (GLOB.level >= (lev))\
	{\
		int _save = TraceLevel;\
		TraceLevel = GLOB.level;\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
		TraceLevel = _save;\
	}\
	else if (TraceLevel >= (lev))\
	{\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
	}\
}
#else
#define M2M_TRDBG(lev,...)
#endif


typedef struct {
	char *targetID;
	char *template;
} outMapping_t;


#define	IEC_LOADMAP_TIME 10
char *M2M_CONFIG_FILE = "m2mConfig.xml";

char *M2M_applications = "/m2m/applications/";
char *M2M_subscriptions = "/subscriptions";

/**
 * @brief IEC_GLOCK_TIME minimum time in seconds between two 'load'
 */
int IEC_GLOCK_TIME = 30;

/**
 * @brief External functions
 */
extern void setVariable (iec_variable_t *var, void *value);
void check_IEC_APPLICATION(char *IEC_appID, int tid, char *peer);
int create_IEC_APPLICATION(char *IEC_appID, int tid, char *peer);
int delete_IEC_APPLICATION(char *IEC_appID);
int commit_IEC_APPLICATION(char *appId);
void rollback_IEC_APPLICATION(char *appId);
int create_IEC_ENGINE();
void sendSubscriptions(BOOL force);

/**
 * @brief External variables
 */
extern iec_global_t GLOB;
extern void *MainTbPoll;
extern int TraceLevel;
extern char *typeString[];

/**
 * @brief Global variables
 */
static dia_context_t   *g_dia = NULL;

#if 0 // DEPRECATED
static BOOL g_driverSpeaking = FALSE;
#endif // DEPRECATED
char *iec_to_obix_types[] = {
	"bool", "int", "int", "int", "int", "int", "int", "int",
	"int", "int", "int", "int", "int", "real", "real",
	"time", "date", "dt", "tod", "str", "union", NULL };


/**
 * @brief Shared variables between dIa request and response
 * g_lock_retrieve is the protection lock to avoid multiple simultaneous application retrieves
 */
BOOL g_lock_retrieve = FALSE;

char *param_app_id = NULL;
int tid_params = -1;
int tid_conf = -1;
int tid_mapping = -1;
time_t g_lock_time = 0;
iec_buff_t content_configuration = {NULL,0,NULL};
iec_buff_t content_mapping = {NULL,0,NULL};
BOOL rollbacking = FALSE;

#define DIA_FIFO

#if !defined(DIA_FIFO)
int tid_engine = -1;
int tid_rights = -1;
int tid_discover = -1;
#endif


char *grepMinusV(char *in, char *notExpected) {
	char *res = malloc(strlen(in)+1);
	*res = 0;

	char *line = in;
	char *end = in+strlen(in);

	while	(line < end) {
		char *eol = strchr(line, '\n');
		if	(!eol) {
			strcat (res, line);
			break;
		}
		*eol = 0;
		if	(!strstr(line, notExpected)) {
			strcat (res, line);
			strcat (res, "\n");
		}
		line = eol+1;
	}
	return res;
}


void *local_dictionnary = NULL;
void IEC_New_dictionnary() {
	if	(local_dictionnary)
		rtl_htblDestroy (local_dictionnary);
	local_dictionnary = rtl_htblCreateSpec(32, NULL, HTBL_KEY_STRING|HTBL_FREE_DATA);
}

void IEC_Add_dictionnary (char *param, char *value) {
	if	(!local_dictionnary)
		IEC_New_dictionnary();
	char *old = rtl_htblGet (local_dictionnary, param);
	if	(old) {
		rtl_htblRemove (local_dictionnary, param);
	}
	rtl_htblInsert (local_dictionnary, param, strdup(value));
}

char *IEC_Get_dictionnary(char *param) {
	char *ret = IEC_Config (param);
	if (!ret)
		ret = rtl_htblGet (local_dictionnary, param);
	if (!ret) ret="";
	return ret;
}

/**
	Version with math
 */
char *IEC_Get_dictionnary_math(char *param) {
	static char tmp[30];
	char *ret;
	char *work = strdup(param);

	char *pt = strchr(work, '*');
	if	(!pt)
		pt	= strchr(work, '+');
	if	(!pt)
		pt	= strchr(work, '-');

	if	(pt) {
		char ope = *pt;
		*pt++ = 0;
		char *operand1 = IEC_Get_dictionnary(work);
		if	(!operand1 || !*operand1)
			return "";
		char *operand2 = IEC_Get_dictionnary(pt);
		if	(!operand2 || !*operand2)
			return "";
		int lvalue = atoi(operand1);
		int rvalue = atoi(operand2);
		switch (ope) {
		case '*' :
			sprintf (tmp, "%d", lvalue * rvalue);
			break;
		case '+' :
			sprintf (tmp, "%d", lvalue + rvalue);
			break;
		case '-' :
			sprintf (tmp, "%d", lvalue - rvalue);
			break;
		}
		ret = tmp;
	}
	else {
		ret = IEC_Get_dictionnary(param);
	}
	free (work);
	return ret;
}

#define GSC_DELETE 0
#define GSC_CREATE 1
#define GSC_RETRIEVE 2
#define GSC_UPDATE 3



//-------------------------- Queue of configurations to be loaded -----------------------

typedef struct {
	struct list_head head;
	char *str;
} list_elem_t;

struct list_head _queue;
int _queue_inited = 0;

list_elem_t *_enqueue(char *str) {
	if	(!_queue_inited) {
		INIT_LIST_HEAD (&_queue);
		_queue_inited = 1;
	}

	list_elem_t *elt = Calloc(sizeof(list_elem_t), 1);
	if	(!elt)
		return NULL;
	elt->str = str;
	list_add_tail (&elt->head, &_queue);
	return elt;
}

char *_dequeue() {
	if	(!_queue_inited) {
		INIT_LIST_HEAD (&_queue);
		_queue_inited = 1;
	}

	struct list_head *elem;
	list_for_each (elem, &_queue) {
		list_elem_t *elt	= list_entry(elem, list_elem_t, head);
		list_del (&elt->head);
		char *str = elt->str;
		free (elt);
		return	str;
	}
	return NULL;
}


#if 0
double getXmlDurationDouble(char *str) {
	if	(str[0] == 'P' && str[1] == 'T') {
		str += 2;
		if	(str[strlen(str)-1] == 'S')
			str[strlen(str)-1] = 0;
	}
	return	atof(str);
}
//--------------- Bucket

void User_BucketInit(iec_user_t *user) {
	user->m_bucket.rate = 0.2;
	user->m_bucket.size = 50;
	if	(user->m_dictionnary) {
		char *res;
		res = User_GetDictionnary(user, "outTokenBucketRate");
		if	(res) {
			user->m_bucket.rate = getXmlDurationDouble(res);
		}
		res = User_GetDictionnary(user, "outTokenBucketSize");
		if	(res)
			user->m_bucket.size = atoi(res);
	}
	//	The bucket is full at startup : max tokens allowed
	user->m_bucket.count = user->m_bucket.size;
	//user->m_bucket.count = 0;
	gettimeofday (&user->m_bucket.last, NULL);
	RTL_TRDBG (TRACE_DETAIL, "User_BucketInit\n");
}

void User_BucketTime(iec_user_t *user) {
	double sec;
	struct timeval tv, last;

	if	(!user)
		return;

	gettimeofday (&tv, NULL);
	last = tv;
	SUB_TIMEVAL (tv, user->m_bucket.last);
	user->m_bucket.last = last;

	sec	= tv.tv_sec + (double)tv.tv_usec/1000000.0;
	user->m_bucket.count	+= sec / user->m_bucket.rate;

	if	(user->m_bucket.count > user->m_bucket.size)
		user->m_bucket.count = user->m_bucket.size;

	//RTL_TRDBG (TRACE_DETAIL, "User_BucketTime sec=%f count=%f\n", sec, user->m_bucket.count);
}

void AllUsers_BucketTime() {
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user = list_entry(elem, iec_user_t, head);
		User_BucketTime (user);
	}
}

BOOL User_BucketGet(iec_user_t *user) {
	RTL_TRDBG (TRACE_DETAIL, "User_BucketGet count=%f\n", user->m_bucket.count);
	if	(user->m_bucket.count < 1) {
		User_GSC_log(user, "OUT_TOKEN_BUCKET_EXCEEEDED", "Output change exceeded token bucket regulator limit. The Output has been discarded.", "", "");
		Board_Put(&GLOB.board_errors, "Output change exceeded token bucket regulator limit. The Output has been discarded.", "", "");
		return	FALSE;
	}
	user->m_bucket.count -= 1.0;
	return	TRUE;
}

#endif

//---------------

char *IEC_evalPath(char *buf, int n) {
	int out_size = n*2;
	char *interm	= Calloc(out_size, 1);

	// Replace ${} with the value in disctionnary
	rtl_evalPath (buf, interm, out_size, IEC_Get_dictionnary_math);

	// Remove all Obix val="" (deprecated)
	char *out = grepMinusV(interm, "val=\"\"");
	free (interm);

	//char *out2 = grepMinusV(out, "></");		???
	char *out2 = grepMinusV(out, "<m2m:maxByteSize></m2m:maxByteSize>");
	free (out);

	return out2;
}

char *User_GetRequestingEntity(iec_user_t *user) {
	if	(!user || !user->m_dictionnary)
		return IEC_Config("w_reqEntity");
	char *res = rtl_htblGet (user->m_dictionnary, "iecRequestingEntity");
	if	(!res)
		return IEC_Config("w_reqEntity");
	return	res;
}

int GSC_send(iec_user_t *user, int type, char *targetID, char *file) {
	int tid;
	char *out = NULL;

	if	(file) {
		struct stat st;
		int fd = open(file, O_RDONLY);
		if	(fd < 0) {
			return -1;
		}
		if	(fstat(fd, &st) < 0) {
			close (fd);
			return -1;
		}
		char *buf	= Calloc(st.st_size+1, 1);
		int n = read (fd, buf, st.st_size);
		buf[n] = 0;
		close (fd);

		out	= IEC_evalPath(buf, n);

		RTL_TRDBG (TRACE_DETAIL, "Document [%s]\n", out);
		free (buf);
	}

	switch (type) {
	case GSC_DELETE :
		//diaDeleteRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid);
		diaDeleteRequest (g_dia, User_GetRequestingEntity(user), targetID, NULL, NULL, &tid);
		break;
	case GSC_CREATE :
		//diaCreateRequest (g_dia, IEC_Config("w_reqEntity"), targetID, out, out ? strlen(out):0, "application/xml", NULL, NULL, &tid);
		diaCreateRequest (g_dia, User_GetRequestingEntity(user), targetID, out, out ? strlen(out):0, "application/xml", NULL, NULL, &tid);
		break;
	case GSC_RETRIEVE :
		//diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid);
		diaRetrieveRequest (g_dia, User_GetRequestingEntity(user), targetID, NULL, NULL, &tid);
		break;
	case GSC_UPDATE :
		diaUpdateRequest (g_dia, User_GetRequestingEntity(user), targetID, out, out ? strlen(out):0, "application/xml", NULL, NULL, &tid);
		break;
	}
	if	(out) free(out);
	return tid;
}

struct dIa_action {
	int	type;
	int	tid;
	char	*targetID;
	char	*file;
	char *scode;
	char *resourceURI;
	char *ctt;
	int cttlen;
	char *cttType;
	void	*udata;
	BOOL	more;
	void (*cb)(struct dIa_action *da);
	struct list_head mylist;
};

void Free_da(struct dIa_action *da) {
	list_del (&da->mylist);
	free (da->targetID);
	free (da->file);
	if	(da->udata)
		free (da->udata);
	free (da);
}

int GSC_next() {
	struct list_head *elem;
	struct dIa_action *da = NULL;

	if	(list_empty(&GLOB.dIa_list))
		return -1;

	list_for_each (elem, &GLOB.dIa_list) {
		da	= list_entry(elem, struct dIa_action, mylist);
		break;
	}
	if	(!da) return -1;

	da->tid = GSC_send(NULL, da->type, da->targetID, da->file);
	if	(da->tid < 0) {
		Free_da (da);
		return	GSC_next();
	}
	return 0;
}

struct dIa_action *GSC_send2(int type, char *targetID, char *file, void (*cb)(struct dIa_action *da)) {
	struct dIa_action *da = Calloc(sizeof(struct dIa_action), 1);
	BOOL empty = list_empty(&GLOB.dIa_list);

	da->tid = -999;
	da->type = type;
	da->targetID = Strdup(targetID);
	da->file = Strdup(file);
	da->cb = cb;
	list_add_tail (&da->mylist, &GLOB.dIa_list);

	if	(empty)
		GSC_next();
	return da;
}

int GSC_response(int tid, char *scode, char *resourceURI, void *ctt, int len, char* cttType) {
	struct dIa_action *da = NULL;

	if	(list_empty(&GLOB.dIa_list))
		return -1;

	da	= list_entry(GLOB.dIa_list.next, struct dIa_action, mylist);
	if	(!da || (tid != da->tid)) {
		M2M_TRDBG (TRACE_DETAIL, "GSC_response tid %d != %d expected\n", tid, da->tid);
		return -1;
	}

	if	(da->cb) {
		M2M_TRDBG (TRACE_DETAIL, "GSC_response call cb function for tid %d\n", tid);
		da->scode = scode;
		da->resourceURI = resourceURI;
		da->ctt = ctt;
		da->cttlen = len;
		da->cttType = cttType;
		(*da->cb)(da);
	}
	else if (strcmp(scode, "STATUS_OK") && strcmp(scode, "STATUS_CREATED") && strcmp(scode, "STATUS_DELETED")) {
		//	remove requests of the same group
		while	(da && (da->more == TRUE)) {
			M2M_TRDBG (TRACE_DETAIL, "****** Remove %d\n", da->tid);
			Free_da (da);
			da = NULL;
			if	(!list_empty(&GLOB.dIa_list))
				da	= list_entry(GLOB.dIa_list.next, struct dIa_action, mylist);
		}
		if	(da && da->cb) {
			M2M_TRDBG (TRACE_DETAIL, "GSC_response call cb function for tid %d\n", tid);
			da->scode = scode;
			da->resourceURI = resourceURI;
			da->ctt = ctt;
			da->cttlen = len;
			da->cttType = cttType;
			(*da->cb)(da);
		}
	}

	if	(da)
		Free_da (da);
	return GSC_next ();
}




/**
 * @brief Sends a log to the GSC
 * @param subsystem Can be either IEC_61131_VM or an application name
 * @param error
 * @param message
 * @param ref
 * @param detail
 * @return void

Example of log message :
<log>
	<error>IN_XPATH_DOES_NOT_MATH</error>
	<message>The IEC-Engine was unable to match the XPath expression. The input has been ignored.</message>
	<ref>/m2m/applications/APP_020000ffff002b0a.1/containers/0x0052.0x0000.0.m2m/ contentInstances</ref>
</log>
 */
void GSC_log(char *error, char *message, char *ref, char *detail, ...) {
	char *bigbuf = NULL;
	char *targetID = NULL;
	int tid;

	if	(!g_dia)
		return;
	iec_asprintf (&bigbuf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<log>\n");
	iec_asprintf (&bigbuf, "  <error>%s</error>\n", error);
	iec_asprintf (&bigbuf, "  <message>%s</message>\n", message);
	iec_asprintf (&bigbuf, "  <ref>%s</ref>\n", ref);
	if	(detail) {
		va_list listArg;
		va_start(listArg,detail);
		iec_asprintf  (&bigbuf, "  <detail>");
		iec_vasprintf (&bigbuf, detail, listArg);
		iec_asprintf  (&bigbuf, "</detail>\n");
		va_end(listArg);
	}
	iec_asprintf (&bigbuf, "</log>\n");

	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers/appLogs/contentInstances",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
/*
	//	For testing log failure
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers/appLogs/BLABLA",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
*/

	diaCreateRequest (g_dia, IEC_Config("w_reqEntity"), targetID, bigbuf, strlen(bigbuf), "application/xml", NULL, NULL, &tid);

	free (targetID);
	free (bigbuf);
}

void User_GSC_log(iec_user_t *user, char *error, char *message, char *ref, char *detail, ...) {
	char *bigbuf = NULL;
	char *targetID = NULL;
	int tid;

	if	(!g_dia)
		return;
	iec_asprintf (&bigbuf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<log>\n");
	iec_asprintf (&bigbuf, "  <error>%s</error>\n", error);
	iec_asprintf (&bigbuf, "  <message>%s</message>\n", message);
	iec_asprintf (&bigbuf, "  <ref>%s</ref>\n", ref);
	if	(detail) {
		va_list listArg;
		va_start(listArg,detail);
		iec_asprintf  (&bigbuf, "  <detail>");
		iec_vasprintf (&bigbuf, detail, listArg);
		iec_asprintf  (&bigbuf, "</detail>\n");
		va_end(listArg);
	}
	iec_asprintf (&bigbuf, "</log>\n");

	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s/containers/appLogs/contentInstances",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), user->name);

	diaCreateRequest (g_dia, User_GetRequestingEntity(user), targetID, bigbuf, strlen(bigbuf), "application/xml", NULL, NULL, &tid);

	free (targetID);
	free (bigbuf);
}


/**
 * @brief Converts an obix variable type to an IEC variable type
 * @param A string containing the obix type
 * @return An integer representing the IEC type (see param_u.h)
 */
int obix_to_iec_type(char *obix_type) {
	int	i;
	for	(i=0; iec_to_obix_types[i]; i++)
		if	(!strcasecmp(obix_type, iec_to_obix_types[i]))
			return i;
	return -1;
}

/**
 * @brief Remove network section from URI
 * @param An Uri 
 * @return The path section.
 */
char *pathFromUri(char *uri) {
	char *pt = uri;
	if	(!strncasecmp(pt, "coap://", 7))
		pt	+= 7;
	else if	(!strncasecmp(pt, "coaps://", 8))
		pt	+= 8;
	pt	= strchr(pt, '/');
	return	pt;
}

/**
 * @brief Formats an ISO time
 * @param A time_t (Unix time)
 * @param The output buffer
 * @return void
 */
void Unixtime_to_iso8601(time_t t, char *buf) {
	struct tm *tm = localtime(&t);
	int h, m;
	h = tm->tm_gmtoff/3600;
	m = (tm->tm_gmtoff - h*3600) / 60;
	char sign = (h<0) ? '-':'+';
	sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.000%c%02d:%02d",
		tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec,
		sign, abs(h), abs(m));
}

void tv_to_iso8601(struct timeval *tv, char *buf) {
	struct tm *tm = localtime(&tv->tv_sec);
	int h, m;
	h = tm->tm_gmtoff/3600;
	m = (tm->tm_gmtoff - h*3600) / 60;
	char sign = (h<0) ? '-':'+';
	sprintf (buf, "%04d-%02d-%02dT%02d:%02d:%02d.%03ld%c%02d:%02d",
		tm->tm_year + 1900, tm->tm_mon + 1,
		tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, tv->tv_usec/1000,
		sign, abs(h), abs(m));
}

void GetCurrentIso8601date(char *buf) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	tv_to_iso8601(&tv, buf);
}

/**
 * @brief Converts an ISO date like 2012-12-14T15:00:17.717Z
			    2013-07-22T02:17:29.377-07:00
 *        into an IEC DT
 * @param iso An ISO date string
 * @param dt The resulting IEC DT
 * @return void
 */
void iso8601_to_IECdate(char *iso, IEC_DT *dt, BOOL convertUTC) {
	int y, m, d;
	double h, min, s;

	if	(!iso || strlen(iso) < 17)
		return;

	RTL_TRDBG (TRACE_DETAIL, "iso8601_to_IECdate [%s]\n", iso);

	y = (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0') * 10 + (iso[3]-'0');
	m = (iso[5]-'0')*10 + (iso[6]-'0');
	d = (iso[8]-'0')*10 + (iso[9]-'0');
	h = (iso[11]-'0')*10 + (iso[12]-'0');
	min = (iso[14]-'0')*10 + (iso[15]-'0');

	char type = iso[23];
	iso[23] = 0;
	s	= atof(&iso[17]);

	*dt      = __dt_to_timespec(s, min, h, d, m, y);

	if	(convertUTC) {
		if	(type == '+' || type == '-') {
			int offset;
			offset	= ((iso[24]-'0')*10 + (iso[25]-'0')) * 3600;
			offset	+= ((iso[27]-'0')*10 + (iso[28]-'0')) * 60;
			if	(type == '+')
				dt->tv_sec	-= offset;
			else
				dt->tv_sec	+= offset;
		}
	}
}

#if 0
/**
 * @brief Retrieve the text contained in a xml token, eg <tok>content</tok>
 * @param node The xml node
 * @return The contained text
 */
char *getContainedText(mxml_node_t *node) {
	char *content = NULL;
	node	= mxmlWalkNext (node, NULL, MXML_DESCEND);
	if	(node && node->type == MXML_OPAQUE) {
		content = (char *)mxmlGetOpaque (node);
		if	(content && !strcmp(content, "\n")) {
			node	= mxmlWalkNext (node, NULL, MXML_DESCEND);
			content = (char *)mxmlGetCDATA(node);
		}
	}
	return content;
}
#endif

/*
 A notify message is a doubly encoded base64 xml message

At level 1 :

<?xml version="1.0" encoding="UTF-8"?>
<m2m:notify xmlns:m2m="http://uri.etsi.org/m2m" xmlns:xmime="http://www.w3.org/2005/05/xmlmime">
<m2m:statusCode>STATUS_OK</m2m:statusCode>
<m2m:subscriptionReference>http://{gateway}/m2m/applications/APP/containers/CNT/contentInstances/subscriptions/AYFGAJHBJCXDCV</m2m:subscriptionReference>
<m2m:representation xmime:contentType="application/xml; charset=utf-8">PG0ybTpjb250ZW50SW5zdGFuY2VDb2xsZWN0aW9uIHhtbG5zOm0ybT0iaHR0cDovL3VyaS5ldHNpLm9yZy9tMm0iIHhtbG5zOnhtaW1lPSJodHRwOi8vd3d3LnczLm9yZy8yMDA1LzA1L3htbG1pbWUiPg0KCTxtMm06Y29udGVudEluc3RhbmNlIG0ybTppZD0iMjAxMi0xMi0xNFQxNTowMDoxNy43MTdaIiBocmVmPSIyMDEyLTEyLTE0VDE1JTNBMDAlM0ExNy43MTdaLyI+DQoJCTxtMm06Y3JlYXRpb25UaW1lPjIwMTItMTItMTRUMTU6MDA6MTcuNzE3WjwvbTJtOmNyZWF0aW9uVGltZT4NCiAgICAgICAgPG0ybTpsYXN0TW9kaWZpZWRUaW1lPjIwMTItMTItMTRUMTU6MDA6MTcuNzE3WjwvbTJtOmxhc3RNb2RpZmllZFRpbWU+DQogICAgICAgIDxtMm06Y29udGVudFR5cGVzPg0KCQkJPG0ybTpjb250ZW50VHlwZT5hcHBsaWNhdGlvbi94bWw7IGNoYXJzZXQ9dXRmLTg8L20ybTpjb250ZW50VHlwZT4NCiAgICAgICAgPC9tMm06Y29udGVudFR5cGVzPg0KICAgICAgICA8bTJtOmNvbnRlbnRTaXplPjQ3ODwvbTJtOmNvbnRlbnRTaXplPg0KICAgICAgICA8bTJtOmNvbnRlbnQgeG1pbWU6Y29udGVudFR5cGU9ImFwcGxpY2F0aW9uL3htbDsgY2hhcnNldD11dGYtOCI+UEQ5NGJXd2dkbVZ5YzJsdmJqMGlNUzR3SWlCbGJtTnZaR2x1WnowaVZWUkdMVGdpUHo0TkNqeHZZbWw0T205aWFpQm9jbVZtUFNJdlUxbFRWRVZOSWlCNGJXeHVjenB2WW1sNFBTSm9kSFJ3T2k4dmIySnBlQzV2Y21jdmJuTXZjMk5vWlcxaEx6RXVNU0lnZUcxc2JuTTZlbUk5SW1oMGRIQTZMeTkxY21rdVpYUnphUzV2Y21jdmJUSnRMM3BwWjJKbFpTOXZZbWw0SWo0TkNnMEtJQ0E4YzNSeUlHNWhiV1U5SW5Sb2FXNW5iR1YwU1VRaUlIWmhiRDBpVTFsVFZFVk5JaUF2UGcwS0lDQThiR2x6ZENCdVlXMWxQU0oyWlhKemFXOXVjeUlnYUhKbFpqMGlMMjB5YlM5aGNIQnNhV05oZEdsdmJuTXZVMWxUVkVWTkwzSmxkR0Z5WjJWMGFXNW5NUzkyWlhKemFXOXVjeUlnTHo0TkNpQWdQRzl3SUc1aGJXVTlJbTl3Wlc1VGMyaFVkVzV1Wld3aUlHaHlaV1k5SWk5dE1tMHZZWEJ3YkdsallYUnBiMjV6TDFOWlUxUkZUUzl5WlhSaGNtZGxkR2x1WnpFdmIzQmxibE56YUZSMWJtNWxiQ0lnTHo0TkNpQWdQRzl3SUc1aGJXVTlJbU5zYjNObFUzTm9WSFZ1Ym1Wc0lpQm9jbVZtUFNJdmJUSnRMMkZ3Y0d4cFkyRjBhVzl1Y3k5VFdWTlVSVTB2Y21WMFlYSm5aWFJwYm1jeEwyTnNiM05sVTNOb1ZIVnVibVZzSWlBdlBnMEtQQzl2WW1sNE9tOWlhajROQ2c9PTwvbTJtOmNvbnRlbnQ+DQogICAgPC9tMm06Y29udGVudEluc3RhbmNlPg0KPC9tMm06Y29udGVudEluc3RhbmNlQ29sbGVjdGlvbj4=</m2m:representation>
</m2m:notify>

At level 2 (m2m:representation part decoded) :
<m2m:contentInstanceCollection xmlns:m2m="http://uri.etsi.org/m2m" xmlns:xmime="http://www.w3.org/2005/05/xmlmime">
<m2m:contentInstance m2m:id="2012-12-14T15:00:17.717Z" href="2012-12-14T15%3A00%3A17.717Z/">
	<m2m:creationTime>2012-12-14T15:00:17.717Z</m2m:creationTime>
<m2m:lastModifiedTime>2012-12-14T15:00:17.717Z</m2m:lastModifiedTime>
<m2m:contentTypes>
		<m2m:contentType>application/xml; charset=utf-8</m2m:contentType>
</m2m:contentTypes>
<m2m:contentSize>478</m2m:contentSize>
<m2m:content xmime:contentType="application/xml; charset=utf-8">PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiPz4NCjxvYml4Om9iaiBocmVmPSIvU1lTVEVNIiB4bWxuczpvYml4PSJodHRwOi8vb2JpeC5vcmcvbnMvc2NoZW1hLzEuMSIgeG1sbnM6emI9Imh0dHA6Ly91cmkuZXRzaS5vcmcvbTJtL3ppZ2JlZS9vYml4Ij4NCg0KICA8c3RyIG5hbWU9InRoaW5nbGV0SUQiIHZhbD0iU1lTVEVNIiAvPg0KICA8bGlzdCBuYW1lPSJ2ZXJzaW9ucyIgaHJlZj0iL20ybS9hcHBsaWNhdGlvbnMvU1lTVEVNL3JldGFyZ2V0aW5nMS92ZXJzaW9ucyIgLz4NCiAgPG9wIG5hbWU9Im9wZW5Tc2hUdW5uZWwiIGhyZWY9Ii9tMm0vYXBwbGljYXRpb25zL1NZU1RFTS9yZXRhcmdldGluZzEvb3BlblNzaFR1bm5lbCIgLz4NCiAgPG9wIG5hbWU9ImNsb3NlU3NoVHVubmVsIiBocmVmPSIvbTJtL2FwcGxpY2F0aW9ucy9TWVNURU0vcmV0YXJnZXRpbmcxL2Nsb3NlU3NoVHVubmVsIiAvPg0KPC9vYml4Om9iaj4NCg==</m2m:content>
</m2m:contentInstance>
</m2m:contentInstanceCollection>


At level 3 (Content of 'm2m:content') :

<?xml version="1.0" encoding="UTF-8"?>
<obix:obj href="/SYSTEM" xmlns:obix="http://obix.org/ns/schema/1.1" xmlns:zb="http://uri.etsi.org/m2m/zigbee/obix">
<str name="thingletID" val="SYSTEM" />
<list name="versions" href="/m2m/applications/SYSTEM/retargeting1/versions" />
<op name="openSshTunnel" href="/m2m/applications/SYSTEM/retargeting1/openSshTunnel" />
<op name="closeSshTunnel" href="/m2m/applications/SYSTEM/retargeting1/closeSshTunnel" />
</obix:obj>

*/

#if 0 // DEPRECATED
void User_SetObixVar(iec_user_t *user, char *path, char *obix_type, char *obix_val, IEC_DT lastModifiedTime) {
	if	(!user->m_inputTable)
		return;
	char *iecVariable = rtl_htblGet (user->m_inputTable, path);
	if	(!iecVariable)
		return;

	iec_variable_t *var = rtl_htblGet(user->m_accessVars, iecVariable);
	if	(!var)
		return;

	//	Type match
	if	(obix_type) {
		int iec_type = obix_to_iec_type(obix_type);
		if	((iec_type < 0) || (iec_type != var->type)) {
			RTL_TRDBG (TRACE_ERROR, "obix type '%s' and iec variable %s of type '%s' don't match\n",
				obix_type, iecVariable, typeString[(int)var->type]);
			return;
		}
	}
	setVariable (var, obix_val);
	var->lastModifiedTime	= lastModifiedTime;
	CLI_valueHasChanged(var);

	RTL_TRDBG (TRACE_ERROR, "SUCCESS M2M variable %s=%s\n", iecVariable, obix_val);
}
#endif // DEPRECATED

mxml_node_t *mxmlLoadBuffer(void *buf, int len, char **temp, BOOL display) {
	*temp = Calloc(len+1, 1);
	if	(!*temp)
		return NULL;
	memcpy (*temp, buf, len);
	(*temp)[len] = 0;
	if	(display)
		RTL_TRDBG (TRACE_DETAIL, "mxmlLoadBuffer [%s]\n", *temp);
	mxml_node_t *doc = mxmlLoadString(NULL, *temp, MXML_OPAQUE_CALLBACK);
	if	(!doc) {
		free (*temp);
		*temp = NULL;
	}
	return doc;
}

char *base64_decode_alloc(char *from, unsigned int len) {
	int target_size = (double)len*3.0/4.0 + 10;
	char *out = Calloc(target_size, 1);
	long sz = rtl_base64_decode (out, from, len);
	if	(sz < 0) {
		free (out);
		return NULL;
	}
	return out;
}



#if 0 // DEPRECATED
//--------------------- Xpath ---------------------

void User_SetXpathVar(iec_user_t *user, char *iecVariable, char *obix_type, char *obix_val, IEC_DT lastModifiedTime) {
	RTL_TRDBG (TRACE_DETAIL, "User_SetXpathVar type=%s val=%s iecVariable=%s\n", obix_type, obix_val, iecVariable);
	iec_variable_t *var = rtl_htblGet(user->m_accessVars, iecVariable);
	if	(!var || !obix_val)
		return;

	//	Type match
	if	(obix_type) {
		int iec_type = obix_to_iec_type(obix_type);
		if	((iec_type < 0) || (iec_type != var->type)) {
			RTL_TRDBG (TRACE_ERROR, "obix type '%s' and iec variable %s of type '%s' don't match\n",
				obix_type, iecVariable, typeString[(int)var->type]);
			User_GSC_log(user, "OBIX_TYPE_MISMATCH", "The obix type received in a notify doesn't match the IEC variable type",
				"obix type '%s' and iec variable %s of type '%s' don't match\n",
				obix_type, iecVariable, typeString[(int)var->type]);
			return;
		}
	}
	setVariable (var, obix_val);
	var->lastModifiedTime	= lastModifiedTime;
	CLI_valueHasChanged(var);

	RTL_TRDBG (TRACE_DETAIL, "SUCCESS M2M variable %s=%s\n", iecVariable, obix_val);
}

char *XpathExpression(mxml_node_t *top, char *expr) {
	char *res = NULL;
	char *attr = NULL;
	char *val = NULL;

	if	(*expr == '/')
		expr ++;
	char *pt = strchr(expr, '/');
	if	(pt)
		*pt = 0;
	else {
		pt = strstr(expr, "[@");
		if	(pt) {
			*pt = 0;
			attr = pt+2;
			val = strchr(attr, '=');
			pt = strchr(attr, ']');
			if	(!pt)
				return NULL;
			*pt = 0;
			pt = NULL;
			if (val) {
				*val++ = 0;
				if	(*val == '\'')
					val ++;
				if	(val[strlen(val)-1] == '\'')
					val[strlen(val)-1] = 0;
			}
		//printf ("attr (%s) val (%s)\n", attr, val);
		}
	}
	//printf ("expr=%s attr=%s val=%s\n", expr, expr, val);

	mxml_node_t *node = mxmlFindElement(top, top, expr, attr, val, MXML_DESCEND);
	if	(node) {
		if	(pt) {
			res = XpathExpression (node, pt+1);
		}
		else if (attr) {
			res = (char *)mxmlElementGetAttr(node, attr);
		}
		else {
			res = getContainedText(node);
			//printf ("getContainedText = (%s)\n", res);
		}
	}
	else {
		//printf ("### FAILED mxmlFindElement(%s)\n", expr);
	}
	return res;
}

void XpathAdd (iec_user_t *user, char *targetID, char *xpath_expr, char *iec_input_var, char *value) {
	RTL_TRDBG (TRACE_DETAIL, "XpathAdd targetID=%s xpath=%s iec_input_var=%s value=%s\n",
		targetID, xpath_expr, iec_input_var, value);
	iec_xpath_t *xpath = Calloc(sizeof(iec_xpath_t), 1);
	xpath->targetID	= Strdup(targetID);
	xpath->xpath_expr	= Strdup(xpath_expr);
	xpath->iec_input_var	= Strdup(iec_input_var);
	xpath->value	= Strdup(value);
	xpath->subscription_ref = NULL;
	xpath->msg = NULL;
	xpath->nextSubscription = 0;
	list_add_tail (&xpath->head, &user->xpath_list);
}

void XpathSetReference(int tid, char *ref, char *msg) {
	iec_user_t *user;
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		user	= list_entry(elem, iec_user_t, head);

		struct list_head *elem2;
		list_for_each (elem2, &user->xpath_list) {
			iec_xpath_t *elt = list_entry(elem2, iec_xpath_t, head);
			if	(elt->tid == tid) {
				RTL_TRDBG (TRACE_DETAIL, "XpathSetReference tid=%d ref=%s %s\n", tid, ref, elt->targetID);
				if	(elt->subscription_ref)
					free (elt->subscription_ref);
				if	(ref)
					elt->subscription_ref = Strdup(ref);
				else {
					elt->nextSubscription = 0;
					elt->subscription_ref = NULL;
				}
				if	(elt->msg)
					free (elt->msg);
				elt->msg	= Strdup(msg);
				return;
			}
		}
	}
	RTL_TRDBG (TRACE_DETAIL, "XpathSetReference failed tid=%d ref=%s\n", tid, ref);
}

void XpathUnsetReference(char *ref) {
	iec_user_t *user;
	struct list_head *elem;
	RTL_TRDBG (TRACE_DETAIL, "XpathUnsetReference ref=%s\n", ref);
	list_for_each (elem, &GLOB.user_list) {
		user	= list_entry(elem, iec_user_t, head);

		struct list_head *elem2;
		list_for_each (elem2, &user->xpath_list) {
			iec_xpath_t *elt = list_entry(elem2, iec_xpath_t, head);
			if	(elt->subscription_ref && !strcmp(ref, elt->subscription_ref)) {
				RTL_TRDBG (TRACE_DETAIL, "XpathUnsetReference ref=%s\n", ref);
				free (elt->subscription_ref);
				elt->subscription_ref = NULL;
				elt->nextSubscription = 0;
				return;
			}
		}
	}
}


iec_xpath_t *XpathGetWithInputVar (iec_user_t *user, char *input_var) {
	struct list_head *elem;
	//RTL_TRDBG (TRACE_DETAIL, "XpathGetWithInputVar input_var=%s\n", input_var);
	list_for_each (elem, &user->xpath_list) {
		iec_xpath_t *elt = list_entry(elem, iec_xpath_t, head);
		if	(!strcmp(input_var, elt->iec_input_var))
			return elt;
	}
	return NULL;
}

void XpathAnalyzeInput(iec_user_t *user, char *uri_str, mxml_node_t *node, IEC_DT lastModifiedTime) {
	struct list_head *elem;

	uri_str	= Strdup(uri_str);

	char *subscription_ref = strstr(uri_str, M2M_subscriptions);
	char *path = strstr(uri_str, M2M_applications);
	if	(!subscription_ref)
		goto end;
	*subscription_ref = 0;
	subscription_ref += strlen(M2M_subscriptions)+1;

	RTL_TRDBG (TRACE_DETAIL, "XpathAnalyzeInput path=%s subscription_ref=%s\n", path, subscription_ref);

	list_for_each (elem, &user->xpath_list) {
		iec_xpath_t *elt = list_entry(elem, iec_xpath_t, head);

		RTL_TRDBG (TRACE_DETAIL, "  XpathAnalyzeInput elt->subscription_ref=%s targetID=%s\n", elt->subscription_ref, elt->targetID);
		if	(!elt->subscription_ref || strcmp(subscription_ref, elt->subscription_ref)) {
			if	(strcmp(path, elt->targetID)) {
				continue;
			}
		}

		if	(!elt->xpath_expr) {
			char *obix_type = node->value.element.name;
			char *obix_val = (char *)mxmlElementGetAttr(node, "val");

			RTL_TRDBG (TRACE_DETAIL, "XpathAnalyzeInput OK case 1 : iec_input_var=%s type=%s val=%s\n",
				elt->iec_input_var, obix_type, obix_val);
			User_SetXpathVar (user, elt->iec_input_var, obix_type, obix_val, lastModifiedTime);
			goto end;
		}

		char *res = XpathExpression(node, elt->xpath_expr);
		if	(res) {
			RTL_TRDBG (TRACE_DETAIL, "XpathAnalyzeInput OK case 2 : iec_input_var=%s res=%s val=%s\n",
				elt->iec_input_var, res, elt->value);
			User_SetXpathVar (user, elt->iec_input_var, NULL, elt->value ? elt->value : res, lastModifiedTime);
			goto end;
		}
	}
end:
	free (uri_str);
}

//-----------------
#endif

/**
 * @brief Scans a notify message. A notify m2m message has three levels.
 *        The URI is stored in m2m:subscriptionReference
 * @param m2m:notify node
 * @return <0 if error, 0 else
 */
int scan_notify(mxml_node_t *top, char *appId) {
	IEC_DT	lastModifiedTime;
	iec_user_t *user;

	RTL_TRDBG (TRACE_DETAIL, "Entering scan_notify (%s)\n", appId);

	user = User_GetByUri(appId);
	if	(!user) {
		RTL_TRDBG (TRACE_ERROR, "ERROR Notify received on unknown appId %s\n", appId);
		Board_Put (&GLOB.board_errors, "ERROR Notify received on unknown appId %s", appId);
		GSC_log("NOTIFY_BAD_APPID", "Notify received on unknown appId", "", "Unknown appId %s", appId);
		return -1;
	}

	//	Level 1 decoding
	mxml_node_t *node = mxmlFindElement(top, top, "subscriptionReference", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		RTL_TRDBG (TRACE_ERROR, "No subscriptionReference in notify document\n");
		Board_Put (&GLOB.board_errors, "No subscriptionReference in notify document");
		User_GSC_log(user, "NO_SUBSCRIPTION_REFERENCE", "No subscriptionReference in notify document", "", "");
		return -1;
	}
	char *uri_str = getContainedText(node);
	RTL_TRDBG (TRACE_DETAIL, "subscriptionReference %s\n", uri_str);
	//char *path = pathFromUri(uri_str);

	//	Level 2 decoding
	node = mxmlFindElement(top, top, "representation", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		RTL_TRDBG (TRACE_ERROR, "No representation in notify document\n");
		Board_Put (&GLOB.board_errors, "No representation in notify document");
		User_GSC_log(user, "NO_REPRESENTATION", "No representation in notify document", "", "");
			return -1;
	}

	char *content = getContainedText(node);
	char *out = base64_decode_alloc(content, strlen(content));
	if	(!out) {
		RTL_TRDBG (TRACE_ERROR, "representation base64 error\n");
		Board_Put (&GLOB.board_errors, "representation base64 error");
		User_GSC_log(user, "REPRESENTATION_BASE64_ERROR", "representation base64 error", "", "");
		return -1;
	}

	RTL_TRDBG (TRACE_DETAIL, "NOTIFY MESSAGE [%s]\n", out);
	mxml_node_t *doc = mxmlLoadString(NULL, out, MXML_OPAQUE_CALLBACK);
	if	(!doc) {
		RTL_TRDBG (TRACE_ERROR, "representation xml error\n");
		Board_Put (&GLOB.board_errors, "representation xml error");
		User_GSC_log(user, "REPRESENTATION_XML_ERROR", "representation xml error", "", "");
		free (out);
		return -1;
	}

	//	Status from GSC. Eg. STATUS_EXPIRED.
	node = mxmlFindElement(doc, doc, "m2m:additionalInfo", NULL, NULL, MXML_DESCEND);
	if	(node) {
		char *subscription_ref = strstr(uri_str, M2M_subscriptions);
		subscription_ref += strlen(M2M_subscriptions)+1;
		XpathUnsetReference(subscription_ref);

		char *txt = getContainedText(node);
		RTL_TRDBG (TRACE_ERROR, "%s %s\n", txt, uri_str);
		Board_Put (&GLOB.board_errors, "GSC : %s %s", txt, uri_str);
		free (out);
		mxmlDelete (doc);
		return 0;
	}

	//	Level 3 decoding
	node = mxmlFindElement(doc, doc, "m2m:lastModifiedTime", NULL, NULL, MXML_DESCEND);
	if	(node) {
		iso8601_to_IECdate (getContainedText(node), &lastModifiedTime, FALSE);
	}

	node = mxmlFindElement(doc, doc, "m2m:content", NULL, NULL, MXML_DESCEND);
	if	(!node) {
		RTL_TRDBG (TRACE_ERROR, "No m2m:content in a Notify\n");
		Board_Put (&GLOB.board_errors, "No m2m:content in a Notify %s", uri_str);
		User_GSC_log(user, "M2M_CONTENT_ERROR", "No m2m:content in a Notify", "", uri_str);
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	content = getContainedText(node);

	char *out2 = base64_decode_alloc(content, strlen(content));
	if	(!out2) {
		RTL_TRDBG (TRACE_ERROR, "m2m:content base64 error\n");
		Board_Put (&GLOB.board_errors, "m2m:content base64 error");
		User_GSC_log(user, "M2M_CONTENT_BASE64_ERROR", "m2m:content base64 error", "", "");
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	RTL_TRDBG (TRACE_DETAIL, "OBIX MESSAGE [%s]\n", out2);
	mxml_node_t *doc2 = mxmlLoadString(NULL, out2, MXML_OPAQUE_CALLBACK);
	if	(!doc2) {
		RTL_TRDBG (TRACE_ERROR, "m2m:content xml error\n");
		Board_Put (&GLOB.board_errors, "m2m:content xml error");
		User_GSC_log(user, "M2M_CONTENT_XML_ERROR", "m2m:content xml error", "", "");
		free (out2);
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	node = mxmlFindElement(doc2, doc2, NULL, NULL, NULL, MXML_DESCEND);
	if	(!node) {
		RTL_TRDBG (TRACE_ERROR, "obix:obj failed\n");
		mxmlDelete (doc2);
		free (out2);
		mxmlDelete (doc);
		free (out);
		return -1;
	}

	char *obix_type = node->value.element.name;
	char *obix_val = (char *)mxmlElementGetAttr(node, "val");
#if 0 // DEPRECATED
	char *x_date = (char *)mxmlElementGetAttr(node, "x-date");
	if	(x_date)
		iso8601_to_IECdate (x_date, &lastModifiedTime, FALSE);
#endif // DEPRECATED

	RTL_TRDBG (TRACE_DETAIL, "SUCCESS Notify type=%s val=%s\n", obix_type, obix_val);

#if 0 // DEPRECATED
	char *path = "";
	char *pt = strstr(uri_str, M2M_applications);
	if	(pt) {
		path = pt;
		pt = strstr(pt, M2M_subscriptions);
		if	(pt)
			*pt = 0;
	}
	RTL_TRDBG (TRACE_DETAIL, "URI:%s path:%s\n", uri_str, path);

	XpathAnalyzeInput(user, path, node, lastModifiedTime);
#endif // DEPRECATED

	XpathAnalyzeInput(user, uri_str, node, lastModifiedTime);

	mxmlDelete (doc2);
	free (out2);
	mxmlDelete (doc);
	free (out);
	return 0;
}

#if 0 // DEPRECATED
/**
 * @brief Scans obix message, received directly from a driver
 * @param node 
 * @param uri
 * @return
 */
int scan_obix(mxml_node_t *node, char *uri) {
	IEC_DT	lastModifiedTime;

	RTL_TRDBG (TRACE_DETAIL, "Entering scan_obix\n");

	// Get path part from uri
	char *path = pathFromUri(uri);

	char *obix_type = node->value.element.name;
	char *obix_val = (char *)mxmlElementGetAttr(node, "val");
	char *x_date = (char *)mxmlElementGetAttr(node, "x-date");
	if	(x_date)
		iso8601_to_IECdate (x_date, &lastModifiedTime, FALSE);

	//	Set each user inputTable
	struct list_head *elem;
	list_for_each (elem, &GLOB.user_list) {
		iec_user_t *user	= list_entry(elem, iec_user_t, head);
		User_SetObixVar (user, path, obix_type, obix_val, lastModifiedTime);
	}

	return	0;
}
#endif // DEPRECATED

#ifndef __NO_TRACE__
#define M2M_TRDBG(lev,...) \
{\
	extern int TraceDebug;\
	extern int TraceLevel;\
	if (GLOB.level >= (lev))\
	{\
		int _save = TraceLevel;\
		TraceLevel = GLOB.level;\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
		TraceLevel = _save;\
	}\
	else if (TraceLevel >= (lev))\
	{\
		rtl_tracedbg((lev),TraceDebug <= 0?NULL:__FILE__,__LINE__,__VA_ARGS__);\
	}\
}
#else
#define M2M_TRDBG(lev,...)
#endif

int retrieve_IEC_APPLICATION(char *appId) {
	char *targetID;

	if	(g_lock_retrieve && (time(0) - g_lock_time < IEC_GLOCK_TIME)) {
		M2M_TRDBG (TRACE_ERROR, "Another load command already running\n");
		Board_Put (&GLOB.board_errors, "Another load command already running");
		GSC_log("LOAD_ALREADY_RUNNING", "Another load command already running", "", "");
		return -1;
	}

	if	(!appId) {
		char *data = _dequeue ();
		if	(!data)
			return -1;
		appId	= data;	// data allocated with strdup()
	}
	else
		appId = strdup(appId);

	M2M_TRDBG (TRACE_DETAIL, "### BEGIN LOADING %s\n", appId);

	g_lock_retrieve = TRUE;
	g_lock_time = time(0);

	// Free previous one
	if	(param_app_id)
		free (param_app_id);
	param_app_id = appId;

	//	Retrieve parameters
	targetID = NULL;
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s/containers/DESCRIPTOR/contentInstances/latest/content",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid_params);
	free (targetID);

	//	Retrieve configuration
	targetID = NULL;
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s/containers/configurations/contentInstances/latest/content",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid_conf);
	free (targetID);

	//	Retrieve mapping
	targetID = NULL;
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s/containers/mappings/contentInstances/latest/content",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid_mapping);
	free (targetID);

	rollbacking = FALSE;
	return 0;
}

/*
To discover all IEC configurations, one use a GET on :

http://192.168.1.116:8080/m2m/discovery?searchPrefix=/m2m/applications&searchString=ETSI.ObjectType/ETSI.GA&searchString=IEC.ObjectType/IEC_61131_INSTANCE&scope=exact

And the result will be like :

<?xml version="1.0"?>
<m2m:discovery xmlns:m2m="http://uri.etsi.org/m2m">
  <m2m:matchSize>3</m2m:matchSize>
  <m2m:discoveryURI>
    <reference>/m2m/applications/A1</reference>
    <reference>/m2m/applications/A6</reference>
    <reference>/m2m/applications/aaa</reference>
  </m2m:discoveryURI>
</m2m:discovery>
*/

void m2m_discover();

void discoverResponse(struct dIa_action *da) {
	M2M_TRDBG (TRACE_DETAIL, "discoverResponse %s [%s]\n", da->scode, da->ctt);
	if	(strcmp(da->scode, "STATUS_OK")) {
		// re-do a discover after some wait
		M2M_TRDBG (TRACE_DETAIL, "Sleeping before discover re-attempt\n");
		Board_Put (&GLOB.board_errors, "Sleeping before discover re-attempt");
		sleep (1);
		create_IEC_ENGINE();
		m2m_discover ();
		return;
	}
	char *temp;
	mxml_node_t *doc = mxmlLoadBuffer(da->ctt, da->cttlen, &temp, TRUE);
	if	(!doc) {
		M2M_TRDBG (TRACE_ERROR, "Invalid xml document in response to Discover\n");
		Board_Put (&GLOB.board_errors, "Invalid xml document in response to Discover");
		GSC_log("DISCOVER_BAD_XML", "Invalid xml document in response to Discover", "", "");
		sleep (1);
		create_IEC_ENGINE();
		m2m_discover ();
		return;
	}
	int flg = MXML_DESCEND;
	mxml_node_t *node = doc;
	while	((node = mxmlFindElement(node, doc, "reference", NULL, NULL, flg))) {
		flg = MXML_NO_DESCEND;
		char *txt = getContainedText(node);
		char *pt = strstr(txt, M2M_applications);
		if	(pt)
			txt = pt + strlen(M2M_applications);
		M2M_TRDBG (TRACE_DETAIL, "### REFERENCE %s\n", txt);
		_enqueue(strdup(txt));
	}
	mxmlDelete (doc);
	free (temp);

	rollback_IEC_APPLICATION (NULL);
}

void m2m_discover() {
	char *targetID = NULL;

	iec_asprintf (&targetID, "coap://%s:%s/m2m/discovery?searchPrefix=/m2m/applications&searchString=ETSI.ObjectType/ETSI.GA&searchString=IEC.ObjectType/IEC_61131_INSTANCE&scope=exact",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	//diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid_discover);
	GSC_send2 (GSC_RETRIEVE, targetID, NULL, discoverResponse);
	free (targetID);
}

void checkResponse(struct dIa_action *da) {
	M2M_TRDBG (TRACE_DETAIL, "Initial ENGINE check response %s\n", da->scode);
	if	(strcmp(da->scode, "STATUS_OK")) {
		create_IEC_ENGINE();
	}
	if	(strcasecmp(IEC_Config_Default("appdiscover", "TRUE"), "FALSE")) {
		m2m_discover();
	}
	else
		Board_Put (&GLOB.board_errors, "NOTICE : appdiscover option is FALSE (spvconfig.xml)");
}

void m2m_checkENGINE() {
	char *targetID = NULL;

	M2M_TRDBG (TRACE_DETAIL, "Initial ENGINE check\n");
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	GSC_send2 (GSC_RETRIEVE, targetID, NULL, checkResponse);
	free (targetID);
}



/**
 * @brief Scans obix message and set a dictionnary with the parameters
 * @param content Obix message
 * @param size Message size
 * @return 0=success
 */
int Obix2Dictionnary(void *content, int size, void **dict) {
	if	(!content || size <= 0)
		return -1;
	char *temp;
	mxml_node_t *doc = mxmlLoadBuffer(content, size, &temp, TRUE);
	if	(!doc) {
		M2M_TRDBG (TRACE_ERROR, "invalid obix document\n");
		Board_Put (&GLOB.board_errors, "Invalid obix document");
		GSC_log("OBIX_BAD_XML", "Invalid obix document", "", "");
		return -1;
	}

	if	(*dict)
		rtl_htblDestroy (*dict);
	*dict = rtl_htblCreateSpec(32, NULL, HTBL_KEY_STRING|HTBL_FREE_DATA);

	mxml_node_t *obj = mxmlFindElement(doc, doc, NULL, NULL, NULL, MXML_DESCEND);
	mxml_node_t *node = obj;

	int flg = MXML_DESCEND;
	while	((node = mxmlFindElement(node, obj, NULL, NULL, NULL, flg))) {
		flg = MXML_NO_DESCEND;
		char *name = (char *)mxmlElementGetAttr(node, "name");
		char *val = (char *)mxmlElementGetAttr(node, "val");
		if	(name && val) {
			M2M_TRDBG (TRACE_DETAIL, "### %s = %s\n", name, val);
			//IEC_Add_dictionnary(name, val);
			char *old = rtl_htblGet (*dict, name);
			if	(old) {
				rtl_htblRemove (*dict, name);
				free (old);
			}
			rtl_htblInsert (*dict, name, strdup(val));
		}
	}

	mxmlDelete (doc);
	free (temp);
	return 0;
}

#define IEC_UNKNOWN_COMMAND -99
int execCommand(char *cmd, char *appId, char *content, int size, int tid, char *peer) {
	int rc	= 0;
	iec_user_t *user = NULL;

	M2M_TRDBG (TRACE_DETAIL, "execCommand %s appId=%s\n", cmd, appId);
	GSC_log("EXEC_COMMAND", "A command is received", "", "Command '%s' on appId '%s'", cmd, appId);

	if	(strstr(cmd, "load")) {
		_enqueue(strdup(appId));
		rc = retrieve_IEC_APPLICATION (NULL);
	}
	else if	(strstr(cmd, "start")) {
		user = User_GetByUri(appId);
		if	(!user)
			rc =  -1;
		else
			User_Start(user);
	}
	else if	(strstr(cmd, "suspend")) {
		user = User_GetByUri(appId);
		if	(!user)
			rc =  -1;
		else
			User_Pause(user, TRUE);
	}
	else if	(strstr(cmd, "resume")) {
		user = User_GetByUri(appId);
		if	(!user)
			rc =  -1;
		else
			User_Resume(user);
	}
	else if	(strstr(cmd, "createInstance")) {
		if (Obix2Dictionnary(content, size, &local_dictionnary) == 0) {
			char *app = IEC_Get_dictionnary("iecApplicationID");
			if	(app && *app) {
				user = User_GetByUri(app);
				if	(user) {
					Board_Put (&GLOB.board_errors, "createInstance : %s configuration already defined\n", app);
					M2M_TRDBG (TRACE_ERROR, "createInstance : %s configuration already defined\n", app);
					GSC_log("CONFIGURATION_ALREADY_REFINED", "createInstance : configuration already defined", "", "%s configuration already defined", app);
					return 400;
				}
				check_IEC_APPLICATION (app, tid, peer);
				return -2;
			}
			else {
				char *tmp=NULL;
				iec_asprintf (&tmp, "app%d_%d", time(0), getpid());
				IEC_Add_dictionnary ("iecApplicationID", tmp);
				free (tmp);

				app = IEC_Get_dictionnary("iecApplicationID");
				IEC_Add_dictionnary ("APPLICATION", app);
				M2M_TRDBG (TRACE_DETAIL, "createInstance %s\n", app);
				create_IEC_APPLICATION (app, tid, peer);
			}
		}
	}
	else if	(strstr(cmd, "delete")) {
		user = User_GetByUri(appId);
		if	(user) {
			User_Free(user);
		}
		else {
			M2M_TRDBG (TRACE_ERROR, "delete : %s configuration not found\n", appId);
			Board_Put (&GLOB.board_errors, "delete : %s configuration not found\n", appId);
			GSC_log("CONFIGURATION_NOT_FOUND", "delete : configuration not found", "", "%s", appId);
		}
		delete_IEC_APPLICATION (appId);
	}
	else if	(strstr(cmd, "reset")) {
		rc = User_Reset (appId);
	}
	else if	(strstr(cmd, "commit")) {
		commit_IEC_APPLICATION(appId);
	}
	else if	(strstr(cmd, "rollback")) {
		rollback_IEC_APPLICATION(appId);
	}
	else {
		rc = IEC_UNKNOWN_COMMAND;
	}
	return rc;
}

#if 0 // DEPRECATED
char *dumpToFile(void *content, int len) {
	static int n = 0;
	static char fn[30];
	sprintf (fn, "/tmp/content_%d.xml", n++);
	int fd = open(fn, O_RDWR | O_CREAT | O_TRUNC, 0644);
	int nw = write (fd, content, len);
	if (nw <= len) {
		RTL_TRDBG (TRACE_ERROR, "Write failed rc=%d errno=%d\n", nw, errno);
		close (fd);
		return NULL;
	}
	close (fd);
	RTL_TRDBG (TRACE_DETAIL, "Dumped %d bytes into %s\n", len, fn);
	if	(n == 10)
		n = 0;
	return fn;
}
#endif // DEPRECATED



/**
 * @brief dIa callback when receiving diaCreateRequest
 * @param dia dIa context
 * @param reqEntity Requesting entity
 * @param targetID Identity of the target
 * @param ctt/len Content (xml). Warning : ctt is not NULL terminated
 * @param cttType Content type (xml)
 * @param optAttr
 * @param optHeader
 * @param tid Transaction ID. To be passed as is in the response
 * @param peer Peer IP address. To be passed as is in the response
 * @return <0 if error, 0 if ok
 *
 * WARNING : never reply with diaCreateResponse too early, because ctt will not be valid anymore (freed !)
 */
int cb_diaCreateRequest(dia_context_t *dia, char *reqEntity, char *targetID, void *ctt, int len, char *cttType,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	mxml_node_t *doc = NULL;
	char *temp = NULL;
	int	rc = 0;

	char *args[30];
	char *work = strdup(targetID);

	// Step 1 : COMMANDS
	// reqEntity=coap://plciecong.actility.com:5683/m2m/applications/IEC_app
	// targetID=coap://iecengine:5688/IEC_app/retargeting2/load
	int n = my_strcut(work, "/", args, 30);
	if	(!strcmp(args[n-2], "retargeting1") || !strcmp(args[n-2], "retargeting2")) {

		rc = execCommand(args[n-1], args[n-3], ctt, len, tid, peer);

		if	(rc != IEC_UNKNOWN_COMMAND) {
			switch (rc) {
			case 0 :
				diaCreateResponse(dia, "STATUS_CREATED", targetID, NULL, 0, "application/xml", NULL, tid, peer);
				break;
			case -2 :
				// Response differed
				break;
			case 400 :
				diaCreateResponse(g_dia, "STATUS_BAD_REQUEST", NULL, NULL, 0, "application/xml", NULL, tid, peer);
				break;
			default :
				diaCreateResponse(dia, "STATUS_NOT_FOUND", targetID, NULL, 0, "application/xml", NULL, tid, peer);
				break;
			}
			free (work);
			return 0;
		}
	}

	// Step 2 : notifications
	if	(len <= 0 || !ctt) {
		M2M_TRDBG (TRACE_ERROR, "cb_diaCreateRequest called without content\n");
		Board_Put (&GLOB.board_errors, "cb_diaCreateRequest called without content");
		GSC_log("DIA_ERROR", "cb_diaCreateRequest called without content", "", "");
		rc = -1;
		goto bye;
	}

	doc = mxmlLoadBuffer(ctt, len, &temp, FALSE);

	if	(!doc) {
		M2M_TRDBG (TRACE_ERROR, "cb_diaCreateRequest called with bad xml document\n");
		Board_Put (&GLOB.board_errors, "cb_diaCreateRequest called with bad xml document (tid=%d, targetID=%s)", tid, targetID);
		GSC_log("DIA_ERROR", "cb_diaCreateRequest called with bad xml document", "", "");
		rc = -1;
		goto bye;
	}

	mxml_node_t *node = mxmlFindElement(doc, doc, NULL, NULL, NULL, MXML_DESCEND);
	if (!node || node->type != MXML_ELEMENT) {
		M2M_TRDBG (TRACE_ERROR, "obix document issue\n");
		Board_Put (&GLOB.board_errors, "cb_diaCreateRequest called with bad xml document");
		GSC_log("DIA_ERROR", "cb_diaCreateRequest called with bad xml document", "", "");
		rc = -1;
		goto bye;
	}

	if	(!strcasecmp(node->value.element.name, "m2m:notify")) {
/*
		char *appId = strstr (targetID, "/contact/");
		if	(appId) {
			appId	+= 9;
			char *pt = strchr (appId, '/');
			if	(pt) *pt = 0;
		}
*/
		char *appId = strstr (targetID, "/retargeting4");
		if	(appId) {
			*appId = 0;
			appId = reverse_strchr(targetID, '/');
			if	(appId)	appId ++;
		}
		if	(!appId)	appId = "";
		rc = scan_notify(node, appId);
	}
#if 0 // DEPRECATED
	// Direct Driver Speaking Mode is deprecated
	else if	(g_driverSpeaking) {
		rc = scan_obix(node, targetID);
	}
#endif // DEPRECATED

bye:
	free (work);
	if (doc)
		mxmlDelete (doc);
	if (temp)
		free (temp);
	if	(rc == 0)
		diaCreateResponse(dia, "STATUS_CREATED", targetID, NULL, 0, "application/xml", NULL, tid, peer);
	else
		diaCreateResponse(dia, "STATUS_NOT_FOUND", targetID, NULL, 0, "application/xml", NULL, tid, peer);

	return	rc;
}

void cb_diaUpdateResponse(dia_context_t *dia, char *scode, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
	M2M_TRDBG (TRACE_DETAIL, "cb_diaUpdateResponse %s : [%s]\n", scode, ctt);

	if	(!strcmp(scode, "STATUS_OK")) {
	}
	else {
		// Remove reference. This will cause a new subscription.
		Board_Put (&GLOB.board_errors, "Subscription failed %s", scode);
		XpathSetReference(tid, NULL, scode);
	}
}

/**
 * @brief dIa callback when receiving diaCreateResponse
 */
void cb_diaCreateResponse(dia_context_t *dia, char *scode, char *resourceURI, void *ctt, int len, char* cttType,
	Opt *optHeader, int tid) {
	M2M_TRDBG (TRACE_DETAIL, "cb_diaCreateResponse %s [%s]\n", scode, ctt);

	GSC_response(tid, scode, resourceURI, ctt, len, cttType);

#if !defined(DIA_FIFO)
	//	Response of a create engine
	if (tid == tid_engine) {
		create_IEC_ENGINE_phase2 ();
	}
	//	Response of a rights creation
	if (tid == tid_rights) {
		create_IEC_APPLICATION_phase2 ();
	}
#endif

	if	(!strcmp(scode, "STATUS_CREATED")) {
		char *ref = strstr(resourceURI, M2M_subscriptions);
		if	(ref) {
			ref += strlen(M2M_subscriptions)+1;
			XpathSetReference(tid, ref, NULL);
		}
	}
	else {
/*
<?xml version="1.0" encoding="UTF-8"?>
<m2m:errorInfo xmlns:m2m="http://uri.etsi.org/m2m">
  <m2m:statusCode>STATUS_BAD_REQUEST</m2m:statusCode>
  <m2m:additionalInfo>m2m:expirationTime attribute value is too short: 2013-06-24T22:53:01.000+02:00</m2m:additionalInfo>
</m2m:errorInfo>
*/
		//	Error handling
		char *temp;
		mxml_node_t *doc = mxmlLoadBuffer(ctt, len, &temp, FALSE);
		if	(!doc) {
			M2M_TRDBG (TRACE_ERROR, "ERROR decoding additionalInfo\n");
			Board_Put (&GLOB.board_errors, "GSC ERROR without xml document");
			return;
		}
		mxml_node_t *node = mxmlFindElement(doc, doc, "m2m:additionalInfo", NULL, NULL, MXML_DESCEND);
		if	(!node) {
			M2M_TRDBG (TRACE_ERROR, "No m2m:additionalInfo\n");
			Board_Put (&GLOB.board_errors, "GSC ERROR without m2m:additionalInfo");
			mxmlDelete (doc);
			free (temp);
			return;
		}
		char *txt = getContainedText(node);
		M2M_TRDBG (TRACE_ERROR, "%s\n", txt);
		Board_Put (&GLOB.board_errors, "GSC %s : %s", scode, txt);
		XpathSetReference(tid, NULL, txt);
		mxmlDelete (doc);
		free (temp);
	}
}

void cb_diaDeleteResponse(dia_context_t *dia, char *scode, Opt *optHeader, int tid) {
	M2M_TRDBG (TRACE_DETAIL, "cb_diaDeleteResponse %s\n", scode);
	GSC_response(tid, scode, NULL, NULL, 0, NULL);
}

void handleError(int tid) {
	if	(tid == tid_conf) {
		M2M_TRDBG (TRACE_ERROR, "### ERROR loading appId %s\n", param_app_id);

		Free (content_configuration.data);
		Free (content_configuration.contentType);
		content_configuration.len = 0;

		Free (content_mapping.data);
		Free (content_mapping.contentType);
		content_mapping.len = 0;

		g_lock_retrieve = FALSE;

		/* Another app to download ? */
		if	(rollbacking) {
			rollbacking = FALSE;
			retrieve_IEC_APPLICATION (param_app_id);
		}
		else {
			rollback_IEC_APPLICATION (NULL);
		}
	}
}

/**
 * @brief dIa callback when receiving diaRetrieveRequest
 *        eg. runningStatus, loadedConf, loadedMapping
 */
int cb_diaRetrieveRequest(dia_context_t *dia, char *reqEntity, char *targetID,
	Opt *optAttr, Opt *optHeader, int tid, char *peer) {
	iec_user_t *user;
	char *bigbuf = NULL;
	iec_buff_t out = {NULL,0,NULL};
	char *args[10];
	char *work = strdup(targetID);
	int rc = -1;

	int n = my_strcut(work, "/", args, 10);
	if	(!strcmp(args[n-2], "retargeting3")) {
		if	(!strcmp(args[n-1], "runningStatus")) {
			user = User_GetByUri(args[n-3]);
			if	(user) {
				iec_asprintf (&bigbuf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
				iec_asprintf (&bigbuf, "<obix:bool xmlns:obix=\"http://obix.org/ns/schema/1.1\" name=\"iecRunningStatus\" val=\"%s\"/>",
					user->flagPause ? "FALSE" : "TRUE");
				out.data = bigbuf;
				out.len = strlen(bigbuf);
				out.contentType = "application/xml";
				rc = 0;
			}
		}
		else if	(!strcmp(args[n-1], "loadedConf")) {
			user = User_GetByUri(args[n-3]);
			if	(user && user->content_configuration.data) {
				out	= user->content_configuration;
				rc = 0;
			}
		}
		else if	(!strcmp(args[n-1], "loadedMapping")) {
			user = User_GetByUri(args[n-3]);
			if	(user && user->content_mapping.data) {
				out	= user->content_mapping;
				rc = 0;
			}
		}
	}

	free (work);

	if	(rc == 0)
		diaRetrieveResponse(dia, "STATUS_OK", out.data, out.len, out.contentType, NULL, tid, peer);
	else
		diaRetrieveResponse(dia, "STATUS_NOT_FOUND", NULL, 0, "application/xml", NULL, tid, peer);

	if	(bigbuf)
		free (bigbuf);
	return 0;
}

/**
 * @brief dIa callback when receiving diaRetrieveResponse
 */
void cb_diaRetrieveResponse(dia_context_t *dia, char *scode, void *ctt, int len, char *cttType, Opt *optHeader, int tid) {
	static void *dict = NULL;

	GSC_response(tid, scode, NULL, ctt, len, cttType);

	if	(!strcmp(scode, "STATUS_OK")) {
		if	(tid == tid_params) {
			/*
			 * Parse the dictionnary of options.
			 * WARNING : dict is static and is used next to set user->m_dictionnary
			 */
			Obix2Dictionnary(ctt, len, &dict);
		}
		else if	(tid == tid_conf) {
			content_configuration.data = Calloc(len+1, 1);
			memcpy (content_configuration.data, ctt, len);
			content_configuration.len = len;
			content_configuration.data[len] = 0;
			content_configuration.contentType = Strdup(cttType);
		}
		else if	(tid == tid_mapping) {
			content_mapping.data = Calloc(len+1, 1);
			memcpy (content_mapping.data, ctt, len);
			content_mapping.len = len;
			content_mapping.data[len] = 0;
			content_mapping.contentType = Strdup(cttType);
		}
/*
		else if	(tid == tid_discover) {
			discoverResponse (ctt, len);
		}
*/

		/*
	 	* We have the configuration and the mapping : we can start the IEC configuration.
	 	*/
		if	(content_configuration.data && content_mapping.data) {
			M2M_TRDBG (TRACE_DETAIL, "### SUCCESS LOADING %s\n", param_app_id);

			// The third parameter is the trace level
			iec_user_t *user = User_LoadUri(param_app_id, content_configuration,
				IEC_Config_Int_Default("tracelevel", TraceLevel));

			// trace level is modified by User_LoadUri

			if	(user) {
				M2M_TRDBG (TRACE_DETAIL, "### MAPPING [%s]\n", content_mapping);
				User_LoadMapping(user, content_mapping.data);
				user->m_dictionnary = dict;
				dict = NULL;	// really important. dict is static
				user->content_configuration = content_configuration;
				user->content_mapping = content_mapping;
				User_BucketInit (user);
			}
			else {
				GSC_log("LOAD_FAILED", "Bad configuration document", param_app_id, "");
				Board_Put(&GLOB.board_errors, "Bad configuration document for %s", param_app_id);
				M2M_TRDBG (TRACE_DETAIL, "### Bad configuration document [%s]\n", content_configuration.data);
				M2M_TRDBG (TRACE_DETAIL, "### MAPPING [%s]\n", content_mapping.data);
			}

			content_configuration.data = NULL;
			content_mapping.data = NULL;
			g_lock_retrieve = FALSE;

			sendSubscriptions (TRUE);

			/* Another app to download ? */
			rollback_IEC_APPLICATION (NULL);
		}
	}
	else {
		/* ERROR CASE */
		handleError (tid);
	}
}

/**
 * @brief dIa callback when receiving a dIa or network error
 */
void cb_diaErrorResponse(dia_context_t *dia, void *ctt, int len, char* cttType, Opt *optHeader, int tid) {
	M2M_TRDBG (TRACE_ERROR, "ERROR in dIa link : no response from the GSC\n");
	Board_Put (&GLOB.board_errors, "FATAL : no response from the GSC");
	GSC_response(tid, "ERROR", NULL, ctt, len, cttType);
	handleError (tid);

	XpathSetReference(tid, NULL, "no response from the GSC");
}

static void User_freeTables(iec_user_t *user) {
	inline void fct(void *h, char *key, void *content) {
		// key already freed by rtl_htblDestroy
		outMapping_t *outM = (outMapping_t *)content;
		if (outM->targetID) free(outM->targetID);
		if (outM->template) free(outM->template);
		free (content);
	}

#if 0 // DEPRECATED
	if	(user->m_inputTable) {
		rtl_htblSetRmovFunc (user->m_inputTable, fct);
		rtl_htblDestroy (user->m_inputTable);
		user->m_inputTable	= NULL;
	}
#endif
	if	(user->m_outputTable) {
		rtl_htblSetRmovFunc (user->m_outputTable, fct);
		rtl_htblDestroy (user->m_outputTable);
		user->m_outputTable	= NULL;
	}
	if	(user->xpath_list.next) {
		struct list_head *elem, *pos;
		list_for_each_safe (elem, pos, &user->xpath_list) {
			iec_xpath_t *xpath	= list_entry(elem, iec_xpath_t, head);
			free (xpath->targetID);
			free (xpath->xpath_expr);
			free (xpath->iec_input_var);
			free (xpath->value);
			free (xpath->subscription_ref);
			free (xpath->msg);
			free (xpath);
		}
	}
}

int User_LoadMapping(iec_user_t *user, char *string) {
	mxml_node_t *doc = mxmlLoadString(NULL, string, MXML_OPAQUE_CALLBACK);
	if	(!doc) {
		Board_Put (&GLOB.board_errors, "NOTICE : Mapping for appId %s failed (bad xml)");
		return -2;
	}

	// TODO : just for testing. REMOVE !!!!
	User_BucketInit (user);

	User_freeTables (user);
#if 0 // DEPRECATED
	user->m_inputTable	= rtl_htblCreate(32, NULL);
#endif // DEPRECATED
	user->m_outputTable	= rtl_htblCreate(32, NULL);
	INIT_LIST_HEAD (&user->xpath_list);

	//	maxINvariables and maxOUTvariables control
	char *res;
	int maxIN, maxOUT, countIN, countOUT;
	res = User_GetDictionnary(user, "iecMaxINvariables");
	maxIN = res ? atoi(res) : INT_MAX;
	res = User_GetDictionnary(user, "iecMaxOUTvariables");
	maxOUT = res ? atoi(res) : INT_MAX;

	countIN = countOUT = 0;

	mxml_node_t *map;
	map = doc;
	int flg = MXML_DESCEND;
	while	((map = mxmlFindElement(map, doc, "map", NULL, NULL, flg))) {
		flg = MXML_NO_DESCEND;
		char *targetID = (char *)mxmlElementGetAttr(map, "targetID");
		char *output = (char *)mxmlElementGetAttr(map, "output");
		char *input = (char *)mxmlElementGetAttr(map, "input");
		char *value = (char *)mxmlElementGetAttr(map, "value");
		char *priority = (char *)mxmlElementGetAttr(map, "priority");

		if	(!targetID)
			continue;

		if	(output) {
#if 0 // DEPRECATED. targetID can't be pre-calculated
			if	(strncasecmp(targetID, "coap", 4)) {
				char *bigbuf = NULL;
				iec_asprintf (&bigbuf, "coap://%s%s", IEC_Config("w_coapaddr_r"), targetID);
				targetID	= bigbuf;
			}
			else
				targetID	= strdup(targetID);
#endif

			outMapping_t *outM = calloc(sizeof(outMapping_t), 1);

			mxml_node_t *template = mxmlFindElement(map, map, NULL,NULL, NULL,MXML_DESCEND);
			if (template) {
				char *str = template->value.element.name;
				if	(!memcmp(str, "![CDATA[", 8))
					str += 8;
				while	(str[strlen(str)-1] == ']')
					str[strlen(str)-1] = 0;
				outM->template = strdup(str);
				//RTL_TRDBG (TRACE_DETAIL, "template (%s)\n", outM->template);
			}

			outM->targetID	= strdup(targetID);

			if	(value) {
				char *bigbuf = NULL;
				iec_asprintf (&bigbuf, "%s_%s", output, value);
				output = bigbuf; // no need to free
			}
			else {
				output	= strdup(output);
			}
			//rtl_htblInsert (user->m_outputTable, output, targetID);
			rtl_htblInsert (user->m_outputTable, output, outM);
			M2M_TRDBG (TRACE_DETAIL, "MAPPING output %s ==> %s\n", output, outM->targetID);

			if	(priority && !strcasecmp(priority, "HIGH")) {
				iec_variable_t *v = rtl_htblGet(user->m_accessVars, output);
				if	(v) {
					v->flags	|= VARFLAG_PRIORITY_HIGH;
				}
			}
			countOUT ++;
		}
		else if	(input) {
#if 0 // DEPRECATED
/*
			if	(value) {
				sprintf (bigbuf, "%s_%s", targetID, value);
				targetID = bigbuf;
			}
*/
			rtl_htblInsert (user->m_inputTable, strdup(targetID), strdup(input));
			rtl_htblInsert (user->m_inputTable, strdup(input), strdup(targetID));
#endif // DEPRECATED

			char *xpath_expr = (char *)mxmlElementGetAttr(map, "path");
			XpathAdd (user, targetID, xpath_expr, input, value);

			M2M_TRDBG (TRACE_DETAIL, "MAPPING input %s <=> %s\n", targetID, input);
			countIN ++;
		}

		if	(countIN > maxIN) {
			User_GSC_log(user, "MAXIN_REACHED", "maxINvariable reached", "", "");
		}

		if	(countOUT > maxOUT) {
			User_GSC_log(user, "MAXOUT_REACHED", "maxOUTvariable reached", "", "");
		}
	}
	mxmlRelease (doc);
	return 0;
}

int User_LoadFileMapping(iec_user_t *user, char *fn) {
	char *buf;
	struct stat st;

	int	fd = open(fn, O_RDONLY);
	if	(fd < 0)
		return	-1;

	if	(fstat(fd, &st) < 0) {
		close (fd);
		return	-1;
	}

	buf	= Calloc(st.st_size+1, 1);
	int n = read (fd, buf, st.st_size);
	buf[n] = 0;
	close (fd);

	return User_LoadMapping(user, buf);
}

void M2MsendVar(iec_user_t *user, void *table, char *key, char *value, iec_variable_t *var) {
	char *bigbuf = NULL;

	RTL_TRDBG (TRACE_DETAIL, "dIa SENDVAR %s %s\n", key, value);

	if	(!g_dia || !table)
		return;

	//char *targetID = rtl_htblGet (table, key);
	outMapping_t *outM = rtl_htblGet (table, key);
	if	(!outM) {
		iec_asprintf (&bigbuf, "%s_%s", key, value);
		outM = rtl_htblGet (table, bigbuf);
		free (bigbuf);
		if	(!outM) {
			RTL_TRDBG (TRACE_DETAIL, "  Variable %s is not declared in output\n", key);
			return;
		}
	}

	if	(!(var->flags & VARFLAG_PRIORITY_HIGH)) {
		if	(User_BucketGet(user) == FALSE) {
			return;
		}
	}

	bigbuf = NULL;
	iec_asprintf (&bigbuf, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

	char *template = outM->template;
	char *targetID = outM->targetID;

	if	(!template) {
#if 0 // DEPRECATED
		char hour[50];
		Unixtime_to_iso8601 (time(0), hour);
		iec_asprintf (&bigbuf, "<%s val=\"%s\" xmlns=\"http://obix.org/ns/schema/1.1\" x-date=\"%s\"/>",
			iec_to_obix_types[(int)var->type], value, hour);
#endif
		iec_asprintf (&bigbuf, "<%s val=\"%s\" xmlns=\"http://obix.org/ns/schema/1.1\" />",
			iec_to_obix_types[(int)var->type], value);
	}
	else {
		inline char *__getvar(char *name) {
			char *value = rtl_htblGet(user->m_variables, name);
			return value ? value:"";
		}
		char tmp[200];
		rtl_evalPath (template, tmp, 200, __getvar);
		iec_asprintf (&bigbuf, "%s", tmp);
	}

	RTL_TRDBG (TRACE_DETAIL, "%s\n", bigbuf);

	/*
	 * ref-ongv2.0-iec-engine-integration-rev6.docx : 3.5 Outgoing M2M requests
	 */

	char *tmp = NULL;
	char *nsc = IEC_Config("w_nsclbaseuri");

	if	(!strncmp(targetID, nsc, strlen(nsc))) {
		// Case 2 : targetID is the NSC : use dIaProxy(GSC) for the UDP dialog
		iec_asprintf (&tmp, "coap://%s:%s", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
		diaProxy (g_dia, tmp);
	}
	else if	(strstr(targetID, "://")) {
		// Case 3 : targetID is another GSCL
		iec_asprintf (&tmp, "coap://%s:%s", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
		diaProxy (g_dia, tmp);
	}
	else {
		// Case 1 : targetID is local (destination is the GSC)
		iec_asprintf (&tmp, "coap://%s:%s%s",
			IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), targetID);
		targetID	= tmp;
		diaProxy (g_dia, NULL);
RTL_TRDBG (TRACE_DETAIL, "Case1 targetID=%s\n", targetID);
	}
	int tid;
	diaCreateRequest (g_dia, User_GetRequestingEntity(user), targetID, bigbuf, strlen(bigbuf), "application/xml", NULL, NULL, &tid);
	free (bigbuf);
	if	(tmp) free (tmp);
}

/**
 * @brief Sends a dIa createRequest for each updated % variable
 * @return void
 */
void checkAccessVars() {
	iec_user_t *user;

	inline void fct(char *key, void *content) {

		//RTL_TRDBG (TRACE_DETAIL, "checkAccessVars %s\n", key);
		// Only % variables. Others are inter-script globals
		if	(*key != '%')
			return;
		iec_variable_t *var = (iec_variable_t *)content;
		if	((var->flags & VARFLAG_CHANGED) || (var->flags & VARFLAG_PRIORITY_HIGH)) {
			var->flags &= ~VARFLAG_CHANGED;
			char *value = NULL;
			printVarValue (var, &value);
			CLI_valueHasChanged(var);
			M2MsendVar(user, user->m_outputTable, key, value, var);
			free (value);
		}
	}
	static time_t next = 0;
	time_t now = time(0);
	if	(!next)
		next	= now;

	if	(now > next) {
		next	+= IEC_Config_Int_Default("accessVars_time", 1);
		struct list_head *elem;
		list_for_each (elem, &GLOB.user_list) {
			user	= list_entry(elem, iec_user_t, head);
			if	(user->m_outputTable)
				rtl_htblDump(user->m_accessVars, fct);
		}
	}
}

/**
 * @brief Manages subscriptions
 * @param A time_t (Unix time)
 * @param The output buffer
 * @return void
 */
void sendSubscriptions(BOOL force) {
	iec_user_t *user;
	char sub[200], resub[200];

	inline void fct(char *key, void *content) {
		// Only % variables. Others are inter-script globals
		if	(*key != '%')
			return;
/*
		// mapping VAR => targetID
		char *targetID = rtl_htblGet (user->m_inputTable, key);
		if	(!targetID)
			return;
*/
		iec_xpath_t *xp = XpathGetWithInputVar (user, key);
		if	(!xp)
			return;

		if	(xp->nextSubscription > time(0))
			return;

		xp->nextSubscription	= time(0) + IEC_Config_Int_Default("subscriptions_time", 3600);

		char isoTimeBuf[100];
		Unixtime_to_iso8601 (xp->nextSubscription, isoTimeBuf);
		IEC_Add_dictionnary ("expirationTime", isoTimeBuf); //  ${expirationTime} is used by subscription.xml

		// we must re-subscribe before largely before subscription's end
		xp->nextSubscription	-= IEC_Config_Int_Default("subscriptions_delta_time", 60);

		char *targetID = xp->targetID;
		RTL_TRDBG (TRACE_DETAIL, "sendSubscription %s %s %s\n", key, targetID, isoTimeBuf);

		char *tmp = NULL;
		char *nsc = IEC_Config("w_nsclbaseuri");

		/*
		 * ref-ongv2.0-iec-engine-integration-rev6.docx : 3.5 Outgoing M2M requests
		 */

		if	(!strncmp(targetID, nsc, strlen(nsc))) {
			// Case 2 : targetID is the NSC : use dIaProxy(GSC) for the UDP dialog
			iec_asprintf (&tmp, "http://localloopback:8080?targetID=http://%s.%s:8080%s/%s/retargeting4",
				IEC_Config("w_boxname"), IEC_Config("w_domainname"), IEC_Config("s_rootapp"),
				IEC_Get_dictionnary("APPLICATION"));
			IEC_Add_dictionnary ("CONTACT", tmp);
			free (tmp); tmp=NULL;

			iec_asprintf (&tmp, "coap://%s:%s", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
			diaProxy (g_dia, tmp);
			free (tmp); tmp=NULL;
			iec_asprintf (&tmp, "%s/subscriptions", targetID);
		}
		else if	(strstr(targetID, "://")) {
			// Case 3 : targetID is another GSCL
			iec_asprintf (&tmp, "%s?targetID=http://%s.%s:8080%s/%s/retargeting4",
				nsc,
				IEC_Config("w_boxname"), IEC_Config("w_domainname"), IEC_Config("s_rootapp"),
				IEC_Get_dictionnary("APPLICATION"));
			IEC_Add_dictionnary ("CONTACT", tmp);
			free (tmp); tmp=NULL;

			iec_asprintf (&tmp, "coap://%s:%s", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
			diaProxy (g_dia, tmp);
			free (tmp); tmp=NULL;
			iec_asprintf (&tmp, "%s/subscriptions", targetID);
		}
		else {
			// Case 1 : targetID is local (destination is the GSC)
			iec_asprintf (&tmp, "coap://%s:%s/%s/retargeting4",
				IEC_Config("w_coapaddr_l"), IEC_Config("w_coapport_l"), IEC_Get_dictionnary("APPLICATION"));
			IEC_Add_dictionnary ("CONTACT", tmp);
			free (tmp); tmp=NULL;

			iec_asprintf (&tmp, "coap://%s:%s%s/subscriptions",
				IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), targetID);
			diaProxy (g_dia, NULL);
		}

		int	reqType;
		char	*file;

		//	Is it a re-subscription ?
		if	(xp->subscription_ref) {
			RTL_TRDBG (TRACE_DETAIL, "Xpath re-subscription %s %s\n", xp->targetID, xp->subscription_ref);
			iec_asprintf (&tmp, "/%s", xp->subscription_ref);
			reqType	= GSC_UPDATE;
			file	= resub;
		}
		else {
			RTL_TRDBG (TRACE_DETAIL, "Xpath subscription %s\n", xp->targetID);
			reqType	= GSC_CREATE;
			file	= sub;
		}

		targetID	= tmp;

		// We need an immediate send to know the tid
		int tid = GSC_send (user, reqType, targetID, file);
		if	(tid > 0)
			xp->tid	= tid;
		if (tmp) free (tmp);
	}

#if 0 // DEPRECATED
	if	(g_driverSpeaking)
		return;
#endif // DEPRECATED

	static time_t next = 0;
	time_t now = time(0);
	if	(!next || force)
		next	= now;

	if	(now >= next) {
		sprintf (sub, "%s/subscription.xml", Operating_TemplatesDir());
		sprintf (resub, "%s/resubscription.xml", Operating_TemplatesDir());
/*
		next	+= IEC_Config_Int_Default("subscriptions_time", 3600);
		// we must re-subscribe before largely before subscription's end
		next	-= IEC_Config_Int_Default("subscriptions_delta_time", 60);
*/
		next	+= 60;

/*
		Unixtime_to_iso8601 (next, isoTimeBuf);
		RTL_TRDBG (TRACE_DETAIL, "subscriptions %s\n", isoTimeBuf);
		//  ${expirationTime} is used by subscription.xml
		IEC_Add_dictionnary ("expirationTime", isoTimeBuf);
*/
		struct list_head *elem;
		list_for_each (elem, &GLOB.user_list) {
			user	= list_entry(elem, iec_user_t, head);
			//  ${APPLICATION} is used by subscription.xml
			IEC_Add_dictionnary ("APPLICATION", user->name);
			rtl_htblDump(user->m_accessVars, fct);
		}
	}
}


#if !defined(DIA_FIFO)
int create_IEC_ENGINE() {
	char targetID[256];
	char file[200];

	if	(!g_dia)
		return 0;
/*
IEC_61131_VM_c.xml
IEC_61131_VM/containers/DESCRIPTOR_c.xml
IEC_61131_VM/containers/DESCRIPTOR/contentInstances/latest_c.xml
*/

	sprintf (targetID, "coap://%s:%s/m2m/applications", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_61131_VM_c.xml", Operating_TemplatesDir());
	tid_engine = GSC_send (GSC_CREATE, targetID, file);
	return 0;
}

int create_IEC_ENGINE_phase2() {
	char targetID[256];
	char file[200];

	sprintf (targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_61131_VM/containers/DESCRIPTOR_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers/DESCRIPTOR/contentInstances", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_61131_VM/containers/DESCRIPTOR/contentInstances/latest_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_61131_VM/containers/appLogs_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	return 0;
}

#else
int create_IEC_ENGINE() {
	char targetID[256];
	char file[200];

	if	(!g_dia)
		return 0;

	M2M_TRDBG (TRACE_DETAIL, "Create ENGINE\n");

	sprintf (targetID, "coap://%s:%s/m2m/applications", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_61131_VM_c.xml", Operating_TemplatesDir());
	GSC_send2 (GSC_CREATE, targetID, file, NULL);

	sprintf (targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));

	sprintf (file, "%s/applications/IEC_61131_VM/containers/DESCRIPTOR_c.xml", Operating_TemplatesDir());
	GSC_send2 (GSC_CREATE, targetID, file, NULL);
	sprintf (file, "%s/applications/IEC_61131_VM/containers/appLogs_c.xml", Operating_TemplatesDir());
	GSC_send2 (GSC_CREATE, targetID, file, NULL);

	sprintf (targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM/containers/DESCRIPTOR/contentInstances", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_61131_VM/containers/DESCRIPTOR/contentInstances/latest_c.xml", Operating_TemplatesDir());
	GSC_send2 (GSC_CREATE, targetID, file, NULL);

	return 0;
}

#endif

int delete_IEC_ENGINE() {
	char targetID[256];

	if	(!g_dia)
		return 0;
	sprintf (targetID, "coap://%s:%s/m2m/applications/IEC_61131_VM", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	GSC_send2 (GSC_DELETE, targetID, NULL, NULL);
	return 0;
}

#if !defined(DIA_FIFO)
/**
 * @brief Phase 1 of application creation : accessRights
 * @param An Application Identifier
 * @return 0 if success, -1 else
 */
int create_IEC_APPLICATION(char *IEC_appID) {
	char targetID[256];
	char file[200];

	if	(!g_dia) return 0;

	sprintf (targetID, "coap://%s:%s/m2m/accessRights", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/accessRights/IEC_app_AR_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/accessRights", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/accessRights/IEC_app_AR2_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/accessRights", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/accessRights/IEC_app_AR3_c.xml", Operating_TemplatesDir());
	tid_rights = GSC_send (GSC_CREATE, targetID, file);

	return 0;
}

/**
 * @brief Phase 2 of application creation : called when tid_rights received a response
 * @return 0 if success, -1 else
 */
int create_IEC_APPLICATION_phase2() {
	char targetID[256];
	char file[200];

	if	(!g_dia) return 0;

	char *IEC_appID = IEC_Get_dictionnary("APPLICATION");

/*
IEC_app_c.xml
IEC_app/containers/DESCRIPTOR_c.xml
IEC_app/containers/CONFIGURATION_c.xml
IEC_app/containers/DESCRIPTOR/contentInstances/latest_c.xml
IEC_app/containers/MAPPING_c.xml
*/

	sprintf (targetID, "coap://%s:%s/m2m/applications", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_app_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	sprintf (file, "%s/applications/IEC_app/containers/DESCRIPTOR_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (file, "%s/applications/IEC_app/containers/CONFIGURATION_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (file, "%s/applications/IEC_app/containers/MAPPING_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (file, "%s/applications/IEC_app/containers/committedConf_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (file, "%s/applications/IEC_app/containers/committedMapping_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers/DESCRIPTOR/contentInstances", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	sprintf (file, "%s/applications/IEC_app/containers/DESCRIPTOR/contentInstances/latest_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	sprintf (file, "%s/applications/IEC_app/containers/appLogs_c.xml", Operating_TemplatesDir());
	GSC_send (GSC_CREATE, targetID, file);

	return 0;
}
#else

typedef struct {
	int tid;
	char peer[30];
} createAppInfo_t;

/**
	Response to check_IEC_APPLICATION
 */
void check_IEC_APPLICATION_resp(struct dIa_action *da) {
	createAppInfo_t *info = da->udata;
	M2M_TRDBG (TRACE_DETAIL, "check_IEC_APPLICATION_resp %s [%s]\n", da->scode, da->ctt);
	if	(!strcmp(da->scode, "STATUS_OK")) {
		diaCreateResponse(g_dia, "STATUS_BAD_REQUEST", NULL, NULL, 0, "application/xml", NULL, info->tid, info->peer);
	}
	else {
		//diaCreateResponse(g_dia, "STATUS_CREATED", NULL, NULL, 0, "application/xml", NULL, info->tid, info->peer);
		char *app = IEC_Get_dictionnary("iecApplicationID");
		IEC_Add_dictionnary ("APPLICATION", app);
		M2M_TRDBG (TRACE_DETAIL, "createInstance %s\n", app);
		create_IEC_APPLICATION (app, info->tid, info->peer);
	}
}

/**
	Check if an appID still exists in GSC
 */
void check_IEC_APPLICATION(char *IEC_appID, int tid, char *peer) {
	if	(!g_dia) return;
	M2M_TRDBG (TRACE_DETAIL, "check_IEC_APPLICATION %s\n", IEC_appID);
	char *targetID = NULL;
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);

	struct dIa_action *da = GSC_send2 (GSC_RETRIEVE, targetID, NULL, check_IEC_APPLICATION_resp);

	createAppInfo_t *info = Calloc(sizeof(createAppInfo_t), 1);
	info->tid = tid;
	strcpy (info->peer, peer);
	da->udata = info;

	free (targetID);
}

void create_IEC_APPLICATION_resp(struct dIa_action *da) {
	createAppInfo_t *info = da->udata;
	M2M_TRDBG (TRACE_DETAIL, "create_IEC_APPLICATION_resp %s\n", da->scode);
	char *scode;
	if	(!strcmp(da->scode, "STATUS_OK"))
		scode = "STATUS_CREATED";
	else if	(!strcmp(da->scode, "STATUS_CREATED"))
		scode = "STATUS_CREATED";
	else
		scode = "STATUS_METHOD_NOT_ALLOWED";

	diaCreateResponse(g_dia, scode, NULL, NULL, 0, "application/xml", NULL, info->tid, info->peer);
}

/**
	WARNING : global dictionnary must have the variable APPLICATION
 */
int create_IEC_APPLICATION(char *IEC_appID, int tid, char *peer) {
	char targetID[256];
	char file[200];
	struct dIa_action *da;

	if	(!g_dia) return 0;

	// accessRights
	sprintf (targetID, "coap://%s:%s/m2m/accessRights", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));

	sprintf (file, "%s/accessRights/IEC_app_AR_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/accessRights/IEC_app_AR2_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/accessRights/IEC_app_AR3_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/accessRights/IEC_app_AR4_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;

	// IEC_app
	sprintf (targetID, "coap://%s:%s/m2m/applications", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"));
	sprintf (file, "%s/applications/IEC_app_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;

	// PLC configuration
	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);

	sprintf (file, "%s/applications/IEC_app/containers/DESCRIPTOR_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/applications/IEC_app/containers/CONFIGURATION_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
/*
	// For testing app creation failure
	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containersBLABLA", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
*/
	sprintf (file, "%s/applications/IEC_app/containers/MAPPING_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/applications/IEC_app/containers/committedConf_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/applications/IEC_app/containers/committedMapping_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;
	sprintf (file, "%s/applications/IEC_app/containers/appLogs_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, NULL);
	da->more = TRUE;

	// attributes
	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers/DESCRIPTOR/contentInstances", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	sprintf (file, "%s/applications/IEC_app/containers/DESCRIPTOR/contentInstances/latest_c.xml", Operating_TemplatesDir());
	da=GSC_send2 (GSC_CREATE, targetID, file, create_IEC_APPLICATION_resp);
	createAppInfo_t *info = Calloc(sizeof(createAppInfo_t), 1);
	info->tid = tid;
	strcpy (info->peer, peer);
	da->udata = info;

	return 0;
}
#endif


/**
 * @brief
 * @return 0 if success, -1 else
 */
int commit_IEC_APPLICATION(char *appId) {
	char targetID[256];
	int tid;

	if	(!g_dia) return -1;

	iec_user_t *user = User_GetByUri(appId);
	if	(!user || !user->content_configuration.data || !user->content_mapping.data)
		return -1;

	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers/committedConf/contentInstances", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaCreateRequest (g_dia, IEC_Config("w_reqEntity"), targetID, user->content_configuration.data,
		user->content_configuration.len,
		"application/xml", NULL, NULL, &tid);

	sprintf (targetID, "coap://%s:%s/m2m/applications/%s/containers/committedMapping/contentInstances", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaCreateRequest (g_dia, IEC_Config("w_reqEntity"), targetID, user->content_mapping.data,
		user->content_mapping.len,
		"application/xml", NULL, NULL, &tid);

	return 0;
}

void rollback_IEC_APPLICATION(char *appId) {
	char *targetID = NULL;

	if	(g_lock_retrieve && (time(0) - g_lock_time < IEC_GLOCK_TIME)) {
		M2M_TRDBG (TRACE_ERROR, "rollback : Another load command already running\n");
		Board_Put (&GLOB.board_errors, "rollback : Another load command already running");
		GSC_log("LOAD_ALREADY_RUNNING", "rollback : Another load command already running", "", "");
		return;
	}

	if	(!appId) {
		char *data = _dequeue ();
		if	(!data)
			return;
		appId	= data;
	}
	else
		appId = strdup(appId);

	M2M_TRDBG (TRACE_DETAIL, "### BEGIN ROLLBACK %s\n", appId);

	g_lock_retrieve = TRUE;
	g_lock_time = time(0);

	// Free previous one
	if	(param_app_id)
		free (param_app_id);
	param_app_id = strdup(appId);

	//	Retrieve configuration
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s/containers/committedConf/contentInstances/latest/content",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid_conf);
	free (targetID);

	//	Retrieve mapping
	targetID = NULL;
	iec_asprintf (&targetID, "coap://%s:%s/m2m/applications/%s/containers/committedMapping/contentInstances/latest/content",
		IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), appId);
	diaRetrieveRequest (g_dia, IEC_Config("w_reqEntity"), targetID, NULL, NULL, &tid_mapping);
	free (targetID);

	rollbacking = TRUE;
}

int delete_IEC_APPLICATION(char *IEC_appID) {
	char targetID[256];

	if	(!g_dia)
		return 0;
	sprintf (targetID, "coap://%s:%s/m2m/applications/%s", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	GSC_send2 (GSC_DELETE, targetID, NULL, NULL);

	sprintf (targetID, "coap://%s:%s/m2m/accessRights/%s_AR", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	GSC_send2 (GSC_DELETE, targetID, NULL, NULL);

	sprintf (targetID, "coap://%s:%s/m2m/accessRights/%s_AR2", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	GSC_send2 (GSC_DELETE, targetID, NULL, NULL);

	sprintf (targetID, "coap://%s:%s/m2m/accessRights/%s_AR3", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	GSC_send2 (GSC_DELETE, targetID, NULL, NULL);

	sprintf (targetID, "coap://%s:%s/m2m/accessRights/%s_AR4", IEC_Config("w_coapaddr_r"), IEC_Config("w_coapport_r"), IEC_appID);
	GSC_send2 (GSC_DELETE, targetID, NULL, NULL);

	return 0;
}

/**
 * @brief Retrieve the minimum delay to sleep before the next dIa event
 * @param tv A timeval struct
 * @return void
 */
void iec_m2m_nextTimer(struct timeval *tv) {
	if	(g_dia)
		dia_nextTimer(g_dia, tv);
	else
		tv->tv_sec = tv->tv_usec = 0;
}

int iec_m2mPoll() {
	if	(g_dia) {
		dia_set_log_level(IEC_Config_Int_Default("tracedia", 0));
		AllUsers_BucketTime();
		checkAccessVars();
		sendSubscriptions (FALSE);
	}
	return 0;
}

int network_request (void *tab, int fd, void *ref1, void *ref2, int events) {
	return	POLLIN;
}

int network_input (void *tab, int fd, void *ref1, void *ref2, int events) {
	dia_input(ref1);
	return	0;
}

dia_callbacks_t callbacks = {
	cb_diaRetrieveRequest,
	cb_diaRetrieveResponse,
	cb_diaCreateRequest,
	cb_diaCreateResponse,
	NULL,
	cb_diaUpdateResponse,
	NULL,
	cb_diaDeleteResponse,
	cb_diaErrorResponse
};

void AddBrackets() {
	char buf[128];
	char *addr = IEC_Config("w_coapaddr_l");
	if  (strchr(addr,':') != NULL) { // ipv6
		sprintf(buf,"[%s]",addr);
		IEC_AddConfig ("w_coapaddr_l", buf);
	}
	addr = IEC_Config("w_coapaddr_r");
	if  (strchr(addr,':') != NULL) { // ipv6
		sprintf(buf,"[%s]",addr);
		IEC_AddConfig ("w_coapaddr_r", buf);
	}
}

int iec_m2mInit() {
	if	(!strcasecmp(IEC_Config_Default("withdia", "TRUE"), "FALSE")) {
		Board_Put (&GLOB.board_errors, "NOTICE : withdia option is FALSE (spvconfig.xml)");
		return -1;
	}
	dia_set_log_level(IEC_Config_Int_Default("tracedia", 0));
	GLOB.level = (IEC_Config_Int_Default("tracem2m", 0));
	g_dia = dia_createContext(IEC_Config("w_coapaddr_l"), IEC_Config("w_coapport_l"), &callbacks);

	if (!g_dia) {
		M2M_TRDBG (TRACE_ERROR, "dia_createContext error %s %s\n", IEC_Config("w_coapaddr_l"), IEC_Config("w_coapport_l"));
		Board_Put (&GLOB.board_errors, "FATAL : dIa bind failed on %s port %s", IEC_Config("w_coapaddr_l"), IEC_Config("w_coapport_l"));
		return -1;
	}

	AddBrackets();

	dia_setNonBlocking (g_dia, 20);
	INIT_LIST_HEAD (&GLOB.dIa_list);

	if	(!strcasecmp(IEC_Config_Default("delete_IEC_61131_VM", "FALSE"), "TRUE")) {
		delete_IEC_ENGINE();
	}

	m2m_checkENGINE();

	// By default, IEC_ENGINE is already created, so we do a discover. If discover fails, then we
	// try a create_IEC_ENGINE.
	//create_IEC_ENGINE();
/*
	if	(strcasecmp(IEC_Config_Default("appdiscover", "TRUE"), "FALSE")) {
		m2m_discover();
	}
	else
		Board_Put (&GLOB.board_errors, "NOTICE : appdiscover option is FALSE (spvconfig.xml)");
*/

	return rtl_pollAdd (MainTbPoll, dia_getFd(g_dia), network_input, network_request, g_dia, NULL);
}
