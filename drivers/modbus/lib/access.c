#include "modbus-acy.h"


char *
obixTypeFromModbus(char *type, int *id)
{
  struct _map{
    char *modbus, *obix;
    int id;
  } map[] = {
    {"bool",      "o:bool", ACCESS_BOOL},
    {"uint8",     "o:int",  ACCESS_UINT8},
    {"int8",      "o:int",  ACCESS_INT8},
    {"uint16",    "o:int",     ACCESS_UINT16},
    {"int16",     "o:int",     ACCESS_INT16},
    {"uint32",    "o:int",     ACCESS_UINT32},
    {"int32",     "o:int",     ACCESS_INT32},
    {"float",     "o:real", ACCESS_FLOAT},
    {"double",    "o:real", ACCESS_DOUBLE},
    {"datetime",  "o:abstime", ACCESS_DATE_TIME},
    {"array",     "o:obj",     ACCESS_ARRAY}
  };
  
  int i = 0;
  int nb = sizeof(map) / sizeof(struct _map);
  for(i=0; i<nb; i++) {
    if (strcmp(type, map[i].modbus) == 0) {
      if (id != NULL) {
        *id = map[i].id;
      }
      return map[i].obix;
    }
  }
  
  return NULL;
}

// reg=/<map>/<sz> [/mask/<mask>]
// reg=/<map>/<sz> [/bytes/<mask>]
// reg=/holding/1/mask/8
// reg=/holding/1/bytes/5
// reg=/holding/1
void
decodeModbusAccess(Attr_t *attr, char *reg)
{
  int map;
  int addr;
  int sz;
  char *pt;
  
  attr->map = MAP_NOT_MAPPED;
  
  if (*reg != '/')
    return;
  reg++;

  // Read Map
  pt = index(reg, '/');
  if (pt == NULL)
    return;
  sz = pt - reg;
  if (strncmp(reg, "holding", sz) == 0) {
    map = MAP_HOLDING;
  } else {
    return;
  }
  
  // Read Sz
  if (*(pt + 1) == '\0')
    return;
  reg = pt + 1;
  pt = index(reg, '/');
  if (pt == NULL)
    return;
  *pt = '\0';
  addr = atoi(reg);
  *pt = '/';
  
  // Save
  attr->map = map;
  attr->reg = addr;
  RTL_TRDBG(TRACE_IMPL, "map=%u addr=%u\n", map, addr);
  
  // Check mask
  if (*(pt + 1) == '\0')
    return;
  reg = pt + 1;
  pt = index(reg, '/');
  if (pt == NULL)
    return;
  sz = pt - reg;
  if (strncmp(reg, "mask", sz) == 0) {
    if (*(pt + 1) != '\0') {
      reg = pt + 1;
      pt = index(reg, '/');
      if (pt != NULL) {
        *pt = '\0';
        attr->mask = atoi(reg);
        *pt = '/';
        
        RTL_TRDBG(TRACE_IMPL, "mask=0x%04hX\n", attr->mask);
      }
    }
  } else if (strncmp(reg, "bytes", sz) == 0) {
    if (*(pt + 1) != '\0') {
      reg = pt + 1;
      pt = index(reg, '/');
      if (pt != NULL) {
        *pt = '\0';
        attr->bytes = atoi(reg);
        *pt = '/';
        
        RTL_TRDBG(TRACE_IMPL, "bytes=%u\n", attr->bytes);
      }
    }
  } 
}

void
sendAnswerFromXo(void *xo, char *code)
{
  void *w = NULL;
  char *buffer = NULL;
  char *ctype  = "application/xml";
  int sz;
  
  w = XoWritXmlMem(xo, 0, &buffer, "");
  if (w != NULL) {
    RTL_TRDBG(TRACE_ERROR, "ERROR cannot serialize (XML) xo '%s'\n", XoNmType(xo));
    XoFree(xo, 1);
    diaIncomingRequestCompletion(-1, NULL, 0, "application/xml", WDiaStatusCode(DIA_RETRIEVE, 500));
    return;
  }
  sz = strlen(buffer);
  XoWritXmlFreeCtxt(w);

  diaIncomingRequestCompletion(-1, buffer, sz, ctype, code);
  XoFree(xo, 1);
}


#define MODBUS_CHECK_WRITE_ERROR() \
  if (rc == -1) {                                         \
    RTL_TRDBG(TRACE_ERROR, "Modbus error '%s'(%d)\n", modbus_strerror(errno), errno);      \
    diaIncomingRequestCompletion(-1, NULL, 0, "application/xml", WDiaStatusCode(-1, 503)); \
    return 2;                                           \
  }
  
int
modbusAccessWriteBool(modbus_t *ctx, Attr_t *attr, bool val)
{
  int rc;
  uint16_t mask = attr->mask;
  int reg = attr->reg;
  int map = attr->map;
  uint16_t value16;
  
  switch (map) {
    case MAP_COILS:
      rc = modbus_write_bit(ctx, reg, val);
      MODBUS_CHECK_WRITE_ERROR();
      break;

    case MAP_HOLDING:
      rc = modbus_read_registers(ctx, reg, 1, &value16);
      MODBUS_CHECK_WRITE_ERROR();
      
      if (val) {
        value16 |= mask;
      } else {
        value16 &= ~mask;
      }
      
      rc = modbus_write_register(ctx, reg, value16);
      MODBUS_CHECK_WRITE_ERROR();
      break;
    
    default:
      diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", WDiaStatusCode(DIA_UPDATE, 501));
      return 1;
  } 

  diaIncomingRequestCompletion(-1, "", 0, "application/xml", WDiaStatusCode(DIA_UPDATE, 204));
  return 0;
}


void
modbusAccessWrite(Sensor_t *device, Attr_t *attr, void *xo)
{
  int rc;
  modbus_t *ctx = NULL;
  
  RTL_TRDBG(TRACE_INFO, "modbus write access on '%s', ctx=%p or %p\n",
    device->name, device->modbusCtx, device->network->modbusCtx);
  
  if (device->modbusCtx != NULL) {
    ctx = device->modbusCtx;
  } else {
    ctx = device->network->modbusCtx;
  }
  
  if (ctx == NULL) {
    diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", WDiaStatusCode(DIA_RETRIEVE, 503));
    return;
  }

  if (attr->modbusTypeID == ACCESS_BOOL) {
    bool val;
    rc = getObixBoolValue(xo, &val, "val");
    if (rc < 0) {
      diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", WDiaStatusCode(DIA_RETRIEVE, 400));
      return;
    }
    
    if (2 == modbusAccessWriteBool(ctx, attr, val)) {
      // Modbus connection issue detected
      DeviceModbusDisconnection(device);
    }
    return;
  }
  
  // modbusTypeID access not supported
  diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", WDiaStatusCode(DIA_RETRIEVE, 501));

}


#define MODBUS_CHECK_READ_ERROR() \
  if (rc == -1) {                                         \
    RTL_TRDBG(TRACE_ERROR, "Modbus error '%s'(%d)\n", modbus_strerror(errno), errno);           \
    if (xo == NULL) {                                       \
      diaIncomingRequestCompletion(-1, NULL, 0, "application/xml", WDiaStatusCode(-1, 503)); \
    } else {                                          \
      *xo = NULL;                                       \
    }                                               \
    return 2;                                           \
  }


/**
 *
 * @return 2 if READ_ERROR happens, 1 for other Xo, applicative, ... issues or 0 if
 * successful.
 */
int
modbusAccessReadBool(modbus_t *ctx, Attr_t *attr, char **descriptor, void **xo, 
  Value_t *lastVal)
{
  int rc;
  uint16_t mask = attr->mask;
  int reg = attr->reg;
  int map = attr->map;
  uint8_t value8;
  uint16_t value, value16;
  
  switch (map) {
    case MAP_DISCRETES:
      rc = modbus_read_input_bits(ctx, reg, 1, &value8);
      MODBUS_CHECK_READ_ERROR();
      break;
      
    case MAP_COILS:
      rc = modbus_read_bits(ctx, reg, 1, &value8);
      MODBUS_CHECK_READ_ERROR();
      break;
      
    case MAP_INPUT:
      rc = modbus_read_input_registers(ctx, reg, 1, &value16);
      MODBUS_CHECK_READ_ERROR();
      break;
      
    case MAP_HOLDING:
      rc = modbus_read_registers(ctx, reg, 1, &value16);
      MODBUS_CHECK_READ_ERROR();
      break;
    
    default:
      if (xo == NULL) {
        diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", 
          WDiaStatusCode(DIA_RETRIEVE, 501));
      } else {
        *xo = NULL;
      }
      return 1;
  } 
  
  if (map == MAP_INPUT || map == MAP_HOLDING) {
    value = value16 & mask;
  } else {
    value = value8;
  }
  
  if (descriptor != NULL) {
    // Descriptor
    if (value) {
      *descriptor = strdup("true");
    } else {
      *descriptor = strdup("false");
    }
  } else {

    if (value) {
      SetVar("r_value", "true");
    } else {
      SetVar("r_value", "false");
    }

    int parseflags = XOML_PARSE_OBIX;
    void *xoxo = WXoNewTemplate("dat_bool.xml", parseflags);
    if (xoxo == NULL) {
      RTL_TRDBG(TRACE_ERROR, "Can't parse template 'dat_bool.xml'\n");
      if (xo == NULL) {
        diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", 
          WDiaStatusCode(DIA_RETRIEVE, 500));
      }
      return 1;
    }
    
    if (xo == NULL) {
      sendAnswerFromXo(xoxo, WDiaStatusCode(DIA_RETRIEVE, 204));
    } else {
      if ((lastVal) && (! lastVal->boolValueHasChanged(lastVal, value)))
      {
        // value has not changed since last read
        return 1;
      }
      *xo = xoxo;
      if (lastVal)
      {
        lastVal->setBool(lastVal, value);
      }
      return 0; 
    }
  }
  return 1;
}

char *uint16ToStr(uint16_t *array, uint16_t size) {
  int i;
  char szTemp[16];
  static char szRes[1024];
  
  memset(szRes, 0, sizeof(szRes));
  for (i = 0; (i < size) && ((i * 7) + 1 < sizeof(szRes)); i++) {
    sprintf(szTemp, "0x%.4x ", array[i]);
    strcat(szRes, szTemp);
  }

  return szRes;
}

/**
 *
 * @return 2 if READ_ERROR happens, 1 for other Xo, applicative, ... issues or 0 if
 * successful.
 */
int
modbusAccessReadArray(modbus_t *ctx, Attr_t *attr, char **descriptor, void **xo, 
  Value_t *lastVal)
{
  int rc;
  int reg = attr->reg;
  int map = attr->map;
  uint16_t array[MAX_MODBUS_ACCESS_SIZE];
  int nbRead = attr->bytes/2 + attr->bytes%2;

  if (nbRead > MAX_MODBUS_ACCESS_SIZE) {
    RTL_TRDBG(TRACE_ERROR, "The register size the driver is trying to access "
      "is too wild; aborted (nbRead:%d) (max:%d)\n", nbRead, MAX_MODBUS_ACCESS_SIZE);
    return 1;
  }
 
  switch (map) {
    case MAP_INPUT:
      rc = modbus_read_input_registers(ctx, reg, nbRead, array);
      MODBUS_CHECK_READ_ERROR();
      if (TraceLevel >= TRACE_DETAIL) {
        RTL_TRDBG(TRACE_DETAIL, "modbusAccessReadArray: read input register (ctx:%p) "
          "(reg:%d) (nbRead:%d) (read:%s)\n", ctx, reg, nbRead, uint16ToStr(array, nbRead));
      }
      break;
      
    case MAP_HOLDING:
      rc = modbus_read_registers(ctx, reg, nbRead, array);
      MODBUS_CHECK_READ_ERROR();
      if (TraceLevel >= TRACE_DETAIL) {
        RTL_TRDBG(TRACE_DETAIL, "modbusAccessReadArray: read holding register (ctx:%p) "
          "(reg:%d) (nbRead:%d) (read:%s)\n", ctx, reg, nbRead, uint16ToStr(array, nbRead));
      }
      break;
   
     
    default:
      if (xo == NULL) {
        diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", 
          WDiaStatusCode(DIA_RETRIEVE, 501));
      } else {
        *xo = NULL;
      }
      return 1;
  } 
  
  if (descriptor != NULL) {
    // Descriptor
    *descriptor = strdup("unsupported");
  } else {
    // build xo for logging
    // TODO: dissociate all processing (logging, retargeting, descriptor) in upper layers

    if (xo == NULL) {
      // re-targeting (?)
      void *xoxo = attr->buildXoFromProductDescriptor(attr, array, nbRead);

      if (xoxo == NULL) {
        RTL_TRDBG(TRACE_ERROR, "Can't build xo from product description file\n");
        if (xo == NULL) {
          diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", 
            WDiaStatusCode(DIA_RETRIEVE, 500));
        }
        return 1;
      }
    
      sendAnswerFromXo(xoxo, WDiaStatusCode(DIA_RETRIEVE, 204));
    } else {
      // logging
      if ((lastVal) && (! lastVal->arrayValueHasChanged(lastVal, array, nbRead)))
      {
        // value has not changed since last read
        return 1;
      }
      void *xoxo = attr->buildXoFromProductDescriptor(attr, array, nbRead);

      if (xoxo == NULL) {
        RTL_TRDBG(TRACE_ERROR, "Can't build xo from product description file\n");
        return 1;
      }
    
      *xo = xoxo;
      if (lastVal)
      {
        lastVal->setArray(lastVal, array, nbRead);
      }
      return 0; 
    }
  }
  return 1;
}


bool
_modbusAccessRead(Sensor_t *device, Attr_t *attr, char **descriptor, 
  void **xo, Value_t *lastVal)
{
  modbus_t *ctx = NULL;
  int rc = -1;
  
  RTL_TRDBG(TRACE_INFO, "modbus read access on '%s', ctx=%p or %p\n",
    device->name, device->modbusCtx, device->network->modbusCtx);
  
  if (device->modbusCtx != NULL) {
    ctx = device->modbusCtx;
  } else {
    ctx = device->network->modbusCtx;
  }
  
  if (ctx == NULL) {
    if (xo == NULL) {
      diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", 
        WDiaStatusCode(DIA_RETRIEVE, 503));
    } else {
      *xo = NULL;
    }
    return false;
  }

  switch (attr->modbusTypeID)
  {
    case ACCESS_BOOL:
      rc = modbusAccessReadBool(ctx, attr, descriptor, xo, lastVal);

    case ACCESS_ARRAY:
      rc = modbusAccessReadArray(ctx, attr, descriptor, xo, lastVal);
  }
  switch (rc) {
    case 0:
      return true;
    case 1:
      return false;
    case 2:
      // Modbus connection issue detected
      DeviceModbusDisconnection(device);
      return false;

    default:
      break; 
  }
  
  // modbusTypeID access not supported
  if (xo == NULL) {
    diaIncomingRequestCompletion(-1, NULL, 0,"application/xml", 
      WDiaStatusCode(DIA_RETRIEVE, 501));
  } else {
    *xo = NULL;
  }
  return false;
}


void
modbusAccessRead(Sensor_t *device, Attr_t *attr)
{
  _modbusAccessRead(device, attr, NULL, NULL, NULL);
}

/**
 * Build a string representation of the raw value provided in valArray.
 * @param valArray the raw data to represent.
 * @param valLen the length of the raw data.
 * @param modbusType the modbus data type of the raw data.
 * @param strVal the buffer in which format the data representation.
 * @return the string representation.
 */
char *
modbusValueToString(uint8_t *valArray, uint16_t valLen, int modbusType, char *strVal)
{
  short i;
  char tmp[6];

  if (!valArray) return NULL;
  if (!valLen) return NULL;
  if (!strVal) return NULL;

  switch (modbusType)
  {
    case ACCESS_BOOL:
      if (1 == valLen) {
        uint8_t val = valArray[0];
        if (val) { 
          sprintf(strVal, "true");
        } else {
          sprintf(strVal, "false");
        }
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_UINT8:
      if (1 == valLen) {
        uint8_t val = valArray[0]; 
        sprintf(strVal, "%u", val);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_INT8:
      if (1 == valLen) {
        int8_t val = valArray[0]; 
        sprintf(strVal, "%d", val);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_UINT16:
      if (2 == valLen) {
        union {
          uint8_t r[2];
          uint16_t v;
        } u;
        memcpy(u.r, valArray, 2);
        sprintf(strVal, "%u", u.v);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_INT16:
      if (2 == valLen) {
        union {
          uint8_t r[2];
          int16_t v;
        } u;
        memcpy(u.r, valArray, 2);
        sprintf(strVal, "%d", u.v);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_UINT32:
      if (4 == valLen) {
        union {
          uint8_t r[4];
          uint32_t v;
        } u;
        memcpy(u.r, valArray, 4);
        sprintf(strVal, "%u", u.v);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_INT32:
      if (4 == valLen) {
        union {
          uint8_t r[4];
          int32_t v;
        } u;
        memcpy(u.r, valArray, 4);
        sprintf(strVal, "%d", u.v);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_FLOAT:
      if (4 == valLen) {
        union {
          uint8_t r[4];
          float v;
        } u;
        memcpy(u.r, valArray, 4);
        sprintf(strVal, "%g", u.v);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_DOUBLE:
      if (8 == valLen) {
        union {
          uint8_t r[8];
          double v;
        } u;
        memcpy(u.r, valArray, 8);
        sprintf(strVal, "%g", u.v);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_DATE_TIME:
      if (4 == valLen) {
        union {
          uint8_t r[4];
          uint32_t v;
        } u;
        memcpy(u.r, valArray, 4);
        struct timeval tv;
        tv.tv_sec = u.v;
        tv.tv_usec = 0;
        rtl_gettimeofday_to_iso8601date(&tv, NULL, strVal);
      } else {
        sprintf(strVal, VAL_IN_ERROR);
      }
      break;

    case ACCESS_ARRAY:
    default:
      // dump hex buffer
      sprintf(strVal, "0x");
      for (i = 0; i < valLen; i++) {
        sprintf(tmp, "%.2x", valArray[i]);
        strcat(strVal, tmp);
      }
      strcat(strVal, "\0");
      break;
  }

  return strVal;
}

