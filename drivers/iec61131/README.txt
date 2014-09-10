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



- IEC61131-3 virtual machine -

Version	: 1.0
Date	: 2013-01-10


0. Typical use
--------------

- edit your application with Beremiz
- run your application with :
	./bin/plc.x -f <yourScript>.xml -t <traceLevel> -I <stdInclude> -p <CLI port>

  <traceLevel> : [0-9], default 9
  <stdInclude> : include directory for standards FBS (default is $ROOTACT/iec61131/bin)
  <CLI port> : listen port for CLI, default 2002
- use CLI interface to manage runing applications

Note :
	plc.x can run multiple applications : -f<xml1> -t<level1> -f<xml2> -t<level2> ...

1. ST Interpreter
-----------------

ST interpreter is in the ./inter directory. It is available as a standalone executable (./inter/parser).
Unit tests are available in ./inter/tests.


1.1 BOOL
--------

Examples :
A := TRUE;
B := TRUE;
A := NOT A;
C := (A AND B) XOR (B OR A);

1.2 DINT
-------
Only the DINT type is available. INT, SINT, LINT are not.

Examples :
A := 1;
B := 2;
C := A + B - 5;
D := (A+B)/(A-B)*3;

1.3 REAL
---------

Examples :
A := 1.5;
B := 2.8;
C := A + B - (5/3);
D := (A+B)/(A-B)*3.0;

Counter example :
A := 10;      this is a DINT
A := 10.0;    this is a REAL

1.4 TIME
--------

Examples :
D1 := T#10h30m15s;
D2 := T#10h30m14s;
D2 := D2 + T#1s;
D2 = D1;

1.5 TOD
-------

Examples :
D1 := TIME_OF_DAY#15:36:55.36;
D2 := TOD#15:37:55.36;
D2 > D1;

1.6 DATE
--------

Examples :
D1 := DATE#1984-06-25;
D2 := D#2012-12-19;
D2 > D1;

1.7 DT
------

Examples :
D1 := DATE_AND_TIME#1984-06-25-15:36:55.36;
D2 := DT#1985-06-25-15:36:55.36;
D2 > D1;

1.8 STRING
----------

Examples :
A := 'bonjour chez vous';
B := "bonjour chez vous";
I := LEFT(A,3);
L := LEN(A);

1.9 Expressions
--------------
Examples :
A := 1;
B := 2;
((A+B) >= 3) AND (B < 3)

1.10 Condition
--------------

Examples :
A := 1;
IF A EQ 1 THEN B := 5; END_IF
IF A NE 1 THEN
	B := 1;
ELSE
	C := 99;
	IF B EQ 2 THEN
		D := 2000;
	ELSIF B EQ 3 THEN
		D := 3000;
	ELSIF B EQ 4 THEN
		D := 4000;
	ELSE
		D := 9999;
	END_IF
END_IF

D EQ 3000;

1.11 LocalDT() extension
---------------------------

The LocalDT() function returns a DT variable. Functions are available to retrieve certains parts of the DT,
as the POSIX localtime() do.

T1 := LocalDT();
x := sec(T1);
x := min(T1);
x := hour(T1);
x := mday(T1);
x := mon(T1);
x := year(T1);
x := wday(T1);
x := yday(T1);
x := isdst(T1);
x := gmtoff(T1);

G y := tmzone(T1);


1.12 Day/Night and Lat/Lon extension
------------------------------------
The setLatLon() function takes two REAL values, indicating our position, used to determine the Day or Night Time.
Note : Northern latitudes are positives. Estern longitudes are positives. (like GPS convention).
The isDay() function returns a BOOL indicating if we are in Day Time.

Example:
setLatLon(50.1234, -3.33);
T1 := LocalDT();
d := isDay(T1);

sunsetTime :
SUNSET := sunsetTime();

sunriseTime :
SUNRISE := sunriseTime();


1.13 Comments
-------------
/* comment 1 */
// comment 2



1.14 Calendar Functions
-----------------------

The newCalendarEvent(STRING id, STRING dayStart, STRING start, STRING end) function creates a ponctual calendar event.
	id : event's identifier, eg 'sleepTime'
	dayStart : TODAY, TOMORROW or '+<aNumberOfDaysSincToday>'
	start : start time
	end : end time

The newCalendarDailyEvent(STRING id, STRING start, STRING end, DINT interval) function creates a 
daily occuring event.

The newCalendarWeeklyEvent(STRING id, STRING start, STRING end, STRING daysOfWeek) function creates a
weekly occuring event.

The iec_calendarEventExpires(STRING id, STRING expirationDate) function add an expiration date to an existing daily or weekly event.


BOOL checkCalendarEvent(STRING id, DT t) functions checks if the time 't' matches the event 'id'
This functions returns a BOOL that can be used in a SFC's <transition>


Example:
newCalendarEvent ('Dentist', 'today', '1500', '1600');
T := LocalDT();
DENTIST := checkCalendarEvent ('Dentist', T);



1.15 Misc
---------

The DINT RAND(DINT max) function returns a random DINT between 0 and max.

Example :
X := RAND(1000);




2. Standard Functions and FB
----------------------------
- all IEC61131-3 standard functions and function blocks are available, thanks to the work of matiec
- VAR_GLOBAL, VAR_EXTERNAL are not allowed
- meanwhile executed in the same Unix process, each IEC application is executed in its own sandbox. No interaction
  is permitted between applications
- only one task allowed for a application


3. SFC
------
IEC variables and ST expressions are used to activate Steps.
A condition is made of a sequence of ST and FBD.



Not implemented :
	- ordered execution in transitions
	- calling FBD in a transition
	- action qualifiers. Only 'N' is implemented


4. FBD
------



5. IL/LD
--------
Not implemented


6. Command Line Interface
-------------------------
plc.x binds a CLI in the TCP port 2002 and can be accessed by telnet.

~/Actility/iec61131/bin$ telnet 0 2002
Trying 0.0.0.0...
Connected to 0.
Escape character is '^]'.
Welcome to IEC61131 Virtual Machine
>>?
help, ?     : display this help message
exit, quit  : end the current CLI session
core        : do generate a core file of the current process
ls	: Lists applications (0 params)
run	: run <xmlfile> : run a application (1 params)
pause	: pause <application#> (1 params)
restart	: restart <application#> (1 params)
unload	: unload <application#> (1 params)
trace	: trace <level> (1 params)
info	: info (0 params)
ctx	: ctx <application#> (1 params)
mode	: mode text|json (1 params)
inter	: inter <script#> <code> (2 params)


Examples :

Start a application : run TON_test.xml

Displays running applications : ls
	Script 0 :TON_test.xml

Displays process info : info
	Pid : 5595	Vsize : 4497408	Cpu : 0.470000

Displays contexts of application 0 : ctx 0
name=OnDelay type=BLOCK state=1
	ET (0x8d54c90) TIME 0.0
	Q (0x8d561e8) BOOL FALSE
name=STEP1 type=STEP state=0
	OnDelay (0x8d540d0) derived 
	25.ET (0x8d558a0) TIME 10.0
	SENSOR (0x8d53e80) BOOL FALSE
	Counter (0x8d56758) TIME 10.0
	Running (0x8d54d98) BOOL TRUE
	25.Q (0x8d55440) BOOL TRUE
	DELAY (0x8d53d58) TIME 10.0
name=main type=POU state=1
	OnDelay (0x8d540d0) derived 
	25.ET (0x8d558a0) TIME 10.0
	SENSOR (0x8d53e80) BOOL FALSE
	Counter (0x8d56758) TIME 10.0
	Running (0x8d54d98) BOOL TRUE
	25.Q (0x8d55440) BOOL TRUE
	DELAY (0x8d53d58) TIME 10.0
name=STD_TASK type=TASK state=1

Start a second application : run CTUD_test.xml

Displays running applications : ls
	Script 0 :TON_test.xml
	Script 1 :CTUD_test.xml

Unload applications : unload 1
		unload 0


7. M2M binding (not yet implemented)

The association between an M2M resource (InterupteurSalon) and an IEC variable (BTN1)
is done with <externalVars> declaration.

<externalVars>
 <variable name="BTN1" address="InterupteurSalon">
  <type>
   <BOOL/>
  </type>
 </variable>
</externalVars>

In the application startup, the VM send a subscription for each binding.
When a notification reach the VM, the association is made a the IEC variable is modified.

8. M2M application resources (not yet implemented)

A pool of application resources, containing XML source code, is defined in ONG.
The VM is aware of any change (add/remove/update) in this pool and download/run/stop
adequately the applications.



9. Unit test

Unit test are available in the ./bin directory. The name of the file indicate
its object.

-rw-rw-r-- 1 user user 14414 Jan 21 14:31 CTD_test.xml
-rw-rw-r-- 1 user user 17099 Jan 21 14:31 CTUD_test.xml
-rw-rw-r-- 1 user user 14401 Jan 21 14:31 CTU_test.xml
-rw-rw-r-- 1 user user 11493 Jan 21 14:31 F_TRIG_test.xml
-rw-rw-r-- 1 user user  6433 Jan 21 14:31 Minuterie.xml
-rw-rw-r-- 1 user user 10009 Jan 21 14:31 Reveil.xml
-rw-rw-r-- 1 user user  8190 Jan 21 14:31 RS_test.xml
-rw-rw-r-- 1 user user 13794 Jan 21 14:31 RTC_test.xml
-rw-rw-r-- 1 user user 11494 Jan 21 14:31 R_TRIG_test.xml
-rwxrwxr-x 1 user user 31842 Jan 21 14:31 SFC_TEST.xml*
-rwxrwxr-x 1 user user 14514 Jan 22 16:53 simultaneousDivergence.xml*
-rw-rw-r-- 1 user user  8337 Jan 21 14:31 SR_test.xml
-rw-rw-r-- 1 user user 14166 Jan 21 14:31 TOF_test.xml
-rw-rw-r-- 1 user user 13911 Jan 21 14:31 TON_test.xml
-rw-rw-r-- 1 user user 13900 Jan 21 14:31 TP_test.xml


10. Global variables

Global variables are shared between scripts.
To create a variable, declare variables as Global in <resource>.

11. RETAIN

Global variables can be declared as RETAIN. They are saved in a file named retain.txt
in the current directory.
This file is loaded at plc.x startup.



12. LIMITATIONS

- 32 actions P per actionBlock

