#include "modbus-acy.h"

/**********************************************************************************************************************
 *  Utils
 */
 
 
t_sensor *
SensorNumSerial(int num, int serial)
{
  return NULL;
}

t_cmn_sensor *
CmnSensorNumSerial(int num, int serial)
{
  return NULL;
}

/*!
 *
 * @brief called by DIA server module to retrieve the common structure of a 
 * sensor with its ieee address. Mainly used for retargeting purpose. (i.e. foo_grp_0_0_6.1)
 * @param ieee sensor ieee address
 * @return :
 *   - the address of the common structure in the sensor found
 *   - NULL if bad ieee address
 * 
 * used by diaserver.c
 */
t_cmn_sensor *
CmnSensorFindByIeee(char *ieee)
{
  RTL_TRDBG(TRACE_DETAIL, "Search cmn with ident='%s'\n", ieee);

  char *sep = index(ieee, '_');
  if (sep == NULL) {
    return NULL;
  }
  
  *sep = '\0';
  Network_t *network = NetworkFindFromName(ieee);
  if (network == NULL) {
    return NULL;
  }
  
  Sensor_t *device = DeviceFindFromName(network, sep + 1);
  if (device == NULL) {
    return NULL;
  }

  return &(device->cp_cmn);
}

t_cmn_sensor *
ModbusDiaLookup(int dev, int serial)
{
  struct list_head *index;
  Network_t *network;
  
  list_for_each(index, &NetworkInternalList) {
    network = list_entry(index, Network_t, list);
    
    if (dev == CP_NUM_NETWORK) {
      t_cmn_sensor *cmn = &(network->cp_cmn);
      if (cmn->cmn_serial == serial) {
        return cmn;
      }
    } else
    if (dev == CP_NUM_DEVICE) {
      struct list_head *index2;
      Sensor_t *device;
      
      list_for_each(index2, &(network->device)) {
        device = list_entry(index2, Sensor_t, list);
        
        t_cmn_sensor *cmn = &(device->cp_cmn);
        if (cmn->cmn_serial == serial) {
          return cmn;
        }
      }
    }
  }
  
  return NULL;
}

/**********************************************************************************************************************
 *  Network tools
 */
Network_t *
NetworkFindFromName(char *name)
{
  struct list_head *index;
  Network_t *network;

  list_for_each(index, &NetworkInternalList) {
    network = list_entry(index, Network_t, list);

    if (strcmp(network->name, name) == 0) {
      return network;
    }
  }
  
  return NULL;
}

Network_t *
NetworkCreate(char *name)
{
  Network_t *network;
  
  // Ensure the network don't exist
  network = NetworkFindFromName(name);
  if (network != NULL) {
    return NULL;
  }
  
  network = (Network_t *) malloc (sizeof(Network_t));
  RTL_TRDBG(TRACE_INFO, "create network %p name=%s\n", network, name);
  
  network->isInit = false;
  network->toDelete = false;
  network->modbusCtx = NULL;
  network->scan = NETWORK_NO_SCAN;
  network->uart[0] = '\0';
  strncpy(network->name, name, NAME_MAX);
  INIT_LIST_HEAD(&(network->device));
  
  // Init DIA ctx
  network->cp_self = &(network->cp_self);
  network->cp_num = CP_NUM_NETWORK;
  network->cp_serial = DoSerial();
  strncpy(network->cp_ieee, name, CMN_IEEE_SIZE);

  // Append the network on the internal list
  list_add(&(network->list), &NetworkInternalList);
  
  return network;
}

Network_t *
NetworkCreateEthernet(char *name)
{
  Network_t *network = NetworkCreate(name);
  if (network == NULL) {
    return NULL;
  }
  
  // Ethernet specific init
  network->type = NETWORK_ETHERNET;
  
  return network;
}

Network_t *
NetworkCreateSerial(char *name, char *modbusUART, char *modbusBaudrate, char *modbusParity, char *modbusDataSize, char *modbusStopSize)
{
  char *endptr;

  // Check inputs
  int baudrate = strtol (modbusBaudrate, &endptr, 10);
  if (*endptr != '\0') {
    RTL_TRDBG(TRACE_ERROR, "Baudrate parameter has wrong format. Must be numeric. "
      "(modbusBaudrate=%s)\n", modbusBaudrate);
    return NULL;
  }
  
  int datasize = strtol (modbusDataSize, &endptr, 10);
  if (*endptr != '\0') {
    RTL_TRDBG(TRACE_ERROR, "Data size parameter has wrong format. Must be numeric. "
      "(modbusDataSize=%s)\n", modbusDataSize);
    return NULL;
  }

  int stopsize = strtol (modbusStopSize, &endptr, 10);
  if (*endptr != '\0') {
    RTL_TRDBG(TRACE_ERROR, "Data stop size has wrong format. Must be numeric. "
      "(modbusStopSize=%s)\n", modbusStopSize);
    return NULL;
  }
  
  if (*modbusParity != 'N' && *modbusParity != 'O' && *modbusParity != 'E') {
    RTL_TRDBG(TRACE_ERROR, "Parity must be 'N', 'E', or 'O'. (modbusParity=%s)\n", 
      modbusParity);
    return NULL;
  }
  
  // Don't check if the UART device exist because it's can spawn after.
  
  // Allocate a new network
  Network_t *network = NetworkCreate(name);
  if (network == NULL) {
    return NULL;
  }

  // Serial specific init
  network->type = NETWORK_SERIAL;
  strncpy(network->uart, modbusUART, NAME_MAX);

  network->baudrate = baudrate;
  network->datasize = datasize;
  network->stopsize = stopsize;
  network->parity = modbusParity[0];
  
  network->modbusCtx = modbus_new_rtu(network->uart, network->baudrate, 
    network->parity, network->datasize, network->stopsize);
  
  return network;
}

void
NetworkDestroy(Network_t *network)
{
  // Remove the network from global network list
  list_del(&(network->list));
  
  // Close Serial modbus ctx
  if (network->modbusCtx != NULL) {
    modbus_close(network->modbusCtx);
    modbus_free(network->modbusCtx);
  }
  
  free(network);
  
  // Refresh network list inthe IPU
  DiaIpuStart();
}

void
NetworkDelete(Network_t *network)
{
  network->toDelete = true;
  
  RTL_TRDBG(TRACE_INFO, "Starting network '%s' delete state-machine\n", network->name);
  NetworkDeleteNext(network);
}

void
NetworkDeleteNext(Network_t *network)
{
  struct list_head *index;
      
  if (network == NULL || network->toDelete == false) {
    return;
  }
  
  // Delete all devices, one by one
  list_for_each(index, &(network->device)) {
    Sensor_t *device = list_entry(index, Sensor_t, list);
    
    DeviceDelete(device);
    return;
  }

  // Delete the network itself
  DiaNetDeleteWithSerial(network->cp_num, network->cp_serial, ModbusDiaLookup);
}

void
NetworkEnsureDeviceAreInit(Network_t *network)
{
  struct list_head *index;
      
  if (network == NULL || network->toDelete == true) {
    return;
  }
  
  list_for_each(index, &(network->device)) {
    Sensor_t *device = list_entry(index, Sensor_t, list);
    
    // Request burst from here !
    DeviceDiaCreateNext(device);
  }
}

void
NetworkStart(Network_t *network)
{
  if (network == NULL || network->toDelete == true) {
    return;
  }
  
  if (network->isInit == false) {
    DiaNetStartWithSerial(network->cp_num, network->cp_serial, ModbusDiaLookup);
    DiaIpuStart();
    return;
  }
  
  NetworkEnsureDeviceAreInit(network);
}

void
NetworkStartAll(void)
{
  struct list_head *index;
      
  list_for_each(index, &NetworkInternalList) {
    Network_t *network = list_entry(index, Network_t, list);
    
    NetworkStart(network);
  }
}

void
NetworkList(void *cl)
{
  struct list_head *index;
   
  list_for_each(index, &NetworkInternalList) {
    Network_t *network = list_entry(index, Network_t, list);
    AdmPrint(cl, "name=%s type=%s isInit=%s toDelete=%s\n",
      network->name,
      (network->type == NETWORK_SERIAL) ? "serial" : "ethernet",
      network->isInit ? "yes" : "no",
      network->toDelete ? "yes" : "no");
  }
}

void
NetworkListDevice(void *cl, Network_t *network)
{
  struct list_head *index;
      
  list_for_each(index, &(network->device)) {
    Sensor_t *device = list_entry(index, Sensor_t, list);
    AdmPrint(cl, "name=%s addr=%s isInit=%s toDelete=%s\n",
      device->name,
      device->addr,
      device->isInit ? "yes" : "no",
      device->toDelete ? "yes" : "no");
  }
}


/**********************************************************************************************************************
 *  Device tools
 */
Sensor_t *
DeviceFindFromName(Network_t *network, char *name)
{
  struct list_head *index;
  Sensor_t *device;
  
  list_for_each(index, &(network->device)) {
    device = list_entry(index, Sensor_t, list);

    if (strcmp(device->name, name) == 0) {
      return device;
    }
  }
  
  return NULL;
}
 
Sensor_t *
DeviceCreate(Network_t *network, char *name, char *addr, char *product)
{
  char *endptr;
  int serial_addr;
  int tcp_port;
  char *port;
  Sensor_t *device;

  // Check addr format
  if (network->type == NETWORK_SERIAL) {
    serial_addr = strtol (addr, &endptr, 10);
    if (*endptr != '\0') {
      RTL_TRDBG(TRACE_ERROR, "Adress of device on a serial line has wrong format. "
        "Must be numeric. (addr=%s)\n", addr);
      return NULL;
    }
  } else {
    port = index(addr, ':');
    if (port == NULL) {   // || *port == '\0' || *(port + 1) == '\0'
      tcp_port = 502;
    } else {
      tcp_port = strtol (port + 1, &endptr, 10);
      if (*endptr != '\0') {
        RTL_TRDBG(TRACE_ERROR, "TCP port of device has wrong format. Must be numeric. "
          "(port=%s)\n", port);
        return NULL;
      }
    }
  }

  device = (Sensor_t *) malloc (sizeof(Sensor_t));
  if (device == NULL) {
    RTL_TRDBG(TRACE_ERROR, "malloc failure\n");
    return NULL;
  }
  RTL_TRDBG(TRACE_API, "create device %p name=%s addr=%s product=%s network=%s\n", 
    device, name, addr, product, network->name);
  
  INIT_LIST_HEAD(&(device->ops));
  INIT_LIST_HEAD(&(device->cnts));
  device->network = network;
  device->name = strdup(name);
  device->addr = strdup(addr);
  device->product = strdup(product);
  device->isInit = false;
  device->toDelete = false;
  device->isARInit = false;
  device->isAR2Init = false;
  device->isAR3Init = false;
  device->isAppInit = false;
  device->modbusCtx = NULL;
  device->xo = ProductsGetDescription(product);
  if (network->type == NETWORK_SERIAL) {
    device->serial_addr = serial_addr;
  } else {
    *port = '\0';
    device->tcp_ip = strdup(addr);
    device->tcp_port = tcp_port;
  }
  
  snprintf(device->cp_ieee, CMN_IEEE_SIZE, "%s_%s", network->name, device->name);
  device->cp_num = CP_NUM_DEVICE;
  device->cp_serial = DoSerial();
  
  list_add(&(device->list), &(network->device));
  
  DeviceCheckModbusConnection(device);
  
  return device;
}

int
DeviceCheckModbusConnection(Sensor_t *device)
{
  modbus_t *modbusCtx = NULL;
  Network_t *network = device->network;
  
  if (network->type == NETWORK_SERIAL) {
    if (network->modbusCtx == NULL) {
      modbusCtx = network->modbusCtx = modbus_new_rtu(network->uart, 
        network->baudrate, network->parity, network->datasize, network->stopsize);
    }
  } else {
    if (device->modbusCtx == NULL) {
      modbusCtx = device->modbusCtx = modbus_new_tcp(device->tcp_ip, device->tcp_port);
    }
  }
  
  if (modbusCtx == NULL) {
    return 0;
  }

  // Set a timeout
  struct timeval response_timeout = {
    .tv_sec = 5,
    .tv_usec = 0
  };
  modbus_set_response_timeout(modbusCtx, &response_timeout);

  // For ethernet device, open the TCP/IP connection
  if (network->type == NETWORK_ETHERNET) {
    RTL_TRDBG(TRACE_INFO, "device %s try to connect. ip=%s port=%d\n", 
      device->name, device->tcp_ip, device->tcp_port);
    if (modbus_connect(modbusCtx) == -1) {
      device->modbusCtx = NULL;
      RTL_TRDBG(TRACE_ERROR, "Modbus connection failed: %s\n", 
        modbus_strerror(errno));
      modbus_free(modbusCtx);
      return 0;
    }
    
    RTL_TRDBG(TRACE_API, "device %s connected. ip=%s port=%d\n", 
      device->name, device->tcp_ip, device->tcp_port);
  }
  
  return 1;
}

int
DeviceModbusDisconnection(Sensor_t *device)
{
  if (device->network->type == NETWORK_SERIAL) {
    if (NULL != device->network->modbusCtx) {
      modbus_close(device->network->modbusCtx);
      modbus_free(device->network->modbusCtx);
      device->network->modbusCtx = NULL;
    }
  } else {
    if (NULL != device->modbusCtx) {
      modbus_close(device->modbusCtx);
      modbus_free(device->modbusCtx);
      device->modbusCtx = NULL;
    }
  }
  
  return 1;
}

void
DeviceDelete(Sensor_t *device)
{
  device->toDelete = true;
  
  RTL_TRDBG(TRACE_INFO, "Starting device '%s' delete state-machine\n", device->name);
  DeviceDiaDeleteNext(device);
}

void
DeviceDestroy(Sensor_t *device)
{
  struct list_head *index, *next;
  Network_t *network = device->network;
  
  // Remove the device from the network
  list_del(&(device->list));

  // Cleanup Attr_t
  list_for_each_safe(index, next, &(device->ops)) {
    Attr_t *attr = list_entry(index, Attr_t, list);
    list_del(index);
    free(attr);
  }

  // Cleanup Point_t
  list_for_each_safe(index, next, &(device->ops)) {
    Point_t *pt = list_entry(index, Point_t, list);
    list_del(index);
    pt->free(pt);
  }

  free(device->name);
  free(device->addr);
  free(device->product);
  
  if (device->tcp_ip != NULL) {
    free(device->tcp_ip);
  }
  
  if (device->modbusCtx != NULL) {
    modbus_close(device->modbusCtx);
    modbus_free(device->modbusCtx);
  }
  
  // Do not free Xo, it's just a reference to product catalog
  
  free(device);
  
  if (network->toDelete == false) {
    // Update the network
    DiaNetStartWithSerial(network->cp_num, network->cp_serial, ModbusDiaLookup);
  } else {
    // Device deleted from a delete operation on the network
    // Continue the delete of the network
    NetworkDeleteNext(network);
  }
}

/*
 *  Also known as What next ?
 *  State machine for delete of M2M Application on ONG
 *
 *  @return the number of create request created
 */
int
DeviceDiaDeleteNext(Sensor_t *device)
{
  t_dia_req req[1];
  struct list_head *index;

  if (device == NULL) {
    return 0;
  }

  memset(req, 0, sizeof(t_dia_req));
  req->rq_dev = device->cp_num;
  req->rq_serial = device->cp_serial;
  req->rq_name = (char *)__func__;
  req->rq_cbserial = ModbusDiaLookup;

  // Device Application containers
  // It's can be a partial delete during a product descriptor update
  list_for_each(index, &(device->cnts)) {
    Point_t *pt = list_entry(index, Point_t, list);   

    if (pt->isInit == true && pt->toDelete == true) {
      RTL_TRDBG(TRACE_INFO, "\n\ndevice=%s\t\t0x%04hX.0x%04hX.%hu.m2m DELETE\n",
        device->name, pt->rq_cluster, pt->rq_attribut, pt->rq_member);
      req->rq_app = 0;
      req->rq_cluster = pt->rq_cluster;
      req->rq_attribut = pt->rq_attribut;
      req->rq_member = pt->rq_member;
      DiaDatContDelete(req, NULL);
      return 1;
    }
  }

  // Before delete, APP, DEV and AR, ensure this a real delete query
  if (device->toDelete == false) {
      return 0;
  }

  // Device Application
  if (device->isAppInit == true) {
    req->rq_app = 0;
    DiaAppElemDelete(req, NULL);
    return 1;
  }
  
  if (device->isARInit == true) {
    req->rq_app = 0;
    req->rq_access = 0;
    DiaAccElemDelete(req, NULL);
    return 1;
  }
  
  if (device->isAR2Init == true) {
    req->rq_app = 0;
    req->rq_access = 1;
    DiaAccElemDelete(req, NULL);
    return 1;
  }
  
  if (device->isAR3Init == true) {
    req->rq_app = 0;
    req->rq_access = 2;
    DiaAccElemDelete(req, NULL);
    return 1;
  }
  
  // Device
  if (device->isInit == true) {
    DiaDevElemDelete(req, NULL);
    return 1;
  }

  return 0;
}

/*
 *  Also known as What next ?
 *  State machine for creation of M2M Application on ONG
 *
 *  @return the number of create request created
 */
int
DeviceDiaCreateNext(Sensor_t *device)
{
  struct list_head *index;
  t_dia_req req[1];
  
  /*
   *  Ensure the device descriptor is loaded,
   *  if not all M2M exchange for this device are aborded sync the descriptor will be received
   */
  if (device == NULL) {
    return 0;
  }
  
  if (device->xo == NULL) {
    device->xo = ProductsGetDescription(device->product);
    if (device->xo == NULL) {
      return 0;
    }
  }
  
  memset(req, 0, sizeof(t_dia_req));
  req->rq_dev = device->cp_num;
  req->rq_serial = device->cp_serial;
  req->rq_name = (char *)__func__;
  req->rq_cbserial = ModbusDiaLookup;
    
  /*
   *  Physical device to create ?
   */
  if (device->isInit == false) {
    DiaDevElemRetrieve(req, NULL);
    return 1;
  }
  
  /*
   *  Physical application to create, or associated AR ?
   */
  if (device->isARInit == false) {
    req->rq_app = 0;
    req->rq_access = 0;
    DiaAccElemRetrieve(req, NULL);
    return 1;
  }
  if (device->isAR2Init == false) {
    req->rq_app = 0;
    req->rq_access = 1;
    DiaAccElemRetrieve(req, NULL);
    return 1;
  }
  if (device->isAR3Init == false) {
    req->rq_app = 0;
    req->rq_access = 2;
    DiaAccElemRetrieve(req, NULL);
    return 1;
  }
  if (device->isAppInit == false) {
    req->rq_app = 0;
    DiaAppElemRetrieve(req, NULL);
    return 1;
  } 
  
  /*
   *  Container for GroupAddr application to create ?
   */
  list_for_each(index, &(device->cnts)) {
    Point_t *pt = list_entry(index, Point_t, list);   

    if (pt->isInit == false) {
      RTL_TRDBG(TRACE_INFO, "\n\ndevice=%s\t\t0x%04hX.0x%04hX.%hu.m2m CREATE\n",
        device->name, pt->rq_cluster, pt->rq_attribut, pt->rq_member);
      req->rq_app = 0;
      req->rq_cluster = pt->rq_cluster;
      req->rq_attribut = pt->rq_attribut;
      req->rq_member = pt->rq_member;
      DiaDatElemRetrieve(req, NULL);
      return 1;
    }
  }
  
  // Check for pending delete
  DeviceDiaDeleteNext(device);
  
  return 0;
}

void
DeviceNotifyProductUpdate(char *ref)
{
  struct list_head *index, *index2, *index3, *next;
  Network_t *network;
  Sensor_t *device;

  list_for_each(index, &NetworkInternalList) {
    network = list_entry(index, Network_t, list);
    RTL_TRDBG(TRACE_INFO, "network = %p\n", network);
    RTL_TRDBG(TRACE_INFO, "network = %s\n", network->name);
    
    list_for_each(index2, &(network->device)) {
      device = list_entry(index2, Sensor_t, list);
      RTL_TRDBG(TRACE_INFO, "device = %p\n", device);
      RTL_TRDBG(TRACE_INFO, "device = %s\n", device->name);
    
      if (strcmp(device->product, ref) == 0) {
        // Xo is already received and in cache
        // device->xo is already the new product descriptor
        RTL_TRDBG(TRACE_INFO, "Device '%s' must be update !\n", device->name);

        // Drop all retargeting URI
        // May generate some 404 error just before the DESCRIPTOR update
        list_for_each_safe(index3, next, &(device->ops)) {
          Attr_t *attr = list_entry(index3, Attr_t, list);
          list_del(index);
          free(attr);
        }

        // Mark logging cnt to be deleted
        // After the update of the application DESCRIPTOR, only cnt not used anymore will be deleted
        list_for_each(index3, &(device->cnts)) {
          Point_t *pt = list_entry(index3, Point_t, list);
          pt->toDelete = true;
          // direct delete for not init
          RTL_TRDBG(TRACE_INFO, "pt = %p, tag as toDelete\n", pt);
        }
  
        // Rewrite the application descriptor
        device->isAppInit = false;
        DeviceDiaCreateNext(device);
      }
    }
  }
}

Attr_t *
DeviceGetAttrFromRegAccess(Sensor_t *device, char *str)
{
  struct list_head *index;
      
  list_for_each(index, &(device->ops)) {
    Attr_t *attr = list_entry(index, Attr_t, list);
    
    if (strcmp(attr->modbusAccess, str) == 0) {
      return attr;
    }
  }
  
  return NULL;
}

void
DeviceListInterfaces(void *cl, Sensor_t *device)
{
  struct list_head *index;
  //time_t now = time(NULL);
  time_t now = 0;
  rtl_timemono(&now);
  
  AdmPrint(cl, "----- Retargeting -----\n");
  AdmPrint(cl, "Modbus Access\n");
  list_for_each(index, &(device->ops)) {
    Attr_t *attr = list_entry(index, Attr_t, list);
    AdmPrint(cl, "%s\n", attr->modbusAccess);
  }
  
  AdmPrint(cl, "----- Logging -----\n");
  AdmPrint(cl, "Container name\t\tPeriod\tNext\tMax\tModbus Access\n");
  list_for_each(index, &(device->cnts)) {
    Point_t *pt = list_entry(index, Point_t, list);
    if (pt->toDelete) {
      AdmPrint(cl, "0x%04hX.0x%04hX.%hu.m2m\tWaiting for delete\n",
        pt->rq_cluster, pt->rq_attribut, pt->rq_member);
    } else {
      AdmPrint(cl, "0x%04hX.0x%04hX.%hu.m2m\t%d\t%d\t%d\t%s\n", 
        pt->rq_cluster, pt->rq_attribut, pt->rq_member,
        pt->minInterval,
        (pt->nextRead - now) < 0 ? 0 : (pt->nextRead - now),
        (pt->nextMaxInterval - now) < 0 ? 0 : (pt->nextMaxInterval - now),
        pt->attr->modbusAccess);
    }
  }
}

Point_t *
DeviceGetPointOrCreate(Sensor_t *device, char *containerID, bool *isNew)
{
  struct list_head *index;
  Point_t *pt;
  
  // Parse cntID
  uint16_t rq_cluster, rq_attribut, rq_member;
  int ret = sscanf(containerID, "0x%hX.0x%hX.%hu.m2m", 
    &(rq_cluster), &(rq_attribut), &(rq_member));
  if (ret != 3) {
    return NULL;
  }
  
  // Previouly used cntId
  list_for_each(index, &(device->cnts)) {
    pt = list_entry(index, Point_t, list);
    // --WARNING-- pt->containerID is a pointer into a memory already free ! --WARNING--
    
    if (pt->rq_cluster == rq_cluster &&
      pt->rq_attribut == rq_attribut &&
      pt->rq_member == rq_member) {
      
      // pt->isInit stay in previous state
      RTL_TRDBG(TRACE_INFO, "device=%s Point from cache(%s)=%p\n", device->name, containerID, pt);
      *isNew = false;
      return pt;
    }
  }
  
  // New cntId
  pt = new_Point_t();
  if (pt == NULL) {
    return NULL;
  }
  *isNew = true;
  RTL_TRDBG(TRACE_INFO, "device=%s new Point(%s)=%p\n", device->name, containerID, pt);
  return pt;
}
 

void
templateSetDevice(Sensor_t *device)
{
  char *tmp;
  
  if (device == NULL || device->xo == NULL) {
    return;
  }

  void *xo = *(device->xo);
  if (xo == NULL) {
    return;
  }

  SetVarFromXo("modbus_dev_manufacturer_name", xo, "modbus:vendor.value$");
  SetVarFromXo("modbus_dev_name", xo, "modbus:product.value$");
  SetVarFromXo("modbus_dev_version", xo, "modbus:version.value$");
  SetVarFromXo("modbus_dev_ref", xo, "modbus:ref.value$");
}

