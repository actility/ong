
#ifndef _TOOLS_H_
#define _TOOLS_H_

extern char *reverse_strchr(const char *s, char c);
extern int my_strcut(char *str, char *tok, char **args, int szargs);
char *stringToLower(char *str);

extern void *__IECcalloc(int line, int x, int y);
#define Calloc(X,Y) __IECcalloc(__LINE__,X,Y)
extern void *__IECstrdup(int line, char *s);
#define Strdup(S) __IECstrdup(__LINE__,S)
#define Free(S) if (S) free(S);S=NULL;


#endif //_TOOLS_H_
