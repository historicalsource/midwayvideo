/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#ifndef __dj_include_sys_time_h_
#define __dj_include_sys_time_h_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __dj_ENFORCE_ANSI_FREESTANDING

#ifndef __STRICT_ANSI__

#ifndef _POSIX_SOURCE

#include <time.h>

#define ITIMER_REAL      0
#define ITIMER_PROF      1

struct itimerval {
  struct  timeval it_interval;    /* timer interval */
  struct  timeval it_value;       /* current value */
};

int getitimer(int _which, struct itimerval *_value);
int setitimer(int _which, struct itimerval *_value, struct itimerval *_ovalue);
int utimes(const char *_file, struct timeval _tvp[2]);

#endif /* !_POSIX_SOURCE */
#endif /* !__STRICT_ANSI__ */
#endif /* !__dj_ENFORCE_ANSI_FREESTANDING */

#ifndef __dj_ENFORCE_FUNCTION_CALLS
#endif /* !__dj_ENFORCE_FUNCTION_CALLS */

#ifdef __cplusplus
}
#endif

#endif /* !__dj_include_sys_time_h_ */
