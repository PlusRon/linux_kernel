#ifndef THREAD_TLS
#define THREAD_TLS

struct thread_TLS {
    int thread_id;
    char thread_name[16];
};

typedef struct thread_TLS thread_local_data;


#endif // THREAD_TLS