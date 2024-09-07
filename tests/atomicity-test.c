/* Test program for atomicity from https://en.cppreference.com/w/c/language/atomic */

#include "../cthread.h"
#include "../catomic.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

atomic_size_t acnt;
size_t cnt = 0;

int f(void *thr_data) {
    (void)thr_data;
    int n;
    for (n = 0; n < 1000; ++n) {
        ++cnt;
        // ++acnt;
        // for this example, relaxed memory order is sufficient, e.g.
        atomic_fetch_add(&acnt, 1);
    }
    return 0;
}

int main(void) {
    thrd_t thr[10];
    int n, counter = 1;
    while (true) {
        for (n = 0; n < 10; ++n)
            thrd_create(&thr[n], f, NULL);
        for (n = 0; n < 10; ++n)
            thrd_join(thr[n], NULL);

        if (acnt != cnt) {
            assert(acnt > cnt);
            break;
        }

        counter++;
    }

    printf("Found atomicity, took %d tries!\n", counter);
    printf("The atomic counter is %zu\n", acnt);
    printf("The non-atomic counter is %zu\n", cnt);

    return 0;
}
