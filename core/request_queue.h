//
// Created by michael on 03.04.2026.
//

#ifndef OPENMP_FOR_LABA5_QUEUE_FOR_THREADS_H
#define OPENMP_FOR_LABA5_QUEUE_FOR_THREADS_H
#include <pthread.h>

typedef void(*Request)(pthread_t thread_id);


typedef struct RequestQueue{
  Request* requests;
  pthread_t* thread_ids;
  int size_of_q1;
  int capacity_of_q1;


}RequestQueue;

RequestQueue* create_request_queue(int capacity);
void push_request(RequestQueue* q1, Request req,pthread_t thread_id);
Request pop_request(RequestQueue* q1, pthread_t* thread_id);
int has_requests(RequestQueue* q1);
void destroy_request_queue(RequestQueue* q1);


#endif //OPENMP_FOR_LABA5_QUEUE_FOR_THREADS_H