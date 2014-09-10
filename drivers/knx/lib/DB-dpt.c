#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>

#include "knx.h"

static void *dptxo = NULL;

/**
 * @brief Load the KNX DPT database
 * @return  0 on success
 *         -1 if file can't be read
 *         -2 if xml can't be parse
 */
int
DptLoadDatabase(void)
{
	int	parse;
    char file_path[PATH_MAX];
    
    if (!Operating) {
        snprintf (file_path, PATH_MAX, "%s/dpt.xml", getenv(DRIVER_DATA_DIR));
    } else {
        snprintf(file_path, PATH_MAX, "%s/usr/data/%s/dpt.xml", rootactPath, GetAdaptorName());
    }
    
	if (access(file_path, R_OK) != 0) {
		RTL_TRDBG(0, "DPT definition file does not exist: %s\n", file_path);
		dptxo = NULL;
		return -1;
	}

	dptxo = XoReadXmlEx(file_path, NULL, 0, &parse);
	if (dptxo == NULL) {
		RTL_TRDBG(0, "ERROR ret=%d cannot read xml '%s'\n", parse, file_path);
		return -2;
	}

#if _DEBUG_SHOW_DB
	char *buffer;
	void *w = XoWritXmlMem(dptxo, XO_FMT_STD_XML, &buffer, "");
	if (!w) {
		printf("XoWritXmlMem FAILED\n");
	} else {
		printf("%s\n", buffer);
		XoWritXmlFreeCtxt(w);
	}
	free(buffer);
#endif

	return 0;
}

/**
 * @brief Get informations about a specific DPT
 * @param dpt The DPT code to search
 * @param infos Structure to fill with DPT informations
 * @return  0 on success
 *         -1 if database is not load
 *         -2 if database is empty
 *         -3 if the primary code of the DPT is not found
 */
int
DptGetInfos(Dpt_t dpt, DptInfos_t *infos)
{
    int i, nb;
    void *elem, *elem2;
    
    if (dptxo == NULL)
        return -1;
    
    memset(infos, 0, sizeof(infos));
    infos->dpt = dpt;
    
	nb = XoNmNbInAttr(dptxo, "knx:DatapointType", 0);
    if (nb <= 0) {
        return -2;        
    }
    
    /*
     *  Search the DatapointType based on the primary dpt code
     */
    for	(i=0; i<nb; i++) {
	    elem = XoNmGetAttr(dptxo, "knx:DatapointType[%d]", 0, i);
	    if (elem == NULL) {
	        continue;
        }
        
        char *Number = XoNmGetAttr(elem, "knx:id", 0);
        if (Number == NULL) {
            continue;
        }
        if (atoi(Number) != dpt.primary) {
            continue;
        }
        
        break;
    }
    if (i == nb) {
        // Primary code not found
        return -3;
    }

    char *sizeinbit = XoNmGetAttr(elem, "knx:sizeinbit", 0);
    if (sizeinbit == NULL) {
        infos->bits = 0;
    } else {
        infos->bits = atoi(sizeinbit);
    }
/*
    char *unit = XoNmGetAttr(elem, "knx:unit", 0);
    if (unit == NULL) {
        infos->unit = NULL;
    } else {
        infos->unit = unit;
    }
    */
    /*
     *  Search the DatapointSubtype based on the sub dpt code
     */
	nb = XoNmNbInAttr(elem, "knx:DatapointSubtype", 0);
    if (nb > 0) {
        for	(i=0; i<nb; i++) {
            elem2 = XoNmGetAttr(elem, "knx:DatapointSubtype[%d]", 0, i);
            if (elem2 == NULL) {
                continue;
            }
            
            char *Number = XoNmGetAttr(elem2, "knx:id", 0);
            if (Number == NULL) {
                continue;
            }
            if (atoi(Number) != dpt.sub) {
                continue;
            }
            
            break;
        }
        if (i != nb) {
            elem = elem2;    
        }
    }
    
    char *Name = XoNmGetAttr(elem, "knx:name", 0);
    if (Name == NULL) {
        infos->name = "Unknown";
    } else {
        infos->name = Name;
    }
    
    char *unit = XoNmGetAttr(elem, "knx:unit", 0);
    if (unit == NULL) {
        infos->unit = "obix:units/null";
    } else {
        infos->unit = unit;
    }
    
    return 0;
}

/**
 * @brief Convert a ASCII DPT code to binary DPT code
 * @param in ASCII string of DPT code
 * @return Binary version of the DPT code (Dpt_t)
 */
Dpt_t
DptAsciiToBin(char *in)
{
    int rc;
    Dpt_t dpt;
    
    rc = sscanf(in, "%hu.%hu", &(dpt.primary), &(dpt.sub));
    if (rc != 2) {
        dpt.primary = 0;
        dpt.sub = 0;
    }
    
    return dpt;
}

/**
 * @brief Find the interface template to use into M2M Application from a DPT code
 * @param dpt The DPT code to search
 * @return Dia template name as string
 */
char *
DptGetInterfaceTemplateFromCode(Dpt_t dpt)
{
    switch(dpt.primary) {
        case 1:
            return "itf_inst_ga_1.xml";

        case 2:
            return "itf_inst_ga_2.xml";

        case 3:
            return "itf_inst_ga_3.xml";

        case 4:
            switch(dpt.sub) {
                case 1:
                    return "itf_inst_ga_28.xml";

                case 2:
                    return "itf_inst_ga_4.xml";
                    
                default:
                    return NULL;
            }
            break;

        case 5:
            return "itf_inst_ga_5.xml";

        case 6:
            switch(dpt.sub) {
                case 1:
                case 10:
                    return "itf_inst_ga_6_1.xml";

                default:
                    return "itf_inst_ga_6_20.xml";
            }
            break;

        case 7:
            switch(dpt.sub) {
                case 1:
                case 10:
                case 11:
                case 12:
                case 13:
                    return "itf_inst_ga_7_1.xml";

                default:
                    return "itf_inst_ga_7_2.xml";
            }
            break;

        case 8:
            switch(dpt.sub) {
                case 1:
                case 10:
                case 11:
                    return "itf_inst_ga_8_1.xml";

                default:
                    return "itf_inst_ga_8_2.xml";
            }
            break;

        case 9:
            return "itf_inst_ga_9.xml";

        case 10:
            return "itf_inst_ga_10.xml";

        case 11:
            return "itf_inst_ga_11.xml";

        case 12:
            return "itf_inst_ga_12.xml";

        case 13:
            switch(dpt.sub) {
                case 1:
                    return "itf_inst_ga_13_1.xml";

                default:
                    return "itf_inst_ga_13_100.xml";
            }
            break;

        case 14:
            return "itf_inst_ga_14.xml";

        case 15:
            return "itf_inst_ga_15.xml";

        case 16:
            switch(dpt.sub) {
                case 0:
                    return "itf_inst_ga_28.xml";

                case 1:
                    return "itf_inst_ga_4.xml";

                default:
                    return NULL;
            }
            break;

        case 17:
            return "itf_inst_ga_17.xml";

        case 18:
            return "itf_inst_ga_18.xml";

        case 19:
            return "itf_inst_ga_19.xml";

        case 20:
            return "itf_inst_ga_20.xml";

        case 21:
            switch(dpt.sub) {
                case 1:
                    return "itf_inst_ga_21_1.xml";

                case 2:
                    return "itf_inst_ga_21_2.xml";

                default:
                    return NULL;
            }
            break;

        case 23:
            return "itf_inst_ga_23.xml";

        case 24:
            return "itf_inst_ga_4.xml";

        case 26:
            return "itf_inst_ga_26.xml";

        case 27:
            return "itf_inst_ga_27.xml";

        case 28:
            return "itf_inst_ga_28.xml";

        case 217:
            return "itf_inst_ga_217.xml";

        case 219:
            return "itf_inst_ga_219.xml";

        case 221:
            return "itf_inst_ga_221.xml";

       case 202:
            switch(dpt.sub) {
                case 1:     // 5.004
                case 2:     // 5.010
                    return "itf_inst_ga_202_1.xml";
                    break;

                default:
                    return NULL;
            }
            break;

        case 203:
            switch(dpt.sub) {
                case 2:
                    return "itf_inst_ga_203_2.xml";

                case 3:
                    return "itf_inst_ga_203_2.xml";

                case 4:
                    return "itf_inst_ga_203_2.xml";

                case 5:
                    return "itf_inst_ga_203_2.xml";

                case 6:
                    return "itf_inst_ga_203_2.xml";

                case 7:
                    return "itf_inst_ga_203_2.xml";

                case 11:
                case 13:
                    return "itf_inst_ga_203_11.xml";

                case 12:
                    return "itf_inst_ga_203_12.xml";

                case 14:
                    return "itf_inst_ga_203_14.xml";

                case 15:
                    return "itf_inst_ga_203_14.xml";

                case 17:
                    return "itf_inst_ga_203_17.xml";

                default:
                    return NULL;
            }
            break;

        case 204:
            switch(dpt.sub) {
                case 1:
                    return "itf_inst_ga_204_1.xml";

                default:
                    return NULL;
            }
            break;

        case 205:
            switch(dpt.sub) {
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    return "itf_inst_ga_205_2.xml";

                default:
                    return NULL;
            }
            break;

        case 218:
            switch(dpt.sub) {
                case 1:
                    return "itf_inst_ga_218_1.xml";

                default:
                    return NULL;
            }
            break;
            
        case 229:
            switch(dpt.sub) {
                case 1:
                    return "itf_inst_ga_229.xml";

                default:
                    return NULL;
            }
            break; 
            
        default:
            return NULL;
    }
    
    return NULL;
}
