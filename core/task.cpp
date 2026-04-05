#include<cstdio>
#include <pthread.h>

#include <cmath>

long long fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

void task() {
    long long result = fib(42);  // 45 уже очень тяжело, 50 - экстремально
    printf("Thread %lu: fib(42)=%lld\n", pthread_self(), result);
}