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
#include <stdlib.h>
#include <sys/time.h>

#include <rtlbase.h>

#include "plc.h"
#include "literals.h"
#include "timeoper.h"
#include "iec_debug.h"

#ifndef MAIN
extern void iec_gettimeofday(IEC_TIMESPEC *iec_time);
extern void iec_getMonotonicTime(IEC_TIMESPEC *iec_time);
#endif

#ifdef MAIN
void iec_gettimeofday(TIME *iec_time) {
	struct timeval tv;
	gettimeofday (&tv, NULL);
	iec_time->tv_sec	= tv.tv_sec;
	iec_time->tv_nsec	= tv.tv_usec*1000;
}

void iec_getMonotonicTime(TIME *iec_time) {
#ifdef _POSIX_TIMERS
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	iec_time->tv_sec	= ts.tv_sec;
	iec_time->tv_nsec	= ts.tv_nsec;
#else
	iec_gettimeofday (iec_time);
#endif
}

#endif

/*
 (****************************************************************
 
 PWM - pulse width modulation
 
 Precision: 1 second
 
 Input Parameters
 - Start:       IEC type: BOOL      the rising Edge of that Boolean will start the PWM
                                    the falling Edge will reset it depending on the ResetMode parameter
 - ResetMode:   IEC type: DINT      0: On falling edge of Start, PWM will be stopped and counter resetted
                                    1: On falling edge of Start, PWM will be suspended, Output is FALSE
                                       but counter value keept for faster restart
 - Period:      IEC type: TIME      the period in Seconds of the PWM
 - Ratio:       IEC type: DINT      the Ratio, generaly from 0% to 100% for setting the ON time
 - Limit_High:  IEC type: DINT      The High Limit of the Ratio. Is from Limit Limit_Low to 100%
 - Limit_Low:   IEC type: DINT      The Low Limit of the Ratio. Is from 0% to Limit_High
        If Limit_Low > Limit_High, the function block will be Stopped and Resetted
 
 Output Value
 - PWM_Out:     IEC type: BOOL      the result of the PWM
 
 Internal variable IEC Persistant
 - Started:     IEC type: BOOL      Indicates if the PWM has been started
 - Start_PREV   IEC type: BOOL      memory of the previous Start Input for edge detection
 - Start_Time   IEC type: TIME      memory of the current cycle StartTime
 - State        IEC type: SINT      0: not started, resetted
                                    1: started, cycle not finished
                                    2: started, cycle finished
 - Cycle_cnt    IEC type: DINT      Cycle counter in order to have a very precise time stamp
 
 Internal variable (not IEC related)
 - Current_Time
 
 If Limit_High or Limit_Low is changed while a cycle has started, it will be taken in account instantly. The Output will also be updated accordingly if the current ON time within the cycle is out the new range
 If the ratio is changed while a cycle has started, it will be taken in account instantly. The Output will also be updated accordingly if the current ON time within the cycle is terminated
 */

#define PWM_RATIO_MIN 0
#define PWM_RATIO_MAX 100

void PWM_GENERATOR_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
    // INPUT parameters
	BOOL	START = in++->BOOLvar;
	DINT 	RESETMODE = in++->DINTvar;
	TIME	PERIOD = in++->TIMEvar;
	DINT 	RATIO = in++->DINTvar;
	DINT 	LIMIT_HIGH = in++->DINTvar;
	DINT 	LIMIT_LOW  = in++->DINTvar;
    // INTERNAL persistent variables
	BOOL	STARTED  = in++->BOOLvar;
	BOOL	START_PREV = in++->BOOLvar;
	TIME	START_TIME = in++->TIMEvar;
    SINT	STATE  = in++->SINTvar;
    DINT	CYCLE_CNT = in++->DINTvar;
    // OUTPUT parameters
	BOOL	PWM_OUT = FALSE;            // PWM_OUT is not persistent    
    // Local internal variable
	TIME        CURRENT_TIME;
    time_t      period_next;
    time_t      PWM_off_next;
    
    iec_getMonotonicTime(&CURRENT_TIME);

	RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [ENTER] START=%d START_PREV=%d STARTED=%d STATE=%d CYCLE_CNT=%ld\n", START, START_PREV, STARTED, STATE, CYCLE_CNT );
    // check limits
    if (LIMIT_HIGH > PWM_RATIO_MAX)   LIMIT_HIGH = PWM_RATIO_MAX;
    if (LIMIT_LOW < PWM_RATIO_MIN)    LIMIT_LOW = PWM_RATIO_MIN;
	
    RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [ENTER] Limit_High=%d Limit_Low=%d Ratio=%d Period=%d\n", LIMIT_HIGH, LIMIT_LOW, RATIO, PERIOD.tv_sec);
    
    // shall the PWM start ?
	if	((STATE == 0) && !START_PREV && START) {
		RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [Inside] Just started STATE=0\n");
        
		CYCLE_CNT = 0;
        START_TIME.tv_sec = CURRENT_TIME.tv_sec; // store starting time (seconds only)
		STATE	= 1;
        STARTED = TRUE;
    } // STATE = 0
	if	((STARTED == TRUE) && (STATE !=0)) {
		RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [Inside] started STATE=%d\n",STATE);
        // PWM has started and now we can calculate and check the various counters. When period is finished, then we can increment the Cycle number
        period_next = (time_t)START_TIME.tv_sec + (time_t)PERIOD.tv_sec * (CYCLE_CNT+1); // begin time of next period
        PWM_off_next = (time_t)START_TIME.tv_sec + ((time_t)PERIOD.tv_sec * CYCLE_CNT) + (RATIO * PERIOD.tv_sec / 100);
		PWM_OUT = TRUE; // lets put PWM_OUT TRUE until we know if it needs to go off
        
        // Now we know the time for switching off the PWM output, and the start time for the next period
        // STATE is now showing STARTED, cycle not finished

        // Checks if PWM_OUT needs to go OFF. If yes, State can go to level 2
        if (CURRENT_TIME.tv_sec >= PWM_off_next) {
            PWM_OUT = FALSE;
            STATE =2; // cycle finished
            RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [Inside] PWM needs to go OFF\n");
        }

        // Checks if Period finished. If yes increment Cycle counter and State goes to 1
        if (CURRENT_TIME.tv_sec >= period_next) {
            STATE =1; // cycle can restart
            CYCLE_CNT++;
           // calculate PWM_off_next immediatly without waiting for next PLC cycle
           PWM_off_next = (time_t)START_TIME.tv_sec + ((time_t)PERIOD.tv_sec * CYCLE_CNT) + (RATIO * PERIOD.tv_sec / 100);
            iec_getMonotonicTime(&CURRENT_TIME); // update current time now. For Ratio values close to 0, might be already too late to switch ON
            if (CURRENT_TIME.tv_sec < PWM_off_next){
                PWM_OUT = TRUE; // put PWM_OUT immediatly ON again in order to avoid glitch on the output.
            }
            RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [Inside] New cycle can start next time\n");
        }

        RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [Inside1] START=%d START_PREV=%d STARTED=%d STATE=%d CYCLE_CNT=%ld\n", START, START_PREV, STARTED, STATE, CYCLE_CNT );
        RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [Inside2] START_TIME=%d CURRENT_TIME=%d period_next=%d PWM_off_next=%d\n", START_TIME.tv_sec, CURRENT_TIME.tv_sec, period_next, PWM_off_next);
        
        
    } // STATE = 1 or 2
   
    // TODO: handle STOP case
    // TODO: handle ResetMode
    // TODO: handle Limit and Ratio changes on the fly
    // TODO: when EN/ENO suported, handle error cases such as Limit_High < Limit_Low
    
	RTL_TRDBG (TRACE_DETAIL, "ACTILITY_FBS_PWM [EXIT] PWM_OUT=%d\n", PWM_OUT);
	
    RET->t  = BOOLtype;     RET->p.BOOLvar	= PWM_OUT;      RET	++;
    RET->t  = BOOLtype;     RET->p.BOOLvar	= STARTED;      RET	++;
    RET->t  = BOOLtype;     RET->p.BOOLvar	= START_PREV;   RET	++;
	RET->t	= TIMEtype;     RET->p.TIMEvar	= START_TIME;   RET ++;
	RET->t	= SINTtype;     RET->p.SINTvar	= STATE;        RET ++;
	RET->t	= DINTtype;     RET->p.DINTvar	= CYCLE_CNT;

}

#ifdef MAIN
int TraceDebug=0;
int TraceLevel=0;



int main() {

	struct tm *tm;
	time_t now;
    
	now = time(0);

}
#endif



