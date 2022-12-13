#ifndef FUNCTIONS
#define FUNCTIONS
#include <unistd.h>

#define CACHE_SIZE 256000//sysconf(_SC_LEVEL2_CACHE_SIZE)
#define HOP_SIZE 1000
#define N 2
#define H 10

#include "list.h"
#include "structures.h"

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);
relation PartitionedHashJoin(relation *relR, relation *relS);

#endif