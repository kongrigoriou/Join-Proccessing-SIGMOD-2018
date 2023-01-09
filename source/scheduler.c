#include "../headers/scheduler.h"
#include "../headers/jobs.h"

void InitializeMultiThread(JobList** jobList, pthread_t*** threads, int numOfThreads){
    *threads=malloc(numOfThreads*sizeof(pthread_t*));
    InitializeJobList(jobList);
    for(i=0;i<numOfThreads;i++){
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

void DestroyMultiThread(JobList* jobList, pthread_t** threads, int numOfThreads){
    Job* job;
    for (int i=0;i<numOfThreads;i++){
        job=malloc(sizeof(Job));
        job->type=terminate;
        job->parameters=NULL;
        PushJob(jobList, job);
    }
    for(i=0;i<numOfThreads;i++){
        pthread_join(*(threads[i]), (void**)NULL);
    }
    free(threads);
    DestroyJobList(jobList);
}
