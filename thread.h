#ifndef THREAD_H
#define THREAD_H

typedef void *Thread;

extern Thread thr_start(int n, void*(*f)(void *), void *arg);
extern void thr_wait(Thread t);

#endif