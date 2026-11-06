#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mole.h"
#define LAWNIMP
#include "lawnimp.h"
#undef LAWNIMP
#include "error.h"

static int rdm(int lo, int hi) {
  return random()%(hi-lo+1)+lo;
}

/**
 * Allocates a MoleRep and fills it in. Position: x and y are each random in [0, max-1], 
 * where max = lawnsize * molesize (0..599 by default). vimlo/vimhi bound the “vim” 
 * values: three random durations vim0, vim1, vim2, each in [vimlo, vimhi]. Then it 
 * calls lawnimp_mole(mole) to draw it on the lawn and saves the result in mole->box.
 */
extern Mole mole_new(Lawn l, int vimlo, int vimhi) {
  if (!vimlo) vimlo=1;
  if (!vimhi) vimhi=5;

  LawnRep lawn=(LawnRep)l;
  MoleRep mole=(MoleRep)malloc(sizeof(*mole));
  if (!mole) ERROR("malloc() failed");
  mole->size=lawn->molesize;
  int max=lawn->lawnsize*lawn->molesize;
  mole->x=rdm(0,max-1);
  mole->y=rdm(0,max-1);
  mole->vim0=rdm(vimlo,vimhi);
  mole->vim1=rdm(vimlo,vimhi);
  mole->vim2=rdm(vimlo,vimhi);
  mole->lawn=lawn;
  mole->box=lawnimp_mole(mole);
  return mole;
}

/**
 * Calls lawnimp_whack(m), then free(m).
 */
extern void mole_whack(Mole m) {
  lawnimp_whack(m);
  free(m);
}

