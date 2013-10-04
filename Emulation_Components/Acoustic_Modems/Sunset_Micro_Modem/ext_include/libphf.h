/*!
 * \file libphf.h
 * \author Bob Jenkins <bob_jenkins@burtleburtle.net>
 * \author jon shusta <jshusta@whoi.edu>
 * \date 1996.09
 * \brief a hash function for table lookup, same function as lookup.c.

 * Use this code in any way you wish.  Public Domain.  It has no warranty.
 * Source is http://burtleburtle.net/bob/c/lookupa.h
 * 2009.04 jas - gave functions prefix phf_
 * 2008.08 jas - renamed libphf
 *               inserted standard.h
 * 2008.07 jas - edited for package distro, doxygen
 */

#ifndef LIBPHF_H
#define LIBPHF_H

#include <stdio.h>
#include <stddef.h>
#include <linux/types.h>
#include <stdint.h>

#if !defined __STRICT_ANSI__
typedef  unsigned long long  ub8;
#define UB8MAXVAL 0xffffffffffffffffLL
#define UB8BITS 64
#endif

#if !defined __STRICT_ANSI__
typedef  signed long long  sb8;
#define SB8MAXVAL 0x7fffffffffffffffLL
#endif

typedef uint32_t  ub4;   /* unsigned 4-byte quantities */
#define UB4MAXVAL 0xffffffff
#define UB4BITS 32

typedef    signed long  int  sb4;
#define SB4MAXVAL 0x7fffffff

typedef  unsigned short int  ub2;
#define UB2MAXVAL 0xffff
#define UB2BITS 16

typedef    signed short int  sb2;
#define SB2MAXVAL 0x7fff

typedef  unsigned       char ub1;
#define UB1MAXVAL 0xff
#define UB1BITS 8

typedef    signed       char sb1;   /* signed 1-byte quantities */
#define SB1MAXVAL 0x7f

typedef                 int  word;  /* fastest type available */


#define bis(target,mask)  ((target) |=  (mask))
#define bic(target,mask)  ((target) &= ~(mask))
#define bit(target,mask)  ((target) &   (mask))
#define hashsize(n)       ((ub4)1<<(n))
#define hashmask(n)       (hashsize(n)-1)

#ifndef min
# define min(a,b) (((a)<(b)) ? (a) : (b))
#endif

#ifndef max
# define max(a,b) (((a)<(b)) ? (b) : (a))
#endif

#ifndef align
# define align(a) (((ub4)a+(sizeof(void *)-1))&(~(sizeof(void *)-1)))
#endif

#ifndef abs
# define abs(a)   (((a)>0) ? (a) : -(a))
#endif

#define TRUE          (1)  /**/
#define FALSE         (0)  /**/
#define MAXKEYLEN    (30)  /* maximum length of a key */
#define CHECKSTATE    (8)  /**/


/*!
 * mix three 32-bit values reversibly.
 * For every delta with one or two bit set, and the deltas of all three
 * high bits or all three low bits, whether the original value of a,b,c
 * is almost all zero or is uniformly distributed,
 *
 * - If mix() is run forward or backward, at least 32 bits in a,b,c
 * have at least 1/4 probability of changing.
 *
 * - If mix() is run forward, every bit of c will change between 1/3 and
 * 2/3 of the time.  (Well, 22/100 and 78/100 for some 2-bit deltas.)
 *
 * mix() was built out of 36 single-cycle latency instructions in a
 * structure that could supported 2x parallelism, like so:
 *     a -= b;
 *     a -= c; x = (c>>13);
 *     b -= c; a ^= x;
 *     b -= a; x = (a<<8);
 *     c -= a; b ^= x;
 *     c -= b; x = (b>>13);
 *     ...
 *
 * Unfortunately, superscalar Pentiums and Sparcs can't take advantage
 * of that parallelism.  They've also turned some of those single-cycle
 * latency instructions into multi-cycle latency instructions.  Still,
 * this is the fastest good hash I could find.  There were about 2^^68
 * to choose from.  I only looked at a billion or so.
 */
#define mix(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}


/*!
 * mixc eight 4-bit values as quickly and thoroughly as possible.
 * Repeating mix() three times achieves avalanche.
 * Repeating mix() four times eliminates all funnels and all
 * characteristics stronger than 2^{-11}.
*/
#define mixc(a,b,c,d,e,f,g,h) \
{ \
   a^=b<<11; d+=a; b+=c; \
   b^=c>>2;  e+=b; c+=d; \
   c^=d<<8;  f+=c; d+=e; \
   d^=e>>16; g+=d; e+=f; \
   e^=f<<10; h+=e; f+=g; \
   f^=g>>4;  a+=f; g+=h; \
   g^=h<<8;  b+=g; h+=a; \
   h^=a>>9;  c+=h; a+=b; \
}



ub4  phf_lookup(/*_ ub1 *k, ub4 length, ub4 level _*/);

void phf_checksum(/*_ ub1 *k, ub4 length, ub4 *state _*/);


#endif
