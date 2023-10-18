#ifndef __GDEBUG_H__
#define __GDEBUG_H__

/*
** Copyright (c) 1995, 3Dfx Interactive, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of 3Dfx Interactive, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of 3Dfx Interactive, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished  -
** rights reserved under the Copyright Laws of the United States.
**
** $Revision: 1 $
** $Date: 5/04/97 3:44p $
*/

#include <stdarg.h>

#ifdef GDBG_INFO_ON

#define GDBG_INFO(a) gdbg_info a
#define GDBG_PRINTF(a) gdbg_printf a

#else

#define GDBG_INFO(a)
#define GDBG_PRINTF(a)

#endif

#define GDBG_INIT		gdbg_init
#define GDBG_ERROR		gdbg_error

extern long gdbg_level;		

extern void	gdbg_init(void);
extern void	gdbg_vprintf(const char *format, va_list);
extern void	gdbg_printf(const char *format, ...);
extern int	gdbg_info(const int, const char *, ...);
extern int	gdbg_info_more(const int, const char *, ...);
extern void	gdbg_error(const char *, const char *, ...);

extern void *HalveImage(unsigned int w, unsigned int h, void *indata);
extern float timer(int __flag);

#endif /* !__GDEBUG_H__ */
