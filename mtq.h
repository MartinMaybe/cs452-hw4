#ifndef MTQ_H
#define MTQ_H

typedef void *Mtq;
typedef void *Data;
typedef void (*MtqMapF)(Data d);

extern Mtq mtq_new(int cap);

extern void mtq_put(Mtq q, Data d);
extern Data mtq_get(Mtq q);

extern void mtq_del(Mtq q, MtqMapF f);

#endif