/*!
 * \file libnmea2.c
 * \author jon shusta <jshusta@whoi.edu>
 * \date 2009.09
 * \brief support library for manipulating streams of NMEA strings.
 * \mainpage libnmea2
 * example:
 * \code
     nmeamsg_t  msg;
     nmeabuf_t  nbuf;
     char       src[SRCSZ];
     nmea_ctor(&nbuf, src, SRCSZ);
     while (1)
       if (poll(fd, ...))
         if (rxlen = read(fd rxbuf, ...))
           if (nmea_concat(&nbuf, rxbuf, (size_t)rxlen))
             if (nmea_scan(&nbuf, &msg))
               txlen = nmea_parse(txbuf, BUFSZ, &msg);
 * \endcode
 */

//#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "libnmea.h"

/*!
 */
static void nmeabuf_shift(nmeabuf_t *buffer)
{
  size_t  off;
  switch(buffer->state)
  {
    case NMEA_S_CLEAR: off = buffer->index; break;
    case NMEA_S_MARK:  off = buffer->start; break;
    default: return;
  }
  buffer->start   = 0;
  buffer->index  -= off;
  buffer->length -= off;
  if (buffer->length) memmove(buffer->buffer, buffer->buffer + off, sizeof(char) * buffer->length);
}


/*!
 */
void nmea_ctor(nmeabuf_t *buffer, const char *source, const size_t maxlen)
{
  buffer->buffer = (char *)source;
  buffer->alloc  = maxlen;
  nmea_reset(buffer);
}


/*!
 */
void nmea_reset(nmeabuf_t *buffer)
{
  memset(buffer->buffer, '\0', sizeof(char) * buffer->alloc);
  buffer->length = 0;
  buffer->index  = 0;
  buffer->start  = 0;
  buffer->state  = NMEA_S_CLEAR;
}


/*!
 */
int nmea_concat(nmeabuf_t *buffer, const char *rbuf, const size_t rlen)
{
  nmeabuf_shift(buffer);

  if (!rlen) return 0;

  if (rlen > (buffer->alloc - buffer->length)) return 0;

  memcpy(buffer->buffer + buffer->length, rbuf, sizeof(char) * rlen);

  buffer->length += rlen;

  return 1;
}


/*!
 */
int nmea_scan(nmeabuf_t *buffer, nmeamsg_t *message)
{
  size_t  i = buffer->index;
  int     ret = 0;

  while ((i < buffer->length) && (0 == ret))
  {
    switch(buffer->state)
    {
      case NMEA_S_CLEAR:
        if ('$' == buffer->buffer[i])
        {
          buffer->start = i;
          buffer->state = NMEA_S_MARK;
        }
        break;

      case NMEA_S_MARK:
        if ('\n' == buffer->buffer[i])
        {
          if (message)
          {
            message->nmeabuf = buffer;
            message->start   = buffer->start;
            message->length  = i - buffer->start + 1;
          }
          buffer->state = NMEA_S_CLEAR;
          ret = 1;
        }
        break;
    }
    i++;
  }
  buffer->index = i;
  return ret;
}


/*!
 */
size_t nmea_peek(char *wbuf, const size_t maxlen, nmeamsg_t *message)
{
  if (message->length > maxlen) return 0;
  memcpy(wbuf, message->nmeabuf->buffer + message->start, sizeof(char) * message->length);
  return message->length;
}


/*!
 */
size_t nmea_parse(char *wbuf, const size_t maxlen, nmeamsg_t *message)
{
  size_t len = nmea_peek(wbuf, maxlen, message);
  nmeabuf_shift(message->nmeabuf);
  return len;
}


/*!
 */
int nmea_cksum(const char *nmeasz, long *cksum)
{
  char *  end      = NULL,
       *  start    = NULL;
  long    calcsum  = 0,
          checksum = 0;

  /* get start/end of sentence
   * verify they're in order
   */
  start = strchr(nmeasz, '$');
  if (NULL == start) return -1;
  end = strchr(start, '*');
  if (NULL == end) return -1;

  checksum = strtol(end+1, (char **)NULL, 16);
  if (0 > checksum)
  {
    perror("strtol");
    return -1;
  }

  start++;                                    /* begin after '$' */
  while (start < end) calcsum ^= *(start++);  /* calculate */
  if (NULL != cksum) *cksum = calcsum;        /* store calculated checksum */
  return (calcsum == checksum) ? 0 : -1;      /* test, return */
}


/*!
 */
int nmea_cksum_msg(nmeamsg_t *message, long *cksum)
{
  char buf[message->length+1];
  memset(buf, '\0', sizeof(char) * message->length + 1);
  nmea_parse(buf, message->length, message);
  return nmea_cksum(buf, cksum);
}


/*!
 */
void nmea_debug(FILE *stream, const nmeabuf_t *buffer)
{
  size_t i;

  for (i = 0; i < buffer->length; i++)
    fprintf(stream, "%c", isprint(buffer->buffer[i]) ? buffer->buffer[i] : '.');
  fprintf(stream, "\n");

  for (i = 0; i <= buffer->length; i++)
    if (i == buffer->start)
    {
      if (i == buffer->index)
        fprintf(stream, "X");
      else
        fprintf(stream, "S");
    }
    else
    {
      if (i == buffer->index)
        fprintf(stream, "I");
      else
        fprintf(stream, " ");
    }
  fprintf(stream, "\n");
}
