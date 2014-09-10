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
#include <math.h>
#include <time.h>
#include <string.h>
#include <sys/select.h>

#include <rtlbase.h>
#include <rtllist.h>

#ifndef PLC_FBD
#include <iec_types.h>
#include <param_u.h>
#include <literals.h>
#endif

#define BIT(X)	(1<<(X))
#define BIT_IS_SET(OCT,X) (OCT & BIT(X))

#if !defined(FALSE)
typedef int8_t BOOL;
#define FALSE (BOOL)0
#define TRUE (BOOL)1
#endif


typedef struct {
	time_t	t;
	struct tm tm;
} time_ex_t;

typedef enum {
	INACTIVE, ACTIVE
} cal_status_t;


#define Sunday 0x1
#define Monday (Sunday<<1)
#define Tuesday (Sunday<<2)
#define Wednesday (Sunday<<3)
#define Thursday (Sunday<<4)
#define Friday (Sunday<<5)
#define Saturday (Sunday<<6)
#define	ALL_DAYS	0xff

#define SECINDAY	86400

#if !defined(CALENDAR_TYPE)
#define CALENDAR_TYPE_LIMITED
#endif

#ifdef CALENDAR_TYPE_LIMITED

#define CALENDAR_MAX_ID_SIZE 40 // *LNI* max size of ID when passed from IEC shall be checked

typedef enum {
	NONE, DAILY, WEEKLY
} cal_freq_t;

typedef struct {
	cal_freq_t	frequency;
	uint8_t		interval;
	time_ex_t	expires;
	int8_t		daysInWeek;
} cal_repeatRule_t;

typedef struct {
	struct list_head head;
	char	id[CALENDAR_MAX_ID_SIZE];
	time_ex_t	originalStartTime;
	time_ex_t	start;
	time_ex_t	end;
	cal_status_t	status;
	cal_repeatRule_t recurrence;
} calevent_t;

#endif

#ifdef CALENDAR_TYPE_EXTENDED

typedef enum {
	NONE, DAILY, WEEKLY, MONTHLY, YEARLY
} cal_freq_t;

typedef struct {
	cal_freq_t	frequency;
	int32_t		interval;
	time_ex_t	expires;
	time_ex_t	exceptionDates[10];
	int8_t		daysInWeek;
} cal_repeatRule_t;

typedef struct {
	struct list_head head;
	char	id[20];
	char	description[80];
	char	location[80];
	char	summary[80];
	time_ex_t	originalStartTime;
	time_ex_t	start;
	time_ex_t	end;
	cal_status_t	status;
	cal_repeatRule_t recurrence;
} calevent_t;

#endif



//================ EVENT API =====================

static int findFirstDay(time_t now, int8_t daysInWeek) {
	struct tm *tm = localtime(&now);
	int	i;

	for	(i=0; i<7; i++) {
		if	(BIT_IS_SET (daysInWeek, tm->tm_wday))
			return	i;
		tm->tm_wday	++;
		if	(tm->tm_wday == 7)
			tm->tm_wday	= 0;
	}
	return	-1;
}

static int findNextDay(time_t now, int8_t daysInWeek) {
	struct tm *tm = localtime(&now);
	int	i;

	tm->tm_wday ++;
	if	(tm->tm_wday == 7)
		tm->tm_wday	= 0;

	for	(i=0; i<7; i++) {
		if	(BIT_IS_SET (daysInWeek, tm->tm_wday))
			return	i+1;
		tm->tm_wday	++;
		if	(tm->tm_wday == 7)
			tm->tm_wday	= 0;
	}
	return	-1;
}

/**
 * @brief checks if an Event is ACTIVE or not for a given time value
 * @param calendar event structure
 * @param time
 * @return The result is TRUE for ACTIVE or FALSE if INACTIVE
 */

BOOL checkEvent(calevent_t *cal, time_t t) {
	if	(cal->status == INACTIVE)
		return	FALSE;
/*
	printf ("checkEvent %s\n", cal->id);
	struct tm tm;
	tm = *localtime(&cal->start.t);
        printf("  start             %ld %s", cal->start.t, asctime(&tm));
	tm = *localtime(&cal->end.t);
        printf("  end               %ld %s", cal->end.t, asctime(&tm));
*/
	switch (cal->recurrence.frequency) {
	case	NONE :
		if	(cal->start.t == cal->end.t) {
			if	(t>=cal->end.t) {
				cal->status	= INACTIVE;
				return	TRUE;
			}
			return	FALSE;
		}
		else {
			if	(t>=cal->end.t) {
				cal->status	= INACTIVE;
				return	FALSE;
			}
			return	(t>=cal->start.t);
		}
		break;
	case	DAILY :
		if	(cal->recurrence.expires.t && (t>=cal->recurrence.expires.t)) {
			cal->status = INACTIVE;
			return FALSE;
		}
		if	(cal->start.t == cal->end.t) {
			if	(t>=cal->end.t) {
				// calculate next
				cal->start.t	+= (SECINDAY*cal->recurrence.interval);
				cal->start.tm	= *localtime(&cal->start.t);
				cal->end.t	+= (SECINDAY*cal->recurrence.interval);
				cal->end.tm	= *localtime(&cal->end.t);
				return	TRUE;
			}
			return	FALSE;
		}
		else {
			if	(t>=cal->end.t) {
				// calculate next
				cal->start.t	+= (SECINDAY*cal->recurrence.interval);
				cal->start.tm	= *localtime(&cal->start.t);
				cal->end.t	+= (SECINDAY*cal->recurrence.interval);
				cal->end.tm	= *localtime(&cal->end.t);
				return	FALSE;
			}
			return	(t>=cal->start.t);
		}
		break;
	case	WEEKLY :
		if	(cal->recurrence.expires.t && (t>=cal->recurrence.expires.t)) {
			cal->status = INACTIVE;
			return FALSE;
		}
		if	(cal->start.t == cal->end.t) {
			if	(t>=cal->end.t) {
				// calculate next
				int day = findNextDay(t, cal->recurrence.daysInWeek);
				//printf ("Weekly event is %d days after\n", day);
				if	(day < 0) {
					cal->status	= INACTIVE;
					return	FALSE;
				}
				cal->start.t	+= day*SECINDAY;
				cal->start.tm	= *localtime(&cal->start.t);
				cal->end.t	+= day*SECINDAY;
				cal->end.tm	= *localtime(&cal->end.t);
				return	TRUE;
			}
			return	FALSE;
		}
		else {
#if 0
			if	(t>=cal->end.t) {
				// calculate next
				int day = findNextDay(t, cal->recurrence.daysInWeek);
				printf ("Weekly event is %d days after\n", day);
				if	(day < 0) {
					cal->status	= INACTIVE;
					return	FALSE;
				}
				cal->start.t	+= day*SECINDAY;
				cal->start.tm	= *localtime(&cal->start.t);
				cal->end.t	+= day*SECINDAY;
				cal->end.tm	= *localtime(&cal->end.t);
				return	FALSE;
			}
#else
			while	(t>=cal->end.t) {
				// calculate next
				int day = findNextDay(cal->start.t, cal->recurrence.daysInWeek);
				//printf ("Weekly event is %d days after\n", day);
				if	(day < 0) {
					cal->status	= INACTIVE;
					return	FALSE;
				}
				cal->start.t	+= day*SECINDAY;
				cal->start.tm	= *localtime(&cal->start.t);
				cal->end.t	+= day*SECINDAY;
				cal->end.tm	= *localtime(&cal->end.t);
				return	FALSE;
			}
#endif
			return	(t>=cal->start.t);
		}
		break;
	default :
		break;
	}
	return	FALSE;
}

/*
 * Un evenement recurrent est un template. Il cree des evenements unitaires.
 * http://googleappsdeveloper.blogspot.fr/2011/12/calendar-v3-best-practices-recurring.html
 * http://tools.ietf.org/html/rfc5545
 * http://www.w3.org/TR/calendar-api
*/


//================ CALENDAR API =====================

// TODO : move _eventList to iec_user_t structure
static struct list_head _eventList;
static BOOL _eventList_initialized = FALSE;

static inline void initCalendarList() {
	if	(!_eventList_initialized) {
		INIT_LIST_HEAD (&_eventList);
		_eventList_initialized = TRUE;
	}
}

#define DEV_LAURENT

#ifdef DEV_LAURENT
static void dumpEvents() {
	struct list_head *elem;
    
	list_for_each (elem, &_eventList)
    {
		calevent_t *cal	= list_entry(elem, calevent_t, head);
		printf("Calendar found: %s\n",cal->id);
/*
        printf("STRUCTURES Size \n");
        printf("- calevent_t        %d\n", sizeof(calevent_t));
        printf("- time_ex_t         %d\n", sizeof(time_ex_t));
        printf("- cal_status_t      %d\n", sizeof(cal_status_t));
        printf("- cal_freq_t        %d\n", sizeof(cal_freq_t));
        printf("- cal_repeatRule_t  %d\n", sizeof(cal_repeatRule_t));
        printf("PARAMETERS\n");
*/
	struct tm tm;
	tm = *localtime(&cal->start.t);
        printf("  start             %ld %s", cal->start.t, asctime(&tm));
	tm = *localtime(&cal->end.t);
        printf("  end               %ld %s", cal->end.t, asctime(&tm));
    }
}
#endif

static calevent_t *_getEvent(char *id) {
	struct list_head *elem;
	list_for_each (elem, &_eventList) {
		calevent_t *cal	= list_entry(elem, calevent_t, head);
		if	(!strcmp(cal->id, id))
			return	cal;
	}
	return	NULL;
}

void freeCalendarEvent(char *id) {
	initCalendarList();
	calevent_t *cal = _getEvent(id);
	if	(cal && !strcmp(cal->id, id)) {
		list_del (&cal->head);
		free (cal);
	}
}

BOOL checkCalendarEvent(char *id, time_t t) {
	initCalendarList();
	calevent_t *cal = _getEvent(id);
	if	(cal && !strcmp(cal->id, id))
		return	checkEvent(cal, t);
	return	FALSE;
}

BOOL isStillActiveEvents() {
	struct list_head *elem;
	list_for_each (elem, &_eventList) {
		calevent_t *cal	= list_entry(elem, calevent_t, head);
		if (cal->status == ACTIVE)
			return	TRUE;
	}
	return	FALSE;
}

calevent_t *newEvent(char *id, IEC_DATE dayStart, IEC_TOD start, IEC_TOD end) {
	calevent_t	*cal = calloc(sizeof(calevent_t), 1);
	if	(!cal)
		return	NULL;

	strcpy (cal->id, id);
	cal->recurrence.frequency	= NONE;
	cal->status	= ACTIVE;

	//	Calculate the start
	cal->start.t	= dayStart.tv_sec + start.tv_sec;
	cal->start.tm	= *localtime(&cal->start.t);
	cal->originalStartTime = cal->start;

	//	Calculate the end
	cal->end.t	= dayStart.tv_sec + end.tv_sec;
	cal->end.tm	= *localtime(&cal->end.t);

	return cal;
}

void newCalendarEvent(char *id, IEC_DATE daystart, IEC_TOD start, IEC_TOD end) {
	initCalendarList();
	if	(_getEvent(id))
		return;
	calevent_t *cal = newEvent(id, daystart, start, end);
	if	(cal) {
		freeCalendarEvent(id);
		list_add_tail (&cal->head, &_eventList);
	}
}

void setExpires(calevent_t *cal, time_t t) {
	cal->recurrence.expires.tm = *localtime(&t);
	cal->recurrence.expires.t	= t;
}

void calendarEventExpires(char *id, IEC_DATE date) {
	calevent_t *cal = _getEvent(id);
	if	(cal && !strcmp(cal->id, id)) {
		setExpires(cal, date.tv_sec);
	}
}

calevent_t *newDailyEvent(char *id, IEC_TOD start, IEC_TOD end, uint8_t interval) {
	calevent_t	*cal = calloc(sizeof(calevent_t), 1);
	if	(!cal)
		return	NULL;

	strcpy (cal->id, id);
	cal->recurrence.frequency	= DAILY;
	cal->recurrence.interval	= interval > 0 ? interval : 1;
	cal->status	= ACTIVE;

	//	Calculate epoch at 00h00
	time_t now = time(0);
	time_t h0 = now;
	struct tm *tm	= localtime(&h0);
	h0	-= ((tm->tm_hour * 60) + tm->tm_min)*60 + tm->tm_sec;

	//	Calculate the start
	cal->start.t	= h0 + start.tv_sec;

	//	Calculate the end
	cal->end.t	= h0 + end.tv_sec;

	if	((cal->start.t < now) && (cal->end.t < now)) {
		cal->start.t += SECINDAY;
		cal->end.t += SECINDAY;
	}

	cal->start.tm	= *localtime(&cal->start.t);
	cal->end.tm	= *localtime(&cal->end.t);

	cal->originalStartTime = cal->start;
/*
	printf ("Start : %s", ctime(&cal->start.t));
	printf ("End   : %s", ctime(&cal->end.t));
*/
	return cal;
}

void newCalendarDailyEvent(char *id, IEC_TOD start, IEC_TOD end, uint8_t interval) {
	initCalendarList();
	if	(_getEvent(id))
		return;
	calevent_t *cal = newDailyEvent(id, start, end, interval);
	if	(cal) {
		freeCalendarEvent(id);
		list_add_tail (&cal->head, &_eventList);
	}
}

void setWeeklyEvent(calevent_t *cal, IEC_TOD start, IEC_TOD end, int8_t daysInWeek) {
	//	Calculate epoch at 00h00
	time_t now = time(0);
	time_t h0 = now;
	struct tm *tm	= localtime(&h0);
	h0	-= ((tm->tm_hour * 60) + tm->tm_min)*60 + tm->tm_sec;

	int day = findFirstDay(now, daysInWeek);
	//printf ("Weekly event is %d days after\n", day);
	if	(day < 0) {
		cal->status	= INACTIVE;
		return;
	}

	//	Calculate the start
	cal->start.t	= h0 + start.tv_sec;
	cal->start.t	+= day*SECINDAY;
	cal->start.tm	= *localtime(&cal->start.t);
	cal->originalStartTime = cal->start;

	//	Calculate the end
	cal->end.t	= h0 + end.tv_sec;
	cal->end.t	+= day*SECINDAY;
	cal->end.tm	= *localtime(&cal->start.t);

	//	next day ?
	if	(end.tv_sec < start.tv_sec) {
		cal->end.t	+= SECINDAY;
	}
	cal->end.tm	= *localtime(&cal->end.t);
	cal->status	= ACTIVE;
}

calevent_t *newWeeklyEvent(char *id, IEC_TOD start, IEC_TOD end, int8_t daysInWeek) {
	calevent_t	*cal = calloc(sizeof(calevent_t), 1);
	if	(!cal)
		return	NULL;

	strcpy (cal->id, id);
	cal->recurrence.frequency	= WEEKLY;
	cal->recurrence.interval	= 1;
	cal->recurrence.daysInWeek	= daysInWeek;

	setWeeklyEvent (cal, start, end, daysInWeek);
	return cal;
}

void newCalendarWeeklyEvent(char *id, IEC_TOD start, IEC_TOD end, int8_t daysInWeek) {
	calevent_t *cal;
	initCalendarList();
	cal = _getEvent(id);
	if	(cal) {
		setWeeklyEvent (cal, start, end, daysInWeek);
		return;
	}
	cal = newWeeklyEvent(id, start, end, daysInWeek);
	if	(cal) {
		freeCalendarEvent(id);
		list_add_tail (&cal->head, &_eventList);
	}
}




time_t todayAt (char *theTime) {
	time_t now	= time(0);
	struct tm tm;
	char tmp[20];
	char *el, *pt;

	tm	= *localtime(&now);

	strcpy (tmp, theTime);
	el	= tmp;
	pt	= strchr(el, ':');
	*pt = 0;
	tm.tm_hour	= atoi(el);

	el	= pt+1;
	pt	= strchr(el, ':');
	*pt = 0;
	tm.tm_min	= atoi(el);

	el	= pt+1;
	tm.tm_sec	= atoi(el);

	return mktime(&tm);
}

time_t tomorrowAt (char *theTime) {
	return todayAt (theTime) + SECINDAY;
}





#ifdef MAIN


#include "unitTest.h"

#define FAIL 1
#define SUCCESS 0

int do_test() {
	struct tm tm;
	time_t now;
	BOOL ret;
	IEC_DT dt;

	IEC_DATE dayStart = getDate("D#2013-07-29");
	IEC_TOD start = getTimeOfDay("TOD#14:00:00");
	IEC_TOD end   = getTimeOfDay("TOD#20:00:00");
	newCalendarEvent("onetime", dayStart, start, end);

	dt = getDateAndTime("DT#2013-07-29-17:00:00");
	now = dt.tv_sec;
	ret = checkCalendarEvent("onetime", now);
	STAssertTrue(ret, "Test #1 : inside single event");

	dt = getDateAndTime("DT#2013-08-01-17:00:00");
	now = dt.tv_sec;
	ret = checkCalendarEvent("onetime", now);
	STAssertFalse(ret, "Test #2 : outside single event");

	start = getTimeOfDay("TOD#08:00:00");
	end   = getTimeOfDay("TOD#20:00:00");
	newCalendarDailyEvent("daily", start, end, 1);

	now	= todayAt ("10:00:00");
	ret = checkCalendarEvent("daily", now);
	STAssertTrue(ret, "Test #3 : inside daily event");

	now	= todayAt ("21:00:00");
	ret = checkCalendarEvent("daily", now);
	STAssertFalse(ret, "Test #4 : outside daily event");

	now	= tomorrowAt ("10:00:00");
	ret = checkCalendarEvent("daily", now);
	STAssertTrue(ret, "Test #5 : tomorrow and inside daily event");

	now	= tomorrowAt ("21:00:00");
	ret = checkCalendarEvent("daily", now);
	STAssertFalse(ret, "Test #6 : tomorrow and outside daily event");

	start = getTimeOfDay("TOD#10:00:00");
	end   = getTimeOfDay("TOD#18:00:00");
	newCalendarWeeklyEvent("weekly", start, end, Sunday);

	now	= todayAt ("14:00:00");
	tm	= *localtime(&now);
	now += (7 - tm.tm_wday)*SECINDAY;	// goto next sunday

	ret = checkCalendarEvent("weekly", now);
	STAssertTrue(ret, "Test #7 : sunday inside weekly event");

	now	+= SECINDAY;	// goto next monday
	ret = checkCalendarEvent("weekly", now);
	STAssertFalse(ret, "Test #8 : monday outside weekly event");

	now	+= (6*SECINDAY);	// goto next sunday
	ret = checkCalendarEvent("weekly", now);
	STAssertTrue(ret, "Test #9 : next sunday inside weekly event");

	// Checking update
	start = getTimeOfDay("TOD#15:00:00");
	end   = getTimeOfDay("TOD#20:00:00");
	newCalendarWeeklyEvent("weekly", start, end, Sunday|Saturday);

	now	= todayAt ("17:00:00");
	tm	= *localtime(&now);
	now	+= (6 - tm.tm_wday)*SECINDAY;	// goto next saturday
	ret = checkCalendarEvent("weekly", now);
	STAssertTrue(ret, "Test #10 : saturday inside weekly event (updated)");

	STStatistics();

	if	(STErrorCount() != 0)
		return FAIL;
	return	SUCCESS;
}

int main(int ac, char **av) {
	struct tm *tm;
	time_t now;
	char day_display[5];

	if	(ac >= 2 && !strcmp(av[1], "-test")) {
		int rc = do_test ();
		//printf ("do_test %s\n", rc ? "failed" : "succeed");
		exit (rc);
	}
	//	Calendar API
#define TEST_END_DATE "20130703"

	IEC_DATE dayStart = getDate("D#2013-07-29");
	IEC_TOD start = getTimeOfDay("TOD#14:00:00");
	IEC_TOD end   = getTimeOfDay("TOD#15:00:00");
	newCalendarEvent("onetime", dayStart, start, end);

	start = getTimeOfDay("TOD#10:00:00");
	end   = getTimeOfDay("TOD#12:00:00");
	newCalendarDailyEvent("daily", start, end, 1);

	start = getTimeOfDay("TOD#11:00:00");
	end   = getTimeOfDay("TOD#12:00:00");
	newCalendarWeeklyEvent("weekly", start, end, Sunday);

	// Checking update
	start = getTimeOfDay("TOD#15:00:00");
	end   = getTimeOfDay("TOD#18:00:00");
	newCalendarWeeklyEvent("weekly", start, end, Sunday|Saturday);

	dumpEvents();
	
	now = time(0);
	while (isStillActiveEvents()) {
		tm = localtime(&now);
		switch (tm->tm_wday) {
			case 0: strcpy (day_display, "Sun"); break;
			case 1: strcpy (day_display, "Mon"); break;
			case 2: strcpy (day_display, "Tue"); break;
			case 3: strcpy (day_display, "Wed"); break;
			case 4: strcpy (day_display, "Thu"); break;
			case 5: strcpy (day_display, "Fri"); break;
			case 6: strcpy (day_display, "Sat"); break;
			default: strcpy (day_display, "Err"); break;
		}
		printf ("Check for: %s %02d/%02d/%04d %02d:%02d\n",
				day_display, tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900, tm->tm_hour, tm->tm_min);
	 
		BOOL ret = checkCalendarEvent("onetime", now);
		if	(ret == TRUE)
			printf ("%02d/%02d %02d:%02d is%s %s\n",
				tm->tm_mday, tm->tm_mon+1, tm->tm_hour, tm->tm_min, ret ? "":" not", "onetime");

		ret = checkCalendarEvent("daily", now);
		if	(ret == TRUE)
			printf ("%02d/%02d %02d:%02d is%s %s\n",
				tm->tm_mday, tm->tm_mon+1, tm->tm_hour, tm->tm_min, ret ? "":" not", "daily");

		ret = checkCalendarEvent("weekly", now);
		if	(ret == TRUE)
			printf ("%02d/%02d %02d:%02d is%s %s\n",
				tm->tm_mday, tm->tm_mon+1, tm->tm_hour, tm->tm_min, ret ? "":" not", "weekly");

		now	+= 3600 ; // checks 1 hour after
	}
	return 0;
}
#endif
