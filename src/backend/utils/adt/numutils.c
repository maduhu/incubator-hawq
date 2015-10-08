/*-------------------------------------------------------------------------
 *
 * numutils.c
 *	  utility functions for I/O of built-in numeric types.
 *
 *		integer:				pg_atoi, pg_itoa, pg_ltoa
 *
 * Portions Copyright (c) 1996-2009, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *
 * IDENTIFICATION
 *	  $PostgreSQL: pgsql/src/backend/utils/adt/numutils.c,v 1.77 2009/01/01 17:23:49 momjian Exp $
 *
 *-------------------------------------------------------------------------
 */
#include "postgres.h"

#include <math.h>
#include <limits.h>
#include <ctype.h>

#include "utils/builtins.h"

/*
 * pg_atoi: convert string to integer
 *
 * allows any number of leading or trailing whitespace characters.
 *
 * 'size' is the sizeof() the desired integral result (1, 2, or 4 bytes).
 *
 * c, if not 0, is a terminator character that may appear after the
 * integer (plus whitespace).  If 0, the string must end after the integer.
 *
 * Unlike plain atoi(), this will throw ereport() upon bad input format or
 * overflow.
 */
int32
pg_atoi(char *s, int size, int c)
{
	long		l;
	char	   *badp;

	/*
	 * Some versions of strtol treat the empty string as an error, but some
	 * seem not to.  Make an explicit test to be sure we catch it.
	 */
	if (s == NULL)
		elog(ERROR, "NULL pointer");
	if (*s == 0)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for integer: \"%s\"",s),
				 errOmitLocation(true)));

	errno = 0;
	l = strtol(s, &badp, 10);

	/* We made no progress parsing the string, so bail out */
	if (s == badp)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for integer: \"%s\"",s),
				 errOmitLocation(true)));

	switch (size)
	{
		case sizeof(int32):
			if (errno == ERANGE
#if defined(HAVE_LONG_INT_64)
			/* won't get ERANGE on these with 64-bit longs... */
				|| l < INT_MIN || l > INT_MAX
#endif
				)
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				errmsg("value \"%s\" is out of range for type integer", s),
				errOmitLocation(true)));
			break;
		case sizeof(int16):
			if (errno == ERANGE || l < SHRT_MIN || l > SHRT_MAX)
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				errmsg("value \"%s\" is out of range for type smallint", s),
				errOmitLocation(true)));
			break;
		case sizeof(int8):
			if (errno == ERANGE || l < SCHAR_MIN || l > SCHAR_MAX)
				ereport(ERROR,
						(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				errmsg("value \"%s\" is out of range for 8-bit integer", s),
				errOmitLocation(true)));
			break;
		default:
			elog(ERROR, "unsupported result size: %d", size);
	}

	/*
	 * Skip any trailing whitespace; if anything but whitespace remains before
	 * the terminating character, bail out
	 */
	while (*badp && *badp != c && isspace((unsigned char) *badp))
		badp++;

	if (*badp && *badp != c)
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for integer: \"%s\"",s),
				 errOmitLocation(true)));

	return (int32) l;
}

/*
 *		pg_itoa			- converts a short int to its string representation
 *
 *		Note:
 *				previously based on ~ingres/source/gutil/atoi.c
 *				now uses vendor's sprintf conversion
 */
void
pg_itoa(int16 i, char *a)
{
	/* 
	 * The standard postgres way is to sprintf, but that uses a lot of cpu.
	 * Do a fast conversion to string instead.
	 */
	char tmp[33];
	char *tp = tmp;
	char *sp;
	int ii = 0;
	unsigned long v;
	long value = i;
	bool sign = (value < 0);;
	if (sign)
		v = -value;
	else
		v = (unsigned long)value;
	while (v || tp == tmp)
	{
		ii = v % 10;
		v = v / 10;
		*tp++ = ii+'0';
	}
	sp = a;
	if (sign)
		*sp++ = '-';
	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	
}

/*
 *		pg_ltoa			- converts a long int to its string representation
 *
 *		Note:
 *				previously based on ~ingres/source/gutil/atoi.c
 *				now uses vendor's sprintf conversion
 */
void
pg_ltoa(int32 l, char *a)
{
	/* 
	 * The standard postgres way is to sprintf, but that uses a lot of cpu.
	 * Do a fast conversion to string instead.
	 */
	char tmp[33];
	char *tp = tmp;
	char *sp;
	int ii = 0;
	unsigned long v;
	long value = l;
	bool sign = (value < 0);;
	if (sign)
		v = -value;
	else
		v = (unsigned long)value;
	while (v || tp == tmp)
	{
		ii = v % 10;
		v = v / 10;
		*tp++ = ii+'0';
	}
	sp = a;
	if (sign)
		*sp++ = '-';
	while (tp > tmp)
		*sp++ = *--tp;
	*sp = 0;
	
}