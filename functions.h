#ifndef FUNCTIONS
#define FUNCTIONS

#define CACHE_SIZE 80
#define HOP_SIZE 15
#define N 3

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);

#endif