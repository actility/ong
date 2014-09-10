#include <fcntl.h>
#include "codegen.h"


extern char *yy_scan_string(const char *);
extern void yy_delete_buffer(char *);
extern int yylex_destroy  (void);

extern int yyparse();
extern inter_node_t *programBlock;


param_ret_t *do_ST_interpreter (void *userctx, char *pgm) {
	extern char *yy_scan_bytes();
	char *my_string_buffer = yy_scan_bytes (pgm, strlen(pgm)+1);
	int my_parse_result  = yyparse ();
	if	(my_parse_result)
		return	NULL;
	yy_delete_buffer (my_string_buffer);
	yylex_destroy();

	param_ret_t *pret = codeGen (programBlock, userctx);
	cleanUp (programBlock);

	return	pret;
}
