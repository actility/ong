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

/* STANDARD_FBS.c --
 */

/*
  Original Work :

  MATIEC - IEC 61131-3 compiler


  The following compiler has been based on the
  FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)


  Copyright (C) 2003-2011  Mario de Sousa (msousa@fe.up.pt)
*/

#include <stdio.h>
#include <stddef.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#include <mxml.h>

#include <rtlbase.h>
#include <rtllist.h>
#include <rtlhtbl.h>

#include "iec_debug.h"
#include "plc.h"
#include "literals.h"
#include "timeoper.h"


//-------------------------------

void iec_gettimeofday(IEC_TIMESPEC *iec_time) {
	struct timeval tv;
	gettimeofday (&tv, NULL);
	iec_time->tv_sec	= tv.tv_sec;
	iec_time->tv_nsec	= tv.tv_usec*1000;
}

void iec_getMonotonicTime(IEC_TIMESPEC *iec_time) {
#ifdef _POSIX_TIMERS
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	iec_time->tv_sec	= ts.tv_sec;
	iec_time->tv_nsec	= ts.tv_nsec;
#else
	iec_gettimeofday (iec_time);
#endif
}

/*
                  +-----+
S1----------------| >=1 |---Q1
         +---+    |     |
 R------O| & |----|     |
 Q1------|   |    |     |
         +---+    +-----+
*)

FUNCTION_BLOCK SR
  VAR_INPUT
    S1, R : BOOL;
  END_VAR
  VAR_OUTPUT
    Q1 : BOOL;
  END_VAR
  Q1 := S1 OR ((NOT R) AND Q1);
END_FUNCTION_BLOCK
*/

void SR_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	S1 = in++->BOOLvar;
	BOOL	R = in++->BOOLvar;
	BOOL	Q1 = in++->BOOLvar;

	Q1	= S1 | ((!R) & Q1);
	RTL_TRDBG (TRACE_DETAIL, "SR S1=%d R=%d Q1=%d\n", S1, R, Q1);

	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q1;
}

/*
                   +---+
R1----------------O| & |---Q1
        +-----+    |   |
S-------| >=1 |----|   |
Q1------|     |    |   |
        +-----+    +---+
*)

FUNCTION_BLOCK RS
  VAR_INPUT
    S, R1 : BOOL;
  END_VAR
  VAR_OUTPUT
    Q1 : BOOL;
  END_VAR
  Q1 := (NOT R1) AND (S OR Q1);
END_FUNCTION_BLOCK
*/

void RS_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	S = in++->BOOLvar;
	BOOL	R1 = in++->BOOLvar;
	BOOL	Q1 = in++->BOOLvar;

	Q1	= (!R1) & (S | Q1);
	RTL_TRDBG (TRACE_DETAIL, "RS S=%d R1=%d Q1=%d\n", S, R1, Q1);

	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q1;
}

/*
FUNCTION_BLOCK CTU
  VAR_INPUT
    CU : BOOL;
    R : BOOL;
    PV : INT;
  END_VAR
  VAR_OUTPUT
    Q : BOOL;
    CV : INT;
  END_VAR
  VAR
    CU_T: R_TRIG;
  END_VAR

  CU_T(CU);
  IF R THEN CV := 0 ;
  ELSIF CU_T.Q AND (CV < PV)
       THEN CV := CV+1;
  END_IF ;
  Q := (CV >= PV) ;
END_FUNCTION_BLOCK
*/

void CTU_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	static INT PVmax = 32767;
	BOOL	CU = in++->BOOLvar;
	BOOL	R = in++->BOOLvar;
	INT	PV = in++->INTvar;
	INT	CV = in++->INTvar;
	INT	TRIG_M = in++->INTvar;
	BOOL	Q;
	BOOL	TRIG_Q;

	RTL_TRDBG (TRACE_DETAIL, "CTU CU=%d R=%ld PV=%d TRIG_M=%d\n", CU, R, PV, TRIG_M);

	//	R_TRIG copied here for simplicity
	TRIG_Q	= CU & !TRIG_M;
	TRIG_M	= CU;

	if	(R)	CV = 0;
	else if	(TRIG_Q && (CV < PVmax)) CV ++;
	Q = (CV >= PV);

	RTL_TRDBG (TRACE_DETAIL, "CTU Q=%d CV=%d TRIG_M=%d\n", Q, CV, TRIG_M);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= INTtype;
	RET->p.INTvar	= CV;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= TRIG_M;
	RET	++;
}

void CTD_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	static INT PVmin = 0;
	BOOL	CD = in++->BOOLvar;
	BOOL	LD = in++->BOOLvar;
	INT	PV = in++->INTvar;
	INT	CV = in++->INTvar;
	INT	TRIG_M = in++->INTvar;
	BOOL	Q;
	BOOL	TRIG_Q;

	RTL_TRDBG (TRACE_DETAIL, "CTD CD=%d LD=%ld PV=%d TRIG_M=%d\n", CD, LD, PV, TRIG_M);

	//	R_TRIG copied here for simplicity
	TRIG_Q	= CD & !TRIG_M;
	TRIG_M	= CD;

	if	(LD)	CV = PV;
	else if	(TRIG_Q && (CV > PVmin)) CV--;
	Q	= (CV <= 0);

	RTL_TRDBG (TRACE_DETAIL, "CTD Q=%d CV=%d TRIG_M=%d\n", Q, CV, TRIG_M);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= INTtype;
	RET->p.INTvar	= CV;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= TRIG_M;
	RET	++;
}

/*
FUNCTION_BLOCK CTUD
  VAR_INPUT
    CU : BOOL;
    CD : BOOL;
    R  : BOOL;
    LD : BOOL;
    PV : INT;
  END_VAR
  VAR_OUTPUT
    QU : BOOL;
    QD : BOOL;
    CV : INT;
  END_VAR
  VAR
    CD_T: R_TRIG;
    CU_T: R_TRIG;
  END_VAR

  CD_T(CD);
  CU_T(CU);

  IF R THEN CV := 0 ;
  ELSIF LD THEN CV := PV ;
  ELSE
    IF NOT (CU_T.Q AND CD_T.Q) THEN
      IF CU_T.Q AND (CV < PV)
      THEN CV := CV+1;
      ELSIF CD_T.Q AND (CV > 0)
      THEN CV := CV-1;
      END_IF;
    END_IF;
  END_IF ;
  QU := (CV >= PV) ;
  QD := (CV <= 0) ;
END_FUNCTION_BLOCK
*/

void CTUD_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	static INT PVmax = 32767;
	static INT PVmin = 0;
	BOOL	CU = in++->BOOLvar;
	BOOL	CD = in++->BOOLvar;
	BOOL	R = in++->BOOLvar;
	BOOL	LD = in++->BOOLvar;
	INT	PV = in++->INTvar;
	INT	CV = in++->INTvar;
	BOOL	TRIG_MU = in++->BOOLvar;
	BOOL	TRIG_MD = in++->BOOLvar;
	BOOL	QU, QD;
	BOOL	TRIG_QU, TRIG_QD;

	RTL_TRDBG (TRACE_DETAIL, "CTUD CU=%d CD=%d R=%ld LD=%d PV=%d\n", CU, CD, R, LD, PV);

	//	R_TRIG copied here for simplicity
	TRIG_QU	= CU & !TRIG_MU;
	TRIG_MU	= CU;

	//	R_TRIG copied here for simplicity
	TRIG_QD	= CD & !TRIG_MD;
	TRIG_MD	= CD;

	if	(R)	CV = 0;
	else if	(LD)	CV = PV;
	else if (!(TRIG_QU & TRIG_QD)) {
		if	(TRIG_QU & (CV < PVmax))	CV ++;
		else if	(TRIG_QD && (CV > PVmin))	CV--;
	}

	QU	= (CV >= PV);
	QD	= (CV <= 0);

	RTL_TRDBG (TRACE_DETAIL, "CTUD QU=%d QD=%d CV=%d TRIG_MU=%d TRIG_MD=%d\n", QU, QD, CV, TRIG_MU, TRIG_MD);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= QU;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= QD;
	RET	++;
	RET->t		= INTtype;
	RET->p.INTvar	= CV;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= TRIG_MU;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= TRIG_MD;
	RET	++;
}

/*
FUNCTION_BLOCK R_TRIG
     VAR_INPUT  CLK: BOOL; END_VAR
     VAR_OUTPUT  Q: BOOL; END_VAR
     VAR RETAIN M: BOOL; END_VAR
Q := CLK AND NOT M;
M := CLK;
END_FUNCTION_BLOCK
*/
void R_TRIG_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	CLK = in++->BOOLvar;
	BOOL	M = in++->BOOLvar;
	BOOL	Q;

	Q	= CLK & !M;
	M	= CLK;

	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.INTvar	= M;
}

/*
FUNCTION_BLOCK F_TRIG
     VAR_INPUT  CLK: BOOL; END_VAR
     VAR_OUTPUT   Q: BOOL; END_VAR
     VAR RETAIN M: BOOL; END_VAR
Q := NOT CLK AND NOT M;
M := NOT CLK;
END_FUNCTION_BLOCK
*/
void F_TRIG_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	CLK = in++->BOOLvar;
	BOOL	M = in++->BOOLvar;
	BOOL	Q;

	Q	= !CLK & !M;
	M	= !CLK;

	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.INTvar	= M;
}

/*
(****************************************************************

              RTC - Real-time clock

Q is a copy of IN.

When IN = FALSE, CDT is the current date and time as set by the
PLC driver.

When IN changes from FALSE to TRUE, PDT is stored. As long as IN is
TRUE, CDT is equal to PDT + the amount of time since PDT was loaded.

****************************************************************)

FUNCTION_BLOCK RTC
   VAR_INPUT
     IN : BOOL;                  (* 0 - current time, 1 - load time from PDT *)
     PDT : DT;                   (* Preset datetime *)
   END_VAR
   VAR_OUTPUT
     Q : BOOL := FALSE;          (* Copy of IN *)
     CDT : DT;                   (* Datetime, current or relative to PDT *)
   END_VAR

   VAR
     PREV_IN : BOOL := FALSE;
     OFFSET : TIME;
     CURRENT_TIME : DT;
   END_VAR

   {__SET_VAR(data__->,CURRENT_TIME,__CURRENT_TIME)}

   IF IN
   THEN
     IF NOT PREV_IN
     THEN
         OFFSET := PDT - CURRENT_TIME;
     END_IF;

     (* PDT + time since PDT was loaded *)
     CDT := CURRENT_TIME + OFFSET;
   ELSE
     CDT := CURRENT_TIME;
   END_IF;

   Q := IN;
   PREV_IN := IN;

END_FUNCTION_BLOCK
*/

void RTC_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	IN = in++->BOOLvar;
	DT	PDT = in++->DTvar;
	BOOL	PREV_IN = in++->BOOLvar;
	TIME	OFFSET = in++->TIMEvar;
	BOOL	Q = FALSE;
	DT	CDT;
	TIME	CURRENT_TIME;

	iec_gettimeofday (&CURRENT_TIME);

	RTL_TRDBG (TRACE_DETAIL, "RTC IN=%d PDT=%ld PREV_IN=%d OFFSET=%ld CURRENT_TIME=%ld\n", IN, PDT.tv_sec, PREV_IN, OFFSET.tv_sec, CURRENT_TIME.tv_sec);

	if	(IN) {
		if	(!PREV_IN)
			SUB_TIMESPEC3 (OFFSET, PDT, CURRENT_TIME);
		ADD_TIMESPEC3 (CDT, CURRENT_TIME, OFFSET);
	}
	else
		CDT	= CURRENT_TIME;

	Q	= IN;
	PREV_IN	= IN;

	RTL_TRDBG (TRACE_DETAIL, "RTC Q=%d CDT=%ld OFFSET=%ld\n", Q, CDT.tv_sec, OFFSET.tv_sec);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= DTtype;
	RET->p.DTvar	= CDT;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= PREV_IN;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.TIMEvar	= OFFSET;
}


/*
(****************************************************************

                  TP - pulse timing - state machine


            +--------+     ++ ++   +--------+
       IN   |        |     || ||   |        |
          --+        +-----++-++---+        +---------
            t0       t1    t2 t3    t4       t5
            +----+         +----+  +----+
       Q    |    |         |    |  |    |
          --+    +---------+    +--+    +-------------
            t0   t0+PT    t2 t2+PT t4  t4+PT
         PT      +---+          +       +---+
          :     /    |         /|      /    |
       ET :    /     |        / |     /     |
          :   /      |       /  |    /      |
          :  /       |      /   |   /       |
          0-+        +-----+    +--+        +---------
            t0       t1    t2      t4       t5


        2        +---+          +       +---+
STATE   1   +----+   |     +----|  +----+   | 
        0 --+        +-----+    +--+        +---------


****************************************************************)

FUNCTION_BLOCK TP
  VAR_INPUT
    IN : BOOL;  (* first input parameter *)
    PT : TIME;  (* second input parameter *)
  END_VAR
  VAR_OUTPUT
    Q : BOOL := FALSE;  (* first output parameter *)
    ET : TIME := T#0s;  (* second output parameter *)
  END_VAR

  VAR
    STATE : SINT := 0;  (* internal state: 0-reset, 1-counting, 2-set *)
    PREV_IN : BOOL := FALSE;
    CURRENT_TIME, START_TIME : TIME;
  END_VAR

  {__SET_VAR(data__->,CURRENT_TIME,__CURRENT_TIME)}

  IF ((STATE = 0) AND NOT(PREV_IN) AND IN)   (* found rising edge on IN *)
  THEN
    (* start timer... *)
    STATE := 1;
    Q := TRUE;
    START_TIME := CURRENT_TIME;

  ELSIF (STATE = 1)
  THEN
    IF ((START_TIME + PT) <= CURRENT_TIME)
    THEN
      STATE := 2;
      Q := FALSE;
      ET := PT;
    ELSE
      ET := CURRENT_TIME - START_TIME;
    END_IF;
  END_IF;

  IF ((STATE = 2) AND NOT(IN))
  THEN
    ET := T#0s;
    STATE := 0;
  END_IF;


  PREV_IN := IN;

END_FUNCTION_BLOCK
*/

void TP_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	IN = in++->BOOLvar;
	TIME	PT = in++->TIMEvar;
	SINT	STATE = in++->SINTvar;
	BOOL	PREV_IN = in++->BOOLvar;
	TIME	START_TIME = in++->TIMEvar;
/*	
    TIME	ET = getDuration("T#0s");
	BOOL	Q;
*/
	BOOL	Q = in++->BOOLvar;  // Q will be initialized with FALSE at variable creation time
	TIME	ET = in++->TIMEvar; // ET will be initialized with T#0s at variable creation time
	TIME	CURRENT_TIME;

	iec_gettimeofday (&CURRENT_TIME);

//	RTL_TRDBG (TRACE_DETAIL, "TP IN=%d PT=%ld STATE=%d PREV_IN=%d START_TIME=%ld CURRENT_TIME=%ld\n", IN, PT.tv_sec, STATE, PREV_IN, START_TIME.tv_sec, CURRENT_TIME.tv_sec);
	RTL_TRDBG (TRACE_DETAIL, "STANDARD_FBS_TP [Begin] PREV_IN=%d IN=%d STATE=%d PT=%ld Remaining_TIME=%ld \n", PREV_IN, IN, STATE, PT.tv_sec,  START_TIME.tv_sec-CURRENT_TIME.tv_sec);

	if	((STATE == 0) && !PREV_IN && IN) {
		RTL_TRDBG (TRACE_DETAIL, "STANDARD_FBS_TP [Inside] in Branch 1\n");
		STATE	= 1;
		Q = TRUE;
		START_TIME = CURRENT_TIME;
	}
	else if (STATE == 1) {
		TIME tmp;
		ADD_TIMESPEC3 (tmp, START_TIME, PT); // if PT is changed while in STATE 1, the new value will be use
		RTL_TRDBG (TRACE_DETAIL, "STANDARD_FBS_TP [Inside] in Branch 2 . CMP %ld %ld\n", tmp.tv_sec, CURRENT_TIME.tv_sec);
		if (CMP_TIMESPEC (tmp, CURRENT_TIME) <= 0) {
			STATE = 2;
			Q = FALSE;
			ET = PT;
		}
		else {
			SUB_TIMESPEC3 (ET, CURRENT_TIME, START_TIME);
		}
	}

	if	((STATE == 2) && !IN) {
		RTL_TRDBG (TRACE_DETAIL, "STANDARD_FBS_TP [Inside] in Branch 3\n");
		ET	= getDuration("T#0s");
		STATE	= 0;
	}

	PREV_IN	= IN;

	RTL_TRDBG (TRACE_DETAIL, "STANDARD_FBS_TP [End] Q=%d ET=%ld STATE=%d PREV_IN=%d\n", Q, ET.tv_sec, STATE, PREV_IN);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.DTvar	= ET;
	RET	++;
	RET->t		= SINTtype;
	RET->p.BOOLvar	= STATE;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= PREV_IN;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.TIMEvar	= START_TIME;
}

/*
(****************************************************************

             TON - On-delay timing - state machine


            +--------+        +---+   +--------+
       IN   |        |        |   |   |        |
          --+        +--------+   +---+        +-------------
            t0       t1       t2  t3  t4       t5
                 +---+                     +---+
       Q         |   |                     |   |
          -------+   +---------------------+   +-------------
              t0+PT  t1                t4+PT   t5
         PT      +---+                     +---+
          :     /    |            +       /    |
       ET :    /     |           /|      /     |
          :   /      |          / |     /      |
          :  /       |         /  |    /       |
          0-+        +--------+   +---+        +-------------
            t0       t1       t2  t3  t4       t5


        2        +---+                     +---+
STATE   1   +----+   |        +---+   +----+   |
        0 --+        +--------+   +---+        +------


****************************************************************)


FUNCTION_BLOCK TON
  VAR_INPUT
    IN : BOOL;  (* first input parameter *)
    PT : TIME;  (* second input parameter *)
  END_VAR
  VAR_OUTPUT
    Q : BOOL := FALSE;  (* first output parameter *)
    ET : TIME := T#0s;  (* second output parameter *)
  END_VAR

  VAR
    STATE : SINT := 0;  (* internal state: 0-reset, 1-counting, 2-set *)
    PREV_IN : BOOL := FALSE;
    CURRENT_TIME, START_TIME : TIME;
  END_VAR

  {__SET_VAR(data__->,CURRENT_TIME,__CURRENT_TIME)}

  IF ((STATE = 0) AND NOT(PREV_IN) AND IN)   (* found rising edge on IN *)
  THEN
    (* start timer... *)
    STATE := 1;
    Q := FALSE;
    START_TIME := CURRENT_TIME;

  ELSE
    (* STATE is 1 or 2 !! *)
    IF (NOT(IN))
    THEN
      ET := T#0s;
      Q := FALSE;
      STATE := 0;

    ELSIF (STATE = 1)
    THEN
      IF ((START_TIME + PT) <= CURRENT_TIME)
      THEN
        STATE := 2;
        Q := TRUE;
        ET := PT;
      ELSE
        ET := CURRENT_TIME - START_TIME;
      END_IF;
    END_IF;

  END_IF;

  PREV_IN := IN;

END_FUNCTION_BLOCK
*/


void TON_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	IN = in++->BOOLvar;
	TIME	PT = in++->TIMEvar;
	SINT	STATE = in++->SINTvar;
	BOOL	PREV_IN = in++->BOOLvar;
	TIME	START_TIME = in++->TIMEvar;
/*
	TIME	ET = getDuration("T#0s");
	BOOL	Q = FALSE;
*/
	BOOL	Q = in++->BOOLvar;  // Q will be initialized with FALSE at variable creation time
	TIME	ET = in++->TIMEvar; // ET will be initialized with T#0s at variable creation time

	TIME	CURRENT_TIME;

	iec_gettimeofday (&CURRENT_TIME);

	RTL_TRDBG (TRACE_DETAIL, "TON Q=%d ET=%ld IN=%d PT=%ld STATE=%d PREV_IN=%d START_TIME=%ld CURRENT_TIME=%ld\n",
		Q, ET.tv_sec, IN, PT.tv_sec, STATE, PREV_IN, START_TIME.tv_sec, CURRENT_TIME.tv_sec);

	if	((STATE == 0) && !PREV_IN && IN) {
		RTL_TRDBG (TRACE_DETAIL, "Branch 1\n");
		STATE	= 1;
		Q = FALSE;
		START_TIME = CURRENT_TIME;
	}
	else {
		if	(!IN) {
			RTL_TRDBG (TRACE_DETAIL, "Branch 2\n");
			ET	= getDuration("T#0s");
			Q	= FALSE;
			STATE	= 0;
		}
		else if (STATE == 1) {
			TIME tmp;
			ADD_TIMESPEC3 (tmp, START_TIME, PT);
			RTL_TRDBG (TRACE_DETAIL, "Branch 3 . CMP %ld %ld\n", tmp.tv_sec, CURRENT_TIME.tv_sec);
			if (CMP_TIMESPEC (tmp, CURRENT_TIME) <= 0) {
				STATE = 2;
				Q = TRUE;
				ET = PT;
			}
			else {
				SUB_TIMESPEC3 (ET, CURRENT_TIME, START_TIME);
			}
		}
	}

	PREV_IN	= IN;

	RTL_TRDBG (TRACE_DETAIL, "TON Q=%d ET=%ld STATE=%d PREV_IN=%d\n", Q, ET.tv_sec, STATE, PREV_IN);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.DTvar	= ET;
	RET	++;
	RET->t		= SINTtype;
	RET->p.BOOLvar	= STATE;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= PREV_IN;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.TIMEvar	= START_TIME;
}


/*
(****************************************************************

             TOF - Off-delay timing - state machine


              +--------+        +---+   +--------+
       IN     |        |        |   |   |        |
           ---+        +--------+   +---+        +-----------
              t0       t1       t2  t3  t4       t5
              +-------------+   +---------------------+
       Q      |             |   |                     |
           ---+             +---+                     +------
              t0          t1+PT t2                  t5+PT
         PT                 +---+                     +------
          :                /    |       +            /
       ET :               /     |      /|           /
          :              /      |     / |          /
          :             /       |    /  |         /
          0------------+        +---+   +--------+
                       t1           t3           t5


        2                   +---+                     +------
STATE   1              +----+   |   +---+        +----+
        0 -------------+        +---+   +--------+


****************************************************************)

FUNCTION_BLOCK TOF
  VAR_INPUT
    IN : BOOL;  (* first input parameter *)
    PT : TIME;  (* second input parameter *)
  END_VAR
  VAR_OUTPUT
    Q : BOOL := FALSE;  (* first output parameter *)
    ET : TIME := T#0s;  (* second output parameter *)
  END_VAR

  VAR
    STATE : SINT := 0;  (* internal state: 0-reset, 1-counting, 2-set *)
    PREV_IN : BOOL := FALSE;
    CURRENT_TIME, START_TIME : TIME;
  END_VAR

  {__SET_VAR(data__->,CURRENT_TIME,__CURRENT_TIME)}

  IF ((STATE = 0) AND PREV_IN AND NOT(IN))   (* found falling edge on IN *)
  THEN
    (* start timer... *)
    STATE := 1;
    START_TIME := CURRENT_TIME;

  ELSE
    (* STATE is 1 or 2 !! *)
    IF (IN)
    THEN
      ET := T#0s;
      STATE := 0;

    ELSIF (STATE = 1)
    THEN
      IF ((START_TIME + PT) <= CURRENT_TIME)
      THEN
        STATE := 2;
        ET := PT;
      ELSE
        ET := CURRENT_TIME - START_TIME;
      END_IF;
    END_IF;

  END_IF;

  Q := IN OR (STATE = 1);
  PREV_IN := IN;

END_FUNCTION_BLOCK
*/
void TOF_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
    // LNI
    // TODO: modify TOF Interface same as TON
	TEST_EN
	BOOL	IN = in++->BOOLvar;
	TIME	PT = in++->TIMEvar;
	SINT	STATE = in++->SINTvar;
	BOOL	PREV_IN = in++->BOOLvar;
	TIME	START_TIME = in++->TIMEvar;
	TIME	ET = getDuration("T#0s");
	BOOL	Q;
	TIME	CURRENT_TIME;

	iec_gettimeofday (&CURRENT_TIME);

	RTL_TRDBG (TRACE_DETAIL, "TOF IN=%d PT=%ld STATE=%d PREV_IN=%d START_TIME=%ld CURRENT_TIME=%ld\n", IN, PT.tv_sec, STATE, PREV_IN, START_TIME.tv_sec, CURRENT_TIME.tv_sec);

	if	((STATE == 0) && PREV_IN && !IN) {
		RTL_TRDBG (TRACE_DETAIL, "Branch 1\n");
		STATE	= 1;
		START_TIME = CURRENT_TIME;
	}
	else {
		if	(IN) {
			RTL_TRDBG (TRACE_DETAIL, "Branch 2\n");
			ET	= getDuration("T#0s");
			STATE	= 0;
		}
		else if (STATE == 1) {
			TIME tmp;
			ADD_TIMESPEC3 (tmp, START_TIME, PT);
			RTL_TRDBG (TRACE_DETAIL, "Branch 3 . CMP %ld %ld\n", tmp.tv_sec, CURRENT_TIME.tv_sec);
			if (CMP_TIMESPEC (tmp, CURRENT_TIME) <= 0) {
				STATE = 2;
				ET = PT;
			}
			else {
				SUB_TIMESPEC3 (ET, CURRENT_TIME, START_TIME);
			}
		}
	}

	Q	= IN | (STATE == 1);
	PREV_IN	= IN;

	RTL_TRDBG (TRACE_DETAIL, "TOF Q=%d ET=%ld STATE=%d PREV_IN=%d\n", Q, ET.tv_sec, STATE, PREV_IN);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.DTvar	= ET;
	RET	++;
	RET->t		= SINTtype;
	RET->p.BOOLvar	= STATE;
	RET	++;
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= PREV_IN;
	RET	++;
	RET->t		= TIMEtype;
	RET->p.TIMEvar	= START_TIME;
}

/*
(* Following taken directly from the IEC 61131.3 draft standard *)

(*
 * An IEC 61131-3 IL and ST compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 *)


FUNCTION_BLOCK HYSTERESIS
    (* Boolean hysteresis on difference *)
    (* of REAL inputs, XIN1 - XIN2      *)
  VAR_INPUT XIN1, XIN2, EPS : REAL; END_VAR
  VAR_OUTPUT Q : BOOL := 0; END_VAR
  IF Q THEN IF XIN1 < (XIN2 - EPS) THEN Q := 0; END_IF ;
  ELSIF XIN1 > (XIN2 + EPS) THEN Q := 1 ;
  END_IF ;
END_FUNCTION_BLOCK
*/

void HYSTERESIS_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	REAL	XIN1 = in++->REALvar;
	REAL	XIN2 = in++->REALvar;
	REAL	EPS = in++->REALvar;
	BOOL	Q = in++->BOOLvar;
// *LNI*
	if (Q) {
		if (XIN1 < (XIN2 - EPS)) Q= 0;
		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_HYSTERESIS if Q==1 After Q=%d\n", Q);
	}
 	else {
	       
		if (XIN1 > (XIN2 + EPS)) Q= 1;
		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_HYSTERESIS if Q==0 After Q=%d\n", Q);
	}

	RTL_TRDBG (TRACE_FBD, "FBD_TRACE_HYSTERESIS XIN1=%f XIN2=%f EPS=%f Q=%d\n", XIN1, XIN2, EPS, Q);
	RET->t		= BOOLtype;
	RET->p.BOOLvar	= Q;
}

/*
 (* Following taken directly from the IEC 61131.3 draft standard *)
 
 (*
 * An IEC 61131-3 IL and ST compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 *)

            +------------+
            | DERIVATIVE |
     BOOL---|RUN         |
     REAL---|XIN     XOUT|---REAL
     TIME---|CYCLE       |
            +------------+
￼FUNCTION_BLOCK DERIVATIVE 
 VAR_INPUT
    RUN :   BOOL;   (* 0 = reset *)
    XIN :   REAL;   (* Input to be differentiated *)
    CYCLE : TIME;   (* Sampling period *)
 END_VAR

 VAR_OUTPUT
    XOUT :  REAL;   (* Differentiated output *)
 END_VAR
 
 VAR
    X1, X2, X3 : REAL ;
 END_VAR
 
 IF RUN THEN
    XOUT := (3.0 * (XIN - X3) + X1 - X2)
            / (10.0 * TIME_TO_REAL(CYCLE)) ;
    X3 := X2 ; 
    X2 := X1 ; 
    X1 := XIN ;
 ELSE
    XOUT := 0.0; X1 := XIN ; X2 := XIN ; X3 := XIN ; 
 END_IF ;
 
 END_FUNCTION_BLOCK
*/


// Normalize values so that we can make most of the arithmetics using integers
#define SCALE_FACTOR_INT    1000
#define SCALE_FACTOR_FLOAT_FOR_MUL  1000.0
#define SCALE_FACTOR_FLOAT_FOR_DIV  0.001

// DERIVATIVE function block as per the example in the IEC 61131-3 standard
// It is working only when CYCLE is set to 1 second
// The implementation use INTEGER for internal computation, while the interface to IEC
// remains with REAL
// XIN values shall be in the range of -10000 to 10000 in order to keep precision to
// an acceptable level
void DERIVATIVE_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
    // LNI
	TEST_EN
	BOOL	RUN = in++->BOOLvar;
	REAL	XIN = in++->REALvar;
	TIME	CYCLE = in++->TIMEvar;
	DINT	X1_int32 = in++->DINTvar;   // previous values are keeped with scaling factor at IEC level
	DINT	X2_int32 = in++->DINTvar;
	DINT	X3_int32 = in++->DINTvar;
	
    DINT    XIN_int32;
    DINT    XOUT_int32;
    REAL	XOUT;
    
//    REAL   Cycle_second_float;
    DINT   Cycle_second_int32;


    // Normalize and scale XIN
    XIN_int32 = (DINT)(XIN*SCALE_FACTOR_FLOAT_FOR_MUL);
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_DERIVATIVE Before RUN=%d XIN=%f XIN(int)=%d X1=%d X2=%d X3=%d TIME=%ld sec\n", RUN, XIN ,XIN_int32, X1_int32 , X2_int32 , X3_int32, CYCLE.tv_sec);
    
	if	(RUN) {
//		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_DERIVATIVE Running\n");
//        Cycle_second_float = (float)( CYCLE.tv_sec + (CYCLE.tv_nsec/NSEC_PER_SEC));
        Cycle_second_int32 = SCALE_FACTOR_INT * CYCLE.tv_sec;
        Cycle_second_int32 += SCALE_FACTOR_INT * CYCLE.tv_nsec/NSEC_PER_SEC;
//        RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_DERIVATIVE **** Cycle(float)=%f Cycle(int)%ld\n", Cycle_second_float, Cycle_second_int32 );

//        XOUT = ( 3.0 * (XIN-X3) + X1 -X2 ) / ( 10.0 * Cycle_second);
        XOUT_int32 = SCALE_FACTOR_FLOAT_FOR_MUL * (3 * (XIN_int32-X3_int32) + X1_int32 -X2_int32) ;
        RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_DERIVATIVE XOUT_int32=%ld\n", XOUT_int32 );
        // Increse again precision by Scaling factor so that the Division that comes after has a big enough number
        // only last calculation is using floating point math
        XOUT = (REAL) (XOUT_int32 / (10 * Cycle_second_int32)) * SCALE_FACTOR_FLOAT_FOR_DIV;
        X3_int32 = X2_int32;
        X2_int32 = X1_int32;
        X1_int32 = XIN_int32;
	} // end if
	else {
//		RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_DERIVATIVE NOT Running, initialize X1,X2,X3\n");
        XOUT = 0.0;
        X1_int32 = XIN_int32;
        X2_int32 = XIN_int32;
        X3_int32 = XIN_int32;
    } // end else
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_DERIVATIVE After RUN=%d XIN=%f XIN(int)=%d X1=%d X2=%d X3=%d XOUT=%f TIME=%ld sec\n", RUN, XIN ,XIN_int32, X1_int32 , X2_int32 , X3_int32, XOUT, CYCLE.tv_sec);
	
    // return output variable and internal variables;
	RET->t= REALtype;   RET->p.REALvar = XOUT;  RET	++;
	RET->t= DINTtype;   RET->p.DINTvar = X1_int32;    RET	++;
	RET->t= DINTtype;   RET->p.DINTvar = X2_int32;    RET	++;
	RET->t= DINTtype;   RET->p.DINTvar = X3_int32;    RET	++;

}

/*
FUNCTION_BLOCK PID
   VAR_INPUT
     AUTO : BOOL ;        (* 0 - manual , 1 - automatic *)
     PV : REAL ;          (* Process variable *)
     SP : REAL ;          (* Set point *)
     X0 : REAL ;          (* Manual output adjustment - *)
                          (* Typically from transfer station *)
     KP : REAL ;          (* Proportionality constant *)
     TR : REAL ;          (* Reset time *)
     TD : REAL ;          (* Derivative time constant *)
     CYCLE : TIME ;       (* Sampling period *)
   END_VAR
   VAR_OUTPUT XOUT : REAL; END_VAR
   VAR ERROR : REAL ;        (* PV - SP *)
       X1, X2, X3 : REAL ;
   END_VAR

   ERROR := PV - SP ;

   (* INTEGRAL *)
   IF NOT AUTO THEN ITERM_XOUT := TR * (X0 - ERROR) ;
   ELSE ITERM_XOUT := ITERM_XOUT + ERROR * TIME_TO_REAL(CYCLE);
   END_IF ;

   (* DERIVATIVE *)
   IF AUTO THEN
      DTERM_XOUT := (3.0 * (ERROR - X3) + X1 - X2) / (10.0 * TIME_TO_REAL(CYCLE)) ;
      X3 := X2 ; X2 := X1 ; X1 := ERROR ;
   ELSE DTERM_XOUT := 0.0; X1 := ERROR ; X2 := ERROR ; X3 := ERROR ;
   END_IF ;

   XOUT := KP * (ERROR + ITERM_XOUT/TR + DTERM_XOUT*TD) ;
END_FUNCTION_BLOCK
*/

static inline LREAL __time_to_real(TIME IN){
    return (LREAL)IN.tv_sec + ((LREAL)IN.tv_nsec/1000000000);
}

void PID_function(BOOL EN, BOOL *ENO, param_ret_t *RET, UINT count, param_u_t *in) {
	TEST_EN
	BOOL	AUTO = in++->BOOLvar;
	REAL	PV = in++->REALvar;
	REAL	SP = in++->REALvar;
	REAL	X0 = in++->REALvar;
	REAL	KP = in++->REALvar;
	REAL	TR = in++->REALvar;
	REAL	TD = in++->REALvar;
	TIME	CYCLE = in++->TIMEvar;
	// Instance variables
	REAL	X1 = in++->REALvar;
	REAL	X2 = in++->REALvar;
	REAL	X3 = in++->REALvar;
	REAL	ITERM_XOUT = in++->REALvar;
	// Local variables
	REAL	ERROR;
	REAL	DTERM_XOUT;
	REAL	XOUT;
#ifdef DEV_LAURENT
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_PID SP=%f PV=%f Kp=%f TR=%f TD=%f X0=%f\n", SP, PV, KP, TR, TD, X0 );
#endif

	ERROR = PV - SP ;
#ifdef DEV_LAURENT
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_PID Error=%f\n", ERROR );
#endif
	// INTEGRAL
	if	(!AUTO)
		ITERM_XOUT = TR * (X0 - ERROR) ;
	else
		ITERM_XOUT = ITERM_XOUT + ERROR * __time_to_real(CYCLE);
#ifdef DEV_LAURENT
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_PID Integral Term=%f\n", ITERM_XOUT );
#endif

	// DERIVATIVE
	if	(AUTO) {
		DTERM_XOUT = (3.0 * (ERROR - X3) + X1 - X2) / (10.0 * __time_to_real(CYCLE)) ;
		X3=X2; X2=X1; X1=ERROR;
	}
	else {
		DTERM_XOUT = 0.0; X1=X2=X3=ERROR;
	}
#ifdef DEV_LAURENT
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_PID Derivative Term=%f\n", DTERM_XOUT );
#endif

	REAL pterm = ERROR;
	REAL iterm = TR ? ITERM_XOUT/TR : 0;	// avoid a division by 0, the result will be +/-infinite !
	REAL dterm = DTERM_XOUT*TD;

	XOUT = KP * (pterm + iterm + dterm);

#ifdef DEV_LAURENT
    RTL_TRDBG (TRACE_DETAIL, "FBD_TRACE_PID Xout=%f\n", XOUT );
#endif

	RET->t= REALtype;   RET->p.REALvar = XOUT;  RET	++;
	RET->t= REALtype;   RET->p.REALvar = X1;  RET	++;
	RET->t= REALtype;   RET->p.REALvar = X2;  RET	++;
	RET->t= REALtype;   RET->p.REALvar = X3;  RET	++;
	RET->t= REALtype;   RET->p.REALvar = ITERM_XOUT;  RET	++;
}

iec_fctinfos_t STANDARD_FBS_Functions[] = {
	{ "SR_function", SR_function, "BOOL", 0 },
	{ "RS_function", RS_function, "BOOL", 0 },
	{ "CTU_function", CTU_function, "INT", 0 },
	{ "CTD_function", CTD_function, "INT", 0 },
	{ "CTUD_function", CTUD_function, "INT", 0 },
	{ "R_TRIG_function", R_TRIG_function, "BOOL", 0 },
	{ "F_TRIG_function", F_TRIG_function, "BOOL", 0 },
	{ "RTC_function", RTC_function, "BOOL", 0 },
	{ "TP_function", TP_function, "BOOL", 0 },
	{ "TON_function", TON_function, "BOOL", 0 },
	{ "TOF_function", TOF_function, "BOOL", 0 },
	{ "HYSTERESIS_function", HYSTERESIS_function, "BOOL", 0 },
	{ "DERIVATIVE_function", DERIVATIVE_function, "REAL", 0 },
	{ "PID_function", PID_function, "REAL", 0 },
	{ NULL, NULL, NULL }
};


void STANDARD_FBS_init()
{
	iec_addFuncs (STANDARD_FBS_Functions);
}
