#include <stdio.h>
#include "../headers/acutest.h"
#include "../headers/structures.h"

void test1_tableFitsCache(void){
    int fits=TableFitsCache(1000,5,5);
    
    TEST_ASSERT(fits==1);
}

void test2_tableFitsCache(void){
    int fits=TableFitsCache(1000,2,1000);
    
    TEST_ASSERT(fits==0);

}

void testLoadTable(void){
    struct Table *table=malloc(sizeof(Table));
    int load=LoadTable("./input/small/r0",&table[0]);
    
    TEST_ASSERT(load==0);
    TEST_ASSERT(table[0].numColumns==3);
    free(table);
}

TEST_LIST = {
    {"test1_tableFitsCache",test1_tableFitsCache},
    {"test2_tableFitsCache",test2_tableFitsCache},
    {"testLoadTable",testLoadTable},
    {NULL,NULL}
};

