#include<stdio.h>
#include<stdlib.h>
#include<string.h>
struct sdshdr
{
  int len;
  int free;
  char buf[];
}
