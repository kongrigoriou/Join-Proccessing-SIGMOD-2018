#include "../headers/mainPartitionTest.h"
#include "../headers/functions.h"
#include "../headers/jobs.h"
#include <stdlib.h>
#include <math.h>

void num_of_partitions(relation* reOrdered, relation* rel, int** pSum,relation* reOrderedSecStep, int** pSumFinal, int* step, JobList* jobList){
    int tableFits=TableFitsCache(CACHE_SIZE,rel->num_tuples,5);
    int max = 0;
    int curMax;
    int* pSumSecStep;

    *step = 0;
    if(!tableFits){
            
            //firts partition
            Partition(*rel, 0, rel->num_tuples -1, 1, N,reOrdered, &max, &(*pSum), jobList);

            // printf("reOrdered\n");
            // for(int i=0;i<rel->num_tuples;i++){
            //     printf("%d\n",reOrdered->tuples[i].key);
            // }
            // printf("pSum\n");
            // for(int i=0;i<pow(2,3);i++){
            //     printf("%d\n",(*pSum)[i]);
            // }       
            (*step)++;
            if(TableFitsCache(CACHE_SIZE, max, 5)){
                //printf("First partition was succesfull\n");
            }else{
                //begin second partition

                max = 0;
                int j = 0;
                // int** pSumFinal = calloc(pow(2,3),sizeof(int*));

                while(j < pow(2,N) -1){
                    if((*pSum)[j] != (*pSum)[j + 1]){
                        Partition(*reOrdered, (*pSum)[j], (*pSum)[j + 1] -1, 2, N, reOrderedSecStep, &curMax, &pSumSecStep, jobList);
                        if(curMax > max) max = curMax;
                    }else{
                        pSumSecStep = calloc(pow(2,N), sizeof(int));
                    }
                    pSumFinal[j] = pSumSecStep;
                    
                    j++;
                }

                if((*pSum)[j] < rel->num_tuples){
                    // printf("IM HERE %d %d\n", (*pSum)[j], rel->num_tuples);
                    Partition(*reOrdered, (*pSum)[j], reOrdered->num_tuples -1, 2, N, reOrderedSecStep, &curMax, &pSumSecStep, jobList);
                    pSumFinal[j] = pSumSecStep;
                }else{
                    pSumSecStep = calloc(pow(2,N), sizeof(int));
                    pSumFinal[j] = pSumSecStep;
                }
                


                // printf("reOrdered\n");
                // for(int i=0;i<rel->num_tuples;i++){
                //     printf("%d\n",reOrderedSecStep->tuples[i].key);
                // }

                // printf("pSum\n");
                // for(int i=0;i<pow(2,3);i++){
                //     for(int j = 0; j < pow(2,3); j++){
                //         printf("%d\n",pSumFinal[i][j]);
                //     }
                //     printf("%d --------------\n", i);
                // }
                (*step)++;
            }
    }
}
