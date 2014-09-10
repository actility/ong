


%{
#include	<stdlib.h>
#include	<string.h>
#include	<stdio.h>
#include	<unistd.h>
#include	<ctype.h>
#include	<fcntl.h>
#include	<sys/types.h>


#include	"xoapipr.h"

#define		LG_MAX		256
#define		ALIAS_MAX	256
#define		OBJ_MAX		2048

void	listeobj();
void	myerr(char *s,char *msg);
void	init_symb();
void	do_all_alias();
int	add_symb(char *name,void *val,int type);
int	is_symbole(char *name,long *lval,char **cval);
int	find_obj(char *name);
int	add_obj(char *name,int root,int exter,int numalias);
int	do_attr(void *or,char *nameobj,int num,void *super);
void	new_attr(void *ar,char *name,char *nametype);
void	new_obj(void *or,char *name,int root,int exter,int from,int alias,int stext);
int	IsExternObj(char *name);


typedef	struct
{
	char	*o_name ;
	int	o_root ;
	int	o_extern ;
	int	o_alias ;
	char	*o_alias_has[ALIAS_MAX] ;
	int	o_nalias;
	char	*o_or ;
}	t_obj ;

t_obj		*tabobj[OBJ_MAX] ;

char		*Hd ;		/*	XoHeader	*/
char		*Ref ;		/*	XoCfg		*/
char		*Or ;		/*	XoObjRef	*/
char		*Ar ;		/*	XoAttrRef	*/

int		NbObjP0	 ;	/* nbre objet passe 0	*/

int		NbObj ;
int		NbAttr ;

int		NumObj ;

int		MaxOcc		= -1 ;
int		MinOcc		= 0 ;
int		WrapHidden	= 0 ;
int		EmbAttribut	= 0 ;
int		Liste		= 0 ;
int		Nosave		= 0 ;
int		Saveunset	= 0 ;
int		LgMax		= -1 ;
int		ShDico		= 0;

int		AutoNum		= 0 ;

int		Composition	= SINGLE_OBJ ;


char		Desc[LG_MAX+10] ;


FILE		*fpointh ;
FILE		*fpointc ;
char		pointh[128] ;
char		pointc[128] ;


char		buff[LG_MAX] ;
char		arg_initial[LG_MAX+1] ;
char		fichier_initial[LG_MAX+1] ;
char		fichier_courant[LG_MAX+1] ;
char		Appli[LG_MAX] ;
char		Appli0[LG_MAX] ;
int		nl = 1 ;	/* numero de ligne */
int		passe = 0 ;	/* numero de passe */
int		verbose  = 0 ;
int		gener  = 0 ;
int		upperonly  = 0 ;


void	yyerror(char *s)
{
fprintf(stderr,"%s <%d> %s\n",fichier_courant,nl,"syntax error\n") ;
unlink	(pointh) ;
unlink	(pointc) ;
exit(1) ;
}

int	base_name(char *path,char **base)
{
char	*dernslash ;

	if ( path == NULL || strlen(path) == 0 )
		{
		return(-1) ;
		}

	dernslash = (char *)strrchr(path,(int)'/') ;
	if ( dernslash == NULL )
		{	/* ex : "file" */
		*base = path ;
		return(1) ;
		}

	dernslash++ ;

	if ( *dernslash == '\0' )
		{	/* ex : "file/" */
		*base = path ;
		return(1) ;
		}

	*base = dernslash ;
	return(1) ;
}

int	IsExternObj(char *name)
{
	int	i ;

	for	( i = 0 ; i < NbObjP0 ; i++ )
	{
		if	( strcmp(tabobj[i]->o_name,name) == 0 )
			return	tabobj[i]->o_extern ;
	}
	return	0 ;
}

int32	xosaveref(int32 fout)
{
	int32	i ;
	t_objet_ref	*or ;
	void	*cfg ;
	void	*oor ;
	int32	nb ;
	int32	ret ;

	extern	int32		XoNbObj ;
	extern	t_objet_ref	**XoTabObj ;
	extern	int32		XoNbObjInt ;
//	extern	t_objet_ref	XoTabObjInt[] ;

	nb	= 0 ;
	for	( i = XoNbObjInt ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )			continue ;
		if	( IsExternObj(or->or_name) ) continue ;
		if	( or->or_num >= BaseUserObj(0) )	/* !!! */
			nb++ ;
	}

	if	( !nb )
	{
		return	-1 ;
	}

	cfg	= XoNew(XoCfg) ;
	if	( !cfg )
	{
		return	-2 ;
	}

	for	( i = XoNbObjInt ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )			continue ;
		if	( IsExternObj(or->or_name) )
		{
			continue ;
		}
	}

	fprintf(fpointc,"int32\tGenRefXo_%s(void)\n{\n",Appli0) ;

	for	( i = XoNbObjInt ; i < XoNbObj ; i++ )
	{
		or	= XoTabObj[i] ;
		if	( !or )			continue ;
		if	( IsExternObj(or->or_name) )
		{
			printf	("'%s' extern obj not save in .xor\n",
				or->or_name) ;
			continue ;
		}
		if	( or->or_num < BaseUserObj(0) )		/* !!! */
			continue ;

		oor	= XoNewObjRef(or->or_num) ;
		if	( !oor )
		{
			XoFree	(cfg,1) ;
			return	-3 ;
		}
		ret	= XoAddInAttr	((void *)cfg,XoCfgListObjRef,oor,0) ;
		if	( ret < 0 )
		{
			XoFree	(cfg,1) ;
			return	-4 ;
		}

		fprintf(fpointc,"\tif (GenXo_%s() < 0 ) return -%d ;\n",
			or->or_name,(i-XoNbObjInt)+1) ;

	}

	fprintf(fpointc,"\treturn 0 ;\n") ;
	fprintf(fpointc,"}\n") ;

	ret	= XoSave((void *)cfg,fout) ;
	if	( ret < 0 )
	{
		XoFree	(cfg,1) ;
		return	-5 ;
	}

	XoFree	(cfg,1) ;
	return	nb ;
}

int	saveref(char *app,char *desc)
{
	char	tmpout[128] ;
	char	hout[128] ;
	char	cout[128] ;
	int	fout ;

	if	( NbObj <= 0 )
	{
		return 0 ;
	}

	sprintf	(tmpout,"%s.xor",app) ;
	fout	= open	(tmpout,XO_FIC_BINARY|O_RDWR|O_TRUNC|O_CREAT,0666) ;
	if	( fout < 0 )
	{
		fprintf(stderr,"'%s' cannot open\n",tmpout) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}
#if	0
	if	( XoSaveHeader	(desc,fout) < 0 )
	{
		fprintf(stderr,"%s ecriture header impossible\n",tmpout) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}
#endif

#if	0
	if	( XoSaveRef	(fout) < 0 )
#endif
	if	( xosaveref	(fout) < 0 )
	{
		fprintf(stderr,"'%s' error while saving\n",tmpout) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}

	sprintf	(hout,"def%s.h",app) ;
	unlink	(hout) ;
#ifdef	WIN32
	fclose	(fpointh) ;
	if	( XoRenameFile(pointh,hout) < 0 )
	{
		fprintf(stderr,"'%s' -> '%s' cannot rename\n",pointh,hout) ;
	}
#else
	if	( link	(pointh,hout) < 0 )
	{
		fprintf(stderr,"'%s' -> '%s' cannot link\n",pointh,hout) ;
	}
	unlink	(pointh) ;
#endif

	if	(gener)
	{
		sprintf	(cout,"gen%s.c",app) ;
		unlink	(cout) ;
#ifdef	WIN32
		fclose	(fpointc) ;
		if	( XoRenameFile(pointc,cout) < 0 )
		{
			fprintf(stderr,"'%s' -> '%s' cannot rename\n",pointc,cout) ;
		}
#else
		if	( link	(pointc,cout) < 0 )
		{
			fprintf(stderr,"'%s' -> '%s' cannot link\n",pointc,cout) ;
		}
		unlink	(pointc) ;
#endif

	}
	else
	{
#ifdef	WIN32
		fclose	(fpointc) ;
		unlink	(pointc) ;
#endif
	}
	return	0;
}


void	Usage()
{
	fprintf	(stderr,"mkxo.x [-h] [-U] [-v] [-g] -f file\n") ;
	fflush	(stderr) ;
}

void	DoArg(int argc,char *argv[])
{
	int	c ;
	extern	char	*optarg ;

	while	((c = getopt (argc, argv, "hUvgf:")) != -1) 
	{
		switch (c) 
		{
		case	'h' :
			Usage() ;
			exit	(0) ;
		break ;
		case	'v' :
			verbose = 1 ;
		break ;
		case	'g' :
			gener = 1 ;
		break ;
		case	'U' :
			upperonly = 1;
		break;
		case	'f' :
			strcpy(arg_initial,optarg) ;
		break ;
		}
	}
}

#if 1 || defined(WIN32) || defined(LINUX)
int	yywrap()
{
	return	1 ;
}
#endif



int	main(int nb,char *arg[])
{
	FILE	*f ;
	int		nb_passe ;
	char	*basename = NULL ;


	if ( nb < 2 )
	{
		Usage() ;
		exit(1) ;
	}

	DoArg	(nb,arg) ;


	strcpy(Appli,arg_initial) ;
	sprintf(fichier_initial,"%s.ref",Appli) ;

	base_name(Appli,&basename) ;
	strcpy(Appli,basename) ;

	strcpy(Appli0,Appli) ;
	basename = strchr(Appli0,'.') ;
	if	( basename )
		*basename = '\0' ;

if	( verbose )
{
	printf	("input file : '%s'\n",fichier_initial) ;
	printf	("C code generation for class declaration: %s\n",
							gener?"yes":"no") ;
}

#ifdef	WIN32
	sprintf(pointh,"_xoA.%ld",getpid());
#else
	strcpy(pointh,tempnam("./","xo"));
#endif

	fpointh	= fopen(pointh,"w") ;
	if	( !fpointh )
	{
		fprintf(stderr,"'%s' cannot create\n",pointh) ;
		exit(1) ;
	}


	if	( gener )
	{
#ifdef	WIN32
		sprintf(pointc,"_xoB.%ld",getpid());
#else
		strcpy(pointc,tempnam("./","xo"));
#endif
		fpointc	= fopen(pointc,"w") ;
		if	( !fpointc )
		{
			fprintf(stderr,"'%s' cannot create\n",pointc) ;
			exit(1) ;
		}
	}
	else
	{
#ifdef	WIN32
		sprintf(pointc,"_xoC.%ld",getpid());
		fpointc	= fopen(pointc,"w") ;
#else
		pointc[0]	= '\0' ;
		fpointc	= fopen("/dev/null","w+") ;
#endif
		if	( !fpointc )
		{
			fprintf(stderr,"'%s' cannot open\n",pointc) ;
			exit(1) ;
		}
	}

	fprintf(fpointc,"/*\tattention ce fichier est genere par MKXO\t*/\n") ;
	fprintf(fpointc,"/*\til est vivement deconseille de le modifier\t*/\n");
	fprintf(fpointc,"#include\t<stdio.h>\n") ;
	fprintf(fpointc,"#include\t<string.h>\n") ;
	fprintf(fpointc,"#include\t\"xoapipr.h\"\n") ;



	XoInit	(0) ;
	Or	= XoNew	(XoObjRef) ;
	if	( !Or )
	{
		fprintf(stderr,"XoNew(XoObjRef) failed %d\n",XoObjRef) ;
		fflush(stderr) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}
	Ar	= XoNew	(XoAttrRef) ;
	if	( !Ar )
	{
		fprintf(stderr,"XoNew(XoAttrRef) failed %d\n",XoAttrRef) ;
		fflush(stderr) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}

	nb_passe = 2 ;
	for ( passe = 0 ; passe < nb_passe ; passe++ )
	{
		if ( passe == 1 )
			do_all_alias();
		f = freopen(fichier_initial,"r",stdin) ;
		if ( f == NULL )
		{
			fprintf(stderr,"'%s' cannot open\n",arg_initial);
			exit(1) ;
		}
		nl	= 1 ;
		if	(verbose)
			printf	("\tPass %d\n",passe) ;
		init_symb() ;
		yyparse() ;
		if ( passe == 0 && verbose )
		{
			listeobj() ;
		}

	}
	saveref(Appli,Desc) ;
	if	(verbose)
		printf("\tnb obj %d\n",NbObj) ;

	exit(0) ;
}

%}


%union
	{
	char	chaine[LG_MAX+1] ;
	int		entier ;
	}

%token		TDEFINE		
%token		TOBJET		
%token		TDESC		
%token		TLISTE		
%token		TNOSAVE		
%token		TSAVEUNSET
%token		TLGMAX		
%token		TVALUE		
%token		TROOT		
%token		TEXTERN		
%token		TSTEXT		
%token		TDEFAULT		
%token		THELPFILE		
%token		THELPLINE		
%token		TFROM		
%token		TALIAS		
%token		TWITH		
%token		TEMBATTR		
%token		TWRAPHIDDEN		
%token		TMAXOCC		
%token		TMINOCC		
%token		TSHDICO		




%token		<entier>	ENTIER
%token		<chaine>	CHAINE

%type		<entier>	isroot
%type		<entier>	isextern
%type		<entier>	isstext
%type		<entier>	derivefrom
%type		<entier>	aliasof

%start	prog_definition

%%

prog_definition	:
		section_symb	section_desc	section_def
		{
		}
		;

section_symb	:	
			TDEFINE		'{'	liste_symb '}'
			|
		;

liste_symb	:	liste_symb	symb
			|
			/*	vide	*/
		;	

symb		:	CHAINE	'='	ENTIER
			{
				add_symb($1,(void *)$3,ENTIER) ;
			}
			|
			CHAINE	'='	CHAINE
			{
				add_symb($1,(void *)$3,CHAINE) ;
			}
		;

section_desc	:	TDESC	'=' CHAINE	
		{
			if	( passe == 1 )
				strcpy(Desc,$3) ;
		}
		|
		;

section_def 	:
		liste_objet
		;

liste_objet	:
		|
		liste_objet	objet
		;

isroot		:
		TROOT
		{
			$$	= 1 ;
		}
		|
		/*	vide	*/
		{
			$$	= 0 ;
		}
		;
isextern	:
		TEXTERN
		{
			$$	= 1 ;
		}
		|
		/*	vide	*/
		{
			$$	= 0 ;
		}
		;
isstext	:
		TSTEXT
		{
			$$	= 1 ;
		}
		|
		/*	vide	*/
		{
			$$	= 0 ;
		}
		;

derivefrom	:
		TFROM	CHAINE
		{
			if	( passe == 1 )
				$$	= find_obj($2);
			else
				$$	= 0 ;
		}
		|
		/*	vide	*/
		{
			$$	= -1 ;
		}
		;

aliasof	:
		TALIAS	CHAINE
		{
			$$	= find_obj($2);
		}
		;


objet		:
		/* $1    $2       $3      $4      $5     */
		isroot	isextern isstext CHAINE	derivefrom '{'	liste_attr	'}'
		{
			if	( passe == 1 )
				new_obj(Or,$4,$1,$2,$5,-1,$3) ;
			else
				add_obj($4,$1,$2,-1) ;
		}
		|
		aliasof TWITH CHAINE
		{
			if	( passe == 1 )
				new_obj(Or,$3,0,0,-1,$1,0) ;
			else
				add_obj($3,0,0,$1) ;
		}
		;

liste_attr	:
		|
		liste_attr	attr
		;
	
attr		:
		CHAINE	CHAINE	ens_caract
		{
			if	( passe == 1 )
				new_attr(Ar,$1,$2) ;
		}
		;
ens_caract	:
		|
		'{'	liste_caract	'}'
		;

liste_caract	:
		|
		liste_caract	caract
		;

caract		:
		flg_minocc 
		|
		flg_maxocc 
		|
		flg_wraphidden 
		|
		flg_embattr 
		|
		flg_shdico 
		|
		flg_nosave 
		|
		flg_saveunset
		|
		flg_liste 
		|
		lg_max 
		|
		liste_value
		|
		default_value
		|
		help_line
		|
		help_file
		{
		}
		;

flg_maxocc	:
		TMAXOCC '=' ENTIER
		{
			if	( passe == 1 )
				MaxOcc	= $3 ;
		}
		;
flg_minocc	:
		TMINOCC '=' ENTIER
		{
			if	( passe == 1 )
				MinOcc	= $3 ;
		}
		;
flg_embattr	:
		TEMBATTR
		{
/*			printf	("embeded xml attr\n"); */
			if	( passe == 1 )
				EmbAttribut	= 1 ;
		}
		;
flg_shdico	:
		TSHDICO
		{
/*			printf	("shdico attr\n"); */
			if	( passe == 1 )
				ShDico	= 1 ;
		}
		;
flg_wraphidden	:
		TWRAPHIDDEN
		{
/*			printf	("wrapped xml attr\n"); */
			if	( passe == 1 )
				WrapHidden	= 1 ;
		}
		;
flg_liste	:
		TLISTE
		{
			if	( passe == 1 )
				Liste	= 1 ;
		}
		;

flg_nosave	:
		TNOSAVE
		{
			if	( passe == 1 )
				Nosave	= 1 ;
		}
		;

flg_saveunset	:
		TSAVEUNSET
		{
			if	( passe == 1 )
				Saveunset	= 1 ;
		}
		;

lg_max		:
		TLGMAX	'=' ENTIER
		{
			if	( passe == 1 )
				LgMax	= $3 ;
		}
		;

liste_value	:
		TVALUE	'=' CHAINE
		{
			if	( passe == 1 )
				XoSetAttr	(Ar,XoAttrRefValue,$3,0) ;
		}
		|
		TVALUE	'=' ENTIER
		{
			if	( passe == 1 )
			{
				char	tmp[64] ;

				sprintf(tmp,"%d",atoi((char *)$3)) ;
				XoSetAttr	(Ar,XoAttrRefValue,tmp,0) ;
			}
		}
		;


default_value	:
		TDEFAULT	'=' CHAINE
		{
			if	( passe == 1 )
				XoSetAttr	(Ar,XoAttrRefDefault,$3,0) ;
		}
		;

help_line	:
		THELPLINE	'=' CHAINE
		{
			if	( passe == 1 )
				XoSetAttr	(Ar,XoAttrRefHelpLine,$3,0) ;
		}
		;

help_file	:
		THELPFILE	'=' CHAINE
		{
			if	( passe == 1 )
				XoSetAttr	(Ar,XoAttrRefHelpFile,$3,0) ;
		}
		;



%%
#include	"mkxo.l.h"

void	myerr(char *s,char *msg)
{
	fprintf(stderr,"%s <%d>  %s %s\n"
	,fichier_courant,nl,s,msg) ;
}

int	add_obj(char *name,int root,int exter,int numalias)
{

	if	( passe )
		return 0 ;

	if	( NbObjP0 >= OBJ_MAX )
	{
		fprintf(stderr,"too much objects (%d)\n",OBJ_MAX) ;
		yyerror("") ;
		return	0;
	}

	tabobj[NbObjP0]	= (t_obj *)malloc(sizeof(t_obj)) ;
	if	( !tabobj[NbObjP0] )
	{
		perror("malloc()") ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}
	memset	(tabobj[NbObjP0],0,sizeof(t_obj));

	tabobj[NbObjP0]->o_name	= (char *)malloc(strlen(name)+1) ;
	if	( !tabobj[NbObjP0]->o_name )
	{
		perror("malloc()") ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}
	strcpy	(tabobj[NbObjP0]->o_name,name) ;

	tabobj[NbObjP0]->o_root	= root ;
	tabobj[NbObjP0]->o_extern	= exter ;
	tabobj[NbObjP0]->o_alias	= numalias ;

	if	( verbose )
		printf	("add objet %s root %d extern %d alias %d\n",
				name,root,exter,numalias) ;

	NbObjP0++ ;
	return NbObjP0 ;
}

void	listeobj()
{
	int	i ;

	for	( i = 0 ; i < NbObjP0 ; i++ )
	{
		printf	("obj %-25.25s root %d extern %d\n",
		tabobj[i]->o_name, tabobj[i]->o_root,tabobj[i]->o_extern) ;
	}

}

int	find_obj(char *name)
{
	int	i ;

	for	( i = 0 ; i < NbObjP0 ; i++ )
	{
#if 0 // ce test empeche l'utilisation de la clause extern ...
		if	( tabobj[i]->o_extern )		continue ;
#endif
		if	( strcmp(name, tabobj[i]->o_name) == 0 )
			return	i ;
	}
	fprintf(stderr,"super/alias class [%s] not found\n",name) ;
	yyerror("") ;
	return	-1 ;
}

void	do_alias(char *name,char *namealias)
{
	int	i ;
	int	nba ;

	if	(!namealias || !*namealias)
		return;

	for	( i = 0 ; i < NbObjP0 ; i++ )
	{
		if	( tabobj[i]->o_extern )		continue ;
		if	( strcmp(namealias, tabobj[i]->o_name) == 0 )
		{
#if	0	// only one alias
			char	*al;
			al	= tabobj[i]->o_alias_has;
			if (al && *al)
			{
	fprintf(stderr,"[%s] already has an alias [%s] cannot add [%s]\n",
				tabobj[i]->o_name,al,name);
	yyerror("") ;
	return;
			}
#endif
			nba	= tabobj[i]->o_nalias;	
			if	(nba >= ALIAS_MAX)
			{
	fprintf(stderr,"[%s] max alias cannot add [%s]\n",
						tabobj[i]->o_name,name);
	yyerror("") ;
	return;
			}
			if	( verbose )
			{
				printf("[%s] has an alias [%s]\n",
					tabobj[i]->o_name,name);
			}
			tabobj[i]->o_alias_has[nba] = strdup(name);	
			tabobj[i]->o_nalias++;	
			return;
		}
	}
	fprintf(stderr,"alias [%s] not found\n",namealias) ;
	yyerror("") ;
	return;
}


void	do_all_alias()
{
	int	i;
	char	*namealias;

	for	( i = 0 ; i < NbObjP0 ; i++ )
	{
		if	( tabobj[i]->o_extern )		continue ;
		if	( tabobj[i]->o_alias <= 0 )	continue;
		namealias	= tabobj[tabobj[i]->o_alias]->o_name;
//printf("alias = [%s]\n",namealias);
		do_alias(tabobj[i]->o_name,namealias);
	}
}


void	new_obj(void *or,char *name,int root,int exter,int from,int alias,int stext)
{
	int	numself;
	char	*aliashas;
	t_obj	*self = NULL ;
	t_obj	*super = NULL ;
	t_obj	*namealias = NULL ;
	int	nalias;
	int32	wraphidden;


	numself	= find_obj(name);
	if	(numself < 0 || !tabobj[numself])
	{
		fprintf(stderr,"object [%s] not found ...\n",name) ;
		yyerror("") ;
	}
	self	= tabobj[numself];

	if	( from >= 0 )
		super	= tabobj[from] ;
	if	( alias >= 0 )
		namealias= tabobj[alias] ;

	if	( XoNumObjSilent(name) > 0 )
	{
		fprintf(stderr,"object [%s] multiple definitions\n",name) ;
		yyerror("") ;
	}
	if	( verbose )
	{
		printf	("new obj %s",name) ;
		if	( root )
			printf(" root ") ;
		if	( stext )
			printf(" stext ") ;
		if	( super )
			printf	(" derive from %s",super->o_name);
		if	( namealias )
			printf	(" alias of %s",namealias->o_name);
		printf	("\n");
	}
	if	( !exter )
	{
		fprintf(fpointh,"/*\n") ;
		fprintf(fpointh," *\t objet        : %s\n",name) ;
		fprintf(fpointh," *\t nb attr      : %d\n",NbAttr) ;
		fprintf(fpointh," */\n") ;

		fprintf(fpointc,"static\tint32\tGenXo_%s(void)\n{\n",name) ;
		fprintf(fpointc,"\tint32\tret ;\n") ;
		fprintf(fpointc,"\tint32\ti ;\n") ;
		fprintf(fpointc,"\tvoid\t*or ;\n") ;
		fprintf(fpointc,"\tvoid\t*ar[%d] ;\n\n",NbAttr+1) ;

		fprintf(fpointc,"\tmemset(ar,0,sizeof(ar));\n") ;
		fprintf(fpointc,"\tret = -1 ;\n") ;
		fprintf(fpointc,"\tif (!(or=XoNew(XoObjRef))) goto end ;\n") ;
		fprintf(fpointc,"\tret = -2 ;\n") ;
#if	0	// previous memset now
		fprintf(fpointc,"\tfor (i=0;i<%d;i++)\n",NbAttr) ;
		fprintf(fpointc,"\t\tar[i]=NULL ;\n") ;
#endif
		fprintf(fpointc,"\tfor (i=0;i<%d;i++)\n",NbAttr) ;
		fprintf(fpointc,"\t\tif (!(ar[i]=XoNew(XoAttrRef))) goto end ;\n") ;
		fprintf(fpointc,"\tret = -3 ;\n") ;
		fprintf(fpointc,"\tXoSetAttr(or,XoObjRefType,(void *)%d,0) ;\n",
						Composition) ;
		fprintf(fpointc,"\tXoSetAttr(or,XoObjRefName,\"%s\",0) ;\n",
						name) ;
		fprintf(fpointc,"\tXoSetAttr(or,XoObjRefRoot,(void *)%d,0) ;\n",
						root) ;
		fprintf(fpointc,"\n") ;
	}

	if	( verbose && Composition == STRUCT_OBJ )
	{
		printf("objet structure %s\n",name) ;
	}

	XoSetAttr	(or,XoObjRefType,(char *)Composition,0) ;
	XoSetAttr	(or,XoObjRefName,name,0) ;
	if	( root )
	{
		XoSetAttr	(or,XoObjRefRoot,(char *)root,0) ;
	}
	if	( stext )
	{
		XoSetAttr	(or,XoObjRefStExt,(char *)stext,0) ;
	}
	if	( super )
	{
		XoSetAttr	(or,XoObjRefFrom,(char *)super->o_name,0) ;
	}
	if	( namealias )
	{
		XoSetAttr	(or,XoObjRefAlias,(char *)namealias->o_name,0) ;
	}
	nalias	= self->o_nalias;
	if	(nalias)
	{
		int	i;
		int	sz;
		sz	= 0;
		for	(i = 0 ; i < nalias ; i++)
		{
			aliashas = self->o_alias_has[i];
			sz	= sz + strlen(aliashas) + 4;
		}
		aliashas	= (char *)malloc(sz + 4);
		*aliashas	= '\0';
		for	(i = 0 ; i < nalias ; i++)
		{
			strcat(aliashas,self->o_alias_has[i]);
			strcat(aliashas,",");
		}
		XoSetAttr	(or,XoObjRefAliasHas,(char *)aliashas,0) ;
		free(aliashas);
	}


	if	(!exter)
	{
		NbAttr = do_attr(or,name,NumObj,super) ;
		fprintf(fpointh,"\n\n") ;

		
		fprintf(fpointc,"\tXoSetAttr(or,XoObjRefNbAttr,(void *)%d,0) ;\n",
						NbAttr) ;
		fprintf(fpointc,"\tret = -100 ;\n") ;
		fprintf(fpointc,"\tif (XoAddObjRef(or)<0) goto end ;\n") ;
		fprintf(fpointc,"\tret = 0 ;\n") ;
		fprintf(fpointc,"\tend :\n") ;
		fprintf(fpointc,"\tfor (i=0;i<%d;i++)\n",NbAttr) ;
		fprintf(fpointc,"\t\tif (ar[i]) XoFree(ar[i],0) ;\n") ;
		fprintf(fpointc,"\tif (or) XoFree(or,0) ;\n") ;
		fprintf(fpointc,"\treturn\tret ;\n}\n") ;
	}
	XoSetAttr	(or,XoObjRefNbAttr,(char *)NbAttr,0) ;

	if	( XoAddObjRef	(or) < 0 )
	{
		fprintf(stderr,"XoAddObjRef(%s) failed\n",name) ;
		unlink(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}

	tabobj[NbObj]->o_or	= or ;

	Or		= XoNew	(XoObjRef) ;
	if	( !Or )
	{
		fprintf(stderr,"XoNew(XoObjRef) failed %d\n",XoObjRef) ;
		fflush(stderr) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}

	wraphidden	= XoGetRefObjWrapHidden(XoGetNumType(name));
	if	(wraphidden > 0)
	{
		char	*nameattr;

		nameattr	= XoGetRefAttrName(wraphidden);
		printf	("'%s' object has only an hidden wrapper attribut '%s'\n"
			,name,nameattr);
	}

	NbObj++ ;
	NbAttr	= 0 ;
	Composition	= SINGLE_OBJ ;
}

int	attr_defined(char *name,char *tab[],int nb)
{
	int	i ;
	char	*pt ;

	for	( i = 0 ; i < nb ; i++ )
	{
		pt	= tab[i];
		if	( name && *name && pt && *pt && strcmp(name,pt) == 0 )
			return	1;
	}

	return	0;
}

int	do_attr(void *or,char *nameobj,int num,void *psuper)
{
	char	tmp[128] ;
	char	*ar ;
	char	*nameattr ;
	char	*attrtype ;
	int	numattr ;
	int	ret ;
	int32	lg ;
	int	nb ;
	int32	pList ;
	int32	pLgMax ;
	int32	pNoSave ;
	int32	pSaveUnset ;
	int32	pEmbAttr ;
	int32	pShDico ;
	int32	pWrapHidden ;

	char	*tabAttr[2048];
	int	nbMax = sizeof(tabAttr)/sizeof(char *);
	int	nbAttr = 0;

	t_obj	*super = (t_obj *)psuper;

	if	( super )
	{
		ret	= XoFirstAttr(or,XoObjRefListAttr) ;
		while	( ret > 0 && nbAttr < nbMax )
		{
			ar	= (char *)XoGetAttr(or,XoObjRefListAttr,&lg) ;
			tabAttr[nbAttr++]= 
					(char *)XoGetAttr(ar,XoAttrRefName,&lg);
			ret	= XoNextAttr(or,XoObjRefListAttr) ;
		}
	}

	if	( super )
	{	/* on ajoute les attributs de la super classe, sauf s'ils sont
		 * redefinis dans la classe atcuelle
		 */
		XoFirstAttr(or,XoObjRefListAttr) ;	/* debut de liste */
		nb	= 0 ;
		ret	= XoLastAttr(super->o_or,XoObjRefListAttr) ;
		while	( ret > 0 )
		{
			nb++ ;
			ar=(char *)XoGetAttr(super->o_or,XoObjRefListAttr,&lg) ;
			nameattr= (char *)XoGetAttr(ar,XoAttrRefName,&lg) ;
			if	( ar && !attr_defined(nameattr,tabAttr,nbAttr) )
				XoInsInAttr(or,XoObjRefListAttr,ar,0) ;
			ret	= XoPrevAttr(super->o_or,XoObjRefListAttr) ;
		}
		if	( verbose )
			printf	("add %d attributs from classe [%s]\n",
				nb,super->o_name);
	}

	nb	= 0 ;
	ret	= XoFirstAttr(or,XoObjRefListAttr) ;
	while	( ret > 0 )
	{
		nb++ ;
		numattr	= nb ;
		ar	= (char *)XoGetAttr(or,XoObjRefListAttr,&lg) ;
		nameattr= (char *)XoGetAttr(ar,XoAttrRefName,&lg) ;
		attrtype= (char *)XoGetAttr(ar,XoAttrRefTypeName,&lg) ;

		pList= *(int32 *)XoGetAttr(ar,XoAttrRefList,&lg) ;
		pLgMax= *(int32 *)XoGetAttr(ar,XoAttrRefLgMax,&lg) ;
		pNoSave= *(int32 *)XoGetAttr(ar,XoAttrRefNoSave,&lg) ;
		pSaveUnset= *(int32 *)XoGetAttr(ar,XoAttrRefSaveUnset,&lg) ;
		pEmbAttr= *(int32 *)XoGetAttr(ar,XoAttrRefEmbAttr,&lg) ;
		pWrapHidden= *(int32 *)XoGetAttr(ar,XoAttrRefWrapHidden,&lg) ;
		pShDico= *(int32 *)XoGetAttr(ar,XoAttrRefShDico,&lg) ;

		sprintf	(tmp,"%s.%s",nameobj,nameattr) ;
		if	( verbose )
			printf	("\t\t%s -> %d\n",tmp,numattr) ;
		fprintf(fpointh,
		"#define %-32.32s ( %s + %d ) /* %s */\n",
		tmp,nameobj,numattr,attrtype) ;

fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefName,\"%s\",0) ;\n"
					,nb-1,nameattr);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefTypeName,\"%s\",0) ;\n"
					,nb-1,attrtype);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefList,(void *)%d,0) ;\n"
					,nb-1,pList);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefLgMax,(void *)%d,0) ;\n"
					,nb-1,pLgMax);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefNoSave,(void *)%d,0) ;\n"
					,nb-1,pNoSave);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefSaveUnset,(void *)%d,0) ;\n"
					,nb-1,pSaveUnset);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefEmbAttr,(void *)%d,0) ;\n"
					,nb-1,pEmbAttr);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefWrapHidden,(void *)%d,0) ;\n"
					,nb-1,pWrapHidden);
fprintf(fpointc,"\tXoSetAttr(ar[%d],XoAttrRefShDico,(void *)%d,0) ;\n"
					,nb-1,pShDico);

fprintf(fpointc,"\tXoAddInAttr(or,XoObjRefListAttr,ar[%d],0) ;\n\n",nb-1) ;

		ret	= XoNextAttr(or,XoObjRefListAttr) ;
	}
	return	nb;
}

void	new_attr(void *ar,char *name,char *nametype)
{
	int	type ;
	int	mode ;

	mode	= BASIC_ATTR ;
	type	= XoNumObjSilent(nametype) ;
	if	( type  < 0 )
	{
		fprintf(stderr,"object/type [%s] not found\n",nametype) ;
		yyerror("") ;
	}

	if	( type >= BaseUserObj(0) )
	{
		Composition	= STRUCT_OBJ ;
	}
	
	/*
	 *	XoAttrRefNum sera valorise dans do_attr()
	 */
	XoSetAttr	(ar,XoAttrRefName,name,0) ;
	XoSetAttr	(ar,XoAttrRefTypeName,nametype,0) ;
	XoSetAttr	(ar,XoAttrRefList,(char *)Liste,0) ;
	XoSetAttr	(ar,XoAttrRefLgMax,(char *)LgMax,0) ;
	XoSetAttr	(ar,XoAttrRefNoSave,(char *)Nosave,0) ;
	XoSetAttr	(ar,XoAttrRefSaveUnset,(char *)Saveunset,0) ;
	XoSetAttr	(ar,XoAttrRefEmbAttr,(char *)EmbAttribut,0) ;
	XoSetAttr	(ar,XoAttrRefWrapHidden,(char *)WrapHidden,0) ;
	XoSetAttr	(ar,XoAttrRefShDico,(char *)ShDico,0) ;


	XoAddInAttr	(Or,XoObjRefListAttr,ar,0) ;

	NbAttr++ ;

	MaxOcc		= -1 ;
	MinOcc		= 0 ;
	EmbAttribut	= 0 ;
	ShDico		= 0 ;
	WrapHidden	= 0 ;
	Liste		= 0 ;
	Nosave		= 0 ;
	Saveunset	= 0 ;
	LgMax		= -1 ;

	Ar		= XoNew	(XoAttrRef) ;
	if	( !Ar )
	{
		fprintf(stderr,"XoNew(XoAttrRef) failed %d\n",XoAttrRef) ;
		fflush(stderr) ;
		unlink	(pointh) ;
		unlink	(pointc) ;
		exit(1) ;
	}
}


/*
 *	gestion des symboles
 */

#define		MAX_SYMB	1024

typedef	struct
{
	int	type ;
	char	name[33] ;
	char	cval[128] ;
	long	lval ;
}	t_symb ;

t_symb	Tabsymb[MAX_SYMB] ;

int		Nbsymb ;

void	init_symb()
{
	Nbsymb	= 0 ;
}

int	add_symb(char *name,void *val,int type)
{
	if	( Nbsymb + 1 >= MAX_SYMB )
	{
		fprintf(stderr,"max symbols !!!\n") ;
		return -1 ;
	}

	if	( type != ENTIER && type != CHAINE )
	{
		fprintf(stderr,"error type symbol !!!\n") ;
		return -1 ;
	}

	if	( verbose > 1 )
		printf("add symbole [%s]\n",name) ;


	strcpy	(Tabsymb[Nbsymb].name,name) ;
	Tabsymb[Nbsymb].type	= type ;
	switch	(type)
	{
		case	CHAINE :
			strcpy	(Tabsymb[Nbsymb].cval,(char *)val) ;
		break ;
		case	ENTIER :
			Tabsymb[Nbsymb].lval	= (int32)val ;
		break ;
	}
	Nbsymb++ ;
	return	Nbsymb ;
}

int	is_symbole(char *name,long *lval,char **cval)
{
	int	i ;


	for	( i = 0 ; i < Nbsymb ; i++ )
	{
		if	( strcmp(name,Tabsymb[i].name) == 0 )
			break ;
	}
	if	( i >= Nbsymb )
		return	ENTIER+CHAINE ;

	switch	( Tabsymb[i].type )
	{
		case	ENTIER :
			*lval	= Tabsymb[i].lval ;
			if	( verbose > 1 )
			printf("trouve symbole [%s] %d\n",name,(int)*lval) ;
			return	ENTIER ;
		break ;
		case	CHAINE :
			*cval	= &Tabsymb[i].cval[0] ;
			if	( verbose > 1 )
				printf("trouve symbole [%s] %s\n",name,*cval) ;
			return	CHAINE ;
		break ;
	}
	return	ENTIER+CHAINE ;
}
