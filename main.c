#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "deq.h"
#include "lawn.h"
#include "mole.h"
#include "thread.h"
#include "mtq.h"

// static Deq deq;
 
static void *produce(void *a) { 
  void **arg = a;
  Mtq q = (Mtq)arg[0];
  Lawn l = (Lawn)arg[1];
  mtq_put(q, mole_new(l, 0, 0)); 
  return 0;
}

static void *consume(void *a) { 
  void **arg = a;
  Mtq q = (Mtq)arg[0];
  mole_whack(mtq_get(q));
  return 0;
}

int main() {
  srandom(time(0));
  const int n = 10;
  Lawn lawn = lawn_new(0,0);
  Mtq q = mtq_new(4);
  void *arg[] = {q, lawn};

  Thread produce = thr_start(n, produce, arg);
  Thread consume = thr_start(n, consume, arg);
  thr_wait(produce);
  thr_wait(consume);

  mtq_del(q, NULL);
  lawn_free(lawn);
}
