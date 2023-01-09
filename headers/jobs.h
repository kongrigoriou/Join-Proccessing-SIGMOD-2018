#ifndef jobs_h
#define jobs_h
#include <semaphore.h>

typedef enum JobType{
    terminate,loadTable //++++ gia tis douleies
}JobType;

typedef struct Job{
    JobType type;
    void* parameters;
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
