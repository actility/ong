#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "ok.h"

bool knx_parse_bool(uint8_t *buffer, uint32_t bitPosition);
uint32_t knx_parse_uint(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize);
int32_t knx_parse_int(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize);
float knx_parse_float(uint8_t *buffer);

int
main (int argc, char **argv)
{
    bool b;
    uint32_t ui;
    int32_t i;
    float f;

    char *test01 = "\x00";
    b = knx_parse_bool(test01, 7);
    assert(b == false);
    
    char *test02 = "\x01";
    b = knx_parse_bool(test02, 7);
    assert(b == true);
    b = knx_parse_bool(test02, 6);
    assert(b == false);

    char *test03 = "\x05";
    ui = knx_parse_uint(test03, 5, 3);
    assert(ui == 5);
    ui = knx_parse_uint(test03, 6, 2);
    assert(ui == 1);
    i = knx_parse_int(test03, 5, 3);
    assert(i == -1);
    i = knx_parse_int(test03, 6, 2);
    assert(i == 1);

    char *test03b = "\x12";
    i = knx_parse_int(test03b, 0, 8);
    assert(i == 18);
    
    //  KNX is Big endian
    char *test04 = "\x03\x35";
    ui = knx_parse_uint(test04, 6, 10);
    assert(ui == 821);
    i = knx_parse_int(test04, 6, 10);
    assert(i == -309);

    char *test05 = "\x67\x7e\xf6\x42";
    f = knx_parse_float(test05);
    assert(f == 123.24688f);
    
    // FFS : Add more tests
    
    PRINT_OK();
    return 0;
}

