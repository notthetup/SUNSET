
#ifndef RVIDHASH
#define RVIDHASH
#include <libphf.h>
#define RVIDHASHLEN 0x2  /* length of hash mapping table */
#define RVIDHASHNKEYS 4  /* How many keys were hashed */
#define RVIDHASHRANGE 4  /* Range any input might map to */
#define RVIDHASHSALT 0xe3779b90 /* internal, initialize normal hash */

ub4 rvidhash();

#endif  /* RVIDHASH */

