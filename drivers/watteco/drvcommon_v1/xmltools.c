
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

/*! @file xmltools.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"

#include "cmndefine.h"

extern	int	Operating;

char	*GetEnvVarName();
char	*GetAdaptorName();
char	*GetVar(char *name);
char	*SetVar(char *name,char *value);


static	char	*LoadFileMem(char *filename)
{
	int	fd;
	char	*mem;
	struct	stat	st;
	

	fd	= open(filename,0);
	if	(fd < 0)
		return	NULL;
	if	(fstat(fd,&st) < 0)
	{
		close(fd);
		return	NULL;
	}
	mem	= (char *)malloc(st.st_size+10);
	if	(!mem)
	{
		close(fd);
		return	NULL;
	}
	if	(read(fd,mem,st.st_size) != st.st_size)
	{
		free(mem);
		close(fd);
		return	NULL;
	}
	mem[st.st_size]	= '\0';
	close(fd);
	return	mem;
}



/*!
 *
 * @brief verify if a xml/dia templates exists for a given request (name)
 * file is searched in $ROOTACT/ GetAdaptorName() /diatemplates /<name>
 * @param name the file name to search
 * @return 
 * 	- 0 file does not exist
 * 	- 1 file exists
 * 
 */
int	WXoIsTemplate(char *name)
{
	char	path[1024];
	char	*awd;

	awd	= getenv("ROOTACT");
	if	(!Operating)
	{
	sprintf	(path,"%s/diatemplates/%s",getenv(GetEnvVarName()),name);
	if	(access(path,R_OK) == 0)
		return	1;
	sprintf	(path,"%s/%s/diatemplates/%s",awd,GetAdaptorName(),name);
	if	(access(path,R_OK) == 0)
		return	1;
	return	0;
	}

	sprintf(path,"%s/usr/etc/%s/diatemplates/%s",awd,GetAdaptorName(),name);
	if	(access(path,R_OK) == 0)
		return	1;
	sprintf(path,"%s/etc/%s/diatemplates/%s",awd,GetAdaptorName(),name);
	if	(access(path,R_OK) == 0)
		return	1;
	return	0;
}

/*!
 *
 * @brief allocate a xml/dia templates for a given request (name)
 * file is searched in $ROOTACT/ GetAdaptorName() /diatemplates /<name>
 * 	- lg = load the file in memory
 * 	- mm = allocate 2 * lg
 * 	- substitute variables in mm
 * 	- free memory file
 * 	- xo = parse xml with XoReadXml() from mm
 * 	- free mm
 * 	- return xo
 * @param name the file name to search
 * @param parseflags the flags to pass to XoReadXml()
 * @return 
 * 	- NULL on errors
 * 	- a xo/xml object
 * 
 */
void	*WXoNewTemplate(char *name,int parseflags)
{
	char	path[1024];
	char	*awd;
	char	*preload;
	char	*load;
	int	lg;
	int	err;
	void	*xo;

	awd	= getenv("ROOTACT");
	if	(!Operating)
	{
	sprintf	(path,"%s/diatemplates/%s",getenv(GetEnvVarName()),name);
	if	(access(path,R_OK) != 0)
	sprintf	(path,"%s/%s/diatemplates/%s",awd,GetAdaptorName(),name);
	}
	else
	{
	sprintf(path,"%s/usr/etc/%s/diatemplates/%s",awd,GetAdaptorName(),name);
	if	(access(path,R_OK) != 0)
	sprintf	(path,"%s/etc/%s/diatemplates/%s",awd,GetAdaptorName(),name);
	}
	preload	= LoadFileMem(path);
	if	(!preload)
	{
		RTL_TRDBG(0,"ERROR cannot load template '%s'\n",path);
		return	NULL;
	}
	lg	= strlen(preload);
	if	(lg <= 0)
	{
		free(preload);
		RTL_TRDBG(0,"ERROR cannot load template '%s' lg=0\n",path);
		return	NULL;
	}
	load	= (char *)malloc(2*lg);
	if	(!load)
	{
		free(preload);
		RTL_TRDBG(0,"ERROR cannot alloc (%d) for template '%s'\n",
								2*lg,path);
		return	NULL;
	}
	SetVar("w_template",name);
	if	(!rtl_evalPath(preload,load,2*lg,GetVar))
	{
		free(preload);
		free(load);
		return	NULL;
	}
	free(preload);
//fprintf(stdout,"%s\n",load);
//fflush(stdout);

	xo	= XoReadXmlMem(load,strlen(load),NULL,parseflags,&err);
	if	(!xo)
	{
//fprintf(stdout,"%s\n",load);
//fflush(stdout);
		RTL_TRDBG(0,"ERROR cannot parse template '%s' err=%d\n",
							path,err);
		free(load);
		return	NULL;
	}

	free(load);
	return	xo;
}

static	void	WStatXoXml(void *pxo,int xmlsz,char *xmltxt)
{

	void	*xo;
	char	*type		= "error";
	int	binxosz		= -1;
	int	cprxosz		= -1;
	int	ucpxosz		= -1;
	int	dicosz		= -1;
	int	allok		= 0;
	int	lg;
	unsigned	char	out[10*1024];
	unsigned	char	compress[10*1024];
	unsigned	char	uncompress[10*1024];

#if	0
	static	int	count;
	char	tmp[256];
	FILE	*f;

	if	(XoIsObix(pxo)>0)
	sprintf(tmp,"/home/pierrecaillot/Travail/_dicoxo/data/%d.obx",count);
	else
	sprintf(tmp,"/home/pierrecaillot/Travail/_dicoxo/data/%d.xml",count);
	count++;
	f	= fopen(tmp,"w");
	if	(f)
	{
		fprintf(f,"%s",xmltxt);
		fclose(f);
	}
#endif

	xo	= XoNewCopy(pxo,1);
	if	(!xo)
	{
		goto	dotrace;
	}
	type	= XoNmType(xo);

	XoUnSetNameSpace(xo,NULL);

	binxosz	= XoLgSave(xo);
	if	(binxosz > sizeof(out))
	{
		goto	dotrace;
	}

	lg	= XoSaveMem(xo,(char *)out);
	if	(lg != binxosz)
	{
		goto	dotrace;
	}

	dicosz	= XoLgDict((char *)out,lg);
	binxosz	= binxosz - dicosz;
	cprxosz	= XoMemCompress(out,binxosz,compress);
	lg	= XoMemUnCompress(compress,cprxosz,uncompress);
	if	(lg == binxosz)
	{
		ucpxosz	= lg;
		if	(memcmp(out,uncompress,binxosz) == 0)
		{
			allok	= 1;
		}
	}
dotrace :
fprintf(stdout,"###xo %d '%s' xmlsz=%d dicosz=%d binxosz=%d cprxosz=%.1f ucpxosz=%d\n",
	allok,type,xmlsz,dicosz,binxosz,(float)cprxosz/8.0,ucpxosz);
fflush(stdout);

	if	(xo)
		XoFree(xo,1);
}

/*!
 *
 * @brief serialize a xo/xml/dia object and free the original xo
 * The returned pointer must be freed.
 * @param xo xo/xml/dia object
 * @param type type of serialization (XO_FMT_STD_XML)
 * @param flags the flags to pass to XoWritXml()
 * @param sz the lenght of the returned buffer
 * @param cctype the returned content type (application/xml)
 * @return 
 * 	- NULL on errors
 * 	- a allocated buffer (NULL terminated) containing the xml document
 * 
 */
char	*WXoSerializeFree(void *xo,int type,int flags,int *sz,char **ctype)
{
	char	*ret	= NULL;
	void	*w;

	*sz	= 0;
	*ctype	= "";

	switch	(type)
	{
	case	XO_FMT_STD_BIN:		// TODO
			RTL_TRDBG(0,"ERROR cannot serialize (XOBIN) xo '%s'\n",
								XoNmType(xo));
		XoFree(xo,1);
		return	NULL;
	break;
	case	XO_FMT_STD_XML:
	default :
		*ctype	= "application/xml";
		w	= XoWritXmlMem(xo,flags,&ret,"");
		if	(!w)
		{
			RTL_TRDBG(0,"ERROR cannot serialize (XML) xo '%s'\n",
								XoNmType(xo));
			XoFree(xo,1);
			return	NULL;
		}
		*sz	= strlen(ret);
		XoWritXmlFreeCtxt(w);
	break;
	}

#undef	STAT_XOXML_SIZE
#ifdef	STAT_XOXML_SIZE
	WStatXoXml(xo,*sz,ret);
#endif


RTL_TRDBG(2,"xoserialize '%s' '%s' sz=%d\n",*ctype,XoNmType(xo),*sz);
//fprintf(stdout,"%s\n",ret);
//fflush(stdout);
	XoFree(xo,1);
	return	ret;
}


void	WXoSaveXml(void *templ,FILE *fout)
{
	FILE	*f;
	int	ret;
	char	*tmpf;
	char	tmp[2048];

	tmpf	= tempnam("/tmp","xoxml");
	if	(!tmpf)
	{
		RTL_TRDBG(0,"cannot create temp file in /tmp ...\n");
		return;
	}
	if	((ret=XoWritXmlEx(tmpf,templ,0,"")) < 0)
	{
		unlink(tmpf);
		RTL_TRDBG(0,"XoWritXmlEx() error=%d\n",ret);
		return;
	}
	f	= fopen(tmpf,"r");
	if	(!f)
	{
		unlink(tmpf);
		RTL_TRDBG(0,"fopen(%s) error\n",tmpf);
		return;
	}
	while	(fgets(tmp,sizeof(tmp),f))
	{
		fprintf(fout,"%s",tmp);
		fflush(fout);
	}
	fclose(f);
	unlink(tmpf);
}

