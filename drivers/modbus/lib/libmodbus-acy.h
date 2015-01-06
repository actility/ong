/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/access.c */
char *obixTypeFromModbus(char *type, int *id);
void decodeModbusAccess(Attr_t *attr, char *reg);
void sendAnswerFromXo(void *xo, char *code);
void modbusAccessWrite(Sensor_t *device, Attr_t *attr, void *xo);
int modbusAccessReadBool(modbus_t *ctx, Attr_t *attr, char **descriptor, void **xo, Value_t *lastVal);
char *uint16ToStr(uint16_t *array, uint16_t size);
int modbusAccessReadArray(modbus_t *ctx, Attr_t *attr, char **descriptor, void **xo, Value_t *lastVal);
void modbusAccessRead(Sensor_t *device, Attr_t *attr);
char *modbusValueToString(uint8_t *valArray, uint16_t valLen, int modbusType, char *strVal);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/appitf.c */
void ApplicationITF(void *appInterfaces, void *xoProduct, void *xoModeling, Sensor_t *device);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/attr_t.c */
Attr_t *new_Attr_t(void);
void Attr_t_newFree(Attr_t *This);
void *Attr_t_buildXoFromProductDescriptor(Attr_t *This, uint16_t *array, uint16_t size);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/diaproducts.c */
void WDiaUCBRequestOk_DiaProductsDatOk(t_dia_req *preq, t_cmn_sensor *cmn);
int WDiaUCBPreLoadTemplate_DiaProductsDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo, char *templ);
int WDiaUCBPostLoadTemplate_DiaProductsDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo);
void DiaProductsDatStart(void);
int DiaProductsFetch(int dev);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/iso8601-ext.c */
int parseISO8601Duration_ms(char *string, int *ms);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/point_t.c */
Point_t *new_Point_t(void);
void Point_t_newFree(Point_t *This);
void Point_t_setModbusType(Point_t *This, int modbusType);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/products.c */
void **ProductsGetDescription(char *ref);
void ProductsSyncAll(void);
void ProductsSyncNull(void);
void ProductsSync(char *ref);
void ProductsList(void *cl);
char *ProductsGetRefFromId(int id);
void ProductsSetDescription(int id, void *xo);
void ProductsClearCache(void);
void ProductsLoadCache(void);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/value_t.c */
Value_t *new_Value_t(void);
void Value_t_newFree(Value_t *This);
void Value_t_setArray(Value_t *This, uint16_t *val, uint16_t size);
/* /home/mlouiset/Work/ong/git/ong/drivers/modbus/.apu_build/centos6-x86_64/package_runtime/modbus/lib/var-helpers.c */
void SetVarIntoTemplate(char *key, char *value);
void RenderVarIntoTemplate(char *key, char *fmt, ...);
void SetVarAsISO8601Duration(char *key, int32_t s, int32_t ms);
