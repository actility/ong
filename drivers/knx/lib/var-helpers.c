#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "knx.h"

/**
 * @brief Set a variable into the template engine
 * @param key Variable name
 * @param value Variable content, a empty string if the value is NULL
 */
void
SetVarIntoTemplate(char *key, char *value)
{
    if (value == NULL) {
        SetVar(key, "");
    } else {
        SetVar(key, value);
    }
}

/**
 * @brief Set a boolean variable into the template engine
 * @param key Variable name
 * @param value Variable content, true / false
 */
void
SetBoolIntoTemplate(char *key, bool b)
{
    if (b == true) {
        SetVar(key, "true");
    } else {
        SetVar(key, "false");
    }
}

/**
 * @brief Render a variable and set it into the template engine
 * @param key Variable name
 * @param fmt Format string, like printf
 */
void
RenderVarIntoTemplate(char *key, char *fmt, ...)
{
    va_list ap;
    int ret;
    char buffer[1024];

    va_start(ap, fmt);
    ret = vsnprintf(buffer, 1024, fmt, ap);
    va_end(ap);
    
    if (ret < 0) {
        SetVar(key, "ERROR");
    } else { 
        SetVar(key, buffer);
    }
}

/**
 * @brief Render a ISO8601 duration variable and set it into the template engine
 * @param key Variable name
 * @param s Unix timestamp
 * @param ms Number of milli-seconds
 */
void
SetVarAsISO8601Duration(char *key, int32_t s, int32_t ms)
{
    int32_t sign = (s >= 0) ? 0 : 1;
    int32_t tmp;
    int32_t Day=0, Hour=0, Minute=0, Second=0;
    
    if (s == 0 && ms != 0) {
        RenderVarIntoTemplate(key, "%.*sP0S,%03d",
            (ms > 0) ? 0 : 1,
            "-",
            (ms > 0) ? ms : -1 * ms
        );
        
        return;
    }
    
    if (sign) {
        s = -1 * s;
    }

    tmp = s / (24 * 60 * 60);
    if (tmp > 0) {
        Day = tmp;
        s -= (24 * 60 * 60) * Day;
    }

    tmp = s / (60 * 60);
    if (tmp > 0) {
        Hour = tmp;
        s -= (60 * 60) * Hour;
    }
        
    tmp = s / 60;
    if (tmp > 0) {
        Minute = tmp;
        s -= 60 * Minute;
    }
    
    Second = s;
    
    RenderVarIntoTemplate(key, "%.*sP" "%.*d%0.*s" "%0.*d%0.*s" "%0.*d%0.*s" "%0.*d%0.*s" "%.*s%.*d",
        sign,
        "-",

        (Day != 0) ? 6 : 0,
        Day,
        (Day != 0) ? 1 : 0,
        "D",
        
        (Hour != 0) ? 2 : 0,
        Hour,
        (Hour != 0) ? 1 : 0,
        "H",
        
        (Minute != 0) ? 2 : 0,
        Minute,
        (Minute != 0) ? 1 : 0,
        "M",
        
        (Second != 0) ? 2 : 0,
        Second,
        (Second != 0) ? 1 : 0,
        "S",
        
        (ms > 0) ? 1 : 0,
        ",",
        (ms > 0) ? 3 : 0,
        ms
    );
}
