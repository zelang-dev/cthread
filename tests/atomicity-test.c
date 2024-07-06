/* Test program for atomicity from https://en.cppreference.com/w/c/language/atomic */

#include "../cthread.h"
#include <stddef.h>
#include <stdio.h>
#include <assert.h>

atomic_int acnt = 0;
int cnt = 0;

int f(void *thr_data) {
    (void)thr_data;
    int n;
    for (n = 0; n < 1000; ++n) {
        ++cnt;
        // ++acnt;
        // for this example, relaxed memory order is sufficient, e.g.
        atomic_fetch_add_explicit(i32, &acnt, 1, memory_order_relaxed);
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
    printf("The atomic counter is %u\n", acnt);
    printf("The non-atomic counter is %u\n", cnt);

    return 0;
}
