#include "Queue.h"
#include <cstdio>
#include <cstdlib>

queue* create_queue(int capacity) {
    if (capacity <= 0) {
        printf("create_queue: invalid capacity %d\n", capacity);
        return NULL;
    }

    queue* q = (queue*)malloc(sizeof(queue));
    if (!q) {
        printf("create_queue: failed to allocate queue\n");
        return NULL;
    }

    q->functions = (TaskFunc*)malloc(capacity * sizeof(TaskFunc));
    if (!q->functions) {
        printf("create_queue: failed to allocate functions array\n");
        free(q);
        return NULL;
    }

    q->size = 0;
    q->capacity = capacity;

    printf("create_queue: created with capacity %d\n", capacity);
    return q;
}

void push_to_queue(queue* q, TaskFunc func) {

    if (!q) {
        printf("push_to_queue: queue is NULL\n");
        return;
    }


    if (q->size >= q->capacity) {
        printf("push_to_queue: queue is full (size=%d, capacity=%d)\n",
               q->size, q->capacity);
        return;
    }


    if (!func) {
        printf("push_to_queue: function is NULL\n");
        return;
    }

    q->functions[q->size] = func;
    q->size++;

    printf("push_to_queue: added, size=%d/%d\n", q->size, q->capacity);
}

TaskFunc pop_from_queue(queue* q) {

    if (!q) {
        printf("pop_from_queue: queue is NULL\n");
        return NULL;
    }


    if (q->size <= 0) {
        printf("pop_from_queue: queue is empty (size=%d)\n", q->size);
        return NULL;
    }


    if (q->size > q->capacity) {
        printf("pop_from_queue: FATAL - size %d > capacity %d\n",
               q->size, q->capacity);
        return NULL;
    }


    TaskFunc func = q->functions[0];


    for (int i = 0; i < q->size - 1; i++) {
        q->functions[i] = q->functions[i + 1];
    }

    q->size--;

    printf("pop_from_queue: success, new size=%d\n", q->size);
    return func;
}
void destroy_queue(queue* q) {
    if (q) {
        free(q->functions);
        free(q);
    }
}