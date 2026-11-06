#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "thread.h"
#include "error.h"

typedef struct {
    int n;
    pthread_t *tid;
} *Rep;

/**
 * Run f(arg) in n new threads, every thread gets same arg pointer
 */
extern Thread thr_start(int n, void*(*f)(void *), void *arg) {
    Rep r = malloc(sizeof(*r));
    if (!r) ERROR("malloc() failed");

    r->n = n;
    r->tid = malloc(n * sizeof(pthread_t));
    if (!r->tid) ERROR("malloc() failed");

    for (int i = 0; i < n; i++) {
        int rc = pthread_create(&r->tid[i], 0, f, arg);
        if (rc) ERROR("pthread_create() failed: %s", strerror(rc));
    }
    return r;
}

/**
 * Wait on all threads created and free the handle
 */
extern void thr_wait(Thread t) {
    Rep r = t;
    for (int i = 0; i < r->n; i++) {
        int rc = pthread_join(r->tid[i], 0);
        if (rc) ERROR("pthread_join() failed: %s", strerror(rc));
    }
    free(r->tid);
    free(r);
}