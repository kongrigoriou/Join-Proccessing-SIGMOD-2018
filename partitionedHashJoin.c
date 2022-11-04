#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mainPartitionTest.h"
#include "hopscotch.h"
#include "functions.h"
// #include "structures.h"

int TableFitsCache(int cacheSize, int tableSize, int offSet);


result* PartitionedHashJoin(relation *relR, relation *relS){
    //1. partitioning
    //2. building (hopschoch hashing)
    //3. probing 
    relation* reOrderedR, *reOrderedS;
    relation* reOrderedSecStepR, *reOrderedSecStepS;

    //for R relation
    reOrderedR = malloc(sizeof(relation));
    reOrderedR->tuples = malloc(relR->num_tuples * sizeof(tuple));

    reOrderedSecStepR = malloc(sizeof(relation));
    reOrderedSecStepR->tuples = malloc(relR->num_tuples * sizeof(tuple));

    //for S relation
    reOrderedS = malloc(sizeof(relation));
    reOrderedS->tuples = malloc(relS->num_tuples * sizeof(tuple));

    reOrderedSecStepS = malloc(sizeof(relation));
    reOrderedSecStepS->tuples = malloc(relS->num_tuples * sizeof(tuple));

    // int max = 0;                //max bucket size after partition
    // int curMax;
    int *pSumR, *pSumS;
    // int *pSumSecStep;          //array of pSums
    int stepR, stepS;               //how many partiotions were needed
    int** pSumFinalR = calloc(pow(2,3),sizeof(int*));
    int** pSumFinalS = calloc(pow(2,3),sizeof(int*));

    stepR = num_of_partitions(reOrderedR, relR, &pSumR, reOrderedSecStepR, pSumFinalR);
    stepS = num_of_partitions(reOrderedS, relS, &pSumS, reOrderedSecStepS, pSumFinalS);

    printf("steps for R %d\n", stepR);
    printf("steps for S %d\n", stepS);
    
    //find relation with fewer partitions

    int size;
    relation* smallerRel;
    //find smaller partition
    if(relR->num_tuples < relS->num_tuples){
        smallerRel = relR;
        printf("r1\n");
    }else{
        smallerRel = relS;
        printf("r2\n");
    }

    //after partitioning
    //for each bucket use hopscotch hashing
    // printf("Step: %d\n", step);
    hop* hopscotch = NULL;
    hop** hopscothArr = NULL;

    if(stepR == 0){
        hopscotch = create_array(15, 3);

        for(int i = 0; i < smallerRel->num_tuples; i++){
            size = insert(hopscotch, smallerRel->tuples[i]);
        }
    }else if(stepR == 1){
        //array of hopscotch hash tables
        hopscothArr = malloc(pow(2,3) * sizeof(hop*));
        for(int i = 0; i < pow(2,3); i++){
            hopscothArr[i] = create_array(15,3);
        }

        int counter = 0;
        while(counter < pow(2,3)){
            if(pSumR[counter] != pSumR[counter + 1]){
                for(int j = pSumR[counter]; j < pSumR[counter + 1]; j++){
                    size = insert(hopscothArr[counter], reOrderedR->tuples[j]);
                }
            }
            counter++;
        }
    }

    //probing 
    //for each element of every partition
    //search the hopscotch array
    List* results;
    if(stepR == 0){
        int index = 0;
        while(index < pow(2,3)){
            results = search(hopscotch, relS->tuples[index]);
            printf("main print %d\n", results->size);
            list_print(results);
            index++;
        }
    }

    //frees
    free(reOrderedR->tuples);
    free(reOrderedR);
    free(reOrderedS->tuples);
    free(reOrderedS);

    free(pSumFinalR);
    free(pSumFinalS);
    
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

    r2.tuples[3].key = 33633760;

    result* res = PartitionedHashJoin(&r1, &r2);
}