#ifndef VTERM_COMPAT_H
#define VTERM_COMPAT_H

#include <sys/types.h>

#ifndef VTERM_CUSTOM_H
#define VTERM_CUSTOM_H

/* ====================================================================
 * SECTION 1: SYSTEM INCLUDES, TYPEDEFS, AND MACRO DEFINITIONS
 * ==================================================================== */

#include <stddef.h>
#include <stdint.h>

// Forward declaration of the VTerm structure
struct VTerm;

// The error messages suggest the existence of a struct related to the encoding.
// Based on the code's context, a structure like this is likely.
typedef struct {
  void (*init)(struct VTerm* vt);
  void (*encode)(struct VTerm* vt, uint32_t cp, unsigned char* str, size_t* len);
  void (*decode)(struct VTerm* vt, const unsigned char* bytes, size_t len, size_t* processed_bytes, size_t* cplen, uint32_t* cp);
  void (*destroy)(struct VTerm* vt);
} VTermEncoding;

// The variables 'pos', 'cpi', etc. appear to be local variables in decode_utf8,
// but they might have been part of an earlier state structure.
// This struct captures the spirit of what's being missed based on the errors.
typedef struct {
  size_t pos;
  size_t bytelen;
  size_t cpi;
  size_t cplen;
  uint32_t cp;
  const unsigned char* bytes;
  const unsigned char* data_;
} EncodingDecodeState;

// Corrected function prototype for 'encoding_utf8' based on the conflict.
// The VTermEncoding struct must be defined before this.
extern const VTermEncoding encoding_utf8;

// Define a placeholder for the missing 'encoding_usascii'
extern const VTermEncoding encoding_usascii;


/* ====================================================================
 * SECTION 2: STATIC INLINE FUNCTIONS & IMPLEMENTATION OVERRIDES
 * ==================================================================== */

#endif // VTERM_CUSTOM_H

#endif /* VTERM_COMPAT_H */
