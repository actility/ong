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
#include <stddef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>
#include <poll.h>
#include <math.h>

#include <mxml.h>

#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>

#include "iec_debug.h"
#include "plc.h"
#include "literals.h"
#include "timeoper.h"


extern void iec_gettimeofday(IEC_TIMESPEC *iec_time);


void CurrentTime(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	iec_gettimeofday (&RET->p.TIMEvar);
	RET->t	= TIMEtype;
}

void LocalDT(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	iec_gettimeofday (&RET->p.DTvar);
	RET->t	= DTtype;
}

void sec(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_sec;
}

void min(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_min;
}

void hour(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_hour;
}

void mday(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_mday;
}

void mon(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_mon;
}

void year(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_year;
}

void wday(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_wday;
}

void yday(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_yday;
}

void isdst(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= BOOLtype;
	RET->p.BOOLvar = tm->tm_isdst;
}

void gmtoff(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= DINTtype;
	RET->p.DINTvar = tm->tm_gmtoff;
}

void tmzone(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	struct tm *tm = localtime(&in->DTvar.tv_sec);
	RET->t	= STRINGtype;
	setParameter (&RET->p, (void *)tm->tm_zone, STRINGtype);
}

#include <astro.c>
static double Lat=0, Lon=0;
void setLatLon(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	Lat = (double)in++->REALvar;
	Lon = (double)in++->REALvar;
	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void isDay(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	astro_t	ast;
	memset (&ast, 0, sizeof(ast));
	ast.tm = localtime(&in->DTvar.tv_sec);
	ast.Lat	= Lat;
	ast.Lat	= Lon;
	astro_calculate (&ast);
	RET->t	= BOOLtype;
	RET->p.BOOLvar = ast.DayTime;
}

void sunsetTime(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	astro_t	ast;
	memset (&ast, 0, sizeof(ast));
	time_t t = time(0);
	ast.tm = localtime(&t);
	ast.Lat	= Lat;
	ast.Lat	= Lon;
	astro_calculate (&ast);

	ast.tm->tm_hour = ast.tm->tm_min = ast.tm->tm_sec = 0;
	t = mktime(ast.tm);
	t += (time_t)(ast.HC*3600.0);

	RET->t	= DTtype;
	RET->p.DTvar.tv_sec	= t;
	RET->p.DTvar.tv_nsec	= 0;
}

void sunriseTime(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	astro_t	ast;
	memset (&ast, 0, sizeof(ast));
	time_t t = time(0);
	ast.tm = localtime(&t);
	ast.Lat	= Lat;
	ast.Lat	= Lon;
	astro_calculate (&ast);

	ast.tm->tm_hour = ast.tm->tm_min = ast.tm->tm_sec = 0;
	t = mktime(ast.tm);
	t += ast.HL*3600;

	RET->t	= DTtype;
	RET->p.DTvar.tv_sec	= t;
	RET->p.DTvar.tv_nsec	= 0;
}

#define PLC_FBD
#include <cal.c>

#if 0 // OBSOLETE
void iec_newCalendarEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	STRING	DAYSTART = in++->STRINGvar;
	STRING	START = in++->STRINGvar;
	STRING	END = in++->STRINGvar;

	char	*id = strndup((char *)ID.body, ID.len);
	char	*dayStart = strndup((char *)DAYSTART.body, DAYSTART.len);
	char	*start = strndup((char *)START.body, START.len);
	char	*end = strndup((char *)END.body, END.len);

	newCalendarEvent (id, dayStart, start, end);
	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_NewCalendar ID=%s dayStart=%s Start=%s End=%s\n", id, dayStart, start, end );

	free (id); free(dayStart); free(start); free(end); // *LNI* to be removed when datatype will be changed

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_calendarEventExpires(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	STRING	DAY = in++->STRINGvar;

	char	*id = strndup((char *)ID.body, ID.len);
	char	*day = strndup((char *)DAY.body, DAY.len);

	calendarEventExpires (id, day);
    RTL_TRDBG (TRACE_FBD, "FBD_TRACE_calendarEventExpires ID=%s Day=%s \n", id, day );

	free (id); free(day); // *LNI* to be removed when datatype will be changed

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_newCalendarDailyEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	STRING	START = in++->STRINGvar;
	STRING	END = in++->STRINGvar;
	DINT	interval = in++->DINTvar;

	char	*id = strndup((char *)ID.body, ID.len);
	char	*start = strndup((char *)START.body, START.len);
	char	*end = strndup((char *)END.body, END.len);

	newCalendarDailyEvent (id, start, end, interval);

	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_NewDailyCalendar ID=%s Start=%s End=%s Reccuring=%d\n", id, start, end, interval );

	free (id); free(start); free(end); // *LNI* to be removed when datatype will be changed

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_newCalendarWeeklyEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	STRING	START = in++->STRINGvar;
	STRING	END = in++->STRINGvar;
	DINT	daysInWeek = in++->DINTvar;

	char	*id = strndup((char *)ID.body, ID.len);
	char	*start = strndup((char *)START.body, START.len);
	char	*end = strndup((char *)END.body, END.len);

	newCalendarWeeklyEvent (id, start, end, (int8_t)daysInWeek);

	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_NewWeeklyCalendar ID=%s Start=%s End=%s Weekdays=0x%x\n", id, start, end, daysInWeek );

	free (id); free(start); free(end); // *LNI* to be removed when datatype will be changed

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_checkCalendarEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	DT	t = in++->DTvar;

	char	*id = strndup((char *)ID.body, ID.len);
	BOOL ret = checkCalendarEvent(id, t.tv_sec);
	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_checkCalendarEvent ID=%s Status=%d Time: %d \n", id, ret, t.tv_sec  );

	free (id); // *LNI* to be removed when datatype will be changed

	RET->t	= BOOLtype;
	RET->p.BOOLvar = ret;
}
#endif // OBSOLETE

void iec_RAND_DINT(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	int max = in++->DINTvar;
	static BOOL _inited = FALSE;

	if	(!_inited) {
		srand (time(0));
		_inited = TRUE;
	}

	RET->t	= DINTtype;
	RET->p.DINTvar	= rand()%max;
}





/** NEW CALENDAR FUNCTIONS **/

void iec_newCalendarEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	DATE	DAYSTART = in++->DATEvar;
	TOD	START = in++->TODvar;
	TOD	END = in++->TODvar;

	char	*id = strndup((char *)ID.body, ID.len);
	newCalendarEvent (id, DAYSTART, START, END);
	free (id);

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_calendarEventExpires(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	DATE	DAY = in++->DATEvar;

	char	*id = strndup((char *)ID.body, ID.len);
	calendarEventExpires (id, DAY);
	free (id);

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_newCalendarDailyEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	TOD	START = in++->TODvar;
	TOD	END = in++->TODvar;
	DINT	interval = in++->DINTvar;

	char	*id = strndup((char *)ID.body, ID.len);
	newCalendarDailyEvent (id, START, END, interval);
	free (id);

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_newCalendarWeeklyEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	TOD	START = in++->TODvar;
	TOD	END = in++->TODvar;
	DINT	daysInWeek = in++->DINTvar;

	char	*id = strndup((char *)ID.body, ID.len);
	newCalendarWeeklyEvent (id, START, END, (int8_t)daysInWeek);
	free (id);

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

void iec_checkCalendarEvent(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	ID = in++->STRINGvar;
	//DT	t = in++->DTvar;

	char	*id = strndup((char *)ID.body, ID.len);
	//BOOL ret = checkCalendarEvent(id, t.tv_sec);
	BOOL ret = checkCalendarEvent(id, time(0));
	free (id);

	RET->t	= BOOLtype;
	RET->p.BOOLvar = ret;
}




/** WAVEGEN **/

#include <waveGen.c>

void WAVE_GENERATOR_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	float Average = in++->REALvar;
	float Amplitude = in++->REALvar;
	float Period = in++->REALvar;
	INT SigType = in++->INTvar;

	float x	= waveGen(Average, Amplitude, Period, SigType);
	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_WAVE_GENERATOR_function x=%f average=%f Amplitude=%f Period=%f Sig %d \n", x, Average, Amplitude, Period, SigType);
	RET->t		= REALtype;
	RET->p.REALvar	= x;
}

void SQUAREWAVE_GENERATOR_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	float Period = in++->REALvar;
	TIME START_TIME = in++->TIMEvar; // LNI needs to initialize START_TIME at fisrt time of an instance
                                        // Need to pass it to the squarewaveGen function
    
	BOOL x	= (BOOL)squarewaveGen(Period, &START_TIME);
	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_SQUAREWAVE_GENERATOR_function x=%d Period=%f Start_time=%ld \n", x, Period, START_TIME.tv_sec);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= x;
    RET	++;
	RET->t		= TIMEtype;
	RET->p.TIMEvar	= START_TIME;
}

#ifdef PWM_DEV
//void PWM_GENERATOR_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in);
#include "../timerFB/pwm.c"
#endif

extern iec_ctx_t *getCurrentContext();

/**
 * @brief Force the variable to be updated to the GSCL at the current TIC
 * @param varname STRING
 * @return TRUE
 */
void iec_forceUpdate(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	VARNAME = in++->STRINGvar;

	char	*varname = strndup((char *)VARNAME.body, VARNAME.len);

	iec_variable_t *v = getVariable(getCurrentContext(), varname);
	if	(v) {
		v->flags |= VARFLAG_CHANGED;
	}

	free (varname);

	RET->t	= BOOLtype;
	RET->p.BOOLvar = TRUE;
}

/**
 * @brief Returns the lastModifiedTime of a variable.
 * @param varname STRING
 * @return a DT
 */
void iec_lastModifiedTime(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	VARNAME = in++->STRINGvar;

	char	*varname = strndup((char *)VARNAME.body, VARNAME.len);
	iec_variable_t *v = getVariable(getCurrentContext(), varname);
	free (varname);

	DT dt_null = {0,0};
	RET->t	= DTtype;
	RET->p.DTvar = v ? v->lastModifiedTime : dt_null;
}

/**
 * @brief Returns the age of a variable.
 * @param varname STRING
 * @return The age of the variable (TIME)
 */
void iec_ageOfVariable(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	STRING	VARNAME = in++->STRINGvar;

	char	*varname = strndup((char *)VARNAME.body, VARNAME.len);
	iec_variable_t *v = getVariable(getCurrentContext(), varname);
	free (varname);

	DT dt_null = {0,0};
	if	(!v) {
		RET->t	= TIMEtype;
		RET->p.TIMEvar = dt_null;
		return;
	}
	TIME now;
	iec_gettimeofday (&now);
	SUB_TIMESPEC (now, v->lastModifiedTime);

	RET->t	= TIMEtype;
	RET->p.TIMEvar = now;
}

iec_fctinfos_t ActFunctions[] = {
#ifdef DEPRECATED
	{ "CurrentTime", CurrentTime, "TIME", 0 },
#endif
	{ "LocalDT", LocalDT, "DT", 0 },
	{ "sec", sec, "DINT", 0 },
	{ "min", min, "DINT", 0 },
	{ "hour", hour, "DINT", 0 },
	{ "mday", mday, "DINT", 0 },
	{ "mon", mon, "DINT", 0 },
	{ "year", year, "DINT", 0 },
	{ "wday", wday, "DINT", 0 },
	{ "yday", yday, "DINT", 0 },
	{ "isdst", isdst, "DINT", 0 },
	{ "gmtoff", gmtoff, "DINT", 0 },
	{ "tmzone", tmzone, "STRING", 0 },
	{ "setLatLon", setLatLon, "BOOL", 0 },
	{ "isDay", isDay, "BOOL", 0 },
	{ "sunsetTime", sunsetTime, "BOOL", 0 },
	{ "sunriseTime", sunriseTime, "BOOL", 0 },
#if 0 // OBSOLETE
	{ "newCalendarEvent", iec_newCalendarEvent, "BOOL", 0 },
	{ "newCalendarDailyEvent", iec_newCalendarDailyEvent, "BOOL", 0 },
	{ "newCalendarWeeklyEvent", iec_newCalendarWeeklyEvent, "BOOL", 0 },
	{ "calendarEventExpires", iec_calendarEventExpires, "BOOL", 0 },
	{ "checkCalendarEvent", iec_checkCalendarEvent, "BOOL", 0 },
#endif // OBSOLETE
	{ "newCalendarEvent", iec_newCalendarEvent, "BOOL", 0 },
	{ "newCalendarDailyEvent", iec_newCalendarDailyEvent, "BOOL", 0 },
	{ "newCalendarWeeklyEvent", iec_newCalendarWeeklyEvent, "BOOL", 0 },
	{ "calendarEventExpires", iec_calendarEventExpires, "BOOL", 0 },
	{ "checkCalendarEvent", iec_checkCalendarEvent, "BOOL", 0 },

	{ "RAND_DINT", iec_RAND_DINT, "INT", 0 },
	{ "WAVE_GENERATOR_function", WAVE_GENERATOR_function, "REAL", 0 },
	{ "SQUAREWAVE_GENERATOR_function", SQUAREWAVE_GENERATOR_function, "BOOL", 0 },
#ifdef PWM_DEV
	{ "PWM_GENERATOR_function", PWM_GENERATOR_function, "BOOL", 0 },
#endif
	{ "forceUpdate", iec_forceUpdate, "BOOL", 0 },
	{ "lastModifiedTime", iec_lastModifiedTime, "DT", 0 },
	{ "ageOfVariable", iec_ageOfVariable, "TIME", 0 },
	{ NULL, NULL, NULL }
};

void Actility_init() {
	iec_addFuncs (ActFunctions);
}
