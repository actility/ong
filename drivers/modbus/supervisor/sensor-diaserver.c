#include "modbus.h"

/**
 * @brief Call back from retargeted operations on the M2M IPU application.
 * @param ident the IPU identifier (i.e. MODBUS)
 * @param targetlevel The access right level (i.e. 1, 2 or 3)
 * @param obix the request content decoded as oBIX buffer.
 * @param targetid the request URI of the dIa incoming request.
 * @param reqId the dIa request identifier to provide on completion.
 */
int
IpuExecuteMethod(char *ident, int targetlevel, void *obix, char *targetid, int tid)
{
  int ret;
  char *name;
  Network_t *network;

  RTL_TRDBG(0, "%s\n", __FUNCTION__);
  NOT_USED(ident);
  NOT_USED(targetlevel);
  NOT_USED(tid);
  
  char *ope = rindex(targetid, '/') + 1;
  if (ope == NULL || *ope == '\0') {
    RTL_TRDBG(0, "Can't read operation\n");
    return -404;
  }
  
  if (strcmp(ope, "modbusCreateNetwork") == 0) {
  /*
    <?xml version="1.0" encoding="UTF-8"?>
    <obix:obj xmlns:obix="http://obix.org/ns/schema/1.1" href="modbusCreateNetwork">
      <obix:str name="modbusNetworkName" val="myNetwork" />
      <obix:str name="modbusNetworkType" val="ethernet" />
    </obix:obj>

    <?xml version="1.0" encoding="UTF-8"?>
    <obix:obj xmlns:obix="http://obix.org/ns/schema/1.1" href="modbusCreateNetwork">
      <obix:str name="modbusNetworkName" val="myNetwork" />
      <obix:str name="modbusNetworkType" val="Serial" />
      <obix:str name="modbusUART" val="/dev/ttyUSB0" />
      <obix:str name="modbusBaudrate" val="19200" />
      <obix:str name="modbusParity" val="N" />
      <obix:str name="modbusDataSize" val="8" />
      <obix:str name="modbusStopSize" val="1" />
    </obix:obj>
  */

    char *modbusNetworkName = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusNetworkName].val", NULL);
    if (modbusNetworkName == NULL) {
      RTL_TRDBG(0, "Argument 'modbusNetworkName' not found\n");
      return -400;
    }
    
    char *modbusNetworkType = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusNetworkType].val", NULL);
    if (modbusNetworkName == NULL) {
      RTL_TRDBG(0, "Argument 'modbusNetworkType' not found\n");
      return -400;
    } else
    if (!strcmp(modbusNetworkType, "ethernet")) {
      network = NetworkCreateEthernet(modbusNetworkName);
    } else
    if (!strcmp(modbusNetworkType, "serial")) {
      char *modbusUART = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusUART].val", NULL);
      if (modbusUART == NULL) {
        RTL_TRDBG(0, "Argument 'modbusUART' not found\n");
        return -400;
      }
      
      char *modbusBaudrate = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusBaudrate].val", NULL);
      if (modbusBaudrate == NULL) {
        modbusBaudrate = "19200";
        RTL_TRDBG(0, "Argument 'modbusBaudrate' use default value '%s'\n", modbusBaudrate);
      }

      char *modbusParity = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusParity].val", NULL);
      if (modbusParity == NULL) {
        modbusParity = "N";
        RTL_TRDBG(0, "Argument 'modbusParity' use default value '%s'\n", modbusParity);
      }
      
      char *modbusDataSize = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusDataSize].val", NULL);
      if (modbusDataSize == NULL) {
        modbusDataSize = "8";
        RTL_TRDBG(0, "Argument 'modbusDataSize' use default value '%s'\n", modbusDataSize);
      }

      char *modbusStopSize = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusStopSize].val", NULL);
      if (modbusStopSize == NULL) {
        modbusStopSize = "1";
        RTL_TRDBG(0, "Argument 'modbusStopSize' use default value '%s'\n", modbusStopSize);
      }
      
      network = NetworkCreateSerial(modbusNetworkName, modbusUART, modbusBaudrate, modbusParity, modbusDataSize, modbusStopSize);
    } else {
      // Bad type
      RTL_TRDBG(0, "Argument 'modbusNetworkType', value not supported '%s'\n", modbusNetworkType);
      return -400;
    }
    
    // Check the network is really created
    if (network == NULL) {
      RTL_TRDBG(0, "modbusCreateNetwork failed\n");
      return -500;
    }
    
    DiaNetStartWithSerial(network->cp_num, network->cp_serial, ModbusDiaLookup);
    
    // Refresh the network list on the IPU Descriptor
    DiaIpuStart();
    
    return 0;
  } else
  if (strcmp(ope, "modbusLoadProduct") == 0) {
  /*
    <?xml version="1.0" encoding="UTF-8"?>
    <obix:obj xmlns:obix="http://obix.org/ns/schema/1.1">
      <obix:str name="modbusProductRef" val="Telemecanique:OTB1E0DM9LP:0" />
    </obix:obj>
  */
    char *modbusProductRef = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusProductRef].val", NULL);
    if (modbusProductRef == NULL) {
      RTL_TRDBG(0, "Argument 'modbusProductRef' not found\n");
      return -400;
    }
    
    ProductsSync(modbusProductRef);
    return 0;
  } else {
    RTL_TRDBG(0, "Unknown operation '%s' on IPU\n", ope);
    return -404;
  }
  
  return -404;
}

/**
 * @brief Call back from retargeted operations on the M2M network application.
 * @param ident the network identifier (i.e. the network name)
 * @param targetlevel The access right level (i.e. 1, 2 or 3)
 * @param obix the request content decoded as oBIX buffer.
 * @param targetid the request URI of the dIa incoming request.
 * @param reqId the dIa request identifier to provide on completion.
 */
int
CmnNetworkExecuteMethod(char *ident, int targetlevel, void *obix, char *targetid, int tid)
{
  RTL_TRDBG(0, "%s\n", __FUNCTION__);
  NOT_USED(targetlevel);
  NOT_USED(tid);

  // Ensure the ident exists 
  Network_t *network = NetworkFindFromName(ident);
  if (network == NULL) {
    RTL_TRDBG(0, "Network doesn't exist (ident:%s)\n", ident);
    return -404;
  }
  
  char *ope = rindex(targetid, '/') + 1;
  if (ope == NULL || *ope == '\0') {
    RTL_TRDBG(0, "Can't read operation\n");
    return -404;
  }
  
  if (strcmp(ope, "modbusDeleteNetwork") == 0) {
  /*
    No Payload
  */
    NetworkDelete(network);
    return 0;
    
  } else
  if (strcmp(ope, "modbusAddDevice") == 0) {
  /*
    <?xml version="1.0" encoding="UTF-8"?>
    <obix:obj xmlns:obix="http://obix.org/ns/schema/1.1">
      <obix:str name="modbusDeviceName" val="LabjackRoof" />
      <obix:str name="modbusDeviceAddress" val="192.1.1.3:502" />
      <obix:str name="modbusDeviceDefinition" val="labjack:u9:1" />
    </obix:obj>

    <?xml version="1.0" encoding="UTF-8"?>
    <obix:obj xmlns:obix="http://obix.org/ns/schema/1.1">
      <obix:str name="modbusDeviceName" val="LabjackRoof" />
      <obix:str name="modbusDeviceAddress" val="1" />
      <obix:str name="modbusDeviceDefinition" val="labjack:u9:1" />
    </obix:obj>
  */
    char *modbusDeviceName = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusDeviceName].val", NULL);
    if (modbusDeviceName == NULL) {
      RTL_TRDBG(0, "Argument 'modbusDeviceName' not found\n");
      return -400;
    }

    char *modbusDeviceAddress = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusDeviceAddress].val", NULL);
    if (modbusDeviceAddress == NULL) {
      RTL_TRDBG(0, "Argument 'modbusDeviceAddress' not found\n");
      return -400;
    }
    
    char *modbusDeviceDefinition = (char *) XoNmGetAttr(obix, "wrapper$[name=modbusDeviceDefinition].val", NULL);
    if (modbusDeviceDefinition == NULL) {
      RTL_TRDBG(0, "Argument 'modbusDeviceDefinition' not found\n");
      return -400;
    }
    
    Sensor_t *device = DeviceCreate(network, modbusDeviceName, modbusDeviceAddress, modbusDeviceDefinition);
    if (device == NULL) {
      return -500;
    }
    
    DeviceDiaCreateNext(device);
    return 0;
    
  } else
  if (strcmp(ope, "modbusScanNetwork") == 0) {
    // FFS modbusScanNetwork
    return -501;
  } else {
    RTL_TRDBG(0, "Unknown operation '%s' on network\n", ope);
    return -404;
  }


  // Answer a wrong request URI
  return -404;
}

/**
 * @brief Call back raised on incoming dIa create request for operation.
 * Invoked in diaserver.c
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application number (i.e. the end point identifier targeted on the remote
 * node).
 * @param cluster the cluster identifier the command refers to.
 * @param numm the operation number in case of ZCL operation, -1 otherwise (ZDO).
 * @param targetlevel the access right level (i.e. 1, 2 or 3) (useless here)
 * @param obix the dIa request content decoded as an oBIX document.
 * @param targetid the requesting URI of the incoming dIa request.
 * @param reqId the dIa request identifier (to provide for completion).
 * return 0 in case of success, a negative number indicating the error code otherwise.
 */
int
iCmnSensorExecuteMethod(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, void *obix, char *targetid, int tid)
{
  NOT_USED(tid);
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  RTL_TRDBG(0, "%s\n", __FUNCTION__);
  RTL_TRDBG(0, "\tapp = %d\n", app);
  RTL_TRDBG(0, "\tcluster = %d\n", cluster);
  RTL_TRDBG(0, "\tnumm = %d\n", numm);
  RTL_TRDBG(0, "\ttargetlevel = %d\n", targetlevel);
  RTL_TRDBG(0, "\tobix = %p\n", obix);

  char *ope = rindex(targetid, '/') + 1;
  if (ope == NULL || *ope == '\0') {
    RTL_TRDBG(0, "Can't read operation\n");
    return -404;
  }

  if (strcmp(ope, "modbusDeleteDevice") == 0) {
    Network_t *network = device->network;
    DeviceDelete(device);
    return 0;
  } else {
    RTL_TRDBG(0, "Unknown operation '%s' on device\n", ope);
    return -404;
  }
  
  // Answer a wrong request URI
  return -404;
}

/**
 * @brief Call back raised on incoming dIa Retrieve request for attribute.
 * Invoked in diaserver.c
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application number (i.e. the end point identifier targeted on the remote
 * node).
 * @param cluster the cluster identifier the command refers to.
 * @param numm the attribute number the request refers to.
 * @param targetlevel the access right level (i.e. 1, 2 or 3) (useless here)
 * @param targetid the requesting URI of the incoming dIa request.
 * @param reqId the dIa request identifier (to provide for completion).
 * return positive number in case of success, a negative number indicating
 * the error code otherwise.
 */
int
iCmnSensorRetrieveAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, char *targetid, int tid)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);
  
  RTL_TRDBG(0, "%s\n", __FUNCTION__);
  RTL_TRDBG(0, "\tcmn = %p\n", cmn);
  RTL_TRDBG(0, "\tapp = %d\n", app);
  RTL_TRDBG(0, "\tcluster = %d\n", cluster);
  RTL_TRDBG(0, "\tnumm = %d\n", numm);
  RTL_TRDBG(0, "\ttargetlevel = %d\n", targetlevel);
  RTL_TRDBG(0, "\ttargetid = %s\n", targetid);
  RTL_TRDBG(0, "\ttid = %d\n", tid);
  
  char *uuid = strstr(targetid, "/retargeting");
  if (uuid == NULL) {
    return -404;
  }
  uuid++; // move to "r"
  uuid = index(uuid, '/');
  if (uuid == NULL) {
    return -404;
  }
  
  Attr_t *attr = DeviceGetAttrFromRegAccess(device, uuid);
  if (attr == NULL) {
    return -404;
  }

  RTL_TRDBG(1, "modbusType=%s(%d), isReadable=%d, reg=%s\n",
    attr->modbusType, attr->modbusTypeID, attr->isReadable, attr->modbusAccess);

  if (attr->isReadable == false) {
    return -403;
  }
  
  // modbus access
  modbusAccessRead(device, attr);
  return 0;
}

/**
 * @brief Call back raised on incoming dIa Retrieve request for attribute.
 * Invoked in diaserver.c
 * @param cmn the common structure instance for the targeted sensor.
 * @param app the application number (i.e. the end point identifier targeted on the remote
 * node).
 * @param cluster the cluster identifier the command refers to.
 * @param numm the attribute number the request refers to.
 * @param targetlevel the access right level (i.e. 1, 2 or 3) (useless here)
 * @param obix the dIa request content decoded as an oBIX document.
 * @param targetid the requesting URI of the incoming dIa request.
 * @param reqId the dIa request identifier (to provide for completion).
 * return positive number in case of success, a negative number indicating
 * the error code otherwise.
 */
int
iCmnSensorUpdateAttrValue(t_cmn_sensor *cmn, int app, int cluster, int numm, int targetlevel, void *obix, char *targetid, int tid)
{
  Sensor_t *device = container_of(cmn, Sensor_t, cp_cmn);

  RTL_TRDBG(0, "%s\n", __FUNCTION__);
  RTL_TRDBG(0, "\tcmn = %p\n", cmn);
  RTL_TRDBG(0, "\tapp = %d\n", app);
  RTL_TRDBG(0, "\tcluster = %d\n", cluster);
  RTL_TRDBG(0, "\tnumm = %d\n", numm);
  RTL_TRDBG(0, "\ttargetlevel = %d\n", targetlevel);
  RTL_TRDBG(0, "\tobix = %p\n", obix);
  RTL_TRDBG(0, "\ttargetid = %s\n", targetid);
  RTL_TRDBG(0, "\ttid = %d\n", tid);

  char *uuid = strstr(targetid, "/retargeting");
  if (uuid == NULL) {
    return -404;
  }
  uuid++; // move to "r"
  uuid = index(uuid, '/');
  if (uuid == NULL) {
    return -404;
  }
  
  Attr_t *attr = DeviceGetAttrFromRegAccess(device, uuid);
  if (attr == NULL) {
    return -404;
  }

  RTL_TRDBG(1, "modbusType=%s(%d), isWritable=%d, reg=%s\n",
    attr->modbusType, attr->modbusTypeID, attr->isWritable, attr->modbusAccess);

  if (attr->isWritable == false) {
    return -403;
  }
  
  // modbus access
  modbusAccessWrite(device, attr, obix);
  return 0;
}
