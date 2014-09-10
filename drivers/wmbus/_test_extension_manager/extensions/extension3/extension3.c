#include <stdio.h>

int extension3(char *input, int inputLen, char * output, int outputLen)
{
  if (0 == (inputLen % 2))
  {
    sprintf(output, "extension3 succeeds");
    return 1;
  }
  
  sprintf(output, "extension3 fails");
  return -1;
}

