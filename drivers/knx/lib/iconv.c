#include <stdlib.h>
#include <assert.h>
#include <iconv.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/**
 * @brief Convert a string from a encoding to another
 * @param input The string to convert from localeIn encoding to localeOut
 * @param localeIn Locale of the input string
 * @param localeOut Locale of the output string
 * @return The string converted, or NULL on error
 *         The output must be free
 */
char *
do_iconv(char *input, char *localeOut, char *localeIn)
{
    iconv_t cd;
    size_t nconv, insize, outsize;
    char *output, *outputInternal;

    cd = iconv_open (localeOut, localeIn);
    if (cd == (iconv_t) -1) {
        perror ("iconv_open");
        return NULL;
    }

    // FFS : Better memory allocation
    insize = strlen(input);
    outsize = 4 * insize;
    output = outputInternal = malloc (outsize);
    assert(outputInternal);

    nconv = iconv (cd, &input, &insize, &outputInternal, &outsize);
    if (nconv == (size_t) -1) {
        perror ("iconv");
        free(output), output = NULL;
    } else {
        *outputInternal = '\0';
    }

    if (iconv_close (cd) != 0) {
        perror ("iconv_close");
    }
    
    return output;
}
