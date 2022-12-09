#include "structures.h"

struct QueryInfo{
    int* relationsId;
    char* predicates;
    char* filters;
    char* selections;

    int relationsCount;
    int predicatesCount;
    int filtersCount;
    int selectionsCount;
};

typedef struct QueryInfo QueryInfo;

struct QueryArray{
    QueryInfo** array;
    int size;
    int capacity;
};

typedef struct QueryArray QueryArray;

struct Batches{
    QueryArray** batches;
    int size;
    int capacity;
};

typedef struct Batches Batches;

QueryInfo* info_create();
QueryArray* array_create();
Batches* batches_create();
Batches* get_query_info();
void destroy(Batches*);

