#ifndef _POOL_H_
#define _POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX_THREADS
    #define MAX_THREADS 32
    #define MAX_QUEUE 256
#endif

typedef struct pool_s thrd_pool_t;

typedef enum {
    pool_invalid        = -1,
    pool_lock_failure   = -2,
    pool_queue_full     = -3,
    pool_shutdown       = -4,
    pool_thread_failure = -5
} pool_error_t;

typedef enum {
    pool_graceful       = 1
} thrd_destroy_flags_t;

/**
 * @brief Creates a thrd_pool_t object.
 * @param thread_count Number of worker threads.
 * @param queue_size   Size of the queue.
 * @return a newly created thread pool or NULL
 */
thrd_pool_t *thrd_pool(int thread_count, int queue_size);

/**
 * @brief add a new task in the queue of a thread pool
 * @param pool     Thread pool to which add the task.
 * @param function Pointer to the function that will perform the task.
 * @param argument Argument to be passed to the function.
 * @return 0 if all goes well, negative values in case of error (@see
 * pool_error_t for codes).
 */
int thrd_add(thrd_pool_t *pool, void (*routine)(void *), void *arg);

/**
 * @brief Stops and destroys a thread pool.
 * @param pool  Thread pool to destroy.
 * @param flags Flags for shutdown
 *
 * Known values for flags are 0 (default) and pool_graceful in
 * which case the thread pool doesn't accept any new tasks but
 * processes all pending tasks before shutdown.
 */
int thrd_destroy(thrd_pool_t *pool, int flags);

/**
 * @brief Wait until all the tasks are finished in the thread pool.
 * @param pool Thread pool.
 */
void thrd_wait(thrd_pool_t *pool);

#ifdef __cplusplus
}
#endif

#endif /* _POOL_H_ */
