#ifndef jobs_h
#define jobs_h
#include <semaphore.h>

typedef enum JobType{
    
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

#endif /* jobs_h */
