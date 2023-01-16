#include "../headers/mainPartitionTest.h"
#include "../headers/functions.h"
#include "../headers/jobs.h"
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

void num_of_partitions(relation* reOrdered, relation* rel, int** pSum,relation* reOrderedSecStep, int** pSumFinal, int* step, JobList* jobList, int numberOfThreads){
    int tableFits=TableFitsCache(CACHE_SIZE,rel->num_tuples,5);
    int max = 0, completedTuples, *maxThreads;
    int curMax, allProcesses;
    int* pSumSecStep, *startIndexThreads, *endIndexThreads, *partitionStepThreads, *NThreads;
    int** histThreads, *hist;
    int countProcesses;
    pthread_mutex_t* condVarMutex=NULL;
    pthread_cond_t* condVar=NULL;

    if(jobList!=NULL){
        condVarMutex=malloc(sizeof(pthread_mutex_t));
        condVar=malloc(sizeof(pthread_cond_t));
    }
    
    *step = 0;
    if(!tableFits){
        if(jobList!=NULL){
            if (numberOfThreads <= rel->num_tuples)
                allProcesses = numberOfThreads;
            else
                allProcesses = rel->num_tuples;
            pthread_mutex_init(condVarMutex, NULL);
            pthread_cond_init(condVar, NULL);

            countProcesses = 0;
            pthread_mutex_lock(condVarMutex);
            
            histThreads = malloc(allProcesses*sizeof(int*));
            completedTuples = 0;
            maxThreads=malloc(allProcesses*sizeof(int));
            startIndexThreads=malloc(allProcesses*sizeof(int));
            endIndexThreads=malloc(allProcesses*sizeof(int));
            partitionStepThreads=malloc(sizeof(int));
            (*partitionStepThreads)=1;
            NThreads=malloc(sizeof(int));
            (*NThreads)=N;
            for(int i=0;i<allProcesses-1;i++){
                maxThreads[i]=0;
                startIndexThreads[i]=i*((int)(rel->num_tuples/allProcesses));
                endIndexThreads[i]=((i+1)*((int)(rel->num_tuples/ allProcesses)))-1;
                Job* job = malloc(sizeof(Job));
                job->type = buildHistogram;
                job->parameters = malloc(sizeof(args));
                job->parameters->arg1 = rel;
                job->parameters->arg2 = &(startIndexThreads[i]);
                job->parameters->arg3 = &(endIndexThreads[i]);
                job->parameters->arg4 = partitionStepThreads;
                job->parameters->arg5 = NThreads;
                job->parameters->arg6 = &(maxThreads[i]);
                job->parameters->arg7 = &(histThreads[i]);
                job->parameters->arg8 = condVarMutex;
                PushJob(jobList, job);
                
                completedTuples+=(int)rel->num_tuples/allProcesses;
            }
            
            maxThreads[allProcesses-1]=0;
            startIndexThreads[allProcesses-1]=completedTuples;
            endIndexThreads[allProcesses-1]=rel->num_tuples-1;
            Job* job = malloc(sizeof(Job));
            job->type = buildHistogram;
            job->parameters = malloc(sizeof(args));
            job->parameters->arg1 = rel;
            job->parameters->arg2 = &(startIndexThreads[allProcesses-1]);
            job->parameters->arg3 = &(endIndexThreads[allProcesses-1]);
            job->parameters->arg4 = partitionStepThreads;
            job->parameters->arg5 = NThreads;
            job->parameters->arg6 = &(maxThreads[allProcesses-1]);
            job->parameters->arg7 = &(histThreads[allProcesses-1]);
            job->parameters->arg8 = condVarMutex;
            PushJob(jobList, job);
            
            while (countProcesses<allProcesses) {
                pthread_cond_wait(condVar, condVarMutex);
                countProcesses++;
            }
            pthread_mutex_unlock(condVarMutex);

            pthread_cond_destroy(condVar);
            pthread_mutex_destroy(condVarMutex);
            
            max=maxThreads[0];
            for(int i=1;i<allProcesses;i++){
                if(maxThreads[i]>max){
                    max=maxThreads[i];
                }
            }

            hist=malloc((int)pow(2, N) * sizeof(int));
            for (int i=0;i<pow(2,N);i++)
                hist[i]=0;
            
            for(int i=0;i<allProcesses;i++){
                for(int j=0;j<pow(2,N);j++){
                    hist[j] += histThreads[i][j];
                }
                free(histThreads[i]);
            }
            free(histThreads);
            free(startIndexThreads);
            free(endIndexThreads);
            free(partitionStepThreads);
            free(NThreads);
            free(maxThreads);
        }
        else
            Partition(*rel, 0, rel->num_tuples -1, 1, N, &max, &hist);
        
        BuildReorderedFromPSum(*rel, pSum, 0, rel->num_tuples -1, 1, N, reOrdered, hist);
        free(hist);
        
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
                     if(jobList!=NULL){
                         if (numberOfThreads <= (*pSum)[j + 1] - (*pSum)[j])
                             allProcesses = numberOfThreads;
                         else
                             allProcesses = (*pSum)[j + 1] - (*pSum)[j];
                         pthread_mutex_init(condVarMutex, NULL);
                         pthread_cond_init(condVar, NULL);

                         countProcesses = 0;
                         pthread_mutex_lock(condVarMutex);
                         
                         histThreads = malloc(allProcesses*sizeof(int*));
                         completedTuples = 0;
                         maxThreads=malloc(allProcesses*sizeof(int));
                         startIndexThreads=malloc(allProcesses*sizeof(int));
                         endIndexThreads=malloc(allProcesses*sizeof(int));
                         partitionStepThreads=malloc(sizeof(int));
                         (*partitionStepThreads)=2;
                         NThreads=malloc(sizeof(int));
                         (*NThreads)=N;
                         for(int i=0;i<allProcesses-1;i++){
                             maxThreads[i]=0;
                             startIndexThreads[i]=i*((int)(((*pSum)[j + 1] - (*pSum)[j])/allProcesses))+(*pSum)[j];
                             endIndexThreads[i]=((i+1)*((int)(((*pSum)[j + 1] - (*pSum)[j])/ allProcesses)))-1+(*pSum)[j];
                             Job* job = malloc(sizeof(Job));
                             job->type = buildHistogram;
                             job->parameters = malloc(sizeof(args));
                             job->parameters->arg1 = reOrdered;
                             job->parameters->arg2 = &(startIndexThreads[i]);
                             job->parameters->arg3 = &(endIndexThreads[i]);
                             job->parameters->arg4 = partitionStepThreads;
                             job->parameters->arg5 = NThreads;
                             job->parameters->arg6 = &(maxThreads[i]);
                             job->parameters->arg7 = &(histThreads[i]);
                             job->parameters->arg8 = condVarMutex;
                             PushJob(jobList, job);
                             
                             completedTuples+=(int)((*pSum)[j + 1] - (*pSum)[j])/allProcesses;
                         }
                         
                         maxThreads[allProcesses-1]=0;
                         startIndexThreads[allProcesses-1]=completedTuples+(*pSum)[j];
                         endIndexThreads[allProcesses-1]=(*pSum)[j + 1]-1;
                         Job* job = malloc(sizeof(Job));
                         job->type = buildHistogram;
                         job->parameters = malloc(sizeof(args));
                         job->parameters->arg1 = reOrdered;
                         job->parameters->arg2 = &(startIndexThreads[allProcesses-1]);
                         job->parameters->arg3 = &(endIndexThreads[allProcesses-1]);
                         job->parameters->arg4 = partitionStepThreads;
                         job->parameters->arg5 = NThreads;
                         job->parameters->arg6 = &(maxThreads[allProcesses-1]);
                         job->parameters->arg7 = &(histThreads[allProcesses-1]);
                         job->parameters->arg8 = condVarMutex;
                         PushJob(jobList, job);
                         
                         while (countProcesses<allProcesses) {
                             pthread_cond_wait(condVar, condVarMutex);
                             countProcesses++;
                         }
                         pthread_mutex_unlock(condVarMutex);

                         pthread_cond_destroy(condVar);
                         pthread_mutex_destroy(condVarMutex);
                         
                         curMax=maxThreads[0];
                         for(int i=1;i<allProcesses;i++){
                             if(maxThreads[i]>curMax){
                                 curMax=maxThreads[i];
                             }
                         }
                         hist=malloc((int)pow(2, N) * sizeof(int));
                         for (int i=0;i<pow(2,N);i++)
                             hist[i]=0;
                         
                         for(int i=0;i<allProcesses;i++){
                             for(int j=0;j<pow(2,N);j++){
                                 hist[j] += histThreads[i][j];
                             }
                             free(histThreads[i]);
                         }
                         free(histThreads);
                         free(startIndexThreads);
                         free(endIndexThreads);
                         free(partitionStepThreads);
                         free(NThreads);
                         free(maxThreads);
                     }
                     else
                         Partition(*reOrdered, (*pSum)[j], (*pSum)[j + 1] -1, 2, N, &curMax, &hist);
                     
                    BuildReorderedFromPSum(*reOrdered, &pSumSecStep, (*pSum)[j], (*pSum)[j + 1] -1, 2, N, reOrderedSecStep, hist);
                    free(hist);
                    
                    if(curMax > max) max = curMax;
                }else{
                    pSumSecStep = calloc(pow(2,N), sizeof(int));
                }
                pSumFinal[j] = pSumSecStep;
                j++;
            }

            if((*pSum)[j] < rel->num_tuples){
                // printf("IM HERE %d %d\n", (*pSum)[j], rel->num_tuples);
                if(jobList!=NULL){
                    if (numberOfThreads <= reOrdered->num_tuples - (*pSum)[j])
                        allProcesses = numberOfThreads;
                    else
                        allProcesses = reOrdered->num_tuples - (*pSum)[j];
                    pthread_mutex_init(condVarMutex, NULL);
                    pthread_cond_init(condVar, NULL);

                    countProcesses = 0;
                    pthread_mutex_lock(condVarMutex);
                    
                    histThreads = malloc(allProcesses*sizeof(int*));
                    completedTuples = 0;
                    maxThreads=malloc(allProcesses*sizeof(int));
                    startIndexThreads=malloc(allProcesses*sizeof(int));
                    endIndexThreads=malloc(allProcesses*sizeof(int));
                    partitionStepThreads=malloc(sizeof(int));
                    (*partitionStepThreads)=2;
                    NThreads=malloc(sizeof(int));
                    (*NThreads)=N;
                    for(int i=0;i<allProcesses-1;i++){
                        maxThreads[i]=0;
                        startIndexThreads[i]=i*((int)((reOrdered->num_tuples - (*pSum)[j])/allProcesses))+(*pSum)[j];
                        endIndexThreads[i]=((i+1)*((int)((reOrdered->num_tuples - (*pSum)[j])/ allProcesses)))-1+(*pSum)[j];
                        Job* job = malloc(sizeof(Job));
                        job->type = buildHistogram;
                        job->parameters = malloc(sizeof(args));
                        job->parameters->arg1 = reOrdered;
                        job->parameters->arg2 = &(startIndexThreads[i]);
                        job->parameters->arg3 = &(endIndexThreads[i]);
                        job->parameters->arg4 = partitionStepThreads;
                        job->parameters->arg5 = NThreads;
                        job->parameters->arg6 = &(maxThreads[i]);
                        job->parameters->arg7 = &(histThreads[i]);
                        job->parameters->arg8 = condVarMutex;
                        PushJob(jobList, job);
                        
                        completedTuples+=(int)(reOrdered->num_tuples - (*pSum)[j])/allProcesses;
                    }
                    
                    maxThreads[allProcesses-1]=0;
                    startIndexThreads[allProcesses-1]=completedTuples+(*pSum)[j];
                    endIndexThreads[allProcesses-1]=reOrdered->num_tuples-1;
                    Job* job = malloc(sizeof(Job));
                    job->type = buildHistogram;
                    job->parameters = malloc(sizeof(args));
                    job->parameters->arg1 = reOrdered;
                    job->parameters->arg2 = &(startIndexThreads[allProcesses-1]);
                    job->parameters->arg3 = &(endIndexThreads[allProcesses-1]);
                    job->parameters->arg4 = partitionStepThreads;
                    job->parameters->arg5 = NThreads;
                    job->parameters->arg6 = &(maxThreads[allProcesses-1]);
                    job->parameters->arg7 = &(histThreads[allProcesses-1]);
                    job->parameters->arg8 = condVarMutex;
                    PushJob(jobList, job);
                    
                    while (countProcesses<allProcesses) {
                        pthread_cond_wait(condVar, condVarMutex);
                        countProcesses++;
                    }
                    pthread_mutex_unlock(condVarMutex);

                    pthread_cond_destroy(condVar);
                    pthread_mutex_destroy(condVarMutex);
                    
                    curMax=maxThreads[0];
                    for(int i=1;i<allProcesses;i++){
                        if(maxThreads[i]>curMax){
                            curMax=maxThreads[i];
                        }
                    }
                    hist=malloc((int)pow(2, N) * sizeof(int));
                    for (int i=0;i<pow(2,N);i++)
                        hist[i]=0;
                    
                    for(int i=0;i<allProcesses;i++){
                        for(int j=0;j<pow(2,N);j++){
                            hist[j] += histThreads[i][j];
                        }
                        free(histThreads[i]);
                    }
                    free(histThreads);
                    free(startIndexThreads);
                    free(endIndexThreads);
                    free(partitionStepThreads);
                    free(NThreads);
                    free(maxThreads);
                }
                else
                    Partition(*reOrdered, (*pSum)[j], reOrdered->num_tuples -1, 2, N, &curMax, &hist);
                
                BuildReorderedFromPSum(*reOrdered, &pSumSecStep, (*pSum)[j], reOrdered->num_tuples -1, 2, N, reOrderedSecStep, hist);
                free(hist);
            
                pSumFinal[j] = pSumSecStep;
            }else{
                pSumSecStep = calloc(pow(2,N), sizeof(int));
                pSumFinal[j] = pSumSecStep;
            }
            (*step)++;
        }
    }
    if(jobList!=NULL){
        free(condVar);
        free(condVarMutex);
    }
}
