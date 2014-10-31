#ifndef _HEADER_T__H_
#define _HEADER_T__H_

typedef struct header_t
{
  void (*free)(struct header_t *);
  void (*addValue)(struct header_t *, char *);

  char *name;
  char *values;
  struct list_head chainLink;

} header_t;

header_t *new_header_t(char *name);
void header_t_newFree(header_t *This);

void header_t_addValue(header_t *This, char *value);

#endif

