#ifndef STRUCTURES_FILE
#define STRUCTURES_FILE
/** Type definition for a tuple */

#include <stdint.h>

typedef struct tuple {
    int64_t key;
    int64_t payload;
} tuple;
/**
* Type definition for a relation.
* It consists of an array of tuples and a size of the relation.
*/
struct relation {
    tuple *tuples;
    uint64_t num_tuples;
};
/**
* Type definition for a relation.
* It consists of an array of tuples and a size of the relation.
*/
struct result {
    tuple* tuplesR;
    tuple* tuplesS;
};

typedef struct Table{
    uint64_t numColumns;
    uint64_t numRows;
    uint64_t** relations;
} Table;

typedef struct joined{
    int rows;
    int columns;
    int* relations;    //which realtion is in each column
    int** inter;       //the actual intermidate reuslt
} joined;

int TableFitsCache(int cacheSize, int tableSize, int offSet);
int LoadTable(char *fileName,struct Table **table);

#endif
