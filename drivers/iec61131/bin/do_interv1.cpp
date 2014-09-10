#include <fcntl.h>
#include <iostream>
#include "codegen.h"
#include "node.h"

using namespace std;

extern char *yy_scan_string(const char *);
extern void yy_delete_buffer(char *);
extern int yylex_destroy  (void);

extern int yyparse();
extern NBlock* programBlock;


// TODO : BIG MEMORY LEAK : context object ; yacc buffer ; IECParam object

static param_ret_t iec_pret;

extern "C" param_ret_t *do_ST_interpreter (void *userctx, char *pgm) {
	char *my_string_buffer = yy_scan_string (pgm);
	int my_parse_result  = yyparse ();
	if	(my_parse_result)
		return	NULL;
	//yy_delete_buffer (my_string_buffer);
	yylex_destroy();

	//	context object is allocated in the stack -> no delete
	CodeGenContext context(userctx);
	IECParam *pret = context.generateCode(*programBlock);
	programBlock->undo();

	if	(pret) {
		iec_pret	= pret->iec;
		delete pret;
		return &iec_pret;
	}
	return	NULL;
}
