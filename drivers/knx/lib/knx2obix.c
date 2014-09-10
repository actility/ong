#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <iconv.h>
#include <errno.h>

#include "knx.h"

static
void
setRawIntoTemplate(char *buffer, int sz)
{
    int i;
    char *raw, *praw;
    praw = raw = malloc (2 * sz + 1);
    assert(raw);

    for(i=0; i<sz; i++) {
        sprintf(praw, "%02hhX", buffer[i]);
        praw += 2;
    }
    SetVarIntoTemplate("dpt_raw", raw);
    free(raw);
}

static
void
abstract_dpt3(Dpt_t dpt, Dpt_3_t *data)
{
    float f[8] = {0.0f, 50.0f, 25.0f, 12.5f, 6.25f, 3.125f, 1.5625f, 0.78125f};
    if (data->StepCode > 7) {
        SetVarIntoTemplate("dpt_value", "ERROR");
    } else {
        if (data->b == true) {
            RenderVarIntoTemplate("dpt_value", "%f", f[data->StepCode]);
        } else {
            RenderVarIntoTemplate("dpt_value", "%f", -1.0f * f[data->StepCode]);
        }
    }
}

static
void
abstract_dpt5(Dpt_t dpt, Dpt_5_t *data)
{
    float scaleFactor;

    switch(dpt.sub) {
        case 1:
            scaleFactor = 100.0f / 256.0f;
            break;

        case 2:
            scaleFactor = 360.0f / 256.0f;
            break;

        default:
            scaleFactor = 1.0f;
    }

    RenderVarIntoTemplate("dpt_value", "%f", (float)(data->UnsignedValue) * scaleFactor);
}

static
void
abstract_dpt7_2(Dpt_t dpt, Dpt_7_t *data)
{
    uint32_t s = 0, ms = 0;

    switch(dpt.sub) {
        case 2:
            s = data->TimePeriod / 1000;
            ms = data->TimePeriod % 1000;
            break;

        case 3:
            s = data->TimePeriod / 100;
            ms = data->TimePeriod % 100;
            break;

        case 4:
            s = data->TimePeriod / 10;
            ms = data->TimePeriod % 10;
            break;

        case 5:
            s = data->TimePeriod;
            ms = 0;
            break;

        case 6:
            s = data->TimePeriod * 60;
            ms = 0;
            break;

        case 7:
            s = data->TimePeriod * 60 * 60;
            ms = 0;
            break;
    }

    SetVarAsISO8601Duration("dpt_value", s, ms);
}

static
void
abstract_dpt8_1(Dpt_t dpt, Dpt_8_t *data)
{
    float scaleFactor;

    switch(dpt.sub) {
        case 10:
            scaleFactor = 1.0f / 100.0f;
            break;

        default:
            scaleFactor = 1.0f;
    }

    RenderVarIntoTemplate("dpt_value", "%f", (float)(data->SignedValue) * scaleFactor);
}

static
void
abstract_dpt8_2(Dpt_t dpt, Dpt_8_t *data)
{
    int32_t s = 0, ms = 0;

    switch(dpt.sub) {
        case 2:
            s = data->DeltaTime / 1000;
            ms = data->DeltaTime - s;
            break;

        case 3:
            s = data->DeltaTime / 100;
            ms = data->DeltaTime - s;
            break;

        case 4:
            s = data->DeltaTime / 10;
            ms = data->DeltaTime - s;
            break;

        case 5:
            s = data->DeltaTime;
            ms = 0;
            break;

        case 6:
            s = data->DeltaTime * 60;
            ms = 0;
            break;

        case 7:
            s = data->DeltaTime * 60 * 60;
            ms = 0;
            break;
    }

    SetVarAsISO8601Duration("dpt_value", s, ms);
}

static
void
abstract_dpt9(Dpt_t dpt, Dpt_9_t *data)
{
    // FFS: Scale based on Obix units
    RenderVarIntoTemplate("dpt_value", "%f", data->FloatValue);
}

static
void
abstract_dpt10(Dpt_t dpt, Dpt_10_t *data)
{
    // ISO8601: Time
    RenderVarIntoTemplate("dpt_value", "T%d:%d:%d", data->Hour, data->Minutes, data->Seconds);
}

static
void
abstract_dpt11(Dpt_t dpt, Dpt_11_t *data)
{
    if (data->Year >= 90) {
        data->Year += 1900;
    } else {
        data->Year += 2000;
    }

    // ISO8601: Date
    RenderVarIntoTemplate("dpt_value", "%d-%d-%d", data->Year, data->Month, data->Day);
}

static
void
abstract_dpt13_100(Dpt_t dpt, Dpt_13_t *data)
{
   SetVarAsISO8601Duration("dpt_value", data->SignedValue, 0);
}

static
void
abstract_dpt19(Dpt_t dpt, Dpt_19_t *data)
{
    // ISO8601: DateTime format compact
    RenderVarIntoTemplate("dpt_value", "%04u%02u%02uT%02u%02u%02uZ",
        data->Year,
        data->Month,
        data->DayOfMonth,
        data->HourOfDay,
        data->Minutes,
        data->Seconds
    );
}

static
void
abstract_dpt229(Dpt_t dpt, Dpt_229_t *data)
{
    // Decode ValInfField for unit and range

    if ((data->ValInfField & 0xF8) == 0x00) {
        SetVar("dpt_unit", "obix:units/watt_hour");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 3 + 0));
                                                                                    // coding range ----^   ^---- unit offset (knx|obix)  
    } else
    if ((data->ValInfField & 0xFE) == 0x80) {
        SetVar("dpt_unit", "obix:units/megawatt_hour");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x01) + 5 - 6));
    } else
    if ((data->ValInfField & 0xF8) == 0x08) {
        SetVar("dpt_unit", "obix:units/kilojoule");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) + 0 - 3));
    } else
    if ((data->ValInfField & 0xFE) == 0x88) {
        SetVar("dpt_unit", "obix:units/x-gigajoule");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x01) + 8 - 9));
    } else
    if ((data->ValInfField & 0xF8) == 0x10) {
        SetVar("dpt_unit", "obix:units/liter");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 6 - 3));
    } else
    if ((data->ValInfField & 0xF8) == 0x18) {
        SetVar("dpt_unit", "obix:units/kilogram");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 3 + 0));
    } else
    if ((data->ValInfField & 0xF8) == 0x28) {
        SetVar("dpt_unit", "obix:units/watt");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 3 + 0));
    } else
    if ((data->ValInfField & 0xFE) == 0xA8) {
        SetVar("dpt_unit", "obix:units/megawatt");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x01) + 5 - 6));
    } else
    if ((data->ValInfField & 0xF8) == 0x30) {
        SetVar("dpt_unit", "obix:units/x-kilojoules_per_hour");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) + 0 - 3));
    } else
    if ((data->ValInfField & 0xFE) == 0xB0) {
        SetVar("dpt_unit", "obix:units/x-gigajoules_per_hour");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x01) + 8 - 9));
    } else
    if ((data->ValInfField & 0xF8) == 0x38) {
        SetVar("dpt_unit", "obix:units/liters_per_hour");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 6 - 3));
    } else
    if ((data->ValInfField & 0xF8) == 0x40) {
        SetVar("dpt_unit", "obix:units/liters_per_minute");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 7 - 4));
    } else
    if ((data->ValInfField & 0xF8) == 0x41) {
        SetVar("dpt_unit", "obix:units/milliliters_per_second");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 9 + 3));
    } else
    if ((data->ValInfField & 0xF8) == 0x50) {
        SetVar("dpt_unit", "obix:units/kilograms_per_hour");
        RenderVarIntoTemplate("dpt_value", "%f", data->CountVal * powf(10, (data->ValInfField & 0x07) - 3 - 3));
    } else
    if (data->ValInfField == 0x6E) {
        SetVar("dpt_unit", "obix:units/null");
        RenderVarIntoTemplate("dpt_value", "%d", data->CountVal);
    } else {
        SetVar("dpt_unit", "obix:units/null");
        SetVar("dpt_value", "NaN");
    }
}

static
void
abstract_enum(Dpt_t dpt, int i)
{
	char *abstractEnum = NULL;

	RangeGetRenderedValue(dpt, i, &abstractEnum);
    SetVarIntoTemplate("dpt_value", abstractEnum);
}

#define PARSE_BOOL(dptid, field, bit) \
    dptctx->dpt_ ## dptid.field = knx_parse_bool((uint8_t *)buffer, bit); \
    SetBoolIntoTemplate("raw_" #field, dptctx->dpt_ ## dptid.field);

#define PARSE_UINT(dptid, field, bit, len) \
    dptctx->dpt_ ## dptid.field = knx_parse_uint((uint8_t *)buffer, bit, len); \
    RenderVarIntoTemplate("raw_" #field, "%u", dptctx->dpt_ ## dptid.field);

#define PARSE_INT(dptid, field, bit, len) \
    dptctx->dpt_ ## dptid.field = knx_parse_int((uint8_t *)buffer, bit, len); \
    RenderVarIntoTemplate("raw_" #field, "%d", dptctx->dpt_ ## dptid.field);

#define ABSTRACT_ENUM(i) \
    abstract_enum(dpt, (i));

#define ABSTRACT_DPT(i) \
    abstract_dpt ## i (dpt, (Dpt_ ## i ## _t *) dptctx)

void
_knx2obix(Dpt_t dpt, char *buffer, void **xo, int *len, bool dpt21001parseOnly)
{
    Dpt_Ctx_t dptctx[1];
    int dptRawSize = 0;

    // On error, we return NULL for the template to used.
    *xo = NULL;

    /*
     *  Parse buffer and create Xo object
     */
    switch(dpt.primary) {
        case 1:
            PARSE_BOOL(1, b, 7);
            // No abstract needed
            *xo = "dat_1.xml";
            dptRawSize = 1;
            break;

        case 2:
            PARSE_BOOL(2, v, 7);
            PARSE_BOOL(2, c, 6);
            ABSTRACT_ENUM(1 * dptctx->dpt_2.v + 2 * dptctx->dpt_2.c);
            *xo = "dat_2.xml";
            dptRawSize = 1;
            break;

        case 3:
            PARSE_BOOL(3, b, 4);
            PARSE_UINT(3, StepCode, 5, 3);
            ABSTRACT_DPT(3);
            *xo = "dat_3.xml";
            dptRawSize = 1;
            break;

        case 4:
            switch(dpt.sub) {
                case 1:
                    // Pur ASCII, act with the UTF-8 template
                    SetVar("dpt_value", buffer);
                    *xo = "dat_28.xml";
                    dptRawSize = strlen(buffer);
                    break;

                case 2:
                    dptctx->dpt_4.character = do_iconv(buffer, "UTF-8", "ISO_8859-1");
                    if (dptctx->dpt_4.character != NULL) {
                        SetVarIntoTemplate("dpt_value", dptctx->dpt_4.character);
                        free(dptctx->dpt_4.character);
                    }
                    *xo = "dat_4.xml";
                    dptRawSize = strlen(buffer);
                    break;

                default:
                    return;
            }
            break;

        case 5:
            PARSE_UINT(5, UnsignedValue, 0, 8);
            ABSTRACT_DPT(5);
            *xo = "dat_5.xml";
            dptRawSize = 1;
            break;

        case 6:
            switch(dpt.sub) {
                case 1:
                case 10:
                    PARSE_INT(6, RelSignedValue, 0, 8);
                    // Abstracted value = Raw value
                    *xo = "dat_6_1.xml";
                    dptRawSize = 1;
                    break;

                default:
                    PARSE_BOOL(6, a, 0);
                    PARSE_BOOL(6, b, 1);
                    PARSE_BOOL(6, c, 2);
                    PARSE_BOOL(6, d, 3);
                    PARSE_BOOL(6, e, 4);
                    PARSE_UINT(6, mode, 5, 3);
                    // No abstract
                    *xo = "dat_6_20.xml";
                    dptRawSize = 1;
            }
            break;

        case 7:
            switch(dpt.sub) {
                case 1:
                case 10:
                case 11:
                case 12:
                case 13:
                    PARSE_UINT(7, UnsignedValue, 0, 16);
                    // Abstracted value = Raw value
                    *xo = "dat_7_1.xml";
                    dptRawSize = 2;
                    break;

                default:
                    PARSE_UINT(7, TimePeriod, 0, 16);
                    abstract_dpt7_2(dpt, &(dptctx->dpt_7));
                    *xo = "dat_7_2.xml";
                    dptRawSize = 2;
            }
            break;

        case 8:
            switch(dpt.sub) {
                case 1:
                case 10:
                case 11:
                    PARSE_INT(8, SignedValue, 0, 16);
                    abstract_dpt8_1(dpt, &(dptctx->dpt_8));
                    *xo = "dat_8_1.xml";
                    dptRawSize = 2;
                    break;

                default:
                    PARSE_INT(8, DeltaTime, 0, 16);
                    abstract_dpt8_2(dpt, &(dptctx->dpt_8));
                    *xo = "dat_8_2.xml";
                    dptRawSize = 2;
            }
            break;

        case 9:
            dptctx->dpt_9.FloatValue = knx_parse_float16(buffer, 0);
            RenderVarIntoTemplate("raw_FloatValue", "%f", dptctx->dpt_9.FloatValue);
            ABSTRACT_DPT(9);
            *xo = "dat_9.xml";
            dptRawSize = 2;
            break;

        case 10:
            PARSE_UINT(10, Day, 0, 3);
            PARSE_UINT(10, Hour, 3, 5);
            PARSE_UINT(10, Minutes, 10, 6);
            PARSE_UINT(10, Seconds, 18, 6);
            ABSTRACT_DPT(10);
            *xo = "dat_10.xml";
            dptRawSize = 3;
            break;

        case 11:
            PARSE_UINT(11, Day, 3, 5);
            PARSE_UINT(11, Month, 12, 4);
            PARSE_UINT(11, Year, 17, 7);
            ABSTRACT_DPT(11);
            *xo = "dat_11.xml";
            dptRawSize = 3;
            break;

        case 12:
            PARSE_UINT(12, UnsignedValue, 0, 32);
            // Abstracted value = Raw value
            *xo = "dat_12.xml";
            dptRawSize = 4;
            break;

        case 13:
            PARSE_INT(13, SignedValue, 0, 32);
            dptRawSize = 4;
            switch(dpt.sub) {
                case 1:
                    // Abstracted value = Raw value
                    *xo = "dat_13_1.xml";
                    break;

                default:
                    abstract_dpt13_100(dpt, &(dptctx->dpt_13));
                    *xo = "dat_13_100.xml";
            }
            break;

        case 14:
            dptctx->dpt_14.FloatValue = knx_parse_float(buffer, 0);
            RenderVarIntoTemplate("raw_FloatValue", "%f", dptctx->dpt_14.FloatValue);
            // Abstracted value = Raw value
            *xo = "dat_14.xml";
            dptRawSize = 4;
            break;

        case 15:
            PARSE_BOOL(15, e, 24);
            PARSE_BOOL(15, p, 25);
            PARSE_BOOL(15, d, 26);
            PARSE_BOOL(15, c, 27);
            PARSE_UINT(15, d1, 20, 4);
            PARSE_UINT(15, d2, 16, 4);
            PARSE_UINT(15, d3, 12, 4);
            PARSE_UINT(15, d4, 8, 4);
            PARSE_UINT(15, d5, 4, 4);
            PARSE_UINT(15, d6, 0, 4);
            PARSE_UINT(15, index, 28, 4);
            // No abstract
            *xo = "dat_15.xml";
            dptRawSize = 3;
            break;

        case 16:
            switch(dpt.sub) {
                case 0:
                    // Pur ASCII, act with the UTF-8 template
                    *xo = "dat_28.xml";
                    dptRawSize = strlen(buffer);
                    break;

                case 1:
                    // ISO_8859_1
                    dptctx->dpt_4.character = do_iconv(buffer, "UTF-8", "ISO_8859-1");
                    if (dptctx->dpt_4.character != NULL) {
                        SetVarIntoTemplate("dpt_value", dptctx->dpt_4.character);
                        free(dptctx->dpt_4.character);
                    }
                    *xo = "dat_4.xml";
                    dptRawSize = strlen(buffer);
                    break;

                default:
                    return;
            }
            break;

        case 17:
            PARSE_UINT(17, SceneNumber, 2, 6);
            // No abstract
            *xo = "dat_17.xml";
            dptRawSize = 1;
            break;

        case 18:
            PARSE_BOOL(18, c, 0);
            PARSE_UINT(17, SceneNumber, 2, 6);
            // No abstract
            *xo = "dat_18.xml";
            dptRawSize = 1;
            break;

        case 19:
            PARSE_BOOL(19, F, 48);
            PARSE_BOOL(19, WD, 49);
            PARSE_BOOL(19, NWD, 50);
            PARSE_BOOL(19, NY, 51);
            PARSE_BOOL(19, ND, 52);
            PARSE_BOOL(19, NDOW, 53);
            PARSE_BOOL(19, NT, 54);
            PARSE_BOOL(19, SUTI, 55);
            PARSE_BOOL(19, CLQ, 56);
            PARSE_UINT(19, Year, 0, 8);
            PARSE_UINT(19, Month, 12, 4);
            PARSE_UINT(19, DayOfMonth, 19, 5);
            PARSE_UINT(19, DayOfWeek, 24, 3);
            PARSE_UINT(19, HourOfDay, 27, 5);
            PARSE_UINT(19, Minutes, 34, 6);
            PARSE_UINT(19, Seconds, 42, 6);
            ABSTRACT_DPT(19);
            *xo = "dat_19.xml";
            dptRawSize = 8;
            break;

        case 20:
            PARSE_UINT(20, field1, 0, 8);
            ABSTRACT_ENUM(dptctx->dpt_20.field1);
            *xo = "dat_20.xml";
            dptRawSize = 1;
            break;

        case 21:
            dptRawSize = 1;
            switch(dpt.sub) {
                case 1:
                    PARSE_BOOL(21, OutOfService, 7);
                    PARSE_BOOL(21, Fault, 6);
                    PARSE_BOOL(21, Overridden, 5);
                    PARSE_BOOL(21, InAlarm, 4);
                    PARSE_BOOL(21, AlarmUnAck, 3);
                    if (dpt21001parseOnly == false) {
                        SetBoolIntoTemplate("dpt_value", dptctx->dpt_21.OutOfService | dptctx->dpt_21.Fault | dptctx->dpt_21.Overridden | dptctx->dpt_21.InAlarm | dptctx->dpt_21.AlarmUnAck);
                        *xo = "dat_21_1.xml";
                    }
                    break;

                case 2:
                    PARSE_BOOL(21, UserStopped, 7);
                    PARSE_BOOL(21, OwnIA, 6);
                    PARSE_BOOL(21, VerifyMode, 5);
                    // Abstracted value = Raw value
                    *xo = "dat_21_2.xml";
                    break;

                default:
                    return;
            }
            break;

        case 23:
            PARSE_UINT(23, s, 6, 2);
            ABSTRACT_ENUM(dptctx->dpt_23.s);
            *xo = "dat_23.xml";
            dptRawSize = 1;
            break;

        case 24:
            dptctx->dpt_4.character = do_iconv(buffer, "UTF-8", "ISO_8859-1");
            if (dptctx->dpt_4.character != NULL) {
                SetVarIntoTemplate("dpt_value", dptctx->dpt_4.character);
                free(dptctx->dpt_4.character);
            }
            *xo = "dat_4.xml";
            dptRawSize = strlen(buffer);
            break;

        case 26:
            PARSE_UINT(26, SceneNumber, 2, 6);
            PARSE_BOOL(26, b, 1);
            // No abstract
            *xo = "dat_26.xml";
            dptRawSize = 1;
            break;

        case 27:
            PARSE_BOOL(27, m15, 0);
            PARSE_BOOL(27, m14, 1);
            PARSE_BOOL(27, m13, 2);
            PARSE_BOOL(27, m12, 3);
            PARSE_BOOL(27, m11, 4);
            PARSE_BOOL(27, m10, 5);
            PARSE_BOOL(27, m9, 6);
            PARSE_BOOL(27, m8, 7);
            PARSE_BOOL(27, m7, 8);
            PARSE_BOOL(27, m6, 9);
            PARSE_BOOL(27, m5, 10);
            PARSE_BOOL(27, m4, 11);
            PARSE_BOOL(27, m3, 12);
            PARSE_BOOL(27, m2, 13);
            PARSE_BOOL(27, m1, 14);
            PARSE_BOOL(27, m0, 15);
            PARSE_BOOL(27, s15, 16);
            PARSE_BOOL(27, s14, 17);
            PARSE_BOOL(27, s13, 18);
            PARSE_BOOL(27, s12, 19);
            PARSE_BOOL(27, s11, 20);
            PARSE_BOOL(27, s10, 21);
            PARSE_BOOL(27, s9, 22);
            PARSE_BOOL(27, s8, 23);
            PARSE_BOOL(27, s7, 24);
            PARSE_BOOL(27, s6, 25);
            PARSE_BOOL(27, s5, 26);
            PARSE_BOOL(27, s4, 27);
            PARSE_BOOL(27, s3, 28);
            PARSE_BOOL(27, s2, 29);
            PARSE_BOOL(27, s1, 30);
            PARSE_BOOL(27, s0, 31);
            // No abstract
            *xo = "dat_27.xml";
            dptRawSize = 4;
            break;

        case 28:
            SetVar("dpt_value", buffer);
            *xo = "dat_28.xml";
            dptRawSize = strlen(buffer);    // Real bytes size, not the caracters count
            break;

        case 217:
            PARSE_UINT(217, magicNumber, 0, 5);
            PARSE_UINT(217, versionNumber, 5, 5);
            PARSE_UINT(217, revisionNumber, 10, 6);
            RenderVarIntoTemplate("dpt_value", "%u.%u.%u", dptctx->dpt_217.magicNumber, dptctx->dpt_217.versionNumber, dptctx->dpt_217.revisionNumber);
            *xo = "dat_217.xml";
            dptRawSize = 2;
            break;

        case 219:
            PARSE_BOOL(219, Ack_Sup, 39);
            PARSE_BOOL(219, TS_Sup, 38);
            PARSE_BOOL(219, AlarmText_Sup, 37);
            PARSE_BOOL(219, ErrorCode_Sup, 36);
            PARSE_BOOL(219, InAlarm, 47);
            PARSE_BOOL(219, AlarmUnAck, 46);
            PARSE_BOOL(219, Locked, 45);
            PARSE_UINT(219, LogNumber, 0, 8);
            PARSE_UINT(219, AlarmPriority, 8, 8);
            PARSE_UINT(219, ApplicationArea, 16, 8);
            PARSE_UINT(219, ErrorClass, 24, 8);
            // No abstract
            *xo = "dat_219.xml";
            dptRawSize = 6;
            break;

        case 221:
            PARSE_UINT(221, incrementedCode, 0, 32);
            PARSE_UINT(221, manufacturerCode, 32, 16);
            RenderVarIntoTemplate("dpt_value", "%u.%u", dptctx->dpt_221.manufacturerCode, dptctx->dpt_221.incrementedCode);
            *xo = "dat_221.xml";
            dptRawSize = 6;
            break;

        case 229:
            PARSE_INT(229, CountVal, 8, 32);
            PARSE_UINT(229, ValInfField, 0, 8);
            //PARSE_INT(229, CountVal, 0, 32);
            //PARSE_UINT(229, ValInfField, 32, 8);
            ABSTRACT_DPT(229);
            *xo = "dat_229.xml";
            dptRawSize = 5;
            break;
            
        default:
            return;
    }

    *len = dptRawSize;
}

#define HVAC_PARSER(x, y, tpl)                          \
    DPT_SET(dpt2, 21, 1);                               \
    _knx2obix(dpt2, buffer, xo, &len1, true);           \
    DPT_SET(dpt2, x, y);                                \
    _knx2obix(dpt2, buffer + len1, xo, &len2, false);   \
    *xo = tpl;

void
knx2obix(Dpt_t dpt, char *buffer, void **xo)
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
                    HVAC_PARSER(5, 4, "dat_202_1.xml");
                    break;

                default:
                    break;
            }
            break;

        case 203:
            switch(dpt.sub) {
                case 2:
                    HVAC_PARSER(7, 2, "dat_203_2.xml");
                    break;

                case 3:
                    HVAC_PARSER(7, 3, "dat_203_2.xml");
                    break;

                case 4:
                    HVAC_PARSER(7, 4, "dat_203_2.xml");
                    break;

                case 5:
                    HVAC_PARSER(7, 5, "dat_203_2.xml");
                    break;

                case 6:
                    HVAC_PARSER(7, 6, "dat_203_2.xml");
                    break;

                case 7:
                    HVAC_PARSER(7, 7, "dat_203_2.xml");
                    break;

                case 11:
                case 13:
                    HVAC_PARSER(14, 77, "dat_203_11.xml");
                    break;

                case 12:
                    HVAC_PARSER(7, 1, "dat_203_12.xml");
                    break;

                case 14:
                    HVAC_PARSER(9, 24, "dat_203_14.xml");
                    break;

                case 15:
                    HVAC_PARSER(9, 6, "dat_203_14.xml");
                    break;

                case 17:
                    HVAC_PARSER(5, 1, "dat_203_17.xml");
                    break;

                default:
                    break;
            }
            break;

        case 204:
            switch(dpt.sub) {
                case 1:
                    HVAC_PARSER(6, 1, "dat_204_1.xml");
                    break;

                default:
                    break;
            }
            break;

        case 205:
            switch(dpt.sub) {
                case 2:
                    HVAC_PARSER(8, 2, "dat_205_2.xml");
                    break;

                case 3:
                    HVAC_PARSER(8, 3, "dat_205_2.xml");
                    break;

                case 4:
                    HVAC_PARSER(8, 4, "dat_205_2.xml");
                    break;

                case 5:
                    HVAC_PARSER(8, 5, "dat_205_2.xml");
                    break;

                case 6:
                    HVAC_PARSER(8, 6, "dat_205_2.xml");
                    break;

                case 7:
                    HVAC_PARSER(8, 7, "dat_205_2.xml");
                    break;

                default:
                    break;
            }
            break;

        case 218:
            switch(dpt.sub) {
                case 1:
                    HVAC_PARSER(14, 76, "dat_218_1.xml");
                    break;

                default:
                    break;
            }
            break;

        case 229:
            switch(dpt.sub) {
                case 1:
                    HVAC_PARSER(229, 1, "dat_229.xml");
                    break;

                default:
                    break;
            }
            break;
            
        // STD DPT need only one PASS (data)
        default:
            _knx2obix(dpt, buffer, xo, &len1, false);
    }

    setRawIntoTemplate(buffer, len1 + len2);
}

 
