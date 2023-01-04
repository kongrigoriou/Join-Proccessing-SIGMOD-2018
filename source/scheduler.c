#include "../headers/scheduler.h"
#include "../headers/jobs.h"
#define NUM_OF_THREADS 3

void InitializeMultiThread(JobList** jobList, pthread_t*** threads){
    *threads=malloc(NUM_OF_THREADS*sizeof(pthread_t*));
    InitializeJobList(jobList);
    for(i=0;i<NUM_OF_THREADS;i++){
        pthread_create(threads[i], NULL, ThreadStart, (void*)(*jobList));
    }
}

void ThreadStart(JobList* jobList){
    Job* job;
    while(1){
        job=PullJob(jobList);
        if(job->type==terminate)
            pthread_exit(NULL);
        else
            JobExecute(job);
    }
}

void DestroyMultiThread(JobList* jobList, pthread_t** threads){
    Job* job;
    for (int i=0;i<NUM_OF_THREADS;i++){
        job=malloc(sizeof(Job));
        job->type=terminate;
        job->parameters=NULL;
        PushJob(jobList, job);
    }
    for(i=0;i<NUM_OF_THREADS;i++){
        pthread_join(*(threads[i]), (void**)NULL);
    }
    free(threads);
    DestroyJobList(jobList);
}
