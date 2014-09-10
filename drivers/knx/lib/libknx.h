/* DB-dpt.c */
int DptLoadDatabase(void);
int DptGetInfos(Dpt_t dpt, DptInfos_t *infos);
Dpt_t DptAsciiToBin(char *in);
char *DptGetInterfaceTemplateFromCode(Dpt_t dpt);
/* DB-manufacturer.c */
int ManufacturerLoadDatabase(void);
int ManufacturerGetInfos(int id, ManufacturerInfos_t *infos);
/* DB-range.c */
int RangeLoadDatabase(void);
int RangeGetRenderedValue(Dpt_t dpt, int value, char **output);
int RangeGetBinValue(Dpt_t dpt, int *value, char *input);
/* var-helpers.c */
void SetVarIntoTemplate(char *key, char *value);
void RenderVarIntoTemplate(char *key, char *fmt, ...);
void SetVarAsISO8601Duration(char *key, int32_t s, int32_t ms);
/* fp16.c */
fp16_t float2half(float f32);
float half2float(fp16_t value);
/* iso8601-ext.c */
int parseISO8601Duration_ms(char *string, int *ms);
/* iconv.c */
char *do_iconv(char *input, char *localeOut, char *localeIn);
/* knx2obix-bits.c */
uint32_t knx_parse_uint(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize);
int32_t knx_parse_int(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize);
float knx_parse_float(uint8_t *buffer, uint32_t bitPosition);
float knx_parse_float16(uint8_t *buffer, uint32_t bitPosition);
/* knx2obix.c */
void knx2obix(Dpt_t dpt, char *buffer, void **xo);
/* obix2knx-bits.c */
void knx_set_uint(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize, uint32_t value);
void knx_set_int(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize, int32_t value);
void knx_set_float(uint8_t *buffer, uint32_t bitPosition, float value);
void knx_set_float16(uint8_t *buffer, uint32_t bitPosition, float value);
/* obix2knx.c */
void _obix2knx(Dpt_t dpt, void *xo, char *buffer, int *len);
void obix2knx(Dpt_t dpt, void *xo, char *buffer, int *len);
/* eib-client.c */
void EibdSetNetworkNotReachable(NetworkList_t *network);
void EibdConnectToDaemon(NetworkList_t *network);
void EibdStartDaemon(NetworkList_t *network);
int EibdCleanup(NetworkList_t *network);
int EibdExit(NetworkList_t *network);
void EibdCleanupAll(void);
void EibdInit(char *uri, NetworkList_t *network);
int EibdSendGroupValueAPDU(NetworkList_t *network, short dest, char *data, int bitdatalen);
/* version.c */
/* list_sort.c */
void list_sort(void *priv, struct list_head *head, int (*cmp)(void *priv, struct list_head *a, struct list_head *b));
/* diatopology.c */
void WDiaUCBRequestOk_DiaTopologyDatOk(t_dia_req *preq, t_cmn_sensor *cmn);
int WDiaUCBPreLoadTemplate_DiaTopologyDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ);
int WDiaUCBPostLoadTemplate_DiaTopologyDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo);
void DiaTopologyDatStart(void);
int DiaTopologyFetch(NetworkList_t *network, void *cl);
/* covconfig.c */
int knxCovSortFunction(void *priv, struct list_head *a, struct list_head *b);
void AddCov(char *filter, char *minInterval, char *maxInterval);
void CovInit(void *mdl);
void CovToCzDump(void *cl);
void covGetTiming(Dpt_t dpt, int *min, int *max);
void appendReport(t_sensor *cp, void *xo);
