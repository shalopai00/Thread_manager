//
// Created by michael on 03.04.2026.
//
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
    // Защита от NULL
    if (!q) {
        printf("push_to_queue: queue is NULL\n");
        return;
    }

    // Защита от переполнения
    if (q->size >= q->capacity) {
        printf("push_to_queue: queue is full (size=%d, capacity=%d)\n",
               q->size, q->capacity);
        return;
    }

    // Защита от некорректной функции
    if (!func) {
        printf("push_to_queue: function is NULL\n");
        return;
    }

    q->functions[q->size] = func;
    q->size++;

    printf("push_to_queue: added, size=%d/%d\n", q->size, q->capacity);
}

TaskFunc pop_from_queue(queue* q) {
    // Защита от NULL
    if (!q) {
        printf("pop_from_queue: queue is NULL\n");
        return NULL;
    }

    // Защита от пустой очереди
    if (q->size <= 0) {
        printf("pop_from_queue: queue is empty (size=%d)\n", q->size);
        return NULL;
    }

    // Защита от выхода за границы
    if (q->size > q->capacity) {
        printf("pop_from_queue: FATAL - size %d > capacity %d\n",
               q->size, q->capacity);
        return NULL;
    }

    // Берём первую функцию
    TaskFunc func = q->functions[0];

    // Сдвигаем элементы (только если есть что сдвигать)
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