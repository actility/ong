#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <poll.h>

#include "rtlbase.h"

#include "common.h"
#include "header_t.h"

static void header_t_init(header_t *This);

/**
 * Initialize the data structure of the object-like.
 * @param This the object-like instance.
 */
static void header_t_init(header_t *This)
{
  LOG(TRACE_DEBUG, "header_t::init");

  /* Assign the pointers on functions */
  This->addValue = header_t_addValue;
  /* Initialize the members */
  This->name = NULL;
  This->values = NULL;
}

/**
 * Dynamic allocation of the object-like instance
 * @param name the name of the header.
 * @return the created object-like instance.
 */
header_t *new_header_t(char * name)
{
  header_t *This = malloc(sizeof(header_t));
  if (!This)
  {
    return NULL;
  }
  header_t_init(This);
  This->free = header_t_newFree;
  This->name = strdup(name);

  LOG(TRACE_DEBUG, "new header_t (name:%s) (This:%0.8p)", This->name, This);
  return This;
}

/**
 * Free resources of the object-like instance.
 * @param This the object-like instance.
 */
void header_t_newFree(header_t *This)
{
  LOG(TRACE_DEBUG, "header_t::newFree (name:%s)", This->name);
  free(This->name);
  if (This->values)
  {
    free(This->values);
  }
  free(This);
}

/**
 * Add a value to a HTTP header obj. It manages multi-valued headers.
 * @param This the object-like instance.
 * @param value the value to add to the header.
 */
void header_t_addValue(header_t *This, char *value)
{
  LOG(TRACE_DEBUG, "header_t::addHeader (name:%s) (value:%s)", This->name, value);
  if ((!value) || (!*value))
  {
    LOG(TRACE_ERROR, "header_t::addHeader: error - no value provided"
        " (name:%s)", This->name);
    return;
  }

  if (!This->values)
  {
    This->values = strdup(value);
  }
  else
  {
    size_t valuesSize = strlen(This->values) + strlen(value) + 2 /* ", " */+ 1;
    char *newValues = malloc(valuesSize * sizeof(char));
    sprintf(newValues, "%s, %s", This->values, value);
    free(This->values);
    This->values = newValues;
  }
}

