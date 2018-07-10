#include <xinu.h>

static unsigned long randx = 1;

void rand_delay(int ms_max) {
  unsigned int rand;
  /* Generate a pseudo random number */
  rand = (((randx = randx * 1103515245 + 12345) >> 16) & 077777);
  /* Make sure number is below given max */
  rand = rand % ms_max + 1;

  sleepms(rand);
}
