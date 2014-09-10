/*
 * Copyright (C) Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 */

//************************************************************************
//************************************************************************
//**
//** File : ./lib/azap_zcl_attr_val.c
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include <rtlbase.h>

#include "azap_types.h"
#include "azap_api_zcl.h"
#include "azap_api.h"
#include "azap_tools.h"

#include "azap_zcl_attr_val.h"

/*****************************************************************************/
/* Prototypes for local functions */
static void azapZclAttrVal_t_init(azapZclAttrVal_t *This);
static void azapZclAttrVal_t_setType(azapZclAttrVal_t *This, azap_uint8 type);

static void azapZclAttrVal_t_serializeBoolean(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_serializeFloat(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_serializeInteger(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_serializeString(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_serializeDateTime(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_serializeRaw(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);

static void azapZclAttrVal_t_deserializeBoolean(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_deserializeFloat(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_deserializeInteger(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_deserializeString(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_deserializeDateTime(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);
static void azapZclAttrVal_t_deserializeRaw(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size);

extern time_t timegm (struct tm *__tp);

/*****************************************************************************/
/* Constructors / destructor */

/* static allocation */

/**
 * @brief Initialize the azapZclAttrVal_t struct
 * @param This the class instance
 */
static void azapZclAttrVal_t_init(azapZclAttrVal_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapZclAttrVal_t::init\n");

  /* Assign the pointers on functions */
  This->getBooleanValue = azapZclAttrVal_t_getBooleanValue;
  This->setBooleanValue = azapZclAttrVal_t_setBooleanValue;
  This->getFloatValue = azapZclAttrVal_t_getFloatValue;
  This->setFloatValue = azapZclAttrVal_t_setFloatValue;
  This->getIntegerValue = azapZclAttrVal_t_getIntegerValue;
  This->setIntegerValue = azapZclAttrVal_t_setIntegerValue;
  This->getStringValue = azapZclAttrVal_t_getStringValue;
  This->setStringValue = azapZclAttrVal_t_setStringValue;
  This->getDateTimeValue = azapZclAttrVal_t_getDateTimeValue;
  This->getDateTimeMSValue = azapZclAttrVal_t_getDateTimeMSValue;
  This->setDateTimeValue = azapZclAttrVal_t_setDateTimeValue;
  This->getRawValue = azapZclAttrVal_t_getRawValue;
  This->setRawValue = azapZclAttrVal_t_setRawValue;
  This->getValueSize = azapZclAttrVal_t_getValueSize;
  This->getStringRep = azapZclAttrVal_t_getStringRep;
  This->buildFromStringRep = azapZclAttrVal_t_buildFromStringRep;
  This->serialize = azapZclAttrVal_t_serialize;
  This->deserialize = azapZclAttrVal_t_deserialize;

  /* Initialize the members */
  This->valueType = AZAP_UNKNOWN_VAL_TYPE;
  This->zigbeeDataType = 0;
  This->valueSize = 0;
}

/**
 * @brief Set the AZAP_ATTR_VAL_TYPE depending on the provided ZigBee data type.
 * This function is private and cannot be invoked from outside; this is to prevent
 * users from changing attribute type on-the-fly, which is often quite complex to
 * manage, and pretty useless. One better have to create a new azapZclAttrVal_t instead.
 * @param This the class instance
 * @param type the attribute value data type
 */
static void azapZclAttrVal_t_setType(azapZclAttrVal_t *This, azap_uint8 type)
{
  RTL_TRDBG(TRACE_DETAIL, "azapZclAttrVal_t::setType\n");

  This->zigbeeDataType = type;
  switch (type)
  {
    case ZCL_DATATYPE_IEEE_ADDR:
    case ZCL_DATATYPE_DATA8:
    case ZCL_DATATYPE_DATA16:
    case ZCL_DATATYPE_DATA24:
    case ZCL_DATATYPE_DATA32:
    case ZCL_DATATYPE_BITMAP8:
    case ZCL_DATATYPE_BITMAP16:
    case ZCL_DATATYPE_BITMAP24:
    case ZCL_DATATYPE_BITMAP32:
    case ZCL_DATATYPE_ENUM8:
    case ZCL_DATATYPE_ENUM16:
    case ZCL_DATATYPE_CLUSTER_ID:
    case ZCL_DATATYPE_ATTR_ID:
    case ZCL_DATATYPE_BAC_OID:
    case ZCL_DATATYPE_OCTET_STR:
    case ZCL_DATATYPE_LONG_OCTET_STR:
      This->valueType = AZAP_RAW_VAL;
      This->u.rawVal = NULL;
      break;

    case ZCL_DATATYPE_CHAR_STR:
    case ZCL_DATATYPE_LONG_CHAR_STR:
      This->valueType = AZAP_CHAR_STRING_VAL;
      This->u.stringVal = NULL;
      break;

    case ZCL_DATATYPE_BOOLEAN:
      This->valueType = AZAP_BOOLEAN_VAL;
      This->u.booleanVal = false;
      break;

    case ZCL_DATATYPE_INT8:
    case ZCL_DATATYPE_INT16:
    case ZCL_DATATYPE_INT24:
    case ZCL_DATATYPE_INT32:
    case ZCL_DATATYPE_UINT8:
    case ZCL_DATATYPE_UINT16:
    case ZCL_DATATYPE_UINT24:
    case ZCL_DATATYPE_UINT32:
    case ZCL_DATATYPE_UINT40:
    case ZCL_DATATYPE_UINT48:
      This->valueType = AZAP_INTEGER_VAL;
      This->u.longVal = 0;
      break;

    case ZCL_DATATYPE_SEMI_PREC:
    case ZCL_DATATYPE_SINGLE_PREC:
    case ZCL_DATATYPE_DOUBLE_PREC:
      This->valueType = AZAP_FLOAT_VAL;
      This->u.floatVal = 0;
      break;

    case ZCL_DATATYPE_TOD:
    case ZCL_DATATYPE_DATE:
    case ZCL_DATATYPE_UTC:
      This->valueType = AZAP_DATE_TIME_VAL;
      memset(&This->u.dateTimeVal, 0, sizeof(azapDateTime_t));
      break;

    case ZCL_DATATYPE_UNKNOWN:
      This->valueType = AZAP_UNKNOWN_VAL_TYPE;
      break;

    default:
      This->valueType = AZAP_UNKNOWN_VAL_TYPE;
      break;
  }
}

/**
 * @brief do static allocation of a azapZclAttrVal_t.
 * @anchor azapZclAttrVal_t_create
 * @param type the attribute value data type
 * @return the copy of the object
 */
azapZclAttrVal_t azapZclAttrVal_t_create(azap_uint8 type)
{
  azapZclAttrVal_t This;
  azapZclAttrVal_t_init(&This);
  azapZclAttrVal_t_setType(&This, type);
  This.free = azapZclAttrVal_t_free;
  RTL_TRDBG(TRACE_DETAIL, "azapZclAttrVal_t::create (This:0x%.8x)\n", &This);
  return This;
}

/**
 * @brief Destructor for static allocation
 * @anchor azapZclAttrVal_t_free
 */
void azapZclAttrVal_t_free(azapZclAttrVal_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapZclAttrVal_t::free (This:0x%.8x)\n", This);
  if ((AZAP_CHAR_STRING_VAL == This->valueType) && (This->u.stringVal))
  {
    free(This->u.stringVal);
  }
}

/* dynamic allocation */
/**
 * @brief do dynamic allocation of a azapZclAttrVal_t.
 * @param type the attribute value data type
 * @return the copy of the object
 */
azapZclAttrVal_t *new_azapZclAttrVal_t(azap_uint8 type)
{
  azapZclAttrVal_t *This = malloc(sizeof(azapZclAttrVal_t));
  if (!This)
  {
    return NULL;
  }
  azapZclAttrVal_t_init(This);
  azapZclAttrVal_t_setType(This, type);
  This->free = azapZclAttrVal_t_newFree;
  RTL_TRDBG(TRACE_DETAIL, "new azapZclAttrVal_t (This:0x%.8x)\n", This);
  return This;
}

/**
 * @brief Destructor for dynamic allocation
 * @anchor azapZclAttrVal_t_newFree
 */
void azapZclAttrVal_t_newFree(azapZclAttrVal_t *This)
{
  RTL_TRDBG(TRACE_DETAIL, "azapZclAttrVal_t::newFree (This:0x%.8x)\n", This);
  if ((AZAP_CHAR_STRING_VAL == This->valueType) && (This->u.stringVal))
  {
    free(This->u.stringVal);
  }
  free(This);
}

/* implementation methods */
/**
 * @brief Get the boolean attribute value.
 * @anchor azapZclAttrVal_t_getBooleanValue
 * @param This the class instance
 * @return the boolean value if type match, false otherwise.
 */
bool azapZclAttrVal_t_getBooleanValue(azapZclAttrVal_t *This)
{
  if (AZAP_BOOLEAN_VAL != This->valueType)
  {
    return false;
  }
  return This->u.booleanVal;
}

/**
 * @brief Change the boolean attribute value to the one provided.
 * @anchor azapZclAttrVal_t_setBooleanValue
 * @param This the class instance
 * @param val the new value to set.
 */
void azapZclAttrVal_t_setBooleanValue(azapZclAttrVal_t *This, bool val)
{
  if (AZAP_BOOLEAN_VAL != This->valueType)
  {
    return;
  }
  This->u.booleanVal = val;
}

/**
 * @brief Get the floating-point number attribute value.
 * @anchor azapZclAttrVal_t_setFloatValue
 * @param This the class instance
 * @return the float value if type match, 0.0 otherwise.
 */
double azapZclAttrVal_t_getFloatValue(azapZclAttrVal_t *This)
{
  if (AZAP_FLOAT_VAL != This->valueType)
  {
    return 0.0;
  }
  return This->u.floatVal;
}

/**
 * @brief Change the floating-point number attribute value to the one provided.
 * @anchor azapZclAttrVal_t_setFloatValue
 * @param This the class instance
 * @param val the new value to set.
 */
void azapZclAttrVal_t_setFloatValue(azapZclAttrVal_t *This, double val)
{
  if (AZAP_FLOAT_VAL != This->valueType)
  {
    return;
  }
  // TODO: test if val is in a valid range depending on zigbeeDataType
  This->u.floatVal = val;
}

/**
 * @brief Get the integer attribute value.
 * @anchor azapZclAttrVal_t_getIntegerValue
 * @param This the class instance
 * @return the integer value if type match, 0 otherwise.
 */
long long azapZclAttrVal_t_getIntegerValue(azapZclAttrVal_t *This)
{
  if (AZAP_INTEGER_VAL != This->valueType)
  {
    return 0;
  }
  return This->u.longVal;
}

/**
 * @brief Change the integer attribute value to the one provided.
 * @anchor azapZclAttrVal_t_setIntegerValue
 * @param This the class instance
 * @param val the new value to set.
 */
void azapZclAttrVal_t_setIntegerValue(azapZclAttrVal_t *This, long long val)
{
  if (AZAP_INTEGER_VAL != This->valueType)
  {
    return;
  }
  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_INT8:
      if ((val > 0x7FLL) || (val < (signed)0xFFFFFFFFFFFFFF80LL))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_INT16:
      if ((val > 0x7FFFLL) || (val < (signed)0xFFFFFFFFFFFF8000LL))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_INT24:
      if ((val > 0x7FFFFFLL) || (val < (signed)0xFFFFFFFFFF800000LL))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_INT32:
      if ((val > 0x7FFFFFFFLL) || (val < (signed)0xFFFFFFFF80000000LL))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_UINT8:
      if ((val > 0xFFLL) || (val < 0))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_UINT16:
      if ((val > 0xFFFFLL) || (val < 0))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_UINT24:
      if ((val > 0xFFFFFFLL) || (val < 0))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_UINT32:
      if ((val > 0xFFFFFFFFLL) || (val < 0))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_UINT40:
      if ((val > 0xFFFFFFFFFFLL) || (val < 0))
      {
        return;
      }
      break;
    case ZCL_DATATYPE_UINT48:
      if ((val > 0xFFFFFFFFFFFFLL) || (val < 0))
      {
        return;
      }
      break;
  }

  This->u.longVal = val;
}

/**
 * @brief Get the character string attribute value.
 * @anchor azapZclAttrVal_t_getStringValue
 * @param This the class instance
 * @return the character string value if type match, "" otherwise.
 */
char *azapZclAttrVal_t_getStringValue(azapZclAttrVal_t *This)
{
  if (AZAP_CHAR_STRING_VAL != This->valueType)
  {
    return "";
  }
  return This->u.stringVal;
}

/**
 * @brief Change the character string attribute value to the one provided.
 * @anchor azapZclAttrVal_t_setStringValue
 * @param This the class instance
 * @param val the new value to set. WARNING: This should be a null-terminated string.
 */
void azapZclAttrVal_t_setStringValue(azapZclAttrVal_t *This, char *val)
{
  char *newVal = NULL;
  azap_uint8 size = 0;
  if (AZAP_CHAR_STRING_VAL != This->valueType)
  {
    return;
  }
  if (This->u.stringVal)
  {
    free(This->u.stringVal);
  }
  This->valueSize = 0; // size will be updated if string can be allocated.
  if (val)
  {
    size = strlen(val);
    newVal = malloc(size + 1); // this is a null-terminated string
    if (newVal)
    {
      This->valueSize = size;
      memcpy(newVal, val, size + 1); // Also copy the trailing '\0'
    }
  }
  This->u.stringVal = newVal;
}

/**
 * @brief Get the date/time attribute value.
 * @anchor azapZclAttrVal_t_getDateTimeValue
 * @param This the class instance
 * @return the date/time value if type match, NULL otherwise.
 */
struct tm *azapZclAttrVal_t_getDateTimeValue(azapZclAttrVal_t *This)
{
  if (AZAP_DATE_TIME_VAL != This->valueType)
  {
    return NULL;
  }
  return &(This->u.dateTimeVal.dateTime);
}

/**
 * @brief Get the date/time milliseconds attribute value.
 * @anchor azapZclAttrVal_t_getDateTimeMSValue
 * @param This the class instance
 * @return the date/time milliseconds value if type match, 0 otherwise.
 */
int azapZclAttrVal_t_getDateTimeMSValue(azapZclAttrVal_t *This)
{
  if (AZAP_DATE_TIME_VAL != This->valueType)
  {
    return 0;
  }
  return This->u.dateTimeVal.mseconds;
}

/**
 * @brief Change the date/time attribute value to the one provided.
 * @anchor azapZclAttrVal_t_setDateTimeValue
 * @param This the class instance
 * @param val the new value to set.
 * @param msec number of milliseconds (additional information missing in struct tm)
 */
void azapZclAttrVal_t_setDateTimeValue(azapZclAttrVal_t *This, struct tm *val, int msec)
{
  if (AZAP_DATE_TIME_VAL != This->valueType)
  {
    return;
  }

  if (val)
  {
    memcpy(&(This->u.dateTimeVal.dateTime), val, sizeof(struct tm));
  }
  This->u.dateTimeVal.mseconds = msec;
}

/**
 * @brief Get the "raw" attribute value.
 * @anchor azapZclAttrVal_t_getRawValue
 * @param This the class instance
 * @return the "RAW" value if type match, NULL otherwise.
 */
unsigned char *azapZclAttrVal_t_getRawValue(azapZclAttrVal_t *This)
{
  if (AZAP_RAW_VAL != This->valueType)
  {
    return NULL;
  }
  return This->u.rawVal;
}

/**
 * @brief Change the "raw" attribute value to the one provided.
 * @anchor azapZclAttrVal_t_setRawValue
 * @param This the class instance
 * @param val the new value to set.
 * @param size the size of the new value to set.
 */
void azapZclAttrVal_t_setRawValue(azapZclAttrVal_t *This, unsigned char *val,
    azap_uint8 size)
{
  unsigned char *newVal = NULL;
  if (AZAP_RAW_VAL != This->valueType)
  {
    return;
  }
  if (This->u.rawVal)
  {
    free(This->u.rawVal);
  }
  This->valueSize = 0; // size will be updated if string can be allocated.
  if (val)
  {
    newVal = malloc(size);
    if (newVal)
    {
      This->valueSize = size;
      memcpy(newVal, val, size);
    }
  }
  This->u.rawVal = newVal;
}

/**
 * @brief Get the size of the attribute value.
 * @anchor azapZclAttrVal_t_getValueSize
 * @param This the class instance
 * @return the size of the value.
 */
azap_uint8 azapZclAttrVal_t_getValueSize(azapZclAttrVal_t *This)
{
  return This->valueSize;
}


/**
 * @brief Build a string reprensentation of the ZigBee attribute value.
 * WARNING this function is not thread safe, and the returned value is pointing at
 * a static allocated buffer. The String representation cannot exceed 255 characters.
 * @anchor azapZclAttrVal_t_getStringRep
 * @param This the class instance
 * @return the built string representation.
 */
char *azapZclAttrVal_t_getStringRep (azapZclAttrVal_t *This)
{
  static char strRep[AZAP_ATTR_VAL_MAX_SIZE];
  switch (This->valueType)
  {
    case AZAP_UNKNOWN_VAL_TYPE:
      sprintf(strRep, "<UNKNOWN>");
      break;

    case AZAP_BOOLEAN_VAL:
      if (This->u.booleanVal)
      {
        sprintf(strRep, "true");
      }
      else
      {
        sprintf(strRep, "false");
      }
      break;

    case AZAP_FLOAT_VAL:
      sprintf(strRep, "%g", This->u.floatVal);
      break;

    case AZAP_INTEGER_VAL:
    {
      switch (This->zigbeeDataType)
      {
        case ZCL_DATATYPE_INT8:
        case ZCL_DATATYPE_INT16:
        case ZCL_DATATYPE_INT24:
        case ZCL_DATATYPE_INT32:
          sprintf(strRep, "%lld", This->u.longVal);
          break;
        default:
          sprintf(strRep, "%llu", (unsigned long long)This->u.longVal);
          break;
      }
    }
    break;

    case AZAP_CHAR_STRING_VAL:
      snprintf(strRep, AZAP_ATTR_VAL_MAX_SIZE, "%s", This->u.stringVal);
      break;

    case AZAP_DATE_TIME_VAL:
      if (ZCL_DATATYPE_TOD == This->zigbeeDataType)
      {
        char szTemp[12];
        strftime(strRep, AZAP_ATTR_VAL_MAX_SIZE, "%T", &This->u.dateTimeVal.dateTime);
        sprintf(szTemp, ".%03d", This->u.dateTimeVal.mseconds % 1000);
        strcat(strRep, szTemp);
      }
      else if (ZCL_DATATYPE_DATE == This->zigbeeDataType)
      {
        strftime(strRep, AZAP_ATTR_VAL_MAX_SIZE, "%F", &This->u.dateTimeVal.dateTime);
      }
      else // ZCL_DATATYPE_UTC
      {
        strftime(strRep, AZAP_ATTR_VAL_MAX_SIZE, "%FT%T%z", &This->u.dateTimeVal.dateTime);
      }
      break;

    case AZAP_RAW_VAL:
      snprintf(strRep, AZAP_ATTR_VAL_MAX_SIZE, "%s",
          azap_uint8ArrayToString(This->u.rawVal, This->valueSize));
      break;
  }

  return strRep;
}

/**
 * @brief Retrieve the value from a string representation.
 * @anchor azapZclAttrVal_t_buildFromStringRep
 * @param This the class instance
 * @param strRep the string representation to use to retrieve the value.
 * @return true if attribute value can be retrieved from the provided
 * string representation, false otherwise.
 */
bool azapZclAttrVal_t_buildFromStringRep(azapZclAttrVal_t *This, char *strRep)
{
  bool res = false;
  if (!strRep)
  {
    return false;
  }
  switch (This->valueType)
  {
    case AZAP_BOOLEAN_VAL:
    {
      if (!strcmp(strRep, "true"))
      {
        This->u.booleanVal = true;
        res = true;
      }
      else if (!strcmp(strRep, "false"))
      {
        This->u.booleanVal = true;
        res = false;
      }
    }
    break;

    case AZAP_FLOAT_VAL:
    {
      if (sscanf(strRep, "%lg", &(This->u.floatVal)))
      {
        res = true;
      }
    }
    break;

    case AZAP_INTEGER_VAL:
    {
      switch (This->zigbeeDataType)
      {
        case ZCL_DATATYPE_INT8:
        case ZCL_DATATYPE_INT16:
        case ZCL_DATATYPE_INT24:
        case ZCL_DATATYPE_INT32:
          if (sscanf(strRep, "%lld", &(This->u.longVal)))
          {
            res = true;
          }
          break;
        default:
          if (sscanf(strRep, "%llu", &(This->u.longVal)))
          {
            res = true;
          }
          break;
      }
    }
    break;

    case AZAP_CHAR_STRING_VAL:
    {
      This->valueSize = strlen(strRep);
      This->u.stringVal = malloc(This->valueSize + 1);
      sprintf(This->u.stringVal, strRep);
      res = true;
    }
    break;

    case AZAP_DATE_TIME_VAL:
    case AZAP_RAW_VAL:
    default:
      // not supported
      break;
  }

  return res;
}

/**
 * @brief Encode the attribute value, ready to be send in a ZCL payload.
 * It encodes the attribute value and shift the provided pointer on the payload at
 * one position after the end of the encoded value.
 * @anchor azapZclAttrVal_t_serialize
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 */
void azapZclAttrVal_t_serialize(azapZclAttrVal_t *This, azap_byte **data, azap_uint8 *len)
{
  if (!data)
  {
    return;
  }
  if (AZAP_UNKNOWN_VAL_TYPE == This->valueType)
  {
    return;
  }
  if (ZCL_DATATYPE_UNKNOWN == This->zigbeeDataType)
  {
    return;
  }
  azap_uint8 requiredSize = azapGetZclDataLength(This->zigbeeDataType, NULL);
  if (0 == requiredSize)
  {
    if ((ZCL_DATATYPE_CHAR_STR == This->zigbeeDataType) ||
        (ZCL_DATATYPE_OCTET_STR == This->zigbeeDataType))
    {
      requiredSize = This->valueSize + 1;
    }
    else
    {
      // ZCL_DATATYPE_LONG_CHAR_STR or ZCL_DATATYPE_LONG_OCTET_STR
      requiredSize = This->valueSize + 2;
    }
  }
  if (*len < requiredSize)
  {
    RTL_TRDBG(TRACE_ERROR, "azapZclAttrVal_t::serialize - not enough space left (len:%d) "
        "(type:0x%.2x) (requiredSize:%d)\n", *len, This->zigbeeDataType, requiredSize);
    return;
  }

  switch (This->valueType)
  {
    case AZAP_BOOLEAN_VAL:
      azapZclAttrVal_t_serializeBoolean(This, data, len, requiredSize);
      break;

    case AZAP_FLOAT_VAL:
      azapZclAttrVal_t_serializeFloat(This, data, len, requiredSize);
      break;

    case AZAP_INTEGER_VAL:
      azapZclAttrVal_t_serializeInteger(This, data, len, requiredSize);
      break;

    case AZAP_CHAR_STRING_VAL:
      azapZclAttrVal_t_serializeString(This, data, len, requiredSize);
      break;

    case AZAP_DATE_TIME_VAL:
      azapZclAttrVal_t_serializeDateTime(This, data, len, requiredSize);
      break;

    case AZAP_RAW_VAL:
      azapZclAttrVal_t_serializeRaw(This, data, len, requiredSize);
      break;

    default:
      break;
  }
}

/**
 * @brief Decode the attribute value from a ZCL payload.
 * It decodes the attribute value and shift the provided pointer on the payload at
 * one position after the end of the decoded value.
 * @anchor azapZclAttrVal_t_deserialize
 * @param This the class instance
 * @param data the position in the payload where the encoded value starts.
 * @param len the reference on the payload length that remains to be deserialized. Once
 * deserialized the value of len is changed to the remaining byte to be deserialized.
 */
void azapZclAttrVal_t_deserialize(azapZclAttrVal_t *This, azap_byte **data, azap_uint8 *len)
{
  if (!data)
  {
    return;
  }
  if (AZAP_UNKNOWN_VAL_TYPE == This->valueType)
  {
    return;
  }
  azap_uint8 size = azapGetZclDataLength(This->zigbeeDataType, *data);
  if (*len < size)
  {
    RTL_TRDBG(TRACE_ERROR, "azapZclAttrVal_t::deserialize - not enough data (len:%d) "
        "(type:0x%.2x)\n", *len, This->zigbeeDataType);
    return;
  }


  switch (This->valueType)
  {
    case AZAP_BOOLEAN_VAL:
      azapZclAttrVal_t_deserializeBoolean(This, data, len, size);
      break;

    case AZAP_FLOAT_VAL:
      azapZclAttrVal_t_deserializeFloat(This, data, len, size);
      break;

    case AZAP_INTEGER_VAL:
      azapZclAttrVal_t_deserializeInteger(This, data, len, size);
      break;

    case AZAP_CHAR_STRING_VAL:
      azapZclAttrVal_t_deserializeString(This, data, len, size);
      break;

    case AZAP_DATE_TIME_VAL:
      azapZclAttrVal_t_deserializeDateTime(This, data, len, size);
      break;

    case AZAP_RAW_VAL:
      azapZclAttrVal_t_deserializeRaw(This, data, len, size);
      break;

    default:
      break;
  }
}




///////////////////////////////////////////////////////////////////////////////
// WARNING: all the methods that follow assume your host is little endian
// (which is true on our ARM and on x86, our two targets), and so revert the
// byte order compare to zigbee payload encoding.
// We did this as libc version is too old, and there are no standard methods
// to convert 64bit data types in this version...
// (well there are htonl and ntohl for 32bits... but be64toh and htobe64 are
// not supported)



/*   ___          _      _ _         _   _
 *  / __| ___ _ _(_)__ _| (_)_____ _| |_(_)___ _ _
 *  \__ \/ -_) '_| / _` | | |_ / _` |  _| / _ \ ' \
 *  |___/\___|_| |_\__,_|_|_/__\__,_|\__|_\___/_||_|
 */

/**
 * @brief Encode a boolean attribute value.
 * @anchor azapZclAttrVal_t_serializeBoolean
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 * @param requiredSize the number of byte that are required to serialize the value.
 */
static void azapZclAttrVal_t_serializeBoolean(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 requiredSize)
{
  if (This->u.booleanVal)
  {
    (*data)[0] = 0x01;
  }
  else
  {
    (*data)[0] = 0x00;
  }
  (*data) ++;
  (*len) --;
}

/**
 * @brief Encode a floating-point number attribute value.
 * @anchor azapZclAttrVal_t_serializeFloat
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 * @param requiredSize the number of byte that are required to serialize the value.
 */
static void azapZclAttrVal_t_serializeFloat(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 requiredSize)
{
  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_SEMI_PREC:
    {
      // this one is really tricky as semi-precision is not natively supported by
      // the system and would require some hazardous conversions...
      // So here is the story: We use the single precision from 'value',
      // we assume that the 10th first bits of the mantissa for 'value'
      // constitute the mantissa for semi-precision
      // exponent is deduced as follow:
      // ( Exp(semi-prec) - 15 ) = ( Exp(value) - 127 )
      // Exp(semi-prec) = Exp(value) - 112
      int val = 0;
      assert (sizeof(unsigned int) == sizeof(float));
      assert (4 == sizeof(float));
      union
      {
        float f;
        //unsigned long l;
        unsigned int l;
      } x;
      x.f = This->u.floatVal;

      int sign = ((x.l & 0x80000000) >> 16);

      if (isnan(This->u.floatVal))
      {
        val = sign | 0x7E00;
      }
      else if ((1 == isinf(This->u.floatVal)) || (-1 == isinf(This->u.floatVal)))
      {
        val = sign | 0x7C00;
      }
      else
      {
        long mantissa = ((x.l & 0x7FFFFF) >> 13);

        long exponent = ((x.l & 0x7F800000) >> 23);
        if ((exponent == 0) && ((x.l & 0x7FFFFF) == 0))
        {
          // signed zero case
        }
        else if (exponent == 0)
        {
          // de-normalized number case
        }
        else if (0xFFFF == (exponent & 0xFFFF))
        {
          // infinite if mantissa = 0
          // NaN otherwise
        }
        else
        {
          // normalized numbers
          exponent = (exponent - 112) & 0xFFFF;
          exponent = exponent << 10;
        }
        val = sign | mantissa | exponent;
      }
      (*data)[1] = (azap_byte) ((val & 0xFF00) >> 8);
      (*data)[0] = (azap_byte) (val & 0xFF);

      (*data) += 2;
      (*len) -= 2;
    }
    break;

    case ZCL_DATATYPE_SINGLE_PREC:
    {
      union
      {
        float f;
        unsigned char c[4];
      } x;
      x.f = This->u.floatVal;
      memcpy(*data, x.c, 4);
      (*data) += 4;
      (*len) -= 4;
    }
    break;

    case ZCL_DATATYPE_DOUBLE_PREC:
    {
      union
      {
        double d;
        unsigned char c[8];
      } x;
      x.d = This->u.floatVal;
      memcpy(*data, x.c, 8);
      (*data) += 8;
      (*len) -= 8;
    }
    break;

    default:
      break;
  }
}

/**
 * @brief Encode a Integer attribute value.
 * @anchor azapZclAttrVal_t_serializeInteger
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 * @param requiredSize the number of byte that are required to serialize the value.
 */
static void azapZclAttrVal_t_serializeInteger(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 requiredSize)
{
  azap_uint8 sign = 0;
  if (This->u.longVal < 0)
  {
    sign |= 0x80;
  }

  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_INT8:
      (*data)[0] = (This->u.longVal & 0x7F);
      (*data)[0] |= sign;
      (*data) += 1;
      (*len) -= 1;
      break;
    case ZCL_DATATYPE_INT16:
      (*data)[1] = ((This->u.longVal & 0x7F00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data)[1] |= sign;
      (*data) += 2;
      (*len) -= 2;
      break;
    case ZCL_DATATYPE_INT24:
      (*data)[2] = ((This->u.longVal & 0x7F0000) >> 16);
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data)[2] |= sign;
      (*data) += 3;
      (*len) -= 3;
      break;
    case ZCL_DATATYPE_INT32:
      (*data)[3] = ((This->u.longVal & 0x7F000000) >> 24);
      (*data)[2] = ((This->u.longVal & 0xFF0000) >> 16);
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data)[3] |= sign;
      (*data) += 4;
      (*len) -= 4;
      break;

    case ZCL_DATATYPE_UINT8:
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data) += 1;
      (*len) -= 1;
      break;
    case ZCL_DATATYPE_UINT16:
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data) += 2;
      (*len) -= 2;
      break;
    case ZCL_DATATYPE_UINT24:
      (*data)[2] = ((This->u.longVal & 0xFF0000) >> 16);
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data) += 3;
      (*len) -= 3;
      break;
    case ZCL_DATATYPE_UINT32:
      (*data)[3] = ((This->u.longVal & 0xFF000000) >> 24);
      (*data)[2] = ((This->u.longVal & 0xFF0000) >> 16);
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data) += 4;
      (*len) -= 4;
      break;
    case ZCL_DATATYPE_UINT40:
      (*data)[4] = ((This->u.longVal & 0xFF00000000LL) >> 32);
      (*data)[3] = ((This->u.longVal & 0xFF000000) >> 24);
      (*data)[2] = ((This->u.longVal & 0xFF0000) >> 16);
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data) += 5;
      (*len) -= 5;
      break;
    case ZCL_DATATYPE_UINT48:
      (*data)[5] = ((This->u.longVal & 0xFF0000000000LL) >> 40);
      (*data)[4] = ((This->u.longVal & 0xFF00000000LL) >> 32);
      (*data)[3] = ((This->u.longVal & 0xFF000000) >> 24);
      (*data)[2] = ((This->u.longVal & 0xFF0000) >> 16);
      (*data)[1] = ((This->u.longVal & 0xFF00) >> 8);
      (*data)[0] = (This->u.longVal & 0xFF);
      (*data) += 6;
      (*len) -= 6;
      break;
  }
}

/**
 * @brief Encode a String attribute value.
 * @anchor azapZclAttrVal_t_serializeString
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 * @param requiredSize the number of byte that are required to serialize the value.
 */
static void azapZclAttrVal_t_serializeString(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 requiredSize)
{
  if (ZCL_DATATYPE_CHAR_STR == This->zigbeeDataType)
  {
    // the serialize value length is encoded on 1 byte
    requiredSize --;
    (*data)[0] = requiredSize;
    (*data)++;
    (*len)--;
  }
  else if (ZCL_DATATYPE_LONG_CHAR_STR == This->zigbeeDataType)
  {
    // the serialize value length is encoded on 2 bytes (but here only supported on 1
    // byte...)
    requiredSize -= 2;
    (*data)[0] = requiredSize;
    (*data)[1] = 0;
    (*data) += 2;
    (*len) -= 2;
  }
  memcpy(*data, This->u.stringVal, requiredSize);
  (*data) += requiredSize;
  (*len) -= requiredSize;
}

/**
 * @brief Encode a DataTime attribute value.
 * @anchor azapZclAttrVal_t_serializeDataTime
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 * @param requiredSize the number of byte that are required to serialize the value.
 */
static void azapZclAttrVal_t_serializeDateTime(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 requiredSize)
{
  (void) requiredSize;

  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_TOD:
    {
      (*data)[0] = This->u.dateTimeVal.dateTime.tm_hour;
      (*data)[1] = This->u.dateTimeVal.dateTime.tm_min;
      (*data)[2] = This->u.dateTimeVal.dateTime.tm_sec;
      (*data)[3] = This->u.dateTimeVal.mseconds / 10;
      (*data) += 4;
      (*len) -= 4;
    }
    break;
    case ZCL_DATATYPE_DATE:
    {
      (*data)[0] = This->u.dateTimeVal.dateTime.tm_year;
      (*data)[1] = This->u.dateTimeVal.dateTime.tm_mon + 1;
      (*data)[2] = This->u.dateTimeVal.dateTime.tm_mday;
      (*data)[3] = This->u.dateTimeVal.dateTime.tm_wday;
      if (0 == This->u.dateTimeVal.dateTime.tm_wday)
      {
        // Sunday is represented by 7 in ZigBee (and not 0)
        (*data)[3] = 7;
      }
      (*data) += 4;
      (*len) -= 4;
    }
    break;
    case ZCL_DATATYPE_UTC:
    {
      time_t elapsedSec = timegm(&(This->u.dateTimeVal.dateTime));
      // Zigbee UTC time format is the number of seconds since 1st January 2000. So compare
      // to standard C representation, we have to remove 30 years more, that is to say
      // "date --date='2000-01-01 00:00:00 +0000' +%s" gives "Zigbee epoch" --> 946684800
      elapsedSec -= 946684800;
      (*data)[3] = ((elapsedSec & 0xFF000000) >> 24);
      (*data)[2] = ((elapsedSec & 0xFF0000) >> 16);
      (*data)[1] = ((elapsedSec & 0xFF00) >> 8);
      (*data)[0] = (elapsedSec & 0xFF);
      (*data) += 4;
      (*len) -= 4;
    }
    break;
  }
}

/**
 * @brief Encode a Raw attribute value.
 * @anchor azapZclAttrVal_t_serializeRaw
 * @param This the class instance
 * @param data the position in the payload where value has to be encoded. Should be
 * allocated by the issuer, and must be large enough to support the encoding.
 * @param len the reference on the payload length encoded till now. Once serialized,
 * the value of len is changed to the new payload length.
 * @param requiredSize the number of byte that are required to serialize the value.
 */
static void azapZclAttrVal_t_serializeRaw(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 requiredSize)
{
  if (ZCL_DATATYPE_OCTET_STR == This->zigbeeDataType)
  {
    // the serialize value length is encoded on 1 byte
    requiredSize --;
    (*data)[0] = requiredSize;
    (*data)++;
    (*len)--;
  }
  else if (ZCL_DATATYPE_LONG_OCTET_STR == This->zigbeeDataType)
  {
    // the serialize value length is encoded on 2 bytes (but here only supported on 1
    // byte...)
    requiredSize -= 2;
    (*data)[0] = requiredSize;
    (*data)[1] = 0;
    (*data) += 2;
    (*len) -= 2;
  }
  else
  {
    // no value length encoded in other cases
  }
  memcpy(*data, This->u.rawVal, requiredSize);
  (*data) += requiredSize;
  (*len) -= requiredSize;
}

/*   ___                  _      _ _         _   _
 *  |   \ ___ ___ ___ _ _(_)__ _| (_)_____ _| |_(_)___ _ _
 *  | |) / -_|_-</ -_) '_| / _` | | |_ / _` |  _| / _ \ ' \
 *  |___/\___/__/\___|_| |_\__,_|_|_/__\__,_|\__|_\___/_||_|
 */

/**
 * @brief Decode a boolean attribute value from a ZCL payload.
 * @anchor azapZclAttrVal_t_deserializeBoolean
 * @param This the class instance
 * @param data the position in the payload where value is encoded.
 * @param len the reference on the payload length that is still to decode.
 * @param size the size of the value to be deserialized.
 */
static void azapZclAttrVal_t_deserializeBoolean(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size)
{
  (void) size;

  if ((*data)[0] == 0x00)
  {
    This->u.booleanVal = false;
  }
  else
  {
    This->u.booleanVal = true;
  }
  (*data) ++;
  (*len) --;
}

/**
 * @brief Decode a floating-point number attribute value from a ZCL payload.
 * @anchor azapZclAttrVal_t_deserializeFloat
 * @param This the class instance
 * @param data the position in the payload where value is encoded.
 * @param len the reference on the payload length that is still to decode.
 * @param size the size of the value to be deserialized.
 */
static void azapZclAttrVal_t_deserializeFloat(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size)
{
  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_SEMI_PREC:
    {
      // semi precision
      int exponent = (((*data)[1] & 0x7c) >> 2);
      short mantissaShort = (short) ((*data)[0] & 0xFF);
      mantissaShort += ((*data)[1] & 0x03) << 8;
      float mantissa = (float)mantissaShort;
      int sign = (((*data)[1] & 0x80) >> 7);
      float hidden = 1;
      if (0 == exponent)
      {
        // un-normalized numbers
        hidden = 0;
      }
      if (31 == exponent)
      {
        if (0 != mantissaShort)
        {
          // NaN case
          This->u.floatVal = NAN;
        }
        else if (0 == sign)
        {
          // positive infinity
          This->u.floatVal = INFINITY;
        }
        else
        {
          // negative infinity
          This->u.floatVal = -INFINITY;
        }
      }
      else
      {
        This->u.floatVal = pow(-1, sign) * (hidden + (mantissa / 1024)) * pow(2, exponent - 15);
      }
      (*data) += 2;
      (*len) -= 2;
    }
    break;
    case ZCL_DATATYPE_SINGLE_PREC:
    {
      assert(sizeof(float) == 4);
      union
      {
        float f;
        unsigned char c[4];
      } x;

      memcpy(x.c, *data, 4);

      This->u.floatVal = (double)x.f;
      (*data) += 4;
      (*len) -= 4;
    }
    break;
    case ZCL_DATATYPE_DOUBLE_PREC:
    {
      assert(sizeof(double) == 8);
      union
      {
        double f;
        unsigned char c[8];
      } x;

      memcpy(x.c, *data, 8);

      This->u.floatVal = (double)x.f;
      (*data) += 8;
      (*len) -= 8;
    }
    break;
  }
}

/**
 * @brief Decode an Integer attribute value from a ZCL payload.
 * @anchor azapZclAttrVal_t_deserializeInteger
 * @param This the class instance
 * @param data the position in the payload where value is encoded.
 * @param len the reference on the payload length that is still to decode.
 * @param size the size of the value to be deserialized.
 */
static void azapZclAttrVal_t_deserializeInteger(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size)
{
  This->u.longVal = 0;

  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_INT8:
      This->u.longVal += ((*data)[0] & 0x7F);
      if (((*data)[0] & 0x80) == 0x80)
      {
        This->u.longVal -= (1LL << 7);
      }
      (*data) += 1;
      (*len) -= 1;
      break;
    case ZCL_DATATYPE_INT16:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0x7F) << 8);
      if (((*data)[1] & 0x80) == 0x80)
      {
        This->u.longVal -= (1LL << 15);
      }
      (*data) += 2;
      (*len) -= 2;
      break;
    case ZCL_DATATYPE_INT24:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      This->u.longVal += ( ((*data)[2] & 0x7F) << 16);
      if (((*data)[2] & 0x80) == 0x80)
      {
        This->u.longVal -= (1LL << 23);
      }
      (*data) += 3;
      (*len) -= 3;
      break;
    case ZCL_DATATYPE_INT32:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      This->u.longVal += ( ((*data)[2] & 0xFF) << 16);
      This->u.longVal += ( ((*data)[3] & 0x7F) << 24);
      if (((*data)[3] & 0x80) == 0x80)
      {
        This->u.longVal -= (1LL << 31);
      }
      (*data) += 4;
      (*len) -= 4;
      break;

    case ZCL_DATATYPE_UINT8:
      This->u.longVal += ((*data)[0] & 0xFF);
      (*data) += 1;
      (*len) -= 1;
      break;
    case ZCL_DATATYPE_UINT16:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      (*data) += 2;
      (*len) -= 2;
      break;
    case ZCL_DATATYPE_UINT24:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      This->u.longVal += ( ((*data)[2] & 0xFF) << 16);
      (*data) += 3;
      (*len) -= 3;
      break;
    case ZCL_DATATYPE_UINT32:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      This->u.longVal += ( ((*data)[2] & 0xFF) << 16);
      This->u.longVal += ( ((*data)[3] & 0xFF) << 24);
      (*data) += 4;
      (*len) -= 4;
      break;
    case ZCL_DATATYPE_UINT40:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      This->u.longVal += ( ((*data)[2] & 0xFF) << 16);
      This->u.longVal += ( ((*data)[3] & 0xFF) << 24);
      This->u.longVal += ( ((*data)[4] & 0xFFLL) << 32);
      (*data) += 5;
      (*len) -= 5;
      break;
    case ZCL_DATATYPE_UINT48:
      This->u.longVal += ((*data)[0] & 0xFF);
      This->u.longVal += ( ((*data)[1] & 0xFF) << 8);
      This->u.longVal += ( ((*data)[2] & 0xFF) << 16);
      This->u.longVal += ( ((*data)[3] & 0xFF) << 24);
      This->u.longVal += ( ((*data)[4] & 0xFFLL) << 32);
      This->u.longVal += ( ((*data)[5] & 0xFFLL) << 40);
      (*data) += 6;
      (*len) -= 6;
      break;
  }

}

/**
 * @brief Decode a String attribute value from a ZCL payload.
 * @anchor azapZclAttrVal_t_deserializeString
 * @param This the class instance
 * @param data the position in the payload where value is encoded.
 * @param len the reference on the payload length that is still to decode.
 * @param size the size of the value to be deserialized.
 */
static void azapZclAttrVal_t_deserializeString(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size)
{
  char *newVal = NULL;

  if (ZCL_DATATYPE_CHAR_STR == This->zigbeeDataType)
  {
    // shift the first byte
    (*data)++;
    (*len)--;
    size--;
  }
  else if (ZCL_DATATYPE_LONG_CHAR_STR == This->zigbeeDataType)
  {
    // shift the two first bytes
    (*data) += 2;
    (*len) -= 2;
    size -= 2;
  }

  if (This->u.stringVal)
  {
    free(This->u.stringVal);
  }
  This->valueSize = 0; // size will be updated if string can be allocated.
  newVal = malloc(size + 1); // this is a null-terminated string
  if (newVal)
  {
    This->valueSize = size;
    memcpy(newVal, (*data), size);
    newVal[size] = 0;
  }
  This->u.stringVal = newVal;

  (*data) += size;
  (*len) -= size;
}

/**
 * @brief Decode a DataTime attribute value from a ZCL payload.
 * @anchor azapZclAttrVal_t_deserializeDataTime
 * @param This the class instance
 * @param data the position in the payload where value is encoded.
 * @param len the reference on the payload length that is still to decode.
 * @param size the size of the value to be deserialized.
 */
static void azapZclAttrVal_t_deserializeDateTime(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size)
{
  (void) size;
  // reset the previous value before starting deserialization
  memset(&This->u.dateTimeVal, 0, sizeof(azapDateTime_t));

  switch (This->zigbeeDataType)
  {
    case ZCL_DATATYPE_TOD:
    {
      This->u.dateTimeVal.dateTime.tm_hour = (*data)[0];
      This->u.dateTimeVal.dateTime.tm_min = (*data)[1];
      This->u.dateTimeVal.dateTime.tm_sec = (*data)[2];
      This->u.dateTimeVal.mseconds = (*data)[3] * 10;
      (*data) += 4;
      (*len) -= 4;
    }
    break;
    case ZCL_DATATYPE_DATE:
    {
      This->u.dateTimeVal.dateTime.tm_year = (*data)[0];
      This->u.dateTimeVal.dateTime.tm_mon = (*data)[1] - 1;
      This->u.dateTimeVal.dateTime.tm_mday = (*data)[2];
      // Sunday is represented by 7 in ZigBee (and 0 in struct tm)
      This->u.dateTimeVal.dateTime.tm_wday = (*data)[3] % 7;
      (*data) += 4;
      (*len) -= 4;
    }
    break;
    case ZCL_DATATYPE_UTC:
    {
      // Zigbee UTC time format is the number of seconds since 1st January 2000. So compare
      // to standard C representation, we have to add 30 years more, that is to say
      // "date --date='2000-01-01 00:00:00 +0000' +%s" gives "Zigbee epoch" --> 946684800
      time_t elapsedSec = 946684800;
      elapsedSec += ((*data)[0] & 0xFF);
      elapsedSec += ( ((*data)[1] & 0xFF) << 8);
      elapsedSec += ( ((*data)[2] & 0xFF) << 16);
      elapsedSec += ( ((*data)[3] & 0xFF) << 24);
      This->setDateTimeValue(This, gmtime(&elapsedSec), 0);
      (*data) += 4;
      (*len) -= 4;
    }
    break;
  }
}

/**
 * @brief Decode a DataTime attribute value from a ZCL payload.
 * @anchor azapZclAttrVal_t_deserializeDataTime
 * @param This the class instance
 * @param data the position in the payload where value is encoded.
 * @param len the reference on the payload length that is still to decode.
 * @param size the size of the value to be deserialized.
 */
static void azapZclAttrVal_t_deserializeRaw(azapZclAttrVal_t *This, azap_byte **data,
    azap_uint8 *len, azap_uint8 size)
{
  if (ZCL_DATATYPE_OCTET_STR == This->zigbeeDataType)
  {
    // shift the first byte
    (*data)++;
    (*len)--;
    size --;
  }
  else if (ZCL_DATATYPE_LONG_OCTET_STR == This->zigbeeDataType)
  {
    // shift the two first bytes
    (*data) += 2;
    (*len) -= 2;
    size -= 2;
  }
  else
  {
    // raw value starts at first position of *data --> nothing to do
  }
  This->setRawValue(This, *data, size);
  (*data) += size;
  (*len) -= size;
}


