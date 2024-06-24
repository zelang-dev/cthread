#define THREAD 32
#define QUEUE  256

#include "../cthread.h"
#include <stdio.h>
#include <assert.h>

int tasks = 0, done = 0;
mtx_t lock;

void dummy_task(void *arg) {
    usleep(10000);
    mtx_lock(&lock);
    done++;
    mtx_unlock(&lock);
}

int main(int argc, char **argv) {
    thrd_pool_t *pool;

    mtx_init(&lock, mtx_plain);

    assert((pool = thrd_pool(THREAD, QUEUE)) != NULL);
    fprintf(stderr, "Pool started with %d threads and "
            "queue size of %d\n", THREAD, QUEUE);

    while (thrd_add(pool, &dummy_task, NULL) == 0) {
        mtx_lock(&lock);
        tasks++;
        mtx_unlock(&lock);
    }

    fprintf(stderr, "Added %d tasks\n", tasks);

    while ((tasks / 2) > done) {
        usleep(5);
    }

    thrd_wait(pool);
    assert(thrd_destroy(pool, 0) == 0);
    fprintf(stderr, "Did %d tasks\n", done);

    mtx_destroy(&lock);

    return 0;
}
