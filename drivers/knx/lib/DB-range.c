#include <unistd.h>
#include <linux/limits.h>

#include "knx.h"

static void *rangesxo = NULL;

/**
 * @brief Load the KNX Range database
 * @return  0 on success
 *         -1 if file can't be read
 *         -2 if xml can't be parse
 */
int
RangeLoadDatabase(void)
{
	int	parse;
    char file_path[PATH_MAX];
    
    if (!Operating) {
        snprintf (file_path, PATH_MAX, "%s/ranges.xml", getenv(DRIVER_DATA_DIR));
    } else {
        snprintf(file_path, PATH_MAX, "%s/usr/data/%s/ranges.xml", rootactPath, GetAdaptorName());
    }
    
	if (access(file_path, R_OK) != 0) {
		RTL_TRDBG(0, "DPT definition file does not exist: %s\n", file_path);
		rangesxo = NULL;
		return -1;
	}

	rangesxo = XoReadXmlEx(file_path, NULL, XOML_PARSE_OBIX, &parse);
	if (rangesxo == NULL) {
		RTL_TRDBG(0, "ERROR ret=%d cannot read xml '%s'\n", parse, file_path);
		return -2;
	}
	
#if _DEBUG_SHOW_DB
	char *buffer;
	void *w = XoWritXmlMem(rangesxo, XO_FMT_STD_XML, &buffer, "");
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
 * @brief Stringify a binary value
 * @param dpt The DPT code to use (KNX type)
 * @param value The binary value of the enum
 * @param output The value stringified
 * @return  0 on success
 *         -1 if database is not load
 */
int
RangeGetRenderedValue(Dpt_t dpt, int value, char **output)
{
    int rc;
    char href[1024];
    
    if (rangesxo == NULL)
        return -1;
    
    snprintf(href, 1024, "wrapper$[href=knx:ranges/dpt%hu_%03hu].wrapper$[val=%d].name",
                         dpt.primary,
                         dpt.sub,
                         value);
    rc = getObixStrValue(rangesxo, *output, href);
    if (rc != 0) {
        *output = "";
    }
    
    return 0;
}

/**
 * @brief Convert a binary value to its ASCII string representation 
 * @param dpt The DPT code to use (KNX type)
 * @param value The binary representation of the input
 * @param input The value stringified to convert
 * @return  0 on success
 *         -1 if database is not load
 */
int
RangeGetBinValue(Dpt_t dpt, int *value, char *input)
{
    int rc;
    char href[1024];
    
    if (rangesxo == NULL)
        return -1;
    
    snprintf(href, 1024, "wrapper$[href=knx:ranges/dpt%hu_%03hu].wrapper$[name=%s].val",
                         dpt.primary,
                         dpt.sub,
                         input);
    rc = getObixIntValue(rangesxo, value, href);
    if (rc != 0) {
        *value = 0;
    }
    
    return 0;
}
