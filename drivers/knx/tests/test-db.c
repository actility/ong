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
    int i, rc;
    char *str;
    ManufacturerInfos_t maninfos[1];
    DptInfos_t dptinfo[1];
    Dpt_t dpt;
    
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

    if(getenv(DRIVER_DATA_DIR) == NULL) {
        printf("Check your environnement varaibles (" DRIVER_DATA_DIR ").\n");
        exit(1);
    }


    /*
     *  Manufacturer 
     */
    rc = ManufacturerLoadDatabase();
    assert(rc == 0);

    rc = ManufacturerGetInfos(3, maninfos);
    assert(rc != 0);

    rc = ManufacturerGetInfos(157, maninfos);
    assert(rc == 0);
    assert(maninfos->id == 157);
    assert(!strcmp(maninfos->name, "EGi Electroacustica General Iberica"));

    rc = ManufacturerGetInfos(250, maninfos);
    assert(rc == 0);
    assert(maninfos->id == 250);
    assert(!strcmp(maninfos->name, "KNX Association"));
    
    /*
     *  DPT 
     */
    rc = DptLoadDatabase();
    assert(rc == 0);

    DPT_SET(dpt, 10000, 23);
    rc = DptGetInfos(dpt, dptinfo);
    assert(rc != 0);
    
    DPT_SET(dpt, 9, 23);
    rc = DptGetInfos(dpt, dptinfo);
    assert(rc == 0);
    assert(dptinfo->dpt.primary == 9);
    assert(dptinfo->dpt.sub == 23);
    assert(dptinfo->bits == 16);
    assert(!strcmp(dptinfo->name, "DPT_KelvinPerPercent"));

    /*
     *  Range (Enum)
     *  Bin to String
     */    
    rc = RangeLoadDatabase();
    assert(rc == 0);
    
    DPT_SET(dpt, 2, 3);
    rc = RangeGetRenderedValue(dpt, 0, &str);
    assert(rc == 0);
    assert(!strcmp(str, "noControl"));

    DPT_SET(dpt, 20, 13);
    rc = RangeGetRenderedValue(dpt, 14, &str);
    assert(rc == 0);
    assert(!strcmp(str, "2,5min"));

    /*
     *  Range (Enum)
     *  String to Bin
     */
    DPT_SET(dpt, 2, 3);
    rc = RangeGetBinValue(dpt, &i, "noControl");
    assert(rc == 0);
    assert(i == 0); 

    DPT_SET(dpt, 20, 13);
    rc = RangeGetBinValue(dpt, &i, "2,5min");
    assert(rc == 0);
    assert(i == 14); 

    PRINT_OK();
    return 0;
}

