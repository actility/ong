
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

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include 	<time.h>
#include 	<sys/time.h>

#ifdef	ARCHARM
#define		COUNTLOOP	100000
#else
#define		COUNTLOOP	1000000
#endif


typedef	struct
{
	char	*name;
	unsigned int nbcall;
	float	reference;	// valeur pour 1 coeur d'1 Q8300 2.5Ghz
	time_t	(*tfunction)();
}	t_test;


time_t	TestMemset();
time_t	TestMallocmemsetfree();
time_t	TestGetset();
time_t	TestStrcasecmp();
time_t	TestFloat();
time_t	TestCallfunction();

int	Count;
int	Count2;

t_test	TbTest[] =
{
{
"memset 4K"			,COUNTLOOP*10,	7751938,	TestMemset
},
{
"malloc+memset+free 512o"	,COUNTLOOP*10,	11363636,	TestMallocmemsetfree
},
{
"*pt,st->xxx++,if(*pt>x&&*pt<y)",COUNTLOOP,	638977,		TestGetset
},
{
"strcpy+strcasecmp"		,COUNTLOOP*10,	11904762,	TestStrcasecmp
},
{
"float"				,COUNTLOOP*10,	8718396,	TestFloat
},
{
"callfunctions"			,COUNTLOOP*100, 136612016,	TestCallfunction
},
{
NULL,
},
};

int	NbTest	= sizeof(TbTest)/sizeof(t_test);


static	time_t	rtl_tmms()	/* TS */
{
	static	time_t base;
	struct timeval tv;
	struct timezone tz;
	time_t	ret;

	gettimeofday(&tv,&tz);
	if(!base)
	{
		base	= tv.tv_sec;
	}
	tv.tv_sec -= base;

#ifdef SCO32v4
	tv.tv_usec%=1000000;
#endif

	ret	= (tv.tv_usec/1000)+(1000*tv.tv_sec);

	return	ret;
}

time_t	TestMemset(unsigned int nb)
{
    	time_t  t0,t1;
	int	i ;

	t0 = rtl_tmms();
	for	(i = 0 ; i < nb ; i++)
	{
		char	buf[4096];

		memset	(buf,'a',sizeof(buf));
		if	(buf[100] == 'a')
		{
			Count++;
			buf[100]	= 'A';
		}
	}
	t1 = rtl_tmms();
	t1 = t1 - t0;
	return	t1;
}

time_t	TestMallocmemsetfree(unsigned int nb)
{
    	time_t  t0,t1;
	int	i ;

	t0 = rtl_tmms();
	for	(i = 0 ; i < nb ; i++)
	{
		char	*pt;

		pt	= (char *)malloc(512);
		memset	(pt,0,512);
		if	(pt[100] == 'a')
			pt[100]	= 'A';
		free	(pt);
	}
	t1 = rtl_tmms();
	t1 = t1 - t0;
	return	t1;
}

time_t	TestGetset(unsigned int nb)
{
    	time_t  t0,t1;
	int	i ;

	t0 = rtl_tmms();
	for	(i = 0 ; i < nb ; i++)
	{
		char	*pt;
		int	j;
		struct str
		{
			char	buf[256];
			int	letter;
			int	other;
		} st, *pst;

		pst	= &st;
		pt	= &st.buf[0];
		for (j = 0 ; j < sizeof(st.buf) ; j++)
		{
			if ( *pt >= 'a' && *pt <= 'z' )
				pst->letter++;
			else
				pst->other++;
			pt++;
		}
		Count	= pst->letter - pst->other;

	}
	t1 = rtl_tmms();
	t1 = t1 - t0;
	return	t1;
}

time_t	TestStrcasecmp(unsigned int nb)
{
    	time_t  t0,t1;
	int	i ;

	t0 = rtl_tmms();
	for	(i = 0 ; i < nb ; i++)
	{
		int	ok = 0;
		char	buf[128];
		char	*target = "c 'est une chaine";

		strcpy	(buf,target);
		if	(strcasecmp(buf,target) == 0)
		{
			buf[2]	= '\0';
			ok++;
			Count++;
		}
	}
	t1 = rtl_tmms();
	t1 = t1 - t0;
	return	t1;
}


time_t	TestFloat(unsigned int nb)
{
    	time_t  t0,t1;
	unsigned int	i;
	float	f1,f2;

	f1	= 2.2;
	f2	= 2.1;
	t0 = rtl_tmms();
#if	1
	for	(i = 0 ; i < nb ; i++)
	{
		f1	= f1 * f2;
		if	(f1 == 3.12)
		{
			f1 = f1 / 13.745;
			printf	(":coucou\n");
			Count++;
		}
	}
#else
	for	(i = 0 ; i < nb ; i++)
	{
		f1	= f1 * f2;
		if	(f1 >= 10.23 * 4000.456)
		{
			f1 = f1 / 13.745;
			f1 = 2.2;
			f2 = 2.1;
			Count++;
		}
	}
#endif
	t1 = rtl_tmms();
	t1 = t1 - t0;
	return	t1;
}

float	AFunction(float f1,float f2,int i)
{
	if (f1 == f2)
		Count--;
	else
		Count++;
	return	3.14;
}

time_t	TestCallfunction(unsigned int nb)
{
    	time_t  t0,t1;
	unsigned int	i;
	float	f1,f2;

	f1	= 2.2;
	f2	= 2.1;
	Count	= 0;
	printf("count=%d\n",Count);
	t0 = rtl_tmms();
	for	(i = 0 ; i < nb ; i++)
	{
		f1	= AFunction(f1,f2,i);
	}
	t1 = rtl_tmms();
	t1 = t1 - t0;
	return	t1;
}

int	main(int argc,char *argv[])
{
    	time_t  t;
	int	i;
	unsigned int	nb;
	float	nbpersec;
	char	*name;
	float	ref;
	float	coeff;
	char	oper;


	for	(i = 0 ; i < NbTest ; i++)
	{
		name	= TbTest[i].name;
		ref	= TbTest[i].reference;
		nb	= TbTest[i].nbcall;
		if	(!name || !*name)
			break;
		t	= (*TbTest[i].tfunction)(nb);

		nbpersec= (float)(nb/(float)((float)(t)/1000.0));
		if	(ref > nbpersec)
		{
			coeff	= ref/nbpersec;
			oper	= '/';
		}
		else
		{
			coeff	= nbpersec/ref;
			oper	= 'x';
		}
		printf("perf=%s tm=%d nb/s=%f coeff= (%c) %f\n",
			name,(int)t,nbpersec,oper,coeff);
	}
	return	0;

}
