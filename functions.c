#include "mainPartitionTest.h"
#include "functions.h"
#include <stdlib.h>
#include <math.h>

int num_of_partitions(relation* reOrdered, relation* rel, int** pSum,relation* reOrderedSecStep, int** pSumFinal){
    int tableFits=TableFitsCache(CACHE_SIZE,rel->num_tuples,5);
    int step = 0;
    int max = 0;
    int curMax;
    int* pSumSecStep;

    if(!tableFits){
            
            //firts partition
            Partition(*rel, 0, rel->num_tuples -1, 1, 3,reOrdered, &max, &(*pSum));

            printf("reOrdered\n");
            for(int i=0;i<rel->num_tuples;i++){
                printf("%d\n",reOrdered->tuples[i].key);
            }
            printf("pSum\n");
            for(int i=0;i<pow(2,3);i++){
                printf("%d\n",(*pSum)[i]);
            }       
            step++;
            if(TableFitsCache(CACHE_SIZE, max, 5)){
                printf("First partition was succesfull\n");
            }else{
                //begin second partition

                max = 0;
                int j = 0;
                int** pSumFinal = calloc(pow(2,3),sizeof(int*));

                while(j < pow(2,3) -1){
                    if((*pSum)[j] != (*pSum)[j + 1]){
                        Partition(*reOrdered, (*pSum)[j], (*pSum)[j + 1] -1, 2, 3, reOrderedSecStep, &curMax, &pSumSecStep);
                        if(curMax > max) max = curMax;
                    }
                    //add new pSum to an existing one so we can have a final
                    pSumFinal[j] = pSumSecStep;
                    
                    j++;
                }

                printf("reOrdered\n");
                for(int i=0;i<10;i++){
                    printf("%d\n",reOrderedSecStep->tuples[i].key);
                }

                printf("pSum\n");
                for(int i=0;i<pow(2,3)-1;i++){
                    for(int j = 0; j < pow(2,3); j++){
                        printf("%d\n",pSumFinal[i][j]);
                    }
                    printf("%d --------------\n", i);
                }
                step++;
            }
    }
    return step;
}