#ifndef THREAD_TLS
#define THREAD_TLS

#include <pthread.h>

struct thread_TLS {
    int thread_id; // int thread_id;
    char thread_name[16];
};

typedef struct thread_TLS thread_local_segment;

extern __thread thread_local_segment thread_data;

#endif // THREAD_TLS