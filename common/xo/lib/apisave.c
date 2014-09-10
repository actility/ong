
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


/*
 *	le format de sauvegarde est le suivant :
 *	 0      4                       8
 *	<magic><version+type+flags><offsetdico><objet racine><lgdico><dico>
 *	                                     |                    ^
 *	                                     |____________________|
 *
 *
 *	magic = FADAFADA
 *	type = 01
 *	flags = 0000
 *	version = 44
 *
 *	apres sauvegarde la position courante se trouve apres le dico
 *	version,offsetdico et lgdico sont sur 4 octets .
 */

#include	<stdio.h>
#include	<stdlib.h>
#ifndef		DOS
#include	<unistd.h>
#endif
#include	<string.h>
#include	<ctype.h>
#include	<memory.h>
#include	<errno.h>
#include	<time.h>
#ifndef		DOS
#include	<sys/time.h>
#endif

#include	"xoapipr.h"
#include	"intstr.h"
#include	"xointpr.h"
#ifdef	RTL_MBR
#include	"mbr/include/mbr.h"
#endif

/*
#define		TRACE_SAUVE
*/

/*
 *	grain de reallocation du dico
 */

#define		GRAIN_DICO	1024
#define		GRAIN_ALLOC	1024


#define		SZBUFOUT	1024

extern	char	*XoDico;	// heap pointer on data dictionary
extern	int32	XoOffDico;	// next offset (ie the useful lenght)

int LZ_Compress( char *in, char *out, unsigned int insize );

static	char	*xo_file = __FILE__ ;


#if	0
static	char	*BuffDico ;	/* le dictionnaire		*/
static	int32 	LgDico ;	/* sa longueur			*/
static	int32 	OffDico ;	/* dernier offset		*/
static	int32 	DepDico ;	/* ou ecrire l'offset du dico	*/

static	char	BufOut[SZBUFOUT] ;
static	int32 	LgBufOut ;
static	int32 	TypeIo ;	/* IO_FILE , IO_SIZE , IO_MEMORY*/
static	char	*Out ;		/* pte sur buffer en sortie SaveMem*/
static	int32 	LgOut ;		/* lg de Out			*/
static	int32 	PosOut ;	/* position courante dans Out	*/
static	char	PadOut[4] ;
#endif

typedef	struct
{
	char	*BuffDico ;
	int32	Flags;
	int32	LgDico ;
	int32	OffDico ;
	int32	DepDico ;

	char	BufOut[SZBUFOUT] ;
	int32	LgBufOut ;
	int32	TypeIo ;
	char	*Out ;
	int32	LgOut ;
	int32	PosOut ;
	int32	LgAlloc;
	char	PadOut[4] ;

}	t_ctxt;

#define		BuffDico	ctxt->BuffDico
#define		Flags		ctxt->Flags
#define		LgDico		ctxt->LgDico
#define		OffDico		ctxt->OffDico
#define		DepDico		ctxt->DepDico
#define		BufOut		ctxt->BufOut
#define		LgBufOut	ctxt->LgBufOut
#define		TypeIo		ctxt->TypeIo
#define		Out		ctxt->Out
#define		LgOut		ctxt->LgOut
#define		PosOut		ctxt->PosOut
#define		LgAlloc		ctxt->LgAlloc
#define		PadOut		ctxt->PadOut




#define		DO_OUT_INT32(ctxt,fd,val)\
	{ int32 n ; \
	n = format_int32((val)) ;\
	do_out_buf((ctxt),(fd),&n,sizeof(int32)) ; }

/*
 *	cette fonction donne les informations necessaires aux
 *	rechargement des attributs
 *
 *	1)	est-ce 1 attribut liste
 *	2)	est-ce 1 buffer ou qque chose qui tient sur 4 octets
 *
 */
int32	xomodesv(t_attribut_ref *par)
{
	register	t_attribut_ref	*ar ;

	ar	= par ;
	if	( IS_A_LIST(ar) )
	{	/* c'est une liste */
		switch	(ar->ar_mode)
		{
			case	LIST_BASIC_ATTR :
				switch(ar->ar_type)
				{
					case	XoInt4B :
					case	XoDate :
					case	XoChar :
					case	XoBool :
						return	CFG_LSTINT ;
					break ;
					default :
						return	CFG_LSTBUF ;
					break ;
				}
			break ;
			case	LIST_OBJ_ATTR :
				return	CFG_LSTOBJ ;
			break ;
		}
		return	-1 ;
	}
	/*
	 * ce n'est pas une liste 
	 */
	switch	(ar->ar_mode)
	{
		case	BASIC_ATTR :
			switch(ar->ar_type)
			{
				case	XoInt4B :
				case	XoDate :
				case	XoChar :
				case	XoBool :
					return	CFG_INT ;
				break ;
				default :
					return	CFG_BUF ;
				break ;
			}
		break ;
		case	OBJ_ATTR :
			return	CFG_OBJ ;
		break ;
	}
	return	-1 ;
}

/*
 *	formatte un entier sous une forme non dependant machine
 *	le format choisi est de type PC
 */

static	int32	format_int32(register uint32 nombre)
{
	extern	int32	XolittleBig ;

#if	0
	if(littleBig == 0)
	{
		char buf[4];
		char *pt = &buf[0];

		buf[0] = 1;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;

		if(*(int32 *) pt == 1)
		{
			littleBig = 1;
		}
		else
		{
			littleBig = 2;
		}
	}
#endif
	if(XolittleBig == 2)
	{	/* format RS 6000 */
		nombre =	(nombre >> 24) | (nombre << 24) |
				((nombre >> 8) & (uint32) 0xFF00) |
				((nombre << 8) & (uint32) 0xFF0000);
	}

	return	nombre ;
}

static	int32	do_write(t_ctxt *ctxt,int32 fout,char *data,int32 lg)
{
/*
printf	("lg to write %ld adr %ld\n",(long)lg,(long)do_lseek((int32)fout,0L,1)) ;
*/
	switch	(TypeIo)
	{
		case	IO_FILE :
			return	( write((int)fout,data,(unsigned int)lg) ) ;
		break ;
		case	IO_SIZE :
			LgOut	+= lg ;
			PosOut	= LgOut ;
			return	lg ;
		break ;
		case	IO_MEMORY :
			memcpy	(Out+PosOut,data,(size_t)lg) ;
			PosOut	+= lg ;
			LgOut	+= lg ;
			return	lg ;
		break ;
		case	IO_ALLOC :
			while	(LgOut + lg > LgAlloc)
			{
				char	*pt ;
//printf("realloc memory lgout=%d lg=%d lgalloc=%d\n",LgOut,lg,LgAlloc);
				pt = (char *)realloc(Out,
						(size_t)(LgAlloc+GRAIN_ALLOC)) ;
				if	( !pt )
				{
					XOERR("realloc mem",LgAlloc+GRAIN_ALLOC) ;
					return	-500 ;
				}
				Out	= pt ;
				LgAlloc	+= GRAIN_ALLOC ;
			}
			memcpy	(Out+PosOut,data,(size_t)lg) ;
			PosOut	+= lg ;
			LgOut	+= lg ;
			return	lg ;
		break;
		default :
			XOERR("TypeIo ???",TypeIo) ;
		break ;
	}

	return	-1 ;
}

/*
 *	les sorties sur disque sont bufferisees
 *	force l'ecriture
 */

static	int32	flush_out_buf(t_ctxt *ctxt,int32 out)
{
	if	( LgBufOut <= 0 )
		return	0 ;
	if	( do_write(ctxt,(int32)out,BufOut,LgBufOut) != LgBufOut )
	{
		XOERR("do_write errno  : ",errno) ;
		return -1 ;
	}
/*
fprintf(stderr,"xo save do_write() %ld\n",(long)LgBufOut) ;
fflush(stderr) ;
*/
	LgBufOut	= 0 ;
	return	1 ;
}

static	int32	do_lseek(t_ctxt *ctxt,int32 fout,int32 off,int32 where)
{
	switch	(TypeIo)
	{
		case	IO_FILE :
			return	( lseek((int)fout,(long)off,(int)where) ) ;
		break ;
		case	IO_SIZE :
		case	IO_MEMORY :
		case	IO_ALLOC :
			switch	(where)
			{
				case	0 :	/* positionnement */
					PosOut	= off ;
					return	PosOut ;
				break ;
				case	1 :	/* posit. courante */
					return	PosOut ;
				break ;
				default:
					XOERR("Type lseek ???",where) ;
				break ;
			}
		break ;
		default :
			XOERR("TypeIo ???",TypeIo) ;
		break ;
	}

	return	0 ;
}

/*
 *	ecriture bufferisee sur fichier
 */

static	int32	write_out(t_ctxt *ctxt,int32 out, void *val, int32 lg)
{
	if	( LgBufOut + lg > SZBUFOUT )
	{
		if	( flush_out_buf(ctxt,out) < 0 )
		{
			return	-1 ;
		}
	}
	if	( LgBufOut + lg > SZBUFOUT )
	{
		if	( do_write(ctxt,(int32)out,val,lg) != lg )
		{
			XOERR("do_write errno  : ",errno) ;
			return -1 ;
		}
/*
fprintf(stderr,"xo save do_write() %ld\n",(long)lg) ;
fflush(stderr) ;
*/
	}
	else
	{
		memcpy	(BufOut+LgBufOut,val,(size_t)lg) ;
		LgBufOut	+= lg ;
	}
	return	lg ;
}


static	int32	do_out_buf(t_ctxt *ctxt,int32 out, void *val, int32 lg)
{
	int32	lgPad ;
/*
fprintf(stderr," write out %ld %ld\n",(long)out,(long)lg) ;
fflush(stderr) ;
*/

	if	( lg <= 0 )
	{
		XOERR("write <0  : ",lg) ;
		return -1;
	}

	lgPad	= ((lg+3)/4)*4 - lg ;

	if	( write_out(ctxt,(int32)out,val,lg) != lg )
	{
		XOERR("write_out errno  : ",errno) ;
		return -1;
	}
	if	( lgPad )
	{
		if	( write_out(ctxt,(int32)out,PadOut,lgPad) != lgPad )
		{
			XOERR("write_out errno  : ",errno) ;
			return -1;
		}
	}
	return	lg ;
}

/*
 *	verifie qu'une chaine n'est pas deja dans le dictionnaire
 */

static	int32	FindInDico(t_ctxt *ctxt,char *s)
{
	char	*pt ;
	int32	pos ;

	pt	= BuffDico ;
	pos	= 0 ;
	while	( pt && pos < OffDico )
	{
		if	( strcmp(pt,s) == 0 )
		{
			return	pos ;
		}
		pos	+= strlen(pt) + 1 ;
		pt	= BuffDico + pos ;
	}

	return	-1 ;
}

/*
 *	ajoute une chaine dans le dictionnaire et retourne son offset
 */

static	int32	AddInDico(t_ctxt *ctxt,char *s)
{
	int32	lgs ;
	int32	offret ;
	int32	flags	= Flags;

	if	((flags & XO_SHARED_DICO) == XO_SHARED_DICO)
	{
		offret	= XoFindInSharedDico(s);
//printf("%08x '%s'\n",offret,s);
		if	(offret < 0)
			offret	= 0;
		return	offret;
	}

	lgs	= strlen(s) + 1 ;
	if	( !BuffDico )
	{
		BuffDico	= (char *)malloc(GRAIN_DICO) ;
		if	( !BuffDico )
		{
			XOERR("alloc dico",GRAIN_DICO) ;
			return	-1 ;
		}
		LgDico	= GRAIN_DICO ;
	}

	offret	= FindInDico(ctxt,s) ;
	if	( offret != -1 )
	{	/*	chaine deja presente	*/
		return	offret ;
	}
/*
fprintf(stderr,"AddInDico(%s)\n",s) ;
fflush(stderr) ;
*/

	while	( OffDico + lgs >= LgDico )
	{
		char	*pt ;

		pt = (char *)realloc(BuffDico,(size_t)(LgDico+GRAIN_DICO)) ;
		if	( !pt )
		{
			XOERR("realloc dico",LgDico+GRAIN_DICO) ;
			return	-1 ;
		}
		BuffDico	= pt ;
		LgDico	+= GRAIN_DICO ;
	}

	memcpy		(BuffDico+OffDico,s,(size_t)lgs) ;
	offret		= OffDico ;
	OffDico	+= lgs ;

#ifdef	TRACE_SAUVE
fprintf(stderr	,"ajout de [%s] off = %ld\n",s,(long)offret) ;
#endif

	return		offret ;
}

/*
 *	initialise le dictionnaire
 *	sauvegarde l'offset dans fd ou il faudra ecrire l'emplacement
 *	du dico
 */

static	uint32	DoHeader(uint32 version,uint32 format,uint32 flags)
{
	version	= (version << 24) | (format << 16) | (0x0000FFFF & flags);
	return	version;
}

static	int32	InitDico(t_ctxt *ctxt,int32 fout)
{
	int32	zero	= 0 ;
	uint32	version ;	// 8bits
	uint32	format ;	// 8bits
	uint32	flags = Flags;	// 16bits

	if	((flags & XO_SHARED_DICO) == XO_SHARED_DICO)
	{
		if	(XoDico == NULL || XoOffDico <= 0)
		{
			XOERR("no shared dico",0);
			return	-1;
		}
	}
	if	((flags & XO_COMPRESS_0) == XO_COMPRESS_0)
	{
		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
		{
			XOERR("compress 0 require shared dico",0);
			return	-2;
		}
	}
	if	((flags & XO_COMPRESS_LZ77) == XO_COMPRESS_LZ77)
	{
		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
		{
			XOERR("compress lz77 require shared dico",0);
			return	-2;
		}
	}

	LgBufOut = 0 ;
	DO_OUT_INT32(ctxt,fout,XO_FMAGIC) ;

	version	= XO_VERSION ;
	format	= XO_FMT_STD_BIN;
#if	0
	version	= (version << 24) | (format << 16) | (0x0000FFFF & flags);
#endif
	version	= DoHeader(version,format,flags);
	DO_OUT_INT32(ctxt,fout,version) ;
	flush_out_buf(ctxt,fout) ;

	DepDico	= do_lseek(ctxt,fout,0L,1) ;	/*	position courante */
	DO_OUT_INT32(ctxt,fout,zero) ;
	flush_out_buf(ctxt,fout) ;
/*
fprintf(stderr,"InitDico() DepDico=%ld\n",(long)DepDico); ;
fflush(stderr) ;
*/
	LgDico	= 0 ;
	OffDico	= 0 ;
	if	( BuffDico )
	{
		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
			free	(BuffDico) ;
		BuffDico = NULL ;
	}


	if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
		AddInDico(ctxt,"*********************") ;
	return	0 ;
}

/*
 *	fin du dictionnaire a sauver
 */

static	int32	EndDico(t_ctxt *ctxt,int32 fout)
{
	int32	startdico ;
	int32	enddico ;
	uint32	flags = Flags;	// 16bits


	flush_out_buf(ctxt,fout) ;
	startdico	= do_lseek(ctxt,fout,0L,1)  ;

	if	((flags & XO_SHARED_DICO) == XO_SHARED_DICO)
	{
		startdico	= 0;
		OffDico		= 0;
	}

	DO_OUT_INT32	(ctxt,fout,OffDico) ;		/* lg du dico 	*/
	flush_out_buf	(ctxt,fout) ;
	if	( OffDico > 0 )
	{
		do_out_buf(ctxt,fout,BuffDico,OffDico) ;/* le dico	*/
		flush_out_buf(ctxt,fout) ;
	}
	enddico		= do_lseek(ctxt,fout,0L,1)  ;

	do_lseek	(ctxt,fout,DepDico,0) ;
	DO_OUT_INT32	(ctxt,fout,startdico) ;
	flush_out_buf	(ctxt,fout) ;
	if	(TypeIo == IO_SIZE || TypeIo == IO_MEMORY || TypeIo == IO_ALLOC)
	{
		LgOut	-= sizeof(int32) ;
	}

	do_lseek	(ctxt,fout,enddico,0) ;		/* fin fichier	*/
/*
fprintf(stderr,"EndDico() startdico=%ld LgDico=%ld end=%ld\n",
	(long)startdico,(long)OffDico,(long)enddico) ;
fflush(stderr) ;
*/
	if (BuffDico)
		if	((flags & XO_SHARED_DICO) != XO_SHARED_DICO)
			free(BuffDico);
	BuffDico = NULL;
	return	0 ;
}


/*
 *	verifie qu'un attribut est different de sa valeur par default
 *	retourne 1 si les valeurs sont differentes sinon 0
 *
 *	pour les XoBool si la valeur par defaut n'est pas precisee
 *	on considere que c'est '0'
 *
 *	les XoBuffer n'ont pas de valeur par defaut
 */

int32	xo_diff_default(u_val *v, t_attribut_ref *ar)
{
	char	*buf ;
	int32	defbool ;

#if	0
	if	(strcmp(ar->ar_name,"xmlns") == 0)
	{
		printf("default '%s'\n",ar->ar_default);
		printf("value '%s'\n",v->v_dyn);
	}
#endif

	if	( ar->ar_type == XoBool )
	{
		if	( ar->ar_default )
			defbool	= (int32)(ar->ar_default[0]  - '0') ;
		else
			defbool	= 0 ;
		if	( v->v_l != defbool )
			return	1 ;
		return	0 ;
	}

	if	( !ar->ar_default )
	{
		return	1 ;
	}

	switch	( ar->ar_type )
	{
#if	0
	case	XoVoid :
		return	1 ;
	break ;
#endif
	case	XoBuffer :
		return	1 ;
	break ;
	case	XoString :
		if	( IS_NICE_STR(ar) )
			buf	= v->v_str ;
		else
			buf	= v->v_dyn ;
		return	( strcmp(ar->ar_default,buf) != 0 ) ;
	break ;
	case	XoInt4B :
		return	( xoatoi(ar->ar_default) != v->v_l) ;
	break ;
	case	XoChar :
		return	( (unsigned char)ar->ar_default[0] != v->v_c) ;
	break ;
	default :
	break ;
	}
	return	0 ;
}

/*
 *	sauvegarde d'une value ayant comme attr ref ar
 *	cette ar est dans or
 */

static	int32	aff_value(t_ctxt *ctxt,u_val *v, t_attribut_ref *ar, int32 fout, int32 inliste)
{
	char	*val ;
	int32	lg ;

#if	0
	if (strcmp(ar->ar_name,"m2m:path")==0)
	{
		if (!strcmp(v->v_dyn,"retargeting1"))
		{
		return	0;
		}
		if (!strcmp(v->v_dyn,"retargeting2"))
		{
		return	0;
		}
		if (!strcmp(v->v_dyn,"retargeting3"))
		{
		return	0;
		}
	}
	if (strcmp(ar->ar_name,"m2m:accessRightID")==0)
	{
		if (!strcmp(v->v_dyn,"/m2m/accessRights/Locadmin_AR"))
		{
		return	0;
		}
		if (!strcmp(v->v_dyn,"/m2m/accessRights/Locadmin_AR2"))
		{
		return	0;
		}
		if (!strcmp(v->v_dyn,"/m2m/accessRights/Locadmin_AR3"))
		{
		return	0;
		}
	}
	if (inliste && strcmp(ar->ar_name,"m2m:searchString")==0)
	{
		if (!strcmp(v->v_dyn,"ETSI.ObjectSemantic/OASIS.OBIX_1_1"))
		{
		return	0;
		}
		if (!strcmp(v->v_dyn,"ETSI.ObjectType/ETSI.AN_NODE"))
		{
		return	0;
		}
	}
#endif

	switch	( ar->ar_type )
	{
#if	0
	case	XoVoid :
	break ;
#endif
	case	XoString :
	case	XoBuffer :
		if	( ar->ar_type == XoString )
		{
			if	( IS_NICE_STR(ar) )
				val	= v->v_str ;
			else
				val	= v->v_dyn ;
			lg	= strlen(val)+1 ;
		}
		else
		{
			val	= v->v_dyn ;
			lg	= v->v_lg ;
		}
		if	( !inliste )
		{
			int32	dep ;
			dep	= AddInDico(ctxt,ar->ar_name) ;
			DO_OUT_INT32(ctxt,fout,dep) ;
			DO_OUT_INT32(ctxt,fout,ar->ar_mode_sv) ;
		}
		if	((Flags & XO_SHARED_DICO) == XO_SHARED_DICO
			&& ar->ar_type == XoString && IS_SHDICO(ar))
		{
			int32	offshdico	= -1;
			int32	offprefixe	= 0;
			offshdico	= 
				XoFindInSharedDicoPrefixed(val,&offprefixe);
			if	(offshdico < 0)
			{
				offshdico	= 0;
			}
//printf("xxxx find in shdico '%s' at off=%d\n",val,offshdico);
			if	(offshdico == 0)
			{// declared shdico for data but not found in dico
				if	(offprefixe > 0)
				{// but prefixe found, just save difference
					char	*pref;
					offshdico	= offprefixe;
					pref	= XoDico + offprefixe;
					val	= val + strlen(pref);
//printf("xxxx pref='%s' val='%s'\n",pref,val);
					lg	= strlen(val)+1;
				}
				else
				{
//					XOERR(val,0);
//					fprintf(stderr,"%s\n",val);
				}

				DO_OUT_INT32(ctxt,fout,offshdico) ;
				DO_OUT_INT32(ctxt,fout,lg) ;
				do_out_buf(ctxt,fout,val,lg) ;
			}
			else
			{// full match add offset and (lg == 0)
				DO_OUT_INT32(ctxt,fout,offshdico) ;
				DO_OUT_INT32(ctxt,fout,0) ;
//				do_out_buf(ctxt,fout,"",1) ;
			}
		}
		else
		{
			DO_OUT_INT32(ctxt,fout,lg) ;
			do_out_buf(ctxt,fout,val,lg) ;
		}
	break ;
	case	XoBool :
	case	XoDate :
	case	XoInt4B :
		if	( !inliste )
		{
			int32	dep ;
			dep	= AddInDico(ctxt,ar->ar_name) ;
			DO_OUT_INT32(ctxt,fout,dep) ;
			DO_OUT_INT32(ctxt,fout,ar->ar_mode_sv) ;
		}
		DO_OUT_INT32(ctxt,fout,v->v_l) ;
	break ;
	case	XoChar :
		if	( !inliste )
		{
			int32	dep ;
			dep	= AddInDico(ctxt,ar->ar_name) ;
			DO_OUT_INT32(ctxt,fout,dep) ;
			DO_OUT_INT32(ctxt,fout,ar->ar_mode_sv) ;
		}
		DO_OUT_INT32(ctxt,fout,(int32)v->v_c) ;
	break ;
	default :
		XOERR("error type attr",0) ;
		return -1 ;
	break ;
	}
	return	0 ;
}

static	int32	aff_value_unset(t_ctxt *ctxt,t_attribut_ref *ar, int32 fout)
{
	char	*val ;
	int32	lg ;
	int32	dep ;
	int32	mode ;

	mode	= ar->ar_mode_sv ;
	switch	(mode)
	{
		case	CFG_INT :
			mode	= CFG_INT_UNSET ;
		break ;
		case	CFG_BUF :
			mode	= CFG_BUF_UNSET ;
		break ;
	}

	switch	( ar->ar_type )
	{
	case	XoBuffer :
	case	XoString :
	case	XoBool :
	case	XoDate :
	case	XoInt4B :
	case	XoChar :
		dep	= AddInDico(ctxt,ar->ar_name) ;
		DO_OUT_INT32(ctxt,fout,dep) ;
		DO_OUT_INT32(ctxt,fout,mode) ;
	break ;
	default :
		XOERR("error type attr",0) ;
		return -1 ;
	break ;
	}

	switch	( ar->ar_type )
	{
	case	XoBuffer :
	case	XoString :
		val	= "" ;
		lg	= strlen(val)+1 ;
		DO_OUT_INT32(ctxt,fout,lg) ;
		do_out_buf(ctxt,fout,val,lg) ;
	break ;
	case	XoBool :
	case	XoDate :
	case	XoInt4B :
		DO_OUT_INT32(ctxt,fout,(int32)0) ;
	break ;
	case	XoChar :
		DO_OUT_INT32(ctxt,fout,(int32)'\0') ;
	break ;
	default :
		XOERR("error type attr",0) ;
		return -1 ;
	break ;
	}
	return	0 ;
}



/*
 *	sauvegarde d'une liste d'attributs basic
 */

static	int32	aff_list_attr(t_ctxt *ctxt,t_attribut_ref *arpere, t_list_info *li, int32 fout)
{
	t_list		*l ;
	t_attribut_ref	*ar ;
	int32		dep ;
	int32		nb ;

	if	( !li )
	{
		XOERR("",0) ;
		return(-1000) ;
	}

	ar	= XoGetRefAttr(li->li_num) ;
	if	( !ar )
	{
		XOERR("",0) ;
		return(-1001) ;				/* !!! */
	}
	if	( li->li_nb <= 0 )
		return	1 ;
	nb	= 0 ;
	dep	= AddInDico(ctxt,arpere->ar_name) ;
	DO_OUT_INT32(ctxt,fout,dep) ;
	DO_OUT_INT32(ctxt,fout,arpere->ar_mode_sv) ;
	DO_OUT_INT32(ctxt,fout,li->li_nb) ;
	l	= li->li_first ;
	while	(l)
	{
		nb++ ;
		aff_value	(ctxt,&(l->l_val),ar,fout,1) ;
		l		= l->l_next ;
	}
	if	( nb != li->li_nb )
	{
		XOERR("count list error",0) ;
		return(-1002) ;
	}
	return	1 ;
}


static	int32	
	xosave(t_ctxt *ctxt, void *obj,int32 fout,t_attribut_ref *indattrpere,int32 inliste);

/*
 *	sauvegarde d'une liste d'attributs objets
 */

static	int32	aff_list_obj(t_ctxt *ctxt,t_attribut_ref *arpere, t_list_info *li, int32 fout)
{
	t_list		*l ;
	t_attribut_ref	*ar ;
	int32		ret ;
	int32		dep ;
	int32		nb ;

	ret	= 1 ;
	if	( !li )
	{
		XOERR("",0) ;
		return(-2000) ;
	}

	ar	= XoGetRefAttr(li->li_num) ;
	if	( !ar )
	{
		XOERR("",0) ;
		return(-2001) ;				/* !!! */
	}
	if	( li->li_nb <= 0 )
		return	1 ;
	nb	= 0 ;
	dep	= AddInDico(ctxt,arpere->ar_name) ;
	DO_OUT_INT32(ctxt,fout,dep) ;
	DO_OUT_INT32(ctxt,fout,arpere->ar_mode_sv) ;
	DO_OUT_INT32(ctxt,fout,li->li_nb) ;
	l	= li->li_first ;
	while	(l && ret > 0 )
	{
		nb++ ;
		ret	= xosave(ctxt,l->l_obj,fout,ar,1) ;
		l	= l->l_next ;
	}
	if	( ret < 0 )
		XOERR("",ret) ;
	if	( nb != li->li_nb )
	{
		char	tmp[128] ;

		sprintf(tmp,"count list error nb=%d li=%d",nb,li->li_nb) ;
		XOERR(tmp,0) ;
		return(-2002) ;
	}
	return	ret ;
}





/*
 *	sauvegarde d'une occurence d'objet reccursif
 */

static	int32	xosave(t_ctxt *ctxt,void *obj, int32 fout, t_attribut_ref *indattrpere, int32 inliste)
{
	t_obj		*o ;
	t_objet_ref	*or ;

	u_attr		*a ;
	t_attribut_ref	*ar ;

	int32		mode ;
	int32		nb ;
	int32		i ;
	int32		ret ;
	int32		dep ;
/*
fprintf(stderr,"obj=%lx fout=%ld pere=%ld liste=%ld\n",
(long)obj,(long)fout,(long)indattrpere,(long)inliste) ;
fflush(stderr) ;
*/

	ret	= 1 ;
	o	= (t_obj *)obj ;

	if	( !o )
	{
		XOERR("null",0) ;
		return	-11 ;
	}

	if	( o->o_magic != O_MAGIC )
	{
		XOERR("magic",0) ;
		return	-1 ;
	}

	or	= XoGetRefObj(o->o_num) ;
	if	( !or )
	{
		XOERR("ref",o->o_num) ;
		return	-2 ;
	}

	nb	= or->or_nb_attr ;
	/*
	 *	un objet est tjrs sauve :
	 */
	if	( indattrpere && !inliste )
	{
		/*
		 *	c'est un objet fils qui n'est pas dans une liste
		 */
		dep	= AddInDico(ctxt,indattrpere->ar_name) ;
		DO_OUT_INT32(ctxt,fout,dep) ;
		DO_OUT_INT32(ctxt,fout,indattrpere->ar_mode_sv) ;
	}
/*
fprintf(stderr,"or_name [%s]\n",or->or_name) ;
fflush(stderr) ;
*/
	dep	= AddInDico(ctxt,or->or_name) ;
	DO_OUT_INT32(ctxt,fout,dep) ;

	ret	= 1 ;
	for	( i = 0 ; i < nb && ret > 0 ; i++ )
	{
		a	= &(o->o_tab_attr[i]) ;
		ar	= &(or->or_tab_attr[i]) ;
		if	( IS_NO_SAVE(ar) /* || ar->ar_type == XoVoid */ )
			continue ;
		if	( ar->ar_num == XoObjRefNum 
			|| ar->ar_num == XoAttrRefNum 
			|| ar->ar_num == XoAttrRefType )
				continue ;
		mode	= XoModeAttr(ar->ar_num) ;
		switch	(mode)
		{
 		case	BASIC_ATTR :
			if	( a->a_ba )
			{
				t_basic_attr	*ba ;

				ba	= a->a_ba ;


		if	(O_IS_OBIX(o) && !strcmp(ar->ar_name,XOML_BALISE_ATTR)
			&& !strcmp(ba->ba_val.v_sdyn.d_pt,or->or_name))
		{
#ifdef	TRACE_SAUVE
printf("##TEST do not save name$ : '%s' in '%s'\n",ba->ba_val.v_sdyn.d_pt,or->or_name);
#endif
			continue;
		}
				if	(xo_diff_default(&(ba->ba_val),ar))
				{
#ifdef	TRACE_SAUVE
fprintf(stderr,"attr '%s' valorise != valeur defaut => SAUVE\n",ar->ar_name) ;
fflush(stderr) ;
#endif
					aff_value(ctxt,&(ba->ba_val),ar,fout,0) ;
				}
				else
				{
#ifdef	TRACE_SAUVE
fprintf(stderr,"attr '%s' valorise a valeur defaut => NON SAUVE\n",ar->ar_name);
fflush(stderr) ;
#endif
				}
			}
			else
			{	/* attribut non valorise */
				if	( IS_SAVE_UNSET(ar) )
				{	/* mais flag , on le sauve quand meme */
#ifdef	TRACE_SAUVE
fprintf(stderr,"attr '%s' non valorise mais saveunset => SAUVE\n",ar->ar_name) ;
fflush(stderr) ;
#endif
					aff_value_unset(ctxt,ar,fout) ;
				}
				else
				{
#ifdef	TRACE_SAUVE
fprintf(stderr,"attr '%s' non valorise => NON SAUVE\n",ar->ar_name) ;
fflush(stderr) ;
#endif
				}
			}
		break ;
		case	OBJ_ATTR :
			if	( a->a_ob )
			{
				ret= xosave(ctxt,a->a_ob,fout,ar,0) ;
			}
		break ;
		case	LIST_BASIC_ATTR :
			if	( a->a_li )
			{
				ret = aff_list_attr(ctxt,ar,a->a_li,fout);
			}
		break ;
		case	LIST_OBJ_ATTR :
			if	( a->a_li )
			{
				ret= aff_list_obj(ctxt,ar,a->a_li,fout);
			}
		break ;
		default :
		break ;
		}
	}
	/*
	 *	marque la fin de l'objet
	 */
	DO_OUT_INT32(ctxt,fout,0) ;

	if	( ret < 0 )
		XOERR("",ret) ;
	return	ret ;
}



/*
 *	sauvegarde d'une occurence d'objet reccursif sur fichier
 */

/*DEF*/	int32	__XoSave(void *obj, int32 fout, int32 flags)
{
	int32	ret ;
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));
	Flags	= flags;
	TypeIo	= IO_FILE ;
/*
fprintf(stderr,"obj=%lx fout=%ld\n",(long)obj,(long)fout) ;
fflush(stderr) ;
*/
	if	((ret=InitDico(ctxt,fout)) < 0)
	{
		XOERR("binary save initialization error",ret) ;
		return	ret ;
	}
	ret	= xosave(ctxt,obj,fout,0,0) ;
	if	( ret < 0 )
	{
		free(BuffDico);
		XOERR("",ret) ;
		return	ret ;
	}
	EndDico(ctxt,fout) ;
	return	ret ;
}



/*
 *	sauvegarde d'une occurence d'objet reccursif en memoire
 *	fout contient suffisament d'espace
 *	TODO reecrire XoSaveMem avec XoSaveMemAlloc
 */

/*DEF*/	int32	__XoSaveMem(void *obj, char *fout,int32 flags)
{
	int32	ret ;
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));
	Flags	= flags;
	TypeIo	= IO_MEMORY ;
	Out	= fout ;
	LgOut	= 0 ;
	PosOut	= 0 ;
	if	((ret=InitDico(ctxt,-1)) < 0)
	{
		XOERR("binary save initialization error",ret) ;
		return	ret ;
	}
	ret	= xosave(ctxt,obj,-1,0,0) ;
	if	( ret < 0 )
	{
		free(BuffDico);
		XOERR("",ret) ;
		return	ret ;
	}
	EndDico(ctxt,-1) ;
	return	LgOut ;
}

/*
 *	sauvegarde d'une occurence d'objet reccursif en memoire
 *	l'allocation de la memoire se fait au fur et a mesure
 *	elle devra etre liberee par l'appelant
 *	la memoire peut etre preallouee par l'appelant, elle sera reallouee
 *	si necessaire
 */

/*DEF*/	int32	__XoSaveMemAlloc(void *obj, char **fout,int32 *lgprealloc,int32 flags)
{
	int32	ret ;
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));
	Flags	= flags;
	TypeIo	= IO_ALLOC ;
	if (*fout == NULL)
	{
		Out	= (char *)malloc(GRAIN_ALLOC);
		if	(!Out)
		{
			XOERR("alloc mem",GRAIN_ALLOC);
			return	-500;
		}
		*lgprealloc	= GRAIN_ALLOC;
		LgAlloc		= GRAIN_ALLOC;
	}
	else
	{
		Out	= *fout;
		LgAlloc	= *lgprealloc;
	}
	LgOut	= 0 ;
	PosOut	= 0 ;
	if	((ret=InitDico(ctxt,-1)) < 0)
	{
		XOERR("binary save initialization error",ret) ;
		return	ret ;
	}
	ret	= xosave(ctxt,obj,-1,0,0) ;
	if	( ret < 0 )
	{
		*fout		= Out;
		*lgprealloc	= LgAlloc;
		free(BuffDico);
		XOERR("",ret) ;
		return	ret ;
	}
	EndDico(ctxt,-1) ;
	*fout		= Out;
	*lgprealloc	= LgAlloc;
	if	( 	((flags & XO_COMPRESS_0) == XO_COMPRESS_0
			|| (flags & XO_COMPRESS_LZ77) == XO_COMPRESS_LZ77)
		&& (flags & XO_SHARED_DICO) == XO_SHARED_DICO)
	{
		char	*compr;
		int32	nbbyt	= 0;
		int32	lg	= ((3*LgOut+10))/2;	// for lz77
		int32	hdsz	= XO_HEADER_SIZE;

		compr	= (char *)malloc(lg);
		if	(!compr)
		{
			XOERR("cannot alloc compr0",lg) ;
			return	-10 ;
		}
		memcpy	(compr,(*fout)+hdsz,LgOut-hdsz);

		if	((flags & XO_COMPRESS_0) == XO_COMPRESS_0)
			nbbyt	= XoMemDeflate(compr,LgOut-hdsz,(*fout)+hdsz);

		if	((flags & XO_COMPRESS_LZ77) == XO_COMPRESS_LZ77)
			nbbyt	= LZ_Compress(compr,(*fout)+hdsz,LgOut-hdsz);

		if	(nbbyt == 0 || nbbyt > LgOut-hdsz)
		{
			uint32	version;
			uint32	format;

			//  compress failure restore original buffer, and 
			//  suppress XO_COMPRESS_ flags
			memcpy	((*fout)+hdsz,compr,LgOut-hdsz);
			free	(compr);
			XOERR("cannot compress szcompr > szorig",LgOut-hdsz) ;

			flags	= flags & ~XO_COMPRESS_0;
			flags	= flags & ~XO_COMPRESS_LZ77;
			version	= XO_VERSION ;
			format	= XO_FMT_STD_BIN;
			version	= DoHeader(version,format,flags);
			do_lseek	(ctxt,-1,4,0) ;
			DO_OUT_INT32	(ctxt,-1,version) ;
			flush_out_buf	(ctxt,-1) ;
			return	LgOut;
		}

//printf("compression lgin=%d nbbyt=%d left=%d\n",LgOut-hdsz,nbbyt,(*fout)[hdsz]);

		do_lseek	(ctxt,-1,DepDico,0) ;
		DO_OUT_INT32	(ctxt,-1,LgOut-hdsz) ;
		flush_out_buf	(ctxt,-1) ;

		free	(compr);
		return	hdsz + nbbyt;

	}
	return	LgOut ;
}


/*
 *	retourne la taille d'une sauvegarde d'une occurence d'objet reccursif
 */

/*DEF*/	int32	__XoLgSave(void *obj,int32 flags)
{
	int32	ret ;
	t_ctxt	stctxt, *ctxt = &stctxt;

	memset	(&stctxt,0,sizeof(stctxt));
	Flags	= flags;
	TypeIo	= IO_SIZE ;
	LgOut	= 0 ;
	PosOut	= 0 ;
	if	((ret=InitDico(ctxt,-1)) < 0)
	{
		XOERR("binary save initialization error",ret) ;
		return	ret ;
	}
	ret	= xosave(ctxt,obj,-1,0,0) ;
	if	( ret < 0 )
	{
		free(BuffDico);
		XOERR("",ret) ;
		return	ret ;
	}
	EndDico(ctxt,-1) ;
	return	LgOut ;
}

// no flags

/*DEF*/	int32	XoSave(void *obj, int32 fout)
{
	int32	ret;

	ret = __XoSave(obj,fout,0);
	return	ret;
}

/*DEF*/	int32	XoSaveMem(void *obj, char *fout)
{
	int32	ret;

	ret = __XoSaveMem(obj,fout,0);
	return	ret;
}

/*DEF*/	int32	XoLgSave(void *obj)
{
	int32	ret;

	ret = __XoLgSave(obj,0);
	return	ret;
}

/*DEF*/	int32	XoSaveMemAlloc(void *obj, char **fout,int32 *lgprealloc)
{
	int32	ret;

	ret = __XoSaveMemAlloc(obj,fout,lgprealloc,0);
	return	ret;
}

// flags ==  XO_SHARED_DICO | XO_COMPRESS_0

/*DEF*/	int32	XoSaveFlg(void *obj, int32 fout,int32 flags)
{
	int32	ret;

	ret = __XoSave(obj,fout,flags);
	return	ret;
}

/*DEF*/	int32	XoSaveMemFlg(void *obj, char *fout,int32 flags)
{
	int32	ret;

	ret = __XoSaveMem(obj,fout,flags);
	return	ret;
}

/*DEF*/	int32	XoLgSaveFlg(void *obj,int32 flags)
{
	int32	ret;

	ret = __XoLgSave(obj,flags);
	return	ret;
}

/*DEF*/	int32	XoSaveMemAllocFlg(void *obj, char **fout,int32 *lgprealloc,int32 flags)
{
	int32	ret;

	ret = __XoSaveMemAlloc(obj,fout,lgprealloc,flags);
	return	ret;
}



static int32 cfg_rdInt(int32 *nombre,char *buf)
{
	extern int32	XolittleBig;

	/* lecture d'un int32 */
#if	0
	if(littleBig == 0)
	{
		char buf[4];
		char *pt = &buf[0];

		buf[0] = 1;
		buf[1] = 0;
		buf[2] = 0;
		buf[3] = 0;

		if(*(int32 *) pt == 1)
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
		*nombre = *(int32 *) buf;
	}
	else
	{	/* format RS 6000 */
		register uint32 valLu;

		valLu = *(int32 *) buf;
		*nombre =	(valLu >> 24) | (valLu << 24) |
				((valLu >> 8) & (int32) 0xFF00) |
				((valLu << 8) & (int32) 0xFF0000);
	}
	return(1);
}

/*
 *	retourne l'offset du dico apres sauvegarde
 */

/*DEF*/	int32	XoOffsetDict(char *fout,int32 lg)
{
	int32	offset	= -10;

	if	( !fout || lg < XO_HEADER_SIZE )
	{
		XOERR("",0);
		return	-1;
	}
	cfg_rdInt(&offset,fout+8);

	return	offset;

}

/*
 *	retourne la lg du dico apres sauvegarde
 */

/*DEF*/	int32	XoLgDict(char *fout,int32 lg)
{
	int32	version;
	int32	offset;
	int32	lgd	= -10;

	if	( !fout || lg < XO_HEADER_SIZE )
	{
		XOERR("",0);
		return	-1;
	}
	cfg_rdInt(&version,fout);
	cfg_rdInt(&offset,fout+8);

	if	(offset+4 >= lg)
	{
		XOERR("",0);
		return	-2;
	}

	cfg_rdInt(&lgd,fout+offset);

	return	lgd;

}
