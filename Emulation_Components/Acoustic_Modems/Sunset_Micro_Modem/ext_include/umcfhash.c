#include <libphf.h>
#include "umcfhash.h"

/* The hash function */
ub4 umcfhash(key, len)
char *key;
int   len;
{
/* small adjustments to _a_ to make values distinct */
static ub1 tab[] = {
36,10,9,0,0,19,0,0,0,16,51,0,12,0,38,24,
16,46,46,16,10,51,51,46,51,41,51,8,37,0,0,32,
};

  ub4 rsl, val = phf_lookup(key, len, 0x9e3779b9);
  rsl = ((val>>27)^tab[val&0x1f]);
  return rsl;
}

