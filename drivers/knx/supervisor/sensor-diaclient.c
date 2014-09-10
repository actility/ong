#include "knx.h"

t_cmn_sensor * networkDiaLookup(int dev, int serial);

/*
 *  DIA Client Callbacks
 *
 *  - WDiaUCBPreLoadTemplate (route to the following callbacks, do by weak symbols now)
 *      - WDiaUCBPreLoadTemplate_DiaDevContCreate
 *      - WDiaUCBPreLoadTemplate_DiaDevElemCreate
 *      - WDiaUCBPreLoadTemplate_DiaDevInstCreate
 *      - WDiaUCBPreLoadTemplate_DiaAppContCreate
 *      - WDiaUCBPreLoadTemplate_DiaAppElemCreate
 *      - WDiaUCBPreLoadTemplate_DiaAppInstCreate
 *      - WDiaUCBPreLoadTemplate_DiaDatElemCreate
 *      - WDiaUCBPreLoadTemplate_DiaRepInstCreate
 *      - WDiaUCBPreLoadTemplate_DiaDatContCreate
 *  - WDiaUCBPostLoadTemplate (route to the following callbacks, do by weak symbols now)
 *      - WDiaUCBPostLoadTemplate_DiaAppInstCreate
 *      - WDiaUCBPostLoadTemplate_DiaDevInstCreate
 *      - WDiaUCBPostLoadTemplate_DiaNetInstCreate
 *      - WDiaUCBPostLoadTemplate_DiaIpuInstCreate
 *  - WDiaUCBRequestTimeout
 *  - WDiaUCBRequestError
 *  - WDiaUCBRequestOk (route to the following callbacks, do by weak symbols now)
 *      - WDiaUCBRequestOk_DiaIpuOk
 *      - WDiaUCBRequestOk_DiaNetOk
 *      - WDiaUCBRequestOk_DiaDevOk
 *      - WDiaUCBRequestOk_DiaAppOk
 *      - WDiaUCBRequestOk_DiaAccOk
 *      - WDiaUCBRequestOk_DiaDatOk
 *      - WDiaUCBRequestOk_DiaRepOk
 *      - WDiaUCBRequestOk_DiaNetDelOk
 *      - WDiaUCBRequestOk_DiaDevDelOk
 *      - WDiaUCBRequestOk_DiaAppDelOk
 *      - WDiaUCBRequestOk_DiaAccDelOk 
 *  -----------------------------------------------------------------------------------------------------
 */

int
WDiaUCBPreLoadTemplate_DiaDevInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    int parseflags = XOML_PARSE_OBIX;
    char buffer[64];
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
     
    templateSetDevice(cp->xo);
     
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        *xo	= WXoNewTemplate("dev_device_inst.xml", parseflags);
    } else if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
    
        snprintf(buffer, sizeof(buffer), "%u/%u/%u", (cp->knxId >> 11) & 0x1f, (cp->knxId >> 8) & 0x07, cp->knxId & 0xff);
        SetVar("knxGroupAddress", buffer);
        
        *xo	= WXoNewTemplate("dev_ga_inst.xml", parseflags);
    } else {
        RTL_TRDBG(0, "Unknown device type : %d\n", cp->deviceType);   
    }
    
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaDevContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    int parseflags = 0;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    templateSetDevice(cp->xo);
    
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        *xo	= WXoNewTemplate("dev_device_cont.xml", parseflags);
    } else if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
        *xo	= WXoNewTemplate("dev_ga_cont.xml", parseflags);
    } else {
        RTL_TRDBG(0, "Unknown device type : %d\n", cp->deviceType);   
    }
    
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaDevElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    int parseflags = 0;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    templateSetDevice(cp->xo);
    
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        *xo	= WXoNewTemplate("dev_device_elem.xml", parseflags);
    } else if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
        *xo	= WXoNewTemplate("dev_ga_elem.xml", parseflags);
    } else {
        RTL_TRDBG(0, "Unknown device type : %d\n", cp->deviceType);   
    }
    
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaAppInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    int parseflags = XOML_PARSE_OBIX;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
     
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        *xo = WXoNewTemplate("app_device_inst.xml", parseflags);
    } else if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
        *xo	= WXoNewTemplate("app_ga_inst.xml", parseflags);
    } else {
        RTL_TRDBG(0, "Unknown device type : %d\n", cp->deviceType);   
    }
    
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaAppContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    int parseflags = 0;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
     
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        *xo = WXoNewTemplate("app_device_cont.xml", parseflags);
    } else if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
        *xo = WXoNewTemplate("app_ga_cont.xml", parseflags);
    } else {
        RTL_TRDBG(0, "Unknown device type : %d\n", cp->deviceType);   
    }
    
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaAppElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    int parseflags = 0;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
     
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        *xo	= WXoNewTemplate("app_device_elem.xml", parseflags);
    } else if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
        templateSetDPT(cp->dpt);
        *xo	= WXoNewTemplate("app_ga_elem.xml", parseflags);
    } else {
        RTL_TRDBG(0, "Unknown device type : %d\n", cp->deviceType);   
    }
    
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaDatElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    return WDiaUCBPreLoadTemplate_DiaAppElemCreate(preq, target, cmn, xo, templ);
}

int
WDiaUCBPreLoadTemplate_DiaRepInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    *xo	= XoNewCopy(cp->report.xo, 1);
    return 0;
}

int
WDiaUCBPreLoadTemplate_DiaDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    char cnum[128];
    
    snprintf(cnum, sizeof(cnum), "0x%04x.0x%04x.%d.m2m", preq->rq_cluster, preq->rq_attribut, preq->rq_member);
    SetVar("c_num", cnum);

    return 0;
}

/**
 * @brief Call back implementation of the dIa request formatting pre-processing.
 *        This is a generic callback, shared by all requests
 * @param reqname name of the dIa request (e.g. DiaAppElemCreate, DiaAppInstCreate...).
 * @param preq pointer on the dIa request structure.
 * @param target targeted URI
 * @param cmn pointer on the common sensor structure
 * @param xo the XO root document that will constitute the request content.
 * @param templatename the name of the template file to use.
 * @return always 0...
 */
int
WDiaUCBPreLoadTemplate(char *reqname, t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
    char buffer[64];
    
    if (cmn != NULL) {
        t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
        
        SetVar("d_ieee", cmn->cmn_ieee);

        snprintf(buffer, sizeof(buffer), "%u", cp->deviceType);
        SetVar("a_num", buffer);

        if (cp->network != NULL) {
            SetVar("w_netid", cp->network->name);
        }
    }
    
    return 0;
}

static
void
appendGroupInterface(t_sensor *cp, void *dst)
{
    int parseflags = XOML_PARSE_OBIX;
    void *groupAddrDptInterface, *groupAddrCommonInterface;
    char *templateName;
    int i, nb;
    
    // Ensure that the DPT-ID of this group must be report
    if (covDptEnable(cp->dpt) == false) {
        return;
    }
    
    templateSetDPT(cp->dpt);

    templateName = DptGetInterfaceTemplateFromCode(cp->dpt);
    groupAddrDptInterface = WXoNewTemplate(templateName, parseflags);
    if (groupAddrDptInterface == NULL) {
        RTL_TRDBG(0, "cannot parse %s\n", templateName);
        return;
    }
    
    templateName = "itf_inst_ga_common.xml";
    groupAddrCommonInterface = WXoNewTemplate(templateName, parseflags);
    if (groupAddrCommonInterface == NULL) {
        RTL_TRDBG(0, "cannot parse %s\n", templateName);
        return;
    }

    // Merge
    nb = XoNmNbInAttr(groupAddrCommonInterface, "[]", 0);
    for (i=0; i<nb; i++) {
        void *var = XoNmGetAttr(groupAddrCommonInterface, "[%d]", 0, i);
        if (var == NULL) {
            continue;
        }

        void *var2 = XoNewCopy(var, 1);
        XoNmAddInAttr(groupAddrDptInterface, "[]", var2, 0, 0);
    }
    XoFree(groupAddrCommonInterface, 1);
    
    // Insert into APP inst
    XoNmAddInAttr(dst, "[name=interfaces].[]", groupAddrDptInterface, 0, 0);
}

static
void
appendObjectInterface(t_sensor *cp, void *dst, void *xo_object)
{
    int parseflags = XOML_PARSE_OBIX;
    void *objectDptInterface, *objectCommonInterface, *xo_groupaddr, *tmp;
    char *templateName;
    int i, addr;
    int nb_groupaddr;
    
    char buffer[64];
    
    // Ensure that the DPT-ID of this object must be report
    tmp = (char *) XoNmGetAttr(xo_object, "knx:dptId", 0);
    if(tmp == NULL) {
        return;
    }
    Dpt_t dpt = DptAsciiToBin(tmp);
    if (covDptEnable(dpt) == false) {
        return;
    }
    
    templateSetDPT(dpt);
    templateSetObject(xo_object);
    SetVar("a_num", xstr(DEVICE_TYPE_GROUP_ADDR_DEVICE));

    templateName = "itf_inst_device_common.xml";
    objectCommonInterface = WXoNewTemplate(templateName, parseflags);
    if (objectCommonInterface == NULL) {
        RTL_TRDBG(0, "cannot parse %s\n", templateName);
        return;
    }
    
    // All groups in the same object have the same Dpt-ID, so the same interface template
    templateName = DptGetInterfaceTemplateFromCode(dpt);
    
    nb_groupaddr = XoNmNbInAttr(xo_object, "knx:groupAddr", 0);
    for	(i=0; i<nb_groupaddr; i++) {
        xo_groupaddr = XoNmGetAttr(xo_object, "knx:groupAddr[%d]", 0, i);
        if (xo_groupaddr == NULL) {
            continue;
        }
        
        // Restore the d_ieee of this group
        tmp = (char *) XoNmGetAttr(xo_groupaddr, "knx:addr", 0);
        if (tmp == NULL) {
            continue;
        }
        addr = atoi(tmp);
        snprintf(buffer, sizeof(buffer), "%s_grp_%d_%d_%d", cp->network->name,
                                ((addr) >> 11) & 0x1f,
                                ((addr) >> 8) & 0x07,
                                (addr) & 0xff);
        SetVar("d_ieee", buffer);

        objectDptInterface = WXoNewTemplate(templateName, parseflags);
        if (objectDptInterface == NULL) {
            RTL_TRDBG(0, "cannot parse %s\n", templateName);
            return;
        }
        
        // Add tag knxGroupAddress
        snprintf(buffer, sizeof(buffer), "%u/%u/%u", (addr >> 11) & 0x1f, (addr >> 8) & 0x07, addr & 0xff);
        void *sub = XoNmNew("o:str");
        XoNmSetAttr(sub, "name", "knxGroupAddress", 0, 0);
        XoNmSetAttr(sub, "val", buffer, 0, 0);
        XoNmAddInAttr(objectDptInterface, "wrapper$", sub, 0, 0);
        
        XoNmAddInAttr(objectCommonInterface, "[name=knxGroups].[]", objectDptInterface, 0, 0);
    }

    // Insert into APP inst
    XoNmAddInAttr(dst, "[name=interfaces].[]", objectCommonInterface, 0, 0);
}

void
WDiaUCBPostLoadTemplate_DiaAppInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    int i, nb;
    void *xo_object;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);

    CHECK_IS_OBIX(xo);

    if (cp->deviceType == DEVICE_TYPE_GROUP_ADDR_DEVICE) {
        /*
         *  Virtual group have always only one interface,
         *  because we create one device by group addr.
         */
        appendGroupInterface(cp, xo);

    } else if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        /*
         *  Device can have multiple interface,
         *  because we create one interface by device object, those objects may include multiple group addr.
         */
        nb = XoNmNbInAttr(cp->xo, "knx:object", 0);
        for (i=0; i<nb; i++) {
            xo_object = XoNmGetAttr(cp->xo, "knx:object[%d]", 0, i);
            if (xo_object == NULL) {
                continue;
            }
            
            appendObjectInterface(cp, xo, xo_object);
        }
    }
}

int
WDiaUCBPostLoadTemplate_DiaAppElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    int i, j, rc, nb;
    void *xo_object;
    char *tmp, buffer[128];
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);

    // Append search string
    if (cp->deviceType == DEVICE_TYPE_REAL_DEVICE) {
        nb = XoNmNbInAttr(cp->xo, "knx:object", 0);
        for (i=0; i<nb; i++) {
            xo_object = XoNmGetAttr(cp->xo, "knx:object[%d]", 0, i);
            if (xo_object == NULL) {
                continue;
            }

            tmp = (char *) XoNmGetAttr(xo_object, "knx:dptId", 0);
            if (tmp == NULL) {
                continue;
            }
            Dpt_t dpt = DptAsciiToBin(tmp);
            if (covDptEnable(dpt) == false) {
                continue;
            }
    
            // Detect duplicate entry
            bool duplicateEntry = false;
            for (j=0; j<i; j++) {
                void *dup_object = XoNmGetAttr(cp->xo, "knx:object[%d]", 0, j);
                if (dup_object == NULL) {
                    continue;
                }

                tmp = (char *) XoNmGetAttr(dup_object, "knx:dptId", 0);
                if (tmp == NULL) {
                    continue;
                }
                Dpt_t dpt2 = DptAsciiToBin(tmp);
                if (covDptEnable(dpt2) == false) {
                    continue;
                }
            
                if (DPT_IS_EQUAL(dpt, dpt2)) {
                    duplicateEntry = true;
                }
            }
            if (duplicateEntry == true) {
                continue;
            }
    
            snprintf(buffer, sizeof(buffer), "KNX.DptID/%u.%03u", dpt.primary, dpt.sub);
            XoNmAddInAttr(xo, "m2m:searchStrings.m2m:searchString", buffer, 0);
            
            DptInfos_t infos[1];
            rc = DptGetInfos(dpt, infos);
            if (rc == 0) {
                snprintf(buffer, sizeof(buffer), "KNX.DptName/%s", infos->name);
                XoNmAddInAttr(xo, "m2m:searchStrings.m2m:searchString", buffer, 0);
                
                snprintf(buffer, sizeof(buffer), "ETSI.InterfaceID/%s.Srv", infos->name);
                XoNmAddInAttr(xo, "m2m:searchStrings.m2m:searchString", buffer, 0);
            }
        }
    }
    
    return 0;
}

void
WDiaUCBPostLoadTemplate_DiaDevInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    void *dev;
    char href[4096];
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);

    CHECK_IS_OBIX(xo);
    
    /*
     *  Each device have only one application, the application code is based on the deviceType
     */
    dev	= XoNmNew("o:ref");
    if (dev == NULL) {
	    RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
	    return;
    }
    XoNmSetAttr(dev, "name$", "ref", 0);
    snprintf(href, sizeof(href), "%s/" SCL_APP "%s.%d/containers/DESCRIPTOR/contentInstances/latest/content", SCL_ROOT_APP, cp->cp_ieee, cp->deviceType);
    XoNmSetAttr(dev, "href", href, 0);
    XoNmAddInAttr(xo, "[name=applications].[]", dev, 0, 0);
}

void
WDiaUCBPostLoadTemplate_DiaNetInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    void *dev;
    char href[4096];
    struct list_head *index;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    t_sensor *cpdev;
    NetworkList_t *network = cp->network;

    CHECK_IS_OBIX(xo);

    // Real devices
    list_for_each(index, &(network->deviceApp)) {
        cpdev = list_entry(index, t_sensor, list);
        
	    dev	= XoNmNew("o:ref");
	    if (!dev) {
		    RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
		    break;
	    }
	    snprintf(href, sizeof(href), "%s/" SCL_DEV "%s/containers/DESCRIPTOR/contentInstances/latest/content", GetVar("s_rootapp"), cpdev->cp_ieee);
	    XoNmSetAttr(dev, "href", href, 0);
	    XoNmAddInAttr(xo, "[name=nodes].[]", dev, 0, 0);
    }
    
    // Group Addr devices
    list_for_each(index, &(network->groupApp)) {
        cpdev = list_entry(index, t_sensor, list);
        
	    dev	= XoNmNew("o:ref");
	    if (!dev) {
		    RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
		    break;
	    }
	    snprintf(href, sizeof(href), "%s/" SCL_DEV "%s/containers/DESCRIPTOR/contentInstances/latest/content", GetVar("s_rootapp"), cpdev->cp_ieee);
	    XoNmSetAttr(dev, "href", href, 0);
	    XoNmAddInAttr(xo, "[name=nodes].[]", dev, 0, 0);
    }    
}

void
WDiaUCBPostLoadTemplate_DiaIpuInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    struct list_head *index;
    NetworkList_t *network;
    char href[4096];
    void *dev;
    
    CHECK_IS_OBIX(xo);

    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);
        
	    dev	= XoNmNew("o:ref");
	    if (!dev) {
		    RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
		    break;
	    }
	    snprintf(href, sizeof(href), "%s/NW_%s/containers/DESCRIPTOR/contentInstances/latest/content", GetVar("s_rootapp"), network->name);
	    XoNmSetAttr(dev, "href", href, 0);
	    XoNmAddInAttr(xo, "[name=networks].[]", dev, 0, 0);
    }
}

/**
 * @brief Call back implementation of the dIa request formatting post-processing.
 * @param reqname name of the dIa request (e.g. DiaAppElemCreate, DiaAppInstCreate...).
 * @param preq pointer on the dIa request structure.
 * @param target targeted URI
 * @param cmn pointer on the common sensor structure
 * @param xo the XO root document that will constitute the request content.
 * @return always 0...
 */
int
WDiaUCBPostLoadTemplate(char *reqname, t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
    return 0;
}

void
WDiaUCBRequestOk_DiaIpuOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    static bool firstPass = true;
    DiaIpuOk++;

    // Ensure containers of IPU are created
    if (firstPass) {
        firstPass = false;

        DiaTopologyDatStart();
    }
}


void
WDiaUCBRequestOk_DiaTopologyDatOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    // Restore previous networks
    SensorLoadNetworksFromFiles();
    SensorStartupNetworks();
}

void
WDiaUCBRequestOk_DiaNetOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    cp->isInit = true;
    DiaNetOk++;

    /*
     *  Update the IPU to add the network
     *  Ensure the IPU already exist and update the DESCRIPTOR
     */
    DiaIpuStart();

    /*
     *  Continue the state-machine
     */
    SensorDiaCreateNext(cp->network);
}

void
WDiaUCBRequestOk_DiaDevOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    cp->isInit = true;
    SensorDiaCreateNext(cp->network);
}

void
WDiaUCBRequestOk_DiaAppOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    cp->isAppInit = true;
    SensorDiaCreateNext(cp->network);
}

void
WDiaUCBRequestOk_DiaAccOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    NOT_USED(preq);
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    if (cp->isARInit == false) {
        cp->isARInit = true;
    } else if (cp->isAR2Init == false) {
        cp->isAR2Init = true;
    } else if (cp->isAR3Init == false) {
        cp->isAR3Init = true;
    }
    
    SensorDiaCreateNext(cp->network);
}

void
WDiaUCBRequestOk_DiaDatOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    cp->isCntInit = true;
    SensorDiaCreateNext(cp->network);
}

void
WDiaUCBRequestOk_DiaRepOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    NOT_USED(preq);
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    cp->report.waitingDia = false;
}

void
WDiaUCBRequestOk_DiaNetDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    NetworkList_t *network = cp->network;
    
    // Remove from network list
    list_del(&(network->list));
    
    // Free the network
    SensorFreeNetwork(network);
    
    // Update the IPU Descriptor
    DiaIpuStart();
}

void
WDiaUCBRequestOk_DiaDevDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    NetworkList_t *network = cp->network;
    
    // AR, APP and DEV have been delete. Delete the device from the network
    list_del(&(cp->list));
    SensorDeleteDevice(cp);
    
    SensorDiaDeleteNext(network);
}

void
WDiaUCBRequestOk_DiaAppDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    cp->isAppInit = false;
    SensorDiaDeleteNext(cp->network);
}

void
WDiaUCBRequestOk_DiaAccDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    if (cp->isARInit == true) {
        cp->isARInit = false;
    } else if (cp->isAR2Init == true) {
        cp->isAR2Init = false;
    } else if (cp->isAR3Init == true) {
        cp->isAR3Init = false;
    }
    
    SensorDiaDeleteNext(cp->network);
}

/**
 * @brief Call back function invoked by drvcommon layer on dIa request success completion.
 * @param reqname the dIa request name (e.g. DiaDatContCreate)
 * @param preq the dIa request that failed.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 * @return always 0.
 */
int
WDiaUCBRequestOk(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
    return 0;
}

/**
 * @brief Call back function invoked by drvcommon layer on dIa request timeout detection.
 * @param reqname the dIa request name (e.g. DiaDatContCreate)
 * @param preq the dIa request that failed.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 * @return always 0.
 */
int
WDiaUCBRequestTimeout(char *reqname,t_dia_req *preq,t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    NOT_USED(preq);
    
    RTL_TRDBG(2, "Enter in %s\n", __FUNCTION__);

    /*
     *  Retry
     */
    if (strncmp(reqname, "DiaIpu", 6) == 0) {
        DiaIpuStart();
    } else
    if (strncmp(reqname, "DiaNet", 6) == 0) {
        DiaNetStartWithSerial(cp->cp_num, cp->cp_serial, networkDiaLookup);
    } else
    if (strncmp(reqname, "DiaDev", 6) == 0) {
        DiaDevStart(cp->cp_num, cp->cp_serial);
    } else
    if (strncmp(reqname, "DiaApp", 6) == 0) {
        DiaAppStart(cp->cp_num, cp->cp_serial, cp->deviceType);
    } else
    if (strncmp(reqname, "DiaDat", 6) == 0) {
        DiaDatStart(cp->cp_num, cp->cp_serial, cp->deviceType, 0, 0, 0);
    } else
    if (strncmp(reqname, "DiaRep", 6) == 0) {
        cp->report.waitingDia = false;  // Auto retry based on sensor-events
    }
    
    return 0;
}

/**
 * @brief Call back function invoked by drvcommon layer on dIa request error detection.
 * @param reqname the dIa request name (e.g. DiaDatContCreate)
 * @param preq the dIa request that failed.
 * @param cmn the sensor on which the request refers to (if relevant, NULL otherwise).
 * @return always 0.
 */
int
WDiaUCBRequestError(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    NOT_USED(preq);
    
    RTL_TRDBG(2, "Enter in %s\n", __FUNCTION__);
    
    if (strncmp(reqname, "DiaRep", 6) == 0) {
        cp->report.waitingDia = false;  // Auto retry based on sensor-events
    }

    return 0;
}
