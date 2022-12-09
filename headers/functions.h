#ifndef FUNCTIONS
#define FUNCTIONS
#include <unistd.h>

#define CACHE_SIZE sysconf(_SC_LEVEL2_CACHE_SIZE)
#define HOP_SIZE 32
#define N 5

#include "list.h"
#include "structures.h"

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);
relation PartitionedHashJoin(relation *relR, relation *relS);

#endif