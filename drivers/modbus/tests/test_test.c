#include <assert.h>
#include "modbus-acy.h"


void test_test1()
{
  printf("test_test1\n");
  printf("OK\n");
}

void test_test2()
{
  printf("test_test2\n");
  //assert(1 == 2);
  printf("OK\n");
}

int main(int argc, char *argv[])
{
  test_test1();
  test_test2();
  return 0;
}
