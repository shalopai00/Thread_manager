
#ifndef OPENMP_FOR_LABA5_QUEUE_H
#define OPENMP_FOR_LABA5_QUEUE_H

typedef void (*TaskFunc)();

typedef struct queue {
    TaskFunc* functions;
    int size;
    int capacity;

}queue;

queue* create_queue(int capacity);
void push_to_queue(queue* q, TaskFunc func);
TaskFunc pop_from_queue(queue* q);
void destroy_queue(queue* q);


#endif //OPENMP_FOR_LABA5_QUEUE_H