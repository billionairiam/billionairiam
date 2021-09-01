#include<stdlib.h>
#include<stdio.h>

typedef struct dictht{
  dictEntry**table;
  unsigned long size;
  unsigned long sizemask;
  unsigend long used;
}dictht;

typedef struct dictEntry{
  void* key;
  union{
    void* val;
    uint64_tu64;
    uint64_ts64;
  }v;
}dictEntry;

typedef struct dict{
  dickType* type;
  void*private;
  dictht[2];
  int trehashidx;/*rehashing not in progress if trehashidx==-1*/
}dict;

typedef struct dickType{
  unsigned int (*hashFunction)(const void *key);
  void*(*keyDup)(void* private,const void* key);
  
}dickType;
