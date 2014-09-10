#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "ok.h"
#include "xo.h"
#include "knx.h"

/*
 *  Create symbols need by Actility shared libs
 */
#include "cmnstruct.h"
int TraceLevel = 9;
int TraceDebug = 9;
int TraceProto = 9;
int Operating = 0;
int	NbVar;
int	InternalVar;
void *MainTbPoll;
char *rootactPath;
char *GetAdaptorVersion() {return "knx-0.0.1-unit-tests";}
char *GetAdaptorName() {return "knx";}
char *GetEnvVarName(void) {if (!Operating){return "ROOTACT";} return NULL;}
/* ---------- */

/*
 *  Create template variables need by KNX parser & XML renderer
 */
#define VAR_INIT_EMPTY(x) {.v_name = #x, .v_default = "", .v_fct = NULL, .v_value = NULL}
t_var TbVar[256] = {
	VAR_INIT_EMPTY(dpt_value)
};

int
main (int argc, char **argv)
{
    // Init actility stuff
    char tmp[512];
    rootactPath = getenv("ROOTACT");
    if ((!rootactPath || !*rootactPath)) {
        printf("ROOTACT unset, abort\n");
        exit(1);
    }
    VarInit();

    /*
     *  SetVar like
     */
    SetVarIntoTemplate("dpt_value", "test");
    assert(!strcmp(GetVar("dpt_value"), "test"));

    SetVarIntoTemplate("dpt_value", "test 2");
    assert(!strcmp(GetVar("dpt_value"), "test 2"));

    /*
     *  Bool
     */
    SetBoolIntoTemplate("dpt_value", true);
    assert(!strcmp(GetVar("dpt_value"), "true"));

    SetBoolIntoTemplate("dpt_value", false);
    assert(!strcmp(GetVar("dpt_value"), "false"));
    
    /*
     *  Varadic function, use printf format 
     */
    RenderVarIntoTemplate("dpt_value", "%.3f", 123.456f);
    assert(!strcmp(GetVar("dpt_value"), "123.456"));

    RenderVarIntoTemplate("dpt_value", "%.*s", 3, "ABCDEF");
    assert(!strcmp(GetVar("dpt_value"), "ABC"));

    /*
     *  ISO 8601
     */
    SetVarAsISO8601Duration("dpt_value", 12, 0);
    assert(!strcmp(GetVar("dpt_value"), "P12S"));

    SetVarAsISO8601Duration("dpt_value", 3600 + 60 + 1, 0);
    assert(!strcmp(GetVar("dpt_value"), "P01H01M01S"));
    
    SetVarAsISO8601Duration("dpt_value", 2 * 24 * 3600, 0);
    assert(!strcmp(GetVar("dpt_value"), "P000002D"));

    SetVarAsISO8601Duration("dpt_value", 12, 123);
    assert(!strcmp(GetVar("dpt_value"), "P12S,123"));
    
    PRINT_OK();
    return 0;
}

