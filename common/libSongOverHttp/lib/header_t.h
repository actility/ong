
#ifndef _HEADER_T__H_
#define _HEADER_T__H_

typedef struct header_t
{
  // "name" and "values" members are first in the struct so that it can be
  // casted into sohHeader_t type and thus avoid copies
  char *name;
  char *values;

  void (* free) (struct header_t *);
  void (* addValue) (struct header_t *, char *);

  struct list_head chainLink;
    
} header_t;

header_t *new_header_t(char *name);
void header_t_newFree(header_t *This);

void header_t_addValue(header_t *This, char *value);

#endif

