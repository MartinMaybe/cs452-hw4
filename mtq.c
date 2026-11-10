#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "deq.h"
#include "mtq.h"
#include "error.h"

typedef struct {
    Deq q;
    int cap;                    // max items
    pthread_mutex_t lock;
    pthread_cond_t notEmpty;    // consumer sleeps while q is empty
    pthread_cond_t notFull;     // cosnumer sleeps while q is full
} *Rep;

/**
 * pthread functions return error or 0 on success. This helper checks
 * the result and reports failures.
 */
static void check(int rc, const char *what) {
    if (rc) ERROR("%s failed: %s", what, strerror(rc));
}

/**
 * Creats empty queue with cap items at most. 
 */
extern Mtq mtq_new(int cap) {
    Rep r = (Rep)malloc(sizeof(*r));
    if (!r) ERROR("malloc() failed");

    r->q = deq_new();
    r->cap = cap;

    // initialize mutex and condition variables before any thread 
    // uses the queue
    check(pthread_mutex_init(&r->lock, 0), "pthread_mutex_init()");
    check(pthread_cond_init(&r->notEmpty, 0), "pthread_cond_init()");
    check(pthread_cond_init(&r->notFull, 0), "pthread_cond_init()");
    return r;
}

/**
 * Add d at the tail, blocks while queue is full
 */
extern void mtq_put(Mtq q, Data d) {
    Rep r = (Rep)q;

    // Take the lock, no other thread tocuhes deque
    check(pthread_mutex_lock(&r->lock), "pthread_mutex_lock()");

    // while queue is full (cap != 0), sleep on notFull
    // pthread_cond_wait releases lock and sleeps, so a consumer can
    // get in and remove an item. Wakes up, reaquires lock, returns
    while (r->cap && deq_len(r->q) >= r->cap)
        check(pthread_cond_wait(&r->notFull, &r->lock), "pthread_cond_wait()");
    deq_tail_put(r->q, d);
    
    // queue is non empty, wake any consumer in waiting with signal
    check(pthread_cond_signal(&r->notEmpty), "pthread_cond_signal()");
    check(pthread_mutex_unlock(&r->lock), "pthread_mutex_unlock()");
}

/**
 * Remove and return item at the head, blocks while queue is empty
 */
extern Data mtq_get(Mtq q) {
  Rep r = (Rep)q;
  check(pthread_mutex_lock(&r->lock), "pthread_mutex_lock()");

  // while queue is empty, sleep on notEmpty. Lock releases while asleep
  // to allow producer to add an item. Wakes up, reaquires lock, returns
  while (deq_len(r->q) == 0)
    check(pthread_cond_wait(&r->notEmpty, &r->lock), "pthread_cond_wait()");
  Data d = deq_head_get(r->q);

  // A slot just opened up, so wake one producer that may be waiting in
  // mtq_put because the queue was full.
  check(pthread_cond_signal(&r->notFull), "pthread_cond_signal()");
  check(pthread_mutex_unlock(&r->lock), "pthread_mutex_unlock()");
  return d;
}

/** 
 * Destroy the queue. NOT MT-safe: call it only after every thread using
 * the queue has been joined. If f is non-NULL it is applied to each item
 * still in the queue (for example, to free leftovers).
 */ 
extern void mtq_del(Mtq q, MtqMapF f) {
  Rep r = (Rep)q;
  deq_del(r->q, f);                     // free deque and nodes
  pthread_mutex_destroy(&r->lock);
  pthread_cond_destroy(&r->notEmpty);
  pthread_cond_destroy(&r->notFull);
  free(r);
}