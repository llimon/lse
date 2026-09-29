#include "snprintf_compat.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>

/* Lock in modern 64-bit long long & Solaris behavior defaults */
#ifndef SNPRINTF_LONGLONG_SUPPORT
#define SNPRINTF_LONGLONG_SUPPORT
#endif

#ifndef SOLARIS_COMPATIBLE
#define SOLARIS_COMPATIBLE
#endif

#ifndef SOLARIS_BUG_COMPATIBLE
#define SOLARIS_BUG_COMPATIBLE
#endif

#ifndef NEED_ASPRINTF
#define NEED_ASPRINTF
#endif

#ifndef NEED_VASPRINTF
#define NEED_VASPRINTF
#endif

#ifndef NEED_ASNPRINTF
#define NEED_ASNPRINTF
#endif

#ifndef NEED_VASNPRINTF
#define NEED_VASNPRINTF
#endif

#if defined(__GNUC__)
#  if defined(__ELF__) || defined(__solaris__) || defined(SOLARIS2)
     /* ELF systems: Use pragma weak */
#    pragma weak snprintf
#    pragma weak vsnprintf
#    pragma weak asprintf
#    pragma weak vasprintf
#    pragma weak asnprintf
#    pragma weak vasnprintf
#  elif defined(__aout__) || defined(sun) || defined(__sunos__)
     /* SunOS 4 / a.out systems: Standard declarations without weak attributes */
     int snprintf(char *str, size_t str_m, const char *fmt, ...);
     int vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap);
     int asprintf(char **ptr, const char *fmt, ...);
     int vasprintf(char **ptr, const char *fmt, va_list ap);
     int asnprintf(char **ptr, size_t str_m, const char *fmt, ...);
     int vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap);
#  else
     /* Fallback for other GCC platforms supporting weak attributes */
     int snprintf(char *str, size_t str_m, const char *fmt, ...)          __attribute__((weak));
     int vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap)  __attribute__((weak));
     int asprintf(char **ptr, const char *fmt, ...)                       __attribute__((weak));
     int vasprintf(char **ptr, const char *fmt, va_list ap)              __attribute__((weak));
     int asnprintf(char **ptr, size_t str_m, const char *fmt, ...)        __attribute__((weak));
     int vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap) __attribute__((weak));
#  endif
#endif
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#ifdef isdigit
#undef isdigit
#endif
#define isdigit(c) ((c) >= '0' && (c) <= '9')

#if defined(__sparc__) || defined(__sparc)
#  define breakeven_point  33
#else
#  define breakeven_point  6
#endif

#define fast_memcpy(d,s,n) { register size_t nn = (size_t)(n); if (nn >= breakeven_point) memcpy((d), (s), nn); else if (nn > 0) { register char *dd; register const char *ss; for (ss=(s), dd=(d); nn>0; nn--) *dd++ = *ss++; } }

#define fast_memset(d,c,n) { register size_t nn = (size_t)(n); if (nn >= breakeven_point) memset((d), (int)(c), nn); else if (nn > 0) { register char *dd; register const int cc=(int)(c); for (dd=(d); nn>0; nn--) *dd++ = cc; } }

#ifndef va_copy
#  ifdef __va_copy
#    define va_copy(d,s) __va_copy(d,s)
#  else
#    define va_copy(d,s) ((d) = (s))
#  endif
#endif

static char credits[] = 
    "\n"
    "@(#)snprintf.c, v2.2: Mark Martinec, <mark.martinec@ijs.si>\n"
    "@(#)snprintf.c, v2.2: Copyright 1999, Mark Martinec. Frontier Artistic License applies.\n";

int vasprintf(char **ptr, const char *fmt, va_list ap) {
  size_t str_m;
  int str_l;

  *ptr = NULL;
  { va_list ap2;
    va_copy(ap2, ap);
    str_l = vsnprintf(NULL, (size_t)0, fmt, ap2);
    va_end(ap2);
  }
  assert(str_l >= 0);
  *ptr = (char *) malloc(str_m = (size_t)str_l + 1);
  if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
  else {
    int str_l2 = vsnprintf(*ptr, str_m, fmt, ap);
    assert(str_l2 == str_l);
  }
  return str_l;
}

int asprintf(char **ptr, const char *fmt, ...) {
  va_list ap;
  int str_l;

  va_start(ap, fmt);
  str_l = vasprintf(ptr, fmt, ap);
  va_end(ap);
  return str_l;
}

int vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap) {
  int str_l;

  *ptr = NULL;
  { va_list ap2;
    va_copy(ap2, ap);
    str_l = vsnprintf(NULL, (size_t)0, fmt, ap2);
    va_end(ap2);
  }
  assert(str_l >= 0);
  if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;
  if (str_m == 0) {
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

int asnprintf(char **ptr, size_t str_m, const char *fmt, ...) {
  va_list ap;
  int str_l;

  va_start(ap, fmt);
  str_l = vasnprintf(ptr, str_m, fmt, ap);
  va_end(ap);
  return str_l;
}

int snprintf(char *str, size_t str_m, const char *fmt, ...) {
  va_list ap;
  int str_l;

  va_start(ap, fmt);
  str_l = vsnprintf(str, str_m, fmt, ap);
  va_end(ap);
  return str_l;
}

int vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap) {
  size_t str_l = 0;
  const char *p = fmt;

  if (!p) p = "";
  while (*p) {
    if (*p != '%') {
      const char *q = strchr(p+1,'%');
      size_t n = !q ? strlen(p) : (q-p);
      if (str_l < str_m) {
        size_t avail = str_m-str_l;
        fast_memcpy(str+str_l, p, (n>avail?avail:n));
      }
      p += n; str_l += n;
    } else {
      const char *starting_p;
      size_t min_field_width = 0, precision = 0;
      int zero_padding = 0, precision_specified = 0, justify_left = 0;
      int alternate_form = 0, force_sign = 0;
      int space_for_positive = 1;
      char length_modifier = '\0';
      char tmp[32];

      const char *str_arg;
      size_t str_arg_l;
      unsigned char uchar_arg;
      size_t number_of_zeros_to_pad = 0;
      size_t zero_padding_insertion_ind = 0;
      char fmt_spec = '\0';

      str_arg = credits;
      str_arg = NULL;
      starting_p = p; p++;

      while (*p == '0' || *p == '-' || *p == '+' ||
             *p == ' ' || *p == '#' || *p == '\'') {
        switch (*p) {
        case '0': zero_padding = 1; break;
        case '-': justify_left = 1; break;
        case '+': force_sign = 1; space_for_positive = 0; break;
        case ' ': force_sign = 1; break;
        case '#': alternate_form = 1; break;
        case '\'': break;
        }
        p++;
      }

      if (*p == '*') {
        int j;
        p++; j = va_arg(ap, int);
        if (j >= 0) min_field_width = j;
        else { min_field_width = -j; justify_left = 1; }
      } else if (isdigit((int)(*p))) {
        unsigned int uj = *p++ - '0';
        while (isdigit((int)(*p))) uj = 10*uj + (unsigned int)(*p++ - '0');
        min_field_width = uj;
      }

      if (*p == '.') {
        p++; precision_specified = 1;
        if (*p == '*') {
          int j = va_arg(ap, int);
          p++;
          if (j >= 0) precision = j;
          else {
            precision_specified = 0; precision = 0;
          }
        } else if (isdigit((int)(*p))) {
          unsigned int uj = *p++ - '0';
          while (isdigit((int)(*p))) uj = 10*uj + (unsigned int)(*p++ - '0');
          precision = uj;
        }
      }

      if (*p == 'h' || *p == 'l') {
        length_modifier = *p; p++;
        if (length_modifier == 'l' && *p == 'l') {
#ifdef SNPRINTF_LONGLONG_SUPPORT
          length_modifier = '2';
#else
          length_modifier = 'l';
#endif
          p++;
        }
      }
      fmt_spec = *p;

      switch (fmt_spec) {
      case 'i': fmt_spec = 'd'; break;
      case 'D': fmt_spec = 'd'; length_modifier = 'l'; break;
      case 'U': fmt_spec = 'u'; length_modifier = 'l'; break;
      case 'O': fmt_spec = 'o'; length_modifier = 'l'; break;
      default: break;
      }

      switch (fmt_spec) {
      case '%':
      case 'c':
      case 's':
        length_modifier = '\0';
        str_arg_l = 1;
        switch (fmt_spec) {
        case '%':
          str_arg = p; break;
        case 'c': {
          int j = va_arg(ap, int);
          uchar_arg = (unsigned char) j;
          str_arg = (const char *) &uchar_arg;
          break;
        }
        case 's':
          str_arg = va_arg(ap, const char *);
          if (!str_arg) str_arg_l = 0;
          else if (!precision_specified) str_arg_l = strlen(str_arg);
          else if (precision == 0) str_arg_l = 0;
          else {
            const char *q = memchr(str_arg, '\0',
                              precision <= 0x7fffffff ? precision : 0x7fffffff);
            str_arg_l = !q ? precision : (q-str_arg);
          }
          break;
        default: break;
        }
        break;
      case 'd': case 'u': case 'o': case 'x': case 'X': case 'p': {
        int arg_sign = 0;
        int int_arg = 0; unsigned int uint_arg = 0;
        long int long_arg = 0; unsigned long int ulong_arg = 0;
        void *ptr_arg = NULL;

#ifdef SNPRINTF_LONGLONG_SUPPORT
        long long int long_long_arg = 0;
        unsigned long long int ulong_long_arg = 0;
#endif
        if (fmt_spec == 'p') {
          if (length_modifier == '2') length_modifier = '\0';
          ptr_arg = va_arg(ap, void *);
          if (ptr_arg != NULL) arg_sign = 1;
        } else if (fmt_spec == 'd') {
          switch (length_modifier) {
          case '\0':
          case 'h':
            int_arg = va_arg(ap, int);
            if      (int_arg > 0) arg_sign =  1;
            else if (int_arg < 0) arg_sign = -1;
            break;
          case 'l':
            long_arg = va_arg(ap, long int);
            if      (long_arg > 0) arg_sign =  1;
            else if (long_arg < 0) arg_sign = -1;
            break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
          case '2':
            long_long_arg = va_arg(ap, long long int);
            if      (long_long_arg > 0) arg_sign =  1;
            else if (long_long_arg < 0) arg_sign = -1;
            break;
#endif
          }
        } else {
          switch (length_modifier) {
          case '\0':
          case 'h':
            uint_arg = va_arg(ap, unsigned int);
            if (uint_arg) arg_sign = 1;
            break;
          case 'l':
            ulong_arg = va_arg(ap, unsigned long int);
            if (ulong_arg) arg_sign = 1;
            break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
          case '2':
            ulong_long_arg = va_arg(ap, unsigned long long int);
            if (ulong_long_arg) arg_sign = 1;
            break;
#endif
          }
        }
        str_arg = tmp; str_arg_l = 0;

        if (precision_specified) zero_padding = 0;

        if (fmt_spec == 'd') {
          if (force_sign && arg_sign >= 0)
            tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
        } else if (alternate_form) {
          if (arg_sign != 0 && (fmt_spec == 'x' || fmt_spec == 'X') )
            { tmp[str_arg_l++] = '0'; tmp[str_arg_l++] = fmt_spec; }
        }
        zero_padding_insertion_ind = str_arg_l;
        if (!precision_specified) precision = 1;
        if (precision == 0 && arg_sign == 0) {
        } else {
          char f[5]; int f_l = 0;
          f[f_l++] = '%';
          if (!length_modifier) { }
          else if (length_modifier=='2') { f[f_l++] = 'l'; f[f_l++] = 'l'; }
          else f[f_l++] = length_modifier;
          f[f_l++] = fmt_spec; f[f_l++] = '\0';
          if (fmt_spec == 'p') str_arg_l += sprintf(tmp+str_arg_l, f, ptr_arg);
          else if (fmt_spec == 'd') {
            switch (length_modifier) {
            case '\0':
            case 'h': str_arg_l+=sprintf(tmp+str_arg_l, f, int_arg);  break;
            case 'l': str_arg_l+=sprintf(tmp+str_arg_l, f, long_arg); break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
            case '2': str_arg_l+=sprintf(tmp+str_arg_l, f, long_long_arg); break;
#endif
            }
          } else {
            switch (length_modifier) {
            case '\0':
            case 'h': str_arg_l+=sprintf(tmp+str_arg_l, f, uint_arg);  break;
            case 'l': str_arg_l+=sprintf(tmp+str_arg_l, f, ulong_arg); break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
            case '2': str_arg_l+=sprintf(tmp+str_arg_l, f, ulong_long_arg);break;
#endif
            }
          }
          if (zero_padding_insertion_ind < str_arg_l &&
              tmp[zero_padding_insertion_ind] == '-') {
            zero_padding_insertion_ind++;
          }
          if (zero_padding_insertion_ind+1 < str_arg_l &&
              tmp[zero_padding_insertion_ind]   == '0' &&
             (tmp[zero_padding_insertion_ind+1] == 'x' ||
              tmp[zero_padding_insertion_ind+1] == 'X') ) {
            zero_padding_insertion_ind += 2;
          }
        }
        { size_t num_of_digits = str_arg_l - zero_padding_insertion_ind;
          if (alternate_form && fmt_spec == 'o'
              && !(zero_padding_insertion_ind < str_arg_l
                   && tmp[zero_padding_insertion_ind] == '0')
          ) {
            if (!precision_specified || precision < num_of_digits+1) {
              precision = num_of_digits+1; precision_specified = 1;
            }
          }
          if (num_of_digits < precision)
            number_of_zeros_to_pad = precision - num_of_digits;
        }
        if (!justify_left && zero_padding) {
          int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
          if (n > 0) number_of_zeros_to_pad += n;
        }
        break;
      }
      default:
        zero_padding = 0;
        str_arg = p; str_arg_l = 0;
        if (*p) str_arg_l++;
        break;
      }
      if (*p) p++;
      if (!justify_left) {
        int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            fast_memset(str+str_l, (zero_padding?'0':' '), (n>avail?avail:n));
          }
          str_l += n;
        }
      }
      if (number_of_zeros_to_pad <= 0) {
        zero_padding_insertion_ind = 0;
      } else {
        int n = zero_padding_insertion_ind;
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            fast_memcpy(str+str_l, str_arg, (n>avail?avail:n));
          }
          str_l += n;
        }
        n = number_of_zeros_to_pad;
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            fast_memset(str+str_l, '0', (n>avail?avail:n));
          }
          str_l += n;
        }
      }
      { int n = str_arg_l - zero_padding_insertion_ind;
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            fast_memcpy(str+str_l, str_arg+zero_padding_insertion_ind,
                        (n>avail?avail:n));
          }
          str_l += n;
        }
      }
      if (justify_left) {
        int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
        if (n > 0) {
          if (str_l < str_m) {
            size_t avail = str_m-str_l;
            fast_memset(str+str_l, ' ', (n>avail?avail:n));
          }
          str_l += n;
        }
      }
    }
  }
  if (str_m > 0) {
    str[str_l <= str_m-1 ? str_l : str_m-1] = '\0';
  }
  return (int) str_l;
}

/* Embedded Standalone Unit Test */
#ifdef _TEST_SNPRINTF_COMPAT

int main(void) {
    char buf[128];
    char *dyn_buf = NULL;
    int len;
    long long big_num = 9223372036854775807LL;

    printf("=== Solaris / SunOS Portable snprintf Test Suite ===\n");

    /* 1. Test 64-bit Long Long Formatting */
    len = snprintf(buf, sizeof(buf), "LLMAX: %lld", big_num);
    printf("[1] snprintf 64-bit int: '%s' (len: %d)\n", buf, len);
    assert(strcmp(buf, "LLMAX: 9223372036854775807") == 0);

    /* 2. Test Truncation & Null Termination Boundary */
    len = snprintf(buf, 10, "1234567890ABCDEF");
    printf("[2] snprintf truncation (bound 10): '%s' (reported len: %d)\n", buf, len);
    assert(strlen(buf) == 9);
    assert(strcmp(buf, "123456789") == 0);
    assert(len == 16);

    /* 3. Test asprintf Dynamic Allocation */
    len = asprintf(&dyn_buf, "Dynamic %s %lld", "Alloc", big_num);
    printf("[3] asprintf allocated: '%s' (len: %d)\n", dyn_buf, len);
    assert(dyn_buf != NULL);
    assert(strcmp(dyn_buf, "Dynamic Alloc 9223372036854775807") == 0);
    free(dyn_buf);

    printf("\nSUCCESS: All snprintf, vsnprintf, and asprintf tests passed cleanly!\n");
    return 0;
}

#endif /* _TEST_SNPRINTF_COMPAT */

