#ifndef THREAD_WORK_FUNCTION
#define THREAD_WORK_FUNCTION


#include "../include/thread_TLS.h"
#include <semaphore.h>
#include <pthread.h>

// semaphore
extern sem_t sem;
extern void init_semaphore();

// mutex
extern pthread_mutex_t print_lock;
extern void init_mutex();

// work function call in the thread function
extern void segment_detail(int thread_id, int* thread_dynamic);

// thread function for pthread_create()
extern void* function_1(void* arg);
extern void* function_2(void* arg);


void* get_main_address(void); 

#endif // THREAD_WORK_FUNCTION
