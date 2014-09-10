#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <byteswap.h>

#include "knx.h"

void
knx_set_bool(uint8_t *buffer, uint32_t bitPosition, bool value)
{
    //printf("knx_set_bool: bit %u to %u\n", bitPosition, value);
    
    if (value == true) {
        buffer[bitPosition / 8] |= (0x80 >> (bitPosition % 8));
    } else {
        buffer[bitPosition / 8] &= ~(0x80 >> (bitPosition % 8));
    }
}

void
knx_set_uint(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize, uint32_t value)
{
    int i;
    
    // Process bit by bit, an easy way to handle : 6 bits int write on two bytes
    for(i=0; i<bitSize; i++) {
        knx_set_bool(buffer, bitPosition + bitSize - 1 - i, value & 1);
        value /= 2;
    }
}

void 
knx_set_int(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize, int32_t value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    // Move the bit sign
    if (value & 0x80000000) {
        value |= (1 << (bitSize - 1));
    } else {
        value &= ~(1 << (bitSize - 1));
    }
#else
    #error Not implemented for BE target
#endif
    
    knx_set_uint(buffer, bitPosition, bitSize, (uint32_t) value);
}

void 
knx_set_float(uint8_t *buffer, uint32_t bitPosition, float value)
{
    uint32_t tmp;
    BUILD_BUG_ON(sizeof(uint32_t) != sizeof(float));
    
    memcpy(&tmp, &value, sizeof(uint32_t));
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    tmp = bswap_32(tmp);
#else
    tmp = tmp;
#endif

    knx_set_uint(buffer, bitPosition, 32, tmp);
}

void 
knx_set_float16(uint8_t *buffer, uint32_t bitPosition, float value)
{
    fp16_t tmp = float2half(value);

    knx_set_uint(buffer, bitPosition, 16, tmp);
}
