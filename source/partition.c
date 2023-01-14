//
//  partition.c
//  
#include "../headers/mainPartitionTest.h"
#include "../headers/jobs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

void Partition(struct relation initialRelation, int startIndex, int endIndex, int partitionStep, int numberOfBytes, struct relation* reOrdered, int* max, int** pSum, JobList* jobList){
    
    int* hist;  //isws na thelei malloc
    int currentIndex[(int)pow(2,numberOfBytes)]; //boithitikos pinakas pou kratw se poia thesi prepei na topothetisw ton epomeno
    int hashedValue, countProcesses;
    pthread_mutex_t* writeMutex = NULL, *condVarMutex = NULL;
    pthread_cond_t* condVar = NULL;
    
    *pSum = malloc((int)pow(2,numberOfBytes)*sizeof(int));
    hist=malloc((int)pow(2,numberOfBytes)*sizeof(int));
    for(int i=0;i<(int)pow(2,numberOfBytes);i++){
        hist[i]=0;
    }
    *max=0;
    if(jobList!=NULL){
        pthread_mutex_init(writeMutex, NULL);
        pthread_mutex_init(condVarMutex, NULL);
        pthread_cond_init(condVar, NULL);

        countProcesses = 0;
        pthread_mutex_lock(condVarMutex);
    }
    for(int j=startIndex;j<=endIndex;j++){
        if(jobList !=NULL){
            Job* job = malloc(sizeof(Job));
            job->type = buildHistogram;
            job->parameters = malloc(sizeof(args));
            job->parameters->arg1 = &(initialRelation.tuples[j].key);
            job->parameters->arg2 = &partitionStep;
            job->parameters->arg3 = &numberOfBytes;
            job->parameters->arg4 = hist;
            job->parameters->arg5 = max;
            job->parameters->arg6 = writeMutex;
            job->parameters->arg7 = condVarMutex;
            PushJob(jobList, job);
        }
        else
            BuildHistogram(initialRelation.tuples[j].key, partitionStep, numberOfBytes, hist, max, writeMutex);
    }
    if(jobList!=NULL){
        while (countProcesses <= endIndex-startIndex) {
            pthread_cond_wait(condVar, condVarMutex);
            countProcesses++;
        }
        pthread_mutex_unlock(condVarMutex);
        pthread_cond_destroy(condVar);
        pthread_mutex_destroy(writeMutex);
    }
    (*pSum)[0]=startIndex;
    currentIndex[0]=startIndex;
    for(int i=1;i<(int)pow(2,numberOfBytes);i++){
        (*pSum)[i]=(*pSum)[i-1]+hist[i-1];
        currentIndex[i]=(*pSum)[i];
    }
    for(int j=startIndex;j<=endIndex;j++){
        hashedValue=initialRelation.tuples[j].key>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
        hashedValue=hashedValue%((int)pow(2,numberOfBytes));  //petaw ta prwta pshfia pou de xreiazomai
        reOrdered->tuples[currentIndex[hashedValue]].key=initialRelation.tuples[j].key;
        reOrdered->tuples[currentIndex[hashedValue]].payload=initialRelation.tuples[j].payload;
        currentIndex[hashedValue]++;
    }
    free(hist);
}

void BuildHistogram(int tupleKey, int partitionStep, int numberOfBytes, int* hist, int* max, pthread_mutex_t* writeMutex){
    int hashedValue;
    
    hashedValue=tupleKey>>((partitionStep-1)*numberOfBytes); // petaw ta teleutaia pshfia pou de xreiazomai
    hashedValue=hashedValue%((int)pow(2,numberOfBytes));  //petaw ta prwta pshfia pou de xreiazomai
    if(writeMutex!=NULL)
        pthread_mutex_lock(writeMutex);
    hist[hashedValue]++;
    if(hist[hashedValue]>(*max))
        (*max)=hist[hashedValue];
    if(writeMutex!=NULL)
        pthread_mutex_unlock(writeMutex);
}
