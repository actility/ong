#ifndef __KNX_XO_HELPERS__
#define __KNX_XO_HELPERS__

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "xo.h"

/*
 *  Xo don't expose varadic interface (like vprintf)
 *  So helper must be write as macro to forward varadic parameters
 *
 *  Xo support only "%s", "%d" without any feature
 *  Use a snprintf before if you need more complex formating
 */


#define getObixBoolValue(obix, value, path, args...)    \
({                                                      \
    int32_t rc = -1;                                    \
    void *obj;                                          \
                                                        \
    obj = XoNmGetAttr(obix, path, NULL, ##args);        \
    if (obj != NULL) {                                  \
        rc = 0;                                         \
        if (0 == strcmp("true", (char *)obj)) {         \
            *value = true;                              \
        } else {                                        \
            *value = false;                             \
        }                                               \
    }                                                   \
    rc;                                                 \
})

/*
 *  Returned pointer don't need to be free
 */
#define getObixStrValue(obix, value, path, args...)     \
({                                                      \
    int32_t rc = -1;                                    \
    void *obj;                                          \
                                                        \
    obj = XoNmGetAttr(obix, path, NULL, ##args);        \
    if (obj != NULL) {                                  \
        rc = 0;                                         \
        value = obj;                                    \
    }                                                   \
    rc;                                                 \
})

#define getObixIntValue(obix, value, path, args...)     \
({                                                      \
    int32_t rc = -1;                                    \
    void *obj;                                          \
                                                        \
    obj = XoNmGetAttr(obix, path, NULL, ##args);        \
    if (obj != NULL) {                                  \
        rc = 0;                                         \
        *value = atoi(obj);                             \
    }                                                   \
    rc;                                                 \
})

#define getObixUintValue(obix, value, path, args...)    \
({                                                      \
    int32_t rc = -1;                                    \
    void *obj;                                          \
                                                        \
    obj = XoNmGetAttr(obix, path, NULL, ##args);        \
    if (obj != NULL) {                                  \
        rc = 0;                                         \
        *value = (uint32_t) strtoul(obj, NULL, 10);     \
    }                                                   \
    rc;                                                 \
})

#define getObixRealValue(obix, value, path, args...)    \
({                                                      \
    int32_t rc = -1;                                    \
    void *obj;                                          \
                                                        \
    obj = XoNmGetAttr(obix, path, NULL, ##args);        \
    if (obj != NULL) {                                  \
        rc = 0;                                         \
        *value = atof(obj);                             \
    }                                                   \
    rc;                                                 \
})

#endif /* __KNX_XO_HELPERS__ */
