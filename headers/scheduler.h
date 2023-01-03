#ifndef scheduler_h
#define scheduler_h

void InitializeMultiThread(JobList** jobList, pthread_t*** threads);

void ThreadStart(JobList* jobList);

void DestroyMultiThread(JobList* jobList, pthread_t** threads);

#endif /* scheduler_h */
