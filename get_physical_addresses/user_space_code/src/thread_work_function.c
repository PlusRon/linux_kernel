#include "../include/thread_work_function.h"
#include "../include/user_get_physical_addresses.h"
#include "../include/thread_TLS.h"

// global variable with initial (.data)
int init_global_variable = 123;

// non_initial global variable (.bss)
int non_init_global_variable;

// define semaphore variable
sem_t sem;
// initailize the semaphore variable
void init_semaphore(){
    sem_init(&sem, 0, 1);
}


void segment_detail(int thread_id, int* thread_dynamic, thread_local_data thread_data){
    // use semaphore to simulate mutex_lock
    sem_wait(&sem);
    
    // local variable (stack)
    int local_variable = 10;
    local_variable = local_variable + thread_id;

    // dynamic variable (heap)
    int* dynamic_variable = (int*)malloc(sizeof(int));
    dynamic_variable = local_variable;

    // global variable with initial (.data)
    printf("In thread %d \n", thread_id);
    printf("the value of 'init_global_variable' is %d, the offset of the logical address of 'init_global_variable' is %#jx, ", init_global_variable, &init_global_variable);
    printf("the physical address of 'init_global_variable' is %#jx\n\n", user_get_physical_addresses(&init_global_variable) );
    
    // non_initial global variable (.bss)
    printf("the value of 'non_init_global_variable' is %d, the offset of the logical address of 'non_init_global_variable' is %#jx, ",  non_init_global_variable, &non_init_global_variable);
    printf("the physical address of 'non_init_global_variable' is %#jx\n\n", user_get_physical_addresses(&non_init_global_variable) );
    
    // local variable (stack)
    printf("the value of 'local_variable' is %d, the offset of the logical address of 'local_variable' is %#jx, ",  local_variable, &local_variable);
    printf("the physical address of 'local_variable' is %#jx\n\n", user_get_physical_addresses(&local_variable) );

    // thread_dynamic (heap)
    printf("the value of 'thread_dynamic' is %d, the offset of the logical address of 'thread_dynamic' is %#jx, ",  *thread_dynamic, &thread_dynamic);
    printf("the physical address of 'thread_dynamic' is %#jx\n\n", user_get_physical_addresses(&thread_dynamic) );

    // dynamic variable (heap)
    printf("the value of 'dynamic_variable' is %d, the offset of the logical address of 'dynamic_variable' is %#jx, ",  *dynamic_variable, &dynamic_variable);
    printf("the physical address of 'dynamic_variable' is %#jx\n\n", user_get_physical_addresses(&dynamic_variable) );

    // thread local variable
    printf("the value of 'thread_data' are thread_id = %d and thread_name = %s, the offset of the logical address of 'thread_data' is %#jx, ",  thread_data.thread_id, thread_data.thread_name, &thread_data);
    printf("the physical address of 'thread_data' is %#jx\n\n", user_get_physical_addresses(&thread_data) );

    // function
    printf("the offset of the logical address of function 'segment_detail' is %#jx, ", segment_detail);
    printf("the physical address of function 'segment_detail' is %#jx\n", user_get_physical_addresses(segment_detail));

    // library function
    printf("the offset of the logical address of 'library function printf' is %#jx, ", printf);
    printf("the physical address of 'library function printf' is %#jx\n", user_get_physical_addresses(printf));
    printf("====================================================================================================================\n");

    // use semaphore to simulate mutex_lock
    sem_post(&sem);
}


// void* function_1(void* arg){

// }




