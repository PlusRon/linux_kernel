#include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <pthread.h>
#include <windows.h> // #include <sys/syscall.h>
#include "../include/thread_work_function.h"


int main(void){
    printf("Hello, Linux Kernel\n\n");
    

    pthread_t thread_process[2];
    char t_name[2][16];

    // Thread 1
    strncpy(t_name[0], "Thread 1", sizeof(t_name[0]) - 1);
    t_name[0][sizeof(t_name[0]) - 1] = '\0';
    // Create Thread 1
    if(pthread_create(&thread_process[0], NULL, function_1, (void*)t_name[0]) != 0){
        perror("pthread_create Thread 1");
        return 1;
    }

    // Thread 2
    strncpy(t_name[1], "Thread 2", sizeof(t_name[1]) - 1);
    t_name[1][sizeof(t_name[1]) - 1] = '\0';
    // Create Thread 2
    if(pthread_create(&thread_process[1], NULL, function_2, (void*)t_name[1]) != 0){
        perror("pthread_create Thread 2");
        return 1;
    }
    

    // main Thread
    DWORD main_t_id = GetCurrentThreadId();
    thread_data.thread_id = main_t_id;

    strncpy(thread_data.thread_name, "Main", sizeof(thread_data.thread_name) - 1);
    thread_data.thread_name[sizeof(thread_data.thread_name) - 1] = '\0';

    unsigned long* main_dynamic = (unsigned long*)malloc(sizeof(unsigned long)); // perror : malloc(SIZE_MAX)
    if(main_dynamic == NULL){
        perror("malloc main_dynamic");
        return 1;
    }
    *main_dynamic = main_t_id;

    // mutex
    pthread_mutex_lock(&print_lock);
    // critical section --
    printf("I am 'Main Thread with ID : %lu' executing main().\n", thread_data.thread_id);
    segment_detail(thread_data.thread_id, main_dynamic);
    // -- critical section
    pthread_mutex_unlock(&print_lock);
    
    free(main_dynamic);

    // main() be asked to wait the Thread1, Thread2 doing finished.
    if(pthread_join(thread_process[0], NULL) != 0){
        perror("pthread_join thread 1");
        return 1;
    }
    if(pthread_join(thread_process[1], NULL) != 0){
        perror("pthread_join thread 2");
        return 1;
    }


    return 0;
}