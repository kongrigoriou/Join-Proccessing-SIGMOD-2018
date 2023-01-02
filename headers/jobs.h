#ifndef jobs_h
#define jobs_h

typedef enum JobType{
    
}JobType;

struct Job{
    JobType type;
    void* parameters;
}

typedef struct JobListElement{
    Job* job;
    struct JobListElement *next;
    struct JobListElement *prev;
}JobListElement;

typedef struct JobList{
    JobListElement* Head;
    JobListElement* Last;
    sem_t *push_sem;
    sem_t *pull_sem;
    sem_t *num_of_jobs;            //counting semaphore
}JobList;

#endif /* jobs_h */
