#ifndef FUNCTIONS
#define FUNCTIONS
#include <unistd.h>

#define CACHE_SIZE sysconf(_SC_LEVEL2_CACHE_SIZE)*1000000
#define HOP_SIZE 10000
#define N 2
#define H 1000

#include "list.h"
#include "structures.h"

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);
relation PartitionedHashJoin(relation *relR, relation *relS);

#endif