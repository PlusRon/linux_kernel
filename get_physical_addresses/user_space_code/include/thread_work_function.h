#ifndef THREAD_WORK_FUNCTION
#define THREAD_WORK_FUNCTION

#include <semaphore.h>
#include "../include/thread_TLS.h"
extern sem_t sem;
extern void init_semaphore();

// work function call in the thread function
extern void segment_detail(int thread_id, int* thread_dynamic, thread_local_data thread_data);

// thread function for pthread_create()
extern void* function_1(void* arg);
extern void* function_2(void* arg);




#endif // THREAD_WORK_FUNCTION