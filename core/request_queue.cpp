#include "request_queue.h"
#include <cstdio>
#include <cstdlib>

RequestQueue* create_request_queue(int capacity) {
    if (capacity <= 0) {
        printf("create_request_queue: invalid capacity %d\n", capacity);
        return NULL;
    }

    RequestQueue* q1 = (RequestQueue*)malloc(sizeof(RequestQueue));
    if (!q1) {
        printf("create_request_queue: failed to allocate queue\n");
        return NULL;
    }

    q1->requests = (Request*)malloc(capacity * sizeof(Request));
    if (!q1->requests) {
        printf("create_request_queue: failed to allocate requests array\n");
        free(q1);
        return NULL;
    }

    // Добавляем массив для ID потоков
    q1->thread_ids = (pthread_t*)malloc(capacity * sizeof(pthread_t));
    if (!q1->thread_ids) {
        printf("create_request_queue: failed to allocate thread_ids array\n");
        free(q1->requests);
        free(q1);
        return NULL;
    }

    q1->size_of_q1 = 0;
    q1->capacity_of_q1 = capacity;

    printf("create_request_queue: created with capacity %d\n", capacity);
    return q1;
}

void push_request(RequestQueue* q1, Request req, pthread_t thread_id) {
    if (!q1) {
        printf("push_request: queue is NULL\n");
        return;
    }

    if (q1->size_of_q1 >= q1->capacity_of_q1) {
        printf("push_request: queue is full (size=%d, capacity=%d)\n",
               q1->size_of_q1, q1->capacity_of_q1);
        return;
    }

    if (!req) {
        printf("push_request: request function is NULL\n");
        return;
    }

    q1->requests[q1->size_of_q1] = req;
    q1->thread_ids[q1->size_of_q1] = thread_id;
    q1->size_of_q1++;

    printf("push_request: added request from thread %lu, size=%d/%d\n",
           thread_id, q1->size_of_q1, q1->capacity_of_q1);
}

Request pop_request(RequestQueue* q1, pthread_t* thread_id) {
    if (!q1) {
        printf("pop_request: queue is NULL\n");
        return NULL;
    }

    if (q1->size_of_q1 <= 0) {
        printf("pop_request: queue is empty (size=%d)\n", q1->size_of_q1);
        return NULL;
    }

    if (q1->size_of_q1 > q1->capacity_of_q1) {
        printf("pop_request: FATAL - size %d > capacity %d\n",
               q1->size_of_q1, q1->capacity_of_q1);
        return NULL;
    }

    Request req = q1->requests[0];
    *thread_id = q1->thread_ids[0];

    // Сдвигаем элементы
    for (int i = 0; i < q1->size_of_q1 - 1; i++) {
        q1->requests[i] = q1->requests[i + 1];
        q1->thread_ids[i] = q1->thread_ids[i + 1];
    }

    q1->size_of_q1--;

    printf("pop_request: popped request from thread %lu, new size=%d\n",
           *thread_id, q1->size_of_q1);
    return req;
}

int has_requests(RequestQueue* q1) {
    return q1 && q1->size_of_q1 > 0;
}

void destroy_request_queue(RequestQueue* q1) {
    if (q1) {
        if (q1->requests) free(q1->requests);
        if (q1->thread_ids) free(q1->thread_ids);
        free(q1);
        printf("destroy_request_queue: queue destroyed\n");
    }
}