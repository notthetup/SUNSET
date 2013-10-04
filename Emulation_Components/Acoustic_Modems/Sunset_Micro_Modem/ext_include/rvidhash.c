#include <libphf.h>
#include "rvidhash.h"

/* The hash function */
ub4 rvidhash(key, len)
char *key;
int   len;
{
/* small adjustments to _a_ to make values distinct */
static ub1 tab[] = {
  0,  2,};

  ub4 rsl, val = phf_lookup(key, len, 0xe3779b90);
  rsl = ((val>>31)^tab[val&0x1]);
  return rsl;
}

