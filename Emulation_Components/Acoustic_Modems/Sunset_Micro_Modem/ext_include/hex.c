/*!
 * \file hex.c
 * \author jon shusta <jshusta@whoi.edu>
 * \date 2009.09
 * \brief hex string encoding and decoding.
 */

#include <stdlib.h>


static unsigned char h(unsigned int v)
{
  if (v < 10) return '0' + v;
  if (v < 16) return 'a' + (v - 10);
  return '.';
}
static unsigned int b(unsigned char v)
{
  if (v < '0') return 0;
  if (v < ':') return v - '0';
  if (v < 'A') return 0;
  if (v < '[') return v - 'A';
  if (v < 'a') return 0;
  if (v < '{') return v - 'a';
  return 0;
}


/*!
 * \brief convert each byte of a char array to two-digit human readable (big endian) hex values.
 *
 * \param ret the buffer to write human-readable digits into.
 * \param maxlen the total available size of \a ret.
 * \param src the source buffer
 * \param len length of the source buffer
 *
 * \pre \a maxlen is 2 * \a len or greater.
 * \pre \a ret points to at least \a maxlen bytes in memory.
 * \pre \a ret and \a src do not overlap.
 *
 * \return the number of bytes written to \a ret (should be twice \a len).
 */
size_t um_hexencode(char *        ret,
                    const size_t  maxlen,
                    const char *  src,
                    const size_t  len)
{
  size_t i;

  if ((len << 1) > maxlen) return 0;
  for (i = 0; i < len; i++)
  {
    ret[(i << 1)]   = h(0xf & (src[i] >> 4));
    ret[(i << 1)+1] = h(0xf & src[i]);
  }
  return len << 1;
}


/*!
 * \brief convert two-digit human readable hex values to a byte array. this
 * function may operate in-place; ret and src may point to the same location.
 *
 * \param ret destination buffer to decode to
 * \param maxlen
 * \param src hexencoded buffer
 * \param len length of hexencoded
 *
 * \pre \a ret points to at least \a maxlen bytes in memory.
 * \pre \a maxlen is 0.5 * \a len or greater.
 *
 * \returns length of decoded destination buffer.
 */
size_t um_hexdecode(char *        ret,
                    const size_t  maxlen,
                    const char *  src,
                    const size_t  len)
{
  size_t  i;

  if (len & 1) return 0;
  if ((len >> 1) > maxlen) return 0;

  for (i = 0; i < len; i++)
  {
    if (i & 1) ret[i >> 1] += b(src[i]);
    else       ret[i >> 1]  = b(src[i]) << 4;
  }
  return len >> 1;
}
