#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "knx.h"

/*
 *  Obix Parser helpers
 *  Use xo-helpers & obix2knx-bits
 */
#define PARSE_BOOL(pos, path) {                 \
    bool b;                                     \
    rc = getObixBoolValue(xo, &b, path);        \
    if (rc == 0) {                              \
        knx_set_bool((uint8_t *)buffer, pos, b);           \
    }                                           \
}

#define PARSE_UINT(pos, sz, path) {                     \
    uint32_t ui;                                        \
    rc = getObixUintValue(xo, &ui, path);               \
    if (rc == 0) {                                      \
        knx_set_uint((uint8_t *)buffer, pos, sz, (uint32_t)ui);    \
    }                                                   \
}

#define PARSE_INT(pos, sz, path) {              \
    uint32_t i;                                 \
    rc = getObixIntValue(xo, &i, path);         \
    if (rc == 0) {                              \
        knx_set_int((uint8_t *)buffer, pos, sz, i);        \
    }                                           \
}

#define PARSE_FLOAT16(pos, path) {              \
    float f;                                    \
    rc = getObixRealValue(xo, &f, path);        \
    if (rc == 0) {                              \
        knx_set_float16((uint8_t *)buffer, pos, f);        \
    }                                           \
}

#define PARSE_FLOAT(pos, path) {                \
    float f;                                    \
    rc = getObixRealValue(xo, &f, path);        \
    if (rc == 0) {                              \
        knx_set_float((uint8_t *)buffer, pos, f);          \
    }                                           \
}

#define UNABSTRACT_DPT(i) \
    unabstract_dpt ## i (dpt, xo, (uint8_t *)buffer)

static
void
unabstract_dpt2(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int i;
    int32_t rc;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    rc = RangeGetBinValue(dpt, &i, str);
    if (rc != 0) {
        return;
    }

    knx_set_bool(buffer, 7, i & 0x01);
    knx_set_bool(buffer, 6, i & 0x02);
}

static
void
unabstract_dpt3(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t i, j, rc;
    float f[8] = {0.0f, 50.0f, 25.0f, 12.5f, 6.25f, 3.125f, 1.5625f, 0.78125f};
    float in, near = 9999.0f;

    rc = getObixRealValue(xo, &in, "val");
    if (rc != 0) {
        return;
    }

    // Sign
    if (in < 0) {
        in = -1.0f * in;
        knx_set_bool(buffer, 4, false);
    } else {
        knx_set_bool(buffer, 4, true);
    }

    // Special case == break
    if (in == 0.0f) {
        knx_set_uint(buffer, 5, 3, 0);
        return;
    }

    // Find near percent
    for (i=j=1; i<8; i++) {
        if (fabsf(in - f[i]) < near) {
            near = f[i];
            j = i;
        }
    }
    knx_set_uint(buffer, 5, 3, j);
}

static
void
unabstract_dpt5(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc;
    float in, scaleFactor;

    rc = getObixRealValue(xo, &in, "val");
    if (rc != 0) {
        return;
    }

    switch(dpt.sub) {
        case 1:
            scaleFactor = 256.0f / 100.0f;
            break;

        case 2:
            scaleFactor = 256.0f / 360.0f;
            break;

        default:
            scaleFactor = 1.0f;
    }

    in = in * scaleFactor;

    knx_set_uint(buffer, 0, 8, ((uint32_t) in));
}

static
void
unabstract_dpt7(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc, ts, ms;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }
    
    ts = parseISO8601Duration_ms(str, &ms);
    if (ts < 0) {
        // No negative duration here
        ts = 0;
        ms = 0;
    }   

    switch(dpt.sub) {
        case 2:
            ts = ts * 1000 + ms;
            break;

        case 3:
            ts = ts * 100 + (ms / 10);
            break;

        case 4:
            ts = ts * 10 + (ms / 100);
            break;

        case 5:
            ts = ts * 1;
            break;

        case 6:
            ts = ts / 60;
            break;

        case 7:
            ts = ts / (60 * 60);
            break;
    }
    
    knx_set_uint(buffer, 0, 16, ((uint32_t) ts));
}

static
void
unabstract_dpt8_1(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc;
    float in, scaleFactor;

    rc = getObixRealValue(xo, &in, "val");
    if (rc != 0) {
        return;
    }

    switch(dpt.sub) {
        case 10:
            scaleFactor = 100.0f;
            break;

        default:
            scaleFactor = 1.0f;
    }

    in = in * scaleFactor;

    knx_set_int(buffer, 0, 16, ((uint32_t) in));
}

static
void
unabstract_dpt8_2(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc, ts, ms;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    ts = parseISO8601Duration_ms(str, &ms);
    switch(dpt.sub) {
        case 2:
            ts = ts * 1000 + ms;
            break;

        case 3:
            ts = ts * 100 + (ms / 10);
            break;

        case 4:
            ts = ts * 10 + (ms / 100);
            break;

        case 5:
            ts = ts * 1;
            break;

        case 6:
            ts = ts / 60;
            break;

        case 7:
            ts = ts / (60 * 60);
            break;
    }
    
    knx_set_int(buffer, 0, 16, ts);
}

static
void
unabstract_dpt9(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc;
    float f;

    rc = getObixRealValue(xo, &f, "val");
    if (rc != 0) {
        return;
    }
    
    // FFS : Scale ?
    
    knx_set_float16(buffer, 0, f);    
}

static
void
unabstract_dpt10(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc;
    uint32_t hours, minutes, seconds;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    // ISO8601 Time
    rc = sscanf(str, "T%u:%u:%u", &hours, &minutes, &seconds);
    if (rc != 3) {
        return;
    }
    
    knx_set_uint(buffer, 3, 5, hours);
    knx_set_uint(buffer, 10, 6, minutes);
    knx_set_uint(buffer, 18, 6, seconds);
}

static
void
unabstract_dpt11(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc;
    uint32_t year, month, day;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    // ISO8601 Date
    rc = sscanf(str, "%u-%u-%u", &year, &month, &day);
    if (rc != 3) {
        return;
    }
    
    if (year < 1990 || year > 2089)
        return;
    
    year = year % 100;
    
    knx_set_uint(buffer, 3, 5, day);
    knx_set_uint(buffer, 12, 4, month);
    knx_set_uint(buffer, 17, 7, year);
}

static
void
unabstract_dpt13_100(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc, ts, ms;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    ts = parseISO8601Duration_ms(str, &ms);
    switch(dpt.sub) {
        case 2:
            ts = ts * 1000 + ms;
            break;

        case 3:
            ts = ts * 100 + (ms / 10);
            break;

        case 4:
            ts = ts * 10 + (ms / 100);
            break;

        case 5:
            ts = ts * 1;
            break;

        case 6:
            ts = ts / 60;
            break;

        case 7:
            ts = ts / (60 * 60);
            break;
    }
    
    knx_set_int(buffer, 0, 32, ts);
}

static
void
unabstract_dpt14(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t rc;
    float f;

    rc = getObixRealValue(xo, &f, "val");
    if (rc != 0) {
        return;
    }
    
    // FFS : Scale ?
    
    knx_set_float(buffer, 0, f);    
}

static
void
unabstract_dpt19(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    uint32_t year, month, day, hours, minutes, seconds;
    int32_t rc;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }


    rc = sscanf(str, "%u-%u-%uT%u:%u:%u", &year, &month, &day, &hours, &minutes, &seconds);
    if (rc != 6) {
        return;
    }
    
    year -= 1900;
    
    knx_set_uint(buffer, 0, 8, year);
    knx_set_uint(buffer, 12, 4, month);
    knx_set_uint(buffer, 19, 5, day);
    knx_set_uint(buffer, 27, 5, hours);
    knx_set_uint(buffer, 34, 6, minutes);
    knx_set_uint(buffer, 42, 6, seconds);
    
    knx_set_bool(buffer, 50, true);     // No Working Day information available
    knx_set_bool(buffer, 56, false);    // No External Synch of the time
    knx_set_bool(buffer, 53, true);     // Day of week field not valid

}

static
void
unabstract_dpt20(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int32_t i, rc;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    rc = RangeGetBinValue(dpt, &i, str);
    if (rc != 0) {
        return;
    }

    knx_set_uint(buffer, 0, 8, ((uint32_t) i));
}

static
void
unabstract_dpt23(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int i, rc;
    char *str;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    rc = RangeGetBinValue(dpt, &i, str);
    if (rc != 0) {
        return;
    }

    knx_set_uint(buffer, 0, 8, ((uint32_t) i));
}

static
void
unabstract_dpt217(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int rc;
    char *str;
    Dpt_217_t data;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    rc = sscanf(str, "%u.%u.%u", &(data.magicNumber), &(data.versionNumber), &(data.revisionNumber));
    if (rc != 3) {
        return;
    }

    knx_set_uint(buffer, 0, 5, data.magicNumber);
    knx_set_uint(buffer, 5, 5, data.versionNumber);
    knx_set_uint(buffer, 10, 6, data.revisionNumber);
}

static
void
unabstract_dpt229(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int rc;

    char *unit = NULL;
    rc = getObixStrValue(xo, unit, "unit");
    if (rc != 0) {
        return;
    }

    PARSE_INT(8, 32, "val");    // CountVal

    if (strcmp(unit, "obix:units/watt_hour") == 0) {
        knx_set_uint(buffer, 0, 8, 0x03);   // ValInfField
    } else
    if (strcmp(unit, "obix:units/megawatt_hour") == 0) {
        knx_set_uint(buffer, 0, 8, 0x81);
    } else
    if (strcmp(unit, "obix:units/kilojoule") == 0) {
        knx_set_uint(buffer, 0, 8, 0x0B);
    } else
    if (strcmp(unit, "obix:units/x-gigajoule") == 0) {
        knx_set_uint(buffer, 0, 8, 0x89);
    } else
    if (strcmp(unit, "obix:units/liter") == 0) {
        knx_set_uint(buffer, 0, 8, 0x16);
    } else
    if (strcmp(unit, "obix:units/kilogram") == 0) {
        knx_set_uint(buffer, 0, 8, 0x1B);
    } else
    if (strcmp(unit, "obix:units/watt") == 0) {
        knx_set_uint(buffer, 0, 8, 0x2B);
    } else
    if (strcmp(unit, "obix:units/megawatt") == 0) {
        knx_set_uint(buffer, 0, 8, 0xA9);
    } else
    if (strcmp(unit, "obix:units/x-kilojoules_per_hour") == 0) {
        knx_set_uint(buffer, 0, 8, 0x30);
    } else
    if (strcmp(unit, "obix:units/x-gigajoules_per_hour") == 0) {
        knx_set_uint(buffer, 0, 8, 0xB1);
    } else
    if (strcmp(unit, "obix:units/liters_per_hour") == 0) {
        knx_set_uint(buffer, 0, 8, 0x3E);
    } else
    if (strcmp(unit, "obix:units/liters_per_minute") == 0) {
        knx_set_uint(buffer, 0, 8, 0x47);
    } else
    if (strcmp(unit, "obix:units/milliliters_per_second") == 0) {
        knx_set_uint(buffer, 0, 8, 0x4B);
    } else
    if (strcmp(unit, "obix:units/kilograms_per_hour") == 0) {
        knx_set_uint(buffer, 0, 8, 0x53);
    }
    // For other coding, use the raw mode
}

static
void
unabstract_dpt221(Dpt_t dpt, void *xo, uint8_t *buffer)
{
    int rc;
    char *str;
    Dpt_221_t data;

    rc = getObixStrValue(xo, str, "val");
    if (rc != 0) {
        return;
    }

    rc = sscanf(str, "%u.%u", &(data.manufacturerCode), &(data.incrementedCode));
    if (rc != 3) {
        return;
    }

    knx_set_uint(buffer, 0, 32, data.incrementedCode);
    knx_set_uint(buffer, 32, 16, data.manufacturerCode);
}

void
_obix2knx(Dpt_t dpt, void *xo, char *buffer, int *len)
{
    Dpt_Ctx_t dptctx[1];
    int32_t rc;
    char *str;

    *len = 0;

    switch(dpt.primary) {
        case 1:
            PARSE_BOOL(7, "val");
            PARSE_BOOL(7, "wrapper$[name=b].val");
            *len = 1;
            break;

        case 2:
            UNABSTRACT_DPT(2);
            PARSE_BOOL(7, "wrapper$[name=v].val");
            PARSE_BOOL(6, "wrapper$[name=c].val");
            *len = 2;
            break;

        case 3:
            UNABSTRACT_DPT(3);
            PARSE_BOOL(4, "wrapper$[name=b].val");
            PARSE_UINT(5, 3, "wrapper$[name=StepCode].val");
            *len = 4;
            break;

        case 4:
            switch(dpt.sub) {
                case 1:
                    rc = getObixStrValue(xo, str, "val");
                    if (rc == 0) {
                        dptctx->dpt_4.character = do_iconv(str, "ASCII", "UTF-8");
                        if (dptctx->dpt_4.character != NULL) {
                            *len = 8 * (strlen(dptctx->dpt_4.character) + 1);
                            strncpy(buffer, dptctx->dpt_4.character, *len);
                            free(dptctx->dpt_4.character);
                        }
                    }
                    break;

                case 2:
                    rc = getObixStrValue(xo, str, "val");
                    if (rc == 0) {
                        dptctx->dpt_4.character = do_iconv(str, "ISO_8859-1", "UTF-8");
                        if (dptctx->dpt_4.character != NULL) {
                            *len = 8 * (strlen(dptctx->dpt_4.character) + 1);
                            strncpy(buffer, dptctx->dpt_4.character, *len);
                            free(dptctx->dpt_4.character);
                        }
                    }
                    break;

                default:
                    return;
            }
            break;

        case 5:
            UNABSTRACT_DPT(5);
            PARSE_UINT(0, 8, "wrapper$[name=UnsignedValue].val");
            *len = 8;
            break;

        case 6:
            switch(dpt.sub) {
                case 1:
                case 10:
                    // Abstracted value = Raw value
                    PARSE_INT(0, 8, "val");
                    PARSE_INT(0, 8, "wrapper$[name=RelSignedValue].val");
                    *len = 8;
                    break;

                case 2:
                    // No abstracted value
                    PARSE_BOOL(0, "wrapper$[name=a].val");
                    PARSE_BOOL(1, "wrapper$[name=b].val");
                    PARSE_BOOL(2, "wrapper$[name=c].val");
                    PARSE_BOOL(3, "wrapper$[name=d].val");
                    PARSE_BOOL(4, "wrapper$[name=e].val");
                    PARSE_UINT(5, 3, "wrapper$[name=mode].val");
                    *len = 8;

                default:
                    break;
            }
            break;

        case 7:
            switch(dpt.sub) {
                case 1:
                case 10:
                case 11:
                case 12:
                case 13:
                    // Abstracted value = Raw value
                    PARSE_UINT(0, 16, "val");
                    PARSE_UINT(0, 16, "wrapper$[name=UnsignedValue].val");
                    *len = 16;
                    break;

                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    UNABSTRACT_DPT(7);
                    PARSE_UINT(0, 16, "wrapper$[name=TimePeriod].val");
                    *len = 16;

                default:
                    break;
            }
            break;

        case 8:
            switch(dpt.sub) {
                case 1:
                case 10:
                case 11:
                    UNABSTRACT_DPT(8_1);
                    PARSE_INT(0, 16, "wrapper$[name=SignedValue].val");
                    *len = 16;
                    break;

                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    UNABSTRACT_DPT(8_2);
                    PARSE_INT(0, 16, "wrapper$[name=DeltaTime].val");
                    *len = 16;
                    break;

                default:
                    break;
            }
            break;


        case 9:
            UNABSTRACT_DPT(9);
            PARSE_FLOAT16(0, "wrapper$[name=FloatValue].val");
            *len = 16;
            break;

        case 10:
            UNABSTRACT_DPT(10);
            PARSE_UINT(0, 3, "wrapper$[name=Day].val");
            PARSE_UINT(3, 5, "wrapper$[name=Hour].val");
            PARSE_UINT(10, 6, "wrapper$[name=Minutes].val");
            PARSE_UINT(18, 6, "wrapper$[name=Seconds].val");
            *len = 24;
            break;

        case 11:
            UNABSTRACT_DPT(11);
            PARSE_UINT(3, 5, "wrapper$[name=Day].val");
            PARSE_UINT(12, 4, "wrapper$[name=Month].val");
            PARSE_UINT(17, 7, "wrapper$[name=Year].val");
            *len = 24;
            break;

        case 12:
            // Abstracted value = Raw value
            PARSE_UINT(0, 32, "val");
            PARSE_UINT(0, 32, "wrapper$[name=UnsignedValue].val");
            *len = 32;
            break;

        case 13:
            switch(dpt.sub) {
                case 1:
                    // Abstracted value = Raw value
                    break;

                case 100:
                    UNABSTRACT_DPT(13_100);
                    break;

                default:
                    break;
            }
            PARSE_INT(0, 32, "wrapper$[name=SignedValue].val");
            *len = 32;
            break;

        case 14:
            UNABSTRACT_DPT(14);
            PARSE_FLOAT(0, "wrapper$[name=FloatValue].val");
            *len = 32;
            break;


        case 15:
            // No abstracted value
            PARSE_BOOL(24, "wrapper$[name=e].val");
            PARSE_BOOL(25, "wrapper$[name=p].val");
            PARSE_BOOL(26, "wrapper$[name=d].val");
            PARSE_BOOL(27, "wrapper$[name=c].val");
            PARSE_UINT(20, 4, "wrapper$[name=d1].val");
            PARSE_UINT(16, 4, "wrapper$[name=d2].val");
            PARSE_UINT(12, 4, "wrapper$[name=d3].val");
            PARSE_UINT(8, 4, "wrapper$[name=d4].val");
            PARSE_UINT(4, 4, "wrapper$[name=d5].val");
            PARSE_UINT(0, 4, "wrapper$[name=d6].val");
            PARSE_UINT(28, 4, "wrapper$[name=index].val");
            *len = 32;
            break;

        case 16:
            switch(dpt.sub) {
                case 0:
                    rc = getObixStrValue(xo, str, "val");
                    if (rc == 0) {
                        dptctx->dpt_16.character = do_iconv(str, "ASCII", "UTF-8");
                        if (dptctx->dpt_16.character != NULL) {
                            *len = 8 * (strlen(dptctx->dpt_16.character) + 1);
                            strncpy(buffer, dptctx->dpt_16.character, *len);
                            free(dptctx->dpt_16.character);
                        }
                    }
                    break;

                case 1:
                    rc = getObixStrValue(xo, str, "val");
                    if (rc == 0) {
                        dptctx->dpt_16.character = do_iconv(str, "ISO_8859-1", "UTF-8");
                        if (dptctx->dpt_16.character != NULL) {
                            *len = 8 * (strlen(dptctx->dpt_16.character) + 1);
                            strncpy(buffer, dptctx->dpt_16.character, *len);
                            free(dptctx->dpt_16.character);
                        }
                    }
                    break;

                default:
                    return;
            }
            break;

        case 17:
            // Abstracted value = Raw value
            PARSE_UINT(2, 6, "val");
            PARSE_UINT(2, 6, "wrapper$[name=SceneNumber].val");
            *len = 8;
            break;

        case 18:
            // No abstracted value
            PARSE_BOOL(0, "wrapper$[name=c].val");
            PARSE_UINT(2, 6, "wrapper$[name=SceneNumber].val");
            *len = 8;
            break;

        case 19:
            UNABSTRACT_DPT(19);
            PARSE_BOOL(48, "wrapper$[name=F].val");
            PARSE_BOOL(49, "wrapper$[name=WD].val");
            PARSE_BOOL(50, "wrapper$[name=NWD].val");
            PARSE_BOOL(51, "wrapper$[name=NY].val");
            PARSE_BOOL(52, "wrapper$[name=ND].val");
            PARSE_BOOL(53, "wrapper$[name=NDOW].val");
            PARSE_BOOL(54, "wrapper$[name=NT].val");
            PARSE_BOOL(55, "wrapper$[name=SUTI].val");
            PARSE_BOOL(56, "wrapper$[name=CLQ].val");
            PARSE_UINT(0, 8, "wrapper$[name=Year].val");
            PARSE_UINT(12, 4, "wrapper$[name=Month].val");
            PARSE_UINT(19, 5, "wrapper$[name=DayOfMonth].val");
            PARSE_UINT(24, 3, "wrapper$[name=DayOfWeek].val");
            PARSE_UINT(27, 5, "wrapper$[name=HourOfDay].val");
            PARSE_UINT(34, 6, "wrapper$[name=Minutes].val");
            PARSE_UINT(42, 6, "wrapper$[name=Seconds].val");
            *len = 64;
            break;

        case 20:
            UNABSTRACT_DPT(20);
            PARSE_UINT(0, 8, "wrapper$[name=field1].val");
            *len = 8;
            break;

        case 21:
            switch(dpt.sub) {
                case 1:
                    PARSE_BOOL(7, "val");   // Hard to reverse abstract
                    PARSE_BOOL(7, "wrapper$[name=OutOfService].val");
                    PARSE_BOOL(6, "wrapper$[name=Fault].val");
                    PARSE_BOOL(5, "wrapper$[name=Overridden].val");
                    PARSE_BOOL(4, "wrapper$[name=InAlarm].val");
                    PARSE_BOOL(3, "wrapper$[name=AlarmUnAck].val");
                    break;

                case 2:
                    // Abstracted value = Raw value
                    PARSE_BOOL(7, "val");
                    PARSE_BOOL(7, "wrapper$[name=UserStopped].val");
                    PARSE_BOOL(6, "wrapper$[name=OwnIA].val");
                    PARSE_BOOL(5, "wrapper$[name=VerifyMode].val");
                    break;

                default:
                    return;
            }
            *len = 8;
            break;

        case 23:
            UNABSTRACT_DPT(23);
            PARSE_UINT(0, 8, "wrapper$[name=s].val");
            *len = 2;
            break;

        case 24:
            // Latin 1 String
            rc = getObixStrValue(xo, str, "val");
            if (rc == 0) {
                dptctx->dpt_24.character = do_iconv(str, "ISO_8859-1", "UTF-8");
                if (dptctx->dpt_24.character != NULL) {
                    *len = 8 * (strlen(dptctx->dpt_24.character) + 1);
                    strncpy(buffer, dptctx->dpt_24.character, *len);
                    free(dptctx->dpt_24.character);
                }
            }
            break;

        case 26:
            // No abstracted value
            PARSE_BOOL(1, "wrapper$[name=b].val");
            PARSE_UINT(2, 6, "wrapper$[name=SceneNumber].val");
            *len = 8;
            break;

        case 27:
            // No abstracted value
            PARSE_BOOL(0, "wrapper$[name=m15].val");
            PARSE_BOOL(1, "wrapper$[name=m14].val");
            PARSE_BOOL(2, "wrapper$[name=m13].val");
            PARSE_BOOL(3, "wrapper$[name=m12].val");
            PARSE_BOOL(4, "wrapper$[name=m11].val");
            PARSE_BOOL(5, "wrapper$[name=m10].val");
            PARSE_BOOL(6, "wrapper$[name=m9].val");
            PARSE_BOOL(7, "wrapper$[name=m8].val");
            PARSE_BOOL(8, "wrapper$[name=m7].val");
            PARSE_BOOL(9, "wrapper$[name=m6].val");
            PARSE_BOOL(10, "wrapper$[name=m5].val");
            PARSE_BOOL(11, "wrapper$[name=m4].val");
            PARSE_BOOL(12, "wrapper$[name=m3].val");
            PARSE_BOOL(13, "wrapper$[name=m2].val");
            PARSE_BOOL(14, "wrapper$[name=m1].val");
            PARSE_BOOL(15, "wrapper$[name=m0].val");
            PARSE_BOOL(16, "wrapper$[name=s15].val");
            PARSE_BOOL(17, "wrapper$[name=s14].val");
            PARSE_BOOL(18, "wrapper$[name=s13].val");
            PARSE_BOOL(19, "wrapper$[name=s12].val");
            PARSE_BOOL(20, "wrapper$[name=s11].val");
            PARSE_BOOL(21, "wrapper$[name=s10].val");
            PARSE_BOOL(22, "wrapper$[name=s9].val");
            PARSE_BOOL(23, "wrapper$[name=s8].val");
            PARSE_BOOL(24, "wrapper$[name=s7].val");
            PARSE_BOOL(25, "wrapper$[name=s6].val");
            PARSE_BOOL(26, "wrapper$[name=s5].val");
            PARSE_BOOL(27, "wrapper$[name=s4].val");
            PARSE_BOOL(28, "wrapper$[name=s3].val");
            PARSE_BOOL(29, "wrapper$[name=s2].val");
            PARSE_BOOL(30, "wrapper$[name=s1].val");
            PARSE_BOOL(31, "wrapper$[name=s0].val");
            *len = 32;
            break;

        case 28:
            // UTF-8 String
            rc = getObixStrValue(xo, str, "val");
            if (rc == 0) {
                *len = 8 * (strlen(str) + 1);
                strncpy(buffer, str, *len);
            }
            break;

        case 217:
            UNABSTRACT_DPT(217);
            PARSE_UINT(0, 5, "wrapper$[name=magicNumber].val");
            PARSE_UINT(5, 5, "wrapper$[name=versionNumber].val");
            PARSE_UINT(10, 6, "wrapper$[name=revisionNumber].val");
            *len = 16;
            break;

        case 219:
            // No abstracted value
            PARSE_BOOL(39, "wrapper$[name=Ack_Sup].val");
            PARSE_BOOL(38, "wrapper$[name=TS_Sup].val");
            PARSE_BOOL(37, "wrapper$[name=AlarmText_Sup].val");
            PARSE_BOOL(36, "wrapper$[name=ErrorCode_Sup].val");
            PARSE_BOOL(47, "wrapper$[name=InAlarm].val");
            PARSE_BOOL(46, "wrapper$[name=AlarmUnAck].val");
            PARSE_BOOL(45, "wrapper$[name=Locked].val");
            PARSE_UINT(0, 8, "wrapper$[name=LogNumber].val");
            PARSE_UINT(8, 8, "wrapper$[name=AlarmPriority].val");
            PARSE_UINT(16, 8, "wrapper$[name=ApplicationArea].val");
            PARSE_UINT(24, 8, "wrapper$[name=ErrorClass].val");
            *len = 48;
            break;

        case 221:
            UNABSTRACT_DPT(221);
            PARSE_UINT(0, 32, "wrapper$[name=incrementedCode].val");
            PARSE_UINT(32, 16, "wrapper$[name=manufacturerCode].val");
            *len = 48;
            break;

        case 229:
            UNABSTRACT_DPT(229);
            PARSE_INT(8, 32, "wrapper$[name=CountVal].val");
            PARSE_UINT(0, 8, "wrapper$[name=ValInfField].val");
            *len = 40;
            break;
            
        default:
            break;
    }
}


#define HVAC_PARSER(x, y) \
    DPT_SET(dpt2, 21, 1);                        \
    _obix2knx(dpt2, xo, buffer, &len1);          \
    DPT_SET(dpt2, x, y);                         \
    _obix2knx(dpt2, xo, buffer + (len1 / 8), &len2);   \
    *len = len1 + len2;

void
obix2knx(Dpt_t dpt, void *xo, char *buffer, int *len)
{
    Dpt_t dpt2;
    int len1, len2;

    len1 = 0;
    len2 = 0;

    /*
     *  Handle HVAC DPT which need 2 parser call
     */
    switch(dpt.primary) {
        // HVAC DPT need 2 PASS (data + status)
        case 202:
            switch(dpt.sub) {
                case 1:     // 5.004
                case 2:     // 5.010
                    HVAC_PARSER(5, 4);
                    break;

                default:
                    break;
            }
            break;

        case 203:
            switch(dpt.sub) {
                case 2:
                    HVAC_PARSER(7, 2);
                    break;

                case 3:
                    HVAC_PARSER(7, 3);
                    break;

                case 4:
                    HVAC_PARSER(7, 4);
                    break;

                case 5:
                    HVAC_PARSER(7, 5);
                    break;

                case 6:
                    HVAC_PARSER(7, 6);
                    break;

                case 7:
                    HVAC_PARSER(7, 7);
                    break;

                case 11:
                case 13:
                    HVAC_PARSER(14, 77);
                    break;

                case 12:
                    HVAC_PARSER(7, 1);
                    break;

                case 14:
                    HVAC_PARSER(9, 24);
                    break;

                case 15:
                    HVAC_PARSER(9, 6);
                    break;

                case 17:
                    HVAC_PARSER(5, 1);
                    break;

                default:
                    break;
            }
            break;

        case 204:
            switch(dpt.sub) {
                case 1:
                    HVAC_PARSER(6, 1);
                    break;

                default:
                    break;
            }
            break;

        case 205:
            switch(dpt.sub) {
                case 2:
                    HVAC_PARSER(8, 2);
                    break;

                case 3:
                    HVAC_PARSER(8, 3);
                    break;

                case 4:
                    HVAC_PARSER(8, 4);
                    break;

                case 5:
                    HVAC_PARSER(8, 5);
                    break;

                case 6:
                    HVAC_PARSER(8, 6);
                    break;

                case 7:
                    HVAC_PARSER(8, 7);
                    break;

                default:
                    break;
            }
            break;

        case 218:
            switch(dpt.sub) {
                case 1:
                    HVAC_PARSER(14, 76);
                    break;

                default:
                    break;
            }
            break;
            
        case 229:
            switch(dpt.sub) {
                case 1:
                    HVAC_PARSER(229, 1);
                    break;

                default:
                    break;
            }
            break;
            
        // STD DPT need only one PASS (data)
        default:
            _obix2knx(dpt, xo, buffer, len);
    }
}

