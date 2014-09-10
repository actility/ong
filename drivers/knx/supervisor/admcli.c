#include <sys/types.h>
#include <unistd.h>

#include "knx.h"

/**
 * @brief Display on CLI a help message about the supported commands.
 * @param cl the CLI context.
 */
void
AdmUsage(t_cli *cl)
{
    AdmPrint(cl,"quit|exit\n");
    AdmPrint(cl,"stop\n");
    AdmPrint(cl,"who\n");
    AdmPrint(cl,"close\n");
    AdmPrint(cl,"core\n");
    AdmPrint(cl,"xoocc\n");

    AdmPrint(cl,"trace [level]\n");
    AdmPrint(cl,"getvar [varname]\n");
    AdmPrint(cl,"setvar varname [value]\n");
    AdmPrint(cl,"addvar varname\n");

    AdmPrint(cl,"help\n");
    AdmPrint(cl,"info\n");
    AdmPrint(cl,"covcfg\n");
    
    /*
     *  KNX Stuff
     */
    AdmPrint(cl, "create network <name>\n");
    AdmPrint(cl, "delete network <name>\n");
    AdmPrint(cl, "load networks\n");
    AdmPrint(cl, "init networks\n");
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

	vsize	= rtl_vsize(getpid());

	nbm	= rtl_imsgVerifCount(MainIQ);
	nbt	= rtl_timerVerifCount(MainIQ);

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
		CovToCzDump(cl);
		return CLI_PROMPT;
	}
	
    /*
     *  KNX Stuff
     */
    if (strcmp(buf,"load networks") == 0) {
        AdmPrint(cl,"Ensure each network in the driver cache folder is loaded.\n");
		SensorLoadNetworksFromFiles();
		SensorDisplayNetworksListOnCli(cl);
		return CLI_PROMPT;
	}
	
    if (strcmp(buf,"init networks") == 0) {
		SensorStartupNetworks();
		AdmPrint(cl,"Ensure the state machine to initialize M2M applications per networks is running.\n");
		return CLI_PROMPT;
	}

    if (strncmp(buf, "create network ", 15) == 0) {
		SensorCreateNetwork(buf + 15);
		return CLI_PROMPT;
	}

    if (strncmp(buf, "delete network ", 15) == 0) {
		SensorDeleteNetworkFromName(buf + 15);
		return CLI_PROMPT;
	}

	return CLI_NFOUND;
}
