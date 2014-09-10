#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "ok.h"
#include "knx.h"

/*
 *  Create symbols need by Actility shared libs
 */

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
    XoInit(0);
    if (!Operating) {
        sprintf(tmp, "%s/%s/xo/.", rootactPath, GetAdaptorName());
        XoLoadNameSpaceDir(tmp);
        XoLoadNameSpaceDir(".");
    } else {
        sprintf(tmp, "%s/etc/xo/.", rootactPath);
        XoLoadNameSpaceDir(tmp);
        sprintf(tmp, "%s/etc/%s/.", rootactPath, GetAdaptorName());
        XoLoadNameSpaceDir(tmp);
    }

    if (LoadM2MXoRef() < 0)
        exit(1);

    if (LoadDrvXoRef(GetAdaptorName()) < 0)
        exit(1);

    /* ------------------------------------------------------------- */

    void *xo;
    char *xml, *str;
    int	err, rc, i;
    uint32_t ui;
    bool b, c;
    float f;
    
#define INIT_XO(str) \
    xo = XoReadXmlMem(str, strlen(str), NULL, XOML_PARSE_OBIX, &err);      \
    if (xo == NULL) { printf("Error on XoReadXmlMem\n"); exit(1); }
    
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"false\">"
                "<bool name=\"b\" val=\"true\"/>"
                "<bool name=\"c\" val=\"false\"/>"
                "<int name=\"i\" val=\"123\"/>"
                "<int name=\"ui\" val=\"4294901760\"/>"
                "<real name=\"f\" val=\"123.5\"/>"
                "<str name=\"raw\" val=\"0x00\"/>"
            "</bool>");

    rc = getObixBoolValue(xo, &b, "wrapper$[name=%s].val", "b");
    assert(rc == 0);
    assert(b == true);

    rc = getObixBoolValue(xo, &c, "wrapper$[name=%s].val", "c");
    assert(rc == 0);
    assert(c == false);

    rc = getObixStrValue(xo, str, "wrapper$[name=%s].val", "raw");
    assert(rc == 0);
    assert(!strcmp(str, "0x00"));

    rc = getObixIntValue(xo, &i, "wrapper$[name=%s].val", "i");
    assert(rc == 0);
    assert(i == 123);

    rc = getObixRealValue(xo, &f, "wrapper$[name=%s].val", "f");
    assert(rc == 0);
    assert(f == 123.5f);
    
    rc = getObixUintValue(xo, &ui, "wrapper$[name=%s].val", "ui");
    assert(rc == 0);
    assert(ui == (uint32_t)4294901760);
    
    PRINT_OK();
    return 0;
}

