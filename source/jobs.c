#include "../headers/jobs.h"

void InitializeJobList(JobList** jobList){
    *jobList=malloc(sizeof(JobList));
    sem_init(jobList->editSem, 0, 1);
    sem_init(jobList->jobsCount, 0, 0);
    jobList->Head=NULL;
    jobList->Last=NULL;
}

void PushJob(JobList* jobList, Job* job){
    JobListElement* jobListElement;
    
    sem_wait(jobList->editSem);
    jobListElement=malloc(sizeof(JobListElement));
    jobListElement->job=job;
    if (jobList->Head==NULL){
        jobList->Head=jobListElement;
        jobList->Last=jobListElement;
        jobListElement->prev=NULL;
        jobListElement->next=NULL;
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
    sem_post(jobList->editSem);
    return job;
}

void DestroyJobList(JobList* jobList){
    sem_destroy(jobList->editSem);
    sem_destroy(jobList->jobsCount);
    free(jobList);
}
