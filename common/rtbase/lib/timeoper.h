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

#ifdef __cplusplus
extern "C" {
#endif

//------- timespec macros	Taken from Mac OSX /usr/include/mach/clock_types.h

#define NSEC_PER_USEC	1000		/* nanoseconds per microsecond */
#define USEC_PER_SEC	1000000		/* microseconds per second */
#define NSEC_PER_SEC	1000000000	/* nanoseconds per second */
#define NSEC_PER_MSEC	1000000ull	/* nanoseconds per millisecond */

#define BAD_TIMESPEC(t)\
	((t).tv_nsec < 0 || (t).tv_nsec >= NSEC_PER_SEC)

#define NULL_TIMESPEC(t)\
	((t).tv_sec == 0 && (t).tv_nsec == 0)

/* t1 <=> t2, also (t1 - t2) in nsec with max of +- 1 sec */
#define CMP_TIMESPEC(t1, t2)\
	((t1).tv_sec > (t2).tv_sec ? +NSEC_PER_SEC :\
	((t1).tv_sec < (t2).tv_sec ? -NSEC_PER_SEC :\
			(t1).tv_nsec - (t2).tv_nsec))

/* t1  += t2 */
#define ADD_TIMESPEC(t1, t2)\
  do {\
	if (((t1).tv_nsec += (t2).tv_nsec) >= NSEC_PER_SEC) {\
		(t1).tv_nsec -= NSEC_PER_SEC;\
		(t1).tv_sec  += 1;\
	}\
	(t1).tv_sec += (t2).tv_sec;\
  } while (0)

/* dest = t1+t2 */
#define	ADD_TIMESPEC3(dest, t1, t2)\
  do {\
	dest = t1;\
	ADD_TIMESPEC(dest,t2);\
  } while (0)

/* t1  -= t2 */
#define SUB_TIMESPEC(t1, t2)\
  do {\
	if (((t1).tv_nsec -= (t2).tv_nsec) < 0) {\
		(t1).tv_nsec += NSEC_PER_SEC;\
		(t1).tv_sec  -= 1;\
	}\
	(t1).tv_sec -= (t2).tv_sec;\
  } while (0)

/* dest = t1-t2 */
#define	SUB_TIMESPEC3(dest, t1, t2)\
  do {\
	dest = t1;\
	SUB_TIMESPEC(dest,t2);\
  } while (0)

/* t1  += t2 */
#define ADD_TIMEVAL(t1, t2)\
  do {\
	if (((t1).tv_usec += (t2).tv_usec) >= USEC_PER_SEC) {\
		(t1).tv_usec -= USEC_PER_SEC;\
		(t1).tv_sec  += 1;\
	}\
	(t1).tv_sec += (t2).tv_sec;\
  } while (0)

/* t1  -= t2 */
#define SUB_TIMEVAL(t1, t2)\
  do {\
	if (((t1).tv_usec -= (t2).tv_usec) < 0) {\
		(t1).tv_usec += USEC_PER_SEC;\
		(t1).tv_sec  -= 1;\
	}\
	(t1).tv_sec -= (t2).tv_sec;\
  } while (0)


#define TIMEVAL2TIMESPEC(tv, ts)\
  do {\
	(ts).tv_nsec = (tv).tv_usec*1000;\
	(ts).tv_sec  = (tv).tv_sec;\
  } while (0)

#ifdef __cplusplus
};
#endif
