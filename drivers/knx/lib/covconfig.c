#include <stdio.h>            /************************************/
#include <stdlib.h>            /*     run in main thread           */
#include <string.h>            /************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "knx.h"

LIST_HEAD(covConfig);

/*
 * The comparison function @cmp must return a negative value if @a
 * should sort before @b, and a positive value if @a should sort after
 * @b. If @a and @b are equivalent, and their original relative
 * ordering is to be preserved, @cmp must return 0.
 */
int
knxCovSortFunction(void *priv, struct list_head *a, struct list_head *b)
{
    knxCov_t *cova = container_of(a, knxCov_t, list);
    knxCov_t *covb = container_of(b, knxCov_t, list);
    
    if (cova->dpt.primary < covb->dpt.primary)
        return -1;
        
    if (cova->dpt.primary > covb->dpt.primary)
        return 1;

    if (cova->primaryConfig == true && covb->primaryConfig == false)
        return 1;
        
    if (cova->primaryConfig == false && covb->primaryConfig == true)
        return -1;
    
    if (cova->primaryConfig == true && covb->primaryConfig == true)
        return 0;   // Duplicate Cov config

    if (cova->dpt.sub < covb->dpt.sub)
        return -1;
        
    if (cova->dpt.sub > covb->dpt.sub)
        return 1;
    
    return 0; // Duplicate Cov config
}

// Add a rule into the covConfig list
void
AddCov(char *filter, char *minInterval, char *maxInterval)
{
    int rc;
    knxCov_t *cov;

    cov = (knxCov_t *) malloc(sizeof(knxCov_t));
    if (cov == NULL) {
        return;
    }
    cov->lastReportTime = 0;

    rc = sscanf(filter, "knx:/dpt/%hu.%hu", &(cov->dpt.primary), &(cov->dpt.sub));
    if (rc == 0) {
        free (cov);
        return;
    } else if (rc == 1) {
        cov->primaryConfig = true;      // knx:/dpt/%u
        cov->dpt.sub = 0;
    } else {
        cov->primaryConfig = false;     // knx:/dpt/%u.%u
    }

    cov->minInterval = parseISO8601Duration(minInterval);
    
    if (maxInterval == NULL) {
        cov->maxInterval = 10000 * 24 * 60 * 60;
    } else {
        cov->maxInterval = parseISO8601Duration(maxInterval);
        if (cov->maxInterval == 0) {
            cov->maxInterval = 24 * 60 * 60;
        }
    }
    
    list_add(&(cov->list), &covConfig);
}

// Load all rules
void
CovInit(void *mdl)
{
    int i, itfCount;
    void *itf;
    void *cov;
    char *filter, *minInterval, *maxInterval;

    /*
     *  Interface filter have the following format
     *  - knx:/dpt/7        (All the 7 section)
     *  - knx:/dpt/7.001    (A specific DPT code)
     *
     *  They have always 1 covConfigurations and it don't have filter
     */
    itfCount = XoNmNbInAttr(mdl, "ong:interfaces");
    for(i=0; i<itfCount; i++) {
        itf = XoNmGetAttr(mdl, "ong:interfaces[%d]", NULL, i);
        if (itf == NULL) {
            continue;
        }

        filter = (char *) XoNmGetAttr(itf, "ong:filter", NULL, 0);
        if (filter == NULL || filter[0] == '\0') {
            continue;
        }

        cov = XoNmGetAttr(itf, "ong:covConfigurations[%d]", NULL, 0);
        if (cov == NULL) {
            continue;
        }
        
        minInterval = (char *) XoNmGetAttr(cov,"ong:minInterval", NULL);
        maxInterval = (char *) XoNmGetAttr(cov,"ong:maxInterval", NULL);
        
        AddCov(filter, minInterval, maxInterval);
    }
    
    list_sort(NULL, &covConfig, knxCovSortFunction);
}

// Display current Cov configuration on CLI interface
void
CovToCzDump(void *cl)
{
    struct list_head *index;
    knxCov_t *cov;
    
    AdmPrint(cl, "current cov configuration loaded (white list):\n");
	AdmPrint(cl, "Primary\t\tSub\t\tGroupFilter\tminInterval\tmaxInterval\n");
    list_for_each(index, &covConfig) {
        cov = list_entry(index, knxCov_t, list);
        
        AdmPrint(cl, "%d\t\t%d\t\t%s\t\t%d\t\t%d\n",
            cov->dpt.primary,
            cov->dpt.sub,
            cov->primaryConfig ? "true" : "false",
            cov->minInterval,
            cov->maxInterval);
    }
}

// Return true if the dpt match a cov rule
bool
covDptEnable(Dpt_t dpt)
{
    struct list_head *index;
    knxCov_t *cov;
    
    list_for_each(index, &covConfig) {
        cov = list_entry(index, knxCov_t, list);
        
        if (dpt.primary == cov->dpt.primary) {
            if (cov->primaryConfig == true) {
                return true;
            } else {
                if (dpt.sub == cov->dpt.sub) {
                    return true;
                }
            }
        }
    }

    return false;
}

void
covGetTiming(Dpt_t dpt, int *min, int *max)
{
    struct list_head *index;
    knxCov_t *cov;
    
    list_for_each(index, &covConfig) {
        cov = list_entry(index, knxCov_t, list);
        
        if (dpt.primary == cov->dpt.primary) {
            if (cov->primaryConfig == true) {
                *min = cov->minInterval;
                *max = cov->maxInterval;
                return;
            } else {
                if (dpt.sub == cov->dpt.sub) {
                    *min = cov->minInterval;
                    *max = cov->maxInterval;
                    return;
                }
            }
        }
    }

    // Must be never reached
    *min = 5 * 60;
    *max = 60 * 60;
}

void
appendReport(t_sensor *cp, void *xo)
{
    void *oldxo;
    time_t ts;
    
    if (xo == NULL) {
        return;
    }
    //ts = time(NULL);
    rtl_timemono(&ts);
    cp->report.time = ts;
    
    oldxo = cp->report.xo;
    cp->report.xo = xo;
    if (oldxo != NULL) XoFree(oldxo, 1);
    cp->report.waitingDia = false;

    // Debug
    RTL_TRDBG(2, "Create report into group %d for network %s\n", cp->knxId, cp->network->name);
    if ((cp->report.lastTxTime + cp->report.minInterval) > ts) {
        RTL_TRDBG(2, "\tBefore minInterval, waiting to %d\n", cp->report.lastTxTime + cp->report.minInterval);
    } else {
        RTL_TRDBG(2, "\tAfter minInterval\n");
    }
}
