
#include <string.h>


char *reverse_strchr(const char *s, char c) {
	char *pt;
	for (pt = s + strlen(s) - 1 ; pt >= s; pt--) {
		if (*pt == c)
			return pt;
	}
	return NULL;
}

int my_strcut(char *str, char *tok, char **args, int szargs) {
	register char *pt, *opt;
	int	 n;

	opt = pt = str;
	n	= 0;
	memset (args, 0, szargs*sizeof(char *));
	while ((pt = strstr(pt, tok)) && (n < szargs-1)) {
		*pt++		   = 0;
		args[n++]	   = opt;
		opt			 = pt;
	}
	if (*opt && (n < szargs))
		args[n++]	   = opt;
	return  n;
}

char *stringToLower(char *str) {
	char *pt = str;
	while	(*pt) {
		*pt	= tolower(*pt);
		pt ++;
	}
	return str;
}
