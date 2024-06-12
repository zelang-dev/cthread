
# C Thread, emulated C11 and thread pool

Using [Pthreads](https://en.wikipedia.org/wiki/Pthreads), or [Pthreads4w](http://sourceforge.net/projects/pthreads4w/).

The **Windows** build in **_deps_** folder, _this fork has ABI differences,_ see original [README.md](https://github.com/GerHobbelt/pthread-win32/blob/master/README.md).

> This branch has some changes to be able to be compiled using [Tiny C compiler](https://github.com/zelang-dev/tinycc).

**CThread** is a minimal, portable implementation of basic threading classes for C. They closely mimic the functionality and naming of the [C11 standard](https://en.cppreference.com/w/c/thread), and should be easily replaceable with the corresponding standard variants.

This package also implements emulated **Thread-local storage** via ***thrd_local(type, variable)*** macro as:

_example.h_
```h
#include "cthread.h"
#include <stdio.h>
#include <assert.h>
thrd_local_create(int, gLocalVar);
```
_example.c_
```c
#include "example.h"
#define THREAD_COUNT 5

thrd_local(int, gLocalVar)

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

## Thread pool implementation

 * Starts all threads on creation of the thread pool.
 * Stops and joins all worker threads on destroy.
