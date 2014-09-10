#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <byteswap.h>

#include "knx.h"

/*
 *  KNX Parser
 */
bool
knx_parse_bool(uint8_t *buffer, uint32_t bitPosition)
{
    if (buffer[bitPosition / 8] & (0x80 >> (bitPosition % 8))) {
        return true;
    } else {
        return false;
    }
}

uint32_t
knx_parse_uint(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize)
{
    uint32_t value;
    uint32_t *pvalue;

    pvalue = (uint32_t *)(buffer + (bitPosition / 8));
    value = get_unaligned(pvalue);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    value = bswap_32(value);
    value = value << (bitPosition % 8);
    value = value >> (32 - bitSize);
#else
    #error Not implemented for BE target
#endif


    return value;
}

int32_t
knx_parse_int(uint8_t *buffer, uint32_t bitPosition, uint32_t bitSize)
{
    uint32_t ui = knx_parse_uint(buffer, bitPosition, bitSize);

    // Move the bit sign
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    if (ui & (1 << (bitSize - 1))) {
        ui &= ~(1 << (bitSize - 1));
        ui = ~ui + 1;
        ui |= 0x80000000;
    }
#else
    #error Not implemented for BE target
#endif
    return (int32_t)ui;
}

float
knx_parse_float(uint8_t *buffer, uint32_t bitPosition)
{
    BUILD_BUG_ON(sizeof(uint32_t) != sizeof(float));
    
    uint32_t *ui = (uint32_t *)(buffer + (bitPosition / 8));
    uint32_t val = get_unaligned(ui);
    val = be32toh(val);
    
    float ret;
    memcpy(&ret, &val, sizeof(uint32_t));
    
    return ret;
}

float
knx_parse_float16(uint8_t *buffer, uint32_t bitPosition)
{
    fp16_t *f = (fp16_t *)(buffer + bitPosition / 8);

    f = get_unaligned(f);
    f = htobe16(f);
    
    return half2float(f);
}
