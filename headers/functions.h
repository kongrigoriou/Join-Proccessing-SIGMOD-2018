#ifndef FUNCTIONS
#define FUNCTIONS
#include <unistd.h>

#define CACHE_SIZE 256000//sysconf(_SC_LEVEL2_CACHE_SIZE)
#define HOP_SIZE 100000
#define N 2
#define HN  100

#define NUM_OF_THREADS 4

#include "list.h"
#include "structures.h"
#include "jobs.h"
#include "hopscotch.h"

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);
relation PartitionedHashJoin(relation *relR, relation *relS, JobList*);
void Probing(hop* hopscotch, relation* reOrdered,int* start, int* finish, List** final);


#endif
