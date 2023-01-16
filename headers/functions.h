#ifndef FUNCTIONS
#define FUNCTIONS
#include <unistd.h>
#include "jobs.h"

#define CACHE_SIZE 256000//sysconf(_SC_LEVEL2_CACHE_SIZE)
#define HOP_SIZE 100000
#define N 2
#define H 100

#include "list.h"
#include "structures.h"

typedef struct relation relation;
typedef struct result result;

void num_of_partitions(relation*,relation*, int**,relation*,int**,int*,JobList*,int);
relation PartitionedHashJoin(relation *relR, relation *relS, JobList* jobList, int numberOfThreads);

#endif
