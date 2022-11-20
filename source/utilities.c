#include "../headers/structures.h"

int TableFitsCache(int cacheSize, int tableSize, int offSet){ //offSet se kb
    
    if(tableSize>0 && ((cacheSize)-(offSet))>=tableSize*sizeof(struct tuple))
        return 1;
    else
        return 0;
};
