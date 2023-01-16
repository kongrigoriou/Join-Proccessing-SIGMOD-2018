//
//  mainPartitionTest.h
//  


#ifndef mainPartitionTest_h
#define mainPartitionTest_h

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "../headers/structures.h"
#include "../headers/jobs.h"

void Partition(struct relation initialRelation, int startIndex, int endIndex, int partitionStep, int numberOfBytes, int* max, int** hist);

void BuildReorderedFromPSum(struct relation initialRelation, int** pSum, int startIndex, int endIndex, int partitionStep, int numberOfBytes, struct relation* reOrdered, int* hist);
#endif /* mainPartitionTest_h */
