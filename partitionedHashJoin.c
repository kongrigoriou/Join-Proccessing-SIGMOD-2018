#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mainPartitionTest.h"
#include "hopscotch.h"
#include "functions.h"
// #include "structures.h"

int TableFitsCache(int cacheSize, int tableSize, int offSet);


List* PartitionedHashJoin(relation *relR, relation *relS){
    //1. partitioning
    //2. building (hopschoch hashing)
    //3. probing 
    relation* reOrderedR, *reOrderedS;
    relation* reOrderedSecStepR, *reOrderedSecStepS;

    // printf("num tuples %d %d\n", relR->num_tuples, relS->num_tuples);

    //for R relation
    reOrderedR = malloc(sizeof(relation));
    reOrderedR->tuples = malloc(relR->num_tuples * sizeof(tuple));
    reOrderedR->num_tuples = relR->num_tuples;

    reOrderedSecStepR = malloc(sizeof(relation));
    reOrderedSecStepR->tuples = malloc(relR->num_tuples * sizeof(tuple));
    reOrderedSecStepR->num_tuples = relR->num_tuples;

    //for S relation
    reOrderedS = malloc(sizeof(relation));
    reOrderedS->tuples = malloc(relS->num_tuples * sizeof(tuple));
    reOrderedS->num_tuples = relS->num_tuples;

    reOrderedSecStepS = malloc(sizeof(relation));
    reOrderedSecStepS->tuples = malloc(relS->num_tuples * sizeof(tuple));
    reOrderedSecStepS->num_tuples = relS->num_tuples;

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
    // relation* smallerRel;
    // //find smaller partition
    // if(relR->num_tuples < relS->num_tuples){
    //     smallerRel = relR;
    //     printf("r1\n");
    // }else{
    //     smallerRel = relS;
    //     printf("r2\n");
    // }

    //after partitioning
    //for each bucket use hopscotch hashing
    // printf("Step: %d\n", step);
    hop* hopscotch = NULL;
    hop** hopscothArr = NULL;
    hop*** hopscotchTwoSteps = NULL;
    // int flagS = 0;

    if(stepR == 0){
        hopscotch = create_array(15, 3);

        for(int i = 0; i < relR->num_tuples; i++){
            size = insert(hopscotch, relR->tuples[i]);
        }
        // print_array(hopscotch->array, hopscotch->size);j
    }else if(stepR == 1){
        //array of hopscotch hash tables
        if(stepS == 0){
            hopscotch = create_array(15, 3);

            for(int i = 0; i < relS->num_tuples; i++){
                size = insert(hopscotch, relS->tuples[i]);
            }
        }else{
            hopscothArr = malloc(pow(2,3) * sizeof(hop*));
            for(int i = 0; i < pow(2,3); i++){
                hopscothArr[i] = create_array(15,3);
            }

            int counter = 0;
            while(counter < pow(2,3) -1){
                if(pSumR[counter] != pSumR[counter + 1]){
                    for(int j = pSumR[counter]; j < pSumR[counter + 1]; j++){
                        size = insert(hopscothArr[counter], reOrderedR->tuples[j]);
                    }
                }
                counter++;
            }
            if(pSumR[counter] != relR->num_tuples){
                for(int j = pSumR[counter]; j < relR->num_tuples; j++){
                    size = insert(hopscothArr[counter], reOrderedR->tuples[j]);
                }
            }


        }

    }else if(stepR == 2){
        //2d array of psums
        if(stepS == 0){
            hopscotch = create_array(15, 3);

            for(int i = 0; i < relS->num_tuples; i++){
                size = insert(hopscotch, relS->tuples[i]);
            }
        }else if(stepS == 1){
            hopscothArr = malloc(pow(2,3) * sizeof(hop*));
            for(int i = 0; i < pow(2,3); i++){
                hopscothArr[i] = create_array(15,3);
            }

            int counter = 0;
            while(counter < pow(2,3) -1){
                if(pSumS[counter] != pSumS[counter + 1]){
                    for(int j = pSumS[counter]; j < pSumS[counter + 1]; j++){
                        size = insert(hopscothArr[counter], reOrderedS->tuples[j]);
                    }
                }
                counter++;
            }
            if(pSumS[counter] != relS->num_tuples){
                for(int j = pSumS[counter]; j < relS->num_tuples; j++){
                    size = insert(hopscothArr[counter], reOrderedS->tuples[j]);
                }
            }

        }else if(stepS ==2){
            //hsit
            hopscotchTwoSteps = malloc(pow(2,3) * sizeof(hop**));
            for(int i = 0; i < pow(2,3); i++){
                hopscotchTwoSteps[i] = malloc(pow(2,3) * sizeof(hop*));
                for(int j = 0; j < pow(2,3); j++){
                    hopscotchTwoSteps[i][j] = create_array(15,3);
                }
            }

            for(int i = 0; i < pow(2,3); i++){
                for(int j = 0; j < pow(2,3) -1; j++){
                    if(pSumFinalR[i][j] < pSumFinalR[i][j+1]){
                        for(int k = pSumFinalR[i][j]; k < pSumFinalR[i][j + 1]; k++){
                            printf("i: %d j:%d psumfinal:%d\n\n", i, j, k);
                            size = insert(hopscotchTwoSteps[i][j], reOrderedSecStepR->tuples[k]);
                        }
                    }
                }
                int m = pow(2,3) -1;
                if(pSumFinalR[m][m] < relR->num_tuples){
                    for(int k = pSumFinalR[m][m]; k < relR->num_tuples; k++){
                        printf("last insert\n");

                        size = insert(hopscotchTwoSteps[m][m], reOrderedSecStepR->tuples[k]);
                    }
                }
                
            }
            
            
        }
    }

    //else if stepR = 2



    //probing 
    //for each element of every partition
    //search the hopscotch array
    List* results, *final;
    int index = 0;
    // result* res_array = malloc(sizeof(result));
    final = list_create();

    if(stepR == 0){
        if(stepS == 0){
            while(index < relS->num_tuples){
                results = search(hopscotch, relS->tuples[index]);
                // printf("main print %d\n", relS->tuples[index]);
                final = list_append(final, results);
                list_print(results);
                index++;
            }
        }else if(stepS == 1){
            while(index < pow(2,3)){
                if(pSumS[index] != pSumS[index + 1]){
                    for(int j = pSumS[index]; j < pSumS[index + 1]; j++){
                        results = search(hopscotch, reOrderedS->tuples[j]);
                        final = list_append(final, results);
                    }
                    list_print(results);
                }
                index++;
            }
            if(pSumS[index] != relS->num_tuples){
                for(int j = pSumS[index]; j < relS->num_tuples; j++){
                    results = search(hopscotch, reOrderedS->tuples[j]);
                    final = list_append(final, results);
                }
                list_print(results);
            }
        }else if(stepS == 2){
            for(int i = 0; i < pow(2,3); i++){
                index = 0;
                while(index < pow(2,3)){
                    if(pSumFinalS[i][index] != pSumFinalS[i][index + 1]){
                        for(int j = pSumFinalS[i][index]; j < pSumFinalS[i][index + 1]; j++){
                            results = search(hopscotch, reOrderedSecStepS->tuples[j]);
                            final = list_append(final, results);

                        }
                    }
                    index++;
                }
            }
        }
    }else if(stepR == 1){
        //has an array of hash tables
        if(stepS == 0){
            //hash table on 
            // int count = 0;
            while(index < relR->num_tuples){
                results = search(hopscotch, relR->tuples[index]);
                final = list_append(final, results);
                list_print(results);
                index++;
            }

        }else if(stepS == 1){
            while(index < pow(2,3) -1){
                if(pSumS[index] != pSumS[index + 1]){
                    for(int j = pSumS[index]; j < pSumS[index + 1]; j++){
                        results = search(hopscothArr[index], reOrderedS->tuples[j]);
                        final = list_append(final, results);
                    }
                    list_print(results);
                }   
                index++;
            }
            if(pSumS[index] != relS->num_tuples){
                for(int j = pSumS[index]; j < relS->num_tuples; j++){
                    results = search(hopscothArr[index], reOrderedS->tuples[j]);
                    final = list_append(final, results);
                }
                list_print(results);
            }   
            // index++;


        }else if(stepS == 2){
            for(int i = 0; i < pow(2,3); i++){
                index = 0;
                while(index < pow(2,3)){
                    if(pSumFinalS[i][index] != pSumFinalS[i][index + 1]){
                        for(int j = pSumFinalS[i][index]; j < pSumFinalS[i][index + 1]; j++){
                            results = search(hopscothArr[i], reOrderedSecStepS->tuples[j]);
                            final = list_append(final, results);

                            list_print(results);
                        }
                    }
                    index++;
                }
            }
        }
    }else if(stepR == 2){
        if(stepS == 0){
            for(int i = 0; i < pow(2,3); i++){
                index = 0;
                while(index < pow(2,3)){
                    if(pSumFinalR[i][index] != pSumFinalR[i][index + 1]){
                        for(int j = pSumFinalR[i][index]; j < pSumFinalR[i][index + 1]; j++){
                            results = search(hopscotch, reOrderedSecStepR->tuples[j]);
                            final = list_append(final, results);
                        }
                    }
                    index++;
                }
            }
        }else if(stepS == 1){
            for(int i = 0; i < pow(2,3); i++){
                index = 0;
                while(index < pow(2,3) -1){
                    if(pSumFinalR[i][index] != pSumFinalR[i][index + 1]){
                        for(int j = pSumFinalR[i][index]; j < pSumFinalR[i][index + 1]; j++){
                            results = search(hopscothArr[i], reOrderedSecStepR->tuples[j]);
                            final = list_append(final, results);

                            list_print(results);
                        }
                    }
                    index++;
                }
                if(pSumFinalR[i][index] != relR->num_tuples){
                    for(int j = pSumFinalR[i][index]; j < relR->num_tuples; j++){
                        results = search(hopscothArr[i], reOrderedSecStepR->tuples[j]);
                        final = list_append(final, results);

                        list_print(results);
                    }
                } 
            }
        }else if(stepS == 2){
            for(int i = 0; i < pow(2,3); i++){
                index = 0;
                while(index < pow(2,3) -1){
                    if(pSumFinalS[i][index] != pSumFinalS[i][index + 1]){
                        for(int j = pSumFinalS[i][index]; j < pSumFinalS[i][index + 1]; j++){
                            results = search(hopscotchTwoSteps[i][index], reOrderedSecStepS->tuples[j]);
                            final = list_append(final, results);

                            list_print(results);
                        }
                    }
                    index++;
                }
                // int n = pow(2,3) -1;
                if(pSumFinalS[i][index] < relS->num_tuples){
                    printf("psumfjhrf %d\n", pSumFinalS[i][index]);
                    for(int k = pSumFinalS[i][index]; k < relS->num_tuples; k++){
                        results = search(hopscotchTwoSteps[i][index], reOrderedSecStepS->tuples[k]);
                        final = list_append(final, results);

                        list_print(results);
                    }
                }
            }
        }
    }
    list_print(final);

    //frees
    free(reOrderedR->tuples);
    free(reOrderedR);
    free(reOrderedS->tuples);
    free(reOrderedS);

    free(pSumFinalR);
    free(pSumFinalS);
    
    return final;
}

// int main(void){
//     struct relation r1, r2;
//     r1.num_tuples=7;
//     r2.num_tuples = 3;
//     r1.tuples=malloc(7*sizeof(struct tuple));
//     r2.tuples = malloc(3*sizeof(tuple));
    
//     r1.tuples[0].key=1;
//     r1.tuples[1].key=2;
//     r1.tuples[2].key=3;
//     r1.tuples[3].key=4;
//     r1.tuples[4].key=4;
//     r1.tuples[5].key=4;
//     r1.tuples[6].key=4;


//     // r1.tuples[4].key=42163975;
//     // r1.tuples[5].key=25949786;
//     // r1.tuples[6].key=21354045;
//     // r1.tuples[7].key=20344488;
//     // r1.tuples[8].key=16341213;
//     // r1.tuples[9].key=62732974;

//     // for(int i = 0; i < 20; i++){
//     //     r2.tuples[i].key = i;
//     //     r2.tuples[i].payload = 0;
//     // }

//     r2.tuples[0].key = 1;
//     r2.tuples[1].key = 1;
//     r2.tuples[2].key = 3;


//     for(int i = 0; i < 6; i++){
//         r1.tuples[i].payload = i;
//     }

//     for(int i = 0; i < 3; i++){
//         r2.tuples[i].payload = i;
//     }    

//     // r2.tuples[3].key = 33633760;

//     result* res = PartitionedHashJoin(&r1, &r2);
// }