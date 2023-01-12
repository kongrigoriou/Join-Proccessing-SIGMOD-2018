#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "../headers/mainPartitionTest.h"
#include "../headers/hopscotch.h"
#include "../headers/functions.h"
#include "../headers/jobs.h"
// #include "structures.h"

int TableFitsCache(int cacheSize, int tableSize, int offSet);

relation PartitionedHashJoin(relation *relR, relation *relS, JobList* jobList){
    //1. partitioning
    //2. building (hopschoch hashing)
    //3. probing 
    relation* reOrderedR, *reOrderedS;
    relation* reOrderedSecStepR, *reOrderedSecStepS;
    int countProcesses;
    pthread_mutex_t condVarMutex, *hopMutexWrite, *hopMutexRead;
    pthread_cond_t condVar;
    int* noOfReaders = malloc(sizeof(int));

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
    int *pSumR, *pSumS;
    int **pSumRPtr = &pSumR, **pSumSPtr = &pSumS;
    int stepR, stepS;               //how many partiotions were needed
    int *stepRPtr = &stepR, *stepSPtr = &stepS;               //how many partiotions were needed
    int** pSumFinalR = calloc(pow(2,N),sizeof(int*));
    int** pSumFinalS = calloc(pow(2,N),sizeof(int*));
    //printf("before partitions\n");

    if (jobList != NULL) {
        pthread_mutex_init(&condVarMutex, NULL);
        pthread_cond_init(&condVar, NULL);

        countProcesses = 0;
        pthread_mutex_lock(&condVarMutex);

        Job* job = malloc(sizeof(Job));
        job->type = numOfPartitions;
        job->parameters = malloc(sizeof(args));
        job->parameters->arg1 = reOrderedR;
        job->parameters->arg2 = relR;
        job->parameters->arg3 = pSumRPtr;
        job->parameters->arg4 = reOrderedSecStepR;
        job->parameters->arg5 = pSumFinalR;
        job->parameters->arg6 = stepRPtr;
        job->parameters->arg7 = &condVarMutex;
        PushJob(jobList, job);

        job = malloc(sizeof(Job));
        job->type = numOfPartitions;
        job->parameters = malloc(sizeof(args));
        job->parameters->arg1 = reOrderedS;
        job->parameters->arg2 = relS;
        job->parameters->arg3 = pSumSPtr;
        job->parameters->arg4 = reOrderedSecStepS;
        job->parameters->arg5 = pSumFinalS;
        job->parameters->arg6 = stepSPtr;
        job->parameters->arg7 = &condVarMutex;
        PushJob(jobList, job);

        while (countProcesses < 2) {
            pthread_cond_wait(&condVar, &condVarMutex);
            countProcesses++;
        }
        pthread_mutex_unlock(&condVarMutex);

        pthread_cond_destroy(&condVarMutex);
        pthread_mutex_destroy(&condVarMutex);
    }
    else {
        num_of_partitions(reOrderedR, relR, pSumRPtr, reOrderedSecStepR, pSumFinalR, stepRPtr);
        num_of_partitions(reOrderedS, relS, pSumSPtr, reOrderedSecStepS, pSumFinalS, stepSPtr);
    }
    
    //printf("steps for R %d\n", stepR);
    //printf("steps for S %d\n", stepS);
    
    //find relation with fewer partitions

    // int size;

    //after partitioning
    //for each bucket use hopscotch hashing
    hop* hopscotch = NULL;
    hop** hopscothArr = NULL;
    hop*** hopscotchTwoSteps = NULL;
    int reverse=0;
    if(stepR == 0){
        hopscotch = create_array(HOP_SIZE, H);
        if (jobList != NULL) {
            *noOfReaders = 0;
            pthread_mutex_init(hopMutexRead, NULL);
            pthread_mutex_init(hopMutexWrite, NULL);
            pthread_mutex_init(&condVarMutex, NULL);
            pthread_cond_init(&condVar, NULL);

            countProcesses = 0;
            pthread_mutex_lock(&condVarMutex);
        }
        for(int i = 0; i < relR->num_tuples; i++){
            //relR->tuples[i];
            if (jobList != NULL) {
                tuple* tuplePtr = &(relR->tuples[i]);
                Job* job = malloc(sizeof(Job));
                job->type = insertHopScotch;
                job->parameters = malloc(sizeof(args));
                job->parameters->arg1 = hopscotch;
                job->parameters->arg2 = tuplePtr;
                job->parameters->arg3 = &condVarMutex;
                job->parameters->arg4 = hopMutexRead;
                job->parameters->arg5 = hopMutexWrite;
                job->parameters->arg6 = noOfReaders;
                PushJob(jobList, job);
            }
            else
                insert(hopscotch, relR->tuples[i], NULL, NULL, NULL);
        }
        if (jobList != NULL) {
            while (countProcesses < relR->num_tuples) {
                pthread_cond_wait(&condVar, &condVarMutex);
                countProcesses++;
            }
            pthread_mutex_unlock(&condVarMutex);

            pthread_cond_destroy(&condVarMutex);
            pthread_mutex_destroy(&condVarMutex);
            pthread_mutex_destroy(hopMutexRead);
            pthread_mutex_destroy(hopMutexWrite);
        }
    }else if(stepR == 1){
        //array of hopscotch hash tables
        if(stepS == 0){
            hopscotch = create_array(HOP_SIZE, H);
            reverse=1;
            if (jobList != NULL) {
                pthread_mutex_init(hopMutexWrite, NULL);
                pthread_mutex_init(hopMutexRead, NULL);
                pthread_mutex_init(&condVarMutex, NULL);
                pthread_cond_init(&condVar, NULL);

                countProcesses = 0;
                pthread_mutex_lock(&condVarMutex);
            }
            for(int i = 0; i < relS->num_tuples; i++){
                if (jobList != NULL) {
                    tuple* tuplePtr = &(relS->tuples[i]);
                    Job* job = malloc(sizeof(Job));
                    job->type = insertHopScotch;
                    job->parameters = malloc(sizeof(args));
                    job->parameters->arg1 = hopscotch;
                    job->parameters->arg2 = tuplePtr;
                    job->parameters->arg3 = &condVarMutex;
                    job->parameters->arg4 = hopMutexRead;
                    job->parameters->arg5 = hopMutexWrite;
                    job->parameters->arg6 = noOfReaders;
                    PushJob(jobList, job);
                }
                else
                    insert(hopscotch, relS->tuples[i], NULL, NULL, NULL);
            }
            if (jobList != NULL) {
                while (countProcesses < relS->num_tuples) {
                    pthread_cond_wait(&condVar, &condVarMutex);
                    countProcesses++;
                }
                pthread_mutex_unlock(&condVarMutex);

                pthread_cond_destroy(&condVarMutex);
                pthread_mutex_destroy(&condVarMutex);
                pthread_mutex_destroy(hopMutexRead);
                pthread_mutex_destroy(hopMutexWrite);
            }
        }else{
            hopscothArr = malloc(pow(2,N) * sizeof(hop*));
            for(int i = 0; i < pow(2,N); i++){
                hopscothArr[i] = create_array(HOP_SIZE,H);
            }

            int counter = 0;
            while(counter < pow(2,N) -1){
                if(pSumR[counter] != pSumR[counter + 1]){
                    if (jobList != NULL) {
                        pthread_mutex_init(hopMutexRead, NULL);
                        pthread_mutex_init(hopMutexWrite, NULL);
                        pthread_mutex_init(&condVarMutex, NULL);
                        pthread_cond_init(&condVar, NULL);

                        countProcesses = 0;
                        pthread_mutex_lock(&condVarMutex);
                    }
                    for(int j = pSumR[counter]; j < pSumR[counter + 1]; j++){
                        if (jobList != NULL) {
                            tuple* tuplePtr = &(reOrderedR->tuples[j]);
                            Job* job = malloc(sizeof(Job));
                            job->type = insertHopScotch;
                            job->parameters = malloc(sizeof(args));
                            job->parameters->arg1 = hopscothArr[counter];
                            job->parameters->arg2 = tuplePtr;
                            job->parameters->arg3 = &condVarMutex;
                            job->parameters->arg4 = hopMutexRead;
                            job->parameters->arg5 = hopMutexWrite;
                            job->parameters->arg6 = noOfReaders;
                            PushJob(jobList, job);
                        }
                        else
                            insert(hopscothArr[counter], reOrderedR->tuples[j], NULL, NULL, NULL);
                    }
                    if (jobList != NULL) {
                        while (countProcesses < pSumR[counter + 1] - pSumR[counter]) {
                            pthread_cond_wait(&condVar, &condVarMutex);
                            countProcesses++;
                        }
                        pthread_mutex_unlock(&condVarMutex);

                        pthread_cond_destroy(&condVarMutex);
                        pthread_mutex_destroy(&condVarMutex);
                        pthread_mutex_destroy(hopMutexRead);
                        pthread_mutex_destroy(hopMutexWrite);
                    }
                }
                counter++;
            }
            // if(pSumR[counter] != relR->num_tuples){
            if (jobList != NULL) {
                pthread_mutex_init(hopMutexRead, NULL);
                pthread_mutex_init(hopMutexWrite, NULL);
                pthread_mutex_init(&condVarMutex, NULL);
                pthread_cond_init(&condVar, NULL);

                countProcesses = 0;
                pthread_mutex_lock(&condVarMutex);
            }
            for(int j = pSumR[counter]; j < relR->num_tuples; j++){
                if (jobList != NULL) {
                    tuple* tuplePtr = &(reOrderedR->tuples[j]);
                    Job* job = malloc(sizeof(Job));
                    job->type = insertHopScotch;
                    job->parameters = malloc(sizeof(args));
                    job->parameters->arg1 = hopscothArr[counter];
                    job->parameters->arg2 = tuplePtr;
                    job->parameters->arg3 = &condVarMutex;
                    job->parameters->arg4 = hopMutexRead;
                    job->parameters->arg5 = hopMutexWrite;
                    job->parameters->arg6 = noOfReaders;
                    PushJob(jobList, job);
                }
                else
                    insert(hopscothArr[counter], reOrderedR->tuples[j], NULL, NULL, NULL);
            }
            if (jobList != NULL) {
                while (countProcesses < relR->num_tuples - pSumR[counter]) {
                    pthread_cond_wait(&condVar, &condVarMutex);
                    countProcesses++;
                }
                pthread_mutex_unlock(&condVarMutex);

                pthread_cond_destroy(&condVarMutex);
                pthread_mutex_destroy(&condVarMutex);
                pthread_mutex_destroy(hopMutexRead);
                pthread_mutex_destroy(hopMutexWrite);
            }
            // }
        }

    }else if(stepR == 2){
        //2d array of psums
        
        if(stepS == 0){
            hopscotch = create_array(HOP_SIZE, H);
            reverse=1;
            if (jobList != NULL) {
                pthread_mutex_init(hopMutexRead, NULL);
                pthread_mutex_init(hopMutexWrite, NULL);
                pthread_mutex_init(&condVarMutex, NULL);
                pthread_cond_init(&condVar, NULL);

                countProcesses = 0;
                pthread_mutex_lock(&condVarMutex);
            }
            for(int i = 0; i < relS->num_tuples; i++){
                if (jobList != NULL) {
                    tuple* tuplePtr = &(relS->tuples[i]);
                    Job* job = malloc(sizeof(Job));
                    job->type = insertHopScotch;
                    job->parameters = malloc(sizeof(args));
                    job->parameters->arg1 = hopscotch;
                    job->parameters->arg2 = tuplePtr;
                    job->parameters->arg3 = &condVarMutex;
                    job->parameters->arg4 = hopMutexRead;
                    job->parameters->arg5 = hopMutexWrite;
                    job->parameters->arg6 = noOfReaders;
                    PushJob(jobList, job);
                }
                else
                    insert(hopscotch, relS->tuples[i], NULL, NULL, NULL);
            }
            if (jobList != NULL) {
                while (countProcesses < relS->num_tuples) {
                    pthread_cond_wait(&condVar, &condVarMutex);
                    countProcesses++;
                }
                pthread_mutex_unlock(&condVarMutex);

                pthread_cond_destroy(&condVarMutex);
                pthread_mutex_destroy(&condVarMutex);
                pthread_mutex_destroy(hopMutexRead);
                pthread_mutex_destroy(hopMutexWrite);
            }
        }else if(stepS == 1){
            reverse=1;
            hopscothArr = malloc(pow(2,N) * sizeof(hop*));
            for(int i = 0; i < pow(2,N); i++){
                hopscothArr[i] = create_array(HOP_SIZE,H);
            }

            int counter = 0;
            while(counter < pow(2,N) -1){
                if(pSumS[counter] != pSumS[counter + 1]){
                    if (jobList != NULL) {
                        pthread_mutex_init(hopMutexRead, NULL);
                        pthread_mutex_init(hopMutexWrite, NULL);
                        pthread_mutex_init(&condVarMutex, NULL);
                        pthread_cond_init(&condVar, NULL);

                        countProcesses = 0;
                        pthread_mutex_lock(&condVarMutex);
                    }
                    for(int j = pSumS[counter]; j < pSumS[counter + 1]; j++){
                        if (jobList != NULL) {
                            tuple* tuplePtr = &(reOrderedS->tuples[j]);
                            Job* job = malloc(sizeof(Job));
                            job->type = insertHopScotch;
                            job->parameters = malloc(sizeof(args));
                            job->parameters->arg1 = hopscothArr[counter];
                            job->parameters->arg2 = tuplePtr;
                            job->parameters->arg3 = &condVarMutex;
                            job->parameters->arg4 = hopMutexRead;
                            job->parameters->arg5 = hopMutexWrite;
                            job->parameters->arg6 = noOfReaders;
                            PushJob(jobList, job);
                        }
                        else
                            insert(hopscothArr[counter], reOrderedS->tuples[j], NULL, NULL, NULL);
                    }
                    if (jobList != NULL) {
                        while (countProcesses < pSumS[counter + 1] - pSumS[counter]) {
                            pthread_cond_wait(&condVar, &condVarMutex);
                            countProcesses++;
                        }
                        pthread_mutex_unlock(&condVarMutex);

                        pthread_cond_destroy(&condVarMutex);
                        pthread_mutex_destroy(&condVarMutex);
                        pthread_mutex_destroy(hopMutexRead);
                        pthread_mutex_destroy(hopMutexWrite);
                    }
                }
                counter++;
            }
            if(pSumS[counter] != relS->num_tuples){
                if (jobList != NULL) {
                    pthread_mutex_init(hopMutexRead, NULL);
                    pthread_mutex_init(hopMutexWrite, NULL);
                    pthread_mutex_init(&condVarMutex, NULL);
                    pthread_cond_init(&condVar, NULL);

                    countProcesses = 0;
                    pthread_mutex_lock(&condVarMutex);
                }
                for(int j = pSumS[counter]; j < relS->num_tuples; j++){
                    if (jobList != NULL) {
                        tuple* tuplePtr = &(reOrderedS->tuples[j]);
                        Job* job = malloc(sizeof(Job));
                        job->type = insertHopScotch;
                        job->parameters = malloc(sizeof(args));
                        job->parameters->arg1 = hopscothArr[counter];
                        job->parameters->arg2 = tuplePtr;
                        job->parameters->arg3 = &condVarMutex;
                        job->parameters->arg4 = hopMutexRead;
                        job->parameters->arg5 = hopMutexWrite;
                        job->parameters->arg6 = noOfReaders;
                        PushJob(jobList, job);
                    }
                    else
                        insert(hopscothArr[counter], reOrderedS->tuples[j], NULL, NULL, NULL);
                }
                if (jobList != NULL) {
                    while (countProcesses < relS->num_tuples - pSumS[counter]) {
                        pthread_cond_wait(&condVar, &condVarMutex);
                        countProcesses++;
                    }
                    pthread_mutex_unlock(&condVarMutex);

                    pthread_cond_destroy(&condVarMutex);
                    pthread_mutex_destroy(&condVarMutex);
                    pthread_mutex_destroy(hopMutexRead);
                    pthread_mutex_destroy(hopMutexWrite);
                }
            }

        }else if(stepS ==2){
            //hsit
            hopscotchTwoSteps = malloc(pow(2,N) * sizeof(hop**));
            for(int i = 0; i < pow(2,N); i++){
                hopscotchTwoSteps[i] = malloc(pow(2,N) * sizeof(hop*));
                for(int j = 0; j < pow(2,N); j++){
                    hopscotchTwoSteps[i][j] = create_array(HOP_SIZE,H);
                }
            }

            for(int i = 0; i < pow(2,N); i++){
                int counter = 0;

                while(counter < pow(2,N) -1){
                    if(pSumFinalR[i][counter] < pSumFinalR[i][counter+1]){
                        if (jobList != NULL) {
                            pthread_mutex_init(hopMutexRead, NULL);
                            pthread_mutex_init(hopMutexWrite, NULL);
                            pthread_mutex_init(&condVarMutex, NULL);
                            pthread_cond_init(&condVar, NULL);

                            countProcesses = 0;
                            pthread_mutex_lock(&condVarMutex);
                        }
                        for(int k = pSumFinalR[i][counter]; k < pSumFinalR[i][counter + 1]; k++){
                            if (jobList != NULL) {
                                tuple* tuplePtr = &(reOrderedSecStepR->tuples[k]);
                                Job* job = malloc(sizeof(Job));
                                job->type = insertHopScotch;
                                job->parameters = malloc(sizeof(args));
                                job->parameters->arg1 = hopscotchTwoSteps[i][counter];
                                job->parameters->arg2 = tuplePtr;
                                job->parameters->arg3 = &condVarMutex;
                                job->parameters->arg4 = hopMutexRead;
                                job->parameters->arg5 = hopMutexWrite;
                                job->parameters->arg6 = noOfReaders;
                                PushJob(jobList, job);
                            }
                            else
                                insert(hopscotchTwoSteps[i][counter], reOrderedSecStepR->tuples[k], NULL, NULL, NULL);
                        }
                        if (jobList != NULL) {
                            while (countProcesses < pSumFinalR[i][counter + 1] - pSumFinalR[i][counter]) {
                                pthread_cond_wait(&condVar, &condVarMutex);
                                countProcesses++;
                            }
                            pthread_mutex_unlock(&condVarMutex);

                            pthread_cond_destroy(&condVarMutex);
                            pthread_mutex_destroy(&condVarMutex);
                            pthread_mutex_destroy(hopMutexRead);
                            pthread_mutex_destroy(hopMutexWrite);
                        }
                    }
                    counter++;
                }

                if(i < pow(2, N) -1){
                    if(pSumFinalR[i][counter -1] != pSumFinalR[i][counter]){
                        if (jobList != NULL) {
                            pthread_mutex_init(hopMutexRead, NULL);
                            pthread_mutex_init(hopMutexWrite, NULL);
                            pthread_mutex_init(&condVarMutex, NULL);
                            pthread_cond_init(&condVar, NULL);

                            countProcesses = 0;
                            pthread_mutex_lock(&condVarMutex);
                        }
                        for(int k = pSumFinalR[i][counter]; k < pSumFinalR[i+1][0]; k++){
                            if (jobList != NULL) {
                                tuple* tuplePtr = &(reOrderedSecStepR->tuples[k]);
                                Job* job = malloc(sizeof(Job));
                                job->type = insertHopScotch;
                                job->parameters = malloc(sizeof(args));
                                job->parameters->arg1 = hopscotchTwoSteps[i][counter];
                                job->parameters->arg2 = tuplePtr;
                                job->parameters->arg3 = &condVarMutex;
                                job->parameters->arg4 = hopMutexRead;
                                job->parameters->arg5 = hopMutexWrite;
                                job->parameters->arg6 = noOfReaders;
                                PushJob(jobList, job);
                            }
                            else
                                insert(hopscotchTwoSteps[i][counter], reOrderedSecStepR->tuples[k], NULL, NULL, NULL);
                        }
                        if (jobList != NULL) {
                            while (countProcesses < pSumFinalR[i + 1][0] - pSumFinalR[i][counter]) {
                                pthread_cond_wait(&condVar, &condVarMutex);
                                countProcesses++;
                            }
                            pthread_mutex_unlock(&condVarMutex);

                            pthread_cond_destroy(&condVarMutex);
                            pthread_mutex_destroy(&condVarMutex);
                            pthread_mutex_destroy(hopMutexRead);
                            pthread_mutex_destroy(hopMutexWrite);
                        }
                    }
                }else{
                    if (jobList != NULL) {
                        pthread_mutex_init(hopMutexRead, NULL);
                        pthread_mutex_init(hopMutexWrite, NULL);
                        pthread_mutex_init(&condVarMutex, NULL);
                        pthread_cond_init(&condVar, NULL);

                        countProcesses = 0;
                        pthread_mutex_lock(&condVarMutex);
                    }
                    for(int k = pSumFinalR[i][counter]; k < relR->num_tuples; k++){
                        if (jobList != NULL) {
                            tuple* tuplePtr = &(reOrderedSecStepR->tuples[k]);
                            Job* job = malloc(sizeof(Job));
                            job->type = insertHopScotch;
                            job->parameters = malloc(sizeof(args));
                            job->parameters->arg1 = hopscotchTwoSteps[i][counter];
                            job->parameters->arg2 = tuplePtr;
                            job->parameters->arg3 = &condVarMutex;
                            job->parameters->arg4 = hopMutexRead;
                            job->parameters->arg5 = hopMutexWrite;
                            job->parameters->arg6 = noOfReaders;
                            PushJob(jobList, job);
                        }
                        else
                            insert(hopscotchTwoSteps[i][counter], reOrderedSecStepR->tuples[k], NULL, NULL, NULL);
                    }
                    if (jobList != NULL) {
                        while (countProcesses < relR->num_tuples - pSumFinalR[i][counter]) {
                            pthread_cond_wait(&condVar, &condVarMutex);
                            countProcesses++;
                        }
                        pthread_mutex_unlock(&condVarMutex);

                        pthread_cond_destroy(&condVarMutex);
                        pthread_mutex_destroy(&condVarMutex);
                        pthread_mutex_destroy(hopMutexRead);
                        pthread_mutex_destroy(hopMutexWrite);
                    }
                }
            }
        }
    }

    //probing 
    //for each element of every partition
    //search the hopscotch array
    List* results, *final;
    int index = 0;
    // result* res_array = malloc(sizeof(result));
    final = NULL;

    if(stepR == 0){
        if(stepS == 0){
            while(index < relS->num_tuples){
                results = search(hopscotch, relS->tuples[index]);
                // list_print(results);
                final = list_append(final, results);
                index++;
            }

            destroy_hop(hopscotch);
        }else if(stepS == 1){
            while(index < pow(2,N) -1){
                if(pSumS[index] != pSumS[index + 1]){
                    for(int j = pSumS[index]; j < pSumS[index + 1]; j++){
                        results = search(hopscotch, reOrderedS->tuples[j]);
                        final = list_append(final, results);
                    }
                    // list_print(results);
                }
                index++;
            }
            for(int j = pSumS[index]; j < relS->num_tuples; j++){
                results = search(hopscotch, reOrderedS->tuples[j]);
                final = list_append(final, results);
            }
            // list_print(results);

            destroy_hop(hopscotch);
        }else if(stepS == 2){
            for(int i = 0; i < pow(2,N); i++){
                index = 0;
                while(index < pow(2,N) -1){
                    if(pSumFinalS[i][index] != pSumFinalS[i][index + 1]){
                        for(int j = pSumFinalS[i][index]; j < pSumFinalS[i][index + 1]; j++){
                            results = search(hopscotch, reOrderedSecStepS->tuples[j]);
                            final = list_append(final, results);
                        }
                    }
                    index++;
                }

                if(i < pow(2,N) -1){
                    for(int j = pSumFinalS[i][index]; j < pSumFinalS[i+1][0]; j++){
                        results = search(hopscotch, reOrderedSecStepS->tuples[j]);
                        final = list_append(final, results);
                    }
                }else{
                    for(int j = pSumFinalS[i][index]; j < relS->num_tuples; j++){
                        results = search(hopscotch, reOrderedSecStepS->tuples[j]);
                        final = list_append(final, results);
                    }
                }
            }
            destroy_hop(hopscotch);
        }
    }else if(stepR == 1){
        //has an array of hash tables
        if(stepS == 0){
            //hash table on 
            while(index < relR->num_tuples){
                results = search(hopscotch, relR->tuples[index]);
                final = list_append(final, results);
                index++;
            }

            destroy_hop(hopscotch);

        }else if(stepS == 1){
            while(index < pow(2,N) -1){
                if(pSumS[index] != pSumS[index + 1]){
                    for(int j = pSumS[index]; j < pSumS[index + 1]; j++){
                        results = search(hopscothArr[index], reOrderedS->tuples[j]);
                        final = list_append(final, results);
                    }
                }   
                index++;
            }

            for(int j = pSumS[index]; j < relS->num_tuples; j++){
                results = search(hopscothArr[index], reOrderedS->tuples[j]);
                final = list_append(final, results);
            }

            //free hash table 
            for(int i = 0; i < pow(2,N); i++){
                destroy_hop(hopscothArr[i]);
            }
            free(hopscothArr);


        }else if(stepS == 2){
            for(int i = 0; i < pow(2,N); i++){
                index = 0;
                while(index < pow(2,N) -1){
                    if(pSumFinalS[i][index] != pSumFinalS[i][index + 1]){
                        for(int j = pSumFinalS[i][index]; j < pSumFinalS[i][index + 1]; j++){
                            results = search(hopscothArr[i], reOrderedSecStepS->tuples[j]);
                            final = list_append(final, results);
                        }
                    }
                    index++;
                }

                // if(pSumFinalS[index][index] != relS->num_tuples){
                if(i < pow(2,N) -1){
                    for(int j = pSumFinalS[i][index]; j < pSumFinalS[i + 1][0]; j++){
                        results = search(hopscothArr[index], reOrderedSecStepS->tuples[j]);
                        final = list_append(final, results);
                    }
                }else{
                    for(int j = pSumFinalS[i][index]; j < relS->num_tuples; j++){
                        results = search(hopscothArr[index], reOrderedSecStepS->tuples[j]);
                        final = list_append(final, results);
                    }
                }
            }

            for(int i = 0; i < pow(2,N); i++){
                destroy_hop(hopscothArr[i]);
            }
            free(hopscothArr);
        }
    }else if(stepR == 2){
        if(stepS == 0){
            for(int i = 0; i < pow(2,N); i++){
                index = 0;
                while(index < pow(2,N) -1){
                    if(pSumFinalR[i][index] != pSumFinalR[i][index + 1]){
                        for(int j = pSumFinalR[i][index]; j < pSumFinalR[i][index + 1]; j++){
                            results = search(hopscotch, reOrderedSecStepR->tuples[j]);
                            final = list_append(final, results);
                        }
                    }
                    index++;
                }

                if(i < pow(2, N) -1){
                    for(int j = pSumFinalR[i][index]; j < pSumFinalR[i + 1][0]; j++){
                        results = search(hopscotch, reOrderedSecStepR->tuples[j]);
                        final = list_append(final, results);
                    }
                }else{
                    for(int j = pSumFinalR[i][index]; j < relR->num_tuples; j++){
                        results = search(hopscotch, reOrderedSecStepR->tuples[j]);
                        final = list_append(final, results);
                    }
                }
            }

            destroy_hop(hopscotch);
        }else if(stepS == 1){
            for(int i = 0; i < pow(2,N); i++){
                index = 0;
                while(index < pow(2,N) -1){
                    if(pSumFinalR[i][index] != pSumFinalR[i][index + 1]){
                        for(int j = pSumFinalR[i][index]; j < pSumFinalR[i][index + 1]; j++){
                            results = search(hopscothArr[i], reOrderedSecStepR->tuples[j]);
                            final = list_append(final, results);

                            // list_print(results);
                        }
                    }
                    index++;
                }
                if(i < pow(2, N) -1){
                    for(int j = pSumFinalR[i][index]; j < pSumFinalR[i + 1][0]; j++){
                        results = search(hopscothArr[i], reOrderedSecStepR->tuples[j]);
                        final = list_append(final, results);
                    }
                }else{
                    for(int j = pSumFinalR[i][index]; j < relR->num_tuples; j++){
                        results = search(hopscothArr[i], reOrderedSecStepR->tuples[j]);
                        final = list_append(final, results);
                    }
                }
            }
            for(int i = 0; i < pow(2,N); i++){
                destroy_hop(hopscothArr[i]);
            }
            free(hopscothArr);
        }else if(stepS == 2){
            for(int i = 0; i < pow(2,N); i++){
                index = 0;
                while(index < pow(2,N) -1){
                    if(pSumFinalS[i][index] != pSumFinalS[i][index + 1]){
                        //printf("\n");
                        for(int j = pSumFinalS[i][index]; j < pSumFinalS[i][index + 1]; j++){
                            results = search(hopscotchTwoSteps[i][index], reOrderedSecStepS->tuples[j]);
                            final = list_append(final, results);
                        }
                    }
                    index++;
                }
                // printf("\n");

                // for(int a= 0; a < pow(2,N); a++){
                //     for(int b = 0; b < pow(2,N); b++){
                //         printf("%d ", pSumFinalS[a][b]);
                //     }
                //     printf("\n");
                // }

                // printf("I: %d\nindex: %d\n\n", i, index);
                if(pSumFinalS[i][index-1] != pSumFinalS[i][index]){
                    if(i < pow(2,N) -1){
                        // printf("PSUM INDEX %d,\n psumnext %d\n", pSumFinalS[i][index], pSumFinalS[i+1][0]);
                        for(int k = pSumFinalS[i][index]; k < pSumFinalS[i + 1][0]; k++){
                            // printf("%d\n", k);

                            results = search(hopscotchTwoSteps[i][index], reOrderedSecStepS->tuples[k]);
                            final = list_append(final, results);

                            // list_print(results);
                        }

                    }else{
                        // printf("PSUM INDEX %d,\n psumnext %ld\n", pSumFinalS[i][index], relS->num_tuples);

                        for(int k = pSumFinalS[i][index]; k < relS->num_tuples; k++){
                            results = search(hopscotchTwoSteps[i][index], reOrderedSecStepS->tuples[k]);
                            final = list_append(final, results);
                        }
                    }
                }
            }
            for(int i = 0; i < pow(2,N); i++){
                for(int j = 0; j < pow(2,N); j++){
                    destroy_hop(hopscotchTwoSteps[i][j]);
                }
                free(hopscotchTwoSteps[i]);
            }
            free(hopscotchTwoSteps);
        }
    }
    //list_print(final);
    // free(results);
    // list_destroy(results);
    //frees
    free(reOrderedR->tuples);
    free(reOrderedR);
    free(reOrderedS->tuples);
    free(reOrderedS);
    free(reOrderedSecStepR->tuples);
    free(reOrderedSecStepR);
    free(reOrderedSecStepS->tuples);
    free(reOrderedSecStepS);
    
    if(stepR != 0){
        free(pSumR);
    }
    if(stepS != 0)
        free(pSumS);

    if(stepR == 2)
        for(int i = 0; i < pow(2,N); i++){
           free(pSumFinalR[i]);
        }
        
    free(pSumFinalR);

    if(stepS == 2)
        for(int i = 0; i < pow(2,N); i++){
            free(pSumFinalS[i]);
        }

    free(pSumFinalS);
    relation final_array;
    final_array.num_tuples=final->size;
    final_array.tuples=calloc(final->size,sizeof(tuple));
    ListNode* node=final->head;
    
    //reverse the tuples if the realtions where reversed
    for(int i = 0; i < final->size; i++){
        if(reverse){
            final_array.tuples[i].key=node->data.payload;
            final_array.tuples[i].payload=node->data.key;
        }
        else{
            final_array.tuples[i].key=node->data.key;
            final_array.tuples[i].payload=node->data.payload;
        }
        node=node->next;
    }
    
    list_destroy(final);
    free(noOfReaders);
    return final_array;
}
