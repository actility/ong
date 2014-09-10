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
#include <unistd.h>
#include <sys/time.h>
#define __USE_GNU
#include <math.h>

#ifdef MAIN
typedef struct {
	long int tv_sec;
	long int tv_nsec;
} IEC_TIME;

void iec_gettimeofday(IEC_TIME *iec_time) {
	struct timeval tv;
	gettimeofday (&tv, NULL);
	iec_time->tv_sec	= tv.tv_sec;
	iec_time->tv_nsec	= tv.tv_usec*1000;
}

#endif

IEC_TIME	START_TIME = {0,0}; // *LNI* not re-entrant. If used more than one time, potential bug 

float waveGen(float Average, float Amplitude, float Period, int SigType) {
	if	(START_TIME.tv_sec == 0)
		iec_gettimeofday (&START_TIME);
	IEC_TIME	CURRENT_TIME;
	iec_gettimeofday (&CURRENT_TIME);
	float	x = 0;
	float freq = 1/Period;
	float secs = (float)CURRENT_TIME.tv_nsec/1000000000.0;
	secs	+= (float)(CURRENT_TIME.tv_sec - START_TIME.tv_sec);

	float t	= secs * freq;

	switch (SigType) {
	case 0 :	// constant
	default:
		x	= 0;
		break;
	case 1 :	// sine wave
		x = sinf(2*M_PI*t);
		break;
	case 2 :	// square wave
		x = sinf(2*M_PI*t);
		x = (x >= 0) ? 1 : -1;
		break;
	case 3 :	// triangle wave
		//x = 2 * fabs(t - 2*floorf(t/2) - 1) - 1;
		x = 1 - 4*fabs(roundf(t-0.25)-(t-0.25));
		break;
	case 4 :	// sawTooth wave
		x = 2*(t-floorf(t+0.5));
		break;
	case 5 :	// log wave
		break;
	}

	x	= x*Amplitude/2 + Average;
	return	x;
}

int squarewaveGen(float Period, IEC_TIME *start_time) {
	if	(start_time->tv_sec == 0)
		iec_gettimeofday (start_time);
	IEC_TIME	CURRENT_TIME;
	iec_gettimeofday (&CURRENT_TIME);
	float	x = 0;
	float freq = 1/Period;
	float secs = (float)CURRENT_TIME.tv_nsec/1000000000.0;
	secs	+= (float)(CURRENT_TIME.tv_sec - start_time->tv_sec);
    
	float t	= secs * freq;
    
    x = 1 - 4*fabs(roundf(t-0.25)-(t-0.25));
    //         printf ("current_time %ld t= %f x= %f val=%d \n", CURRENT_TIME.tv_sec,t, x ,((x >= 0) ? 1 : 0));
    return	(x>=0 ? 1:0);
}

#ifdef MAIN

float g_Average = 0;
float g_Amplitude = 2;
float g_Period = 10;
float x;
int i;

int main(int ac, char **av) {
	int type = ac >= 2 ? atoi(av[1]) : 1;


	printf ("Average %f Amplitude %f Period %f\n", g_Average, g_Amplitude, g_Period);

	while (1) {
        switch (type){
        
// TODO to be removed
            case 6:
                i= squarewaveGen (g_Period, &START_TIME);
                printf ("%d\n", i);
                break;
            default:
                x = waveGen(g_Average, g_Amplitude, g_Period, type);
                printf ("%f\n", x);
                break;
        }
#if 0
        printf ("%f\n", x);
#endif
		usleep (100000);
		//sleep (1);
	}
	return	0;
}
#endif
