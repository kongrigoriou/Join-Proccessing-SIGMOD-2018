#ifndef jobs_h
#define jobs_h
#include <semaphore.h>
#include "../headers/structures.h"
#include <pthread.h>

// #include

typedef enum JobType{
    terminate,loadTable,barrier,numOfPartitions,insertHopScotch
}JobType;

typedef struct args{
    void* arg1;
    void* arg2;
    void* arg3;
    void* arg4;
    void* arg5;
    void* arg6;
    void* arg7;
}args;

typedef struct Job{
    JobType type;
    args* parameters;
}Job;

typedef struct JobListElement{
    Job* job;
    struct JobListElement *next;
    struct JobListElement *prev;
}JobListElement;

typedef struct JobList{
    JobListElement* Head;
    JobListElement* Last;
    sem_t *editSem;
    sem_t *jobsCount;            //counting semaphore
}JobList;


void InitializeJobList(JobList** jobList);

void PushJob(JobList* jobList, Job* job);

Job* PullJob(JobList* jobList);

void DestroyJobList(JobList* jobList);

void JobExecute(Job* job);

#endif /* jobs_h */
