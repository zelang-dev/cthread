#include "../cthread.h"
#include <stdio.h>
#include <assert.h>

#define THREAD 4
#define SIZE   256

thrd_pool_t *pool;
int left;
mtx_t lock;

int error;

void dummy_task(void *arg) {
    usleep(5);
    mtx_lock(&lock);
    left--;
    mtx_unlock(&lock);
}

int main(int argc, char **argv)
{
    int i;

    mtx_init(&lock, mtx_plain);

    /* Testing immediate shutdown */
    left = SIZE;
    pool = thrd_pool(THREAD, SIZE);
    for(i = 0; i < SIZE; i++) {
        assert(thrd_add(pool, &dummy_task, NULL) == 0);
    }
    assert(thrd_destroy(pool, 0) == 0);
    assert(left > 0);

    /* Testing graceful shutdown */
    left = SIZE;
    pool = thrd_pool(THREAD, SIZE);
    for(i = 0; i < SIZE; i++) {
        assert(thrd_add(pool, &dummy_task, NULL) == 0);
    }
    assert(thrd_destroy(pool, pool_graceful) == 0);
    assert(left == 0);

    mtx_destroy(&lock);

    puts("Passed!");
    return 0;
}
