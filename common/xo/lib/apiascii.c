
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
/*  Keyword             : %W% */
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

/*
#define	TRACE_GET
#define	TRACE_COMM
*/

#define	MAX_LG_word	128

#define	UNSET_VALUE	"unset"
#define	IS_AFFECTATION	"is"

#define	TXT_EOT		"\004\n"
#define	BUF_EOT		"---\n"

#define	DEBUT_OBJ	"{"
#define	FIN_OBJ		"}"

#define	DEBUT_LISTE	"["
#define	FIN_LISTE	"]"

#define	NICE_CHAR(c)	( (unsigned char)(c) > ' ' && (unsigned char)(c) < 127 )

static	char	StartFlux[128] ;
static	char	EndFlux[128] ;

void	XoStartEndFluxAscii(char *pstart,char *pend)
{
	strcpy	(StartFlux,pstart) ;
	strcpy	(EndFlux,pend) ;
}

/*
 *	partie sauvegarde thread safe mais pas fprintf() (SF)
 */

static	int32	xosaveascii(void *obj,FILE *fout,int32 level,char *svnameobj) ;
static	int32	xosaveasciiattr(void *obj, int32 numattr, 
						FILE *fout, int32 level) ;
static	void xosaveasciivalue(int32 type, char *value, int32 lg,
						FILE *fout, int32 level) ;

static	void	local_tab(int32 lev, FILE *fout)
{
	char	tmp[128] ;

	lev	*= 3 ;

	memset	(tmp,' ',128) ;
	tmp[lev] = '\0' ;
	fprintf(fout,"%s",tmp) ;
}


static	void xosavestringvalue(char *value, FILE *fout, int32 level)
{
	char	*rc ;

	local_tab(level,fout) ;
	rc	= strchr(value,'\n') ;
	if	( !rc )
	{	/* pas de retour chariot =>   chp = aaaaaaa */
		fprintf(fout," = %s\n",value) ;
	}
	else
	{
		fprintf(fout," :\n") ;
		fprintf(fout,"%s\n",value) ;
		fprintf(fout,TXT_EOT) ;
	}
}

static	void xosavebuffervalue(char *value, int32 lg, FILE *fout, int32 level)
{
	unsigned	char	*v ;
	int		i ;
	int		n ;

	local_tab(level,fout) ;
	v	= (unsigned char *)value ;
	if	( lg <= 25 )
	{
		fprintf(fout," =") ;
		for	( i = 0 ; i < lg ; i++ )
		{
			fprintf	(fout," %02x",v[i]) ;
		}
		fprintf(fout,"\n") ;
	}
	else
	{
		fprintf(fout," : \n") ;
		for	( i = 0 , n = 1 ; i < lg ; i++ , n++ )
		{
			fprintf	(fout," %02x",v[i]) ;
			if	( n % 25 == 0 )
				fprintf(fout,"\n") ;
		}
		fprintf(fout,"\n") ;
		fprintf(fout,BUF_EOT) ;
	}
}

static	void xosaveasciivalue(int32 type, char *value, int32 lg, FILE *fout, int32 level)
{
	int32	l ;
	char	c ;

	switch	(type)
	{
	case	XoBool :
	case	XoInt4B :
	case	XoDate :
		l	= *(int32 *)value ;
		local_tab(level,fout) ;
		fprintf(fout," = %ld\n",(long)l) ;
	break ;
	case	XoChar :
		c	= *(char *)value ;
		local_tab(level,fout) ;
		if	( NICE_CHAR(c) )
			fprintf(fout," = %c\n",c) ;
		else
			fprintf(fout," = %ld\n",(long)c) ;
	break ;
#if	0
	case	XoVoid :
		fprintf(fout," = %ld\n",(long)value) ;
	break ;
#endif
	case	XoString :
		xosavestringvalue(value,fout,level) ;
	break ;
	case	XoBuffer :
		xosavebuffervalue(value,lg,fout,level) ;
	break ;
	default :
		fprintf(fout,"\n") ;
	break ;
	}
}

static	int32	xosaveasciiattr(void *obj, int32 numattr, FILE *fout, int32 level)
{
	t_attribut_ref	*ar ;
	int32	type ;
	int32	mode ;
	char	*nameattr ;

	int32	lg ;
	int32	nb ;
	void	*value ;

	int32	pos ;

	char	*svObjName	= ":=";

	nameattr	= XoGetRefAttrName(numattr) ;
	if	( !nameattr )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	ar	= XoGetRefAttr(numattr) ;
	if	( !ar )
	{
		XOERR("",0) ;
		return	-11 ;
	}

	if	( IS_NO_SAVE(ar) /* || ar->ar_type == XoVoid */ )
		return	0 ;

	if	(strcmp(ar->ar_type_name,"XoVoidObj") == 0)
		svObjName	= "";

	mode	= XoModeAttr(numattr) ;
	if	( mode < 0 )
	{
		XOERR("",0) ;
		return	-2 ;
	}

	type	= XoTypeAttr(numattr) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-3 ;
	}

	if	(mode==BASIC_ATTR && IS_SAVE_UNSET(ar) && 
					XoIsSetAttr(obj,numattr)==0)
	{
		/* attribut non valorise et flag saveUnset */

		local_tab(level,fout) ;
		fprintf(fout,"%s %s\n",nameattr,UNSET_VALUE) ;
		fflush(fout) ;
		return	1 ;
	}

	switch	(mode)
	{
	case	BASIC_ATTR :
		if	( XoAttrValueIsDef(obj,numattr) )
			break ;
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( value )
		{
			local_tab(level,fout) ;
			fprintf(fout,"%s ",nameattr) ;
			fflush(fout) ;
			xosaveasciivalue(type,value,lg,fout,level) ;
		}
	break ;
	case	LIST_BASIC_ATTR :
		nb	= XoNbInAttr(obj,numattr) ;
		if	( nb <= 0 )
			break ;
		local_tab(level,fout) ;
		fprintf(fout,"%s ",nameattr) ;
		fflush(fout) ;
		local_tab(level,fout) ;
		fprintf(fout,"\n") ;
		local_tab(level,fout) ;
		fprintf(fout,"%s\n",DEBUT_LISTE) ;
		fflush(fout) ;

		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				xosaveasciivalue(type,value,lg,fout,level) ;
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
		local_tab(level,fout) ;
		fprintf(fout,"%s\n",FIN_LISTE) ;
		fflush(fout) ;
	break ;
	case	OBJ_ATTR :
		value	= XoGetAttr	(obj,numattr,&lg) ;
		if	( value )
		{
			local_tab(level,fout) ;
			fprintf(fout,"%s  ",nameattr);
			fflush(fout) ;
			xosaveascii(value,fout,level+1,svObjName) ;
		}
	break ;
	case	LIST_OBJ_ATTR :
		nb	= XoNbInAttr(obj,numattr) ;
		if	( nb <= 0 )
			break ;
		local_tab(level,fout) ;
		fprintf(fout,"%s ",nameattr) ;
		fflush(fout) ;
		local_tab(level,fout) ;
		fprintf(fout,"\n") ;
		local_tab(level,fout) ;
		fprintf(fout,"%s\n",DEBUT_LISTE) ;
		fflush(fout) ;
		pos	= XoFirstAttr(obj,numattr) ;
		while	( pos > 0 )
		{
			value	= XoGetAttr	(obj,numattr,&lg) ;
			if	( value )
			{
				xosaveascii(value,fout,level+1,svObjName) ;
			}
			pos	= XoNextAttr(obj,numattr) ;
		}
		fprintf(fout,"\n") ;
		local_tab(level,fout) ;
		fprintf(fout,"%s\n",FIN_LISTE) ;
	break ;
	default	:
		XOERR("",0) ;
		return	-3 ;
	break ;
	}

	return	1 ;

}

static	int32	xosaveascii(void *obj, FILE *fout, int32 level,char *svnameobj)
{
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;
	int32	numattr ;

	type	= XoType(obj) ;
	if	( type < 0 )
	{
		XOERR("",0) ;
		return	-1 ;
	}
	nbattr	= XoNbAttr(type) ;
	if	( nbattr < 0 )
	{
		XOERR("",0) ;
		return	-2 ;
	}
	nameobj	= XoGetRefObjName(type) ;
	if	( !nameobj )
	{
		XOERR("",0) ;
		return	-3 ;
	}

	local_tab(level,fout) ;
	if (svnameobj && *svnameobj)
		fprintf(fout,"%s\n",svnameobj) ;
	else
		fprintf(fout,"%s\n",nameobj) ;
	local_tab(level,fout) ;
	fprintf(fout,"%s\n",DEBUT_OBJ) ;
	fflush(fout) ;

	for	( a = 0 ; a < nbattr ; a++ )
	{
		numattr	= type + a + 1 ;
		xosaveasciiattr(obj,numattr,fout,level) ;
	}
	local_tab(level,fout) ;
	fprintf(fout,"%s\n",FIN_OBJ) ;
	fflush(fout) ;

	return	1 ;
}

/*DEF*/	int32	__XoSaveAscii(void *obj, FILE *fout)
{
	int32	ret ;

	if	( strlen(StartFlux) )
	{
		fprintf(fout,"%s",StartFlux) ;
		fflush(fout) ;
	}
	ret	= xosaveascii(obj,fout,0,"") ;
	if	( strlen(EndFlux) )
	{
		fprintf(fout,"%s",EndFlux) ;
		fflush(fout) ;
	}
	return	ret ;
}

/*DEF*/	int32	XoSaveAscii(void *obj, FILE *fout)
{
	int32	ret;

	ret = __XoSaveAscii(obj,fout);
	return	ret;
}


/*
 *	partie lecture non thread sage (not SF)
 */



static	char		FlagMore	= 0 ;
static	char		BuffMore[1024] ;
static	int		NumLine		= 0 ;
static	int32	IsUnComplete		= 0 ;
static	int32	AcceptUnComplete	= 0 ;
static	char		*MemBuf;
static	int32	MemLg;
static	int32	MemPos;

static	int		(*CustGetc)(void *stream)  ;
static	int		(*CustUnGetc)(int c,void *stream)  ;

/*
 *	appel a la fonction getc() ou custom getc() pour obtenir 
 *	un caractere
 */

static	int	do_getc(FILE *fin)
{
	int	ret ;

	if	(!CustGetc)	/* lecture standard	*/
	{
		return	( getc(fin) ) ;
	}
	/*
	 *	lecture custom
	 */
	ret	= (*CustGetc)((void *)fin) ;
#ifdef	TRACE_GET
fprintf	(stderr,"custom getc() %d %c\n",ret,(ret>=' '&&ret<127?ret:'?')) ;
fflush	(stderr) ;
#endif
	return	ret ;
}

/*
 *	appel a la fonction ungetc() ou custom ungetc() pour remettre 
 *	un caractere
 */

static	int	do_ungetc(int c,FILE *fin)
{
	int	ret ;

	if	(!CustUnGetc)	/* lecture standard	*/
	{
		return	( ungetc(c,fin) ) ;
	}
	/*
	 *	lecture custom
	 */
	ret	= (*CustUnGetc)(c,(void *)fin) ;
#ifdef	TRACE_GET
fprintf	(stderr,"custom ungetc() %d %c\n",c,(c>=' '&&c<127?c:'?')) ;
fflush	(stderr) ;
#endif
	return	ret ;
}

/*
 *	appel a la fonction fgets() ou utilisation de custom getc() pour 
 *	obtenir une ligne jusqu'au '\n'
 */

static	char	*do_fgets(char *s,int n,FILE *fin)
{
	char	*svs ;
	int	l ;
	int	c ;

	if	(!CustGetc)	/* lecture standard	*/
	{
		svs	= fgets(s,n,fin) ;
		if	( svs && *svs )
		{
			char	*r ;

			r	= strrchr(svs,'\r') ;
			if	( r )
				*r	= '\0' ;
		}
		return	svs ;
	}
	/*
	 *	lecture custom
	 */
	*s	= '\0' ;
	svs	= s ;
	l	= 0 ;
	c	= -1 ;
	while	( c != '\n' && l < n - 1 )
	{
		c	= do_getc(fin) ;
		if	( c == EOF )
		{
			if	( l == 0 )
				return	NULL ;
			else
				break ;
		}
		*s++	= (char)c ;
		l++ ;
	}
#ifdef	TRACE_GET
fprintf	(stderr,"custom fgets() %d\n",l) ;
fflush	(stderr) ;
#endif
	*s	= '\0' ;
	return	svs ;
}

/*
 *	permet de compter les lignes 
 */

static	char	*my_fgets(char *s,int n,FILE *fin)
{
	char	*ret ;

	*s	= '\0' ;
	ret	= do_fgets	(s,n,fin) ;
	if	( ret )
	{
		if	( strchr(s,'\n') )
		{
			NumLine	++ ;
		}
	}
	return	ret ;
}

/*
 *	scanf specifique
 *	compte les lignes
 *	pas plus de MAX_LG_word caracteres retournes dans word
 *	tient compte des separateurs ' ' '\t' '\n'
 */

static	int	my_fscanf(FILE *fin, char *fmt_not_use,char *word)
{
	int	c ;
	int	debut ;
	char	*pt ;
	int	lg ;

	lg	= 0 ;
	*word	= '\0' ;
	c	= do_getc(fin) ;
	if	( c == EOF )
		return	EOF ;

	debut	= 1 ;
	pt	= word ;
	while	( 1 )
	{
		switch	(c)
		{
			case	EOF	:
				goto	stoppe ;
			break ;
			case	' '	:
			case	'\t'	:
				if	( !debut )
				{
					do_ungetc	(c,fin) ;
					goto	stoppe ;
				}
			break ;
			case	'\r'	:
			break ;
			case	'\n'	:
				NumLine	++ ;
				if	( !debut )
				{
					NumLine	-- ;
					do_ungetc	(c,fin) ;
					goto	stoppe ;
				}
			break ;
			default		:
				debut	= 0 ;
				lg++ ;
				if	( lg >= MAX_LG_word )
				{
					XOERR	("max. token size",NumLine) ;
					do_ungetc	(c,fin) ;
					goto	stoppe ;
				}
				*pt	= (char)c ;
				pt++ ;
			break ;
		}
		c	= do_getc(fin) ;
	}

stoppe	: 
	*pt	= '\0' ;
	if	( *word == '\0' )
		return	EOF ;
	return	1 ;
}

/*
 *	replace un token en entree
 */

static	void	unget_word(char *more)
{
	strcpy	(BuffMore,more) ;
	FlagMore	= 1 ;
}

/*
 *	obtient un token en entree 
 *	gestion simplifiee de commentaires / * * / ou #
 */

static	char	*get_word(FILE *fin, char *word)
{
	if	(FlagMore)
	{
		strcpy	(word,BuffMore) ;
		FlagMore	= 0 ;
		return	word ;
	}
	while	(1)
	{
		if	( my_fscanf(fin,"%s",word) == EOF )
		{
			return	NULL ;
		}
		if	( word[0] == '#' )
		{
			char	buf[4096] ;

			my_fgets(buf,sizeof(buf),fin) ;
			continue ;
		}
		if	( word[0] == '/' && word[1] == '*' )
		{
			char	buf[4096] ;
#ifdef	TRACE_COMM
fprintf	(stderr,"debut de comm. en %d\n",NumLine) ;
fflush	(stderr) ;
#endif
			while	( my_fgets(buf,sizeof(buf),fin) )
			{
#ifdef	TRACE_COMM
fprintf	(stderr,"entre comm. '%s' en %d\n",buf,NumLine) ;
fflush	(stderr) ;
#endif
				if	( buf[0] == '*' && buf[1] == '/' )
				{
#ifdef	TRACE_COMM
fprintf	(stderr,"fin de comm. en %d\n",NumLine) ;
fflush	(stderr) ;
#endif
					break ;
				}
			}
		}
		else
			break ;
	}
	return	word ;
}

static	int32	xosetattr(void *obj, int32 modeAttr, int32 numAttr, 
					void *value, int32 lg)
{
	switch	(modeAttr)
	{
		case	BASIC_ATTR :
		case	OBJ_ATTR :
			return	XoSetAttr(obj,numAttr,value,lg) ;
		break ;
		case	LIST_BASIC_ATTR :
		case	LIST_OBJ_ATTR :
			return	XoAddInAttr(obj,numAttr,value,lg) ;
		break ;
	}
	return	-1 ;	/* erreur */
}


static	char	*xoreadtxt(FILE *fin,char *end)
{
	char	tmp[4096] ;
	char	*buf ;
	char	*newbuf ;
	int32	lgmax ;
	int32	lglue ;
	int32	lg ;

	buf	= (char *)malloc(4096) ;
	if	( !buf )
		return	NULL ;
	*buf	= '\0' ;
	lgmax	= 4096 ;
	lglue	= 0 ;

	while	( my_fgets(tmp,4096,fin) )
	{
		if	( strcmp(tmp,end) == 0 )
			break ;
		lg	= strlen(tmp) ;
		if	( lglue + lg > lgmax )
		{
			newbuf	= (char *)realloc(buf,(size_t)(lgmax+4096)) ;
			if	( !newbuf )
			{
				free(buf) ;
				return	NULL ;
			}
			buf	= newbuf ;
			lgmax	+= 4096 ;
		}
		memcpy	(buf+lglue,tmp,(size_t)lg) ;
		lglue	+= lg ;
	}
	*(buf+lglue) = '\0' ;
	return	buf ;
}

static	void	*xoreadobj(FILE *fin,int32 skeep,char *waitClass) ;

static	int32	xoreadattr(FILE *fin, void *obj, int32 modeSv, char *nameAttr,char *waitClass)
{
	char	tmp[256] ;
	char	buf[4096+10] ;
	char	*pt ;

	void	*objFils ;
	char	value[256] ;
	int32	int32Value ;
	int32	lgValue ;

	int32	numAttr	= -1 ;
	int32	typeAttr	= -1 ;
	int32	modeAttr	= -1 ;

	int32	quotted ;

	value[0]	= '\0';
	quotted	= 0 ;
	if	(obj)
	{
		numAttr	= XoNumAttrInObj(obj,nameAttr) ;
		if	( numAttr < 0 )		/* attr.n'existe plus */
		{
			obj	= NULL ;	
			modeAttr= -1 ;
			typeAttr= -1 ;
		}
		else
		{
			modeAttr=XoModeAttr(numAttr) ;
			typeAttr=XoTypeAttr(numAttr) ;
		}
	}

	switch	(modeSv)
	{
		case	CFG_INT :
		case	CFG_INT_UNSET :
		case	CFG_LSTINT :
		case	CFG_BUF :
		case	CFG_BUF_UNSET :
		case	CFG_LSTBUF :
			if	(!get_word(fin,value))	
			{
				XOERR("unexpected eof",NumLine) ;
				return -1 ;	/*erreur*/
			}
		break ;
	}

	if	(*value && strcmp(value,UNSET_VALUE) == 0)
	{
		XoUnSetAttr(obj,numAttr) ;		
		return	1 ;
	}


	switch	(modeSv)
	{
		case	CFG_INT :
		case	CFG_LSTINT :
			if	( *value != '=' )
			{

				sprintf	(tmp,"wait '=' after '%s'",nameAttr) ;
				XOERR	(tmp,NumLine) ;
				return -1 ;
			}
		break ;
		case	CFG_BUF :
		case	CFG_LSTBUF :
			if	(strcmp(value,IS_AFFECTATION) == 0)
			{
				strcpy	(value,"=") ;
				quotted	= 1 ;
			}
			if	( *value != '=' && *value != ':' )
			{
				sprintf	(tmp,"wait '=' or ':' or is after '%s'",
							nameAttr) ;
				XOERR	(tmp,NumLine) ;
				return -1 ;
			}
		break ;
	}
	switch	(modeSv)
	{
		case	CFG_INT :
		case	CFG_LSTINT :
		case	CFG_BUF :
		case	CFG_LSTBUF :
			if	( *value == '=' && *(value+1) != '\0' )
			{
				sprintf	(tmp,"wait %s = value",nameAttr) ;
				XOERR	(tmp,NumLine) ;
				return -1 ;
			}
		break ;
	}


	if	(typeAttr == XoBuffer)
	{
		int	ok ;
		unsigned char *bufHex ;
		int		lgHex ;

		ok	= 0 ;
		switch	(modeSv)
		{
		case	CFG_BUF :
		case	CFG_LSTBUF :
			if	( *value == '=' )
			{
				if	( !my_fgets(buf,4096,fin) )
					break ;
				lgValue	= strlen(buf) ;
				if	( lgValue <= 1 )
					break ;
				if	( buf[strlen(buf)-1] == '\n' )
					buf[strlen(buf)-1] = '\0' ;
				pt	= buf ;
				pt++ ;	/* pour le ' ' qui suit '=' */
				ok	= 1 ;
				break ;
			}
			if	( *value == ':' )
			{
				if	( !my_fgets(buf,4096,fin) )
					break ;
				pt	= xoreadtxt(fin,BUF_EOT) ;
#ifdef	TRACE
fprintf(stderr,"-----------------------------------") ;
fprintf(stderr,"%s\n",pt) ;
fprintf(stderr,"-----------------------------------") ;
fflush(stderr) ;
#endif
				if	( pt)
					ok = 2 ;
				break ;
			}
		}
		if	( !ok || !pt )
		{
			sprintf(tmp,"bad format for '%s'", nameAttr) ;
			XOERR	(tmp,NumLine) ;
			return	-1 ;
		}
		lgValue	= strlen(pt) ;
		bufHex	= (unsigned char *)malloc(lgValue) ;
		if	( !bufHex )
		{
			XOERR	("cannot malloc",NumLine) ;
			return	-1 ;
		}
		lgHex	= XoAsc2Hex((unsigned char *)pt,(unsigned char *)bufHex) ;
		if	( lgHex < 0 )
		{
			XOERR("asc -> hexa error format",NumLine) ;
			free	(bufHex) ;
			return	-1 ;
		}
		xosetattr(obj,modeAttr,numAttr,bufHex,lgHex) ;
		free	(bufHex) ;
		if	( ok == 2 )
			free	(pt) ;
	}
	else
	{
	switch	(modeSv)
	{
		case	CFG_INT :
		case	CFG_LSTINT :
			if	(!my_fgets(value,MAX_LG_word,fin) )
			{
				sprintf	(tmp,"wait a value '%s'",nameAttr) ;
				XOERR	(tmp,NumLine) ;
				return -1 ;	/*erreur*/
			}	
			pt	= value ;
			while	( *pt && ( *pt == ' ' || *pt == '\t' ) )
				pt++ ;
			if	(!obj)
				return	1 ;
			if	( strlen(pt) >= 1 && pt[strlen(pt)-1] == '\n' )
				pt[strlen(pt)-1] = '\0' ;
			if	(typeAttr==XoChar 
				&& NICE_CHAR(pt[0]) 
				&& !pt[1] )
			{
#ifdef	TRACE
fprintf(stderr,"load a nice char value '%s' '%c'\n",pt,*pt) ;
fflush(stderr) ;
#endif
				int32Value	= *pt ;
			}
			else
			{
#ifdef	TRACE
fprintf(stderr,"load an ascii char value '%s' %d\n",pt,xoatoi(pt)) ;
fflush(stderr) ;
#endif
				int32Value=xoatoi(pt) ;
			}
			xosetattr(obj,modeAttr,numAttr,(void *)int32Value,0) ;
		break ;

		case	CFG_BUF :
		case	CFG_LSTBUF :
			if	( *value == '=' && quotted == 0)
			{
				if	( !my_fgets(buf,4096,fin) )
					break ;
				lgValue	= strlen(buf) ;
				if	( lgValue <= 1 )
					break ;
				if	( buf[strlen(buf)-1] == '\n' )
					buf[strlen(buf)-1] = '\0' ;
				pt	= buf ;
				pt++ ;	/* pour le ' ' qui suit '=' */
				xosetattr(obj,modeAttr,numAttr,pt,0) ;
				break ;
			}
			if	( *value == '=' && quotted == 1)
			{
				char	endSep ;
				char	*pt2 ;

				if	( !my_fgets(buf,4096,fin) )
					break ;
				pt	= buf ;
				while	( *pt && ( *pt == ' ' || *pt == '\t' ) )
					pt++ ;
				if	( !*pt )
				{
					xosetattr(obj,modeAttr,numAttr,"",0) ;
					break ;
				}
				switch	(*pt)
				{
				case	'"' :	endSep	= '"' ;		break ;
				case	'\'' :	endSep	= '\'' ;	break ;
				case	'`' :	endSep	= '`' ;		break ;
				default :
					sprintf(tmp,"bad quote char for '%s'",
								nameAttr) ;
					XOERR	(tmp,NumLine) ;
					return -1 ;	/*erreur*/
				break ;
				}
				pt++ ;
				pt2	= strchr(pt,(int)endSep) ;
				if	( !pt2 )
				{
					sprintf(tmp,"wait quote char for '%s'",
								nameAttr) ;
					XOERR	(tmp,NumLine) ;
					return -1 ;	/*erreur*/
				}
				*pt2	= '\0' ;
				xosetattr(obj,modeAttr,numAttr,pt,0) ;
				break ;
			}
			if	( *value == ':' )
			{
				if	( !my_fgets(buf,4096,fin) )
					break ;
				pt	= xoreadtxt(fin,TXT_EOT) ;
				if	( pt && *pt )
				{
					if	( pt[strlen(pt)-1] == '\n' )
						pt[strlen(pt)-1] = '\0' ;
					xosetattr(obj,modeAttr,numAttr,pt,0) ;
#ifdef	TRACE
fprintf(stderr,"-----------------------------------") ;
fprintf(stderr,"%s\n",pt) ;
fprintf(stderr,"-----------------------------------") ;
fflush(stderr) ;
#endif
				}
				if	( pt)
					free(pt) ;
				break ;
			}
		break ;

		case	CFG_OBJ :
		case	CFG_LSTOBJ :
		{
			// TODO
			// ici on connait la classe de objFils, ca pourrait
			// eviter d'avoir a la rappeller dans le .asc
			objFils	= xoreadobj(fin,obj?0:1,waitClass);
			if	(!objFils)
				return	-1 ;	/* erreur */
			if	(!obj)
				return	1 ;
			xosetattr(obj,modeAttr,numAttr,objFils,0) ;
		}
		break ;
		default	:
			return -1 ;		/* erreur */
		break ;
	}
	}
	return	1 ;
}

static	void	*xoreadobj(FILE *fin, int32 skeep,char *waitClass)
{
	int32	numClass ;
	void	*obj ;
	int32	numAttr ;

	char	nameClasse[256] ;
	char	nameAttr[256] ;

	int32	modeSv ;

	if	(!get_word(fin,nameClasse))
	{
		return	NULL ;			/* erreur */
	}

	if	(strcmp(nameClasse,":=") == 0 && waitClass && *waitClass)
	{
		strcpy(nameClasse,waitClass);
#ifdef	TRACE
fprintf	(stderr,"class supposed to be : '%s'",waitClass);
fflush(stderr);
#endif
	}

	obj	= NULL ;
	numClass= XoGetNumType(nameClasse) ;	/* cette classe n'existe plus*/
#if	0
	if	( numClass < 0 )
		skeep	= 1 ;
#endif
	if	( numClass < 0 )
	{
		char	tmp[128] ;

		sprintf	(tmp,"class '%s' does not exist",nameClasse) ;
		XOERR	(tmp,NumLine) ;
		if	(obj)	XoFree(obj,1) ;
		return	NULL ;
	}


	{
		char	tmp[256] ;

		if	(!get_word(fin,tmp))
		{
			XOERR	("unexpected eof",NumLine) ;
			if	(obj)	XoFree(obj,1) ;
			return	NULL ;
		}

		if	( strcmp(tmp,DEBUT_OBJ) != 0 )
		{
			sprintf	(tmp,"wait '%s'",DEBUT_OBJ) ;
			XOERR	(tmp,NumLine) ;
			if	(obj)	XoFree(obj,1) ;
			return	NULL ;
		}
	}

	if	(!skeep)
		obj	= XoNew(numClass) ;
#ifdef	TRACE
fprintf(stderr,"new obj '%s'\n",nameClasse) ;
fflush(stderr) ;
#endif

	
	while	(get_word(fin,nameAttr))
	{
		if	( strcmp(nameAttr,FIN_OBJ) == 0 )	
			break ;		/*fin*/
#ifdef	TRACE
fprintf(stderr,"new att '%s' in object '%s'\n",nameAttr,nameClasse);
fflush(stderr) ;
#endif
		numAttr	= XoNumAttrInObj(obj,nameAttr) ;
		if	( numAttr < 0 )		/* attr.n'existe plus */
		{
			char	tmp[512] ;

			if	( AcceptUnComplete )
			{
				IsUnComplete	= 1 ;
				return	obj ;
			}
			sprintf(tmp,"attr '%s' does not exist in '%s'",
				nameAttr,nameClasse) ;
			XOERR	(tmp,NumLine) ;	
			if	(obj)	XoFree(obj,1) ;
			return	NULL ;
		}
		else
		{
			t_attribut_ref	*ar ;
			ar	= XoGetRefAttr (numAttr);
			if	(!ar)
			{
				char	tmp[256] ;

				sprintf	(tmp,"err. RefAttr '%s'", nameAttr) ;
				XOERR	(tmp,NumLine) ;
				if	(obj)	XoFree(obj,1) ;
				return	NULL ;
			}
#if	0
			modeSv	= xomodesv(ar) ;
#endif
			modeSv	= ar->ar_mode_sv ;
			waitClass	= ar->ar_type_name;
#ifdef	TRACE
fprintf(stderr,"wait att '%s' of type '%s'\n",nameAttr,waitClass);
fflush(stderr) ;
#endif
		}

		switch	(GET_MDSV_TYPE(modeSv))
		{
			case	CFG_OBJ :
			case	CFG_INT :
			case	CFG_BUF :
			case	CFG_INT_UNSET :
			case	CFG_BUF_UNSET :
				if (xoreadattr(fin,obj,modeSv,nameAttr,waitClass)<0)
				{
					char	tmp[256] ;

					sprintf	(tmp,"err. readattr '%s'",
								nameAttr) ;
					XOERR	(tmp,NumLine) ;
					if	(obj)	XoFree(obj,1) ;
					return	NULL ;
				}
			break ;
			case	CFG_LSTOBJ :
			case	CFG_LSTINT :
			case	CFG_LSTBUF :
			{
				char	marqueListe[256] ;
				int32	encore = 1 ;

				if	(!get_word(fin,marqueListe))	
				{
					char	tmp[256] ;

					sprintf	(tmp,"unexpected eof '%s'",
								nameAttr) ;
					XOERR	(tmp,NumLine) ;
					if	(obj)	XoFree(obj,1) ;
					return	NULL ;
				}
				if	(strcmp(marqueListe,DEBUT_LISTE))
				{
					char	tmp[256] ;

					sprintf	(tmp,"wait '[' after '%s'",
								nameAttr) ;
					XOERR	(tmp,NumLine) ;
					if	(obj)	XoFree(obj,1) ;
					return	NULL ;
				}
#ifdef	TRACE
fprintf(stderr,"debut de liste '%s'\n",nameAttr) ;
fflush(stderr) ;
#endif
				while	(encore)
				{
					if	(!get_word(fin,marqueListe))	
						break ;	/*erreur*/
					if	(!strcmp(marqueListe,FIN_LISTE))
					{
#ifdef	TRACE
fprintf(stderr,"fin de liste\n") ;
fflush(stderr) ;
#endif
						break ;
					}
					unget_word(marqueListe) ;
					if(xoreadattr(fin,obj,modeSv,nameAttr,waitClass)<0)
					{
						if	(obj)	XoFree(obj,1) ;
						return	NULL ;
					}
				}	
				
			}
			break ;
			default :
			break ;
		}
	}

	if	(!obj)
		return (char *)1 ;
	return	obj ;
}

static	void	xosynchro(FILE *fin, char *tag)
{
	char	tmp[4096] ;

	if	( !tag || !strlen(tag) )
		return ;

	while	( my_fgets(tmp,4096,fin) )
	{
		if	( strcmp(tmp,tag) == 0 )
		{
#ifdef	TRACE
fprintf	(stderr,"find tag '%s' at %d\n",tag,NumLine) ;
fflush	(stderr) ;
#endif
			return ;
		}
	}
#ifdef	TRACE
fprintf	(stderr,"tag '%s' not found\n",tag) ;
fflush	(stderr) ;
#endif
}


/*DEF*/	void	*__XoLoadAscii(FILE *fin)
{
	void	*retour;
	NumLine	= 0 ;
	xosynchro(fin,StartFlux) ;
	retour	= xoreadobj(fin,0,"") ;
	xosynchro(fin,EndFlux) ;
	if	(retour == (void *)1)
	{
		XOERR("root class undefinied",NumLine);
		return	NULL ;
	}	
	return	retour ;
}

/*DEF*/	void	*XoLoadAscii(FILE *fin)
{
	void	*ret;

	ret = __XoLoadAscii(fin);
	return	ret;
}

/*DEF*/	void	*XoLoadAsciiUnComplete(FILE *fin,int32 *uncomplete)
{
	void	*retour;

	*uncomplete		= 0 ;
	AcceptUnComplete	= 1 ;
	IsUnComplete		= 0 ;
	retour	= XoLoadAscii((void *)fin) ;
	if	( IsUnComplete )
		*uncomplete	= 1 ;
	AcceptUnComplete	= 0 ;
	IsUnComplete		= 0 ;
	return	retour ;
}

/*DEF*/	void	XoLoadAsciiSetGetc(int (*fct)(void *par))
{
	CustGetc	= fct ;
}

/*DEF*/	void	XoLoadAsciiSetUnGetc(int (*fct)(int c,void *par))
{
	CustUnGetc	= fct ;
}


int	xogetc(void *buf)
{
	if	( MemPos >= MemLg - 1 || MemBuf[MemPos] == '\0' )
		return	EOF;

	return	(int)MemBuf[MemPos++];
}

int	xoungetc(int c,void *p)
{
	if	( MemPos >= 1 )
		MemPos--;

	return	MemPos;
}

/*DEF*/	void	*__XoLoadAsciiMem(char *buf,int32 lg)
{
	void	*ret;

	CustGetc	= xogetc;
	CustUnGetc	= xoungetc;


	MemBuf		= buf;
	MemLg		= lg;
	MemPos		= 0;

	ret = XoLoadAscii((FILE *)buf);

	CustGetc	= NULL;
	CustUnGetc	= NULL;

	return	ret;
}

/*DEF*/	void	*XoLoadAsciiMem(char *buf,int32 lg)
{
	void	*ret;

	ret = __XoLoadAsciiMem(buf,lg);
	return	ret;
}
