#include "ok.h"
#include "knx.h"

extern struct list_head covConfig;

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
t_var TbVar[256] = {};
/* ---------- */

int
main (int argc, char **argv)
{
    bool enable;
    int min, max;
    Dpt_t dpt;

    // Simulate a CovInit
    AddCov("knx:/dpt/7.016", "PT5M", "PT15M");
    AddCov("knx:/dpt/7.018", "PT4M", "PT14M");
    AddCov("knx:/dpt/7.002", "PT3M", "PT13M");
    AddCov("knx:/dpt/7.001", "PT2M", "PT12M");
    AddCov("knx:/dpt/1.005", "PT1M", "PT11M");
    AddCov("knx:/dpt/1.002", "PT1H", "PT11H");
    AddCov("knx:/dpt/8.005", "PT2H", "PT12H");
    AddCov("knx:/dpt/8",     "PT3H", "PT13H");
    list_sort(NULL, &covConfig, knxCovSortFunction);
    
    // Test : covDptEnable
    DPT_SET(dpt, 7, 16);
    enable = covDptEnable(dpt);
    assert(enable == true);

    DPT_SET(dpt, 7, 17);
    enable = covDptEnable(dpt);
    assert(enable == false);
    
    DPT_SET(dpt, 7, 18);
    enable = covDptEnable(dpt);
    assert(enable == true);    

    DPT_SET(dpt, 8, 5);
    enable = covDptEnable(dpt);
    assert(enable == true);  

    DPT_SET(dpt, 8, 8);
    enable = covDptEnable(dpt);
    assert(enable == true);
    
    
    // Test : covGetTiming
    DPT_SET(dpt, 8, 5);
    covGetTiming(dpt, &min, &max);
    assert(min == 2 * 3600);
    assert(max == 12 * 3600);
    
    DPT_SET(dpt, 8, 8);
    covGetTiming(dpt, &min, &max);
    assert(min == 3 * 3600);
    assert(max == 13 * 3600);    
    
    
    PRINT_OK();
	return 0;	
}
