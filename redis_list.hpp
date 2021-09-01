#include<stdio.h>
#include<stdlib.h>

typedef struct listNode{
  listNode*prev;
  listNode*next;
  void* val;
}listNode;

typedef struct list{
  listNode*head;
  listNode*tail;
  unsigned long len;
  void*(*dup)(void* ptr);
  void*(*free)(void* ptr);
  int (*match)(void *ptr,void *key);
};
