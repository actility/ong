%{

struct	instr
{
	char	*instruct ;
	int	token ;
} ;


struct	instr	tab_instr[] = 
{
		{	"DEFINE" 	, 	TDEFINE		} ,
		{	"OBJET" 	, 	TOBJET		} ,
		{	"DESC" 		, 	TDESC		} ,
		{	"LISTE" 	, 	TLISTE		} ,
		{	"LIST" 		, 	TLISTE		} ,
		{	"NOSAVE" 	, 	TNOSAVE		} ,
		{	"SAVEUNSET" 	, 	TSAVEUNSET	} ,
		{	"LGMAX" 	, 	TLGMAX		} ,
		{	"VALUE" 	, 	TVALUE		} ,

		{	"ROOT" 		, 	TROOT		} ,
		{	"EXTERN" 	, 	TEXTERN		} ,
		{	"FROM" 		, 	TFROM		} ,
		{	"ALIAS" 	, 	TALIAS		} ,
		{	"WITH" 	, 		TWITH		} ,
		{	"STEXT" 	, 	TSTEXT		} ,


		{	"DEFAULT" 	, 	TDEFAULT	} ,
		{	"HELPLINE" 	, 	THELPLINE	} ,
		{	"HELPFILE" 	, 	THELPFILE	} ,
		{	"EMBATTR" 	, 	TEMBATTR	} ,
		{	"WRAPHIDDEN" 	, 	TWRAPHIDDEN	} ,
		{	"MINOCC"	, 	TMINOCC		} ,
		{	"MAXOCC"	, 	TMAXOCC		} ,
		{	"SHDICO" 	, 	TSHDICO	} ,
		{	""		,	0		}

		/* attention indispensable "" , 0 */
} ;


%}

%%

[-]?[0-9][0-9]*	{	/* entier base 10 */
		int		ind ;
	
			ind	= 0 ;
			if	( yytext[0] == '-' )
				ind	= 1 ;
			yylval.entier = 0 ;
			for ( ; ind < yyleng ; ind++ )
			{
				yylval.entier = yylval.entier*10 + yytext[ind]-'0' ;
			}
			if	( yytext[0] == '-' )
				yylval.entier = -yylval.entier ;

			return(ENTIER) ;
		}
[0][xX][0-9a-fA-F][0-9a-fA-F]*	{
		int		ind ;

			yylval.entier = 0 ;
			for ( ind = 2 ; ind < yyleng ; ind++ )
			{
				if ( isdigit(yytext[ind]) )
				{
					yylval.entier = yylval.entier*16 + yytext[ind]-'0' ;
				}
				else
				{
					yylval.entier = yylval.entier*16 + 10 + yytext[ind]-'a' ;
				}
			}
			return(ENTIER) ;
		}	

[A-Za-z_][A-Za-z_0-9:$]*	{	/* chaine de caracteres ou instruct ou symbole user */
		long		ind ;
		char		*val ;

			if ( yyleng >= LG_MAX )
			{
				myerr("nom trop long\n","") ;
				exit(1) ;
			}
/*
			tok	= find_token(yytext);
			if	(tok > 0)
				return	tok;
*/

			for ( ind = 0 ; tab_instr[ind].instruct[0] ; ind++ )
			{
				if ( upperonly )
				{
					if ( !strcmp(yytext,tab_instr[ind].instruct) )
					{
						return(tab_instr[ind].token) ;
					}
					continue;
				}
				if ( !strcasecmp(yytext,tab_instr[ind].instruct) )
				{
					return(tab_instr[ind].token) ;
				}
			}

			switch	(is_symbole(yytext,&ind,&val))
			{
				case	ENTIER :
					yylval.entier = ind ;
					return	ENTIER ;
				break ;
				case	CHAINE :
					strcpy(yylval.chaine,val) ;
					return	CHAINE ;
				break ;
			}


			memset(yylval.chaine,0,sizeof(yylval.chaine)) ;
			strcpy(yylval.chaine,yytext) ;
			return(CHAINE) ;
		}


["]		{	/* chaine de caracteres entre " */
		int		ind ;
		int	car ;

		for ( ind = 0 ;(car=input()) != '"' && ind < LG_MAX ; ind++ )
		{
			yylval.chaine[ind] = car ;
		}
		yylval.chaine[ind++]= '\0' ;
/*
fprintf(stderr,"litteral [%s]\n",yylval.chaine) ;
*/
		return(CHAINE) ;
		}

[;#]		{	/* commentaires */
		int		ind ;
		int	car ;

		for ( ind = 0 ;(car=input()) != '\n' ; ind++ ) ;
		nl++ ;
		}

[/][/]		{	/* commentaires */
		int		ind ;
		int	car ;

		for ( ind = 0 ;(car=input()) != '\n' ; ind++ ) ;
		nl++ ;
		}

[/][*]		{	 /* commentaires */
		int	etat,fin,car;
			
		etat=fin=0;
		while(!fin)
		{
			car = input();
			if	(car == '\0')
				break;
			if (car ==  '\n')
				nl++;
			switch(etat)
			{
				case 0 : 
					if (car == '*')
						etat=1;
					break;
				case 1 :
					if (car == '/')
						fin=1;
					else if (car != '*')
							etat=0;
			}
		}
		}



[\n]		{
			nl++ ;
		}

[ \t]		{
		}

.		{
			return(yytext[0]) ;
		}



%%
