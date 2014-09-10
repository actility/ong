
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
#include <unistd.h>

#include <libxml/xmlreader.h>

#include "xoapipr.h"
#include "xmlstr.h"

#ifdef	MACOSX
t_objet_ref	*XoTabObjUser ;
int32		XoNbObjUser ;
#endif


void	Usage(char *p)
{
printf("%s [-haspitIBNX] [-r file.xor] [-o file.xo] [-Ppath] [-Rrootype] [-Ttargettype] [-Atype] [-Spath:value] file.xml\n",p);
printf("parse & load file.xml and dump object in compact XML form or access attributes\n");
printf(" -B : use oBix object model\n");
printf(" -N : assume all input prefixes are the native/real prefixes\n");
printf(" -R roottype : force the root type document\n");
printf(" -T targettype : partial decoding and encoding in context of targettype \n");
printf(" -P path : path access to an attribute to dump (ex:obj[0].attr)\n");
printf(" -S path=value : path access to an attribute to set (ex:obj[0].attr=xxx)\n");
printf(" -D defns : default output namespace (xmlns:defns=URI(defns)\n");
printf(" -A type : allocate an object of type 'type' and dump/save it (no parse)\n");
printf(" -X : retry search attribut name as gived by parser\n");
printf(" -W : search automatic wrapper\n");
printf(" -o file.xo : save loaded object using xo binary format\n");
printf(" -q file.xo : save loaded object using xo binary format (shared dico)\n");
printf(" -0 file.xo : save loaded object using xo binary format (shared+compression 0)\n");
printf(" -z file.xo : save loaded object using xo binary format (shared+lz77)\n");
#ifdef	XO_WITH_EXIP
printf(" -e file.exi : save loaded object using EXI schema less\n");
printf(" -E file.exi : save loaded object using EXI schema informed \n");
printf(" -x sch1.exs,sch2.exs... : uses EXI schemas\n");
printf(" -b schemaID : defines schemaID\n");
#else
printf(" -[eExb] : EXI not available\n");
#endif
printf(" -r file.xor ... -r file.xor : load references files. By default, all .xor in current directory are loaded\n");
printf(" -w : dump object in a readable XML form rather than compact one\n");
printf(" -a : dump object using xo ascii format rather than XML format\n");
printf(" -j : dump object using xo JSON format rather than XML format\n");
printf(" -p : parse only\n");
printf(" -i : interactive parsing\n");
printf(" -t : parser traces\n");
printf(" -s : no dump at all, just access to attribut\n");
printf(" -I : give some internal informations about the object loaded\n");
printf(" -n : do not load namespace.xns (loaded by default)\n");
printf(" -d : do not load shareddictionary.xdi (loaded by default)\n");
printf(" -f : force all xmlns: before saving\n");
printf(" -c : clear all xmlns: before saving (if both -c and -f clear is done before)\n");
printf(" -h : this help\n");


}

void	DevInfos(char *type,char *path)
{
	void	*o;
	int32	numobj;
	int32	numattr;
	int32	modeattr;
	int32	typeattr;
	char	*typeobj;
	char	*typename;

	o	= XoNmNew(type);
	if	(!o)
	{
		printf("XoNmNew('%s') error\n",type);
		exit(1);
	}

	numobj	= XoType(o);
	if	(numobj < 0)
	{
		printf	("'%s' cannot retreive numobj\n",type);
		exit(1);
	}
	typeobj	= XoGetRefObjName(numobj);
	if	(!type)
	{
		printf	("cannot retreive typeobj '%s'\n",typeobj);
		exit(1);
	}
	if	(strcmp(type,typeobj))
	{
		printf	("'%s' is an alias of '%s'\n",type,typeobj);
	}
	printf	("'%s' typeobj is %s\n",type,typeobj);
	numattr	= XoNmExistField(o,path);
	if	(numattr < 0)
	{
		printf	("'%s' does not exist in object '%s'\n",path,type);
		exit(1);
	}
	printf	("'%s' numobj is %d\n",type,numobj);
	printf	("'%s' numattr is %d\n",path,numattr);
	modeattr	= XoModeAttr(numattr);
	if	(modeattr < 0)
	{
		printf	("'%s'.'%s' cannot retreive modeattr\n",type,path);
		exit(1);
	}
	typeattr	= XoTypeAttr(numattr);
	if	(modeattr < 0)
	{
		printf	("'%s'.'%s' cannot retreive modeattr\n",type,path);
		exit(1);
	}
	typename	= XoGetRefObjName(typeattr);
	switch	(modeattr)
	{
	case	BASIC_ATTR:
		switch	(typeattr)
		{
		case XoInt4B:
		case XoString:
		case XoBuffer :
		case XoDate :
		case XoBool:
		case XoChar:
			printf	("'%s' is a basic value '%s'\n",path,typename);
		break;
		default :
			printf("bad type %d ???\n",typeattr);
			exit(1);
		break;
		}
	break;
	case	LIST_BASIC_ATTR:
		printf	("'%s' is a list of basic values '%s'\n",path,typename);
	break;
	case	OBJ_ATTR:
		printf	("'%s' is an object '%s'\n",path,typename);
	break;
	case	LIST_OBJ_ATTR:
		printf	("'%s' is a list of objects '%s'\n",path,typename);
	break;
	default :
		printf("bad mode ???\n");
		exit(1);
	break;
	}

	return;
}

void	SaveSharedCompress(char *fileout,void *o,int flgcompress)
{
	FILE	*f;
	char	*buf	= NULL;
	int	lg	= 0;
	int	ret;


	f	= fopen(fileout,"w");
	if	(!f)
	{
		printf("can not create file '%s'\n",fileout);
		exit(0);
	}

	ret	= XoSaveMemAllocFlg(o,&buf,&lg,XO_SHARED_DICO|flgcompress);
	if	(ret<0)
	{
		printf("XoSaveMemAlloc(%s) NOK\n",fileout);
		exit(0);
	}


	if	(write(fileno(f),buf,ret) != ret)
	{
		printf("can not write to file '%s'\n",fileout);
		exit(0);
	}

	free	(buf);

	fclose(f);
}

void	SaveExi(char *fileout,void *o,int flg, char *schemaID, int useRootObj)
{
	int	ret;

#ifdef	XO_WITH_EXIP
	ret	= XoWritExiEx(fileout, o, flg, schemaID, useRootObj);
	if	(ret<0)
	{
		printf("XoWritExiEx(%s) NOK\n",fileout);
		exit(0);
	}
#else
		printf("EXI not available XoWritExiEx(%s) NOK\n",fileout);
		exit(0);
#endif
}


int	main(int argc,char *argv[])
{
	char	tmp[512];
	char	fileout[512];
	char	schemas[512];
	char	schemaID[512];
	void	*o;
	char	*pt = argv[0];
	int	flags = 0;
	int	parseerr;

	int	c;
	char	*path	= NULL;
	char	*setpath	= NULL;
	char	*targettype	= NULL;
	char	*roottype	= NULL;
	char	*type		= NULL;
	int	save		= 1;
	int	writ		= 1;
	int	ascii		= 0;
	int	json		= 0;
	int	optr		= 0;
	int	optInfo		= 0;
	int	obix		= 0;
	int	allo		= 0;
	int	force		= 0;
	int	clear		= 0;
	int	namespace	= 1;
	int	shareddico	= 1;
	int	useshareddico	= 0;
	int	usecompress	= 0;
	int	useexi		= 0;
	int	useRootObj	= 0;
	int	useschema	= 0;
	int	nbload		= 0;
	char	*defns		= "";

	extern char *optarg;
	extern int optind;


	while (*pt == '.' || *pt == '/') pt++;
	if (pt && *pt == 'i')
		flags	= XOML_PARSE_INTERACTIF|XOML_PARSE_TRACE;



	XoInit(0);
	XoVersion();
//	XoAcceptDuplObjRef(); true by default

//	XoAddNameSpace("xsd",XOML_NAMESPACE_XSD);

//	XoDumpNameSpace(stdout);


	fileout[0]	= '\0';
	schemas[0]	= '\0';
	schemaID[0]	= '\0';
	while ((c = getopt (argc, argv, "dncfBNIXWajr:swhpite:o:q:0:z:P:S:T:R:A:D:E:x:b:")) != -1) 
	{
		switch(c)
		{
		case	'd':
			shareddico	= 0;
		break;
		case	'n':
			namespace	= 0;
		break;
		case	'D' :
			defns	= strdup(optarg);
		break;
		case	'c':
			clear	= 1;
		break;
		case	'f':
			force	= 1;
		break;
		case	'B':
			obix	= XOML_PARSE_OBIX;
		break;
		case	'N' :	// native prefixes
			flags	= flags | XOML_PARSE_NATIVENS;
		break;
		case	'X' :	// retry search attribut
			flags	= flags | XOML_PARSE_RETRYATTR;
		break;
		case	'W' :	// search wrapper
			flags	= flags | XOML_PARSE_AUTOWRAPP;
		break;
		case	'h':
			Usage(argv[0]);
			exit(0);
		break;
		case	'r' :
			if (XoExtLoadRef(optarg) < 0)
			{
				printf("XoExtLoadRef(%s) NOK\n",optarg);
				exit(1);
			}
			optr++;
		break;
		case	'I' :
			optInfo	= 1;
		break;
		case	's' :
			save	= 0;
		break;
		case	'a' :
			ascii	= 1;
		break;
		case	'j' :
			ascii	= 1;
			json	= 1;
		break;
		case	'w' :
			writ	= 0;
		break;
		case	'o' :
			strcpy	(fileout,optarg);
		break;
		case	'z' :
			useshareddico	= 1;
			usecompress	= XO_COMPRESS_LZ77;
			strcpy	(fileout,optarg);
		break;
		case	'0' :
			useshareddico	= 1;
			usecompress	= XO_COMPRESS_0;
			strcpy	(fileout,optarg);
		break;
		case	'q' :
			useshareddico	= 1;
			strcpy	(fileout,optarg);
		break;
		case	'e' :
			useexi		= 1;
			strcpy	(fileout,optarg);
		break;
		case	'E' :
			useexi		= 1;
			useschema	= 1;
			useRootObj	= 1;
			strcpy	(fileout,optarg);
		break;
		case	'x' :
			useexi		= 1;
			useschema	= 1;
			useRootObj	= 0;
			strcpy	(schemas,optarg);
		break;
		case	'b' :
			useexi		= 1;
			useschema	= 1;
			useRootObj	= 0;
			strcpy	(schemaID,optarg);
		break;
		case	'p' :	// parse only
			flags	= flags | XOML_PARSE_ONLY;
		break;
		case	'i' :	// interactif
			flags	= flags | XOML_PARSE_INTERACTIF;
		break;
		case	't' :	// traces
			XoDumpNameSpace(stderr);
			flags	= flags | XOML_PARSE_TRACE;
		break;
		case	'T' :	// target obj
			flags	= flags | XOML_PARSE_TARGET;
			targettype	= optarg;
			type		= targettype;
		break;
		case	'R' :	// root obj
			roottype	= optarg;
			type		= roottype;
		break;
		case	'P' :	// path to access element
			path		= optarg;
		break;
		case	'S' :	// path to set element
			setpath		= optarg;
		break;
		case	'A' :	// just alloc and save
			allo		= 1;
			roottype	= optarg;
			type		= roottype;
		break;
		}
	}

	if	(useexi)
	{
#ifdef	XO_WITH_EXIP
#else
		printf("EXI not available\n");
		exit(1);
#endif
	}

	if	(namespace)
		XoLoadNameSpaceDir(".");
	if	(shareddico)
		XoLoadSharedDicoDir(".");

	if	(obix)
	{
		flags	= flags | XOML_PARSE_OBIX;
	}

	if	(optr == 0)
	{
		nbload	= XoExtLoadRefDir(".");
		if	(nbload == 0)
		{
			sprintf	(tmp,"%s/m2mxoref/xoref",getenv("ROOTACT"));
			nbload	= XoExtLoadRefDir(tmp);
		}
		if (nbload < 0)
		{
			printf("XoExtLoadRefDir() error\n");
			exit(1);
		}
	}

	if	(allo && path && *path)
	{

		DevInfos(type,path);

		exit(0);
	}

	if	(allo)
	{
		o	= XoNmNewNest(type);
		if	(!o)
		{
			printf("XoNmNew('%s') error\n",type);
		}
//		XoNmSetAttr(o,XOML_BALISE_ATTR,type,0);
//		XoSetNameSpace(o,"m2m");
		XoSetNameSpace(o,NULL);
	}
	else
	{
		// really parse a file
		if	(optind >= argc)
		{
			Usage(argv[0]);
			exit(1);
		}


		o	= XoReadXmlEx(argv[optind],type,flags,&parseerr);
		if	(parseerr > 0)
		{
			printf("XoReadXmlEx() parsing error=%d\n",parseerr);
			exit(1);
		}
		if	(parseerr < 0)
		{
			printf("XoReadXmlEx() loading error=%d\n",parseerr);
			exit(1);
		}

		if (!o)
		{
			if ( (flags & XOML_PARSE_ONLY) == XOML_PARSE_ONLY)
				exit(0);
			printf("XoReadXmlEx() NOK\n");
			exit(1);
		}
	}
	if	(clear)
	{
		XoUnSetNameSpace(o,NULL);
	}
	if	(force)
	{
		XoSetNameSpace(o,NULL);
	}
	if	(setpath && *setpath)
	{
		char	*eq;

		eq	= strrchr(setpath,'=');
		if	(eq)
		{
			*eq	= '\0';
			eq++;
			if	(*setpath == '+')
				XoNmAddInAttr(o,++setpath,eq,0);
			else
				XoNmSetAttr(o,setpath,eq,0);
		}
	}


	if	(ascii && save)
	{
		if	(json)
			XoWritJsoEx(NULL,o,0);
		else
			XoSaveAscii(o,stdout);
	}

	if	(!ascii && save && writ)
	{
		void	*w;
		char	*buf;

//		XoNmSetAttr(o,"name$",XoNmType(o),0);	// XoNmType(o)
		w	= XoWritXmlMem(o,0,&buf,defns);
		if	(w)
		{
			printf("%s\n",buf);
			XoWritXmlFreeMem(w);
		}
	}

#if	0	// TODO trouver un writter avec FILE * pour passer stdout
#endif
	if	(!ascii && save && !writ)
	{
		FILE	*f;
		int	ret;
		char	*tmpf;
		char	tmp[2048];

//		XoSaveXml(o,stdout,0);
		tmpf	= tempnam("/tmp","xoxml");
		if	(!tmpf)
		{
			printf("cannot create temp file in /tmp ...\n");
			exit(1);
		}
		if	((ret=XoWritXmlEx(tmpf,o,0,defns)) < 0)
		{
			unlink(tmpf);
			printf("XoWritXmlEx() error=%d\n",ret);
			exit(1);
		}
		f	= fopen(tmpf,"r");
		if	(!f)
		{
			unlink(tmpf);
			printf("fopen(%s) error\n",tmpf);
			exit(1);
		}
		while	(fgets(tmp,sizeof(tmp),f))
			printf("%s",tmp);
		fclose(f);
		unlink(tmpf);
	}

	if	(path && *path)
	{
		void	*c;
		char	*pt;
		char	tmp[1024];

		printf	("path='%s'\n",path);
		pt	= "???";
		c	= XoNmGetAttr(o,path,0);
		if (c)
		{
			pt	= XoNmType(o);
		}
		printf	("\ttype='%s'\n",pt);

		pt	= tmp;
		pt	= XoNmAttrToAscii(o,path,tmp);
		printf	("\tval='%s'\n",pt);
	}
	if	(strlen(fileout) && !useshareddico && !useexi)
	{
		XoExtSave(fileout,o,NULL);
	}
	if	(strlen(fileout) && useshareddico)
	{
		if	(usecompress)
		{
			SaveSharedCompress(fileout,o,usecompress);
		}
		else
		{
			XoExtSaveFlg(fileout,o,NULL,XO_SHARED_DICO);
		}
	}
	if	(strlen(fileout) && useexi)
	{
#ifdef	XO_WITH_EXIP
		if	(useschema && !*schemaID && !useRootObj) {
			if	(*schemas)
				XoLoadEXISchemas(schemas);
			else
				XoLoadEXISchemaDir (".");
		}

		SaveExi(fileout,o,0,schemaID,useRootObj);
#else
		printf("EXI not available\n");
		exit(1);
#endif
	}

	if	(optInfo)
	{
		char	*pt;
		char	*ptcpr;
		int	lg;
		int	lgdico;
		int	lgcpr;

		lg	= XoLgSave(o);
		pt	= (char *)malloc(lg);
		if	(!pt)
			exit(1);
		ptcpr	= (char *)malloc(lg+4);
		if	(!ptcpr)
			exit(1);
		XoSaveMem(o,pt);
		lgdico	= XoLgDict(pt,lg);
		printf	("length required to serialize this object : %d (dictionary size %d)\n",lg,lgdico);
		lgcpr	= XoMemCompress((unsigned char *)pt,lg,
						(unsigned char *)ptcpr+4);
		printf	("length required to serialize this compressed object : %d\n",lgcpr/8);
		lg	= lg - lgdico;
		lgcpr	= XoMemCompress((unsigned char *)pt,lg,
						(unsigned char *)ptcpr+4);
		printf	("length required to serialize this compressed object (no dictionary) : %d\n",lgcpr/8);
	}
	XoFree(o,1);
	XoDumpNbOcc(stdout,1,0);
	return	0;
}

