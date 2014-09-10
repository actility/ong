#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>

#include "ok.h"
#include "knx.h"

fp16_t float2half(float f32);
float half2float(fp16_t value);

int
main (int argc, char **argv)
{
    fp16_t h1;
    float f1, f2;

    /*
     *  From Half to single
     */
    h1 = 0x3E52;
    f2 = half2float(h1);
    assert(fabs(f2 - 2071.04f) < 0.1f);

    h1 = 0x3CA1;
    f2 = half2float(h1);
    assert(fabs(f2 - 1516.80f) < 0.1f);
    
    /*
     *  Some test on non rouded values
     */
    f1 = 0.0f;
    h1 = float2half(f1);
    f2 = half2float(h1);
    assert(f1 == f2);

    f1 = -0.0f;
    h1 = float2half(f1);
    f2 = half2float(h1);
    assert(f1 == f2);
    
    f1 = 1.0f;
    h1 = float2half(f1);
    f2 = half2float(h1);
    assert(f1 == f2);

    f1 = -2.0f;
    h1 = float2half(f1);
    f2 = half2float(h1);
    assert(f1 == f2);
    
    f1 = 1516.80f;
    h1 = float2half(f1);
    f2 = half2float(h1);
    assert(fabs(f2 - f1) < 0.1f);
    
    PRINT_OK();
    return 0;
}
