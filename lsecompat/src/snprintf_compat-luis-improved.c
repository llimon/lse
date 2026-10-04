/*
 * Copyright Patrick Powell 1995
 * This code is based on code written by Patrick Powell (papowell@astart.com)
 * It may be used for any purpose as long as this notice remains intact
 * on all source code distributions
 */
#define SNPRINTF_CONST const

/**************************************************************
 * Original:
 * Patrick Powell Tue Apr 11 09:48:21 PDT 1995
 * A bombproof version of doprnt (dopr) included.
 * Sigh.  This sort of thing is always nasty do deal with.  Note that
 * the version here does not include floating point...
 *
 * snprintf() is used instead of sprintf() as it does limit checks
 * for string length.  This covers a nasty loophole.
 *
 * The other functions are there to prevent NULL pointers from
 * causing nast effects.
 *
 * More Recently:
 *  Brandon Long <blong@fiction.net> 9/15/96 for mutt 0.43
 *  This was ugly.  It is still ugly.  I opted out of floating point
 *  numbers, but the formatter understands just about everything
 *  from the normal C string format, at least as far as I can tell from
 *  the Solaris 2.5 printf(3S) man page.
 *
 *  Brandon Long <blong@fiction.net> 10/22/97 for mutt 0.87.1
 *    Ok, added some minimal floating point support, which means this
 *    probably requires libm on most operating systems.  Don't yet
 *    support the exponent (e,E) and sigfig (g,G).  Also, fmtint()
 *    was pretty badly broken, it just wasn't being exercised in ways
 *    which showed it, so that's been fixed.  Also, formated the code
 *    to mutt conventions, and removed dead code left over from the
 *    original.  Also, there is now a builtin-test, just compile with:
 *           gcc -DTEST_SNPRINTF -o snprintf snprintf.c -lm
 *    and run snprintf for results.
 * 
 *  Thomas Roessler <roessler@guug.de> 01/27/98 for mutt 0.89i
 *    The PGP code was using unsigned hexadecimal formats. 
 *    Unfortunately, unsigned formats simply didn't work.
 *
 *  Michael Elkins <me@cs.hmc.edu> 03/05/98 for mutt 0.90.8
 *    The original code assumed that both snprintf() and vsnprintf() were
 *    missing.  Some systems only have snprintf() but not vsnprintf(), so
 *    the code is now broken down under HAVE_SNPRINTF and HAVE_VSNPRINTF.
 *
 *  Andrew Tridgell (tridge@samba.org) Oct 1998
 *    fixed handling of %.0f
 *    added test for HAVE_LONG_DOUBLE
 *
 * tridge@samba.org, idra@samba.org, April 2001
 *    got rid of fcvt code (twas buggy and made testing harder)
 *    added C99 semantics
 *
 * date: 2002/12/19 19:56:31;  author: herb;  state: Exp;  lines: +2 -0
 * actually print args for %g and %e
 * 
 * date: 2002/06/03 13:37:52;  author: jmcd;  state: Exp;  lines: +8 -0
 * Since includes.h isn't included here, VA_COPY has to be defined here.  I don't
 * see any include file that is guaranteed to be here, so I'm defining it
 * locally.  Fixes AIX and Solaris builds.
 * 
 * date: 2002/06/03 03:07:24;  author: tridge;  state: Exp;  lines: +5 -13
 * put the ifdef for HAVE_VA_COPY in one place rather than in lots of
 * functions
 * 
 * date: 2002/05/17 14:51:22;  author: jmcd;  state: Exp;  lines: +21 -4
 * Fix usage of va_list passed as an arg.  Use __va_copy before using it
 * when it exists.
 * 
 * date: 2002/04/16 22:38:04;  author: idra;  state: Exp;  lines: +20 -14
 * Fix incorrect zpadlen handling in fmtfp.
 * Thanks to Ollie Oldham <ollie.oldham@metro-optix.com> for spotting it.
 * few mods to make it easier to compile the tests.
 * addedd the "Ollie" test to the floating point ones.
 *
 * Martin Pool (mbp@samba.org) April 2003
 *    Remove NO_CONFIG_H so that the test case can be built within a source
 *    tree with less trouble.
 *    Remove unnecessary SAFE_FREE() definition.
 *
 * Martin Pool (mbp@samba.org) May 2003
 *    Put in a prototype for dummy_snprintf() to quiet compiler warnings.
 *
 *    Move #endif to make sure VA_COPY, LDOUBLE, etc are defined even
 *    if the C library has some snprintf functions already.
 *
 * Damien Miller (djm@mindrot.org) Jan 2007
 *    Fix integer overflows in return value.
 *    Make formatting quite a bit faster by inlining dopr_outch()
 *
 **************************************************************/


#if defined(BROKEN_SNPRINTF)		/* For those with broken snprintf() */
# undef HAVE_SNPRINTF
# undef HAVE_VSNPRINTF
#endif

#ifndef VA_COPY
# ifdef HAVE_VA_COPY
#  define VA_COPY(dest, src) va_copy(dest, src)
# else
#  ifdef HAVE___VA_COPY
#   define VA_COPY(dest, src) __va_copy(dest, src)
#  else
#   define VA_COPY(dest, src) (dest) = (src)
#  endif
# endif
#endif

#if !defined(HAVE_SNPRINTF) || !defined(HAVE_VSNPRINTF)

#include <sys/types.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <errno.h>
#include <stddef.h>

#ifdef HAVE_LONG_DOUBLE
# define LDOUBLE long double
#else
# define LDOUBLE double
#endif

#ifdef HAVE_LONG_LONG
# define LLONG long long
#else
# define LLONG long
#endif

/*
 * dopr(): poor man's version of doprintf
 */

/* format read states */
#define DP_S_DEFAULT 0
#define DP_S_FLAGS   1
#define DP_S_MIN     2
#define DP_S_DOT     3
#define DP_S_MAX     4
#define DP_S_MOD     5
#define DP_S_CONV    6
#define DP_S_DONE    7

/* format flags - Bits */
#define DP_F_MINUS 	(1 << 0)
#define DP_F_PLUS  	(1 << 1)
#define DP_F_SPACE 	(1 << 2)
#define DP_F_NUM   	(1 << 3)
#define DP_F_ZERO  	(1 << 4)
#define DP_F_UP    	(1 << 5)
#define DP_F_UNSIGNED 	(1 << 6)

/* Conversion Flags */
#define DP_C_SHORT   1
#define DP_C_LONG    2
#define DP_C_LDOUBLE 3
#define DP_C_LLONG   4

#define char_to_int(p) ((p)- '0')
#ifndef MAX
# define MAX(p,q) (((p) >= (q)) ? (p) : (q))
#endif

#define DOPR_OUTCH(buf, pos, buflen, thechar) \
	do { \
		if (pos + 1 >= INT_MAX) { \
			errno = ERANGE; \
			return -1; \
		} \
		if (pos < buflen) \
			buf[pos] = thechar; \
		(pos)++; \
	} while (0)

static int dopr(char *buffer, size_t maxlen, const char *format, 
    va_list args_in);
static int fmtstr(char *buffer, size_t *currlen, size_t maxlen,
    char *value, int flags, int min, int max);
static int fmtint(char *buffer, size_t *currlen, size_t maxlen,
    LLONG value, int base, int min, int max, int flags);
static int fmtfp(char *buffer, size_t *currlen, size_t maxlen,
    LDOUBLE fvalue, int min, int max, int flags);

static int
dopr(char *buffer, size_t maxlen, const char *format, va_list args_in)
{
	char ch;
	LLONG value;
	LDOUBLE fvalue;
	char *strvalue;
	int min;
	int max;
	int state;
	int flags;
	int cflags;
	size_t currlen;
	va_list args;
        int base;

	VA_COPY(args, args_in);
	
	state = DP_S_DEFAULT;
	currlen = flags = cflags = min = 0;
	max = -1;
	ch = *format++;
	
	while (state != DP_S_DONE) {
		if (ch == '\0') 
			state = DP_S_DONE;

		switch(state) {
		case DP_S_DEFAULT:
			if (ch == '%') 
				state = DP_S_FLAGS;
			else
				DOPR_OUTCH(buffer, currlen, maxlen, ch);
			ch = *format++;
			break;
		case DP_S_FLAGS:
			switch (ch) {
			case '-':
				flags |= DP_F_MINUS;
				ch = *format++;
				break;
			case '+':
				flags |= DP_F_PLUS;
				ch = *format++;
				break;
			case ' ':
				flags |= DP_F_SPACE;
				ch = *format++;
				break;
			case '#':
				flags |= DP_F_NUM;
				ch = *format++;
				break;
			case '0':
				flags |= DP_F_ZERO;
				ch = *format++;
				break;
			default:
				state = DP_S_MIN;
				break;
			}
			break;
		case DP_S_MIN:
			if (isdigit((unsigned char)ch)) {
				min = 10*min + char_to_int (ch);
				ch = *format++;
			} else if (ch == '*') {
				min = va_arg (args, int);
				ch = *format++;
				state = DP_S_DOT;
			} else {
				state = DP_S_DOT;
			}
			break;
		case DP_S_DOT:
			if (ch == '.') {
				state = DP_S_MAX;
				ch = *format++;
			} else { 
				state = DP_S_MOD;
			}
			break;
		case DP_S_MAX:
			if (isdigit((unsigned char)ch)) {
				if (max < 0)
					max = 0;
				max = 10*max + char_to_int (ch);
				ch = *format++;
			} else if (ch == '*') {
				max = va_arg (args, int);
				ch = *format++;
				state = DP_S_MOD;
			} else {
				state = DP_S_MOD;
			}
			break;
		case DP_S_MOD:
			switch (ch) {
			case 'h':
				cflags = DP_C_SHORT;
				ch = *format++;
				break;
			case 'l':
				cflags = DP_C_LONG;
				ch = *format++;
				if (ch == 'l') {	/* It's a long long */
					cflags = DP_C_LLONG;
					ch = *format++;
				}
				break;
			case 'z':                       /* C99 size_t / ssize_t */
                                if (sizeof(size_t) == sizeof(LLONG))
                                        cflags = DP_C_LLONG;
                                else 
                                        cflags = DP_C_LONG;
                                ch = *format++;
                                break;
                        case 'j':                       /* C99 intmax_t / uintmax_t */
                                cflags = DP_C_LLONG;
                                ch = *format++;
                                break;
                        case 't':                       /* C99 ptrdiff_t */
                                cflags = DP_C_LONG;     /* Always 32-bit long on SPARC V7/V8 */
                                ch = *format++;
                                break;

			case 'L':
				cflags = DP_C_LDOUBLE;
				ch = *format++;
				break;
			default:
				break;
			}
			state = DP_S_CONV;
			break;
		case DP_S_CONV:
			switch (ch) {
			case 'd':
			case 'i':
				if (cflags == DP_C_SHORT) 
					value = va_arg (args, int);
				else if (cflags == DP_C_LONG)
					value = va_arg (args, long int);
				else if (cflags == DP_C_LLONG)
					value = va_arg (args, LLONG);
				else
					value = va_arg (args, int);
				if (fmtint(buffer, &currlen, maxlen,
				    value, 10, min, max, flags) == -1)
					return -1;
				break;
			case 'o':
			case 'u':
			case 'X':
			case 'x':
				if (cflags == DP_C_SHORT)
                                        value = va_arg (args, unsigned int);
                                else if (cflags == DP_C_LONG)
                                        value = va_arg (args, unsigned long int);
                                else if (cflags == DP_C_LLONG)
                                        value = va_arg (args, unsigned LLONG);
                                else
                                        value = va_arg (args, unsigned int);

                                base = (ch == 'u') ? 10 : ((ch == 'o') ? 8 : 16);
                                if (fmtint(buffer, &currlen, maxlen,
                                    value, base, min, max, flags) == -1)
                                        return -1;
                                break;
			case 'f':
				if (cflags == DP_C_LDOUBLE)
					fvalue = va_arg (args, LDOUBLE);
				else
					fvalue = va_arg (args, double);
				if (fmtfp(buffer, &currlen, maxlen, fvalue,
				    min, max, flags) == -1)
					return -1;
				break;
			case 'E':
				flags |= DP_F_UP;
			case 'e':
				if (cflags == DP_C_LDOUBLE)
					fvalue = va_arg (args, LDOUBLE);
				else
					fvalue = va_arg (args, double);
				if (fmtfp(buffer, &currlen, maxlen, fvalue,
				    min, max, flags) == -1)
					return -1;
				break;
			case 'G':
				flags |= DP_F_UP;
			case 'g':
				if (cflags == DP_C_LDOUBLE)
					fvalue = va_arg (args, LDOUBLE);
				else
					fvalue = va_arg (args, double);
				if (fmtfp(buffer, &currlen, maxlen, fvalue,
				    min, max, flags) == -1)
					return -1;
				break;
			case 'c':
				DOPR_OUTCH(buffer, currlen, maxlen,
				    va_arg (args, int));
				break;
			case 's':
				strvalue = va_arg (args, char *);
				if (!strvalue) strvalue = "(NULL)";
				if (max == -1) {
					max = strlen(strvalue);
				}
				if (min > 0 && max >= 0 && min > max) max = min;
				if (fmtstr(buffer, &currlen, maxlen,
				    strvalue, flags, min, max) == -1)
					return -1;
				break;
			case 'p':
				strvalue = va_arg (args, void *);
				if (fmtint(buffer, &currlen, maxlen,
				    (long) strvalue, 16, min, max, flags) == -1)
					return -1;
				break;
			case 'n':
				if (cflags == DP_C_SHORT) {
					short int *num;
					num = va_arg (args, short int *);
					*num = currlen;
				} else if (cflags == DP_C_LONG) {
					long int *num;
					num = va_arg (args, long int *);
					*num = (long int)currlen;
				} else if (cflags == DP_C_LLONG) {
					LLONG *num;
					num = va_arg (args, LLONG *);
					*num = (LLONG)currlen;
				} else {
					int *num;
					num = va_arg (args, int *);
					*num = currlen;
				}
				break;
			case '%':
				DOPR_OUTCH(buffer, currlen, maxlen, ch);
				break;
			case 'w':
				/* not supported yet, treat as next char */
				ch = *format++;
				break;
			default:
				/* Unknown, skip */
				break;
			}
			ch = *format++;
			state = DP_S_DEFAULT;
			flags = cflags = min = 0;
			max = -1;
			break;
		case DP_S_DONE:
			break;
		default:
			/* hmm? */
			break; /* some picky compilers need this */
		}
	}
	if (maxlen != 0) {
		if (currlen < maxlen - 1) 
			buffer[currlen] = '\0';
		else if (maxlen > 0) 
			buffer[maxlen - 1] = '\0';
	}
	
	return currlen < INT_MAX ? (int)currlen : -1;
}

static int
fmtstr(char *buffer, size_t *currlen, size_t maxlen,
    char *value, int flags, int min, int max)
{
	int padlen, strln;     /* amount to pad */
	int cnt = 0;

#ifdef DEBUG_SNPRINTF
	printf("fmtstr min=%d max=%d s=[%s]\n", min, max, value);
#endif
	if (value == 0) {
		value = "<NULL>";
	}

	for (strln = 0; strln < max && value[strln]; ++strln); /* strlen */
	padlen = min - strln;
	if (padlen < 0) 
		padlen = 0;
	if (flags & DP_F_MINUS) 
		padlen = -padlen; /* Left Justify */
	
	while ((padlen > 0) && (cnt < max)) {
		DOPR_OUTCH(buffer, *currlen, maxlen, ' ');
		--padlen;
		++cnt;
	}
	while (*value && (cnt < max)) {
		DOPR_OUTCH(buffer, *currlen, maxlen, *value);
		*value++;
		++cnt;
	}
	while ((padlen < 0) && (cnt < max)) {
		DOPR_OUTCH(buffer, *currlen, maxlen, ' ');
		++padlen;
		++cnt;
	}
	return 0;
}

/* Have to handle DP_F_NUM (ie 0x and 0 alternates) */

static int
fmtint(char *buffer, size_t *currlen, size_t maxlen,
       LLONG value, int base, int min, int max, int flags)
{
        int signvalue = 0;
        unsigned LLONG uvalue;
        char convert[20];
        int place = 0;
        int spadlen = 0; /* amount to space pad */
        int zpadlen = 0; /* amount to zero pad */
        int caps = 0;
        int zero_precision = (max == 0); /* Track explicit .0 precision */
        int has_precision = (max >= 0);  /* Track if precision was set */

        /* C99 Rule: If precision is specified for integers, ignore '0' flag */
        if (has_precision) {
                flags &= ~DP_F_ZERO;
        }
        
        if (max < 0)
                max = 0;
        
        uvalue = value;
        
        if(!(flags & DP_F_UNSIGNED)) {
                if( value < 0 ) {
                        signvalue = '-';
                        uvalue = -value;
                } else {
                        if (flags & DP_F_PLUS)  /* Do a sign (+/i) */
                                signvalue = '+';
                        else if (flags & DP_F_SPACE)
                                signvalue = ' ';
                }
        }
  
        if (flags & DP_F_UP) caps = 1; /* Should characters be upper case? */

        /* C99 Rule: If value is 0 and precision is explicitly .0, produce NO digits */
        if (uvalue == 0 && zero_precision) {
                place = 0;
        } else {
                do {
                        convert[place++] =
                                (caps? "0123456789ABCDEF":"0123456789abcdef")
                                [uvalue % (unsigned)base  ];
                        uvalue = (uvalue / (unsigned)base );
                } while(uvalue && (place < 20));
                if (place == 20) place--;
                convert[place] = 0;
        }

        zpadlen = max - place;
        spadlen = min - MAX (max, place) - (signvalue ? 1 : 0);
        if (zpadlen < 0) zpadlen = 0;
        if (spadlen < 0) spadlen = 0;
        if (flags & DP_F_ZERO) {
                zpadlen = MAX(zpadlen, spadlen);
                spadlen = 0;
        }
        if (flags & DP_F_MINUS) 
                spadlen = -spadlen; /* Left Justify */

#ifdef DEBUG_SNPRINTF
        printf("zpad: %d, spad: %d, min: %d, max: %d, place: %d\n",
               zpadlen, spadlen, min, max, place);
#endif

        /* Spaces */
        while (spadlen > 0) {
                DOPR_OUTCH(buffer, *currlen, maxlen, ' ');
                --spadlen;
        }

        /* Sign */
        if (signvalue) 
                DOPR_OUTCH(buffer, *currlen, maxlen, signvalue);

        /* Zeros */
        if (zpadlen > 0) {
                while (zpadlen > 0) {
                        DOPR_OUTCH(buffer, *currlen, maxlen, '0');
                        --zpadlen;
                }
        }

        /* Digits */
        while (place > 0) {
                --place;
                DOPR_OUTCH(buffer, *currlen, maxlen, convert[place]);
        }
  
        /* Left Justified spaces */
        while (spadlen < 0) {
                DOPR_OUTCH(buffer, *currlen, maxlen, ' ');
                ++spadlen;
        }
        return 0;
}

static LDOUBLE abs_val(LDOUBLE value)
{
	LDOUBLE result = value;

	if (value < 0)
		result = -value;
	
	return result;
}

static LDOUBLE POW10(int val)
{
	LDOUBLE result = 1;
	
	while (val) {
		result *= 10;
		val--;
	}
  
	return result;
}

static LLONG ROUND(LDOUBLE value)
{
	LLONG intpart;

	intpart = (LLONG)value;
	value = value - intpart;
	if (value >= 0.5) intpart++;
	
	return intpart;
}

/* a replacement for modf that doesn't need the math library. Should
   be portable, but slow */
static double my_modf(double x0, double *iptr)
{
	int i;
	long l;
	double x = x0;
	double f = 1.0;

	for (i=0;i<100;i++) {
		l = (long)x;
		if (l <= (x+1) && l >= (x-1)) break;
		x *= 0.1;
		f *= 10.0;
	}

	if (i == 100) {
		/*
		 * yikes! the number is beyond what we can handle.
		 * What do we do?
		 */
		(*iptr) = 0;
		return 0;
	}

	if (i != 0) {
		double i2;
		double ret;

		ret = my_modf(x0-l*f, &i2);
		(*iptr) = l*f + i2;
		return ret;
	} 

	(*iptr) = l;
	return x - (*iptr);
}


static int
fmtfp (char *buffer, size_t *currlen, size_t maxlen,
    LDOUBLE fvalue, int min, int max, int flags)
{
	int signvalue = 0;
	double ufvalue;
	char iconvert[311];
	char fconvert[311];
	int iplace = 0;
	int fplace = 0;
	int padlen = 0; /* amount to pad */
	int zpadlen = 0; 
	int caps = 0;
	int idx;
	double intpart;
	double fracpart;
	double temp;
  
	/* 
	 * AIX manpage says the default is 0, but Solaris says the default
	 * is 6, and sprintf on AIX defaults to 6
	 */
	if (max < 0)
		max = 6;

	ufvalue = abs_val (fvalue);

	if (fvalue < 0) {
		signvalue = '-';
	} else {
		if (flags & DP_F_PLUS) { /* Do a sign (+/i) */
			signvalue = '+';
		} else {
			if (flags & DP_F_SPACE)
				signvalue = ' ';
		}
	}

#if 0
	if (flags & DP_F_UP) caps = 1; /* Should characters be upper case? */
#endif

#if 0
	 if (max == 0) ufvalue += 0.5; /* if max = 0 we must round */
#endif

	/* 
	 * Sorry, we only support 16 digits past the decimal because of our 
	 * conversion method
	 */
	if (max > 16)
		max = 16;

	/* We "cheat" by converting the fractional part to integer by
	 * multiplying by a factor of 10
	 */

	temp = ufvalue;
	my_modf(temp, &intpart);

	fracpart = ROUND((POW10(max)) * (ufvalue - intpart));
	
	if (fracpart >= POW10(max)) {
		intpart++;
		fracpart -= POW10(max);
	}

	/* Convert integer part */
	do {
		temp = intpart*0.1;
		my_modf(temp, &intpart);
		idx = (int) ((temp -intpart +0.05)* 10.0);
		/* idx = (int) (((double)(temp*0.1) -intpart +0.05) *10.0); */
		/* printf ("%llf, %f, %x\n", temp, intpart, idx); */
		iconvert[iplace++] =
			(caps? "0123456789ABCDEF":"0123456789abcdef")[idx];
	} while (intpart && (iplace < 311));
	if (iplace == 311) iplace--;
	iconvert[iplace] = 0;

	/* Convert fractional part */
	if (fracpart)
	{
		do {
			temp = fracpart*0.1;
			my_modf(temp, &fracpart);
			idx = (int) ((temp -fracpart +0.05)* 10.0);
			/* idx = (int) ((((temp/10) -fracpart) +0.05) *10); */
			/* printf ("%lf, %lf, %ld\n", temp, fracpart, idx ); */
			fconvert[fplace++] =
			(caps? "0123456789ABCDEF":"0123456789abcdef")[idx];
		} while(fracpart && (fplace < 311));
		if (fplace == 311) fplace--;
	}
	fconvert[fplace] = 0;
  
	/* -1 for decimal point, another -1 if we are printing a sign */
	padlen = min - iplace - max - 1 - ((signvalue) ? 1 : 0); 
	zpadlen = max - fplace;
	if (zpadlen < 0) zpadlen = 0;
	if (padlen < 0) 
		padlen = 0;
	if (flags & DP_F_MINUS) 
		padlen = -padlen; /* Left Justifty */
	
	if ((flags & DP_F_ZERO) && (padlen > 0)) {
		if (signvalue) {
			DOPR_OUTCH(buffer, *currlen, maxlen, signvalue);
			--padlen;
			signvalue = 0;
		}
		while (padlen > 0) {
			DOPR_OUTCH(buffer, *currlen, maxlen, '0');
			--padlen;
		}
	}
	while (padlen > 0) {
		DOPR_OUTCH(buffer, *currlen, maxlen, ' ');
		--padlen;
	}
	if (signvalue) 
		DOPR_OUTCH(buffer, *currlen, maxlen, signvalue);
	
	while (iplace > 0) {
		--iplace;
		DOPR_OUTCH(buffer, *currlen, maxlen, iconvert[iplace]);
	}

#ifdef DEBUG_SNPRINTF
	printf("fmtfp: fplace=%d zpadlen=%d\n", fplace, zpadlen);
#endif

	/*
	 * Decimal point.  This should probably use locale to find the correct
	 * char to print out.
	 */
	if (max > 0) {
		DOPR_OUTCH(buffer, *currlen, maxlen, '.');
		
		while (zpadlen > 0) {
			DOPR_OUTCH(buffer, *currlen, maxlen, '0');
			--zpadlen;
		}

		while (fplace > 0) {
			--fplace;
			DOPR_OUTCH(buffer, *currlen, maxlen, fconvert[fplace]);
		}
	}

	while (padlen < 0) {
		DOPR_OUTCH(buffer, *currlen, maxlen, ' ');
		++padlen;
	}
	return 0;
}
#endif /* !defined(HAVE_SNPRINTF) || !defined(HAVE_VSNPRINTF) */

#if !defined(HAVE_ASPRINTF)
int asprintf(char **ptr, const char *fmt, /*args*/ ...) {
  va_list ap;
  size_t str_m;
  int str_l;

  *ptr = NULL;
  va_start(ap, fmt);                            /* measure the required size */
  str_l = vsnprintf(NULL, (size_t)0, fmt, ap);
  va_end(ap);
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  *ptr = (char *) malloc(str_m = (size_t)str_l + 1);
  if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
  else {
    int str_l2;
    va_start(ap, fmt);
    str_l2 = vsnprintf(*ptr, str_m, fmt, ap);
    va_end(ap);
    assert(str_l2 == str_l);
  }
  return str_l;
}
#endif

#if !defined(HAVE_VASPRINTF)
int vasprintf(char **ptr, const char *fmt, va_list ap) {
  size_t str_m;
  int str_l;

  *ptr = NULL;
  { va_list ap2;
    va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
    str_l = vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
    va_end(ap2);
  }
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  *ptr = (char *) malloc(str_m = (size_t)str_l + 1);
  if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
  else {
    int str_l2 = vsnprintf(*ptr, str_m, fmt, ap);
    assert(str_l2 == str_l);
  }
  return str_l;
}
#endif

#if !defined(HAVE_ASNPRINTF)
int asnprintf (char **ptr, size_t str_m, const char *fmt, /*args*/ ...) {
  va_list ap;
  int str_l;

  *ptr = NULL;
  va_start(ap, fmt);                            /* measure the required size */
  str_l = vsnprintf(NULL, (size_t)0, fmt, ap);
  va_end(ap);
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;      /* truncate */
  /* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
  if (str_m == 0) {  /* not interested in resulting string, just return size */
  } else {
    *ptr = (char *) malloc(str_m);
    if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
    else {
      int str_l2;
      va_start(ap, fmt);
      str_l2 = vsnprintf(*ptr, str_m, fmt, ap);
      va_end(ap);
      assert(str_l2 == str_l);
    }
  }
  return str_l;
}
#endif

#if !defined(HAVE_VASNPRINTF)
int vasnprintf (char **ptr, size_t str_m, const char *fmt, va_list ap) {
  int str_l;

  *ptr = NULL;
  { va_list ap2;
    va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
    str_l = vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
    va_end(ap2);
  }
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;      /* truncate */
  /* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
  if (str_m == 0) {  /* not interested in resulting string, just return size */
  } else {
    *ptr = (char *) malloc(str_m);
    if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
    else {
      int str_l2 = vsnprintf(*ptr, str_m, fmt, ap);
      assert(str_l2 == str_l);
    }
  }
  return str_l;
}
#endif


#if !defined(HAVE_VSNPRINTF)
int
vsnprintf (char *str, size_t count, const char *fmt, va_list args)
{
	return dopr(str, count, fmt, args);
}
#endif

#if !defined(HAVE_SNPRINTF)
int
snprintf(char *str, size_t count, SNPRINTF_CONST char *fmt, ...)
{
	size_t ret;
	va_list ap;

	va_start(ap, fmt);
	ret = vsnprintf(str, count, fmt, ap);
	va_end(ap);
	return ret;
}
#endif

/* Embedded Standalone Unit Test */
#ifdef _TEST_SNPRINTF_COMPAT

/* Helper wrapper to validate vasprintf */
static int test_vasprintf_helper(char **strp, const char *fmt, ...) {
    va_list ap;
    int ret;
    va_start(ap, fmt);
    ret = vasprintf(strp, fmt, ap);
    va_end(ap);
    return ret;
}

/* =========================================================================
 * Embedded Unit Test Harness
 * Compile test: gcc -O2 -mcpu=v7 -DHAVE_LONG_DOUBLE -DHAVE_LONG_LONG -D_TEST_SNPRINTF_COMPAT snprintf_compat.c -o test_snprintf
 * ========================================================================= */
int main(void) {
    char buf[128];
    int len;
    int n_count = -1;
    long long big_num = 9223372036854775807LL;
    long double ld_val = 3.141592653589793238462643383279502884L;
    size_t sz_val = 1024;
    char *dyn_buf;

    printf("=== Solaris / SunOS Comprehensive C99 Stress Suite ===\n");

    /* --- BASE C99 COMPLIANCE CHECKS --- */

    /* 1. 64-bit Long Long Formatting */
    len = snprintf(buf, sizeof(buf), "LLMAX: %lld", big_num);
    printf("[01] 64-bit int: '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "LLMAX: 9223372036854775807") == 0);

    /* 2. Truncation & Null Termination Boundary */
    len = snprintf(buf, 10, "1234567890ABCDEF");
    printf("[02] Truncation (bound 10): '%s' (reported len: %d)\n", buf, len);
    assert(strlen(buf) == 9 && strcmp(buf, "123456789") == 0 && len == 16);

    /* 3. Long Double Formatting (%Lf) */
    len = snprintf(buf, sizeof(buf), "LD: %.10Lf", ld_val);
    printf("[03] Long double: '%s' (len: %d)\n", buf, len);
    assert(strstr(buf, "3.1415926536") != NULL);

    /* 4. %n Specifier */
    len = snprintf(buf, sizeof(buf), "Hello %nWorld", &n_count);
    printf("[04] %%n count: '%s' (written %%n: %d, total len: %d)\n", buf, n_count, len);
    assert(n_count == 6 && len == 11 && strcmp(buf, "Hello World") == 0);

    /* 5. NULL-Buffer Dry Run Length Query */
    len = snprintf(NULL, 0, "Test %d string", 123);
    printf("[05] NULL-buffer length query: %d\n", len);
    assert(len == 15);

    /* 6. C99 size_t modifier (%zu) */
    len = snprintf(buf, sizeof(buf), "Size: %zu", sz_val);
    printf("[06] C99 size_t (%%zu): '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "Size: 1024") == 0);


    /* --- ADVANCED EDGE-CASE STRESS TESTS --- */

    /* 7. Signed ssize_t / ptrdiff_t Formatting (%zd, %td) */
    len = snprintf(buf, sizeof(buf), "SSize: %zd, Diff: %td", (ssize_t)-512, (ptrdiff_t)-42);
    printf("[07] Signed %%zd / %%td: '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "SSize: -512, Diff: -42") == 0);

    /* 8. Hexadecimal size_t Formatting (%zx / %zX) */
    len = snprintf(buf, sizeof(buf), "HexSize: 0x%zx", (size_t)0xDEADBEEF);
    printf("[08] Hex size_t (%%zx): '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "HexSize: 0xdeadbeef") == 0);

    /* 9. C99 Precision Zero Rule (value 0 with precision .0 MUST produce 0 chars) */
    len = snprintf(buf, sizeof(buf), "ZeroPrec: '%.0d'", 0);
    printf("[09] Zero value with .0 precision: '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "ZeroPrec: ''") == 0);

    /* 10. Width + Precision Zero Padding Combination (%010.5d) */
    len = snprintf(buf, sizeof(buf), "Padded: '%010.5d'", 42);
    printf("[10] Width + Precision padding: '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "Padded: '     00042'") == 0); /* C99: precision overrides '0' flag */

    /* 11. Left Alignment + Field Width (%-10s) */
    len = snprintf(buf, sizeof(buf), "Left: '%-10s'", "sparc");
    printf("[11] Left alignment (%%-10s): '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "Left: 'sparc     '") == 0);

    /* 12. Dynamic Allocation via asprintf */
    dyn_buf = NULL;
    len = asprintf(&dyn_buf, "Dynamic %s %lld", "Alloc", big_num);
    printf("[12] asprintf dynamic alloc: '%s' (len: %d)\n", dyn_buf, len);
    assert(dyn_buf != NULL);
    assert(strcmp(dyn_buf, "Dynamic Alloc 9223372036854775807") == 0);
    assert(len == 33);
    free(dyn_buf);

    /* 13. Dynamic Allocation via vasprintf */
    dyn_buf = NULL;
    len = test_vasprintf_helper(&dyn_buf, "Size: %zu, Hex: 0x%zx", sz_val, (size_t)0xDEADBEEF);
    printf("[13] vasprintf dynamic alloc: '%s' (len: %d)\n", dyn_buf, len);
    assert(dyn_buf != NULL);
    assert(strcmp(dyn_buf, "Size: 1024, Hex: 0xdeadbeef") == 0);
    assert(len == 27);
    free(dyn_buf);

    printf("\nSUCCESS: All 13 C99 & POSIX string formatting tests passed cleanly!\n");
    return 0;
}
#endif /* _TEST_SNPRINTF_COMPAT */
