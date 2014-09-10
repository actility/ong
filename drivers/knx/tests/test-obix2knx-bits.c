#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "ok.h"
#include "knx.h"

void knx_set_bool(uint8_t *buffer, uint32_t bitPosition, bool value);
void knx_set_uint(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize, uint32_t value);
void knx_set_int(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize, int32_t value);
void knx_set_float(uint8_t *buffer, uint32_t bitPosition, float value);
void knx_set_float16(uint8_t *buffer, uint32_t bitPosition, float value);

int
main (int argc, char **argv)
{
    uint8_t buffer[1024];

    /*
     *  Set some bits
     */
    memset(buffer, 0, 1024);
     
    knx_set_bool(buffer, 0, true);
    assert(buffer[0] & 0x80);

    knx_set_bool(buffer, 0, false);
    assert((buffer[0] & 0x80) == 0);
    
    knx_set_bool(buffer, 1, true);
    assert(buffer[0] & 0x40);
    
    knx_set_bool(buffer, 7, true);
    assert(buffer[0] & 0x01);

    knx_set_bool(buffer, 8, true);
    assert(buffer[1] & 0x80);

    knx_set_bool(buffer, 15, true);
    assert(buffer[1] & 0x01);


    /*
     *  Unsigned int
     */
    memset(buffer, 0, 1024);
    
    knx_set_uint(buffer, 0, 8, 0x55);
    assert(buffer[0] == 0x55);

    knx_set_uint(buffer, 8, 8, 0xFF);
    assert(buffer[1] == 0xFF);

    knx_set_uint(buffer, 16, 4, 0x0A);
    assert(buffer[2] == 0xA0);

    knx_set_uint(buffer, 24, 16, 65535);
    assert(buffer[3] == 0xFF);
    assert(buffer[4] == 0xFF);
    
    /*
     *  Signed int
     */
    memset(buffer, 0, 1024);

    knx_set_int(buffer, 0, 8, -5);
    assert(buffer[0] == 0xFB);

    knx_set_int(buffer, 8, 8, 5);
    assert(buffer[1] == 5);

    /*
     *  Float 32 bits
     */
    memset(buffer, 0, 1024);
    knx_set_float(buffer, 0, 123.24688f);
    assert(buffer[0] == 0x67);
    assert(buffer[1] == 0x7E);
    assert(buffer[2] == 0xF6);
    assert(buffer[3] == 0x42);

    /*
     *  Float 16 bits
     */
    memset(buffer, 0, 1024);
    knx_set_float16(buffer, 0, -2.0f);
    assert(buffer[0] == 0xC8);
    assert(buffer[1] == 0x80);

    PRINT_OK();
    return 0;
}

