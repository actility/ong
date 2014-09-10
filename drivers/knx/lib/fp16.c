#include <knx.h>

//
//  KNX FLOAT 16-Bits (Big endian + Custom encoding)
//


static const float knx_exp[] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f, 128.0f, 256.0f, 512.0f, 1024.0f, 2048.0f, 4096.0f, 8192.0f, 16384.0f, 32768.0f};

fp16_t
float2half(float f32)
{
    int i;
    fp16_t half = 0x0000;

    if (isinf(f32) || isnan(f32)) 
        return 0x7FFF;      // KNX Invalid data

    if (f32 >= 670760.96f)
        return 0x7FFE;

    if (f32 <= -671088.64f)
        return 0xFFFF;

    f32 = f32 * 100.0f;
    
    float e, m;
    for(i=0; i<16; i++) {
        e = knx_exp[i];
        m = f32 / e;
        
        if (m < 2047.0f && m > -2048.0f) {
            break;
        } 
    }
    
    if (f32 < 0.0f) {
        half |= 0x8000;
        m = -1.0 * m;
    }
    
    half |= ((i & 0xF) << 11);
    half |= (((int)m) & 0x7FF);

    return half;
}

float
half2float(fp16_t value)
{
    if (value == 0x7FFF)
        return 0.0f / 0.0f;    // Get a NaN
        
    int e = (value >> 11) & 0x0F;
    int m = value & 0x07FF;
    float single = 0.01f * m * knx_exp[e];
    
    if (value & 0x8000)
        single = single * -1.0f;
    
    return single;
}
