#ifndef FUNCTIONS
#define FUNCTIONS

#define CACHE_SIZE 50
#define HOP_SIZE 15
#define N 3

#include "list.h"

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);
List* PartitionedHashJoin(relation *relR, relation *relS);

#endif