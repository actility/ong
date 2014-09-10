#include <unistd.h>
#include <linux/limits.h>

#include "knx.h"

static void *manxo = NULL;

/**
 * @brief Load the KNX Manufacturer database
 * @return  0 on success
 *         -1 if file can't be read
 *         -2 if xml can't be parse
 */
int
ManufacturerLoadDatabase(void)
{
	int	parse;
    char file_path[PATH_MAX];
    
    if (!Operating) {
        snprintf (file_path, PATH_MAX, "%s/manufacturer.xml", getenv(DRIVER_DATA_DIR));
    } else {
        snprintf(file_path, PATH_MAX, "%s/usr/data/%s/manufacturer.xml", rootactPath, GetAdaptorName());
    }
    
	if (access(file_path, R_OK) != 0) {
		RTL_TRDBG(0, "DPT definition file does not exist: %s\n", file_path);
		manxo = NULL;
		return -1;
	}

	manxo = XoReadXmlEx(file_path, NULL, XOML_PARSE_OBIX, &parse);
	if (manxo == NULL) {
		RTL_TRDBG(0, "ERROR ret=%d cannot read xml '%s'\n", parse, file_path);
		return -2;
	}

#if _DEBUG_SHOW_DB
	char *buffer;
	void *w = XoWritXmlMem(manxo, XO_FMT_STD_XML, &buffer, "");
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
 * @brief Get informations about a specific Manufacturer
 * @param id The manufacturer code to search
 * @param infos Structure to fill with manufacturer informations
 * @return  0 on success
 *         -1 if database is not load
 *         -2 if database is empty
 *         -3 if the manufacturer code is not found
 *         -4 if the manufacturer don't have name
 */
int
ManufacturerGetInfos(int id, ManufacturerInfos_t *infos)
{
    int rc;
    char *name;
    
    if (manxo == NULL)
        return -1;
    
    memset(infos, 0, sizeof(infos));
    infos->id = id;

    rc = getObixStrValue(manxo, name, "[val=%d].name", id);
    if (rc != 0) {
        return -4;
    }

    infos->name = name;
    return 0;
}
