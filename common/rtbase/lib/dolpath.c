
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


/*
 *	Remplacement des sequences ${...}
 */

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>

static	char	*rtl_getvar(char *name)
{
	char	*val ;

	if	( !name || !*name )
		return	"" ;
	val	= (char *)getenv(name) ;
	if	( !val )
		return	"" ;
	return	val ;
}

/*
 *	cherche si *c est egal a un des car de lst
 */

static	char	*rtl_isIn(char *c,char *lst)
{
	while	(*lst)
	{
		if	( *c == *lst )
			return	c;
		lst++;
	}
	return	NULL;
}

/*
 *	cherche dans *in le 1er car. qui est egal a un des car. de *end
 */

static	char	*rtl_skipto(char *in,char *end)
{
	char	*pt ;
	char	*pos ;

	pt	= in;
	while	(*pt)
	{
		pos	= rtl_isIn(pt,end) ;
		if	( pos )
		{
			return	pos ;
		}
		pt++ ;
	}
	return	strchr(in,0) ;
}


/*
 *	copie la chaine <in> dans la chaine <out> en evaluant les
 *	variables $XXXX par getenv()
 *
 *	<in> ne peut etre une variable static
 *
 *	les formes reconnues sont :
 *		${NOM_VAR}
 *		$(NOM_VAR)
 *		$NOM_VAR<delimiteur>
 *		delimiteur == '\\' '/' '.' '$' '\0' '<' '>' ' ' '\t'
 *
 *	si fctvar(char *name) != NULL il est possible de specifier
 *	une fonction d'evaluation de variables , qui doit retourner
 *	le contenu de la variable name ou la chaine vide .
 */

char	*rtl_evalPath( char *in, char *out, int lgmax, char *(*fctvar)(char *v))
{
	int	lg	= 0 ;
	char	*svout	= out ;
	char	*ptvar ;
	char	*ptin ;
	char	*ptskip ;
	char	*value ;
	char	svcar ;
	int	lgvalue ;
	char	flgskip ;
	char	varname[128];
	int	l;

	if	(!out)
		return	NULL;

	if	( fctvar == NULL )
		fctvar	= rtl_getvar ;
	*out	= '\0' ;

	while	( *in && lg < lgmax )
	{
		if	( *in == '$' )
		{	/* une variable	*/
			in++ ;
			switch	(*in)
			{
			case	'{' : flgskip	= 1 ; ptskip	= "}" ; in++ ;
			break ;
			case	'(' : flgskip	= 1 ; ptskip	= ")" ; in++ ;
			break ;
			default :
				flgskip	= 0 ;
				ptskip	= "\\/.$<> \t" ;
			break ;
			}
#if	0	// if <in> is in code segment => core ...
			ptvar	= in ;
			ptin	= rtl_skipto(in,ptskip) ;
			svcar	= *ptin ;
			*ptin	= '\0' ;
			value	= (*fctvar)(ptvar) ;
			if	( !value )
				value	= "" ;
			*ptin	= svcar ;
#endif
			ptvar	= varname;
			ptin	= rtl_skipto(in,ptskip) ;
			if	(!ptin || !*ptin)
				break;
			l	= (ptin - in)+1;
			if	(l <= 0 || l >= sizeof(varname))
				break;
			strncpy	(varname,in,l);
			varname[l-1]	= '\0';
			value	= (*fctvar)(ptvar) ;
			if	( !value )
				value	= "" ;
//printf("varname='%s' => val='%s'\n",varname,value);

			lgvalue	= strlen(value) ;
			if	( lgvalue > 0 )
			{
				if	( lg + lgvalue >= lgmax )
				{
					lgvalue	= lgmax - lg ;
				}
				memcpy	(out,value,lgvalue) ;
				out	+= lgvalue ;
				lg	+= lgvalue ;
			}
			if	( *ptin )
			{
				if	( *ptin == '$' )
				{
					in	= ptin ;
				}
				else
				{
					if	( !flgskip )
					{
						*out	= *ptin ;
						out++ ;
						lg++ ;
					}
					in	= ptin+1 ;
				}
			}
			else
				break ;
		}
		else
		{	/* caractere normal a recopier */
			*out	= *in ;
			in++ ;
			out++ ;
			lg++ ;
		}
	}
	*out	= '\0' ;
	return	svout ;
}


