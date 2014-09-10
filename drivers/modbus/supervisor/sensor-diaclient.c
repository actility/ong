#include "modbus.h"

/*
 *  DIA Client Callbacks
 */

int
WDiaUCBPreLoadTemplate_DiaNetInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
  Network_t *network = container_of(cmn, Network_t, cp_cmn);
  
  if (network->type == NETWORK_SERIAL) {
    SetVar("w_nettype", "serial");
    SetVar("w_netuart", network->uart);
  } else
  if (network->type == NETWORK_ETHERNET) {
    SetVar("w_nettype", "ethernet");
  } else {
    SetVar("w_nettype", "");
  }

  return 0;
}

int
WDiaUCBPreLoadTemplate_DiaDevInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);

  templateSetDevice(device);
  
  return 0;
}

int
WDiaUCBPreLoadTemplate_DiaAppInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);

  templateSetDevice(device);
  
  return 0;
}

int
WDiaUCBPreLoadTemplate_DiaRepInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
  struct list_head *index;
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);

  list_for_each(index, &(device->cnts)) {
    Point_t *pt = list_entry(index, Point_t, list);   

    if (pt->rq_cluster == preq->rq_cluster &&
        pt->rq_attribut == preq->rq_attribut &&
        pt->rq_member == preq->rq_member) {

      *xo = pt->xo;
      pt->xo = NULL;
      return 0;
    }
  }

  RTL_TRDBG(0, "Can't find Point (0x%hX.0x%hX.%u.m2m), report data will be lost.\n",
    preq->rq_cluster, preq->rq_attribut, preq->rq_member);
 
  return 0;
}

int
WDiaUCBPreLoadTemplate_DiaDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ)
{
  char cnum[32];
  
  snprintf(cnum, sizeof(cnum), "0x%04x.0x%04x.%d.m2m", preq->rq_cluster, preq->rq_attribut, preq->rq_member);
  SetVar("c_num", cnum);

  return 0;
}

/**
 * @brief Call back implementation of the dIa request formatting pre-processing.
 *    This is a generic callback, shared by all requests
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
  if (cmn != NULL) {
    SetVar("d_ieee", cmn->cmn_ieee);
    
    if (cmn->cmn_num == 1) {
      SetVar("w_netid", cmn->cmn_ieee);
    }
  }
  
  return 0;
}


int
WDiaUCBPostLoadTemplate_DiaAppInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);

  CHECK_IS_OBIX(xo);
  
  RTL_TRDBG(3, "Call ApplicationITF(%p, %p, %p, %p)\n", xo, device->product, MdlCfg, device);
  ApplicationITF(xo, *(device->xo), MdlCfg, device);
 
  return 0; 
}

int
WDiaUCBPostLoadTemplate_DiaAppElemCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  // Add ETSI/InterfaceID search string
  char tmp[256];
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  void *xoProduct = NULL;
  void *xoModeling = NULL;
  int iItf, nbItf;
  int iModel, nbModel;
  void *interface;
  char *interfaceName;
  char itfFilter[1024];
  void *o;
  void *filter;
  int iSrchStr, nbSrchStr;
  void *srchStr;
  void *srchStrElem;
  int found;

  if (!device) return 1;

  xoProduct = *(device->xo);
  xoModeling = MdlCfg;

  if (!xoProduct || !xoModeling) return 1;

  nbItf = XoNmNbInAttr(xoProduct, "modbus:interfaces", 0);
  nbModel = XoNmNbInAttr(xoModeling, "ong:interfaces", 0);

  for (iItf=0; iItf<nbItf; iItf++)
  {
    interface = XoNmGetAttr(xoProduct, "modbus:interfaces[%d]", 0, iItf);
    if (!interface) continue;
    interfaceName = (char *) XoNmGetAttr(interface, "modbus:id.value$", 0);
    snprintf(itfFilter, sizeof(itfFilter), "modbus:/int/%s", interfaceName);

    // Test if the interfaceID is in modeling, if not ignore this interface
    for (iModel=0; iModel<nbModel; iModel++) 
    {
      o = XoNmGetAttr(xoModeling, "ong:interfaces[%d]", NULL, iModel);
      if (!o) continue;

      filter = XoNmGetAttr(o, "ong:filter", NULL, 0);
      if (!filter) continue;
      
      if (strcmp(filter, itfFilter) == 0) 
      {
        sprintf(tmp, "ETSI.InterfaceID/%s", interfaceName);
        
        // look in the search strings if the entry already exists
        nbSrchStr = XoNmNbInAttr(xo, "m2m:searchStrings.m2m:searchString", 0);
        found = 0;
        for (iSrchStr = 0; iSrchStr<nbSrchStr && !found; iSrchStr++)
        {
          srchStrElem = (char*)XoNmGetAttr(xo, "m2m:searchStrings.m2m:searchString[%d]", 
            0, iSrchStr);
          if (!srchStrElem) continue;

          if (!strcmp(tmp, srchStrElem))
          {
            found = 1;
          }
        }
        if (!found)
        {
          XoNmAddInAttr(xo, "m2m:searchStrings.m2m:searchString", tmp, 0);
        }
        break;
      }
    }
  }

  return 0;
}

int
WDiaUCBPostLoadTemplate_DiaDevInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  char href[4096];
  
  CHECK_IS_OBIX(xo);
  
  /*
   *  Each device have only one application, the application code is based on the deviceType
   */
  void *dev = XoNmNew("o:ref");
  if (dev == NULL) {
    RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
    return 1;
  }
  XoNmSetAttr(dev, "name$", "ref", 0);
  snprintf(href, sizeof(href), "%s/" SCL_APP "%s.%d/containers/DESCRIPTOR/contentInstances/latest/content", GetVar("s_rootapp"), device->cp_ieee, 0);
  XoNmSetAttr(dev, "href", href, 0);
  XoNmAddInAttr(xo, "[name=applications].[]", dev, 0, 0);
  return 0;
}

int
WDiaUCBPostLoadTemplate_DiaNetInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  Network_t *network = container_of(cmn, Network_t, cp_cmn);
  Sensor_t *device;
  struct list_head *index;
  char href[4096];
  
  CHECK_IS_OBIX(xo);
  
  // Remove network specific attr
  if (network->type != NETWORK_SERIAL) {
    XoNmSupInAttr(xo, "wrapper$[name=modbusNetworkUART]", 0);
  }
  
  // Add nodes
  list_for_each(index, &(network->device)) {
    device = list_entry(index, Sensor_t, list);
    
    void *dev = XoNmNew("o:ref");
    if (dev == NULL) {
      RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
      break;
    }
    snprintf(href, sizeof(href), "%s/" SCL_DEV "%s/containers/DESCRIPTOR/contentInstances/latest/content", GetVar("s_rootapp"), device->cp_ieee);
    XoNmSetAttr(dev, "href", href, 0);
    XoNmAddInAttr(xo, "[name=nodes].[]", dev, 0, 0);
  }
  return 0;
}

int
WDiaUCBPostLoadTemplate_DiaIpuInstCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  struct list_head *index;
  Network_t *network;
  char href[4096];
  
  CHECK_IS_OBIX(xo);

  list_for_each(index, &(NetworkInternalList)) {
    network = list_entry(index, Network_t, list);
    
    void *dev = XoNmNew("o:ref");
    if (dev == NULL) {
      RTL_TRDBG(0, "ERROR cannot allocate 'o:ref'\n");
      break;
    }
    snprintf(href, sizeof(href), "%s/NW_%s/containers/DESCRIPTOR/contentInstances/latest/content", GetVar("s_rootapp"), network->name);
    XoNmSetAttr(dev, "href", href, 0);
    XoNmAddInAttr(xo, "[name=networks].[]", dev, 0, 0);
  }
  return 0;
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
  static bool isFirst = true;
  
  // Create the products cnt
  DiaProductsDatStart();
  
  // Load cache
  if (isFirst) {
    isFirst = false;
    DriverLoadNetworksCache();
  }
}

void
WDiaUCBRequestOk_DiaNetOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  Network_t *network = container_of(cmn, Network_t, cp_cmn);
  
  network->isInit = true;
  NetworkEnsureDeviceAreInit(network);
  DriverUpdateNetworksCache();
}

void
WDiaUCBRequestOk_DiaDevOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  NOT_USED(preq);
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  device->isInit = true;
  DeviceDiaCreateNext(device);
  DriverUpdateNetworksCache();
}

void
WDiaUCBRequestOk_DiaAppOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  NOT_USED(preq);
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  device->isAppInit = true;
  DeviceDiaCreateNext(device);
}

void
WDiaUCBRequestOk_DiaAccOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  switch(preq->rq_access) {
    case 0:
      device->isARInit = true;
      break;
      
    case 1:
      device->isAR2Init = true;
      break;
      
    case 2:
      device->isAR3Init = true;
      break;
      
    default:
      ;
  }
  
  DeviceDiaCreateNext(device);
}

void
WDiaUCBRequestOk_DiaDatOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  struct list_head *index;
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);

  list_for_each(index, &(device->cnts)) {
    Point_t *pt = list_entry(index, Point_t, list);   

    if (pt->rq_cluster == preq->rq_cluster &&
        pt->rq_attribut == preq->rq_attribut &&
        pt->rq_member == preq->rq_member) {

      pt->isInit = true;
      RTL_TRDBG(0, "CREATED Point (0x%hX.0x%hX.%u.m2m) = %p\n", preq->rq_cluster, 
        preq->rq_attribut, preq->rq_member, pt);
       
      DeviceDiaCreateNext(device);
      return;
    }
  }
  RTL_TRDBG(0, "Can't find Point (0x%hX.0x%hX.%u.m2m), abort DeviceDiaCreateNext "
    "to avoid infinity loop.\n", preq->rq_cluster, preq->rq_attribut, preq->rq_member);
}

void
WDiaUCBRequestOk_DiaRepOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
}

void
WDiaUCBRequestOk_DiaNetDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  Network_t *network = container_of(cmn, Network_t, cp_cmn);
  
  NetworkDestroy(network);
  DriverUpdateNetworksCache();
}

void
WDiaUCBRequestOk_DiaDevDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  DeviceDestroy(device);
  DriverUpdateNetworksCache();
}

void
WDiaUCBRequestOk_DiaAppDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  device->isAppInit = false;
  DeviceDiaDeleteNext(device);
}

void
WDiaUCBRequestOk_DiaDatDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  struct list_head *index;
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
 
  list_for_each(index, &(device->cnts)) {
    Point_t *pt = list_entry(index, Point_t, list);   
    if (pt->rq_cluster == preq->rq_cluster &&
        pt->rq_attribut == preq->rq_attribut &&
        pt->rq_member == preq->rq_member) {
       
      RTL_TRDBG(0, "DELETED Point (0x%hX.0x%hX.%u.m2m) = %p\n", preq->rq_cluster, 
        preq->rq_attribut, preq->rq_member, pt);
       
      // Remove the Point from device & Free it 
      list_del(&(pt->list));

      if (pt->xo != NULL) {
        XoFree(pt->xo, 1);
      }
      free(pt);
  
      DeviceDiaDeleteNext(device);
      return;
    }
  }
  
  RTL_TRDBG(0, "Can't find Point (0x%hX.0x%hX.%u.m2m), abort DeviceDiaDeleteNext "
    "to avoid more error.\n", preq->rq_cluster, preq->rq_attribut, preq->rq_member);
}

void
WDiaUCBRequestOk_DiaAccDelOk(t_dia_req *preq, t_cmn_sensor *cmn)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  switch(preq->rq_access) {
    case 0:
      device->isARInit = false;
      break;
      
    case 1:
      device->isAR2Init = false;
      break;
      
    case 2:
      device->isAR3Init = false;
      break;
      
    default:
      ;
  }
  
  DeviceDiaDeleteNext(device);
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
WDiaUCBRequestTimeout(char *reqname, t_dia_req *preq, t_cmn_sensor *cmn)
{
  NOT_USED(cmn);
  NOT_USED(preq);
   
  RTL_TRDBG(2, "Enter in %s\n", __FUNCTION__);
   
  /*
   *  Retry
   */
  if (strncmp(reqname, "DiaIpu", 6) == 0) {
    DiaIpuStart();
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
  return 0;
}
