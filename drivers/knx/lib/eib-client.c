

#include "knx.h"
#include "eibclient.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

static
int
EibdFindAvailablePort(void)
{
    int port;
    bool found;

    struct list_head *index;
    NetworkList_t *network = NULL;
    
    found = false;
    for(port=10000; port<20000; port++) {
        found = false;
        list_for_each(index, &(NetworkList.list)) {
            network = list_entry(index, struct _NetworkList_t, list);

            if (network->eibdPort == port) {
                found = true;
                break;
            }
        }
        
        if (found == false) {
            break;
        }
    }
    
    return port;
}

static
int
EibdEventPollCallback(void *tb, int fd, void *r1, void *r2, int revents)
{
    return POLLIN;
}

static
int
EibdRequestPollCallback(void *tb, int fd, void *r1, void *r2, int revents)
{
    int ret;
    eibaddr_t dest;
    eibaddr_t src;
    unsigned char buffer[200];
    char *tpl;
    int parseflags = XOML_PARSE_OBIX;
    NetworkList_t *network = (NetworkList_t *)r1;
    
    if (revents & POLLIN) {
        ret = EIBGetGroup_Src (network->eibdGroupReadCon, sizeof (buffer), buffer, &src, &dest);
        if (ret > 0) {
        
#if 0
            // Debug
            RTL_TRDBG(2, "From : %d.%d.%d\n", (src >> 11) & 0x1f, (src >> 8) & 0x07, (src) & 0xff);
            RTL_TRDBG(2, "To : %d/%d/%d\n", (dest >> 11) & 0x1f, (dest >> 8) & 0x07, (dest) & 0xff);
#endif

            // Search the Group Addr context
            struct list_head *index;
            t_sensor *cp = NULL;
            list_for_each(index, &(network->groupApp)) {
                cp = list_entry(index, t_sensor, list);

                if (cp->knxId == dest) {
                    break;
                }
                
                cp = NULL;
            }

            // Report from a group addr not in the project, ignore it
            // Or not configured in the modelconfig.xml
            if (cp == NULL) {
                RTL_TRDBG(2, "Group addr %d not define in the project or not on the modeling file\n", dest);
                return 0;
            }
            
            // Create a report
            if (ret == 2) {
                buffer[1] &= 0x3F;
                knx2obix(cp->dptId, &(buffer[1]), &tpl);
            } else {
                knx2obix(cp->dptId, &(buffer[2]), &tpl);
            }
            appendReport(cp, WXoNewTemplate(tpl, parseflags));
        }
    }
    
    return 0;
}

void
EibdSetNetworkNotReachable(NetworkList_t *network)
{
    network->eibdGatewayURI = NULL;
    network->eibdURI = NULL;
    network->eibdGroupReadCon = NULL;
}

void
EibdConnectToDaemon(NetworkList_t *network)
{
    int ret;
    
    // Connect to the EIBD instance
    network->eibdGroupReadCon = EIBSocketURL(network->eibdURI);
    if (network->eibdGroupReadCon == NULL) {
        RTL_TRDBG(0, "Connexion to EIBD failed, URI = %s\n", network->eibdURI);
        goto error;
    }
    
    ret = EIBOpen_GroupSocket (network->eibdGroupReadCon, 0);
    if (ret == -1) {
        RTL_TRDBG(0, "EIBOpen_GroupSocket : Connect failed\n");
        EIBClose (network->eibdGroupReadCon);
        network->eibdGroupReadCon = NULL;
        goto error;
    }
    
    // Add the EIBD client in the global poll system
    rtl_pollAdd(MainTbPoll, EIB_Poll_FD (network->eibdGroupReadCon), EibdRequestPollCallback, EibdEventPollCallback, network, NULL);
    return;
    
error:
     RTL_TRDBG(0, "Fail to connect to EIBD for network: %s\n", network->name);
    // FFS Fire event to retry   
}

/*
 * start EIBD like this bash command
 * eibd --Discovery --Server --Tunnelling --listen-tcp=10000 --trace=10 ipt:10.10.12.52 2>&1 | stdin-logger /home/ong/var/log/knx/ cvm 200 5 1 1
 */
void
EibdStartDaemon(NetworkList_t *network)
{
    pid_t pid;
    int des_p[2];

    if(pipe(des_p) == -1) {
        RTL_TRDBG(0, "Pipe creation FAILED for network: %s, EIBD not started\n", network->name);
        return;
    }

    // Start EIBD
    pid = fork();
    if (pid == -1) {
        RTL_TRDBG(0, "Fork FAILED for network: %s, EIBD not started\n", network->name);
        return;
    }
    if (pid == 0) {
        char path[NAME_MAX];
        char *args[32];
        char tcp[128];
        int argc = 0;
        char *p1 = NULL, *p2 = NULL, *p3 = NULL;

        // Setup pipes to stdin-logger
        close(1);          //closing stdout
        (void) dup(des_p[1]);     //replacing stdout with pipe write 
        close(des_p[0]);   //closing pipe read
        close(des_p[1]);

        // Redirect stderr to stdout
        dup2(1, fileno(stderr));


        p1 = GetVar("eibdoption1");
        p2 = GetVar("eibdoption2");
        p3 = GetVar("eibdoption3");
        
        // Close FDs
        AdmTcpClose();   
        DiaUdpClose();     
        
        // Build args from wrapper config
        args[argc++] = "eibd";
        args[argc++] = "--Discovery";
        args[argc++] = "--Server";
        args[argc++] = "--Tunnelling";
        snprintf(tcp, sizeof(tcp), "--listen-tcp=%u", network->eibdPort);
        args[argc++] = tcp;
        
        if (p1 && *p1) {
            args[argc++] = p1;
        }
        
        if (p2 && *p2) {
            args[argc++] = p2;
        }
        
        if (p3 && *p3) {
            args[argc++] = p3;
        }

        args[argc++] = network->eibdGatewayURI;
        args[argc++] = NULL;
        
        // Run EIBD
        if (!Operating) {
            snprintf(path, NAME_MAX, "%s/%s/contrib/root/bin/eibd", rootactPath, GetAdaptorName());
        } else {
            snprintf(path, NAME_MAX, "%s/bin/eibd", rootactPath);
        }
        
#if 0
        RTL_TRDBG(0, "execvp on %s\n", path);
        argc = 0;
        while(args[argc] != NULL) {
            RTL_TRDBG(0, "\t%s\n", args[argc]);
            argc++;
        }
#endif
        execvp(path, args);
        RTL_TRDBG(0, "execvp FAILED for network: %s, %s\n", network->name, strerror(errno));
         
        // Kill the child, if execvp failed
        exit(0);
    } else {
        RTL_TRDBG(1, "Starting EIBD for network: %s, pid = %u\n", network->name, pid);
        network->eibdPid = pid;
    }

    // Start stdin-logger
    pid = fork();
    if (pid == -1) {
        RTL_TRDBG(0, "Fork FAILED for network: %s, EIBD not started\n", network->name);
        return;
    }
    if (pid == 0) {
        char path[NAME_MAX];
        char logFolder[NAME_MAX];
        char *args[32];
        int argc = 0;

        // Setup pipes from eibd
        close(0);          //closing stdin
        (void) dup(des_p[0]);     //replacing stdin with pipe read
        close(des_p[1]);   //closing pipe write
        close(des_p[0]);

        if (!Operating) {
            snprintf(path, sizeof(path), "%s/%s/contrib/root/bin/stdin-logger", rootactPath, GetAdaptorName());
            snprintf(logFolder, sizeof(logFolder), "%s/%s/", rootactPath, GetAdaptorName());
        } else {
            snprintf(path, sizeof(path), "%s/bin/stdin-logger", rootactPath);
            snprintf(logFolder, sizeof(logFolder), "%s/var/log/%s/", rootactPath, GetAdaptorName());
        }
        
        args[argc++] = "stdin-logger";
        args[argc++] = logFolder;
        args[argc++] = "eibd";
        args[argc++] = "200";
        args[argc++] = "5";
        args[argc++] = "1";
        args[argc++] = "1";
        args[argc++] = NULL;
        
#if 0
        RTL_TRDBG(0, "execvp on %s\n", path);
        argc = 0;
        while(args[argc] != NULL) {
            RTL_TRDBG(0, "\t%s\n", args[argc]);
            argc++;
        }
#endif
        execvp(path, args);
        RTL_TRDBG(0, "execvp FAILED for network: %s, %s\n", network->name, strerror(errno));
         
        // Kill the child, if execvp failed
        exit(0);
    } else {
        RTL_TRDBG(1, "Starting stdin-logger for network: %s, pid = %u\n", network->name, pid);
        network->stdinLoggerPid = pid;
    }


    // Let's some time to EIBD to start
    sleep(1);
    EibdConnectToDaemon(network);
}

int
EibdCleanup(NetworkList_t *network)
{
    if (network->eibdURI != NULL) {
        free(network->eibdURI);
        network->eibdURI = NULL;
    }
    
    if (network->eibdGatewayURI != NULL) {
        free(network->eibdGatewayURI);
        network->eibdGatewayURI = NULL;
        
        return EibdExit(network);
    }
    
    return 0;
}

int
EibdExit(NetworkList_t *network)
{
    int ret;

    ret = rtl_pollRmv(MainTbPoll, EIB_Poll_FD (network->eibdGroupReadCon));

    // Terminate EIBD
    RTL_TRDBG(1, "Send SIGTERM to pid %u\n", network->eibdPid);
    kill(network->eibdPid, SIGTERM);
    RTL_TRDBG(1, "Send SIGTERM to pid %u\n", network->stdinLoggerPid);
    kill(network->stdinLoggerPid, SIGTERM);

    return ret;
}

bool
EibdIsRunning(NetworkList_t *network)
{
    char path[NAME_MAX];
    
    // Check if really started
    if (network->eibdPid == 0) {
        return false;
    }
    
    // Check if the pid exists
    snprintf(path, NAME_MAX, "/proc/%d/status", network->eibdPid);
    if (access(path, R_OK) != 0) {
        RTL_TRDBG(0, "EibdIsRunning: unable to access EIBD proc file for network %s\n", network->name);
        return false;
    }

    return true;
}

void
EibdCleanupAll(void)
{
    struct list_head *index;
    NetworkList_t *network;
    
    list_for_each(index, &(NetworkList.list)) {
        network = list_entry(index, struct _NetworkList_t, list);
        EibdCleanup(network);
    }
}

void
EibdInit(char *uri, NetworkList_t *network)
{
    if (EibdIsRunning(network)) {
        EibdCleanup(network);
    }

    (void) asprintf(&(network->eibdGatewayURI), "ipt:%s", uri);
    network->eibdPort = EibdFindAvailablePort();
    (void) asprintf(&(network->eibdURI), "ip:127.0.0.1:%d", network->eibdPort);
    
    EibdStartDaemon(network);
}

/**
 *
 * APDU is defined in KNX 1.1 Volume 3: System Specifications, Part 3 Communication, Chapter 7, Application Layer.
 * In the Common EMI frame, the APDU payload is defined as follows:
 *
 *  +--------+--------+--------+--------+--------+
 *  | TPCI + | APCI + |  Data  |  Data  |  Data  |
 *  |  APCI  |  Data  |        |        |        |
 *  +--------+--------+--------+--------+--------+
 *    byte 1   byte 2  byte 3     ...     byte 16
 *
 * For data that is 6 bits or less in length, only the first two bytes are used in a Common EMI
 * frame. Common EMI frame also carries the information of the expected length of the Protocol
 * Data Unit (PDU). Data payload can be at most 14 bytes long.
 *
 * The first byte is a combination of transport layer control information (TPCI) and application
 * layer control information (APCI). First 6 bits are dedicated for TPCI while the two least
 * significant bits of first byte hold the two most significant bits of APCI field, as follows:
 *
 *    Bit 1    Bit 2    Bit 3    Bit 4    Bit 5    Bit 6    Bit 7    Bit 8      Bit 1   Bit 2
 *  +--------+--------+--------+--------+--------+--------+--------+--------++--------+---- - - -
 *  |        |        |        |        |        |        |        |        ||        |
 *  |  TPCI  |  TPCI  |  TPCI  |  TPCI  |  TPCI  |  TPCI  | APCI   |  APCI  ||  APCI  |
 *  |        |        |        |        |        |        |(bit 1) |(bit 2) ||(bit 3) |
 *  +--------+--------+--------+--------+--------+--------+--------+--------++--------+---- - - -
 *  +                            B  Y  T  E    1                            ||       B Y T E  2
 *  +-----------------------------------------------------------------------++------------- - - -
 *
 *
 * Total number of APCI control bits can be either 4 or 10, depending on which application layer service
 * is being used. The second byte bit structure is as follows:
 *
 *    Bit 1    Bit 2    Bit 3    Bit 4    Bit 5    Bit 6    Bit 7    Bit 8      Bit 1   Bit 2
 *  +--------+--------+--------+--------+--------+--------+--------+--------++--------+---- - - -
 *  |        |        |        |        |        |        |        |        ||        |
 *  |  APCI  |  APCI  | APCI/  |  APCI/ |  APCI/ |  APCI/ | APCI/  |  APCI/ ||  Data  |  Data
 *  |(bit 3) |(bit 4) | Data   |  Data  |  Data  |  Data  | Data   |  Data  ||        |
 *  +--------+--------+--------+--------+--------+--------+--------+--------++--------+---- - - -
 *  +                            B  Y  T  E    2                            ||       B Y T E  3
 *  +-----------------------------------------------------------------------++------------- - - -
 *
 */

/*
 *  A_GroupValue_Write-PDU -> APCI 4bits = 0b0010
 *  0x80, when mapped on the 2nd byte of APDU
 */
int
EibdSendGroupValueAPDU (NetworkList_t *network, short dest, char *data, int bitdatalen)
{
    int len, ret;
    EIBConnection *con;
    unsigned char buffer[16] = {0, 0x80};

    // Ensure a eib deamon is available
    if (network->eibdURI == NULL) {
        return -1;
    }

    con = EIBSocketURL (network->eibdURI);
    if (con == NULL) {
        RTL_TRDBG(0, "EIBSocketURL failed\n");
        return -1;
    }

    ret = EIBOpenT_Group (con, dest, 1);
    if (ret == -1) {
        RTL_TRDBG(0, "EIBOpenT_Group failed\n");
        EIBClose (con);
        return -1;
    }

    if (bitdatalen <= 6) {
        len = 0;
        buffer[1] |= data[0] & 0x3f;
    } else {
        len = bitdatalen / 8;
        if (bitdatalen & 0x07) len++;
        
        memcpy(buffer + 2, data, len);
    }

    ret = EIBSendAPDU (con, 2 + len, buffer);
    if (ret == -1) {
        RTL_TRDBG(0, "EIBSendAPDU failed\n");
        EIBClose (con);
        return -1;
    }

    EIBClose (con);
    return 0;
}



