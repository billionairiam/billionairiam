#include<stdio.h>
#include<stdlib.h>

struct sdshdr
{
  int len;
  int free;
  char buf[];
}
