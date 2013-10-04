#include <libphf.h>
#include "nmeahash.h"

/* The hash function */
ub4 nmeahash(key, len)
char *key;
int   len;
{
/* small adjustments to _a_ to make values distinct */
static ub1 tab[] = {
0,28,34,0,0,0,4,85,4,0,0,0,0,0,0,70,
0,70,20,0,4,0,47,0,101,61,0,0,0,0,4,53,
37,0,0,1,0,10,0,0,0,0,53,0,12,0,1,1,
47,16,0,119,85,0,18,42,0,70,122,0,92,73,0,39,
};

  ub4 rsl, val = phf_lookup(key, len, 0x9e3779b9);
  rsl = ((val>>26)^tab[val&0x3f]);
  return rsl;
}

