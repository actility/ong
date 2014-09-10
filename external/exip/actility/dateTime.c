
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "procTypes.h"

/**
 * @brief Converts an ISO date like 2012-12-14T15:00:17.717Z
 *                                  012345678901234567890123
 * @param iso An ISO date string
 * @param dt The resulting IEC DT
 * @return void
 */

#define BASE_YEAR 2000

void iso8601_to_EXIP(char *iso, EXIPDateTime *dt) {
	if	(!iso || strlen(iso) < 17)
		return;

	memset (dt, 0, sizeof(EXIPDateTime));

	dt->presenceMask = YEAR_PRESENCE | MON_PRESENCE | MDAY_PRESENCE | HOUR_PRESENCE | MIN_PRESENCE | SEC_PRESENCE;
	dt->dateTime.tm_year	= (iso[0]-'0')*1000 + (iso[1]-'0')*100 + (iso[2]-'0')*10 + (iso[3]-'0') - BASE_YEAR;
	dt->dateTime.tm_mon	= (iso[5]-'0')*10 + (iso[6]-'0') - 1;
	dt->dateTime.tm_mday	= (iso[8]-'0')*10 + (iso[9]-'0');
	dt->dateTime.tm_hour	= (iso[11]-'0')*10 + (iso[12]-'0');
	dt->dateTime.tm_min	= (iso[14]-'0')*10 + (iso[15]-'0');
	dt->dateTime.tm_sec	= (iso[17]-'0')*10 + (iso[18]-'0');

	int type;
	char *pt = &iso[19];
	if	(!*pt)
		return;

	if	(*pt == '.') {
		char *frac = ++pt;
		while (isdigit(*pt)) {
			pt ++;
		}
		type = *pt;
		*pt = 0;
		pt ++;
		dt->fSecs.value = atoi(frac);
		dt->fSecs.offset = strlen(frac) - 1;
		dt->presenceMask |= FRACT_PRESENCE;
	}
	else {
		type = *pt++;
	}

	if	(type == '+' || type == '-') {
		int offset;
		int sign = (type == '-') ? -1 : 1;

		dt->TimeZone	= (pt[0]-'0')*10 + (pt[1]-'0');
		dt->TimeZone	<<= 6;
		dt->TimeZone	+= (pt[3]-'0')*10 + (pt[4]-'0');
		dt->TimeZone	*= sign;
		dt->presenceMask |= TZONE_PRESENCE;
	}
}


void EXIP_to_iso8601(EXIPDateTime *dt, char *avalue) {
	*avalue = 0;
	if	(dt->presenceMask & YEAR_PRESENCE) {
		sprintf (avalue+strlen(avalue), "%04d-%02d-%02d",
			dt->dateTime.tm_year + BASE_YEAR,
			dt->dateTime.tm_mon + 1,
			dt->dateTime.tm_mday);
	}
	if	(dt->presenceMask & HOUR_PRESENCE) {
		if	(dt->presenceMask & FRACT_PRESENCE) {
			double sec = dt->dateTime.tm_sec;
			sec += (double)dt->fSecs.value * pow(10, -(dt->fSecs.offset + 1));
			sprintf (avalue+strlen(avalue), "T%02d:%02d:%05.3f",
				dt->dateTime.tm_hour,
				dt->dateTime.tm_min, sec);
		}
		else {
			sprintf (avalue+strlen(avalue), "T%02d:%02d:%02d",
				dt->dateTime.tm_hour,
				dt->dateTime.tm_min, dt->dateTime.tm_sec);
		}
	}
	if	(dt->presenceMask & TZONE_PRESENCE) {
		int h, m, tz;
		char sign;

		sign	= dt->TimeZone >= 0 ? '+' : '-';
		tz	= abs(dt->TimeZone);
		h	= tz >> 6;
		m	= tz & 0x3f;
		sprintf (avalue+strlen(avalue), "%c%02d:%02d", sign, h, m);
	}
	else {
		strcat (avalue, "Z");
	}
}

char *tests[] = {
	"2013-03-10T09:30:15",
	"2013-03-10T09:30:15.32",
	"2013-03-10T09:30:15+06:00",
	"2013-03-10T09:30:15-06:00",
	"2012-12-14T15:00:17.717Z",
	"2012-10-01T22:00:00+02:00",
	NULL
};

int main() {
	int i;
	EXIPDateTime dt;
	char out[100], *in;

	for (i=0; tests[i]; i++) {
		in = strdup(tests[i]);
		iso8601_to_EXIP (in, &dt);
		free (in);
		EXIP_to_iso8601 (&dt, out);
		printf ("I: %s\nO: %s\n", tests[i], out);
	}
}
