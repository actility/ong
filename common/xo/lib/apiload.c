
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
#include	<sys/types.h>
#include	<sys/stat.h>
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

extern	char	*XoDico;	// heap pointer on data dictionary
extern	int32	XoOffDico;	// next offset (ie the useful lenght)

void LZ_Uncompress( char *in, char *out,unsigned int insize );

static	char	*xo_file = __FILE__ ;

/*
#define TRACE
*/

#ifdef TRACE
static int32	nbImb;
static int32	i;
#endif

#define CFG_TAILLE_BUF		1024	/* taille du buffer de lecture disque */

#if	0
static	int32 CfgNbCar;	/* nb de car restant dans le buffer */
static	char	*CfgCarCour;	/* caractere courant du buffer */
static	char	*CfgBuf;	/* point32eur a buffer de lecteur */
static	int32 CfgFile;	/* descripteur de fichier */
static	char	*CfgBufDico;	/* dictionnaire des noms d'attr */
static	int32 CfgDebut;	/* debut de la zon de lecture */
static	int32 CfgFin;	/* debut de la zon de lecture */
static	int32 CfgPos;	/* position courante : cas de memoire */
static	char	*CfgMem;	/* point32eur a buffer de lecture */
static	int32 CfgLgMem;	/* lg de CfgMem */
#endif

typedef	struct
{
	int32	CfgNbCar;	/* nb de car restant dans le buffer */
	char	*CfgCarCour;	/* caractere courant du buffer */
	char	*CfgBuf;	/* point32eur a buffer de lecteur */
	int32	CfgFile;	/* descripteur de fichier */
	char	*CfgBufDico;	/* dictionnaire des noms d'attr */
	int32	CfgDebut;	/* debut de la zon de lecture */
	int32	CfgFin;		/* debut de la zon de lecture */
	int32	CfgPos;		/* position courante : cas de memoire */
	char	*CfgMem;	/* point32eur a buffer de lecture */
	int32	CfgLgMem;	/* lg de CfgMem */
	int32	CfgFlags;

	char	*buf;
	int	lgBuf;

	char	*uncompr;
	int32	lguncompr;
}	t_ctxt;

#define	CfgNbCar	ctxt->CfgNbCar
#define	CfgCarCour	ctxt->CfgCarCour
#define	CfgBuf		ctxt->CfgBuf
#define	CfgFile		ctxt->CfgFile
#define	CfgBufDico	ctxt->CfgBufDico
#define	CfgDebut	ctxt->CfgDebut
#define	CfgFin		ctxt->CfgFin
#define	CfgPos		ctxt->CfgPos
#define	CfgMem		ctxt->CfgMem
#define	CfgLgMem	ctxt->CfgLgMem
#define	CfgFlags	ctxt->CfgFlags


static char *cfg_rdObj(t_ctxt *ctxt,int32 skeep);

void *__XoLoadMem(char *buf, int32 size);

/*
 *	fonction de lecture des caracteres 
 */

static	int32	do_cfg_read(t_ctxt *ctxt,int32 file,char *buf,int32 nb)
{
	int32	reste ;

	if	( file != -1 )
	{
		return	( read((int)file,buf,(unsigned int)nb) ) ;
	}
	reste	= CfgLgMem - CfgPos ;
	if	( reste <= 0 )
		return	0 ;
	if	( reste < nb )
	{
		nb	= reste ;
	}
	memcpy	(buf,CfgMem+CfgPos,(size_t)nb) ;
	CfgPos	+= nb ;
	return	nb ;
}

/*
 *	fonction de positionnement
 */

static	int32	do_cfg_lseek(t_ctxt *ctxt,int32 file,int32 offset,int32 where)
{
	if	( file != -1 )
	{
		return	( lseek((int)file,(long)offset,(int)where) ) ;
	}
	switch	(where)
	{
		case	0 :
			CfgPos	= offset ;
		case	1 :
			return	CfgPos ;
		break ;
	}
	return	0 ;
}


/*
 *	define de remplissage du buffer
 */

#define CFG_FILBUF()	\
(CfgNbCar = do_cfg_read(ctxt,CfgFile,CfgCarCour = CfgBuf,CFG_TAILLE_BUF))

/*
 *	define apres lecture dans CfgBuf
 */

#define CFG_POSTRD(nbRd)		\
{					\
	int32	nb;			\
					\
	nb = ((nbRd+3)>> 2 )<<2;	\
	CfgCarCour	+= nb;		\
	CfgNbCar	-= nb;		\
	if(CfgNbCar <= 0)		\
	{	/* CarBuf vide */	\
		CFG_FILBUF();		\
	}				\
}


/*
 *	minimum de deux nombres
 */

#define cfg_min(x,y)	((x>y)?y:x)


/*
 *	position
 */
	
static int32 cfg_position(t_ctxt *ctxt)
{
	int32 retour;

	retour = do_cfg_lseek(ctxt,CfgFile,0L,1);
	retour -= CfgNbCar;
	return retour;
}


/*
 *	allerA
 */
	
static void cfg_allerA(t_ctxt *ctxt,int32 offset)
{


	do_cfg_lseek(ctxt,CfgFile,offset,0);
}

/*
 *	lecture d'un int32
 */

static int32 cfg_rdInt(t_ctxt *ctxt,int32 *nombre)
{
	extern int32	XolittleBig;

	if(CfgNbCar < 4)
	{	/* on ne peut pas lire un int32 */
		XOERR(	"cannot read 1 int32",0);
		return(-1);
	}
#if	0
	/* lecture d'un int32 dans CarBuf */
	if(littleBig == 0)
	{
		char buf[4];
		char *pt = &buf[0];

		buf[0] = 1;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;

		if(*(int32 *) pt == (int32)1)
		{
			littleBig = 1;
		}
		else
		{
			littleBig = 2;
		}
	}
#endif
	if(XolittleBig == 1)
	{	/* format PC utilise par defaut */
		*nombre = *(int32 *) CfgCarCour;
	}
	else
	{	/* format RS 6000 */
		register uint32 valLu;

		valLu = *(int32 *) CfgCarCour;
		*nombre =	(valLu >> 24) | (valLu << 24) |
				((valLu >> 8) & (int32) 0xFF00) |
				((valLu << 8) & (int32) 0xFF0000);
	}
	CFG_POSTRD(4);

	return(1);
}


/*
 *	lecture d'un buffer
 */

static char *cfg_rdBuf(t_ctxt *ctxt,int32 *lgRd)
{
#if	0
	static char		*buf = NULL;
	static int32		lgBuf = 0;
#endif
	int32		n;
	int32		lgRest;
	int32		lgMod4;

	/* chargement de la longueur a lire */
	if(cfg_rdInt(ctxt,lgRd) == -1)
	{
		XOERR("	cannot get buffer size.",0);
		return NULL;
	}

	if(*lgRd == 0)
		return	NULL;

	/* modulo pour obtenir la longueur reel */
	lgMod4 = ((*lgRd+3)>> 2)<< 2;


	/* allocation du buffer */
	if(!ctxt->buf)
	{	/* si le buffer n'est pas alouee */
		if(!(ctxt->buf = malloc((size_t)lgMod4)))
		{
			XOERR(	"malloc() error.", lgMod4);
			return NULL;
		}
		ctxt->lgBuf	= lgMod4;
	}
	else if(ctxt->lgBuf < lgMod4)
	{	/* si le buffer est trop petit */
		char	*bufNw;

//printf("lgBuf=%d lgMod4=%d\n",ctxt->lgBuf,lgMod4);
		if(!(bufNw = realloc(ctxt->buf, (size_t)lgMod4)))
		{
			XOERR(	"realloc() error .\n", lgMod4);
			return NULL;
		}
		ctxt->buf	= bufNw;
		ctxt->lgBuf	= lgMod4;
	}

	n = cfg_min(lgMod4,CfgNbCar);
	memcpy(ctxt->buf, CfgCarCour, (size_t)n);
	lgRest	= lgMod4 - n;

	if(!lgRest)
	{	/* la lecture est complete */
		CFG_POSTRD(n);
		return ctxt->buf;
	}

	/* lecture du reste */
	if(lgRest > ( (CFG_TAILLE_BUF * 7 ) / 10 ) )
	{	/* c'est plus avantageux de faire une lecture direct */
		if(lgRest != do_cfg_read(ctxt,CfgFile, ctxt->buf + n, lgRest))
		{	/* pas assez de caractere dans fichier */
			XOERR(	"cannot read buffer.",*lgRd);
			return NULL;
		}
		CFG_FILBUF();
		return ctxt->buf;
	}

	/* une lecture bufferise est plus avantageuse */
	CFG_FILBUF();

	/* copie du reste du buffer */
	if( CfgNbCar < lgRest)
	{
		XOERR(	"cannot read buffer.",*lgRd);
		return NULL;
	}
	memcpy(ctxt->buf+n,CfgBuf,(size_t)lgRest);
	CFG_POSTRD(lgRest);

	return ctxt->buf;
}


/*
 *	fonction de lecture d'une valeur d'attribut simple
 *	retourne -1 si erreur,
 *	retourne 1 si ok.
 */

static int32 cfg_rdValAttSp(t_ctxt *ctxt,char *obj, int32 mode, char *nameAttr)
{
	int32	nombre,lgBuf,lgAdd;
	char		*buf,*add,*objFils = NULL;
	char		*addwithpref	= NULL;
	int32		modeAtt, typeAtt, numAtt;
	char 		tmp[64]; /* conversion entier <-> string */
	void		*value ;

	t_attribut_ref	*ar ;	// TODO jusqu'a present c'etait cache


	modeAtt= typeAtt= numAtt= 0;
	if(obj)
	{	/* si on est en train de charger un objet */
		numAtt = XoNumAttrInObj(obj,nameAttr);
		if(numAtt < 0)
		{	/* si l'attribut n'existe plus */
			obj = NULL;
		}
		else
		{	/* si l'attribut existe */
			modeAtt	= XoModeAttr(numAtt);
			typeAtt	= XoTypeAttr(numAtt);
		}
	}

	ar	= XoGetRefAttr(numAtt) ;
	if	(!ar)
	{
		XOERR(	"	cannot find arref",numAtt);
		return -1;
	}

	switch(GET_MDSV_TYPE(mode))
	{
	case CFG_INT :
	case CFG_INT_UNSET :
	case CFG_LSTINT :
		if(cfg_rdInt(ctxt,&nombre) == -1)
		{
			XOERR(	"	cannot read int32.\n",0);
			return -1;
		}

#ifdef TRACE
		for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
		fprintf(stderr,"lecture d'un int32 = %ld.\n",(long)nombre);
#endif

		if(!obj)
		{	/* on ne fait que la lecture */
			return 1;
		}
		
		/* chargement en memoire */
		if(typeAtt==XoString)
		{ /* RVM 13/12/1996 - Cast long->string au chargement */
			sprintf(tmp,"%d",nombre);
			value	= tmp ;
		}
		else
		{
			value	= (void *)nombre ;
		} 
		switch(modeAtt)
		{
		case BASIC_ATTR:
			if(IS_MDSV_SAVE_UNSET(mode))
			{
				XoUnSetAttr(obj,numAtt) ;
				break ;
			}
			if(XoSetAttr(obj,numAtt,value,0) < 0)
			{
				XOERR(	"cannot set attr.", 0);
				return -1;
			}
			break;
		case LIST_BASIC_ATTR:
			if(XoAddInAttr(obj,numAtt,value,0) < 0)
			{
				XOERR(	"cannot add attr.", 0);
				return -1;
			}
			break;
		}
		return 1;

	case CFG_BUF :
	case CFG_BUF_UNSET :
	case CFG_LSTBUF :
		add	= NULL;
		lgAdd	= 0;
		if((CfgFlags & XO_SHARED_DICO) == XO_SHARED_DICO 
				&& IS_SHDICO(ar))
		{
			/* chargement de l'offset */
			if(cfg_rdInt(ctxt,&lgBuf) == -1)
			{
				XOERR("	cannot get buffer offset for shdico",0);
				return -1;
			}
//			printf("xxxxx sz=%d\n",lgBuf);
			if	(lgBuf == 0)
			{ // declared shdico for data but not found in dico
				if(!(buf = cfg_rdBuf(ctxt,&lgBuf)))
				{
					XOERR(	"cannot read buffer.", 0);
					return -1;
				}
			}
			else
			{
				buf	= XoDico + lgBuf;
				if(!(add = cfg_rdBuf(ctxt,&lgAdd)))
				{ // buf complet pas de suite
					lgAdd	= 0;
				}
//printf("xxxx pref='%s' buf='%s'\n",buf,add);
			}
		}
		else
		{
			if(!(buf = cfg_rdBuf(ctxt,&lgBuf)))
			{
				XOERR(	"cannot read buffer.", 0);
				return -1;
			}
		}	

#ifdef TRACE
		for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
		fprintf(stderr,"lecture d'un buffer : %ld caracteres.\n",(long)lgBuf);
		for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
		fprintf(stderr,"buffer = %s.\n",buf);
#endif

		if(!obj)
		{	/* on ne fait que la lecture */
			return 1;
		}
		
		/* chargement en memoire */
		if (typeAtt==XoInt4B||typeAtt==XoBool
		|| typeAtt==XoChar||typeAtt==XoDate)
		{ /* RVM 13/12/1996 - Cast string->long au chargement */
			lgBuf	= 0 ;
			nombre	= xoatoi(buf);
			value	= (void *)nombre ;
		}
		else
		{
			value	= buf ;
		}

		// reconstruire le buffer avec le prefixe du dico & les donnees
		if	(add && lgAdd > 0)
		{
			addwithpref	= (char *)malloc(lgBuf+lgAdd+10);
			if	(!addwithpref)
			{
				XOERR(	"cannot alloc buffer pref+data.", 0);
				return -1;
			}
			strcpy(addwithpref,buf);
			strcat(addwithpref,add);
			value	= addwithpref;
			lgBuf	= strlen(value)+1;
		}
		switch(modeAtt)
		{
		case BASIC_ATTR:
			if(IS_MDSV_SAVE_UNSET(mode))
			{
				XoUnSetAttr(obj,numAtt) ;
				break ;
			}
			if(XoSetAttr(obj,numAtt,value,lgBuf) < 0)
			{
				XOERR(	"cannot set buffer.", 0);
				return -1;
			}
			break;
		case LIST_BASIC_ATTR:
			if(XoAddInAttr(obj,numAtt,value,lgBuf) < 0)
			{
				XOERR(	"cannot add buffer.", 0);
				return -1;
			}
			break;
		}
		if	(addwithpref)
		{
			free(addwithpref);
		}
		return 1;

	case CFG_OBJ :
	case CFG_LSTOBJ :
#ifdef TRACE
		for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
		fprintf(stderr,"lecture d'un objet.\n");
#endif

		if(!(objFils = cfg_rdObj(ctxt,obj?0:1)))
		{
			XOERR(	"cannot read object.", 0);
			return -1;
		}

		if(!obj)
		{	/* on ne fait que la lecture */
			return 1;
		}
		
		/* chargement en memoire */
		switch(modeAtt)
		{
		case OBJ_ATTR:
			if(XoSetAttr(obj,numAtt,objFils,0) < 0)
			{
				XOERR(	"cannot set obj.", 0);
				XoFree(objFils,1);
				return -1;
			}
			break;
		case LIST_OBJ_ATTR:
			if(XoAddInAttr(obj,numAtt,objFils,0) < 0)
			{
				XOERR(	"cannot add obj.", 0);
				XoFree(objFils,1);
				return -1;
			}
			break;
		}
		return 1;
	default :
		XOERR("mode error.",0); 
		return -1 ;
		break;
	}
	return	1 ;
}


/*
 *	fonction de lecture d'une valeur d'attribut
 *	retourne -1 si erreur,
 *	retourne 1 si ok.
 */

static int32 cfg_rdValAtt(t_ctxt *ctxt,char *obj, char *nameAttr)
{
	int32	i,nbEle,mode;
	int32		retour;

	retour = cfg_rdInt(ctxt,&mode);
	if(retour < 0)
	{
		XOERR("cannot read mode.",0);
		return -1;
	}

	switch(GET_MDSV_TYPE(mode))
	{
	case CFG_OBJ :
	case CFG_BUF :
	case CFG_INT :
	case CFG_BUF_UNSET :
	case CFG_INT_UNSET :
		return cfg_rdValAttSp(ctxt,obj,mode,nameAttr);
	case CFG_LSTOBJ :
	case CFG_LSTBUF :
	case CFG_LSTINT :
		if(-1 == cfg_rdInt(ctxt,&nbEle))
		{	/* erreur de lecture */
			XOERR(	"	cannot read elem count in list.\n", 0);
			return -1;
		}

#ifdef TRACE
		for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
		fprintf(stderr,"lecture d'une liste de %ld elements.\n",(long)nbEle);
#endif

		for(i = 0;i < nbEle;i++)
		{
#ifdef	TRACE
			for(j=0;j<nbImb;j++) fprintf(stderr,"\t");
			fprintf(stderr,"lecture du %ld ieme/%ld element\n",(long)(i+1),(long)nbEle) ;
#endif
			if(cfg_rdValAttSp(ctxt,obj,mode,nameAttr)== -1)
			{
				XOERR(	"	cannot read elem in list.\n", 0);
				return -1;
			}
		}
		return 1;
	default :
		XOERR("mode error.", 0);
		return -1;
	}
	return -1;
}


/*
 *	fonction de lecture d'un attribut d'un objet,
 *	retourne 0 si plus d'attribut,
 *	retourne 1 si lecture ok,
 *	retourne -1 en cas d'erreur.
 */

static int32 cfg_rdAtt(t_ctxt *ctxt,char *obj)
{
	int32	offset;
	char		*nameAttr;

	if(cfg_rdInt(ctxt,&offset) == -1)
	{	/* erreur de lecture */
		XOERR("	cannot read name attribut offset.\n", 0);
		return -1;
	}

	if(!offset)
	{	/* fin objet */
		return 0;
	}

#ifdef TRACE
	for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
	fprintf(stderr,"debut d'un attribut\n") ;
#endif

	/* recherche dans le dico le nom de l'attr, est retourne son num */
	nameAttr = CfgBufDico + offset;

#ifdef TRACE
	for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
	fprintf(stderr,"nom d'attribut : <%s>.\n",nameAttr) ;
#endif

	return cfg_rdValAtt(ctxt,obj,nameAttr);
}


/*
 * fonction de lecture d'un objet, connaissant son type,
 * retourne NULL si erreur,
 * retourne &objet si ok.
 */

static char *cfg_rdObj(t_ctxt *ctxt,int32 skeep)
{
	int32	retour;	/* 0 plus d'attributs, 1 ok, -1 erreur */ 
	void	*obj = NULL;
	int32	offsetObjet;
	int32	class;

	/* recuperation de la classe de l'objet */
	if(cfg_rdInt(ctxt,&offsetObjet) == -1)
	{	/* erreur de lecture */
		XOERR("	cannot read root type offset.\n", 0);
		return NULL;
	}

#ifdef TRACE
	for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
	fprintf(stderr,"charge objet : [%s]\n",CfgBufDico+offsetObjet) ;
	fflush(stderr) ;
	nbImb++;
#endif

	/* recherche dans le dico le nom du type racine */
	/* et recuperation de la classe */
	if(!skeep)
	{
		char	*rootname = (CfgBufDico) + offsetObjet;

		class = XoGetNumType(rootname);
		if(class < 0)
		{
			XOERR("cannot find root object class",offsetObjet);
#ifdef	TRACE
			fprintf(stderr, "	nom type objet : <%s> %ld.\n",
				CfgBufDico + offsetObjet,(long)offsetObjet);
#endif
			skeep = 1;
		}
	}

	/* creation de l'objet */
	if(!skeep)
	{
		obj = XoNew(class);
	}

	/* renseignement des attribut */
	while((retour = cfg_rdAtt(ctxt,obj)) == 1);

	/* liberation de l'objet en cas d'erreur */
	if(retour == -1)
	{	/* erreur de lecture */
		XOERR("	cannot read object.\n", 0);
		XoFree(obj,1);
		return NULL;
	}

#ifdef TRACE
	nbImb--;
	for(i=0;i<nbImb;i++) fprintf(stderr,"\t");
	fprintf(stderr,"fin d'objet [%s]\n",CfgBufDico+offsetObjet) ;
	fflush(stderr) ;
#endif

	if(!obj) return (char *) 1;
	return obj;
}


/*
 * fonction de lecture d'une arborescence,
 * retourne NULL si erreur,
 * retourne &objet si ok
 */

static char *cfg_rdTree(t_ctxt *ctxt,char *extDico,int32 szExtDico)
{
	int32		offsetDico = 0;
	char		*dico;
	int32		lgDico;
	char		*retour;
	uint32		version = 0;
	uint32		format = 0;
	uint32		flags = 0;
	int32		douncompress = 0;

#ifdef TRACE
	nbImb = 0;
#endif
	/* lecture du magic */
	if(!cfg_rdInt(ctxt,(int32 *)&version))
	{
		XOERR("cannot read magic.",0);
		return NULL;
	}
	if(version != XO_FMAGIC)
	{
		XOERR("cannot read magic.",0);
		return NULL;
	}

	/* lecture de la version */
	if(!cfg_rdInt(ctxt,(int32 *)&version))
	{
		XOERR("cannot read version.",0);
		return NULL;
	}

	flags	= (version & 0x0000FFFF);
	format	= (version >> 16) & 0x00FF;
	version	= (version >> 24);
/*
fprintf(stderr,"version=%d format=%d flags=%d\n",version,format,flags);
*/

	if(version < XO_VERSION0)
	{
		XOERR("bad version.",0);
		return NULL;
	}

	if(version > XO_VERSION)
	{
		XOERR("bad version.",0);
		XOERR("\trd version ",version - XO_VERSION0);
		XOERR("\tlast version ",XO_VERSION - XO_VERSION0);
		return NULL;
	}


	if(format != XO_FMT_STD_BIN)
	{
		XOERR("bad format.",0);
		return NULL;
	}

	CfgFlags	= flags;

	if	((flags & XO_COMPRESS_0) == XO_COMPRESS_0)
		douncompress	= 1;
	if	((flags & XO_COMPRESS_LZ77) == XO_COMPRESS_LZ77)
		douncompress	= 2;

//printf("cfg_rdTree : buf=%p lg=%d uncomrp=%p douncompr=%d\n",CfgMem,CfgLgMem,
//ctxt->uncompr,douncompress);

	if	(!ctxt->uncompr && douncompress)
	{
		struct	stat	st;
		int32		szorig = 0;	// original size
		int32		szcomp = 0;	// compressed size
		int32		szout = 0;
		char		*inb	= NULL;
		char		*outb;
		int32		hdsz	= XO_HEADER_SIZE;

		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
		{
			XOERR("compress => shared_dico required",0);
			return NULL;
		}

		/* lecture du nombre de bytes compresses */
		if(!cfg_rdInt(ctxt,&szorig))
		{
			XOERR("cannot read #bytes compress.",0);
			return NULL;
		}

		if	(CfgFile == -1)
		{	// in memory
			inb	= CfgMem + hdsz;
			szcomp	= CfgLgMem - hdsz;
		}
		else
		{	// from file
			if	(fstat(CfgFile,&st) < 0 || st.st_size <= 16)
			{
				XOERR("cannot fstat file",0);
				return NULL;
			}
			szcomp	= st.st_size - hdsz;
	
			inb	= (char *)malloc(szcomp);
			if	(!inb)
			{
				XOERR("cannot alloc compress in buffer",szcomp);
				return NULL;
			}
			lseek	(CfgFile,hdsz,0);
			if	(read(CfgFile,inb,szcomp) != szcomp)
			{
				free	(inb);
				XOERR("cannot read file",szcomp);
				return NULL;
			}
		}
		if	(szcomp > szorig)
		{
			if (inb && CfgFile != -1)
				free	(inb);
			XOERR("cannot uncompress szcomp > szorig",szorig);
			return NULL;
		}
		outb	= (char *)malloc(hdsz+(szorig)+10);
		if	(!outb)
		{
			if (inb && CfgFile != -1)
				free	(inb);
			XOERR("cannot alloc compress out buffer",szorig);
			return NULL;
		}

		if	((flags & XO_COMPRESS_0) == XO_COMPRESS_0)
			szout	= XoMemInflate(inb,szcomp,outb+hdsz);

		if	((flags & XO_COMPRESS_LZ77) == XO_COMPRESS_LZ77)
		{
			LZ_Uncompress(inb,outb+hdsz,szcomp);
			szout	= szorig;
		}

//printf("szorig=%d szcomp=%d szout=%d szleft=%d\n",szorig,szcomp,szout,*inb);
		if	(szout == 0 || szout != szorig)
		{
			if (inb && CfgFile != -1)
				free	(inb);
			free	(outb);
			XOERR("cannot uncompress out buffer",szout);
			return NULL;
		}

		if (inb && CfgFile != -1)
			free	(inb);
		memcpy	(outb,CfgBuf,8);
		// XO_COMPRESS_0 => XO_SHARED_DICO => dico size == 0
		outb[8] = outb[9] = outb[10] = outb[11] = '\0';
//write(2,outb,szout+hdsz);
		ctxt->uncompr	= outb;
		ctxt->lguncompr	= hdsz + szout;

		// => restart with XoLoadMem
		return	NULL;
	}

	/* lecture de l'offest */
	if(!cfg_rdInt(ctxt,&offsetDico))
	{
		XOERR("cannot read offset dico.",0);
		return NULL;
	}

	if(offsetDico <= 0)
	{
		if	(XoDico == NULL || XoOffDico <= 0)
		{
			XOERR(	"	dictionary size NULL.",offsetDico);
			return NULL;
		}
		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
		{
			XOERR(	"	dictionary size NULL and flags shared dico not set",offsetDico);
			return NULL;
		}
		extDico	= XoDico;
		lgDico	= XoOffDico;
	}

	/* lecture de l'offset de debut */
	CfgDebut = cfg_position(ctxt);

	/* possitionnement au debut du dico */
	cfg_allerA(ctxt,offsetDico);
	CFG_FILBUF();

	if	(!extDico)
	{
		/* lecture du dico */
		if(!(dico = cfg_rdBuf(ctxt,&lgDico)))
		{
			XOERR(	"	cannot read dictionary.",0);
			return NULL;
		};

		if(lgDico <= 0)
		{
			XOERR(	"	dictionary size NULL.",lgDico);
			return NULL;
		}

		CfgBufDico = (char *) malloc((size_t)lgDico);
		if (!CfgBufDico)
		{
			XOERR(	"	cannot alloc dictionary.",lgDico);
			return NULL;
		}
		memcpy(CfgBufDico,dico,(size_t)lgDico);
	}
	else
	{
		/* dictionnaire externe	*/
		CfgBufDico	= extDico;
		lgDico		= szExtDico;
	}

	CfgFin = cfg_position(ctxt);

	cfg_allerA(ctxt,CfgDebut);
	CFG_FILBUF();

	/* lecture de l'objet */

#ifdef TRACE
	fprintf(stderr,"\ndebut de lecture racine\n") ;
#endif

	retour = cfg_rdObj(ctxt,0);

#ifdef TRACE
	fprintf(stderr,"fin de lecture racine\n\n") ;
#endif

	if	(!extDico)
		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
			free(CfgBufDico);
	cfg_allerA(ctxt,CfgFin);

	if(retour==(char *)1)
	{
		XOERR("root class undefined",0);
		return	NULL;
	}

	return retour;
}

/*
 *	charge des objets de donnees depuis de la memoire
 *	avec un dico externe
 */

/*DEF*/	void *__XoLoadMemExtDict(char *buf, int32 size, char *dico, int32 szDico)
{
	void	*ret;
	char	bufMem[CFG_TAILLE_BUF];	/* buffer de lecture memoire */
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));

	CfgBuf	= bufMem;
	CfgFile	= -1;
	CfgMem	= buf ;
	CfgLgMem= size ;
	CfgPos	= 0;
	if(!CFG_FILBUF())
	{
		XOERR(	"root empty.", 0);
		return NULL;
	}

	ret	= cfg_rdTree(ctxt,dico,szDico);
	if	(ctxt->buf)
		free(ctxt->buf);
	return	ret;
}


/*
 *	charge des objets de donnees depuis de la memoire
 */

/*DEF*/	void *__XoLoadMem(char *buf, int32 size)
{
	void	*ret;
	char	bufMem[CFG_TAILLE_BUF];	/* buffer de lecture memoire */
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));
	CfgBuf	= bufMem;
	CfgFile	= -1;
	CfgMem	= buf ;
	CfgLgMem= size ;
	CfgPos	= 0;
	if(!CFG_FILBUF())
	{
		XOERR(	"root empty.", 0);
		return NULL;
	}

	ret	= cfg_rdTree(ctxt,NULL,0);
	if	(ctxt->buf)
		free(ctxt->buf);
	if	(ret == NULL && ctxt->uncompr && ctxt->lguncompr)
	{
		char	*uncpr	= ctxt->uncompr;
		int32	lgcpr	= ctxt->lguncompr;

		memset	(&stctxt,0,sizeof(stctxt));
		ctxt->uncompr	= uncpr;
		ctxt->lguncompr	= lgcpr;
		CfgBuf	= bufMem;
		CfgFile	= -1;
		CfgMem	= uncpr ;
		CfgLgMem= lgcpr ;
		CfgPos	= 0;
		if(!CFG_FILBUF())
		{
			free	(uncpr);
			XOERR(	"root empty.", 0);
			return NULL;
		}
		ret	= cfg_rdTree(ctxt,NULL,0);
		if	(ctxt->buf)
			free(ctxt->buf);
		free	(uncpr);
	}
	return	ret;
}


/*
 *	charge des objets de donnees depuis un fichier
 */

/*DEF*/	void *__XoLoad(int32 file)
{
	void	*ret;
	char	bufDisk[CFG_TAILLE_BUF];	/* buffer de lecture disque */
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));
	CfgBuf	= bufDisk;
	CfgFile	= file;
	if(!CFG_FILBUF())
	{
		XOERR(	"root empty.", 0);
		return NULL;
	}

	ret	= cfg_rdTree(ctxt,NULL,0);
	if	(ctxt->buf)
		free(ctxt->buf);
	if	(ret == NULL && ctxt->uncompr && ctxt->lguncompr)
	{
		char	*uncpr	= ctxt->uncompr;
		int32	lgcpr	= ctxt->lguncompr;

//printf	("restart XoLoad with XoLoadMem() %d\n",ctxt->lguncompr);
		memset	(&stctxt,0,sizeof(stctxt));
		ctxt->uncompr	= uncpr;
		ctxt->lguncompr	= lgcpr;
		CfgBuf	= bufDisk;
		CfgFile	= -1;
		CfgMem	= uncpr ;
		CfgLgMem= lgcpr ;
		CfgPos	= 0;
		if(!CFG_FILBUF())
		{
			free	(uncpr);
			XOERR(	"root empty.", 0);
			return NULL;
		}
		ret	= cfg_rdTree(ctxt,NULL,0);
		if	(ctxt->buf)
			free(ctxt->buf);
		free	(uncpr);
	}
	return	ret;
}


/*
 *	retourne un objet Xocfg
 */

/*DEF*/	void	*__XoLoadRef(int32 fin)
     	     			/* ou char *memoire */
{
	int32	ret ;
	int32	pos ;
	int32	lg ;
	void	*ref ;		/*	XoCfg		*/
	void	*or ;		/*	XoObjRef	*/

	ref	= XoLoad(fin) ;
	if	( !ref )
	{
		XOERR("XoLoad() failed",0) ;
		return	NULL ;
	}

	pos	= XoFirstAttr	(ref,XoCfgListObjRef) ;
	while	( pos > 0 )
	{
		or	= XoGetAttr	(ref,XoCfgListObjRef,&lg) ;
		if	( !or )
		{
			XOERR("",0) ;
			return	NULL ;
		}
		ret	= XoAddObjRef		(or) ;
		if	( ret < 0 )
		{
			XOERR("",ret) ;
			return	NULL ;
		}
		pos	= XoNextAttr	(ref,XoCfgListObjRef) ;
	}
	if	( pos < 0 )
	{
		XOERR("",pos) ;
		return	NULL ;
	}
	return	ref ;
}



/*DEF*/	void *XoLoadMemExtDict(char *buf, int32 size, char *dico, int32 szDico)
{
	void	*ret;

	ret = __XoLoadMemExtDict(buf,size,dico,szDico);
	return	ret;
}

/*DEF*/	void *XoLoadMem(char *buf, int32 size)
{
	void	*ret;

	ret = __XoLoadMem(buf,size);
	return	ret;
}

/*DEF*/	void *XoLoad(int32 file)
{
	void	*ret;

	ret = __XoLoad(file);
	return	ret;
}

/*DEF*/	void	*XoLoadRef(int32 fin)
{
	void	*ret;

	ret = __XoLoadRef(fin);
	return	ret;
}
