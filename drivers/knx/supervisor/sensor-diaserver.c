#include <ctype.h>

#include "knx.h"

/**
 * @brief Call back from retargeted operations on the M2M IPU application.
 * @param ident the IPU identifier (i.e. KNX)
 * @param targetlevel The access right level (i.e. 1, 2 or 3)
 * @param obix the request content decoded as oBIX buffer.
 * @param targetid the request URI of the dIa incoming request.
 * @param reqId the dIa request identifier to provide on completion.
 */
int
IpuExecuteMethod(char *ident,int targetlevel,void *obix,char *targetid, int tid)
{
    int ret;
    char *name;
    NetworkList_t *network;

    RTL_TRDBG(0, "%s\n", __FUNCTION__);
    NOT_USED(ident);
    NOT_USED(targetlevel);
    NOT_USED(targetid);
    NOT_USED(tid);
    
    /*
        Here, a sample of Obix object
        
        <?xml version="1.0" encoding="UTF-8"?>
        <obix:obj xmlns:obix="http://obix.org/ns/schema/1.1" href="iecCreateInstance">
            <obix:str name="knxNetworkID" val="myNetwork" />
        </obix:obj>
    */
    
    name = rindex(targetid, '/');
    if (name == NULL || *name == '\0' || *(name + 1) == '\0' || strcmp(name + 1, "knxCreateNetwork")) {
        RTL_TRDBG(1, "Unknown operation\n");
        return -50; // STATUS_BAD_REQUEST
    }

    name = (char *) XoNmGetAttr(obix, "wrapper$[name=knxNetworkID].val", NULL);
    if (name == NULL) {
        RTL_TRDBG(1, "knxNetworkID not found\n");
        return -50; // STATUS_BAD_REQUEST
    } 

    // for now on, the GIP only support single network mode
    // If a network already exists, reject the request.
    if (SensorNetworksListEmpty() == 0) {
        RTL_TRDBG(1, "network already created... skipped\n");
        // impossible to send STATUS_CONFLICT with drvcommon for now on
        return -50; // STATUS_BAD_REQUEST
    }

    // Create internal object for the network
    network = SensorCreateNetwork(name);
    if (network == NULL) {
        RTL_TRDBG(0, "SensorCreateNetwork failed\n");
        return -100; // STATUS_INTERNAL_SERVER_ERROR
    }
    
    // Fetch the topology
    ret = DiaTopologyFetch(network, NULL);
    if (ret != 0) {
        RTL_TRDBG(0, "DiaTopologyFetch busy\n");
        return -100; // STATUS_INTERNAL_SERVER_ERROR
    }

    // If the topology fetch failed, a empty network will be created
    // The load operation on the network may be used to reload a new valid topology later
    return 0;   // STATUS_CREATED
}

/**
 * @brief Call back from retargeted operations on the M2M network application.
 * @param ident the network identifier (i.e. the PAN IEEE address, in hex format)
 * @param targetlevel The access right level (i.e. 1, 2 or 3)
 * @param obix the request content decoded as oBIX buffer.
 * @param targetid the request URI of the dIa incoming request.
 * @param reqId the dIa request identifier to provide on completion.
 */
int
CmnNetworkExecuteMethod(char *ident,int targetlevel,void *obix,char *targetid,int tid)
{
    int ret;
    
    RTL_TRDBG(0, "%s\n", __FUNCTION__);
    NOT_USED(targetlevel);
    NOT_USED(targetid);
    NOT_USED(tid);
    
    // Ensure the ident exists 
    NetworkList_t *network = SensorFindNetworkFromName(ident);
    if (network == NULL) {
        RTL_TRDBG(0, "Network don't exists\n");
        return -50;
    }
    
    char *ope = rindex(targetid, '/') + 1;
    if (ope == NULL || *ope == '\0') {
        RTL_TRDBG(0, "Can't read operation\n");
        return -50;
    }
    
    if (strcmp(ope, "knxDeleteNetwork") == 0) {
        SensorDeleteNetwork(network);
    } else
    if (strcmp(ope, "knxUpdateNetwork") == 0) {
        ret = DiaTopologyFetch(network, NULL);
        if (ret != 0) {
            RTL_TRDBG(0, "DiaTopologyFetch busy\n");
            return -50; // STATUS_BAD_REQUEST
        }
    } else {
        RTL_TRDBG(0, "Unknown operation '%s' on network '%s'\n", ope, network->name);
        return -50;
    }
    
    return 0;
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
    int i, j, rc;
    uint8_t knxOut[1024];
    int knxOutLen = 0;
    t_sensor *cp = container_of(cmn, t_sensor, cp_cmn);
    
    RTL_TRDBG(5, "%s\n", __FUNCTION__);
    NOT_USED(app);
    NOT_USED(cluster);
    NOT_USED(numm);
    NOT_USED(targetlevel);
    NOT_USED(targetid);
    NOT_USED(tid);

    /*
        Here, a sample of Obix object
        
        <obix:bool xmlns:obix="http://obix.org/ns/schema/1.1" name="knxData" val="true">
            <bool name="b" val="true" />
            <str name="raw" val="x01" />
        </obix:bool>
    */

    // Raw Mode
    char *raw = (char *) XoNmGetAttr(obix, "wrapper$[name=raw].val", NULL);
    if (raw != NULL && *raw != '\0') {
        // Remove "0x" or "x" prefix 
        if (*raw == 'x') {
            raw += 1;
        } else if (*raw == '0' && *(raw + 1) == 'x') {
            raw += 2;
        }
        
        int len = strlen(raw);
        if (len % 2 != 0) {
            RTL_TRDBG(0, "Missing hexa caracter.\n");
            return -50;
        }
        char *buffer = (char *)malloc(len / 2);
        memset(buffer, 0, len / 2);
        for(i=0, j=0; i<len; j++, i+=2) {
            char c = tolower(raw[i]);
            if (c < 'a') {
                buffer[j] |= ((c - '0') << 4);
            } else {
                buffer[j] |= ((c - 'a' + 10) << 4);
            }

            c = tolower(raw[i + 1]);
            if (c < 'a') {
                buffer[j] |= (c - '0');
            } else {
                buffer[j] |= (c - 'a' + 10);
            }
        }
        
        // DPT with size under one Byte can't use the raw buffer size (#617)
        memset(knxOut, 0, 1024);
        obix2knx(cp->dptId, obix, knxOut, &knxOutLen);  // Fake decode to get the len
        //RTL_TRDBG(0, "knxOutLen = %u\n", knxOutLen);
        rc = EibdSendGroupValueAPDU (cp->network, cp->knxId, buffer, knxOutLen);
        free(buffer);
        
        return (rc == 0) ? 0 : -503;
    }

    // XML Mode
    memset(knxOut, 0, 1024);
    obix2knx(cp->dptId, obix, knxOut, &knxOutLen);
    rc = EibdSendGroupValueAPDU (cp->network, cp->knxId, knxOut, knxOutLen);
    return (rc == 0) ? 0 : -503;
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

    RTL_TRDBG(0, "%s\n", __FUNCTION__);
    RTL_TRDBG(0, "\tcmn = %p\n", cmn);
    RTL_TRDBG(0, "\tapp = %d\n", app);
    RTL_TRDBG(0, "\tcluster = %d\n", cluster);
    RTL_TRDBG(0, "\tnumm = %d\n", numm);
    RTL_TRDBG(0, "\ttargetlevel = %d\n", targetlevel);
    RTL_TRDBG(0, "\ttargetid = %s\n", targetid);
    RTL_TRDBG(0, "\ttid = %d\n", tid);

    // Answer a wrong request URI
    return -1;
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

    RTL_TRDBG(0, "%s\n", __FUNCTION__);
    RTL_TRDBG(0, "\tcmn = %p\n", cmn);
    RTL_TRDBG(0, "\tapp = %d\n", app);
    RTL_TRDBG(0, "\tcluster = %d\n", cluster);
    RTL_TRDBG(0, "\tnumm = %d\n", numm);
    RTL_TRDBG(0, "\ttargetlevel = %d\n", targetlevel);
    RTL_TRDBG(0, "\tobix = %p\n", obix);
    RTL_TRDBG(0, "\ttargetid = %s\n", targetid);
    RTL_TRDBG(0, "\ttid = %d\n", tid);
    
    // Answer a wrong request URI
    return -1;
}
