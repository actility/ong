#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <strings.h>
#include <ctype.h>
#include <dlfcn.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <linux/limits.h>

#include "knx.h"

#define MAX_KNX_DEVICE    10000
static t_sensor	* TbSensor[MAX_KNX_DEVICE];
static int NbSensor = 0;


t_sensor * SensorCreateDevice(int type);
NetworkList_t *SensorFindNetworkFromName(char *name);

void DiaAppCreateAllCnt(t_sensor *cp);
void DiaDevStartAll(void);
void DiaSensorSetSearchStringInObj(t_cmn_sensor *cmn, void *dst);
void DiaSensorSetApplicationListInObj(t_cmn_sensor *cmn, void *dst);
void DiaSensorSetInterfaceListInObj(t_cmn_sensor *cmn, int app, void *dst);

/*
 *  Utils
 *  -----------------------------------------------------------------------------------------------------
 */

static
t_sensor *
SensorNumSerial(int num, int serial)
{
	if (num < 0 || num >= MAX_SSR_PER_RTR || TbSensor[num] == NULL) {
		RTL_TRDBG(0, "ERROR bad num sensor %d\n", num);
		return	NULL;
	}
	
	if (TbSensor[num]->cp_serial != serial) {
		RTL_TRDBG(0, "ERROR bad serial sensor %d\n", serial);
		return	NULL;
	}

	return TbSensor[num];
}

/*!
 *
 * @brief called by DIA server module to retrieve the common structure of a 
 * sensor with its ieee address. Mainly used for retargeting purpose. (i.e. foo_grp_0_0_6.1)
 * @param ieee sensor ieee address
 * @return :
 * 	- the address of the common structure in the sensor found
 * 	- NULL if bad ieee address
 * 
 * used by diaserver.c
 */
t_cmn_sensor *
CmnSensorFindByIeee(char *ieee)
{
    struct list_head *index = NULL;
    t_sensor *cp = NULL;
    NetworkList_t *net = NULL;
    char *tmp = NULL;
    int rc, addr, i, j, k;
    
    // parse
    tmp = strstr(ieee, "_");
    if (tmp == NULL || *tmp == '\0') {
        return NULL;
    }
    *tmp = '\0';
    tmp = strstr(tmp + 1, "_");
    if (tmp == NULL || *tmp == '\0') {
        return NULL;
    }
    rc = sscanf(tmp + 1, "%u_%u_%u.1", &i, &j, &k);
    if (rc != 3) {
        return NULL;
    }
    addr = (i << 11) + (j << 8) + k;
    
    // find the right network
    net = SensorFindNetworkFromName(ieee);
    if (net == NULL) {
        return NULL;
    }

    // find the right group address
    list_for_each(index, &(net->groupApp)) {
        cp = list_entry(index, t_sensor, list);   
        if (cp->knxId == addr) {
            return &(cp->cp_cmn);
        }
    }

    // Not found
	return NULL;
}

t_cmn_sensor *
networkDiaLookup(int dev, int serial)
{
    struct list_head *index;
    NetworkList_t *network;
    
    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);
        t_sensor *cp = network->device;
        
        if (cp->cp_num == dev && cp->cp_serial == serial) {
            return (t_cmn_sensor *) cp;
        }
    }
    
    return NULL;
}

/*
 * used by diadef.h
 */
t_cmn_sensor *
CmnSensorNumSerial(int num, int serial)
{
    // Search in t_sensor
	t_sensor *cp = SensorNumSerial(num, serial);
	if (cp != NULL)
		return &cp->cp_cmn;

	return NULL;
}

#define SetVarFromXo(x,y,z) \
    tmp = (char *) XoNmGetAttr(y, z, 0); \
    if(tmp == NULL) {SetVar(x, "");} else {SetVar(x, tmp);}

/*
 *  Set template variable from a DPT code
 */
void
templateSetDPT(Dpt_t dpt)
{
    char buffer[64];
    DptInfos_t infos[1];
    int rc;

    snprintf(buffer, sizeof(buffer), "%u", dpt.primary);
    SetVar("dpt_primary_id", buffer);
    
    snprintf(buffer, sizeof(buffer), "%u", dpt.sub);
    SetVar("dpt_sub_id", buffer);
    
    snprintf(buffer, sizeof(buffer), "%u.%03u", dpt.primary, dpt.sub);
    SetVar("dpt_id", buffer);
    
    rc = DptGetInfos(dpt, infos);
    if (rc == 0) {
        SetVar("dpt_name", infos->name);
        SetVar("dpt_unit", infos->unit);
    } else {
        RTL_TRDBG(0, "Unknown DPT\n");
        SetVar("dpt_name", "");
        SetVar("dpt_unit", "");
    }
}

/*
 *  Set template variable from knx:object attritute
 */
void
templateSetObject(void *xo)
{
    void *tmp;
    
    SetVarFromXo("knxObjectID",             xo, "knx:id");
    SetVarFromXo("knxObjectName",           xo, "knx:name");
    SetVarFromXo("knxObjectDescription",    xo, "knx:description");
}

/*
 *  Set template variable from knx:object attritute
 */
void
templateSetDevice(void *xo)
{
    char *tmp;
    int i, rc;
    
    SetVarFromXo("knx_dev_description",     xo, "knx:description");
    SetVarFromXo("knx_dev_name",            xo, "knx:name");
    SetVarFromXo("knx_dev_fw",              xo, "knx:fw");
    SetVarFromXo("knx_dev_addr",            xo, "knx:addr");
    SetVarFromXo("knx_dev_area",            xo, "knx:area");
    SetVarFromXo("knx_dev_line",            xo, "knx:line");
    
    tmp = (char *) XoNmGetAttr(xo, "knx:addr", 0);
    if(tmp != NULL) {
        char *dot = index(tmp, '.');
        if (dot != NULL) {
            RenderVarIntoTemplate("knx_dev_area_id", "%.*s", (dot - tmp), tmp);
        } else {
            SetVar("knx_dev_area_id", "");
        }
    } else {
        SetVar("knx_dev_area_id", "");
    }

    tmp = (char *) XoNmGetAttr(xo, "knx:addr", 0);
    if(tmp != NULL) {
        char *dot = rindex(tmp, '.');
        if (dot != NULL) {
            RenderVarIntoTemplate("knx_dev_line_id", "%.*s", (dot - tmp), tmp);
        } else {
            SetVar("knx_dev_line_id", "");
        }
    } else {
        SetVar("knx_dev_line_id", "");
    }
    
    tmp = (char *) XoNmGetAttr(xo, "knx:manufacturer", 0);
    if (tmp != NULL) {
        SetVar("knx_dev_manufacturer", tmp);
        rc = sscanf(tmp, "M-%u", &i);
        if (rc == 1) {
            ManufacturerInfos_t infos;
            rc = ManufacturerGetInfos(i, &infos);
            if (rc == 0) {
                SetVar("knx_dev_manufacturer_name", infos.name);
            } else {
                SetVar("knx_dev_manufacturer_name", "Unknown");
            }
        } else {
            SetVar("knx_dev_manufacturer_name", "Unknown");
        }
    } else {
        SetVar("knx_dev_manufacturer_name", "");
    }
}

void
SensorCreateNetworkCache(NetworkList_t *network, void *xo)
{
    FILE *f;
    char file_path[PATH_MAX];

    // Remove the network from cache
    if (!Operating) {
        snprintf (file_path, PATH_MAX, "%s/networks/%s.xml", getenv(DRIVER_DATA_DIR), network->name);
    } else {
        snprintf(file_path, PATH_MAX, "%s/usr/data/%s/networks/%s.xml", rootactPath, GetAdaptorName(), network->name);
    }

    f = fopen(file_path, "w");
    if (f == NULL) {
        RTL_TRDBG(2, "Can't create cache for network '%s'\n", network->name);
        return;
    }

    if (xo != NULL) {
        char *buffer = NULL;
        void *w = XoWritXmlMem(xo, 0, &buffer, "");
	    if (w == NULL) {
		    RTL_TRDBG(0,"ERROR cannot serialize (XML) xo '%s'\n", XoNmType(xo));
		    return;
	    }
	    int sz = strlen(buffer);
	    XoWritXmlFreeCtxt(w);
        fwrite(buffer, sz, 1, f);
    } else {
        const char *emptyTopo = "<?xml version=\"1.0\" encoding=\"utf-8\"?><knx xmlns=\"http://uri.actility.com/m2m/adaptor-knx\" />";
        fwrite(emptyTopo, strlen(emptyTopo), 1, f);
    }
    
    fclose(f);
    
    // Network cache updated, invalidate M2M Descriptor to force update it
    
}

/*
 *  Display the list of network on the CLI
 */
void
SensorDisplayNetworksListOnCli(t_cli *cl)
{
    struct list_head *index;
    NetworkList_t *network;
    
    AdmPrint(cl, "List of networks :\n");
    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);
        AdmPrint(cl, "- %s\n", network->name);
    }
}


/*
 *  Send DiaNetStart for each network in the NetworkList, creating of
 *  Device, Application, and interface will be chained.
 *  
 *  Start all networks in the same time
 */
void
SensorStartupNetworks(void)
{
    struct list_head *index;
    NetworkList_t *network;
    
    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);

        if (network->isInit == true) {
            continue;
        }
        
        RTL_TRDBG(2, "Starting network '%s' creation state-machine\n", network->name);
        SensorDiaCreateNext(network);
    }
}

// Search network from name
NetworkList_t *
SensorFindNetworkFromName(char *name)
{
    struct list_head *index;
    NetworkList_t *network;

    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);

        if (strcmp(network->name, name) == 0) {
            return network;
        }
    }
    
    return NULL;
}

/*
 *  Create a network, or return it if already exists
 */
NetworkList_t *
SensorCreateNetwork(char *name)
{
    NetworkList_t *network;

    // Ensure name is not empty or too big
    int len = strlen(name);
    if (len == 0 || len > NAME_MAX) {
        return NULL;
    }

    network = SensorFindNetworkFromName(name);
    if (network != NULL) {
        return network;
    }

    // Allocate a new network
    network = (NetworkList_t *) malloc (sizeof(NetworkList_t));
    memset(network, 0, sizeof(NetworkList_t));
    strncpy(network->name, name, NAME_MAX);
    network->xo = NULL;
    
    // Create the network DIA context
    network->device = SensorCreateDevice(DEVICE_TYPE_NETWORK);
    network->device->network = network;
    strncpy(network->device->cp_ieee, network->name, CMN_IEEE_SIZE);
    network->isInit = false;
    INIT_LIST_HEAD(&(network->groupApp));
    INIT_LIST_HEAD(&(network->deviceApp));
    
    // Append the network to the global network list
    list_add(&(network->list), &(NetworkList.list));
    
    return network;
}


int
SensorDeleteNetworkFromName(char *name)
{
    NetworkList_t *network = SensorFindNetworkFromName(name);
    if (network == NULL) {
        return -1;
    }

    // Remove M2M
    SensorDeleteNetwork(network);
    
    return 0;
}

void
SensorDeleteNetwork(NetworkList_t *network)
{
    char file_path[PATH_MAX];
    struct list_head *index;
    t_sensor *cp;
    
    // Stop EIBD
    EibdCleanup(network);
    
    // Destroy ETS Xo
    if (network->xo != NULL) {
        XoFree(network->xo, 1);
    }

    // Remove the network from cache
    if (!Operating) {
        snprintf (file_path, PATH_MAX, "%s/networks/%s.xml", getenv(DRIVER_DATA_DIR), network->name);
    } else {
        snprintf(file_path, PATH_MAX, "%s/usr/data/%s/networks/%s.xml", rootactPath, GetAdaptorName(), network->name);
    }
    unlink(file_path);
    
    // Mark all APP to be delete
    list_for_each(index, &(network->deviceApp)) {
        cp = list_entry(index, t_sensor, list);   
        cp->toDelete = true;
    }
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   
        cp->toDelete = true;
    }
    network->device->toDelete = true;
    
    // Start the M2M delete state machine
    SensorDiaDeleteNext(network);
}

void
SensorFreeNetwork(NetworkList_t *network)
{
    SensorDeleteDevice(network->device);
    XoFree(network->xo, 1);
    free(network);
}

/*
 *  Also known as What next ?
 *  State machine to destroy M2M Application on ONG
 *
 *  @return the number of delete request created
 */
int
SensorDiaDeleteNext(NetworkList_t *network)
{
    struct list_head *index;
    t_sensor *cp;

    /*
     *  GroupAddr application and associated AR
     */
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   
        
        if (cp->toDelete == false) {
            continue;
        }
        
        if (cp->isAppInit == true) {
            DiaAppDelete(cp->cp_num, cp->cp_serial, cp->deviceType);
            return 1;
        }
        if (cp->isARInit == true) {
            DiaAccDelete(cp->cp_num, cp->cp_serial, cp->deviceType, 0);
            return 1;
        }
        if (cp->isAR2Init == true) {
            DiaAccDelete(cp->cp_num, cp->cp_serial, cp->deviceType, 1);
            return 1;
        }
        if (cp->isAR3Init == true) {
            DiaAccDelete(cp->cp_num, cp->cp_serial, cp->deviceType, 2);
            return 1;
        }
    }

    /*
     *  Physical application and associated AR
     */
    list_for_each(index, &(network->deviceApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->toDelete == false) {
            continue;
        }
        
        if (cp->isAppInit == true) {
            DiaAppDelete(cp->cp_num, cp->cp_serial, cp->deviceType);
            return 1;
        }
        if (cp->isARInit == true) {
            DiaAccDelete(cp->cp_num, cp->cp_serial, cp->deviceType, 0);
            return 1;
        }
        if (cp->isAR2Init == true) {
            DiaAccDelete(cp->cp_num, cp->cp_serial, cp->deviceType, 1);
            return 1;
        }
        if (cp->isAR3Init == true) {
            DiaAccDelete(cp->cp_num, cp->cp_serial, cp->deviceType, 2);
            return 1;
        }
    }

    /*
     *  GroupAddr device
     */
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->toDelete == false) {
            continue;
        }
        
        if (cp->isInit == true) {
            DiaDevDelete(cp->cp_num, cp->cp_serial);
            return 1;
        }
    }
    
    /*
     *  Physical device
     */
    list_for_each(index, &(network->deviceApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->toDelete == false) {
            continue;
        }
        
        if (cp->isInit == true) {
            DiaDevDelete(cp->cp_num, cp->cp_serial);
            return 1;
        }
    }

    /*
     *  Network
     */
    cp = network->device;
    if (cp->toDelete == true) {
        if (cp->isInit == true) {
            DiaNetDeleteWithSerial(cp->cp_num, cp->cp_serial, networkDiaLookup);
            return 1;
        }
    }
    
    // The IPU is update in the delete OK callback of network
    return 0;
}

/*
 *  Also known as What next ?
 *  State machine for creation of M2M Application on ONG
 *
 *  @return the number of create request created
 */
int
SensorDiaCreateNext(NetworkList_t *network)
{
    struct list_head *index;
    t_sensor *cp;
    
    /*
     *  Network to create ?
     */
    cp = network->device;
    if (cp->isInit == false) {
        DiaNetStartWithSerial(cp->cp_num, cp->cp_serial, networkDiaLookup);
        return 1;
    }
    
    /*
     *  Physical device to create ?
     */
    list_for_each(index, &(network->deviceApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->isInit == false) {
            DiaDevStart(cp->cp_num, cp->cp_serial);
            return 1;
        }
    }

    /*
     *  GroupAddr device to create ?
     */
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->isInit == false) {
            DiaDevStart(cp->cp_num, cp->cp_serial);
            return 1;
        }
    }
    
    /*
     *  Physical application to create, or associated AR ?
     */
    list_for_each(index, &(network->deviceApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->isARInit == false) {
            DiaAccStart(cp->cp_num, cp->cp_serial, cp->deviceType, 0);
            return 1;
        }
        if (cp->isAR2Init == false) {
            DiaAccStart(cp->cp_num, cp->cp_serial, cp->deviceType, 1);
            return 1;
        }
        if (cp->isAR3Init == false) {
            DiaAccStart(cp->cp_num, cp->cp_serial, cp->deviceType, 2);
            return 1;
        }
        if (cp->isAppInit == false) {
            DiaAppStart(cp->cp_num, cp->cp_serial, cp->deviceType);
            return 1;
        } 
    }
    
    /*
     *  GroupAddr application to create, or associated AR ?
     */
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->isARInit == false) {
            DiaAccStart(cp->cp_num, cp->cp_serial, cp->deviceType, 0);
            return 1;
        }
        if (cp->isAR2Init == false) {
            DiaAccStart(cp->cp_num, cp->cp_serial, cp->deviceType, 1);
            return 1;
        }
        if (cp->isAR3Init == false) {
            DiaAccStart(cp->cp_num, cp->cp_serial, cp->deviceType, 2);
            return 1;
        }
        if (cp->isAppInit == false) {
            DiaAppStart(cp->cp_num, cp->cp_serial, cp->deviceType);
            return 1;
        }
    }
    
    /*
     *  Container for GroupAddr application to create ?
     */
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   

        if (cp->isCntInit == false) {
            DiaDatStart(cp->cp_num, cp->cp_serial, cp->deviceType, 0, 0, 0);
            return 1;
        }
    }
    
    return 0;
}

/*
 *  Executed after a DiaNetOk
 *  Parse Xo object to create all M2M Application.
 *  This function create lot of t_sensor which be created on
 *  the ONG after, it's a async operation.
 */
void
SensorInitializeNetwork(NetworkList_t *network, void *newXo)
{
    int i, j, k, addr, knxId, ret;
    struct list_head *index;
    void *xo_device, *xo_object, *xo_groupaddr, *tmp;
    int nb_device, nb_object, nb_groupaddr;
    t_sensor *cp;
    
    // Nothing todo 
    if (newXo == NULL) {
        if (network->xo != NULL) {
            XoFree(network->xo, 1);
        }
        network->xo = NULL;
        return;    
    }

    // Mark all M2M App to must be delete
    // Application not used in newXo will be delete
    list_for_each(index, &(network->deviceApp)) {
        cp = list_entry(index, t_sensor, list);   
        cp->toDelete = true;
    }
    list_for_each(index, &(network->groupApp)) {
        cp = list_entry(index, t_sensor, list);   
        cp->toDelete = true;
    }

    // Iterate over physical KNX device
    nb_device = XoNmNbInAttr(newXo, "knx:device", 0);
    RTL_TRDBG(2, "\t%d devices\n", nb_device);
    for	(i=0; i<nb_device; i++) {
        t_sensor *cp_device = NULL;
        
        xo_device = XoNmGetAttr(newXo, "knx:device[%d]", 0, i);
        if (xo_device == NULL) {
            continue;
        }
        
        // Get an IEEE Addr based on the Device Addr on Network (Unique per Network)
        int a, b, c;
        tmp = XoNmGetAttr(xo_device, "knx:addr", 0);
        if (tmp == NULL) {
            continue;
        }
        ret = sscanf(tmp, "%u.%u.%u", &a, &b, &c);
        if (ret != 3) {
            continue;
        }
        knxId = (a << 11) + (b << 8) + c;
        
        // Search DIA context for the device in the network cache
        list_for_each(index, &(network->deviceApp)) {
            t_sensor *cp = list_entry(index, t_sensor, list);   

            if (cp->knxId == knxId) {
                cp_device = cp;
                break;
            }
        }
        
        if (cp_device == NULL) {
            // Create a DIA context for the device in the network cache
            cp_device = SensorCreateDevice(DEVICE_TYPE_REAL_DEVICE);
            if (cp_device == NULL) {
                continue;
            }
            list_add(&(cp_device->list), &(network->deviceApp));
        }
        cp_device->toDelete = false;
        cp_device->isInit = false;      // DEV_*
        cp_device->isAppInit = false;   // APP_*
        cp_device->xo = xo_device;
        cp_device->network = network;
        cp_device->knxId = knxId;
        snprintf(cp_device->cp_ieee, CMN_IEEE_SIZE, "%s_dev_%d_%d_%d", network->name, a, b, c);
        
        /*
         *  Iterate over object to list all group addr used by this device
         */
        nb_object = XoNmNbInAttr(xo_device, "knx:object", 0);
        RTL_TRDBG(2, "\t%d objects\n", nb_object);
        for	(j=0; j<nb_object; j++) {
            xo_object = XoNmGetAttr(xo_device, "knx:object[%d]", 0, j);
            if (xo_object == NULL) {
                continue;
            }

            // Ensure that the DPT-ID of this object must be report
            tmp = (char *) XoNmGetAttr(xo_object, "knx:dptId", 0);
            if(tmp == NULL) {
                continue;
            }
            Dpt_t dpt = DptAsciiToBin(tmp);
            if (covDptEnable(dpt) == false) {
                continue;
            }

            nb_groupaddr = XoNmNbInAttr(xo_object, "knx:groupAddr", 0);
            RTL_TRDBG(2, "\t%d groupAddrs\n", nb_groupaddr);
            for	(k=0; k<nb_groupaddr; k++) {
                t_sensor *cp_group = NULL;
                xo_groupaddr = XoNmGetAttr(xo_object, "knx:groupAddr[%d]", 0, k);
                if (xo_groupaddr == NULL) {
                    continue;
                }
                
                // GroupAddr are shared between multiple device, ensure to don't create duplicate t_sensor
                tmp = XoNmGetAttr(xo_groupaddr, "knx:addr", 0);
                addr = atoi(tmp);
                list_for_each(index, &(network->groupApp)) {
                    t_sensor *cp = list_entry(index, t_sensor, list);                        
                    if (addr == cp->knxId) {
                        cp_group = cp;
                        break;
                    }
                }
                
                if (cp_group == NULL) {
                    cp_group = SensorCreateDevice(DEVICE_TYPE_GROUP_ADDR_DEVICE);
                    if (cp_group == NULL) continue;
                    
                    /*
                     *  Store the device to create in the Network Ctx
                     *  Create them later, to avoid burst on DIA
                     */
                    list_add(&(cp_group->list), &(network->groupApp));
                    
                    RTL_TRDBG(2, "Group Addr %d created\n", addr);
                } else {
                    RTL_TRDBG(2, "Group Addr %d already exist, skip creation\n", addr);
                }
                cp_group->toDelete = false;
                cp_group->isInit = false;       // DEV_*
                cp_group->isAppInit = false;    // APP_*
                cp_group->xo = xo_groupaddr;
                cp_group->network = network;
                cp_group->knxId = addr;
                cp_group->dpt = dpt;
                snprintf(cp_group->cp_ieee, CMN_IEEE_SIZE, "%s_grp_%d_%d_%d", network->name,
                            ((cp_group->knxId) >> 11) & 0x1f,
                            ((cp_group->knxId) >> 8) & 0x07,
                            (cp_group->knxId) & 0xff);
                cp_group->dptId = dpt;
                    
                cp_group->groupAddrName = XoNmGetAttr(xo_groupaddr, "knx:name", 0);
                if (cp_group->groupAddrName == NULL) {
                    cp_group->groupAddrName = "";
                }
                    
                cp_group->groupAddrSend = false;
                tmp = XoNmGetAttr(xo_groupaddr, "knx:send", 0);
                if (tmp != NULL) {
                    if (!strcmp("true", tmp)) {
                        cp_group->groupAddrSend = true;
                    }
                }
                    
                // Init report ctx
                cp_group->report.xo = NULL;
                cp_group->report.waitingDia = false;
                cp_group->report.time = 0;
                cp_group->report.lastTxTime = 0;
                covGetTiming(dpt, &(cp_group->report.minInterval), &(cp_group->report.maxInterval));
            }
        }
    }
    network->xo = newXo; // Backup Xo
    network->device->isInit = false;    // Must refresh the network DESCRIPTOR
    
    /*
     *  Delete all device / group not used any more
     */
    SensorDiaDeleteNext(network);

    /*
     *  Start a EIBD handler for this network
     */
    EibdCleanup(network);   
    char *eibd_uri = XoNmGetAttr(network->xo, "knx:gateway", 0);
    if (eibd_uri != NULL) {
        RTL_TRDBG(1, "Connect to KNX gateway: '%s'\n", eibd_uri);
        EibdInit(eibd_uri, network);
    } else {
        RTL_TRDBG(0, "Can't read network gateway, don't start EIBD\n");
        EibdSetNetworkNotReachable(network);
    }
}

/*
 *  Load a network definition from an XML file
 *  Return a Xo object
 */
void *
SensorLoadNetworkFromXML(char *networkName)
{
	void *o;
	int	parse;
    char file_path[PATH_MAX];
    
    if (!Operating) {
        snprintf (file_path, PATH_MAX, "%s/networks/%s.xml", getenv(DRIVER_DATA_DIR), networkName);
    } else {
        snprintf(file_path, PATH_MAX, "%s/usr/data/%s/networks/%s.xml", rootactPath, GetAdaptorName(), networkName);
    }
    
	if (access(file_path, R_OK) != 0) {
		RTL_TRDBG(2, "Network definition file does not exist: %s\n", file_path);
		return NULL;
	}

	o = XoReadXmlEx(file_path, NULL, 0, &parse);
	if (o == NULL) {
		RTL_TRDBG(0, "ERROR ret=%d cannot read xml '%s'\n", parse, file_path);
		return NULL;
	}

	return o;
}

void
SensorInitNetworkList(void)
{
    // Init the linked list
    INIT_LIST_HEAD(&(NetworkList.list));
}

/*
 *  Scan the network folder to load ETS projects
 *  Create the NetworkList, and create Xo object for each XML file
 *
 *  Use by admcli to bootstrap the driver without operation on IPU
 */
void
SensorLoadNetworksFromFiles(void)
{
    void *dir;
    char *file;
    char dir_path[PATH_MAX];

    if (!Operating) {
        snprintf (dir_path, PATH_MAX, "%s/networks", getenv(DRIVER_DATA_DIR));
    } else {
        snprintf(dir_path, PATH_MAX, "%s/usr/data/%s/networks", rootactPath, GetAdaptorName());
    }
    
    // Ensure folder exist
    rtl_mkdirp(dir_path);

    dir = rtl_openDir(dir_path);
    if (dir == NULL) {
        RTL_TRDBG(0, "Can't open folder : %s (skip network listing)\n", dir_path);
        return;
    }

    RTL_TRDBG(0, "Scanning NetworkList\n");
    while ((file = rtl_readDir(dir)) != NULL) {
        char *suffix = rtl_suffixname(file);
        void *xo = NULL;
        if (!strcmp(".xml", suffix)) {
            char name[NAME_MAX];
            sprintf(name, "%.*s", strlen(file) - strlen(suffix), file);
            
            NetworkList_t *network = SensorCreateNetwork(name);
            if (network == NULL) {
                continue;
            }
                       
            xo = SensorLoadNetworkFromXML(network->name);
            if (xo == NULL) {
                RTL_TRDBG(0, "Can't parse network definition from %s/%s.xml (ignore)\n", dir_path, network->name);
                list_del(&(network->list));
                free (network);
                continue;
            }

            SensorInitializeNetwork(network, xo);
            RTL_TRDBG(0, "\t%s\n", network->name);
        }
    }
    RTL_TRDBG(0, "Scan NetworkList done\n");
    rtl_closeDir(dir);
}

/*
 *  Create DIA context for each M2M Applications
 */
t_sensor *
SensorCreateDevice(int type)
{
	int i;
	int room = -1;
	t_sensor *cp;

	// find a room
	for	(i=0; i<MAX_SSR_PER_RTR; i++) {
		if	(TbSensor[i] == NULL) {
			room = i;
			break;
		}
	}
	if (room < 0) {
		RTL_TRDBG(0, "max sensors reached (%d) !!!\n", MAX_SSR_PER_RTR);
		return NULL;
	}

	cp = (t_sensor *) malloc(sizeof(t_sensor));
	if (!cp) {
		RTL_TRDBG(0, "malloc failure\n");
		return NULL;
	}
	memset (cp, 0, sizeof(t_sensor));
	
	cp->cp_self	= (void *)cp;
	cp->cp_num = room;
	cp->cp_serial = DoSerial();
	cp->deviceType = type;
	cp->isInit = false;
	cp->isAppInit = false;
	cp->isCntInit = false;
	cp->knxId = 0;
    
	TbSensor[room] = cp;
	NbSensor++;
	
	RTL_TRDBG(1, "create device %p type=%d room=%d (%d)\n", cp, type, room, NbSensor);
	
	return cp;
}

void
SensorDeleteDevice(t_sensor *cp)
{
    int room = cp->cp_num;
    TbSensor[room] = NULL;
    NbSensor--;
    
    free(cp);
}

/**
 * Tells if the network list is empty or not.
 * @return 1 if the network list is empty, 0 otherwise.
 */
int
SensorNetworksListEmpty()
{
  return list_empty(&(NetworkList.list));
}

