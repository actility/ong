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
//** File : ./_test_azap_zcl_attr_val/azap_zcl_attr_val_test.c
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


#define _ISOC99_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#include <math.h>

#include "azap_types.h"

#include "azap_api_zcl.h"
#include "azap_api.h"
#include "azap_zcl_attr_val.h"

int TraceLevel = 2;
int TraceDebug = 1;

/**
 * Tests for all functions on boolean value type
 */
void test_booleanVal()
{
  azap_byte array[3];
  azap_byte *pPtr = array;
  azap_uint8 len;

  printf("******************************************\n");
  printf("************** test_booleanVal ***********\n");

  printf("\t- basic manipulations\n");
  azapZclAttrVal_t boolVal1 = azapZclAttrVal_t_create(ZCL_DATATYPE_BOOLEAN);
  assert(boolVal1.valueType == AZAP_BOOLEAN_VAL);
  assert(boolVal1.u.booleanVal == false);

  azapZclAttrVal_t boolVal2 = azapZclAttrVal_t_create(ZCL_DATATYPE_BOOLEAN);
  boolVal2.setBooleanValue(&boolVal2, true);
  assert(!strcmp(boolVal2.getStringRep(&boolVal2), "true"));

  printf("\t- few limit case tests\n");
  memset(array, 0, sizeof(array));
  len = 0;
  azap_byte *pStart = array;
  boolVal1.serialize(&boolVal1, &pPtr, &len);
  assert(0 == len);
  assert(pStart == array);
  boolVal1.deserialize(&boolVal1, &pPtr, &len);
  assert(0 == len);
  assert(pStart == array);

  len = sizeof(array);
  printf("\t- serialization\n");
  boolVal1.serialize(&boolVal1, &pPtr, &len);
  assert(len == 2);
  boolVal2.serialize(&boolVal2, &pPtr, &len);
  assert(len == 1);
  boolVal1.serialize(&boolVal1, &pPtr, &len);
  assert(len == 0);

  assert(array[0] == 0x00);
  assert(array[1] == 0x01);
  assert(array[2] == 0x00);

  // now try to deserialize the result
  printf("\t- deserialization\n");
  pPtr = array;
  len = 3;
  azapZclAttrVal_t boolVal3 = azapZclAttrVal_t_create(ZCL_DATATYPE_BOOLEAN);
  boolVal3.deserialize(&boolVal3, &pPtr, &len);
  assert(len == 2);
  assert(boolVal3.u.booleanVal == false);
  boolVal3.deserialize(&boolVal3, &pPtr, &len);
  assert(len == 1);
  assert(boolVal3.u.booleanVal == true);
  boolVal3.deserialize(&boolVal3, &pPtr, &len);
  assert(len == 0);
  assert(boolVal3.u.booleanVal == false);

  boolVal1.free(&boolVal1);
  boolVal2.free(&boolVal2);
  boolVal3.free(&boolVal3);

  printf("\ntest_booleanVal OK\n\n");
}

/**
 * Tests for all functions on floating-point number value type
 */
void test_floatVal()
{
  azap_byte array[52];
  azap_byte *pPtr = array;
  azap_uint8 len = sizeof(array);
  azap_uint8 currentPos;

  printf("******************************************\n");
  printf("************** test_floatVal *************\n");

  printf("\t- basic manipulations\n");
  // floating-number values are used after for testing serialization
  // floatVal1 series
  azapZclAttrVal_t *floatVal1 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal1->valueType == AZAP_FLOAT_VAL);
  assert(floatVal1->u.floatVal == 0);
  floatVal1->setFloatValue(floatVal1, 0.625);
  assert(0 == strcmp("0.625", floatVal1->getStringRep(floatVal1)));

  azapZclAttrVal_t *floatVal1s = new_azapZclAttrVal_t(ZCL_DATATYPE_SINGLE_PREC);
  assert(floatVal1s->valueType == AZAP_FLOAT_VAL);
  floatVal1s->setFloatValue(floatVal1s, 0.625);
  assert(0 == strcmp("0.625", floatVal1s->getStringRep(floatVal1s)));

  azapZclAttrVal_t *floatVal1d = new_azapZclAttrVal_t(ZCL_DATATYPE_DOUBLE_PREC);
  assert(floatVal1d->valueType == AZAP_FLOAT_VAL);
  floatVal1d->setFloatValue(floatVal1d, 0.625);
  assert(0 == strcmp("0.625", floatVal1d->getStringRep(floatVal1d)));

  // floatVal2 series
  azapZclAttrVal_t *floatVal2 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal2->valueType == AZAP_FLOAT_VAL);
  floatVal2->setFloatValue(floatVal2, 0.0);
  assert(0 == strcmp("0", floatVal2->getStringRep(floatVal2)));

  azapZclAttrVal_t *floatVal2s = new_azapZclAttrVal_t(ZCL_DATATYPE_SINGLE_PREC);
  assert(floatVal2s->valueType == AZAP_FLOAT_VAL);
  floatVal2s->setFloatValue(floatVal2s, 0.0);
  assert(0 == strcmp("0", floatVal2s->getStringRep(floatVal2s)));

  azapZclAttrVal_t *floatVal2d = new_azapZclAttrVal_t(ZCL_DATATYPE_DOUBLE_PREC);
  assert(floatVal2d->valueType == AZAP_FLOAT_VAL);
  floatVal2d->setFloatValue(floatVal2d, 0.0);
  assert(0 == strcmp("0", floatVal2d->getStringRep(floatVal2d)));

  // floatVal3 series
  azapZclAttrVal_t *floatVal3 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal3->valueType == AZAP_FLOAT_VAL);
  floatVal3->setFloatValue(floatVal3, -0.625);
  assert(0 == strcmp("-0.625", floatVal3->getStringRep(floatVal3)));

  azapZclAttrVal_t *floatVal3s = new_azapZclAttrVal_t(ZCL_DATATYPE_SINGLE_PREC);
  assert(floatVal3s->valueType == AZAP_FLOAT_VAL);
  floatVal3s->setFloatValue(floatVal3s, -0.625);
  assert(0 == strcmp("-0.625", floatVal3s->getStringRep(floatVal3s)));

  azapZclAttrVal_t *floatVal3d = new_azapZclAttrVal_t(ZCL_DATATYPE_DOUBLE_PREC);
  assert(floatVal3d->valueType == AZAP_FLOAT_VAL);
  floatVal3d->setFloatValue(floatVal3d, -0.625);
  assert(0 == strcmp("-0.625", floatVal3d->getStringRep(floatVal3d)));

  // floatVal4 series
  // due to implementation, semi-precision needs more tests
  // here limit cases for semi-precision: due to mantissa length the closest precision
  // is 1/1024 (and exponent = 0)
  // so look for 1 + 1/1024 = 1.0009765625
  azapZclAttrVal_t *floatVal4 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal4->valueType == AZAP_FLOAT_VAL);
  floatVal4->setFloatValue(floatVal4, 1.0009765625);
  // displayed precision is at most 6 digits
  assert(0 == strcmp("1.00098", floatVal4->getStringRep(floatVal4)));

  // floatVal5 series
  // and all bits set to 1 on mantissa, for single precision (and exponent = 15), there is a
  // bit of precision loose...
  azapZclAttrVal_t *floatVal5 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal5->valueType == AZAP_FLOAT_VAL);
  floatVal5->setFloatValue(floatVal5, 1.9990234);
  // displayed precision is at most 6 digits
  assert(0 == strcmp("1.99902", floatVal5->getStringRep(floatVal5)));

  // floatVal6 series
  // The largest number that can be represented with semi-precision: 65504
  azapZclAttrVal_t *floatVal6 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal6->valueType == AZAP_FLOAT_VAL);
  floatVal6->setFloatValue(floatVal6, 65504.0);
  assert(0 == strcmp("65504", floatVal6->getStringRep(floatVal6)));

  // floatVal7 series
  // Special value NaN (Not-a-Number)
  azapZclAttrVal_t *floatVal7 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal7->valueType == AZAP_FLOAT_VAL);
  floatVal7->setFloatValue(floatVal7, NAN);
  assert(0 == strcmp("nan", floatVal7->getStringRep(floatVal7)));

  // floatVal8 series
  // Special value +INFINITY
  azapZclAttrVal_t *floatVal8 = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  assert(floatVal8->valueType == AZAP_FLOAT_VAL);
  floatVal8->setFloatValue(floatVal8, INFINITY);
  assert(0 == strcmp("inf", floatVal8->getStringRep(floatVal8)));

  printf("\t- serialization\n");
  // current position evolves in this function only depending on the methods that are
  // invoked. And it should follow 'len' theorical value.
  currentPos = len;

  // test the various encoding for 0.625
  floatVal1->serialize(floatVal1, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0x39);

  floatVal1s->serialize(floatVal1s, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SINGLE_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 4] == 0x00);
  assert(array[sizeof(array) - currentPos - 3] == 0x00);
  assert(array[sizeof(array) - currentPos - 2] == 0x20);
  assert(array[sizeof(array) - currentPos - 1] == 0x3F);

  floatVal1d->serialize(floatVal1d, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_DOUBLE_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 8] == 0x00);
  assert(array[sizeof(array) - currentPos - 7] == 0x00);
  assert(array[sizeof(array) - currentPos - 6] == 0x00);
  assert(array[sizeof(array) - currentPos - 5] == 0x00);
  assert(array[sizeof(array) - currentPos - 4] == 0x00);
  assert(array[sizeof(array) - currentPos - 3] == 0x00);
  assert(array[sizeof(array) - currentPos - 2] == 0xE4);
  assert(array[sizeof(array) - currentPos - 1] == 0x3F);

  // test the various encoding for 0.0
  floatVal2->serialize(floatVal2, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0x00);

  floatVal2s->serialize(floatVal2s, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SINGLE_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 4] == 0x00);
  assert(array[sizeof(array) - currentPos - 3] == 0x00);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0x00);

  floatVal2d->serialize(floatVal2d, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_DOUBLE_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 8] == 0x00);
  assert(array[sizeof(array) - currentPos - 7] == 0x00);
  assert(array[sizeof(array) - currentPos - 6] == 0x00);
  assert(array[sizeof(array) - currentPos - 5] == 0x00);
  assert(array[sizeof(array) - currentPos - 4] == 0x00);
  assert(array[sizeof(array) - currentPos - 3] == 0x00);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0x00);

  // test the various encoding for -0.625
  floatVal3->serialize(floatVal3, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0xB9);

  floatVal3s->serialize(floatVal3s, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SINGLE_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 4] == 0x00);
  assert(array[sizeof(array) - currentPos - 3] == 0x00);
  assert(array[sizeof(array) - currentPos - 2] == 0x20);
  assert(array[sizeof(array) - currentPos - 1] == 0xBF);

  floatVal3d->serialize(floatVal3d, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_DOUBLE_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 8] == 0x00);
  assert(array[sizeof(array) - currentPos - 7] == 0x00);
  assert(array[sizeof(array) - currentPos - 6] == 0x00);
  assert(array[sizeof(array) - currentPos - 5] == 0x00);
  assert(array[sizeof(array) - currentPos - 4] == 0x00);
  assert(array[sizeof(array) - currentPos - 3] == 0x00);
  assert(array[sizeof(array) - currentPos - 2] == 0xE4);
  assert(array[sizeof(array) - currentPos - 1] == 0xBF);

  // test the semi-precision encoding for 1.0009765625
  floatVal4->serialize(floatVal4, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0x01);
  assert(array[sizeof(array) - currentPos - 1] == 0x3C);

  // test the semi-precision encoding for 1.9990234
  floatVal5->serialize(floatVal5, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0xFF);
  assert(array[sizeof(array) - currentPos - 1] == 0x3F);

  // test the semi-precision encoding for 65504.0
  floatVal6->serialize(floatVal6, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0xFF);
  assert(array[sizeof(array) - currentPos - 1] == 0x7B);

  // test the semi-precision encoding for NaN
  floatVal7->serialize(floatVal7, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0x7E);

  // test the semi-precision encoding for INFINITY
  floatVal8->serialize(floatVal8, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(len == currentPos);
  assert(array[sizeof(array) - currentPos - 2] == 0x00);
  assert(array[sizeof(array) - currentPos - 1] == 0x7C);

  // For deserialization, we re-use "array" to retrieve the original serialized values
  // (or there rounded value). And so we need to keep exactly the same order of values !
  printf("\t- deserialization\n");
  len = sizeof(array);
  currentPos = sizeof(array);
  pPtr = array;
  azapZclAttrVal_t *floatValSemi = new_azapZclAttrVal_t(ZCL_DATATYPE_SEMI_PREC);
  azapZclAttrVal_t *floatValSingle = new_azapZclAttrVal_t(ZCL_DATATYPE_SINGLE_PREC);
  azapZclAttrVal_t *floatValDouble = new_azapZclAttrVal_t(ZCL_DATATYPE_DOUBLE_PREC);

  // floatVal1 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("0.625", floatValSemi->getStringRep(floatValSemi)));

  floatValSingle->deserialize(floatValSingle, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SINGLE_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("0.625", floatValSingle->getStringRep(floatValSingle)));

  floatValDouble->deserialize(floatValDouble, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_DOUBLE_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("0.625", floatValDouble->getStringRep(floatValDouble)));

  // floatVal2 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("0", floatValSemi->getStringRep(floatValSemi)));

  floatValSingle->deserialize(floatValSingle, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SINGLE_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("0", floatValSingle->getStringRep(floatValSingle)));

  floatValDouble->deserialize(floatValDouble, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_DOUBLE_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("0", floatValDouble->getStringRep(floatValDouble)));

  // floatVal3 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("-0.625", floatValSemi->getStringRep(floatValSemi)));

  floatValSingle->deserialize(floatValSingle, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SINGLE_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("-0.625", floatValSingle->getStringRep(floatValSingle)));

  floatValDouble->deserialize(floatValDouble, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_DOUBLE_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("-0.625", floatValDouble->getStringRep(floatValDouble)));

  // floatVal4 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("1.00098", floatValSemi->getStringRep(floatValSemi)));
  assert(1.0009765625 == floatValSemi->u.floatVal);

  // floatVal5 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("1.99902", floatValSemi->getStringRep(floatValSemi)));
  // did we loose precision during casting from float to double ?
  assert(1.9990234375 == floatValSemi->u.floatVal);

  // floatVal6 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("65504", floatValSemi->getStringRep(floatValSemi)));
  assert(65504.0 == floatValSemi->u.floatVal);

  // floatVal7 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("nan", floatValSemi->getStringRep(floatValSemi)));

  // floatVal8 series
  floatValSemi->deserialize(floatValSemi, &pPtr, &len);
  currentPos -= azapGetZclDataLength(ZCL_DATATYPE_SEMI_PREC, NULL);
  assert(currentPos == len);
  assert(0 == strcmp("inf", floatValSemi->getStringRep(floatValSemi)));


  floatVal1->free(floatVal1);
  floatVal1s->free(floatVal1s);
  floatVal1d->free(floatVal1d);
  floatVal2->free(floatVal2);
  floatVal2s->free(floatVal2s);
  floatVal2d->free(floatVal2d);
  floatVal3->free(floatVal3);
  floatVal3s->free(floatVal3s);
  floatVal3d->free(floatVal3d);
  floatVal4->free(floatVal4);
  floatVal5->free(floatVal5);
  floatVal6->free(floatVal6);
  floatVal7->free(floatVal7);
  floatVal8->free(floatVal8);

  floatValSemi->free(floatValSemi);
  floatValSingle->free(floatValSingle);
  floatValDouble->free(floatValDouble);

  printf("\ntest_floatVal OK\n\n");
}

/**
 * Tests for all functions on integer value type
 */
void test_integerVal()
{
  long long i;
  char szTemp[32];

  printf("******************************************\n");
  printf("************** test_integerVal ***********\n");

  printf("\t- basic manipulations\n");
  printf("\t\t- int1BVal series\n");
  azapZclAttrVal_t *int1BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_INT8);
  assert(int1BVal->valueType == AZAP_INTEGER_VAL);
  int1BVal->setIntegerValue(int1BVal, 128);
  assert(int1BVal->u.longVal == 0);
  int1BVal->setIntegerValue(int1BVal, -129);
  assert(int1BVal->u.longVal == 0);
  for (i = -128; i < 128; i++)
  {
    sprintf(szTemp, "%lld", i);
    int1BVal->setIntegerValue(int1BVal, i);
    assert(int1BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, int1BVal->getStringRep(int1BVal)));
  }

  printf("\t\t- int2BVal series\n");
  azapZclAttrVal_t *int2BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_INT16);
  assert(int2BVal->valueType == AZAP_INTEGER_VAL);
  int2BVal->setIntegerValue(int2BVal, 32768);
  assert(int2BVal->u.longVal == 0);
  int2BVal->setIntegerValue(int2BVal, -32769);
  assert(int2BVal->u.longVal == 0);
  for (i = -32768; i < 32768; i++)
  {
    sprintf(szTemp, "%lld", i);
    int2BVal->setIntegerValue(int2BVal, i);
    assert(int2BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, int2BVal->getStringRep(int2BVal)));
  }

  printf("\t\t- int3BVal series\n");
  azapZclAttrVal_t *int3BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_INT24);
  assert(int3BVal->valueType == AZAP_INTEGER_VAL);
  int3BVal->setIntegerValue(int3BVal, 8388608);
  assert(int3BVal->u.longVal == 0);
  int3BVal->setIntegerValue(int3BVal, -8388609);
  assert(int3BVal->u.longVal == 0);
  for (i = -8388608; i < 8388608; i += 100)
  {
    sprintf(szTemp, "%lld", i);
    int3BVal->setIntegerValue(int3BVal, i);
    assert(int3BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, int3BVal->getStringRep(int3BVal)));
  }
  i = 8388607;
  sprintf(szTemp, "%lld", i);
  int3BVal->setIntegerValue(int3BVal, i);
  assert(int3BVal->u.longVal == i);
  assert(0 == strcmp(szTemp, int3BVal->getStringRep(int3BVal)));


  printf("\t\t- int4BVal series\n");
  azapZclAttrVal_t *int4BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_INT32);
  assert(int4BVal->valueType == AZAP_INTEGER_VAL);
  int4BVal->setIntegerValue(int4BVal, 2147483648ll);
  assert(int4BVal->u.longVal == 0);
  int4BVal->setIntegerValue(int4BVal, -2147483649ll);
  assert(int4BVal->u.longVal == 0);
  for (i = -2147483648ll; i < 2147483648ll; i += 12345)
  {
    sprintf(szTemp, "%lld", i);
    int4BVal->setIntegerValue(int4BVal, i);
    assert(int4BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, int4BVal->getStringRep(int4BVal)));
  }
  i = 2147483647ll; // greater value
  sprintf(szTemp, "%lld", i);
  int4BVal->setIntegerValue(int4BVal, i);
  assert(int4BVal->u.longVal == i);
  assert(0 == strcmp(szTemp, int4BVal->getStringRep(int4BVal)));


  printf("\t\t- uint1BVal series\n");
  azapZclAttrVal_t *uint1BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT8);
  assert(uint1BVal->valueType == AZAP_INTEGER_VAL);
  uint1BVal->setIntegerValue(uint1BVal, -1);
  assert(uint1BVal->u.longVal == 0);
  uint1BVal->setIntegerValue(uint1BVal, 256);
  assert(uint1BVal->u.longVal == 0);
  for (i = 0; i < 256; i++)
  {
    sprintf(szTemp, "%lld", i);
    uint1BVal->setIntegerValue(uint1BVal, i);
    assert(uint1BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, uint1BVal->getStringRep(uint1BVal)));
  }

  printf("\t\t- uint2BVal series\n");
  azapZclAttrVal_t *uint2BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT16);
  assert(uint2BVal->valueType == AZAP_INTEGER_VAL);
  uint2BVal->setIntegerValue(uint2BVal, -1);
  assert(uint2BVal->u.longVal == 0);
  uint2BVal->setIntegerValue(uint2BVal, 65536);
  assert(uint2BVal->u.longVal == 0);
  for (i = 0; i < 65536; i++)
  {
    sprintf(szTemp, "%lld", i);
    uint2BVal->setIntegerValue(uint2BVal, i);
    assert(uint2BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, uint2BVal->getStringRep(uint2BVal)));
  }

  printf("\t\t- uint3BVal series\n");
  azapZclAttrVal_t *uint3BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT24);
  assert(uint3BVal->valueType == AZAP_INTEGER_VAL);
  uint3BVal->setIntegerValue(uint3BVal, -1);
  assert(uint3BVal->u.longVal == 0);
  uint3BVal->setIntegerValue(uint3BVal, 16777216);
  assert(uint3BVal->u.longVal == 0);
  for (i = 0; i < 16777216; i += 100)
  {
    sprintf(szTemp, "%lld", i);
    uint3BVal->setIntegerValue(uint3BVal, i);
    assert(uint3BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, uint3BVal->getStringRep(uint3BVal)));
  }
  i = 16777215; // greater acceptable value
  sprintf(szTemp, "%lld", i);
  uint3BVal->setIntegerValue(uint3BVal, i);
  assert(uint3BVal->u.longVal == i);
  assert(0 == strcmp(szTemp, uint3BVal->getStringRep(uint3BVal)));


  printf("\t\t- uint4BVal series\n");
  azapZclAttrVal_t *uint4BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT32);
  assert(uint4BVal->valueType == AZAP_INTEGER_VAL);
  uint4BVal->setIntegerValue(uint4BVal, -1);
  assert(uint4BVal->u.longVal == 0);
  uint4BVal->setIntegerValue(uint4BVal, 4294967296ll);
  assert(uint4BVal->u.longVal == 0);
  for (i = 0; i < 4294967296ll; i += 12345)
  {
    sprintf(szTemp, "%lld", i);
    uint4BVal->setIntegerValue(uint4BVal, i);
    assert(uint4BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, uint4BVal->getStringRep(uint4BVal)));
  }
  i = 4294967295ll; // greater acceptable value
  sprintf(szTemp, "%lld", i);
  uint4BVal->setIntegerValue(uint4BVal, i);
  assert(uint4BVal->u.longVal == i);
  assert(0 == strcmp(szTemp, uint4BVal->getStringRep(uint4BVal)));


  printf("\t\t- uint5BVal series\n");
  azapZclAttrVal_t *uint5BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT40);
  assert(uint5BVal->valueType == AZAP_INTEGER_VAL);
  uint5BVal->setIntegerValue(uint5BVal, -1);
  assert(uint5BVal->u.longVal == 0);
  uint5BVal->setIntegerValue(uint5BVal, 1099511627776ll);
  assert(uint5BVal->u.longVal == 0);
  for (i = 0; i < 1099511627776ll; i += 12345678)
  {
    sprintf(szTemp, "%lld", i);
    uint5BVal->setIntegerValue(uint5BVal, i);
    assert(uint5BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, uint5BVal->getStringRep(uint5BVal)));
  }
  i = 1099511627775ll; // greater acceptable value
  sprintf(szTemp, "%lld", i);
  uint5BVal->setIntegerValue(uint5BVal, i);
  assert(uint5BVal->u.longVal == i);
  assert(0 == strcmp(szTemp, uint5BVal->getStringRep(uint5BVal)));


  printf("\t\t- uint6BVal series\n");
  azapZclAttrVal_t *uint6BVal = new_azapZclAttrVal_t(ZCL_DATATYPE_UINT48);
  assert(uint6BVal->valueType == AZAP_INTEGER_VAL);
  uint6BVal->setIntegerValue(uint6BVal, -1);
  assert(uint6BVal->u.longVal == 0);
  uint6BVal->setIntegerValue(uint6BVal, 281474976710656ll);
  assert(uint6BVal->u.longVal == 0);
  for (i = 0; i < 281474976710656ll; i += 1234567890)
  {
    sprintf(szTemp, "%lld", i);
    uint6BVal->setIntegerValue(uint6BVal, i);
    assert(uint6BVal->u.longVal == i);
    assert(0 == strcmp(szTemp, uint6BVal->getStringRep(uint6BVal)));
  }
  i = 281474976710655ll; // greater acceptable value
  sprintf(szTemp, "%lld", i);
  uint6BVal->setIntegerValue(uint6BVal, i);
  assert(uint6BVal->u.longVal == i);
  assert(0 == strcmp(szTemp, uint6BVal->getStringRep(uint6BVal)));

  printf("\t- serialization\n");
  typedef struct
  {
    long long value;
    azap_uint8 dataType;
    azap_uint8 serialized[8];
  } int_test_series;

  const azap_uint8 nbTest = 9;
  int_test_series tests[] =
  {
    { -5, ZCL_DATATYPE_INT8, { 0xFB }},
    { -12, ZCL_DATATYPE_INT8, { 0xF4 }},
    { 1000, ZCL_DATATYPE_INT24, { 0xE8, 0x03, 0x00 }},
    { 66536, ZCL_DATATYPE_INT24, { 0xE8, 0x03, 0x01 }},
    { -1000, ZCL_DATATYPE_INT32, {  0x18, 0xFC, 0xFF, 0xFF }},
    { 1000, ZCL_DATATYPE_INT32, { 0xE8, 0x03, 0x00, 0x00 }},
    { 16778216, ZCL_DATATYPE_INT32, { 0xE8, 0x03, 0x00, 0x01 }},
    { 255, ZCL_DATATYPE_UINT8, { 0xFF }},
    { 1234, ZCL_DATATYPE_UINT16, { 0xD2, 0x04 }},
  };

  azap_uint8 sizeOfArray = 0;
  for (i = 0; i < nbTest; i++)
  {
    sizeOfArray += azapGetZclDataLength(tests[i].dataType, NULL);
  }

  azap_byte array[255];
  azap_byte *pPtr = array;
  azap_uint8 len = sizeOfArray;
  azap_uint8 currentPos, j;
  currentPos = sizeOfArray;

  for (i = 0; i < nbTest; i++)
  {
    azap_uint8 size = azapGetZclDataLength(tests[i].dataType, NULL);
    printf("\t\t- test #%lld: testing value %lld (size:%d)\n", i, tests[i].value, size);
    azapZclAttrVal_t *intVal = new_azapZclAttrVal_t(tests[i].dataType);
    assert(intVal->valueType == AZAP_INTEGER_VAL);
    intVal->setIntegerValue(intVal, tests[i].value);
    intVal->serialize(intVal, &pPtr, &len);
    currentPos -= size;
    assert(len == currentPos);
    for ( j = size; j > 0; j-- )
    {
      assert(array[sizeOfArray - currentPos - j] == tests[i].serialized[size - j]);
    }
    intVal->free(intVal);
  }
  assert(0 == len);

  printf("\t- deserialization\n");
  len = sizeOfArray;
  currentPos = sizeOfArray;
  pPtr = array;
  for (i = 0; i < nbTest; i++)
  {
    azap_uint8 size = azapGetZclDataLength(tests[i].dataType, NULL);
    printf("\t\t- test #%lld: testing value %lld (size:%d)\n", i, tests[i].value, size);
    azapZclAttrVal_t *intVal = new_azapZclAttrVal_t(tests[i].dataType);
    intVal->deserialize(intVal, &pPtr, &len);
    currentPos -= size;
    assert(intVal->u.longVal == tests[i].value);
    intVal->free(intVal);
  }
  assert(0 == len);

  printf("\ntest_integerVal OK\n\n");
}

/**
 * Tests for all functions on string value type
 */
void test_stringVal()
{
  azap_uint8 i;

  printf("******************************************\n");
  printf("************** test_stringVal ***********\n");

  printf("\t- basic manipulations\n");
  azapZclAttrVal_t *val = new_azapZclAttrVal_t(ZCL_DATATYPE_CHAR_STR);
  assert(val->valueType == AZAP_CHAR_STRING_VAL);
  val->setStringValue(val, "abcdefghijklmnopqrstuvwxyz");
  assert(0 == strcmp("abcdefghijklmnopqrstuvwxyz", val->getStringRep(val)));

  printf("\t- serialization\n");
  typedef struct
  {
    const char *value;
    azap_uint8 dataType;
    azap_uint8 serialized[255];
  } str_test_series;

  const azap_uint8 nbTest = 5;
  str_test_series tests[] =
  {
    { "test OK", ZCL_DATATYPE_CHAR_STR, { 0x07, 't', 'e', 's', 't', ' ', 'O', 'K' }},
    { "test OK", ZCL_DATATYPE_LONG_CHAR_STR, { 0x07, 0x00, 't', 'e', 's', 't', ' ', 'O', 'K' }},
    {
      "SPECIAL CHAR (,-_?.!*\"=)", ZCL_DATATYPE_CHAR_STR, {
        0x18, 'S', 'P', 'E', 'C', 'I', 'A', 'L', ' ', 'C', 'H', 'A', 'R', ' ',
        '(', ',', '-', '_', '?', '.', '!', '*', '\"', '=', ')'
      }
    },
    { "" , ZCL_DATATYPE_CHAR_STR, { 0x00 }},
    {
      "Esope reste ici et se repose" , ZCL_DATATYPE_CHAR_STR, {
        0x1C, 'E', 's', 'o', 'p',
        'e', ' ', 'r', 'e', 's', 't', 'e', ' ', 'i', 'c', 'i', ' ', 'e', 't', ' ', 's', 'e',
        ' ', 'r', 'e', 'p', 'o', 's', 'e'
      }
    },
  };

  azap_uint8 sizeOfArray = 0;
  for (i = 0; i < nbTest; i++)
  {
    sizeOfArray += azapGetZclDataLength(tests[i].dataType, tests[i].serialized);
  }

  azap_byte array[255];
  azap_byte *pPtr = array;
  azap_uint8 len = sizeOfArray;
  azap_uint8 currentPos, j;
  currentPos = sizeOfArray;

  for (i = 0; i < nbTest; i++)
  {
    azap_uint8 size = azapGetZclDataLength(tests[i].dataType, tests[i].serialized);
    printf("\t\t- test #%d: testing value \"%s\" (size:%d)\n", i, tests[i].value, size);
    azapZclAttrVal_t *strVal = new_azapZclAttrVal_t(tests[i].dataType);
    assert(strVal->valueType == AZAP_CHAR_STRING_VAL);
    strVal->setStringValue(strVal, (char *)tests[i].value);
    strVal->serialize(strVal, &pPtr, &len);
    currentPos -= size;
    assert(len == currentPos);
    for ( j = size; j > 0; j-- )
    {
      assert(array[sizeOfArray - currentPos - j] == tests[i].serialized[size - j]);
    }
    strVal->free(strVal);
  }
  assert(0 == len);


  printf("\t- deserialization\n");
  len = sizeOfArray;
  currentPos = sizeOfArray;
  pPtr = array;
  for (i = 0; i < nbTest; i++)
  {
    azap_uint8 size = azapGetZclDataLength(tests[i].dataType, tests[i].serialized);
    printf("\t\t- test #%d: testing value \"%s\" (size:%d)\n", i, tests[i].value, size);
    azapZclAttrVal_t *strVal = new_azapZclAttrVal_t(tests[i].dataType);
    strVal->deserialize(strVal, &pPtr, &len);
    currentPos -= size;
    assert(!strcmp(strVal->u.stringVal, tests[i].value));
    strVal->free(strVal);
  }
  assert(0 == len);


  printf("\ntest_stringVal OK\n\n");
}

/**
 * Tests for all functions on dateTime value type
 */
void test_dateTimeVal()
{
  printf("******************************************\n");
  printf("************** test_dateTimeVal ***********\n");

  printf("\t- basic manipulations\n");
  // nothing more for now

  printf("\t- serialization\n");
  azap_uint8 i;
  typedef struct
  {
    const char *strRep;
    azapDateTime_t value;
    azap_uint8 dataType;
    azap_uint8 serialized[255];
  } tm_test_series;

  const azap_uint8 nbTest = 3;
  tm_test_series tests[] =
  {
    // 19:33:59.990
    {
      "19:33:59.990",
      {{ 59, 33, 19, 0, 0, 0, 0, 0, 0, 0, 0 }, 990}, // "man 3 ctime" for struct tm definition
      ZCL_DATATYPE_TOD,
      { 0x13, 0x21, 0x3B, 0x63 }
    },
    // Sunday 25/12/2011
    {
      "2011-12-25",
      {{ 0, 0, 0, 25, 11, 111, 0, 0, 0, 0, 0 }, 0},
      ZCL_DATATYPE_DATE,
      { 0x6F, 0x0C, 0x19, 0x07 }
    },

    // Sunday 25/12/2011 at 23:25:25

    // "date --date='2011-12-25 23:24:25 +0000' +%s" gives the number of seconds since
    // epoch --> 1324855465
    // "date --date='2000-01-01 00:00:00 +0000' +%s" gives "Zigbee epoch"
    // --> 946684800
    // so converted result must equal 1324855465 - 946684800 (= 378170665)
    {
      "2011-12-25T23:24:25+0000",
      {{ 25, 24, 23, 25, 11, 111, 0, 0, 0, 0, 0 }, 990},
      ZCL_DATATYPE_UTC,
      { 0x29, 0x6D, 0x8A, 0x16 }
    },

  };

  azap_uint8 sizeOfArray = 0;
  for (i = 0; i < nbTest; i++)
  {
    sizeOfArray += azapGetZclDataLength(tests[i].dataType, tests[i].serialized);
  }

  azap_byte array[255];
  azap_byte *pPtr = array;
  azap_uint8 len = sizeOfArray;
  azap_uint8 currentPos, j;
  currentPos = sizeOfArray;

  for (i = 0; i < nbTest; i++)
  {
    azap_uint8 size = azapGetZclDataLength(tests[i].dataType, tests[i].serialized);
    printf("\t\t- test #%d: testing value \"%s\" (size:%d)\n", i, tests[i].strRep, size);
    azapZclAttrVal_t *val = new_azapZclAttrVal_t(tests[i].dataType);
    assert(val->valueType == AZAP_DATE_TIME_VAL);
    val->setDateTimeValue(val, &(tests[i].value.dateTime), tests[i].value.mseconds);
    val->serialize(val, &pPtr, &len);
    currentPos -= size;
    assert(len == currentPos);
    for ( j = size; j > 0; j-- )
    {
      assert(array[sizeOfArray - currentPos - j] == tests[i].serialized[size - j]);
    }
    val->free(val);
  }
  assert(0 == len);


  printf("\t- deserialization\n");
  len = sizeOfArray;
  currentPos = sizeOfArray;
  pPtr = array;
  for (i = 0; i < nbTest; i++)
  {
    azap_uint8 size = azapGetZclDataLength(tests[i].dataType, tests[i].serialized);
    printf("\t\t- test #%d: testing value \"%s\" (size:%d)\n", i, tests[i].strRep, size);
    azapZclAttrVal_t *val = new_azapZclAttrVal_t(tests[i].dataType);
    val->deserialize(val, &pPtr, &len);
    currentPos -= size;
    assert(!strcmp(val->getStringRep(val), tests[i].strRep));
    val->free(val);
  }
  assert(0 == len);


  printf("\ntest_dateTimeVal OK\n\n");
}


/******************************************************************************/
/**
 * @brief The main entry point of the test program for targeted function set.
 * It invokes every tests.
 * @param argc
 * @param argv
 * @return 0 if all tests complete successfully; the program is
 * aborted if a test fails (see tests)
 */
int main(int argc, char *argv[])
{
  test_booleanVal();
  test_floatVal();
  test_integerVal();
  test_stringVal();
  test_dateTimeVal();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}


