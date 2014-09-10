
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

#include "xmlstr.h"


static	char	*xo_file = __FILE__;


static	t_nsctxt	GlobalNsCtxt;

char	*XoDropPrefixe(char *qname)
{
	char	*nameattr;

	nameattr= strchr((char *)qname,':');
	if	(nameattr)
		nameattr= nameattr+1;
	else
		nameattr= (char *)qname;

	return	nameattr;
}

int	_XoFindCfgNameSpace(char *uri)
{
	int	i;

	if	(!uri)
		return	-1;

	for	(i = 0 ; i < GlobalNsCtxt.nbns && i < XOML_MAX_NAMESPACE ; i++)
	{
		if	(strcasecmp(uri,GlobalNsCtxt.ns[i].ns_uri) == 0)
			return	i;
	}

	return	-2;
}

char	*_XoFindUriNameSpace(char *pref)
{
	int	i;

	if	(!pref)
		return	NULL;

	for	(i = 0 ; i < GlobalNsCtxt.nbns && i < XOML_MAX_NAMESPACE ; i++)
	{
		if	(strcmp(pref,GlobalNsCtxt.ns[i].ns_userprf) == 0)
			return	GlobalNsCtxt.ns[i].ns_uri;
	}

	return	NULL;
}

char	*_XoFindPrfNameSpace(char *uri)
{
	int	i;

	if	(!uri)
		return	NULL;

	for	(i = 0 ; i < GlobalNsCtxt.nbns && i < XOML_MAX_NAMESPACE ; i++)
	{
		if	(strcasecmp(uri,GlobalNsCtxt.ns[i].ns_uri) == 0)
			return	GlobalNsCtxt.ns[i].ns_userprf;
	}

	return	NULL;
}

static	char	*FindPivotNameSpace(t_nsctxt *nsctxt,char *pref)
{
	int	i;
	t_ns	*ns;

	if	(nsctxt->ns == NULL)
	{
		_XoAllocNameSpace(nsctxt,XOML_MAX_NAMESPACE);
	}

//fprintf(stderr,"nbns=%d\n",nsctxt->nbns);
	for	(i = 0 ; i < nsctxt->nbns ; i++ )
	{
		ns	= &(nsctxt->ns[i]);
//fprintf(stderr,"ns->ns_userprf=%s pref='%s'\n",ns->ns_userprf,pref);
		if	(ns->ns_userprf && strcmp(ns->ns_userprf,pref) == 0)
			return	ns->ns_pivotprf;
	}
	return	NULL;
}

/*DEF*/void	XoDumpLocalNameSpace(FILE *fout,t_nsctxt *nsctxt)
{
	int	i;
	char	d = ' ';

	for	(i = 0 ; i < nsctxt->nbns && i < XOML_MAX_NAMESPACE ; i++)
	{
		fprintf	(fout,"%c%-10.10s %-10.10s  %s\n",d,
			nsctxt->ns[i].ns_userprf,
			nsctxt->ns[i].ns_pivotprf,
			nsctxt->ns[i].ns_uri);
		fflush(fout);
	}
}

char	*_XoDoName(t_nsctxt *nsctxt,char *aqname,char *avalue,char *dst)
{
	char	*sep;
	char	*piv;

	*dst	= '\0';

	if (GlobalNsCtxt.nbns <= 0)
	{
		return	aqname;
	}

	if (avalue && *avalue && strcmp(aqname,"xmlns") == 0)
	{
		return	aqname;
	}
	if (avalue && *avalue && strncmp(aqname,"xmlns:",6) == 0)
	{
		int	nscfg;

//fprintf(stderr,"xmlns aqname='%s' with uri='%s'\n",aqname,avalue);
		nscfg	= _XoFindCfgNameSpace(avalue);
		if (nscfg < 0)
			return	aqname;
		sprintf(dst,"%s:%s","xmlns",GlobalNsCtxt.ns[nscfg].ns_userprf);
//fprintf(stderr,"xmlns aqname='%s' -> xoname='%s'\n",aqname,dst);
		return	dst;
	}

	sep	= strchr(aqname,':');
	if	(sep)
	{
//fprintf(stderr,"aqname='%s'\n",aqname);
		*sep	= '\0';
		piv	= FindPivotNameSpace(nsctxt,aqname);
		*sep	= ':';
		if	(!piv)
		{
			XOERR(aqname,0);
			XoDumpLocalNameSpace(stderr,nsctxt);
			return	aqname;
		}
		sprintf	(dst,"%s:%s",piv,sep+1);
		return	dst;
	}
	// no prefixe ...
	if	(nsctxt->nsdefault)
	{	// use default namespace of parsing context
		sprintf	(dst,"%s:%s",nsctxt->nsdefault,aqname);
		return	dst;
	}
#if 0
	piv	= GlobalNsCtxt.nsdefault;
	if	(GlobalNsCtxt.ns && GlobalNsCtxt.nbns && piv && *piv)
	{	// use default namespace of global context
		sprintf	(dst,"%s:%s",piv,aqname);
		return	dst;
	}
#endif

	return	aqname;
}


// cette fonction est appelle a l'init pour indiquer les namespaces qu'on va
// utiliser. elle est aussi appelee a chaque parsing pour stocker les nspaces
// courants et les comparer aux namespaces declares
int	_XoAddNameSpace(void *reader,int attrCount,
	t_nsctxt *nsctxt,char *pname,char *uri)
{
	t_ns	*ns;
	char	*name = pname;
	int	cfgns = 0;
	int	def = 0;
	int	global = 0;

	if	(nsctxt == NULL)	// declaration des namespaces utilises
	{
		nsctxt	= &GlobalNsCtxt;
		if	(nsctxt->ns == NULL)
			_XoAllocNameSpace(nsctxt,XOML_MAX_NAMESPACE);
		global	= 1;
	}

	if	(global == 0)	// parsing des namespaces
	{
		if	(strncmp(pname,"xmlns",5))
			return 0;
		name	= XoDropPrefixe(pname);
		if	(strlen(name) == 0)
			return 0;
		if	(strcmp(name,pname) == 0)
			def	= 1;
	}
	else		// declaration des namespaces
	{
		if	(*name == '*')
		{
			name++;
			def	= 1;
		}
	}

	if	(nsctxt->nbns >= XOML_MAX_NAMESPACE)
	{
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("MAX NAMESPACE\n");
		XOERR("max namespace",global);
		return 0;
	}

	{
	int	i;
	for	(i = 0 ; i < nsctxt->nbns ; i++ )
	{
		ns	= &(nsctxt->ns[i]);
		if	( ns->ns_userprf && *(ns->ns_userprf) && strcmp(ns->ns_userprf,name) == 0 )
		{
			if	( strcmp(ns->ns_uri,uri) != 0 )
			{
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("!!! error '%s' '%s' different namespace redefinition '%s' '%s'\n",
				name,uri,ns->ns_pivotprf,ns->ns_uri);
				return 0;
			}
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("!!! drop '%s' '%s' equal namespace redefinition '%s' '%s'\n",
				name,uri,ns->ns_pivotprf,ns->ns_uri);
			return	nsctxt->nbns;
		}
	}
	}

	if	(global == 0)
	{
		cfgns	= _XoFindCfgNameSpace(uri);
		if	(cfgns < 0)
		{
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("namespace '%s' '%s' not found\n",name,uri);
			return 0;
		}
/*
		ns->ns_pivotprf	= CfgNs[cfgns][0];
		ns->ns_uri	= CfgNs[cfgns][1];
*/
		ns	= &(nsctxt->ns[nsctxt->nbns]);
		if	(global == 0 && def == 1)
		{
			ns->ns_pivotprf	= GlobalNsCtxt.ns[cfgns].ns_userprf;
			ns->ns_uri	= GlobalNsCtxt.ns[cfgns].ns_uri;
			ns->ns_userprf	= strdup(ns->ns_pivotprf);
		}
		else
		{
			ns->ns_pivotprf	= GlobalNsCtxt.ns[cfgns].ns_userprf;
			ns->ns_uri	= GlobalNsCtxt.ns[cfgns].ns_uri;
			ns->ns_userprf	= strdup(name);
		}
	}
	else
	{
		ns	= &(nsctxt->ns[nsctxt->nbns]);
		if	(ns->ns_userprf)
			free(ns->ns_userprf);
		ns->ns_userprf	= strdup(name);
		ns->ns_pivotprf	= NULL;
		if	(ns->ns_uri)
			free(ns->ns_uri);
		ns->ns_uri	= strdup(uri);
	}
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("add namespace glb=%d def=%d prf='%s' val='%s' => pivot='%s'\n",
		global,def,ns->ns_userprf,ns->ns_uri,ns->ns_pivotprf);
	if	(def && !global)
	{
		nsctxt->nsdefault	= ns->ns_pivotprf;
		nsctxt->nsnumdef	= cfgns;
	}
	if	(def && global)
	{
		nsctxt->nsdefault	= ns->ns_userprf;
		nsctxt->nsnumdef	= cfgns;
	}
	return	++nsctxt->nbns;
}

/*DEF*/int32	XoAddNameSpace(char *pname,char *uri)
{
	return	_XoAddNameSpace(NULL,0,NULL,pname,uri);
}

void	*_XoAllocNameSpace(t_nsctxt *nsctxt,int nb)
{
	void	*pt;
	int	sz;

	if	(nb <= 0 || nb > XOML_MAX_NAMESPACE)
		nb	= XOML_MAX_NAMESPACE;

	sz	= sizeof(t_ns) * nb;
	pt	= malloc(sz);
	if	(!pt)
		return	NULL;
	memset	(pt,'\0',sz);
	nsctxt->ns	= (t_ns *)pt;

	if	(nsctxt == &GlobalNsCtxt)
	{
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("allocate global namespace table nbelem=%d sz=%d \n",nb,sz);
	}
	else
	{
if (C_PARSE_TRACE(nsctxt))
XOML_TRACE("allocate parsing namespace table nbelem=%d sz=%d \n",nb,sz);
	}

	return	pt;
}

void	_XoFreeNameSpace(t_nsctxt *nsctxt)
{
	int	i;
	t_ns	*ns;

	if	(!nsctxt->ns)
		return;

	for	(i = 0 ; i < nsctxt->nbns ; i++ )
	{
		ns	= &(nsctxt->ns[i]);
		if	(ns->ns_userprf)
		{
			free(ns->ns_userprf);
			ns->ns_userprf	= NULL;
		}
	}
	nsctxt->nbns	= 0;

	free(nsctxt->ns);
	nsctxt->ns	= NULL;
}


/*DEF*/int32	XoLoadNameSpace(char *file)
{
	FILE	*f;
	char	buf[1024];
	char	prf[1024];
	char	uri[1024];
	int32	nb = 0;
	char	*pt;

	f	= fopen(file,"r");
	if	(!f)
	{
		return	-1;
	}

	while	(fgets(buf,sizeof(buf)-1,f))
	{
		if	(!strlen(buf))	continue;
		if	(buf[0] == '#' || buf[0] == ';')	continue;
		pt	= strchr(buf,'\n');
		if	(pt) *pt = '\0';
		pt	= strchr(buf,'\r');
		if	(pt) *pt = '\0';
		if	(sscanf(buf,"%s%s",prf,uri) == 2)
		{
			XoAddNameSpace(prf,uri);
			nb++;
		}
	}

	fclose(f);
	return	nb;
}

/*DEF*/	int32	XoLoadNameSpaceDir(char *path)
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
		if	( point && *point && strcmp(point,XO_XNS_EXT) == 0 )
		{
			if ((n=XoLoadNameSpace(name)) < 0)
			{
				XOERR("XoLoadNameSpaceDir() failed",0);
			}
			nb	= nb + n;
		}
	}

	xo_closeDir(dir);
	return	nb;
}


/*DEF*/void	XoDumpNameSpace(FILE *fout)
{
	int	i;
	char	d;
	char	*def;

	def	= GlobalNsCtxt.nsdefault;
	for	(i = 0 ; i < GlobalNsCtxt.nbns && i < XOML_MAX_NAMESPACE ; i++)
	{
		d	= ' ';
		if(def && *def && !strcmp(GlobalNsCtxt.ns[i].ns_userprf,def))
			d	= '*';

		fprintf	(fout,"%c%-10.10s %s\n",d,
			GlobalNsCtxt.ns[i].ns_userprf,
			GlobalNsCtxt.ns[i].ns_uri);
		fflush(fout);
	}
}


/*DEF*/int32	XoSetNameSpace(void *obj,char *apref)
{
	int	nb	= 0;
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;
	char	*nameattr;
	int32	numattr ;
	int32	mode ;
	t_attribut_ref	*ar ;


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

	// embeded attr
	for	( a = 0 ; a < nbattr ; a++ )
	{
		char	*value;
		char	*pref;

		numattr	= type + a + 1 ;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		if	(mode != BASIC_ATTR || !IS_EMBATTR(ar))
			continue;
		nameattr	= ar->ar_name;
		if	(strncmp(nameattr,"xmlns:",6))
			continue;
		pref	= XoDropPrefixe(nameattr);
		if	(apref && *apref)
		{	// un seul xmlns: a valoriser
			if	(strcmp(apref,pref) != 0)
				continue;
		}
		value	= _XoFindUriNameSpace(pref);
		if	(!value || !*value)
			continue;
//fprintf(stderr,"attribut '%s' => '%s' '%s'\n",nameattr,pref,value);
		XoSetAttr(obj,numattr,value,0);
		nb++;
	}

	return	nb;
}

// TODO pas clair xmlns & xmlns: 
// pourqoiu j'ai mis les xmlns dans custom$ en obix ?
/*DEF*/int32	XoUnSetNameSpace(void *obj,char *apref)
{
	int	nb	= 0;
	int32	type ;
	int32	nbattr ;
	char	*nameobj ;
	int32	a ;
	char	*nameattr;
	int32	numattr ;
	int32	mode ;
	t_attribut_ref	*ar ;


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

	// TODO en obix si les xmlns etaient des embattr ca serait plus simple 
	if	(XoIsObix(obj) > 0 && XoGetRefObjCustomAttributs(type) > 0)
	{
		int32	posn;
		int32	lg;
		char	*name;

		numattr = XoNumAttrInObj(obj,XOML_CUSTOM_ATTR);
		if (numattr < 0)
		{
			XOERR("Custom Attribute",0) ;
			return	-70 ;
		}

		posn	= XoFirstAttr(obj,numattr) ;
		while	( posn > 0 )
		{
			void	*sub;

			sub	= XoGetAttr	(obj,numattr,&lg);
			if (!sub)
				break;
			name	= XoNmGetAttr	(sub,XOML_CUSTOM_ATTR_NAME,0) ;
			if	(strncmp(name,"xmlns",5) != 0)
				goto	dropattr;
			XoFree(sub,1);
			XoSupInAttr(obj,numattr);
dropattr :
			posn	= XoNextAttr(obj,numattr) ;
		}
		return	nb;
	}

	// embeded attr
	for	( a = 0 ; a < nbattr ; a++ )
	{
		char	*pref;

		numattr	= type + a + 1 ;
		mode	= XoModeAttr(numattr) ;
		ar	= XoGetRefAttr(numattr) ;
		if	(mode != BASIC_ATTR || !IS_EMBATTR(ar))
			continue;
		nameattr	= ar->ar_name;
		if	(strncmp(nameattr,"xmlns:",6))
			continue;
		pref	= XoDropPrefixe(nameattr);
		if	(apref && *apref)
		{	// un seul xmlns: a devaloriser
			if	(strcmp(apref,pref) != 0)
				continue;
		}
//fprintf(stderr,"attribut '%s' => '%s' '%s'\n",nameattr,pref,value);
		XoUnSetAttr(obj,numattr);
		nb++;
	}

	return	nb;
}
