
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
#include <stdlib.h>
#include <string.h>

#include <libxml/xmlreader.h>

#include "xoapipr.h"

#define		GRAIN_DICO	1024

typedef	struct
{
	char	*ed_val;
	int32	ed_off;
}	t_entry_dico;

static	char	*xo_file = __FILE__;

char	*XoDico;	// heap pointer on data dictionary
int32	XoDicoCount;	// number of entries
int32	XoOffDico;	// next offset (ie the useful lenght)
int32	XoAllocDico;	// allocated lenght


static	t_entry_dico	*TabDico;	// sorted entries on XoDico


static	int32	AddInSharedDico(char *s)
{
	int32	lgs ;
	int32	offret ;


	lgs	= strlen(s) + 1 ;
	if	( !XoDico )
	{
		XoDico	= (char *)malloc(GRAIN_DICO) ;
		if	( !XoDico )
		{
			XOERR("alloc dico",GRAIN_DICO) ;
			return	-1 ;
		}
		XoAllocDico	= GRAIN_DICO ;
	}

#if	0	// TODO interdire les doublons
	offret	= FindInDico(ctxt,s) ;
	if	( offret != -1 )
	{	/*	chaine deja presente	*/
		return	offret ;
	}
#endif

	while	( XoOffDico + lgs >= XoAllocDico )
	{
		char	*pt ;

		pt = (char *)realloc(XoDico,(size_t)(XoAllocDico+GRAIN_DICO)) ;
		if	( !pt )
		{
			XOERR("realloc dico",XoAllocDico+GRAIN_DICO) ;
			return	-1 ;
		}
		XoDico	= pt ;
		XoAllocDico	+= GRAIN_DICO ;
	}

	memcpy		(XoDico+XoOffDico,s,(size_t)lgs) ;
	offret		= XoOffDico ;
	XoOffDico	+= lgs ;

//printf("ajout de [%s] off = %ld\n",s,(long)offret) ;

	return		offret ;
}


static	int	entrycmp(const void *o1, const void *o2)
{
	t_entry_dico	*e1	= (t_entry_dico *)o1;
	t_entry_dico	*e2	= (t_entry_dico *)o2;

	return		strcmp(e1->ed_val,e2->ed_val);
}

/*
 *	recherche dicho dans les entrees du dico
 */
static	t_entry_dico	*mybsearchdico(char *name, int32 lg,int32 *where)
{
	int32	l , h , m ;
	int32	ret ;
	t_entry_dico	*os ;

	*where	= -1;
	l	= 0 ;
	h	= XoDicoCount - 1 ;
	if	(h <= 0)
		return	NULL;
	m	= ( l + h ) / 2 ;
	while	( l <= h )
	{
//		m	= ( l + h ) / 2 ;
		os	= &TabDico[m] ;
		ret	= memcmp(name,os->ed_val,(size_t)(lg+1)) ;
		*where	= m;

//printf("[%s] =?= [%s]\n",name,os->ed_val) ;

		if	( ret < 0 )
		{
			h	= m - 1 ;
		}
		else
		{
			if	( ret > 0 )
			{
				l	= m + 1 ;
			}
			else
			{
				return	os ;
			}
		}
		m	= ( l + h ) / 2 ;
		*where	= m;
	}
	if	(*where >= 0)
	{
		os	= &TabDico[*where];
		if	(memcmp(os->ed_val,name,strlen(os->ed_val)) == 0)
		{
			return	((t_entry_dico *)NULL) ;
		}
		*where	= -1;
	}
	return	((t_entry_dico *)NULL) ;
}

/*DEF*/	int32	__XoFindInSharedDico(char *s,int32 *offpref,int32 warn)
{
	char	tmp[256];
	t_entry_dico	*os ;
	int32	sameprefixe;
	char	*after;

	*offpref	= -1;
	if	(!s || !*s)
	{
		XOERR("find in shared dico error",0);
		return	-1;
	}

	if	(!XoDico || XoAllocDico <= 0 || XoDicoCount <= 0)
	{
		XOERR("find in shared dico error",0);
		return	-2;
	}

	os	= mybsearchdico(s,strlen(s),&sameprefixe);
	if	(os)
	{
		// TODO a virer juste pour verification en phase de dev
		if	(strcmp(s,XoDico+os->ed_off))
		{
			XOERR("bug in shared dico",0);
			return	0;
		}
		return	os->ed_off;
	}

	// we assume that the shared dictionary is mandatory complet
	// if we do not find a string this is an error

	if	(warn)
	{
		sprintf(tmp,"missing '%s' in shared dico",s);
		XOERR(tmp,0);
	}

	if	(sameprefixe >= 0)
	{
		os	= &TabDico[sameprefixe];
		after	= os->ed_val;
		*offpref= os->ed_off;
//printf("xxxx common prefixe '%s'\n",after);
	}

	return	0;
}

/*DEF*/	int32	XoFindInSharedDico(char *s)
{
	int32	offpref = 0;
	return	__XoFindInSharedDico(s,&offpref,1);
}

/*DEF*/	int32	XoFindInSharedDicoPrefixed(char *s,int32 *offpref)
{
	return	__XoFindInSharedDico(s,offpref,0);
}

/*DEF*/void	XoSharedDicoFree()
{
	if	(TabDico)
	{
		free(TabDico);
		TabDico	= NULL;
	}
	if	(XoDico)
	{
		free(XoDico);
		XoDico	= NULL;
	}
	XoAllocDico	= 0;
	XoDicoCount	= 0;
}

/*DEF*/int32	XoLoadSharedDico(char *file)
{
	FILE	*f;
	char	buf[1024];
	char	*pt;
	int	nb	= 0;
	int	i;

	f	= fopen(file,"r");
	if	(!f)
	{
		XOERR("cannot load shared dico",0);
		return	-1;
	}

	if	(XoDico || XoAllocDico || XoDicoCount)
	{
		XOERR("shared dico already loaded",0);
		return	-2;
	}

	if	( AddInSharedDico("******shared*********") < 0)
	{
		XOERR("cannot add entry0 in shared dico",0);
		return	-3;
	}

	while	(fgets(buf,sizeof(buf)-1,f))
	{
		if	(!strlen(buf))	continue;
		if	(buf[0] == '#' || buf[0] == ';')	continue;
		pt	= strchr(buf,'\n');
		if	(pt) *pt = '\0';
		pt	= strchr(buf,'\r');
		if	(pt) *pt = '\0';
		if	(!strlen(buf))	continue;
//		printf	("%s\n",buf);
		if	( AddInSharedDico(buf) < 0)
		{
			XOERR("cannot add entry in shared dico",0);
			return	-4;
		}
		nb++;
	}
	fclose(f);

	XoDicoCount	= nb;

	if	(nb <= 0)
		return	0;

	TabDico	= (t_entry_dico *)malloc(nb * sizeof(t_entry_dico));
	if	(!TabDico)
	{
		XOERR("cannot alloc tab entries for shared dico",0);
		return	-5;
	}

	pt	= XoDico + strlen(XoDico)+1;	// ******shared*******
	for	(i = 0 ; i < nb ; i++)
	{
		TabDico[i].ed_val	= pt;
		TabDico[i].ed_off	= pt - XoDico;
		pt			= pt + strlen(pt)+1;
	}

	qsort(TabDico,(size_t)nb,sizeof(t_entry_dico),entrycmp) ;

#if	0
	for	(i = 0 ; i < nb ; i++)
	{
		printf("%03d '%s' %d\n",i,TabDico[i].ed_val,TabDico[i].ed_off);
	}


printf	("shared dico %p\n",XoDico);
printf	("shared dico lg=%d(%d)\n",XoOffDico,XoAllocDico);
printf	("shared dico count=%d\n",XoDicoCount);
#endif

	return	nb;
}

/*DEF*/int32	XoLoadSharedDicoDir(char *path)
{
	void	*dir;
	char	*name;
	char	*point;
	int	nb = 0;
	int	n;

	dir	= xo_openDir(path);
	if	(!dir)
		return	-1;

	while	( (name = xo_readAbsDir(dir)) && *name )
	{
		if	(*name == '.' && *(name+1) == '.')	continue;
		point	= xo_suffixname(name);
		if	( point && *point && strcmp(point,XO_DIC_EXT) == 0 )
		{
			if ((n=XoLoadSharedDico(name)) < 0)
			{
				XOERR("XoLoadSharedDico() failed",n);
				nb	= -2;
				break;
			}
			else
			{
				nb	= nb + n;
				break;
			}
		}
	}

	xo_closeDir(dir);
	return	nb;
}
