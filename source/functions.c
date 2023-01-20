#include "../headers/mainPartitionTest.h"
#include "../headers/functions.h"
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
            Partition(*rel, 0, rel->num_tuples -1, 1, N,reOrdered, &max, &(*pSum));

            step++;
            if(TableFitsCache(CACHE_SIZE, max, 5)){
                //printf("First partition was succesfull\n");
            }else{
                //begin second partition

                max = 0;
                int j = 0;

                while(j < pow(2,N) -1){
                    if((*pSum)[j] != (*pSum)[j + 1]){
                        Partition(*reOrdered, (*pSum)[j], (*pSum)[j + 1] -1, 2, N, reOrderedSecStep, &curMax, &pSumSecStep);
                        if(curMax > max) max = curMax;
                    }else{
                        pSumSecStep = calloc(pow(2,N), sizeof(int));
                    }
                    pSumFinal[j] = pSumSecStep;
                    
                    j++;
                }

                if((*pSum)[j] < rel->num_tuples){
                    // printf("IM HERE %d %d\n", (*pSum)[j], rel->num_tuples);
                    Partition(*reOrdered, (*pSum)[j], reOrdered->num_tuples -1, 2, N, reOrderedSecStep, &curMax, &pSumSecStep);
                    pSumFinal[j] = pSumSecStep;
                }else{
                    pSumSecStep = calloc(pow(2,N), sizeof(int));
                    pSumFinal[j] = pSumSecStep;
                }
                
                step++;
            }
    }
    return step;
}
