/*	$NetBSD: parse-duration.c,v 1.2 2010/12/04 23:08:34 christos Exp $	*/

/* Parse a time duration and return a seconds count
   Copyright (C) 2008 Free Software Foundation, Inc.
   Written by Bruce Korb <bkorb@gnu.org>, 2008.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include <config.h>

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse-duration.h"

#ifndef _
#define _(_s)  _s
#endif

#ifndef NUL
#define NUL '\0'
#endif

#define cch_t char const

typedef enum {
  NOTHING_IS_DONE,
  YEAR_IS_DONE,
  MONTH_IS_DONE,
  WEEK_IS_DONE,
  DAY_IS_DONE,
  HOUR_IS_DONE,
  MINUTE_IS_DONE,
  SECOND_IS_DONE
} whats_done_t;

#define SEC_PER_MIN     60
#define SEC_PER_HR      (SEC_PER_MIN * 60)
#define SEC_PER_DAY     (SEC_PER_HR  * 24)
#define SEC_PER_WEEK    (SEC_PER_DAY * 7)
#define SEC_PER_MONTH   (SEC_PER_DAY * 30)
#define SEC_PER_YEAR    (SEC_PER_DAY * 365)

#define TIME_MAX        0x7FFFFFFF

static inline unsigned long
str_const_to_ul (cch_t * str, cch_t ** ppz, int base)
{
  return strtoul (str, (char **)(intptr_t)ppz, base);
}

static inline long
str_const_to_l (cch_t * str, cch_t ** ppz, int base)
{
  return strtol (str, (char **)(intptr_t)ppz, base);
}

static inline time_t
scale_n_add (time_t base, time_t val, int scale)
{
  if (base == BAD_TIME)
    {
      if (errno == 0)
        errno = EINVAL;
      return BAD_TIME;
    }

  if (val > TIME_MAX / scale)
    {
      errno = ERANGE;
      return BAD_TIME;
    }

  val *= scale;
  if (base > TIME_MAX - val)
    {
      errno = ERANGE;
      return BAD_TIME;
    }

  return base + val;
}

static time_t
parse_hr_min_sec (time_t start, cch_t * pz)
{
  int lpct = 0;

  errno = 0;

  /* For as long as our scanner pointer points to a colon *AND*
     we've not looped before, then keep looping.  (two iterations max) */
  while ((*pz == ':') && (lpct++ <= 1))
    {
      unsigned long v = str_const_to_ul (pz+1, &pz, 10);

      if (errno != 0)
        return BAD_TIME;

      start = scale_n_add (v, start, 60);

      if (errno != 0)
        return BAD_TIME;
    }

  /* allow for trailing spaces */
  while (isspace ((unsigned char)*pz))   pz++;
  if (*pz != NUL)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  return start;
}

static time_t
parse_scaled_value (time_t base, cch_t ** ppz, cch_t * endp, int scale)
{
  cch_t * pz = *ppz;
  time_t val;

  if (base == BAD_TIME)
    return base;

  errno = 0;
  val = str_const_to_ul (pz, &pz, 10);
  if (errno != 0)
    return BAD_TIME;
  while (isspace ((unsigned char)*pz))   pz++;
  if (pz != endp)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  *ppz =  pz;
  return scale_n_add (base, val, scale);
}

static time_t
parse_year_month_day (cch_t * pz, cch_t * ps)
{
  time_t res = 0;

  res = parse_scaled_value (0, &pz, ps, SEC_PER_YEAR);

  ps = strchr (++pz, '-');
  if (ps == NULL)
    {
      errno = EINVAL;
      return BAD_TIME;
    }
  res = parse_scaled_value (res, &pz, ps, SEC_PER_MONTH);

  pz++;
  ps = pz + strlen (pz);
  return parse_scaled_value (res, &pz, ps, SEC_PER_DAY);
}

static time_t
parse_yearmonthday (cch_t * in_pz)
{
  time_t res = 0;
  char   buf[8];
  cch_t * pz;

  if (strlen (in_pz) != 8)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  memcpy (buf, in_pz, 4);
  buf[4] = NUL;
  pz = buf;
  res = parse_scaled_value (0, &pz, buf + 4, SEC_PER_YEAR);

  memcpy (buf, in_pz + 4, 2);
  buf[2] = NUL;
  pz =   buf;
  res = parse_scaled_value (res, &pz, buf + 2, SEC_PER_MONTH);

  memcpy (buf, in_pz + 6, 2);
  buf[2] = NUL;
  pz =   buf;
  return parse_scaled_value (res, &pz, buf + 2, SEC_PER_DAY);
}

static time_t
parse_YMWD (cch_t * pz)
{
  time_t res = 0;
  cch_t * ps = strchr (pz, 'Y');
  if (ps != NULL)
    {
      res = parse_scaled_value (0, &pz, ps, SEC_PER_YEAR);
      pz++;
    }

  ps = strchr (pz, 'M');
  if (ps != NULL)
    {
      res = parse_scaled_value (res, &pz, ps, SEC_PER_MONTH);
      pz++;
    }

  ps = strchr (pz, 'W');
  if (ps != NULL)
    {
      res = parse_scaled_value (res, &pz, ps, SEC_PER_WEEK);
      pz++;
    }

  ps = strchr (pz, 'D');
  if (ps != NULL)
    {
      res = parse_scaled_value (res, &pz, ps, SEC_PER_DAY);
      pz++;
    }

  while (isspace ((unsigned char)*pz))   pz++;
  if (*pz != NUL)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  return res;
}

static time_t
parse_hour_minute_second (cch_t * pz, cch_t * ps)
{
  time_t res = 0;

  res = parse_scaled_value (0, &pz, ps, SEC_PER_HR);

  ps = strchr (++pz, ':');
  if (ps == NULL)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  res = parse_scaled_value (res, &pz, ps, SEC_PER_MIN);

  pz++;
  ps = pz + strlen (pz);
  return parse_scaled_value (res, &pz, ps, 1);
}

static time_t
parse_hourminutesecond (cch_t * in_pz)
{
  time_t res = 0;
  char   buf[4];
  cch_t * pz;

  if (strlen (in_pz) != 6)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  memcpy (buf, in_pz, 2);
  buf[2] = NUL;
  pz = buf;
  res = parse_scaled_value (0, &pz, buf + 2, SEC_PER_HR);

  memcpy (buf, in_pz + 2, 2);
  buf[2] = NUL;
  pz =   buf;
  res = parse_scaled_value (res, &pz, buf + 2, SEC_PER_MIN);

  memcpy (buf, in_pz + 4, 2);
  buf[2] = NUL;
  pz =   buf;
  return parse_scaled_value (res, &pz, buf + 2, 1);
}

static time_t
parse_HMS (cch_t * pz)
{
  time_t res = 0;
  cch_t * ps = strchr (pz, 'H');
  if (ps != NULL)
    {
      res = parse_scaled_value (0, &pz, ps, SEC_PER_HR);
      pz++;
    }

  ps = strchr (pz, 'M');
  if (ps != NULL)
    {
      res = parse_scaled_value (res, &pz, ps, SEC_PER_MIN);
      pz++;
    }

  ps = strchr (pz, 'S');
  if (ps != NULL)
    {
      res = parse_scaled_value (res, &pz, ps, 1);
      pz++;
    }

  while (isspace ((unsigned char)*pz))   pz++;
  if (*pz != NUL)
    {
      errno = EINVAL;
      return BAD_TIME;
    }

  return res;
}

static time_t
parse_time (cch_t * pz)
{
  cch_t * ps;
  time_t  res = 0;

  /*
   *  Scan for a hyphen
   */
  ps = strchr (pz, ':');
  if (ps != NULL)
    {
      res = parse_hour_minute_second (pz, ps);
    }

  /*
   *  Try for a 'H', 'M' or 'S' suffix
   */
  else if (ps = strpbrk (pz, "HMS"),
           ps == NULL)
    {
      /* Its a YYYYMMDD format: */
      res = parse_hourminutesecond (pz);
    }

  else
    res = parse_HMS (pz);

  return res;
}

static char *
trim(char * pz)
{
  /* trim leading white space */
  while (isspace ((unsigned char)*pz))  pz++;

  /* trim trailing white space */
  {
    char * pe = pz + strlen (pz);
    while ((pe > pz) && isspace ((unsigned char)pe[-1])) pe--;
    *pe = NUL;
  }

  return pz;
}

/*
 *  Parse the year/months/days of a time period
 */
static time_t
parse_period (cch_t * in_pz)
{
  char * pz   = xstrdup (in_pz);
  char * pT   = strchr (pz, 'T');
  char * ps;
  void * fptr = pz;
  time_t res  = 0;

  if (pT != NUL)
    {
      *(pT++) = NUL;
      pz = trim (pz);
      pT = trim (pT);
    }

  /*
   *  Scan for a hyphen
   */
  ps = strchr (pz, '-');
  if (ps != NULL)
    {
      res = parse_year_month_day (pz, ps);
    }

  /*
   *  Try for a 'Y', 'M' or 'D' suffix
   */
  else if (ps = strpbrk (pz, "YMWD"),
           ps == NULL)
    {
      /* Its a YYYYMMDD format: */
      res = parse_yearmonthday (pz);
    }

  else
    res = parse_YMWD (pz);

  if ((errno == 0) && (pT != NULL))
    {
      time_t val = parse_time (pT);
      res = scale_n_add (res, val, 1);
    }

  free (fptr);
  return res;
}

static time_t
parse_non_iso8601(cch_t * pz)
{
  whats_done_t whatd_we_do = NOTHING_IS_DONE;

  time_t res = 0;

  do  {
    time_t val;

    errno = 0;
    val = str_const_to_l (pz, &pz, 10);
    if (errno != 0)
      goto bad_time;

    /*  IF we find a colon, then we're going to have a seconds value.
        We will not loop here any more.  We cannot already have parsed
        a minute value and if we've parsed an hour value, then the result
        value has to be less than an hour. */
    if (*pz == ':')
      {
        if (whatd_we_do >= MINUTE_IS_DONE)
          break;

        val = parse_hr_min_sec (val, pz);

        if ((whatd_we_do == HOUR_IS_DONE) && (val >= SEC_PER_HR))
          break;

        return scale_n_add (res, val, 1);
      }

    {
      unsigned int mult;

      /*  Skip over white space following the number we just parsed. */
      while (isspace ((unsigned char)*pz))   pz++;

      switch (*pz)
        {
        default:  goto bad_time;
        case NUL:
          return scale_n_add (res, val, 1);

        case 'y': case 'Y':
          if (whatd_we_do >= YEAR_IS_DONE)
            goto bad_time;
          mult = SEC_PER_YEAR;
          whatd_we_do = YEAR_IS_DONE;
          break;

        case 'M':
          if (whatd_we_do >= MONTH_IS_DONE)
            goto bad_time;
          mult = SEC_PER_MONTH;
          whatd_we_do = MONTH_IS_DONE;
          break;

        case 'W':
          if (whatd_we_do >= WEEK_IS_DONE)
            goto bad_time;
          mult = SEC_PER_WEEK;
          whatd_we_do = WEEK_IS_DONE;
          break;

        case 'd': case 'D':
          if (whatd_we_do >= DAY_IS_DONE)
            goto bad_time;
          mult = SEC_PER_DAY;
          whatd_we_do = DAY_IS_DONE;
          break;

        case 'h':
          if (whatd_we_do >= HOUR_IS_DONE)
            goto bad_time;
          mult = SEC_PER_HR;
          whatd_we_do = HOUR_IS_DONE;
          break;

        case 'm':
          if (whatd_we_do >= MINUTE_IS_DONE)
            goto bad_time;
          mult = SEC_PER_MIN;
          whatd_we_do = MINUTE_IS_DONE;
          break;

        case 's':
          mult = 1;
          whatd_we_do = SECOND_IS_DONE;
          break;
        }

      res = scale_n_add (res, val, mult);

      while (isspace ((unsigned char)*++pz))   ;
      if (*pz == NUL)
        return res;

      if (! isdigit ((unsigned char)*pz))
        break;
    }

  } while (whatd_we_do < SECOND_IS_DONE);

 bad_time:
  errno = EINVAL;
  return BAD_TIME;
}

time_t
parse_duration (char const * pz)
{
  time_t res = 0;

  while (isspace ((unsigned char)*pz)) pz++;

  do {
    if (*pz == 'P')
      {
        res = parse_period (pz + 1);
        if ((errno != 0) || (res == BAD_TIME))
          break;
        return res;
      }

    if (*pz == 'T')
      {
        res = parse_time (pz + 1);
        if ((errno != 0) || (res == BAD_TIME))
          break;
        return res;
      }

    if (! isdigit ((unsigned char)*pz))
      break;

    res = parse_non_iso8601 (pz);
    if ((errno == 0) && (res != BAD_TIME))
      return res;

  } while (0);

  fprintf (stderr, _("Invalid time duration:  %s\n"), pz);
  if (errno == 0)
    errno = EINVAL;
  return BAD_TIME;
}

/*
 * Local Variables:
 * mode: C
 * c-file-style: "gnu"
 * indent-tabs-mode: nil
 * End:
 * end of parse-duration.c */
