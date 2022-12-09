#ifndef STRUCTURES_FILE
#define STRUCTURES_FILE
/** Type definition for a tuple */

#include <stdint.h>

typedef struct tuple {
    int32_t key;
    int32_t payload;
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

struct Table{
    uint64_t numColumns;
    uint64_t numRows;
    struct relation** relations;
};

int TableFitsCache(int cacheSize, int tableSize, int offSet);
int LoadTable(char *fileName,struct Table **table);

#endif
