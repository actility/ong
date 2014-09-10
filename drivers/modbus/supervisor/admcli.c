#include "modbus.h"

/**
 * @brief Display on CLI a help message about the supported commands.
 * @param cl the CLI context.
 */
void
AdmUsage(t_cli *cl)
{
  AdmPrint(cl,"help\t\t\t\t\tShow this Help\n");
  AdmPrint(cl,"quit|exit\n");
  AdmPrint(cl,"stop\n");
  AdmPrint(cl,"who\n");
  AdmPrint(cl,"close\n");
  AdmPrint(cl,"core\n");
  AdmPrint(cl,"xoocc\n");
  AdmPrint(cl,"info\n");
  AdmPrint(cl,"trace [level]\n");
  AdmPrint(cl,"getvar [varname]\n");
  AdmPrint(cl,"setvar varname [value]\n");
  AdmPrint(cl,"addvar varname\n");

  //AdmPrint(cl,"covcfg\n");
  AdmPrint(cl,"\n");

  AdmPrint(cl,"ipu refresh descriptor\t\t\tForce to update the DESCRIPTOR container of the modbus IPU\n");
  AdmPrint(cl,"\n");
  
  AdmPrint(cl,"products list\t\t\t\tList known product for the driver\n");
  AdmPrint(cl,"products fetch <ref>\t\t\tDownload a product definition, only if it do not exist in the driver cache\n");
  AdmPrint(cl,"products sync\t\t\t\tFetch all product definition missing\n");
  AdmPrint(cl,"products sync <ref>\t\t\tDownload a product definition and update the driver cache\n");
  AdmPrint(cl,"\n");
  
  AdmPrint(cl,"network load cache\t\t\tRestore networks and devices from cache.\n");
  AdmPrint(cl,"network write cache\t\t\tWrite networks and devices definition into cache.\n");
  AdmPrint(cl,"network list\t\t\t\tList known networks\n");
  AdmPrint(cl,"network devices <network>\t\tList known devices for a network\n");
  AdmPrint(cl,"network start <name>\t\t\tStart the state-machine which create M2M Application for a network\n");
  AdmPrint(cl,"network startall\t\t\tSame as 'start' but for all networks\n");
  AdmPrint(cl,"network create ethernet <name>\t\tCreate a network with type Ethernet\n");
  AdmPrint(cl,"network create serial <name> <uart> <baudrate> <datasize> <parity> <stopsize>\n");
  AdmPrint(cl,"\t\t\t\t\tCreate a network with type Serial\n");
  AdmPrint(cl,"\n");
  
  AdmPrint(cl,"device interfaces <network> <device>\tList retargeting URI and logging containers for a device\n");
  AdmPrint(cl,"\n");

  AdmPrint(cl,"reader start\t\t\t\tStart the modbus reader thread\n");
  AdmPrint(cl,"reader stop\t\t\t\tStop the modbus reader thread\n");
  AdmPrint(cl,"reader status\t\t\t\tShow the modbus reader thread status\n");
  AdmPrint(cl,"\n");
  
}

/**
 * @brief Display on CLI the collected information on driver.
 * @param cl the CLI context.
 */
void
AdmInfo(t_cli *cl)
{
  unsigned long vsize;
  unsigned int nbm;
  unsigned int nbt;

  vsize  = rtl_vsize(getpid());

  nbm  = rtl_imsgVerifCount(MainIQ);
  nbt  = rtl_timerVerifCount(MainIQ);

  AdmPrint(cl,"vsize=%fMo (%u)\n",((float)vsize)/1024.0/1024.0,vsize);
  AdmPrint(cl,"pseudoong=%d withdia=%d\n", PseudoOng, WithDia);

  AdmPrint(cl,"#msg=%d\n",nbm);
  AdmPrint(cl,"#timer=%d\n",nbt);
  AdmPrint(cl,"diapending=%u(%u)\n",DiaNbReqPending,DiaMaxReqPending);
  AdmPrint(cl,"diasendcount=%u\n",DiaSendCount);
  AdmPrint(cl,"diarecvcount=%u\n",DiaRecvCount);
  AdmPrint(cl,"diatimecount=%u\n",DiaTimeCount);
  AdmPrint(cl,"diadisconnect=%u\n",DiaDisconnected());
}

/**
 * @brief Call back function invoked when a command does not match any built-in command.
 * @param cl the CLI context.
 * @param buf the entered command line.
 * @return an integer that indicates the action to perform next (e.g. display the prompt).
 */
int
AdmCmd(t_cli *cl, char *buf)
{
  if (!strlen(buf))
    return CLI_PROMPT;

  if (strcmp(buf,"help") == 0 || buf[0] == '?') {
    AdmUsage(cl);
    return CLI_PROMPT;
  }

  if (strcmp(buf,"info") == 0) {
    AdmInfo(cl);
    return CLI_PROMPT;
  }

  if (strncmp(buf, "covcfg", 6) == 0) {
    //CovToCzDump(cl);
    return CLI_PROMPT;
  }

#define startby(x) \
    strncmp(buf, x, sizeof(x) - 1) == 0
    
  if (startby("ipu refresh descriptor")) {
    DiaIpuStart();
    return CLI_PROMPT;
  }


  if (startby("products fetch")) {
    char *ref = buf + sizeof("products fetch");
    ProductsGetDescription(ref);
    return CLI_PROMPT;
  }

  if (startby("products sync ")) {
    char *ref = buf + sizeof("products sync");
    void *xo = ProductsGetDescription(ref);
    if (xo != NULL) {
      ProductsSync(ref);
    }
    return CLI_PROMPT;
  }
  
  if (startby("products sync")) {
    ProductsSyncAll();
    return CLI_PROMPT;
  }

  if (startby("products list")) {
    ProductsList(cl);
    return CLI_PROMPT;
  }


  if (startby("network load cache")) {
    DriverLoadNetworksCache();
    return CLI_PROMPT;
  }

  if (startby("network write cache")) {
    DriverUpdateNetworksCache();
    return CLI_PROMPT;
  }
  
  if (startby("network list")) {
    NetworkList(cl);
    return CLI_PROMPT;
  }
  
  if (startby("network startall")) {
    NetworkStartAll();
    return CLI_PROMPT;
  }
  
  if (startby("network start")) {
    char *name = buf + sizeof("network start");
    Network_t *network = NetworkFindFromName(name);
    if (network == NULL) {
      AdmPrint(cl,"Unknown network '%s'\n", name);
      return CLI_PROMPT;
    }
    
    NetworkStart(network);
    return CLI_PROMPT;
  }

  if (startby("network create ethernet")) {
    char *name = buf + sizeof("network create ethernet");
    Network_t *network = NetworkCreateEthernet(name);
    if (network == NULL) {
      AdmPrint(cl,"Error\n");
      return CLI_PROMPT;
    }
    AdmPrint(cl,"Ok\n");
    return CLI_PROMPT;
  }
  
#define ERROR_IF_NULL(x) \
  if (x == NULL) {AdmPrint(cl, "Error on " xstr(x) "\n"); return CLI_PROMPT;}
  
  if (startby("network create serial")) {
    char *name = buf + sizeof("network create serial");
    ERROR_IF_NULL(name);
    
    char *modbusUART = index(name, ' ');
    ERROR_IF_NULL(modbusUART);
    *modbusUART = '\0';
    modbusUART++;
    
    char *modbusBaudrate = index(modbusUART, ' ');
    ERROR_IF_NULL(modbusBaudrate);
    *modbusBaudrate = '\0';
    modbusBaudrate++;
    
    char *modbusDataSize = index(modbusBaudrate, ' ');
    ERROR_IF_NULL(modbusDataSize);
    *modbusDataSize = '\0';
    modbusDataSize++;
    
    char *modbusParity = index(modbusDataSize, ' ');
    ERROR_IF_NULL(modbusParity);
    *modbusParity = '\0';
    modbusParity++;
    
    char *modbusStopSize = index(modbusParity, ' ');
    ERROR_IF_NULL(modbusStopSize);
    *modbusStopSize = '\0';
    modbusStopSize++;
    
    Network_t *network = NetworkCreateSerial(name, modbusUART, modbusBaudrate, modbusParity, modbusDataSize, modbusStopSize);
    if (network == NULL) {
      AdmPrint(cl,"Error\n");
      return CLI_PROMPT;
    }
    AdmPrint(cl,"Ok\n");

    return CLI_PROMPT;
  }
  
  if (startby("network devices")) {
    char *name = buf + sizeof("network devices");
    Network_t *network = NetworkFindFromName (name);
    if (network == NULL) {
      AdmPrint(cl, "Unknown network '%s'\n", name);
      return CLI_PROMPT;
    }
    
    NetworkListDevice(cl, network);
    return CLI_PROMPT;
  }

  if (startby("device interfaces")) {
    char *networkName = buf + sizeof("device interfaces");
    char *deviceName = index(networkName, ' ');
    if (deviceName == NULL) {
      AdmPrint(cl, "ERROR, can't read device name\n");
      return CLI_PROMPT;
    }
    
    *deviceName = '\0';
    deviceName++;
    
    Network_t *network = NetworkFindFromName (networkName);
    if (network == NULL) {
      AdmPrint(cl, "Unknown network '%s'\n", networkName);
      return CLI_PROMPT;
    }
    
    Sensor_t *device = DeviceFindFromName(network, deviceName);
    if (device == NULL) {
      AdmPrint(cl, "Unknown device '%s' in network '%s'\n", deviceName, networkName);
      return CLI_PROMPT;
    }
    
    DeviceListInterfaces(cl, device);
    return CLI_PROMPT;
  }
  
  if (startby("reader start")) {
    int rc = modbusReaderStart();
    if (rc == 0) {
      AdmPrint(cl, "Reader started\n");
    } else {
      AdmPrint(cl, "error (%d)\n", rc);
    }
    return CLI_PROMPT;
  }

  if (startby("reader stop")) {
    int rc = modbusReaderStop();
    if (rc == 0) {
      AdmPrint(cl, "Reader stopped\n");
    } else {
      AdmPrint(cl, "error (%d)\n", rc);
    }
    return CLI_PROMPT;
  }
  
  if (startby("reader status")) {
    modbusReaderStatus(cl);
    return CLI_PROMPT;
  }

  return CLI_NFOUND;
}
