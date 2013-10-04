
#ifndef UMCFHASH
#define UMCFHASH
#include <libphf.h>
#define UMCFHASHLEN 0x20  /* length of hash mapping table */
#define UMCFHASHNKEYS 56  /* How many keys were hashed */
#define UMCFHASHRANGE 64  /* Range any input might map to */
#define UMCFHASHSALT 0x9e3779b9 /* internal, initialize normal hash */

ub4 umcfhash();

#endif  /* UMCFHASH */

