#include<stdio.h>
#include<stdlib.h>

typedef struct zskiplistNode{
  struct zskiplistLevel{
    struct zskiplistNode *forward;
    unsigned int span;
  }level[];
  
  struct zskiplistNode*backward;
  double score;
  robj* obj;
}zskiplistNode;
