#include "modbus.h"

void
DriverLoadNetworksCache(void)
{
  char path[NAME_MAX];
  int  parse;
  void *xo;
  Network_t *network;
  Sensor_t *device;
  

  if (!Operating) {
    snprintf(path, NAME_MAX, "%s/networks.xml", getenv("AW_DATA_DIR"));
  } else {
    snprintf(path, NAME_MAX, "%s/usr/data/%s/networks.xml", rootactPath, GetAdaptorName());
  }

  xo = XoReadXmlEx(path, NULL, 0, &parse);
  if (xo == NULL) {
    RTL_TRDBG(1, "Can't read/parse: '%s'\n", path);
    return;
  }
  
  int nbNetworks, iNetworks;
  int nbDevices, iDevices;
  
  // Iterate on networks from cache
  nbNetworks = XoNmNbInAttr(xo, "modbus:networks", 0);
  RTL_TRDBG(1, "Loading networks from cache, count=%d\n", nbNetworks);
  for (iNetworks=0; iNetworks<nbNetworks; iNetworks++) {
    void *xoNetwork = XoNmGetAttr(xo, "modbus:networks[%d]", 0, iNetworks);
    CONTINUE_IF_NULL(xoNetwork);

    void *networkName = XoNmGetAttr(xoNetwork, "modbus:name", NULL, 0);
    CONTINUE_IF_NULL(networkName);

    void *networkType = XoNmGetAttr(xoNetwork, "modbus:type", NULL, 0);
    CONTINUE_IF_NULL(networkType);
    
    network = NetworkFindFromName(networkName);
    if (network != NULL) {
      RTL_TRDBG(1, "\tNetwork '%s' (previously created)\n", networkName);
    } else {
      if (strcmp(networkType, "ethernet") == 0) {
        network = NetworkCreateEthernet(networkName);
        CONTINUE_IF_NULL(network);
        RTL_TRDBG(1, "\tNetwork '%s'\n", networkName);
      } else {
        RTL_TRDBG(1, "\t\n\n\n\n-----> FFS SERIAL <-----\n\n\n\n");
        continue;
      }
    }
    network->isInit = false;  // force a DESCRIPTOR refresh
    
    // Iterate on devices for this network from cache
    nbDevices = XoNmNbInAttr(xoNetwork, "modbus:devices", 0);
    RTL_TRDBG(1, "Loading devices from cache, count=%d\n", nbDevices);
    for (iDevices=0; iDevices<nbDevices; iDevices++) {
      void *xoDevice = XoNmGetAttr(xoNetwork, "modbus:devices[%d]", 0, iDevices);
      CONTINUE_IF_NULL(xoDevice);

      void *deviceName = XoNmGetAttr(xoDevice, "modbus:name", NULL, 0);
      CONTINUE_IF_NULL(deviceName);

      void *deviceAddr = XoNmGetAttr(xoDevice, "modbus:addr", NULL, 0);
      CONTINUE_IF_NULL(deviceAddr);
      
      void *deviceRef = XoNmGetAttr(xoDevice, "modbus:ref", NULL, 0);
      CONTINUE_IF_NULL(deviceRef);
         
      device = DeviceFindFromName(network, deviceName);
      if (device != NULL) {
        RTL_TRDBG(1, "\tDevice '%s' (previously created)\n", deviceName);
      } else {
        device = DeviceCreate(network, deviceName, deviceAddr, deviceRef);
        CONTINUE_IF_NULL(device);
        RTL_TRDBG(1, "\tDevice '%s'\n", deviceName);
      }
      device->isInit = false;  // force a DESCRIPTOR refresh
    }
  }
  
  XoFree(xo, 1);
  
  // Start M2M state machine for all networks
  NetworkStartAll();
}

void
DriverUpdateNetworksCache(void)
{
  char path[NAME_MAX];
  struct list_head *index, *index2;
  Network_t *network;
  Sensor_t *device;

  if (!Operating) {
    snprintf(path, NAME_MAX, "%s/networks.xml", getenv("AW_DATA_DIR"));
  } else {
    snprintf(path, NAME_MAX, "%s/usr/data/%s/networks.xml", rootactPath, GetAdaptorName());
  }
  
  void *context = XoNmNew("modbus:context");
  if (context == NULL) {
    return;  
  }
  XoNmSetAttr(context, "xmlns:modbus", "http://uri.actility.com/m2m/adaptor-modbus", 0);
  
  list_for_each(index, &NetworkInternalList) {
    Network_t *network = list_entry(index, Network_t, list);
    CONTINUE_IF_NULL(network);
    
    void *xoNetwork = XoNmNew("modbus:network");
    CONTINUE_IF_NULL(xoNetwork);
    
    XoNmSetAttr(xoNetwork, "modbus:name", network->name, 0);
    XoNmSetAttr(xoNetwork, "modbus:type", (network->type == NETWORK_SERIAL) ? "serial" : "ethernet", 0);
    if (network->type == NETWORK_SERIAL) {
      char buffer[1024];
      
      XoNmSetAttr(xoNetwork, "modbus:uart", network->uart, 0);
      
      snprintf(buffer, sizeof(buffer), "%d", network->baudrate);
      XoNmSetAttr(xoNetwork, "modbus:baudrate", buffer, 0);
      
      snprintf(buffer, sizeof(buffer), "%c", network->parity);
      XoNmSetAttr(xoNetwork, "modbus:parity", buffer, 0);
      
      snprintf(buffer, sizeof(buffer), "%d", network->datasize);
      XoNmSetAttr(xoNetwork, "modbus:dataSize", buffer, 0);
      
      snprintf(buffer, sizeof(buffer), "%d", network->stopsize);
      XoNmSetAttr(xoNetwork, "modbus:stopSize", buffer, 0);
    }

    // Append devices
    list_for_each(index2, &(network->device)) {
      device = list_entry(index2, Sensor_t, list);
      CONTINUE_IF_NULL(device);
      
      void *xoDevice = XoNmNew("modbus:device");
      CONTINUE_IF_NULL(xoDevice);
      
      XoNmSetAttr(xoDevice, "modbus:name", device->name, 0);
      XoNmSetAttr(xoDevice, "modbus:addr", device->addr, 0);
      XoNmSetAttr(xoDevice, "modbus:ref", device->product, 0);
      
      XoNmAddInAttr(xoNetwork, "modbus:devices", xoDevice, 0, 0);
    }
    
    XoNmAddInAttr(context, "modbus:networks", xoNetwork, 0, 0);
  }
  
  XoWritXmlEx(path, context, 0, "modbus");
  XoFree(context, 1);
}

void
DriverClearNetworksCache(void)
{
  int rc;
  char path[NAME_MAX];

  if (!Operating) {
    snprintf(path, NAME_MAX, "%s/networks.xml", getenv("AW_DATA_DIR"));
  } else {
    snprintf(path, NAME_MAX, "%s/usr/data/%s/networks.xml", rootactPath, GetAdaptorName());
  }
  
  rc = remove(path);
  if (rc == 0) {
    RTL_TRDBG(1, "\tNetwork cache dropped !\n");
  } else {
    RTL_TRDBG(1, "\tError during network cache drop ! rc=%d\n", rc);
  }
}

