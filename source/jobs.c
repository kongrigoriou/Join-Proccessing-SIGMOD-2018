#include "../headers/jobs.h"
#include "../headers/functions.h"
#include "../headers/hopscotch.h"
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
    // printf("After semaphoes\n");
    if(jobList->Head != NULL){
        if (jobList->Head->next==NULL){
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

    free(jobList->Head);
    free(jobList->Last);
    free(jobList->editSem);
    free(jobList->jobsCount);
    free(jobList);
}

void JobExecute(Job* job){
    if (job->type == loadTable){
        // function(job->parameters);
        LoadTable((char*)job->parameters->arg1,(Table*)(job->parameters->arg2));
    }
    else if (job->type == barrier){
        pthread_barrier_wait(((pthread_barrier_t*)job->parameters->arg1));
    }else if(job->type == probing){
        Probing((hop*)job->parameters->arg1, (relation*)job->parameters->arg2, (int*)job->parameters->arg3, 
        (int*)job->parameters->arg4, (List**)job->parameters->arg5);
    }
    // else if (job->type == numOfPartitions) {
    //     num_of_partitions((relation*)job->parameters->arg1, (relation*)job->parameters->arg2, (int**)job->parameters->arg3, (relation*)job->parameters->arg4, (int**)job->parameters->arg5, (int*)job->parameters->arg6);
    //     pthread_cond_signal(job->parameters->arg7);
    // }
    // else if (job->type == insertHopScotch) {
    //     insert((hop*)job->parameters->arg1, *((tuple*)job->parameters->arg2), (pthread_mutex_t*)job->parameters->arg4, 
    //         (pthread_mutex_t*)job->parameters->arg5, (int*)job->parameters->arg6);
    //     pthread_cond_signal(job->parameters->arg3);
    // }
    free(job->parameters);
    free(job);
}
