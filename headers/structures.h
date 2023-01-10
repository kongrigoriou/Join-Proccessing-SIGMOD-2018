#ifndef STRUCTURES_FILE
#define STRUCTURES_FILE
/** Type definition for a tuple */

#include <stdint.h>
#include "../headers/bitmap.h"

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

typedef struct statistics {
    uint64_t min;
    uint64_t max;
    uint64_t count;
    uint64_t distinct_count;
    unsigned int number;
}stats;

typedef struct Table{
    uint64_t numColumns;
    uint64_t numRows;
    uint64_t** relations;
    stats* stats;
} Table;

typedef struct joined{
    int rows;
    int columns;
    int* relations;    //which realtion is in each column
    int** inter;       //the actual intermidate reuslt
} joined;

int TableFitsCache(int cacheSize, int tableSize, int offSet);
int LoadTable(char *fileName,struct Table *table);
int fill_distinct_count(Table  *T, int table_size);
int index_is_in_hash(bitmap_t b, unsigned int number, uint64_t max, uint64_t min, uint64_t index);
#endif
