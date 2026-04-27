
#ifndef OPENMP_FOR_LABA5_THREAD_MANAGER_H
#define OPENMP_FOR_LABA5_THREAD_MANAGER_H
#include "Queue.h"
#include <pthread.h>
#include "request_queue.h"


typedef struct ThreadManager {
    queue* q;
    RequestQueue* q1;
    pthread_t* threads;
    int thread_count;
    int is_running;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    pthread_cond_t cond1;
}ThreadManager;

ThreadManager* create_thread_manager(int num_threads, int queue_capacity);
void start_manager(ThreadManager* tm);
void add_task(ThreadManager* tm, TaskFunc func);
void stop_manager(ThreadManager* tm);
void destroy_manager(ThreadManager* tm);

#endif //OPENMP_FOR_LABA5_THREAD_MANAGER_H