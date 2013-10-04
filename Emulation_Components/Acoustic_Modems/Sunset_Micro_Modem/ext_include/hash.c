/*!
 * \file hash.c
 * \author jon shusta <jshusta@whoi.edu>
 * \date 2009.09
 * \brief hash functions.
 */

#include <stdlib.h>
#include "libumutil.h"
#include "extern.h"
#include "nmeahash.h"
#include "umcfhash.h"
#include "rvidhash.h"

/*!
 * \brief get hash key for a given NMEA message ID. hashes are case-sensitive.
 *
 * \pre the buffer pointer \a buf points to the beginning of the message ID and
 *      not the beginning of the string. e.g., the 'C' in "$CAXST,0"
 *
 * \returns -1 if the hash fails, otherwise, a unique index in nmeatab
 * which was verified with string comparison.
 */
int um_hash_nmea(const char *buf)
{
  ub4 hk = nmeahash(buf, 5);
  int aux = hk;
  if (NMEAHASHNKEYS <= hk) return -1;
  if (strncmp(buf, nmeatab[hk], 5)) return -1;
  return (int)hk;
}


/*!
 * \brief get hash key for a given NMEA message ID. hashes are case-sensitive.
 *
 * \pre the buffer pointer \a buf points to the beginning of the config key and
 *      not the beginning of the string. e.g., the 'S' in "$CACFQ,SRC,1"
 *
 * \returns -1 if the hash fails, otherwise, a unique index in umcftab
 * which was verified with string comparison.
 */
int um_hash_conf(const char *buf)
{
  ub4 hk = umcfhash(buf, 3);
  if (UMCFHASHNKEYS <= hk) return -1;
  if (strncmp(buf, umcftab[hk], 3)) return -1;
  return (int)hk;
}


/*!
 */
int um_hash_rvid(const char *buf, const size_t len)
{
  ub4 hk = rvidhash(buf, len);
  if (RVIDHASHNKEYS <= hk) return -1;
  if (strncmp(buf, rvidtab[hk], len)) return -1;
  return (int)hk;
}
