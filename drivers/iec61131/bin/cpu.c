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
#include <time.h>
#include <sys/times.h>
#include <rtlbase.h>

#include "plc.h"
#include "board.h"
#include "iec_config.h"
#include "iec_debug.h"

extern iec_global_t GLOB;



#define CPU_CHECK_INTERVAL 10
static time_t next = 0;
static clock_t ticks_initial;
unsigned int cpu_fatal;
unsigned int cpu_danger;
unsigned int cpu_warning;

void iec_check_cpu() {
	time_t now = time(0);
	struct tms tms;

	if	(!next) {
		next	= now;
		cpu_fatal = IEC_Config_Int_Default("cpu_fatal", 90);
		cpu_danger = IEC_Config_Int_Default("cpu_danger", 70);
		cpu_warning = IEC_Config_Int_Default("cpu_warning", 50);
		ticks_initial = times(&tms);
	}

	if	(now > next) {
		next	+= CPU_CHECK_INTERVAL;
		//int cpu = rtl_cpuloadavg();

		clock_t ticks = times(&tms);
		int cpu = tms.tms_utime * 100 / (ticks - ticks_initial);
		//printf ("%ld %ld => %d%%\n", ticks - ticks_initial, tms.tms_utime, cpu);

		if	(cpu >= cpu_fatal) {
			RTL_TRDBG (TRACE_ERROR, "IEC cpu reached fatal zone %d%%\n", cpu_fatal);
			GSC_log("CPU_FATAL", "IEC cpu reached fatal zone.", "", "Fatal zone %d%%", cpu_fatal);

			// and then ... what to do ?
		}
		else if	(cpu >= cpu_danger) {
			RTL_TRDBG (TRACE_ERROR, "IEC cpu reached danger zone %d%%\n", cpu_danger);
			GSC_log("CPU_DANGER", "IEC cpu reached danger zone.", "", "Danger zone %d%%", cpu_danger);
			Board_Put(&GLOB.board_errors, "IEC cpu reached danger zone %d%%", cpu_danger);
		}
		else if	(cpu >= cpu_warning) {
			RTL_TRDBG (TRACE_ERROR, "IEC cpu reached warning zone %d%%\n", cpu_warning);
			GSC_log("CPU_WARNING", "IEC cpu reached warning zone.", "", "Warning zone %d%%", cpu_warning);
			Board_Put(&GLOB.board_errors, "IEC cpu reached warning zone %d%%", cpu_warning);
		}
	}
}
