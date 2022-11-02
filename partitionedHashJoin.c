#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mainPartitionTest.h"
// #include "structures.h"

typedef struct relation relation;
typedef struct result result;

int TableFitsCache(int cacheSize, int tableSize, int offSet);


result* PartitionedHashJoin(relation *relR, relation *relS){
    //1. partitioning
    //2. building (hopschoch hashing)
    //3. probing 
    relation* reOrdered = NULL;
    relation* reOrderedSecStep = NULL;

    int max = 0;                //max bucket size after partition
    int curMax;
    int *pSum;
    int *pSumSecStep;
    //check if table fits cache if not first partition
    int tableFits=TableFitsCache(15,10,5);
    printf("Table fits %d\n", tableFits);

    relation* smallerRel;
    //find smaller partition
    if(relR->num_tuples < relS->num_tuples){
        smallerRel = relR;
        printf("r1\n");
    }else{
        smallerRel = relS;
        printf("r2\n");
    }

    if(!tableFits){
        reOrdered = malloc(sizeof(relation));
        reOrdered->tuples = malloc(relR->num_tuples * sizeof(tuple));
        //firts partition
        Partition(*smallerRel, 0, smallerRel->num_tuples, 1, 3,reOrdered, &max, &pSum);

        printf("reOrdered\n");
        for(int i=0;i<10;i++){
            printf("%d\n",reOrdered->tuples[i].key);
        }
        printf("pSum\n");
        for(int i=0;i<pow(2,3);i++){
            printf("%d\n",pSum[i]);
        }

        if(TableFitsCache(15, max, 5)){
            printf("First partition was succesfull\n");
        }else{
            //begin second partition
            reOrderedSecStep = malloc(sizeof(relation));
            reOrderedSecStep->tuples = malloc(smallerRel->num_tuples * sizeof(tuple));
            max = 0;
            int j = 0;
            int* pSumFinal = malloc(pow(2,3)*sizeof(int));

            while(j < pow(2,3) -1){
                if(pSum[j] != pSum[j + 1]){
                    Partition(*reOrdered, pSum[j], pSum[j + 1] -1, 2, 3, reOrderedSecStep, &curMax, &pSumSecStep);
                    if(curMax > max) max = curMax;
                }
                //add new pSum to an existing one so we can have a final
                for(int i = 0; i < pow(2, 3); i++){
                    pSumFinal[i] += pSumSecStep[i];
                }
                j++;
            }
            printf("reOrdered\n");
            for(int i=0;i<10;i++){
                printf("%d\n",reOrderedSecStep->tuples[i].key);
            }

            printf("pSum\n");
            for(int i=0;i<pow(2,3);i++){
                printf("%d\n",pSumFinal[i]);
            }
        }
    }
    
    return NULL;
}

int main(void){
    struct relation r1, r2;
    r1.num_tuples=10;
    r2.num_tuples = 20;
    r1.tuples=malloc(10*sizeof(struct tuple));
    r2.tuples = malloc(20*sizeof(tuple));
    
    r1.tuples[0].key=26468015;
    r1.tuples[1].key=22766314;
    r1.tuples[2].key=65339549;
    r1.tuples[3].key=33633760;
    r1.tuples[4].key=42163975;
    r1.tuples[5].key=25949786;
    r1.tuples[6].key=21354045;
    r1.tuples[7].key=20344488;
    r1.tuples[8].key=16341213;
    r1.tuples[9].key=62732974;

    for(int i = 0; i < 20; i++){
        r2.tuples[i].key = 26437895/(i+1);
    }

    result* res = PartitionedHashJoin(&r1, &r2);
}