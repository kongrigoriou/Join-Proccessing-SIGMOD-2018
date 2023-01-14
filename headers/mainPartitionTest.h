//
//  mainPartitionTest.h
//  


#ifndef mainPartitionTest_h
#define mainPartitionTest_h

#include <stdio.h>
#include <stdint.h>
#include "../headers/structures.h"

void Partition(struct relation initialRelation, int startIndex, int endIndex, int partitionStep, int numberOfBytes, struct relation* reOrdered, int* max, int** pSum, JobList* jobList);

void BuildHistogram(int tupleKey, int partitionStep, int numberOfBytes, int* hist, int* max, pthread_mutex_t* writeMutex);
#endif /* mainPartitionTest_h */
