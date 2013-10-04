/*!
 */

#include <ctype.h>
#include <time.h>
#include "libumutil.h"


/*!
 */
int um_tstotm(struct tm *ts, int hh, int mm, int ss)
{
  time_t t = time(NULL);
  gmtime_r(&t, ts);
  ts->tm_sec  = ss;
  ts->tm_min  = mm;
  ts->tm_hour = hh;
  return mktime(ts);
}


/*!
 */
int um_dttotm(struct tm *ts, int yy, int mm, int dd, int hh, int ii, int ss)
{
  ts->tm_sec  = ss;
  ts->tm_min  = mm;
  ts->tm_hour = hh;
  ts->tm_mday = dd;
  ts->tm_mon  = mm - 1;
  ts->tm_year = yy - 1900;
  return mktime(ts);
}


/*!
 */
int um_cdtosign(int *deg, char cd[2])
{
  int sign = 1;

  switch(toupper(cd[0]))
  {
    case 'E':
    case 'N':
      break;
    case 'W':
    case 'S':
      sign = -1;
      break;
    default:
      return -1;
  }

  if (*deg > 0) *deg *= sign;
  return 0;
}
