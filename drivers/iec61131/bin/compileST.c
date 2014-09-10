
#include <fcntl.h>
#include "compileST.h"

extern char *yy_scan_string(const char *);
extern void yy_delete_buffer(char *);
extern int yylex_destroy  (void);
extern int yyparse();

int compileST (char *pgm) {
	extern char *yy_scan_bytes();
	char *my_string_buffer = yy_scan_bytes (pgm, strlen(pgm)+1);
	int my_parse_result  = yyparse ();
	if	(my_parse_result)
		return	-1;
	yy_delete_buffer (my_string_buffer);
	yylex_destroy();
	return 0;
}

