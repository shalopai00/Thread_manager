
#include "thread_manager.h"
#include "task.h"
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pthread_setname_np(pthread_self(), "main");
    ThreadManager* tm = create_thread_manager(6,100);
    start_manager(tm);

    sleep(30);
    for (int i=0; i<2; i++) {
        add_task(tm,task);
    }
    sleep(10);



    printf("COMPLETING TASKS");
    sleep(3000000);

    stop_manager(tm);
    destroy_manager(tm);

    return 0;
}
