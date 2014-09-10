#include "knx.h"


static
void
sendReports(time_t ts)
{
    // Check for report to send 
    struct list_head *index, *index2;
    t_sensor *cp;    
    NetworkList_t *network;
    bool waitingDia;
    
    /*
     *  Each network process a report at a time
     *  They are stored in a FIFO : network->reports
     */
    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);
        
        // Ensure they are no pending report for this network
        waitingDia = false;
        list_for_each(index2, &(network->groupApp)) {
            cp = list_entry(index2, t_sensor, list);   

            if (cp->report.waitingDia == true) {
                waitingDia = true;
                RTL_TRDBG(2, "KnxSensorClockSc : report pending for %s\n", network->name);
                break;
            }
        }
        if (waitingDia == true) {
            continue;
        }
        
        // Emit report        
        list_for_each(index2, &(network->groupApp)) {
            cp = list_entry(index2, t_sensor, list);   

            if (cp->report.xo == NULL) {
                continue;
            }
            
            // First time
            if ((cp->report.time >= cp->report.lastTxTime) && (ts > (cp->report.lastTxTime + cp->report.minInterval))) {
                RTL_TRDBG(2, "Initial report for group %d, at %d\n", cp->knxId, ts);
                cp->report.lastTxTime = ts;
                cp->report.waitingDia = true;
                DiaReportValue(cp->cp_num, cp->cp_serial, DEVICE_TYPE_GROUP_ADDR_DEVICE, 0, 0, 0, ""); 
            }
            else
            
            // Replay report
            if (ts > (cp->report.lastTxTime + cp->report.maxInterval)) {
                RTL_TRDBG(2, "Replay report for group %d, at %d\n", cp->knxId, ts);
                cp->report.lastTxTime = ts;
                cp->report.waitingDia = true;
                DiaReportValue(cp->cp_num, cp->cp_serial, DEVICE_TYPE_GROUP_ADDR_DEVICE, 0, 0, 0, ""); 
            }
        }
    }
}

static
void
checkEibdProcessus(time_t ts)
{
    struct list_head *index;
    NetworkList_t *network;
    
    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);

        RTL_TRDBG(9, "checkEibdProcessus on network %s (eibdURI:%s) (eibdPid:%d)\n", 
            network->name, network->eibdURI, network->eibdPid);
        
        if (network->eibdURI == NULL || EibdIsRunning(network)) {
            continue;
        }
        
        RTL_TRDBG(2, "EIDB fault detected on network %s, I will be back !\n", network->name);
        
        EibdExit(network);
        EibdStartDaemon(network);
    }
}

/**
 * @brief Do synchronize each layer with actual time.
 * One second has elapsed since last call. Each layer that needs
 * to process periodical tasks (i.e. recurrent timers) can do this
 * from this call.
 *
 * @param now current time in second (time(2))
 * @return void
 */
void
KnxSensorClockSc(time_t now)
{
    sendReports(now);
    checkEibdProcessus(now);
}

