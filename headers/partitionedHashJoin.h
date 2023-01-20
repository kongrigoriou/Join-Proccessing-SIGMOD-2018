#ifndef partitionedHashJoin_h
#define partitionedHashJoin_h
#include "../headers/structures.h"
#include "../headers/functions.h"

relation PartitionedHashJoin(relation *relR, relation *relS, JobList* jobList);

#endif
