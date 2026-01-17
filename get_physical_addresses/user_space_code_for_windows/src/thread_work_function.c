#include "../include/thread_work_function.h"
#include "../include/user_get_physical_addresses.h"
#include "../include/thread_TLS.h"
#include <stdio.h>
#include <windows.h>


// global variable with initial (.data)
int init_global_variable = 123;

// non_initial global variable (.bss)
int non_init_global_variable;

// thread local storage (TLS)
__thread thread_local_segment thread_data;

// define semaphore variable
sem_t sem;
// initailize the semaphore variable
void init_semaphore(){
    sem_init(&sem, 0, 1);
}

// define mutex variable
pthread_mutex_t print_lock;
// initailize the mutex variable
void init_mutex(){
    // pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_init(&print_lock, NULL);
}

void segment_detail(DWORD thread_id, unsigned long* thread_dynamic){
    // use semaphore to simulate mutex_lock
    // sem_wait(&sem);
    // pthread_mutex_lock(&print_lock);

    
    // local variable (stack)
    int local_variable = 10;
    local_variable = local_variable + (int)thread_id;

    // dynamic variable (heap)
    int* dynamic_variable = (int*)malloc(sizeof(int));
    if(dynamic_variable == NULL){
        perror("molloc dynamic_variable");
        return 1;
    }
    *dynamic_variable = local_variable;

    // global variable with initial (.data)
    printf("In thread %lu \n", thread_id);
    printf("the value of 'init_global_variable' is %d, the offset of the logical address of 'init_global_variable' is %#jx, ", init_global_variable, &init_global_variable);
    printf("the physical address of 'init_global_variable' is %#jx\n\n", user_get_physical_addresses(&init_global_variable) );
    
    // non_initial global variable (.bss)
    printf("the value of 'non_init_global_variable' is %d, the offset of the logical address of 'non_init_global_variable' is %#jx, ",  non_init_global_variable, &non_init_global_variable);
    printf("the physical address of 'non_init_global_variable' is %#jx\n\n", user_get_physical_addresses(&non_init_global_variable) );
    
    // local variable (stack)
    printf("the value of 'local_variable' is %d, the offset of the logical address of 'local_variable' is %#jx, ",  local_variable, &local_variable);
    printf("the physical address of 'local_variable' is %#jx\n\n", user_get_physical_addresses(&local_variable) );

    // thread_dynamic (heap)
    printf("the value of 'thread_dynamic' is %lu, the offset of the logical address of 'thread_dynamic' is %#jx, ",  *thread_dynamic, thread_dynamic);
    printf("the physical address of 'thread_dynamic' is %#jx\n\n", user_get_physical_addresses(thread_dynamic) );

    // dynamic variable (heap)
    printf("the value of 'dynamic_variable' is %d, the offset of the logical address of 'dynamic_variable' is %#jx, ",  *dynamic_variable, dynamic_variable);
    printf("the physical address of 'dynamic_variable' is %#jx\n\n", user_get_physical_addresses(dynamic_variable) );

    // thread local variable
    printf("the value of 'thread_data' are thread_id = %d and thread_name = %s, the offset of the logical address of 'thread_data' is %#jx, ",  thread_data.thread_id, thread_data.thread_name, &thread_data);
    printf("the physical address of 'thread_data' is %#jx\n\n", user_get_physical_addresses(&thread_data) );

    // function
    printf("the offset of the logical address of function 'segment_detail()' is %#jx, ", segment_detail);
    printf("the physical address of function 'segment_detail()' is %#jx\n\n", user_get_physical_addresses(segment_detail));
    printf("the offset of the logical address of function 'function_1()' is %#jx, ", function_1);
    printf("the physical address of function 'function_1()' is %#jx\n\n", user_get_physical_addresses(function_1));
    printf("the offset of the logical address of function 'function_2()' is %#jx, ", function_2);
    printf("the physical address of function 'function_2()' is %#jx\n\n", user_get_physical_addresses(function_2));
    // library function
    printf("the offset of the logical address of 'library function printf' is %#jx, ", printf);
    printf("the physical address of 'library function printf' is %#jx\n\n", user_get_physical_addresses(printf));
    printf("====================================================================================================================\n");
    
    
    free(dynamic_variable);

    // use semaphore to simulate mutex_lock
    // sem_post(&sem);
    // pthread_mutex_unlock(&print_lock);

}


void* function_1(void* arg){
    // thread ID
    DWORD tid = GetCurrentThreadId(); // int pid = syscall(__NR_gettid);
    thread_data.thread_id = tid;
    
    // thread name
    char* thrd_name = (char*)arg;
    strncpy(thread_data.thread_name, thrd_name, sizeof(thread_data.thread_name) - 1); // copy first 15 bit (0~14)
    thread_data.thread_name[sizeof(thread_data.thread_name) - 1] = '\0'; // '\0' be writed into index-15 

    // (heap)
    unsigned long* thread_dynamic = (unsigned long*)malloc(sizeof(unsigned long));
    if(thread_dynamic == NULL){
        perror("malloc thread_dynamic");
        return 1;
    }
    *thread_dynamic = thread_data.thread_id;

    // mutex
    pthread_mutex_lock(&print_lock);
    // critical section --
    printf("I am 'Thread with ID : %lu' executing function_1().\n ", thread_data.thread_id);
    segment_detail(thread_data.thread_id, thread_dynamic);
    // -- critical section
    pthread_mutex_unlock(&print_lock);

    // free heap block
    free(thread_dynamic);
}

void* function_2(void* arg){
    // thread ID
    DWORD tid = GetCurrentThreadId();
    thread_data.thread_id = tid;

    // thread name
    char* thrd_name = (char*)arg;
    strncpy(thread_data.thread_name, thrd_name, sizeof(thread_data.thread_name) - 1);
    thread_data.thread_name[sizeof(thread_data.thread_name) - 1] = '\0';

    unsigned long* thread_dynamic = (unsigned long*)malloc(sizeof(unsigned long));
    if(thread_dynamic == NULL){
        perror("malloc thread_dynamic");
        return 1;
    }
    *thread_dynamic = thread_data.thread_id;

    // mutex
    pthread_mutex_lock(&print_lock);
    // critical section --
    printf("I am 'Thread with ID : %lu' executing function_2().\n ", thread_data.thread_id);
    segment_detail(thread_data.thread_id, thread_dynamic);
    // -- critical section
    pthread_mutex_unlock(&print_lock);

    free(thread_dynamic);
}




