#ifndef scheduler_h
#define scheduler_h
#include "jobs.h"
#include <pthread.h>
#include <stdlib.h>

void InitializeMultiThread(JobList** jobList, pthread_t*** threads, int numOfThreads);

void* ThreadStart(void* jobList);

void DestroyMultiThread(JobList* jobList, pthread_t** threads, int numOfThreads);

#endif /* scheduler_h */
