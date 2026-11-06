#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "deq.h"
#include "lawn.h"
#include "mole.h"

static Deq deq;
 
static void produce(Lawn l) { 
  deq_tail_put(deq, mole_new(l, 0, 0)); 
}

static void consume(void) { 
  Mole m = (Mole)deq_head_get(deq);
  mole_whack(m); 
}

int main() {
  srandom(time(0));
  const int n=10;
  Lawn lawn=lawn_new(0,0);
  deq = deq_new();

  for (int i=1; i<=n; i++) {
    produce(lawn);
    consume();
  }

  deq_del(deq, NULL);
  lawn_free(lawn);
}
