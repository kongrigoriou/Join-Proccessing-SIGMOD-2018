#ifndef FUNCTIONS
#define FUNCTIONS

#define CACHE_SIZE 30

typedef struct relation relation;
typedef struct result result;

int num_of_partitions(relation*,relation*, int**,relation*,int**);

#endif