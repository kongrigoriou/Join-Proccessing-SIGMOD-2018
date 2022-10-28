//
//  mainPartitionTest.h
//  


#ifndef mainPartitionTest_h
#define mainPartitionTest_h

#include <stdio.h>
#include <stdint.h>

typedef struct tuple {
    int32_t key;
    int32_t payload;
} tuple;

struct relation {
    tuple *tuples;
    uint32_t num_tuples;
};

void Partition(struct relation initialRelation, int startIndex, int endIndex, int partitionStep, int numberOfBytes, struct relation* reOrdered, int* max, int** pSum);

#endif /* mainPartitionTest_h */
