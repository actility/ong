#include "modbus.h"

char *
obixTypeFromModbus(char *type, int *id)
{
  struct _map {
    char *modbus, *obix;
    int id;
  } map[] = {
    {"bool",      "o:bool", ACCESS_BOOL},
    {"uint8",     "o:int",  ACCESS_UINT8},
    {"int8",      "o:int",  ACCESS_INT8},
    {"uint16-le", "o:int",  ACCESS_UINT16_LE},
    {"int16-le",  "o:int",  ACCESS_INT16_LE},
    {"uint16-be", "o:int",  ACCESS_UINT16_BE},
    {"int16-be",  "o:int",  ACCESS_INT16_BE},
    {"uint",      "o:int",  ACCESS_UINT},
    {"int",       "o:int",  ACCESS_INT},
    {"uint32-le", "o:int",  ACCESS_UINT32_LE},
    {"int32-le",  "o:int",  ACCESS_INT32_LE},
    {"uint32-be", "o:int",  ACCESS_UINT32_BE},
    {"int32-be",  "o:int",  ACCESS_INT32_BE},
    {"float",     "o:real", ACCESS_FLOAT},
    {"float-le",  "o:real", ACCESS_FLOAT_LE},
    {"float-be",  "o:real", ACCESS_FLOAT_BE},
    {"double",    "o:real", ACCESS_DOUBLE},
    {"double-le", "o:real", ACCESS_DOUBLE_LE},
    {"double-be", "o:real", ACCESS_DOUBLE_BE}
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
// reg=/holding/1/mask/8
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
  RTL_TRDBG(1, "map=%u addr=%u\n", map, addr);
  
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
        
        RTL_TRDBG(1, "mask=0x%04hX\n", attr->mask);
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
    RTL_TRDBG(0, "ERROR cannot serialize (XML) xo '%s'\n", XoNmType(xo));
    XoFree(xo, 1);
    diaIncomingRequestCompletion(-1, NULL, 0, "application/text", WDiaStatusCode(DIA_RETRIEVE, 500));
    return;
  }
  sz = strlen(buffer);
  XoWritXmlFreeCtxt(w);

  diaIncomingRequestCompletion(-1, buffer, sz, ctype, code);
  XoFree(xo, 1);
}

void
modbusAccessWrite(Sensor_t *device, Attr_t *attr, void *xo)
{
  int rc;
  modbus_t *ctx = NULL;
  
  RTL_TRDBG(1, "modbus write access on '%s', ctx=%p or %p\n",
    device->name, device->modbusCtx, device->network->modbusCtx);
  
  if (device->modbusCtx != NULL) {
    ctx = device->modbusCtx;
  } else {
    ctx = device->network->modbusCtx;
  }
  
  if (ctx == NULL) {
    diaIncomingRequestCompletion(-1, NULL, 0,"application/text", WDiaStatusCode(DIA_RETRIEVE, 503));
    return;
  }

  if (attr->modbusTypeID == ACCESS_BOOL) {
    bool val;
    rc = getObixBoolValue(xo, &val, "val");
    if (rc < 0) {
      diaIncomingRequestCompletion(-1, NULL, 0,"application/text", WDiaStatusCode(DIA_RETRIEVE, 400));
      return;
    }
    
    modbusAccessWriteBool(ctx, attr, val);
    return;
  }
  
  // modbusTypeID access not supported
  diaIncomingRequestCompletion(-1, NULL, 0,"application/text", WDiaStatusCode(DIA_RETRIEVE, 501));

}

#define MODBUS_CHECK_WRITE_ERROR() \
  if (rc == -1) {                                         \
    RTL_TRDBG(1, "Modbus error '%s'(%d)\n", modbus_strerror(errno), errno);           \
    diaIncomingRequestCompletion(-1, NULL, 0, "application/text", WDiaStatusCode(-1, 503));   \
    return;                                           \
  }
  
void
modbusAccessWriteBool(modbus_t *ctx, Attr_t *attr, bool val)
{
  int rc;
  uint16_t mask = attr->mask;
  int reg = attr->reg;
  int map = attr->map;
  uint8_t value8;
  uint16_t value, value16;
  
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
      diaIncomingRequestCompletion(-1, NULL, 0,"application/text", WDiaStatusCode(DIA_UPDATE, 501));
      return;
  } 

  diaIncomingRequestCompletion(-1, "", 0, "application/xml", WDiaStatusCode(DIA_UPDATE, 204));
}


#define MODBUS_CHECK_READ_ERROR() \
  if (rc == -1) {                                         \
    RTL_TRDBG(1, "Modbus error '%s'(%d)\n", modbus_strerror(errno), errno);           \
    if (xo == NULL) {                                       \
      diaIncomingRequestCompletion(-1, NULL, 0, "application/text", WDiaStatusCode(-1, 503)); \
    } else {                                          \
      *xo = NULL;                                       \
    }                                               \
    return false;                                           \
  }

bool
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
        diaIncomingRequestCompletion(-1, NULL, 0,"application/text", 
          WDiaStatusCode(DIA_RETRIEVE, 501));
      } else {
        *xo = NULL;
      }
      return false;
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
      RTL_TRDBG(1, "Can't parse template 'dat_bool.xml'\n");
      if (xo == NULL) {
        diaIncomingRequestCompletion(-1, NULL, 0,"application/text", 
          WDiaStatusCode(DIA_RETRIEVE, 500));
      }
      return false;
    }
    
    if (xo == NULL) {
      sendAnswerFromXo(xoxo, WDiaStatusCode(DIA_RETRIEVE, 204));
    } else {
      bool newVal = ( value ); 
      if (lastVal && (lastVal->u.boolValue == newVal))
      {
        // value has not changed since last read
        return false;
      }
      *xo = xoxo;
      if (lastVal)
      {
        lastVal->u.boolValue = newVal;
      }
      return true; 
    }
  }
  return false;
}


bool
_modbusAccessRead(Sensor_t *device, Attr_t *attr, char **descriptor, void **xo, 
  Value_t *lastVal)
{
  modbus_t *ctx = NULL;
  
  RTL_TRDBG(1, "modbus read access on '%s', ctx=%p or %p\n",
    device->name, device->modbusCtx, device->network->modbusCtx);
  
  if (device->modbusCtx != NULL) {
    ctx = device->modbusCtx;
  } else {
    ctx = device->network->modbusCtx;
  }
  
  if (ctx == NULL) {
    if (xo == NULL) {
      diaIncomingRequestCompletion(-1, NULL, 0,"application/text", 
        WDiaStatusCode(DIA_RETRIEVE, 503));
    } else {
      *xo = NULL;
    }
    return false;
  }

  if (attr->modbusTypeID == ACCESS_BOOL) {
    return modbusAccessReadBool(ctx, attr, descriptor, xo, lastVal);
  }
  
  // modbusTypeID access not supported
  if (xo == NULL) {
    diaIncomingRequestCompletion(-1, NULL, 0,"application/text", 
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


