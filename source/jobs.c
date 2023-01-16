#include "../headers/jobs.h"
#include "../headers/functions.h"
#include "../headers/hopscotch.h"
#include "../headers/mainPartitionTest.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>

void InitializeJobList(JobList** jobList){
    *jobList=malloc(sizeof(JobList));
    (*jobList)->editSem = malloc(sizeof(sem_t));
    (*jobList)->jobsCount = malloc(sizeof(sem_t));
    sem_init((*jobList)->editSem, 0, 1);
    sem_init((*jobList)->jobsCount, 0, 0);
    (*jobList)->Head = NULL;
    (*jobList)->Last = NULL;
}

void PushJob(JobList* jobList, Job* job){
    JobListElement* jobListElement;
    
    sem_wait(jobList->editSem);
    jobListElement=malloc(sizeof(JobListElement));
    jobListElement->job=job;
    if (jobList->Head==NULL){
        jobList->Head=jobListElement;
        jobList->Last=jobListElement;
        jobList->Head->prev=NULL;
        jobList->Head->next=NULL;
    }
    else{
        jobList->Last->next=jobListElement;
        jobListElement->prev=jobList->Last;
        jobListElement->next=NULL;
        jobList->Last=jobListElement;
    }
    sem_post(jobList->jobsCount);
    sem_post(jobList->editSem);
}

Job* PullJob(JobList* jobList){
    Job* job;
    
    sem_wait(jobList->jobsCount);
    sem_wait(jobList->editSem);
    if(jobList->Head != NULL){
        if (jobList->Head->next==NULL){
            printf("Only one job in list\n");
            job=jobList->Head->job;
            free(jobList->Head);
            jobList->Head=NULL;
            jobList->Last=NULL;
        }
        else{
            jobList->Head=jobList->Head->next;
            job=jobList->Head->prev->job;
            free(jobList->Head->prev);
            jobList->Head->prev=NULL;
        }
    }else{
        return NULL;
    }
    sem_post(jobList->editSem);
    return job;
}

void DestroyJobList(JobList* jobList){
    sem_destroy(jobList->editSem);
    sem_destroy(jobList->jobsCount);
    free(jobList);
}

void JobExecute(Job* job){
    if (job->type == loadTable){
        // function(job->parameters);
        LoadTable((char*)job->parameters->arg1,(Table*)(job->parameters->arg2));
    }
    else if (job->type == barrier){
        pthread_barrier_wait(((pthread_barrier_t*)job->parameters->arg1));
    }
    else if (job->type == insertHopScotch) {
        insert((hop*)job->parameters->arg1, *((tuple*)job->parameters->arg2), (pthread_mutex_t*)job->parameters->arg4, 
            (pthread_mutex_t*)job->parameters->arg5, (int*)job->parameters->arg6);
        pthread_cond_signal(job->parameters->arg3);
    }
    else if (job->type == buildHistogram) {
        Partition(*((struct relation*)job->parameters->arg1), *((int*)job->parameters->arg2), *((int*)job->parameters->arg3), *((int*)job->parameters->arg4), *((int*)job->parameters->arg5), (int*)job->parameters->arg6, (int**)job->parameters->arg7);
        pthread_cond_signal(job->parameters->arg8);
    }
    free(job->parameters);
    free(job);
}
