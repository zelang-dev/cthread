
# C Thread, emulated C11 and thread pool

Using [Pthreads](https://en.wikipedia.org/wiki/Pthreads), or [Pthreads4w](http://sourceforge.net/projects/pthreads4w/).

The **Windows** build in **_deps_** folder, _this fork has ABI differences,_ see original [README.md](https://github.com/GerHobbelt/pthread-win32/blob/master/README.md). And this build and all general memory allocations based on standard **malloc/stdlib.h** replaced with forked [rpmalloc](https://github.com/zelang-dev/rpmalloc) that has no dependency on `thread_local`.

> This branch has some changes to be able to be compiled using [Tiny C compiler](https://github.com/zelang-dev/tinycc).

**CThread** is a minimal, portable implementation of basic threading classes for C. They closely mimic the functionality and naming of the [C11 standard](https://en.cppreference.com/w/c/thread), and should be easily replaceable with the corresponding standard variants. This package also integrates **C89 compatible atomics** for [C11 Atomic](https://en.cppreference.com/w/c/atomic) support.

All `malloc` operations are [lockless](https://preshing.com/20120612/an-introduction-to-lock-free-programming/), and also implements emulated **Thread-local storage** via macro ***thread_storage(type, variable)*** as:

_example.h_
```h
#include "cthread.h"
#include <stdio.h>
#include <assert.h>
thread_storage_create(int, gLocalVar);
```
_example.c_
```c
#include "example.h"
#define THREAD_COUNT 5

thread_storage(int, gLocalVar)

static int thread_local_storage(void *aArg) {
    int thread = *(int *)aArg;
    free(aArg);

    int data = thread + rand();
    *gLocalVar() = data;
    printf("thread #%d, gLocalVar is: %d\n", thread, *gLocalVar());
    assert(*gLocalVar() == data);
    return 0;
}

void emulated_tls(void) {
    thrd_t t[THREAD_COUNT];
    *gLocalVar() = 1;

    for (int i = 0; i < THREAD_COUNT; i++) {
        int *n = malloc(sizeof * n);
        *n = i;
        thrd_create(t + i, thread_local_storage, n);
    }

    for (int i = 0; i < THREAD_COUNT; i++) {
        thrd_join(t[i], NULL);
    }

    assert(*gLocalVar() == 1);
}

int main(void) {
    emulated_tls();
    return 0;
}
```
> This package implements ***thrd_local(type, variable)*** macro, same behavior as above, but
> _will not emulate_ if feature is _available_ in your compiler, should be used for cross compatibility calling.

## Thread pool implementation

 * Starts all threads on creation of the thread pool.
 * Stops and joins all worker threads on destroy.

### Synopsis
```c
/**
 * Creates a thrd_pool_t object.
 * @param thread_count Number of worker threads.
 * @param queue_size   Size of the queue.
 * @return a newly created thread pool or NULL
 */
thrd_pool_t *thrd_pool(int thread_count, int queue_size);

/**
 * Add a new task in the queue of a thread pool
 * @param pool     Thread pool to which add the task.
 * @param function Pointer to the function that will perform the task.
 * @param argument Argument to be passed to the function.
 * @return 0 if all goes well, negative values in case of error (@see
 * pool_error_t for codes).
 */
int thrd_add(thrd_pool_t *pool, void (*routine)(void *), void *arg);

/**
 * Stops and destroys a thread pool.
 * @param pool  Thread pool to destroy.
 * @param flags Flags for shutdown
 *
 * Known values for flags are 0 (default) and `pool_graceful` in
 * which case the thread pool doesn't accept any new tasks but
 * processes all pending tasks before shutdown.
 */
int thrd_destroy(thrd_pool_t *pool, int flags);

/**
 * Wait until all the tasks are finished in the thread pool.
 * @param pool Thread pool.
 */
void thrd_wait(thrd_pool_t *pool);
```

```c
#define THREAD 32
#define QUEUE  256

#include "cthread.h"
#include <stdio.h>

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
    if ((pool = thrd_pool(THREAD, QUEUE)) != NULL)
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
    if (thrd_destroy(pool, 0) == 0)
        fprintf(stderr, "Did %d tasks\n", done);

    mtx_destroy(&lock);
    return 0;
}
```
