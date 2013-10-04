
#ifndef NMEAHASH
#define NMEAHASH
#include <libphf.h>
#define NMEAHASHLEN 0x40  /* length of hash mapping table */
#define NMEAHASHNKEYS 74  /* How many keys were hashed */
#define NMEAHASHRANGE 128  /* Range any input might map to */
#define NMEAHASHSALT 0x9e3779b9 /* internal, initialize normal hash */

ub4 nmeahash();

#endif  /* NMEAHASH */

