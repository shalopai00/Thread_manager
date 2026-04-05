#include "thread_manager.h"
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "task.h"


typedef struct TaskForWorker {
    pthread_t target_thread;
    TaskFunc task;
    int has_task;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} TaskForWorker;

TaskForWorker* worker_tasks = NULL;
ThreadManager* global_tm = NULL;


void give_task_to_worker(pthread_t thread_id) {
    printf("Scheduler: giving task to thread %lu\n", thread_id);


    TaskFunc task = pop_from_queue(global_tm->q);

    if (task) {

        for (int i = 0; i < worker_tasks_count; i++) {
            if (worker_tasks[i].target_thread == thread_id) {
                pthread_mutex_lock(&worker_tasks[i].mutex);
                worker_tasks[i].task = task;
                worker_tasks[i].has_task = 1;
                pthread_cond_signal(&worker_tasks[i].cond);
                pthread_mutex_unlock(&worker_tasks[i].mutex);
                printf("Scheduler: task assigned to thread %lu\n", thread_id);
                break;
            }
        }
    } else {
        printf("Scheduler: no tasks available for thread %lu\n", thread_id);
    }
}


void handle_request(pthread_t thread_id) {
    printf("Scheduler: processing request from thread %lu\n", thread_id);
    give_task_to_worker(thread_id);
}

// Планировщик - обрабатывает запросы из q1
void* scheduler(void* arg) {
    pthread_setname_np(pthread_self(), "scheduler");
    ThreadManager* tm = (ThreadManager*)arg;
    global_tm = tm;  // сохраняем для доступа в give_task_to_worker

    printf("Scheduler started\n");

    while (tm->is_running) {
        pthread_mutex_lock(&tm->mutex);


        while (tm->is_running && (!has_requests(tm->q1) || tm->q->size == 0)) {
            pthread_cond_wait(&tm->cond1, &tm->mutex);
        }

        if (!tm->is_running) {
            pthread_mutex_unlock(&tm->mutex);
            break;
        }


        pthread_t thread_id;
        Request req = pop_request(tm->q1, &thread_id);

        pthread_mutex_unlock(&tm->mutex);


        if (req) {
            req(thread_id);
        }
    }

    printf("Scheduler stopped\n");
    return NULL;
}


void* worker(void* arg) {
    pthread_setname_np(pthread_self(), "worker");
    ThreadManager* tm = (ThreadManager*)arg;
    pthread_t my_id = pthread_self();


    int my_index = -1;
    for (int i = 0; i < worker_tasks_count; i++) {
        if (worker_tasks[i].target_thread == my_id) {
            my_index = i;
            break;
        }
    }

    printf("Worker %lu: started\n", my_id);

    while (tm->is_running) {

        pthread_mutex_lock(&tm->mutex);
        push_request(tm->q1, handle_request, my_id);
        pthread_cond_signal(&tm->cond1);
        pthread_mutex_unlock(&tm->mutex);

        printf("Worker %lu: request sent, waiting for task...\n", my_id);


        pthread_mutex_lock(&worker_tasks[my_index].mutex);
        while (tm->is_running && !worker_tasks[my_index].has_task) {
            pthread_cond_wait(&worker_tasks[my_index].cond, &worker_tasks[my_index].mutex);
        }

        if (!tm->is_running) {
            pthread_mutex_unlock(&worker_tasks[my_index].mutex);
            break;
        }


        TaskFunc task = worker_tasks[my_index].task;
        worker_tasks[my_index].has_task = 0;
        pthread_mutex_unlock(&worker_tasks[my_index].mutex);


        if (task) {
            printf("Worker %lu: executing task\n", my_id);
            task();  // ← воркер сам выполняет task()
            printf("Worker %lu: task completed\n", my_id);
        }
    }

    printf("Worker %lu: stopped\n", my_id);
    return NULL;
}

ThreadManager* create_thread_manager(int num_threads, int queue_capacity) {
    ThreadManager* tm = (ThreadManager*)malloc(sizeof(ThreadManager));
    tm->q = create_queue(queue_capacity);
    tm->q1 = create_request_queue(queue_capacity);
    tm->threads = (pthread_t*)malloc(sizeof(pthread_t) * num_threads);
    tm->thread_count = num_threads;
    tm->is_running = 0;

    pthread_mutex_init(&tm->mutex, NULL);
    pthread_cond_init(&tm->cond, NULL);
    pthread_cond_init(&tm->cond1, NULL);


    worker_tasks_count = num_threads;
    worker_tasks = (TaskForWorker*)malloc(sizeof(TaskForWorker) * num_threads);
    for (int i = 0; i < num_threads; i++) {
        pthread_mutex_init(&worker_tasks[i].mutex, NULL);
        pthread_cond_init(&worker_tasks[i].cond, NULL);
        worker_tasks[i].has_task = 0;
        worker_tasks[i].task = NULL;
        worker_tasks[i].target_thread = 0;
    }

    return tm;
}

void start_manager(ThreadManager *tm) {
    tm->is_running = 1;


    for (int i = 0; i < tm->thread_count; i++) {
        pthread_create(&tm->threads[i], NULL, worker, tm);
        worker_tasks[i].target_thread = tm->threads[i];
    }


    pthread_t scheduler_thread;
    pthread_create(&scheduler_thread, NULL, scheduler, tm);
    pthread_detach(scheduler_thread);

    printf("Manager started with %d threads\n", tm->thread_count);
}

void add_task(ThreadManager *tm, TaskFunc func) {
    pthread_mutex_lock(&tm->mutex);
    push_to_queue(tm->q, func);
    pthread_cond_signal(&tm->cond);
    pthread_cond_signal(&tm->cond1);
    pthread_mutex_unlock(&tm->mutex);
}

void stop_manager(ThreadManager *tm) {
    tm->is_running = 0;


    pthread_mutex_lock(&tm->mutex);
    pthread_cond_broadcast(&tm->cond);
    pthread_cond_broadcast(&tm->cond1);
    pthread_mutex_unlock(&tm->mutex);


    for (int i = 0; i < worker_tasks_count; i++) {
        pthread_mutex_lock(&worker_tasks[i].mutex);
        worker_tasks[i].has_task = 1;  // чтобы вышли из ожидания
        pthread_cond_signal(&worker_tasks[i].cond);
        pthread_mutex_unlock(&worker_tasks[i].mutex);
    }

    for (int i = 0; i < tm->thread_count; i++) {
        pthread_join(tm->threads[i], NULL);
    }

    printf("Manager stopped\n");
}

void destroy_manager(ThreadManager *tm) {
    destroy_queue(tm->q);
    destroy_request_queue(tm->q1);
    free(tm->threads);


    for (int i = 0; i < worker_tasks_count; i++) {
        pthread_mutex_destroy(&worker_tasks[i].mutex);
        pthread_cond_destroy(&worker_tasks[i].cond);
    }
    free(worker_tasks);

    pthread_mutex_destroy(&tm->mutex);
    pthread_cond_destroy(&tm->cond);
    pthread_cond_destroy(&tm->cond1);
    free(tm);
}