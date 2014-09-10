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

#define VERSION 2

/*
	Algorithm taken from http://jean-paul.cornec.pagesperso-orange.fr/heures_lc.htm

	Itself taken from :
	J. Meeus et D. Savoie :L'Equation du Temps, l'Astronomie, Vol 109, juin 1995, pp 188-193
	D. Savoie : La Gnomonique , Ed. Les Belles Lettres , 2001 .
	C. Dumoulin, J.P. Parisot : Astronomie pratique et informatique, Ed. Masson, 1987.
	J.Meeus : Astronomical Algorithms. William-Bell Inc., 1991


	Values updated for better precision from http://fr.wikipedia.org/wiki/Équation_du_temps
		(see VERSION 2)
*/
/*
	Useful links :
	Sun rise/set : http://www.sunrisesunset.com/calendar.asp
	Epoch : http://www.epochconverter.com
	IMCCE : http://www.imcce.fr/fr/ephemerides/phenomenes/rts/rts.php
	Julian Date : http://aa.usno.navy.mil/data/docs/JulianDate.php
	Sunset/Sunrise : http://aa.usno.navy.mil/data/docs/RS_OneYear.php
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "astro.h"

double dtr = M_PI/180.0;
double rtd = 180.0/M_PI;
inline double sind(double x) { return sin(dtr*x); }
inline double cosd(double x) { return cos(dtr*x); }
inline double tand(double x) { return tan(dtr*x); }
inline double asind(double x) { return asin(x)*rtd; }
inline double acosd(double x) { return acos(x)*rtd; }
inline double atan2d(double x, double y) { return atan2(x,y)*rtd; }

#ifdef MAIN


static char *display_hhmm(double h) {
	double min = (h - floor(h))*60;
	static char tmp[20];
	sprintf (tmp, "%02d:%02d", (int)h, (int)min);
	return tmp;
}

static char *display_hhmmss(double h) {
	if	(h < 0)
		h	+= 24;
	else if	(h >= 24)
		h	-= 24;
	double min = (h - floor(h))*60;
	double sec = (min - floor(min))*60;
	static char tmp[20];
	sprintf (tmp, "%02d:%02d:%02d", (int)h, (int)min, (int)sec);
	return tmp;
}


void astro_display(astro_t *ast) {
	printf ("Lat=%f Lon=%f\n", ast->Lat, ast->Lon);
	//printf ("j=%d\n", ast->j);
	printf ("JD=%f\n", ast->JD);
	printf ("j=%f\n", ast->j);
	printf ("M=%f\n", ast->M);
	printf ("C=%f\n", ast->C);
	printf ("L=%f\n", ast->L);
	printf ("R=%f\n", ast->R);
	printf ("EoT=%f\n", ast->EoT);
	printf ("Dec=%f\n", ast->Dec);
	printf ("cosHo=%f\n", ast->cosHo);
	printf ("Ho=%f\n", ast->Ho);
	//printf ("Az=%f\n", ast->Az);
	printf ("VL=%f\n", ast->VL);
	printf ("TL=%f => %s (UTC)\n", ast->TL, display_hhmmss(ast->TL));
	printf ("HL=%f => %s (Loc)\n", ast->HL, display_hhmmss(ast->HL));
	printf ("VC=%f\n", ast->VC);
	printf ("TC=%f => %s (UTC)\n", ast->TC, display_hhmmss(ast->TC));
	printf ("HC=%f => %s (Loc)\n", ast->HC, display_hhmmss(ast->HC));

struct tm *tm;

printf ("\033[31m");
	printf ("TL_epoch=%ld\n", ast->TL_epoch);
	tm = gmtime(&ast->TL_epoch);
printf ("\033[34m");
	printf ("Sunrise UTC %s", asctime(tm));
printf ("\033[0m");
	tm = localtime(&ast->TL_epoch);
	printf ("Sunrise LOC %s", asctime(tm));

printf ("\033[31m");
	printf ("TC_epoch=%ld\n", ast->TC_epoch);
	tm = gmtime(&ast->TC_epoch);
printf ("\033[34m");
	printf ("Sunset UTC %s", asctime(tm));
printf ("\033[0m");
	tm = localtime(&ast->TC_epoch);
	printf ("Sunset LOC %s", asctime(tm));

	printf ("h=%f => %s\n", ast->h, display_hhmmss(ast->h));
	printf (ast->DayTime ? "day\n" : "night\n");
}
#endif

double JD(int y, int m, double d) {
	if	(m == 1 || m == 2) {
		y --;
		m += 12;
	}
	int	A = y/100;
	int	B = 2 - A + A/4;
	double	JD;
	JD = 36525*(y+4716)/100 + 306*(m+1)/10 + d + B - 1524.5;
	//printf ("y=%d m=%d d=%f A=%d B=%d JD=%f\n", y, m, d, A, B, JD);
	return	JD;
}
/*
Lon : longitude Est+ West-
Lat : latitude North+ South-
j représente le rang du jour dans l'année (1er janvier = 1)
M est l'anomalie moyenne en degrés
C est l'équation du centre (influence de l'ellipticité de l'orbite terrestre) en degrés
L est la longitude vraie du Soleil en degrés
R est la réduction à l'équateur (influence de l'inclinaison de l'axe terrestre) en degrés
*/

void astro_calculate(astro_t *ast) {
	if	(!ast->tm) {
		time_t t = time(0);
		ast->tm	= localtime(&t);
	}
/*
	if	(!ast->Lat && !ast->Lon) {
		ast->Lat	= 45.7500000000;
		ast->Lon	= 4.8333333333;
	}
*/

/*
	ast->j = ast->tm->tm_yday;
*/
	ast->JD = JD (ast->tm->tm_year+1900, ast->tm->tm_mon+1, ast->tm->tm_mday + 0.5);	// 0.5 is 12:00
	ast->j = ast->JD - 2451545.0;	// 2000.0

#if (VERSION==1)
	ast->M = 357 + 0.9856 * ast->j;
	ast->C = 1.914 * sind(ast->M) + 0.02 * sind(2*ast->M);
	ast->L = 280 + ast->C + 0.9856 * ast->j;
	ast->R = -2.465 * sind(2*ast->L) + 0.053 * sind(4*ast->L);
#endif
#if (VERSION==2)
	ast->M = 357.5291 + 0.98560028 * ast->j;
	ast->C = 1.9146 * sind(ast->M) + 0.02 * sind(2*ast->M) + 0.0003 * sind(3*ast->M);
	ast->L = 280.4665 + ast->C + 0.98564736 * ast->j;
	ast->R = -2.4680 * sind(2*ast->L) + 0.053 * sind(4*ast->L) - 0.0014 * sind(6*ast->L);
#endif

	ast->EoT = -(ast->C + ast->R) * 4;

	//	0.397777 représente le sinus de l'obliquité de l'écliptique (23.43929)
	ast->Dec = asind(0.397777 * sind(ast->L));

	//double RA = atan2d(0.9175*sind(ast->L), cosd(ast->L));

	/*
	Le Soleil se lève ou se couche quand le bord supérieur de son disque apparaît ou disparait à l'horizon.
	Du fait de la réfraction atmosphérique le centre du Soleil est alors à 50' sous l'horizon : 34' pour
	l'effet de la réfraction et 16' pour le demi-diamètre du Soleil. L'angle horaire Ho du Soleil, en degrés,
	au moment où son bord supérieur est sur l'horizon est donné par :
	*/
#define UPPER_LIMB -0.01454		// sind(50/60);
#define BUREAU_DES_LONGITUDES -0.01065	// sind(36.6/60);
#define CREPUSCULE_CIVIL -0.105
#define CREPUSCULE_NAUTIQUE -0.208
#define CREPUSCULE_ASTRONOMIQUE -0.309

	/*
	Si vous comparez aux valeurs données par le Bureau des Longitudes (IMCCE) vous constaterez qu'elles sont franchement différentes. En effet le BdL calcule les heures des lever/coucher du centre du Soleil avec une réfraction à l'horizon de 36,6'. Pour vous assurer de la justesse de vos calculs remplacez 0,01454 par 0,01065 dans l'expression de Ho.
	*/

#define Occultation UPPER_LIMB
//#define Occultation BUREAU_DES_LONGITUDES

	ast->cosHo = (Occultation - sind(ast->Dec)*sind(ast->Lat)) / (cosd(ast->Dec)*cosd(ast->Lat));

	/* Si la valeur du cosinus est supérieure à 1 il n'y a pas de lever (et pas de coucher), le Soleil est
	toujours sous l'horizon; si elle est inférieure à -1 il n'y a pas de coucher (et pas de lever), le
	Soleil est toujours au dessus de l'horizon.
	*/

	if	(ast->cosHo < -1) {
		ast->DayTime	= 1;
		return;
	}
	if	(ast->cosHo > 1) {
		ast->DayTime	= 0;
		return;
	}

	ast->Ho = acosd(ast->cosHo);

	//	Azimut
	//ast->cosAz = ( X * sind(ast->Lat) - sind(ast->Dec)) / cosd(ast->Lat);
	//ast->Az = acos(ast->cosAz);

	//	Lever
	ast->VL = 12 - ast->Ho/15;		// Heure vraie
	ast->TL = ast->VL - ast->EoT/60 - ast->Lon/15; // Heure UTC
	ast->HL = ast->TL + ast->tm->tm_gmtoff/3600; // Heure légale

	//	Coucher
	ast->VC = 12 + ast->Ho/15;	// Heure vraie;
	ast->TC = ast->VC - ast->EoT/60 - ast->Lon/15; // Heure UTC
	ast->HC = ast->TC + ast->tm->tm_gmtoff/3600; // Heure légale

	//	Epoch calculation
	struct tm tm	= *ast->tm;
	tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
	time_t hour0	= mktime(&tm);
	hour0	+= tm.tm_gmtoff;
	ast->TL_epoch	= hour0 + ast->TL*3600;
	ast->TC_epoch	= hour0 + ast->TC*3600;

	ast->h = (double)ast->tm->tm_hour + (double)ast->tm->tm_min/60.0 + (double)ast->tm->tm_sec/3600.0;

	if	((ast->h > ast->HL) && (ast->h < ast->HC)) {
		ast->DayTime	= 1;
		return;
	}
	else {
		ast->DayTime	= 0;
		return;
	}
}

#ifdef MAIN

#include "unitTest.h"

#define FAIL 1
#define SUCCESS 0

/**
   Values are taken from http://aa.usno.navy.mil/data/docs/RS_OneYear.php
 */
int do_test() {
	astro_t	ast;
	struct tm tm;
	double accuracy = 1.0 / 60.0;	// one min
	char *testMsg;

	memset (&ast, 0, sizeof(ast));
	memset (&tm, 0, sizeof(tm));

/** Test #1 : 2000/1/1 at 50N/0E : set=0759 rise=1608 UT
*/
	testMsg = "Test #1 : 2000/1/1 at 50N/0E";
	ast.Lat	= 50.0;
	ast.Lon	= 0.0;
	tm.tm_year	= 100;
	tm.tm_mon	= 0;
	tm.tm_mday	= 1;
	time_t t	= mktime(&tm);
	ast.tm	= localtime(&t);

	astro_calculate (&ast);
	astro_display (&ast);
	if	(ast.j != 0.0) return	FAIL;
/*
	if	(STAssertEqualsWithAccuracy(ast.TL, 7.0 + 59.0 / 60.0, accuracy, testMsg) == False) return FAIL;
	if	(STAssertEqualsWithAccuracy(ast.TC, 16.0 + 8.0 / 60.0, accuracy, testMsg) == False) return FAIL;
*/
	STAssertEqualsWithAccuracy(ast.TL, 7.0 + 59.0 / 60.0, accuracy, testMsg);
	STAssertEqualsWithAccuracy(ast.TC, 16.0 + 8.0 / 60.0, accuracy, testMsg);

/** Test #2 : 2000/1/1 at 50N/10E : set=0719 rise=1528 UT
*/
	testMsg = "Test #2 : 2000/1/1 at 50N/10E";
	ast.Lat	= 50.0;
	ast.Lon	= 10.0;
	astro_calculate (&ast);
	astro_display (&ast);
	STAssertEqualsWithAccuracy(ast.TL, 7.0 + 19.0 / 60.0, accuracy, testMsg);
	STAssertEqualsWithAccuracy(ast.TC, 15.0 + 28.0 / 60.0, accuracy, testMsg);

/** Test #3 : 2000/1/1 at 65N/10E : set=0927 rise=1321 UT
*/
	testMsg = "Test #3 : 2000/1/1 at 65N/10E";
	ast.Lat	= 65.0;
	ast.Lon	= 10.0;
	astro_calculate (&ast);
	astro_display (&ast);
	STAssertEqualsWithAccuracy(ast.TL, 9.0 + 27.0 / 60.0, accuracy, testMsg);
	STAssertEqualsWithAccuracy(ast.TC, 13.0 + 21.0 / 60.0, accuracy*2, testMsg);

/** Test #4 : 2000/1/1 at 20N/0E : set=0635 rise=1732 UT
*/
	testMsg = "Test #4 : 2000/1/1 at 20N/0E";
	ast.Lat	= 20.0;
	ast.Lon	= 0.0;
	astro_calculate (&ast);
	astro_display (&ast);
	STAssertEqualsWithAccuracy(ast.TL, 6.0 + 35.0 / 60.0, accuracy, testMsg);
	STAssertEqualsWithAccuracy(ast.TC, 17.0 + 32.0 / 60.0, accuracy, testMsg);

/** Test #5 : 2000/1/1 at 0N/0E : set=0600 rise=1807 UT
*/
	testMsg = "Test #5 : 2000/1/1 at 0N/0E";
	ast.Lat	= 0.0;
	ast.Lon	= 0.0;
	astro_calculate (&ast);
	astro_display (&ast);
	STAssertEqualsWithAccuracy(ast.TL, 6.0 + 0.0 / 60.0, accuracy, testMsg);
	STAssertEqualsWithAccuracy(ast.TC, 18.0 + 7.0 / 60.0, accuracy, testMsg);

	STStatistics();

	if	(STErrorCount() != 0)
		return FAIL;
	return	SUCCESS;
}

int main(int ac, char **av) {

	if	(ac >= 2 && !strcmp(av[1], "-test")) {
		int rc = do_test();
		//fprintf (stderr, "do_test %s\n", rc ? "failed" : "succeed");
		exit (rc);
	}
	astro_t	ast;
	struct tm tm;
	memset (&ast, 0, sizeof(ast));
	memset (&tm, 0, sizeof(tm));

	printf ("Usage : astro <lat> <lon> <y> <m> <d>\n");

	ast.Lat	= ac >= 2 ? atof(av[1]) : 65;
	ast.Lon	= ac >= 3 ? atof(av[2]) : 0;

	tm.tm_year	= ac >= 4 ? atoi(av[3]) - 1900 : 113;;
	tm.tm_mon	= ac >= 5 ? atoi(av[4])-1 : 0;
	tm.tm_mday	= ac >= 6 ? atoi(av[5]) : 1;
	tm.tm_gmtoff	= 0;

	time_t t	= mktime(&tm);
	ast.tm	= localtime(&t);

	astro_calculate (&ast);
	astro_display (&ast);

	return 0;
}

#endif
