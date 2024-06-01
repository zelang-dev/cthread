
#include "../cthread.h"
#include <stdio.h>
#include <assert.h>

#define THREAD 4
#define SIZE   8192
#define QUEUES 64

thrd_pool_t *pool[QUEUES];
int tasks[SIZE], left;
mtx_t lock;

int error;

void dummy_task(void *arg) {
    int *pi = (int *)arg;
    *pi += 1;

    if(*pi < QUEUES) {
        assert(thrd_add(pool[*pi], &dummy_task, arg) == 0);
    } else {
        mtx_lock(&lock);
        left--;
        mtx_unlock(&lock);
    }
}

int main(int argc, char **argv)
{
    int i, copy = 1;

    left = SIZE;
    mtx_init(&lock, mtx_plain);

    for(i = 0; i < QUEUES; i++) {
        pool[i] = thrd_pool(THREAD, SIZE);
        assert(pool[i] != NULL);
    }

    usleep(10);

    for(i = 0; i < SIZE; i++) {
        tasks[i] = 0;
        assert(thrd_add(pool[0], &dummy_task, &(tasks[i])) == 0);
    }

    while(copy > 0) {
        usleep(10);
        mtx_lock(&lock);
        copy = left;
        mtx_unlock(&lock);
    }

    for(i = 0; i < QUEUES; i++) {
        assert(thrd_destroy(pool[i], 0) == 0);
    }

    mtx_destroy(&lock);

    puts("Passed!");
    return 0;
}
