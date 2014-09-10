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
//** File : ./_test_azap_tools/azap_tools_test.c
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



#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include "azap_types.h"
#include "azap_tools.h"



/**
 * Tests for azap_extAddrToString function
 */
void test_azap_extAddrToString()
{
  printf("******************************************\n");
  printf("******* test_azap_extAddrToString *******\n");

  printf("\t- straight test\n");
  azap_uint8 extAddr1[] = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
  azap_uint8 extAddr2[] = { 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09 };
  azap_uint8 extAddr3[] = { 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11 };
  assert(0 == strcmp(azap_extAddrToString(extAddr1, true), "0x0102030405060708"));
  assert(0 == strcmp(azap_extAddrToString(extAddr1, false), "0102030405060708"));

  printf("\t- test null data\n");
  assert(0 == strcmp(azap_extAddrToString(NULL, true), ""));

  printf("\t- Several calls in the same sprintf call\n");
  char szTemp[255];
  sprintf(szTemp, "%s-%s-%s", azap_extAddrToString(extAddr1, true),
      azap_extAddrToString(extAddr2, true),  azap_extAddrToString(extAddr3, true));
  printf("\t\t-> res:%s\n", szTemp);
  assert(0 == strcmp(szTemp, "0x0102030405060708-0x090a0b0c0d0e0f10-0x1112131415161718"));

  printf("\ntest_azap_extAddrToString OK\n\n");
}

/**
 * Tests for stringToAzap_extAddr function
 */
void test_stringToAzap_extAddr()
{
  printf("******************************************\n");
  printf("******* test_stringToAzap_extAddr ********\n");

  printf("\t- straight test\n");
  azap_uint8 extAddr1[] = { 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01 };
  azap_uint8 extAddr2[] = { 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09 };
  azap_uint8 extAddr3[] = { 0x18, 0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11 };
  azap_uint8 extAddr4[] = { 0xFF, 0xEE, 0x7F, 0x80, 0x00, 0xFF, 0x7F, 0x00 };

  azap_uint8 res[IEEE_ADDR_LEN];

  assert(true == stringToAzap_extAddr(res, "0x0102030405060708"));
  assert(0 == memcmp(res, extAddr1, IEEE_ADDR_LEN));

  assert(true == stringToAzap_extAddr(res, "0x090a0b0c0d0e0f10"));
  assert(0 == memcmp(res, extAddr2, IEEE_ADDR_LEN));

  assert(true == stringToAzap_extAddr(res, "0x1112131415161718"));
  assert(0 == memcmp(res, extAddr3, IEEE_ADDR_LEN));

  assert(true == stringToAzap_extAddr(res, "0x007fff00807feeff"));
  assert(0 == memcmp(res, extAddr4, IEEE_ADDR_LEN));

  printf("\t- test null data\n");
  assert(false == stringToAzap_extAddr(NULL, "0x007fff00807feeff"));
  assert(false == stringToAzap_extAddr(res, NULL));

  printf("\t- wrong string format\n");
  assert(false == stringToAzap_extAddr(res, "x007fff00807feeff0"));
  assert(false == stringToAzap_extAddr(res, "007fff00807feeff00"));
  assert(false == stringToAzap_extAddr(res, "0x007fff00807feeff00"));
  assert(false == stringToAzap_extAddr(res, "0x007fff00807feef"));

  printf("\ntest_stringToAzap_extAddr OK\n\n");
}

/**
 * Tests for azap_byteArrayToString function
 */
void test_azap_byteArrayToString()
{
  printf("******************************************\n");
  printf("******* test_azap_byteArrayToString *******\n");
  azap_byte array[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57,
    0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
    0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F
  };

  printf("\t- straight test\n");
  char *szTemp = azap_byteArrayToString(array, 16);
  assert(0 == strcmp(szTemp,
      "0x000102030405060708090a0b0c0d0e0f"));

  printf("\t- test null data\n");
  assert(0 == strcmp(azap_byteArrayToString(NULL, 16), ""));

  printf("\t- buffer overflow prevention test\n");
  szTemp = azap_byteArrayToString(array, 144);
  printf("\t\t-> res:%s\n", szTemp);
  assert( ((((STR_MAX_SIZE - 2) / 2 ) * 2 )) == strlen(szTemp) );

  assert(0 == strncmp(szTemp,
      "0x000102030405060708090a0b0c0d0e0f"
      "101112131415161718191a1b1c1d1e1f"
      "202122232425262728292a2b2c2d2e2f"
      "303132333435363738393a3b3c3d3e3f"
      "404142434445464748494a4b4c4d4e4f"
      "505152535455565758595a5b5c5d5e5f"
      "606162636465666768696a6b6c6d6e6f"
      "707172737475767778797a7b7c7d7e7f"
      "808182838485868788898a8b8c8d8e8f",
      (((STR_MAX_SIZE - 2) / 2 ) * 2 )));

  assert(0 != strcmp(szTemp,
      "0x000102030405060708090a0b0c0d0e0f"
      "101112131415161718191a1b1c1d1e1f"
      "202122232425262728292a2b2c2d2e2f"
      "303132333435363738393a3b3c3d3e3f"
      "404142434445464748494a4b4c4d4e4f"
      "505152535455565758595a5b5c5d5e5f"
      "606162636465666768696a6b6c6d6e6f"
      "707172737475767778797a7b7c7d7e7f"
      "808182838485868788898a8b8c8d8e8f"));

  printf("\ntest_azap_byteArrayToString OK\n\n");
}

/**
 * Tests for stringToAzap_byteArray function
 */
void test_stringToAzap_byteArray()
{
  printf("******************************************\n");
  printf("******* test_stringToAzap_byteArray *******\n");

  azap_byte res[32];
  memset(res, 0, 32);

  printf("\t- straight test\n");
  assert( 16 == stringToAzap_byteArray(res, 16, "0x000102030405060708090a0b0c0d0e0f") );
  assert(0x00 == res[0]);
  assert(0x01 == res[1]);
  assert(0x02 == res[2]);
  assert(0x03 == res[3]);
  assert(0x04 == res[4]);
  assert(0x05 == res[5]);
  assert(0x06 == res[6]);
  assert(0x07 == res[7]);
  assert(0x08 == res[8]);
  assert(0x09 == res[9]);
  assert(0x0a == res[10]);
  assert(0x0b == res[11]);
  assert(0x0c == res[12]);
  assert(0x0d == res[13]);
  assert(0x0e == res[14]);
  assert(0x0f == res[15]);
  assert(0x00 == res[16]);

  printf("\t- test null data\n");
  assert( 0 == stringToAzap_byteArray(NULL, 16, "0x000102030405060708") );
  assert( 0 == stringToAzap_byteArray(res, 16, NULL) );
  assert( 0 == stringToAzap_byteArray(res, 8, "0x000102030405060708") );
  assert( 9 == stringToAzap_byteArray(res, 9, "0x000102030405060708") );

  printf("\ntest_stringToAzap_byteArray OK\n\n");
}

/**
 * Tests for azap_uint8ArrayToString function
 */
void test_azap_uint8ArrayToString()
{
  printf("******************************************\n");
  printf("******* test_azap_uint8ArrayToString *******\n");
  azap_byte array[] =
  {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
    0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
    0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F
  };

  printf("\t- straight test\n");
  char *szTemp = azap_uint8ArrayToString(array, 16);
  assert(0 == strcmp(szTemp,
      "0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f"));

  printf("\t- test null data\n");
  assert(0 == strcmp(azap_uint8ArrayToString(NULL, 16), ""));

  printf("\t- buffer overflow prevention test\n");
  szTemp = azap_uint8ArrayToString(array, 64);
  printf("\t\t-> res:%s\n", szTemp);
  assert(0 != strcmp(szTemp,
      "0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f "
      "0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f "
      "0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f "
      "0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f"));

  assert( ((((STR_MAX_SIZE - 1) / 5 ) * 5 ) - 1) == strlen(szTemp) );

  assert(0 == strncmp(szTemp,
      "0x00 0x01 0x02 0x03 0x04 0x05 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d 0x0e 0x0f "
      "0x10 0x11 0x12 0x13 0x14 0x15 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d 0x1e 0x1f "
      "0x20 0x21 0x22 0x23 0x24 0x25 0x26 0x27 0x28 0x29 0x2a 0x2b 0x2c 0x2d 0x2e 0x2f "
      "0x30 0x31 0x32 0x33 0x34 0x35 0x36 0x37 0x38 0x39 0x3a 0x3b 0x3c 0x3d 0x3e 0x3f",
      (((STR_MAX_SIZE - 1) / 5 ) * 5 ) - 1));

  printf("\ntest_azap_uint8ArrayToString OK\n\n");
}


/**
 * Tests for azap_uint16ArrayToString function
 */
void test_azap_uint16ArrayToString()
{
  printf("******************************************\n");
  printf("******* test_azap_uint16ArrayToString *******\n");
  azap_uint16 array[] =
  {
    0x0000, 0x0001, 0x0002, 0x0003, 0x0004, 0x0005, 0x0006, 0x0007,
    0x0108, 0x0109, 0x010A, 0x010B, 0x010C, 0x010D, 0x010E, 0x010F,
    0x0210, 0x0211, 0x0212, 0x0213, 0x0214, 0x0215, 0x0216, 0x0217,
    0x0318, 0x0319, 0x031A, 0x031B, 0x031C, 0x031D, 0x031E, 0x031F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0528, 0x0529, 0x052A, 0x052B, 0x052C, 0x052D, 0x052E, 0x052F,
    0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x0637,
    0x0738, 0x0739, 0x073A, 0x073B, 0x073C, 0x073D, 0x073E, 0x073F
  };

  printf("\t- straight test\n");
  char *szTemp = azap_uint16ArrayToString(array, 16);
  assert(0 == strcmp(szTemp,
      "0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 "
      "0x0108 0x0109 0x010a 0x010b 0x010c 0x010d 0x010e 0x010f"));

  printf("\t- test null data\n");
  assert(0 == strcmp(azap_uint16ArrayToString(NULL, 16), ""));

  printf("\t- buffer overflow prevention test\n");
  szTemp = azap_uint16ArrayToString(array, 64);
  printf("\t\t-> res:%s\n", szTemp);
  assert(0 != strcmp(szTemp,
      "0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 "
      "0x0108 0x0109 0x010a 0x010b 0x010c 0x010d 0x010e 0x010f "
      "0x0210 0x0211 0x0212 0x0213 0x0214 0x0215 0x0216 0x0217 "
      "0x0318 0x0319 0x031a 0x031b 0x031c 0x031d 0x031e 0x031f "
      "0x0420 0x0421 0x0422 0x0423 0x0424 0x0425 0x0426 0x0427 "
      "0x0528 0x0529 0x052a 0x052b 0x052c 0x052d 0x052e 0x052f "
      "0x0630 0x0631 0x0632 0x0633 0x0634 0x0635 0x0636 0x0637 "
      "0x0738 0x0739 0x073a 0x073b 0x073c 0x073d 0x073e 0x073f" ));

  assert( ((((STR_MAX_SIZE - 1) / 7 ) * 7 ) - 1) == strlen(szTemp) );

  assert(0 == strncmp(szTemp,
      "0x0000 0x0001 0x0002 0x0003 0x0004 0x0005 0x0006 0x0007 "
      "0x0108 0x0109 0x010a 0x010b 0x010c 0x010d 0x010e 0x010f "
      "0x0210 0x0211 0x0212 0x0213 0x0214 0x0215 0x0216 0x0217 "
      "0x0318 0x0319 0x031a 0x031b 0x031c 0x031d 0x031e 0x031f "
      "0x0420 0x0421 0x0422 0x0423 0x0424 0x0425 0x0426 0x0427 "
      "0x0528 0x0529 0x052a 0x052b 0x052c 0x052d 0x052e 0x052f "
      "0x0630 0x0631 0x0632 0x0633 0x0634 0x0635 0x0636 0x0637 "
      "0x0738 0x0739 0x073a 0x073b 0x073c 0x073d 0x073e 0x073f",
      (((STR_MAX_SIZE - 1) / 7 ) * 7 ) - 1));


  printf("\ntest_azap_uint16ArrayToString OK\n\n");
}


/**
 * Tests for isExtAddrNull function
 */
void test_isExtAddrNull()
{
  printf("******************************************\n");
  printf("*********** test_isExtAddrNull ***********\n");

  azap_uint8 extAddr0[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
  azap_uint8 extAddr1[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
  azap_uint8 extAddr2[] = { 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A, 0x09 };
  azap_uint8 extAddr3[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

  assert( true == isExtAddrNull(NULL) );
  assert( true == isExtAddrNull(extAddr0) );
  assert( false == isExtAddrNull(extAddr1) );
  assert( false == isExtAddrNull(extAddr2) );
  assert( false == isExtAddrNull(extAddr3) );

  printf("\ntest_isExtAddrNull OK\n\n");
}



/******************************************************************************/
/**
 * @brief The main entry point of the test program for azap_tools function set.
 * It invokes every tests.
 * @param argc
 * @param argv
 * @return 0 if all tests complete successfully; the program is
 * aborted if a test fails (see tests)
 */
int main(int argc, char *argv[])
{
  test_azap_extAddrToString();
  test_stringToAzap_extAddr();
  test_azap_byteArrayToString();
  test_stringToAzap_byteArray();
  test_azap_uint8ArrayToString();
  test_azap_uint16ArrayToString();

  test_isExtAddrNull();

  printf("\t ___\n");
  printf("\t/ __|_  _ __ __ ___ ______\n");
  printf("\t\\__ \\ || / _/ _/ -_|_-<_-<\n");
  printf("\t|___/\\_,_\\__\\__\\___/__/__/\n");
  printf("\t\n");

  return 0;
}


