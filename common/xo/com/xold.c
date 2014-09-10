
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
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>

#include	"xoapipr.h"

t_objet_ref	*XoTabObjUser ;
int		XoNbObjUser  = 0 ;


int	NbObj;

int	opto = 0;
int	optt = 0;
int	optv = 0;

int	CmpObj(char *name,void *o1,void *o2)
{
	int32	ret;
	int32	lg1;
	int32	lg2;
	char	*pt1;
	char	*pt2;

	lg1	= XoLgSave(o1);
	lg2	= XoLgSave(o2);
	if	(lg1 != lg2)
		return	-1;
	pt1	= (char *)malloc(lg1);
	pt2	= (char *)malloc(lg2);
	if	(!pt1 || !pt2)
	{
		printf("###### cannot alloc mem for obj diff: %d %d\n",lg1,lg2) ;	
		exit(1) ;
	}
	XoSaveMem(o1,pt1);
	XoSaveMem(o2,pt2);

	ret	= memcmp(pt1,pt2,lg1);
	free	(pt1);
	free	(pt2);

//printf("cmp %s %d %d ret=%d\n",name,lg1,lg2,ret);
	return	ret;
}

void	FromTree(char *name,int32 numobj)
{
	t_objet_ref	*or ;

	or	= XoGetRefObj(numobj);
	if	(!or)
	{
		printf("###### cannot get objet structure : %s\n",name);
		exit(1) ;
	}
	if	(or->or_from && *(or->or_from))
	{
		printf("\t\t from %s\n",or->or_from);
		numobj	= XoGetNumType(or->or_from);
		if	(numobj > 0)
			FromTree(name,numobj);
	}
}


void	NewCfg(char *filename,void *cfg)
{
	int	nbobj;
	int	i;
	void	*obj;
	char	*name;
	char	*alias;
	int32	ret;
	int32	numobj;
	int32	nbattr;
	char	*exist;

	nbobj	= XoNmNbInAttr(cfg,"XoCfgListObjRef",0);
	printf	("%s (#%d obj)\n",filename,nbobj);

	for	(i = 0 ; i < nbobj ; i++)
	{
		obj	= XoNmGetAttr(cfg,"XoCfgListObjRef[%d]",0,i);
		if	(!obj)
			break;
		name	= XoNmGetAttr(obj,"XoObjRefName",0);
		alias	= XoNmGetAttr(obj,"XoObjRefAlias",0);
		nbattr	= XoNmNbInAttr(obj,"XoObjRefListAttr",0);
		if	(!alias)
		{
			if	(optv)
				printf	("\t%s (#%d attr) ",name,nbattr);
		}
		else
		{
			if	(optv)
				printf	("\t%s (aliasof %s)",name,alias);
		}

		exist	= "";
		numobj	= XoGetNumType(name);
		if	(numobj > 0)
		{	// il y a deja un objet ayant ce nom
			void	*or;
			exist	= "redefined";
			or	= XoNewObjRef(numobj);
			if	(!or)
			{
				if	(optv)
					printf	(" %s\n",exist);
				printf("###### cannot alloc obj for obj diff: %s\n",name) ;	
				exit(1) ;
			}
			ret	= CmpObj(name,obj,or);
			if	(ret != 0)
			{
				if	(optv)
					printf	(" %s\n",exist);
				printf("###### obj clash: %s\n",name) ;	
				exit(1) ;
			}
			XoFree(or,1);
		}
		ret	= XoAddObjRef(obj);
		if	(ret < 0)
		{
			printf("###### error add obj : %s\n",name) ;	
			exit(1) ;
		}
		if	(optv)
			printf	(" %s\n",exist);
		if	(numobj > 0)
			continue;
		NbObj++;
		numobj	= XoGetNumType(name);
		if	(numobj < 0)
		{
			printf("###### error retrieve obj : %s\n",name) ;	
			exit(1) ;
		}
		if	(optt)
		{
			FromTree(name,numobj);
#if	0
			char	*tabfrom[256];
			int	n;
			ret	= XoGetRefObjFromList(numobj,tabfrom,0);
			for	(n = 0 ; ret > 0 && n < ret ; n++)
			{
				printf("\t\t from %s\n",tabfrom[n]);
			}
#endif
		}

	}
}

void	Usage(char *p)
{
printf("%s [-t] [-o fileout.xor] f1.xor ... fN.xor\n",p);
printf(" + link a set of xo declarations files into a single file\n");
printf(" + duplicate declarations are eliminated\n");
printf(" + different duplicate declarations cause an abort (obj clash)\n");
printf(" -o fileout.xor : generate declarations in fileout.xor\n");
printf(" -v : verbose\n");
printf(" -t : print tree derivation\n");
}

int	main(int argc,char *argv[])
{
	char	*cfg  = NULL ;
	int	f ;
	int	nb ;

	extern char *optarg;
	extern int optind;
	int	c ;
	int	fout ;
	char	*fileout	= NULL;

	if	( argc < 2 )
	{
		Usage	(argv[0]);
		exit	(1) ;
	}

	while ((c = getopt (argc, argv, "vto:")) != -1) 
	{
		switch(c)
		{
		case	'v':
			optv	= 1;
		break;
		case	't':
			optt	= 1;
			optv	= 1;
		break;
		case	'o':
			opto	= 1;
			fileout	= strdup(optarg);
			unlink(fileout);
		break;
		}
	}

	XoInit	(0) ;

	for	( nb = optind ; nb < argc ; nb++ )
	{
		if	(opto && strcmp(argv[nb],fileout) == 0)
			continue;
		f	= open(argv[nb],XO_FIC_BINARY|O_RDONLY) ;
		if	( !f )
			exit(1) ;
		cfg	= XoLoad(f) ;
		if	( !cfg )
		{
			printf("###### \tload error : %s\n",argv[nb]) ;	
			exit(1) ;
		}

		NewCfg(argv[nb],cfg);
		XoFree	(cfg,1) ;
	}

	if	(opto && strlen(fileout))
	{
		printf	("generate link in '%s' (#%d): ...\n",fileout,NbObj);
		fout = open(fileout,XO_FIC_BINARY|O_RDWR|O_TRUNC|O_CREAT,0666) ;
		if	( fout < 0 )
		{
			printf("###### \tcreate error : %s\n",fileout) ;	
			exit(1) ;
		}
		if	( XoSaveRef(fout) < 0 )
		{
			printf("###### '%s' error while saving\n",fileout) ;
			unlink(fileout);
			exit(1);
		}
		close(fout);
	}


	XoEnd() ;

	exit	(0) ;
}
