#include <assert.h>
#include "modbus-acy.h"


//char *
//modbusValueToString(uint8_t *valArray, uint16_t valLen, int modbusType, char *strVal)

void test_modbusValueToString_bool()
{
  uint8_t valArray[2];
  char strVal[8];

  printf("test_modbusValueToString_bool\n");

  valArray[0] = 0x01;
  assert(!strcmp("true", modbusValueToString(valArray, 1, ACCESS_BOOL, strVal)));

  valArray[0] = 0x00;
  assert(!strcmp("false", modbusValueToString(valArray, 1, ACCESS_BOOL, strVal)));

  valArray[0] = 0xFF;
  assert(!strcmp("true", modbusValueToString(valArray, 1, ACCESS_BOOL, strVal)));

  printf("OK\n");
}

void test_modbusValueToString_uint8()
{
  uint8_t valArray[2];
  char strVal[8];

  printf("test_modbusValueToString_uint8\n");

  valArray[0] = 0x00; 
  assert(!strcmp("0", modbusValueToString(valArray, 1, ACCESS_UINT8, strVal)));

  valArray[0] = 0xFF; 
  assert(!strcmp("255", modbusValueToString(valArray, 1, ACCESS_UINT8, strVal)));

  printf("OK\n");
}

void test_modbusValueToString_int8()
{
  uint8_t valArray[2];
  char strVal[8];

  printf("test_modbusValueToString_uint8\n");

  assert(!strcmp("##", modbusValueToString(valArray, 2, ACCESS_INT8, strVal)));

  valArray[0] = 0x00; 
  assert(!strcmp("0", modbusValueToString(valArray, 1, ACCESS_INT8, strVal)));

  valArray[0] = 0xFF;
  modbusValueToString(valArray, 1, ACCESS_INT8, strVal);
  assert(!strcmp("-1", strVal));

  valArray[0] = 0x80;
  modbusValueToString(valArray, 1, ACCESS_INT8, strVal);
  assert(!strcmp("-128", strVal));

  valArray[0] = 0x81;
  modbusValueToString(valArray, 1, ACCESS_INT8, strVal);
  assert(!strcmp("-127", strVal));

  printf("OK\n");
}

void test_modbusValueToString_uint16()
{
  uint8_t valArray[2];
  char strVal[8];

  printf("test_modbusValueToString_uint16\n");

  valArray[1] = 0x00; 
  valArray[0] = 0x00; 
  assert(!strcmp("0", modbusValueToString(valArray, 2, ACCESS_UINT16, strVal)));

  valArray[1] = 0xFF; 
  valArray[0] = 0xFF; 
  assert(!strcmp("65535", modbusValueToString(valArray, 2, ACCESS_UINT16, strVal)));

  valArray[1] = 0xAE; 
  valArray[0] = 0x41; 
  assert(!strcmp("44609", modbusValueToString(valArray, 2, ACCESS_UINT16, strVal)));

  printf("OK\n");
}

void test_modbusValueToString_int16()
{
  uint8_t valArray[2];
  char strVal[8];

  printf("test_modbusValueToString_int16\n");

  valArray[1] = 0x00; 
  valArray[0] = 0x00; 
  assert(!strcmp("0", modbusValueToString(valArray, 2, ACCESS_INT16, strVal)));

  valArray[1] = 0xFF; 
  valArray[0] = 0xFF; 
  assert(!strcmp("-1", modbusValueToString(valArray, 2, ACCESS_INT16, strVal)));

  valArray[1] = 0xAE; 
  valArray[0] = 0x41; 
  assert(!strcmp("-20927", modbusValueToString(valArray, 2, ACCESS_INT16, strVal)));

  printf("OK\n");
}

void test_modbusValueToString_uint32()
{
  uint8_t valArray[4];
  char strVal[16];

  printf("test_modbusValueToString_uint32\n");

  valArray[3] = 0x00; 
  valArray[2] = 0x00; 
  valArray[1] = 0x00; 
  valArray[0] = 0x00; 
  modbusValueToString(valArray, 4, ACCESS_UINT32, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("0", strVal));

  valArray[3] = 0xFF; 
  valArray[2] = 0xFF; 
  valArray[1] = 0xFF; 
  valArray[0] = 0xFF; 
  modbusValueToString(valArray, 4, ACCESS_UINT32, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("4294967295", strVal));

  valArray[3] = 0xAE; 
  valArray[2] = 0x41; 
  valArray[1] = 0x56; 
  valArray[0] = 0x52; 
  modbusValueToString(valArray, 4, ACCESS_UINT32, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("2923517522", strVal));

  printf("OK\n");
}

void test_modbusValueToString_int32()
{
  uint8_t valArray[4];
  char strVal[16];

  printf("test_modbusValueToString_int32\n");

  valArray[3] = 0x00; 
  valArray[2] = 0x00; 
  valArray[1] = 0x00; 
  valArray[0] = 0x00; 
  modbusValueToString(valArray, 4, ACCESS_INT32, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("0", strVal));

  valArray[3] = 0xFF; 
  valArray[2] = 0xFF; 
  valArray[1] = 0xFF; 
  valArray[0] = 0xFF; 
  modbusValueToString(valArray, 4, ACCESS_INT32, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("-1", strVal));

  valArray[3] = 0xAE; 
  valArray[2] = 0x41; 
  valArray[1] = 0x56; 
  valArray[0] = 0x52; 
  modbusValueToString(valArray, 4, ACCESS_INT32, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("-1371449774", strVal));

  printf("OK\n");
}

void test_modbusValueToString_float()
{
  uint8_t valArray[4];
  char strVal[16];

  printf("test_modbusValueToString_float\n");

  valArray[3] = 0xAE; 
  valArray[2] = 0x41; 
  valArray[1] = 0x56; 
  valArray[0] = 0x52; 
  modbusValueToString(valArray, 4, ACCESS_FLOAT, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("-4.39598e-11", strVal));

  printf("OK\n");
}

void test_modbusValueToString_double()
{
  uint8_t valArray[8];
  char strVal[32];

  printf("test_modbusValueToString_double\n");

  valArray[7] = 0xE2; 
  valArray[6] = 0x5F; 
  valArray[5] = 0x11; 
  valArray[4] = 0x90; 
  valArray[3] = 0xAE; 
  valArray[2] = 0x41; 
  valArray[1] = 0x56; 
  valArray[0] = 0x52; 
  modbusValueToString(valArray, 8, ACCESS_DOUBLE, strVal);
  printf("%s\n", strVal);
  assert(!strcmp("-7.15648e+165", strVal));

  printf("OK\n");
}

int main(int argc, char *argv[])
{
  test_modbusValueToString_bool();
  test_modbusValueToString_uint8();
  test_modbusValueToString_int8();
  test_modbusValueToString_uint16();
  test_modbusValueToString_int16();
  test_modbusValueToString_uint32();
  test_modbusValueToString_int32();
  test_modbusValueToString_float();
  test_modbusValueToString_double();
  return 0;
}

