
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



/*  RTL Technologies */
/*  Keyxord             : %W% */
/*  Current module name : %M% */
/*  SCCS file name      : %F% */
/*  SID                 : %I% */
/*  Delta Date          : %E% */
/*  Current date/time   : %D%,%T% */

#include	<stdio.h>
#include	<stdlib.h>
#ifndef		DOS
#include	<unistd.h>
#endif
#include	<string.h>
#include	<ctype.h>
#include	<memory.h>
#include	<errno.h>

#include	"xoapipr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

static	char	*xo_file = __FILE__ ;

#if	1 || defined(AIX) || defined(HP)
int32	_GET_BIT(unsigned char *pt,int32 n)
{
	return	((*(pt+((n)/8))) & (1 << (7-((n) % 8)))) ;
}
int32	_SET_BIT(unsigned char *pt,int32 n)
{
	return	((*(pt+((n)/8))) |= (1 << (7-((n) % 8)))) ;
}
int32	CLR_BIT(unsigned char *pt,int32 n)
{
	return	((*(pt+((n)/8))) &= ~(1 << (7-((n) % 8)))) ;
}
#else
#define	_GET_BIT(pt,n) (((unsigned char)*(pt+((n)/8))) & (1 << (7-((n) % 8))))
#define	_SET_BIT(pt,n) (((unsigned char)*(pt+((n)/8))) |= (1 << (7-((n) % 8))))
#define	CLR_BIT(pt,n) (((unsigned char)*(pt+((n)/8))) &= ~(1 << (7-((n) % 8))))
#endif

#define	GET_BIT(pt,num)	(_GET_BIT((pt),(num)) ? 1 : 0 )
#define	SET_BIT(pt,num,v) ((v) ? _SET_BIT((pt),(num)) : CLR_BIT((pt),(num)))

static	unsigned	char GET_BYTE_AT_BIT(unsigned char *pt,int32 num)
{
	int32	ind ;
	int32	mod ;

	ind	= num / 8 ;
	mod	= num % 8 ;

	if	( mod == 0 )
		return	(unsigned char)pt[ind] ;
	return	( ( pt[ind] << mod ) | ( pt[ind+1] >> ( 8 - mod ) ) ) ;
}

static	void	SET_BYTE_AT_BIT(unsigned char *pt,int32 num,unsigned char val)
{
	int32	ind ;
	int32	mod ;
	unsigned	char	mv1 ;
	unsigned	char	mv2 ;

	ind	= num / 8 ;
	mod	= num % 8 ;
	if	( mod == 0 )
	{
		pt[ind] = val ;
		return ;
	}

	mv1		= (unsigned char)( 0xff << ( 8 - mod ) ) ;
	pt[ind]		&= mv1 ;
	pt[ind]		|= (unsigned char)( val >> mod ) ;

	mv2		= (unsigned char)( 0xff >> mod ) ;
	pt[ind+1]	&= mv2 ;
	pt[ind+1]	|= (unsigned char)( val << ( 8 - mod ) ) ;
}

static	uint32	comprime(unsigned char val,unsigned char *in,int32 lgin,unsigned char *out)
{
	int32	i ;
	uint32	nbbit ;

	nbbit	= 0 ;
	for	( i = 0 ; i < lgin ; i++ , in++ )
	{
		if	( ( nbbit + 8 ) / 8 >= lgin )
			return	0 ;
		if	( *in == val )
		{
			SET_BIT(out,nbbit,0) ;
			nbbit++ ;
		}
		else
		{
			SET_BIT(out,nbbit,1) ;
			nbbit++ ;
			SET_BYTE_AT_BIT(out,nbbit,*in) ;
			nbbit	+= 8 ;
		}
	}
	return	nbbit ;
}

static	int32	decomprime(unsigned char val,unsigned char *in,uint32 lginbits,unsigned char *out)
{
	int32	i ;
	int32	nboct ;

	nboct	= 0 ;
	for	( i = 0 ; i < lginbits ; out++ , nboct++ )
	{
		if	( GET_BIT(in,i) )
		{
			i++ ;
			*out	= GET_BYTE_AT_BIT(in,i) ;
			i	+= 8 ;
		}
		else
		{
			i++ ;
			*out	= val ;
		}
	}

	return	nboct ;
}

//	n	= le nombre d'octets à compresser
//	n0	= le nombre d'octets == 0
//	nn	= le nombre d'octets != 0
//
//	n0 + 9nn < (n0 + nn) * 8
//	=> n0 / n > 1/8 ou n0 / n > 0.125

int32	XoMemCanDeflate(char *in,int32 lgin)
{
	int32	nb	= lgin;
	int32	nb0	= 0;

	if	(!in || lgin <= 0)
		return	0;
	while	(lgin-- > 0)
	{
		if	(*in == '\0')
			nb0++;
		in++;
	}
	if	(((float)nb0 / (float)nb) >= 0.13)
	{
		float	bout = nb0 + (9 * (nb - nb0));
		float	bin = 8 * nb;

		if	(bout > bin)	// ???
			return	0;

		return	(int)(100 - (bout/bin*100.0));
	}
	return	0;
}


int32	XoMemDeflate(char *in,int32 lgin,char *out)
{
	uint32	nbbit;
	uint32	left;
	int32	nbbyt;

	nbbit	= comprime(0,(unsigned char*)in,lgin,(unsigned char*)out+1) ;
	if	(nbbit == 0)	// compress > original => stop
		return	0;
	left	= nbbit % 8;	
	*out	= left;
	if	(left == 0)
		nbbyt =	nbbit / 8;
	else
		nbbyt =	(nbbit / 8) + 1;
	nbbyt	= nbbyt + 1;	// out[0]
//printf("deflate => lgin=%d nbbit=%d nbbyt=%d left=%d\n",lgin,nbbit,nbbyt,*out);
	return	nbbyt;
}

int32	XoMemInflate(char *pin,int32 lgin,char *out)
{	
	unsigned char *in = (unsigned char *)pin;
	uint32	nbbit;
	uint32	left;

	lgin	= lgin - 1;	// in[0]
	if	(lgin <= 2)
		return	0;
	left	= *in;
	if	(left >= 8)
	{
		XOERR("inflate error",left);
		return	0;
	}
	if	(left)
		nbbit	= (lgin * 8) - (8 - (left));
	else
		nbbit	= (lgin * 8);
//printf("inflate => lgin=%d nbbit=%d left=%d\n",lgin+1,nbbit,left);
	return	decomprime(0,(unsigned char *)in+1,nbbit,(unsigned char *)out) ;
}

/* old versions with nbbit ... */

int32	XoMemCompress(unsigned char *in,int32 lgin,unsigned char *out)
{
	/* les sauvegardes xo sont remplies de 0 */
	/* return nbbit */
	return	(int32)comprime(0,in,lgin,out) ;
}

int32	XoMemUnCompress(unsigned char *in,int32 lginbits,unsigned char *out)
{
	return	decomprime(0,in,lginbits,out) ;
}
