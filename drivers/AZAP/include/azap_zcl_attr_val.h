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
//** File : ./include/azap_zcl_attr_val.h
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


#ifndef _AZAP_ZCL_ATTR_VAL__H_
#define _AZAP_ZCL_ATTR_VAL__H_

/**
 * @file azap_zcl_attr_val.h
 * @brief The file brings the definition of azapZclAttrVal_t, which represents a value for a
 * ZCL attribute, whatever its data type is.
 * @author mlouiset
 * @date 2012-08-22
 */

#define AZAP_ATTR_VAL_MAX_SIZE 255

typedef enum
{
  AZAP_UNKNOWN_VAL_TYPE = 0,
  AZAP_BOOLEAN_VAL,
  AZAP_FLOAT_VAL,
  AZAP_INTEGER_VAL,
  AZAP_CHAR_STRING_VAL,
  AZAP_DATE_TIME_VAL,
  AZAP_RAW_VAL,
  // We may need to add others types for handling all zigbee types
  // For now on, values for unsupported types are encoded in RAW type
} AZAP_ATTR_VAL_TYPE;

/**
 * @struct azapDateTime_t
 * For storing date and time, and  milliseconds as well.
 */
typedef struct
{
  struct tm dateTime;
  int mseconds;
} azapDateTime_t;

/**
 * @struct azapZclAttrVal_t
 * The object for AF messages transaction Id management.
 */
typedef struct azapZclAttrVal_t
{
  /* methods */
  /** @ref azapZclAttrVal_t_getBooleanValue */
  bool (* getBooleanValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_setBooleanValue */
  void (* setBooleanValue) (struct azapZclAttrVal_t *, bool);

  /** @ref azapZclAttrVal_t_getFloatValue */
  double (* getFloatValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_setFloatValue */
  void (* setFloatValue) (struct azapZclAttrVal_t *, double);

  /** @ref azapZclAttrVal_t_getIntegerValue */
  long long (* getIntegerValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_setIntegerValue */
  void (* setIntegerValue) (struct azapZclAttrVal_t *, long long);

  /** @ref azapZclAttrVal_t_getStringValue */
  char *(* getStringValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_setStringValue */
  void (* setStringValue) (struct azapZclAttrVal_t *, char *);

  /** @ref azapZclAttrVal_t_getDateTimeValue */
  struct tm *(* getDateTimeValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_getDateTimeMSValue */
  int (* getDateTimeMSValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_setDateTimeValue */
  void (* setDateTimeValue) (struct azapZclAttrVal_t *, struct tm *, int);

  /** @ref azapZclAttrVal_t_getRawValue */
  unsigned char *(* getRawValue) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_setRawValue */
  void (* setRawValue) (struct azapZclAttrVal_t *, unsigned char *, azap_uint8);

  /** @ref azapZclAttrVal_t_getValueSize */
  azap_uint8 (* getValueSize) (struct azapZclAttrVal_t *);

  /** @ref azapZclAttrVal_t_serialize */
  void (* serialize) (struct azapZclAttrVal_t *, azap_byte **, azap_uint8 *);
  /** @ref azapZclAttrVal_t_deserialize */
  void (* deserialize) (struct azapZclAttrVal_t *, azap_byte **, azap_uint8 *);
  /** @ref azapZclAttrVal_t_getStringRep */
  char *(* getStringRep) (struct azapZclAttrVal_t *);
  /** @ref azapZclAttrVal_t_buildFromStringRep */
  bool (* buildFromStringRep)(struct azapZclAttrVal_t *This, char *strRep);
  /** @ref azapZclAttrVal_t_newFree */
  void (* free) (struct azapZclAttrVal_t *);

  /* members */
  /** The type of attribute value. */
  AZAP_ATTR_VAL_TYPE valueType;
  /** The ZigBee data type it implements. */
  azap_uint8 zigbeeDataType;

  /** The attribute value. */
  union
  {
    /** The boolean value when applicable. */
    bool booleanVal;
    /** The floating-point number value when applicable. */
    double floatVal;
    /** The integer value when applicable. */
    long long longVal;
    /** The character string value when applicable. */
    char *stringVal;
    /** The date/time value when applicable. */
    azapDateTime_t dateTimeVal;
    /** The character string value when applicable. */
    unsigned char *rawVal;
    /** The date/time value when applicable. */
  } u;

  /** The size of the value. Only relevant with AZAP_CHAR_STRING_VAL and AZAP_RAW_VAL. */
  azap_uint8 valueSize;

} azapZclAttrVal_t;

/* static allocation */
azapZclAttrVal_t azapZclAttrVal_t_create(azap_uint8 type);
void azapZclAttrVal_t_free(azapZclAttrVal_t *This);

/* dynamic allocation */
azapZclAttrVal_t *new_azapZclAttrVal_t(azap_uint8 type);
void azapZclAttrVal_t_newFree(azapZclAttrVal_t *This);


bool azapZclAttrVal_t_getBooleanValue(azapZclAttrVal_t *);
void azapZclAttrVal_t_setBooleanValue(azapZclAttrVal_t *, bool);
double azapZclAttrVal_t_getFloatValue(azapZclAttrVal_t *);
void azapZclAttrVal_t_setFloatValue(azapZclAttrVal_t *, double);
long long azapZclAttrVal_t_getIntegerValue(azapZclAttrVal_t *);
void azapZclAttrVal_t_setIntegerValue(azapZclAttrVal_t *, long long);
char *azapZclAttrVal_t_getStringValue(azapZclAttrVal_t *);
void azapZclAttrVal_t_setStringValue(azapZclAttrVal_t *, char *);
struct tm *azapZclAttrVal_t_getDateTimeValue(azapZclAttrVal_t *);
int azapZclAttrVal_t_getDateTimeMSValue(azapZclAttrVal_t *);
void azapZclAttrVal_t_setDateTimeValue(azapZclAttrVal_t *, struct tm *, int);
unsigned char *azapZclAttrVal_t_getRawValue(azapZclAttrVal_t *);
azap_uint8 azapZclAttrVal_t_getValueSize(azapZclAttrVal_t *);
void azapZclAttrVal_t_setRawValue(azapZclAttrVal_t *, unsigned char *, azap_uint8);
void azapZclAttrVal_t_serialize(azapZclAttrVal_t *, azap_byte **, azap_uint8 *);
void azapZclAttrVal_t_deserialize(azapZclAttrVal_t *, azap_byte **, azap_uint8 *);
char *azapZclAttrVal_t_getStringRep (azapZclAttrVal_t *);
bool azapZclAttrVal_t_buildFromStringRep(azapZclAttrVal_t *This, char *strRep);

#endif


