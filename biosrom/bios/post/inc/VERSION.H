#if !defined(_VERSION_H_)
#define _VERSION_H_

typedef struct {
  int   maj;        /* Version number: major    (0 - 9) */
  int   min;        /*                 minor    (0 - 9) */
  char  rev;        /*                 revision (a - z) */
} _POST_VERSION;

#endif
