/*-*-c++-*-*/
#ifndef __GDEBUG_H__
#define __GDEBUG_H__

/*
** Copyright (c) 1995, 1996, 1997 3Dfx Interactive, Inc.
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
** $Date: 9/02/98 12:35a $
*/

#include <stdarg.h>

#if defined(FX_DLL_ENABLE)
#define FX_DLL_DEFINITION

#endif
#include <fxdll.h>

#define GDBG_MAX_LEVELS 512

#ifndef GETENV
#define GETENV(a) getenv(a)
#endif

// if debug info turned on then GDBG_INFO does something
#ifdef GDBG_INFO_ON

#define GDBG_INFO gdbg_info
#define GDBG_INFO_MORE gdbg_info_more
#define GDBG_PRINTF gdbg_printf

//#define GDBG_ERROR_SET_CALLBACK   gdbg_error_set_callback
//#define GDBG_ERROR_CLEAR_CALLBACK gdbg_error_clear_callback
#define GDBG_ERROR_SET_CALLBACK   0 && (unsigned long)
#define GDBG_ERROR_CLEAR_CALLBACK 0 && (unsigned long)

#define GDBG_GET_DEBUGLEVEL	gdbg_get_debuglevel
#define GDBG_SET_DEBUGLEVEL	gdbg_set_debuglevel

// otherwise GDBG_INFO does nothing
#else

#if defined(__WATCOMC__) || defined(__WATCOM_CPLUSPLUS__)
/* Turn off the dead code warnings. Also changed the macro definitions
 * to use an 'if' rather than the ternary operator because the
 * type of the result sub-expressions must match.
 *
 * w111: Meaningless use of an expression
 * w201: Unreachable code
 */
#pragma disable_message (111, 201)
#endif /* defined(__WATCOMC__) || defined(__WATCOM_CPLUSPLUS__) */

//#define GDBG_INFO      0 && (unsigned long)
//#define GDBG_INFO_MORE 0 && (unsigned long)
//#define GDBG_PRINTF    0 && (unsigned long)

//#define GDBG_ERROR_SET_CALLBACK   0 && (unsigned long)
//#define GDBG_ERROR_CLEAR_CALLBACK 0 && (unsigned long)



#define GDBG_INFO(args...)	/* ##args */
#define GDBG_INFO_MORE(args...)	/* ##args */
#define GDBG_PRINTF(args...)		/* ##args */

#define GDBG_ERROR_SET_CALLBACK(args...)	/* ##args */
#define GDBG_ERROR_CLEAR_CALLBACK(args...)	/* ##args */





#define GDBG_GET_DEBUGLEVEL(x) 0
#define GDBG_SET_DEBUGLEVEL(a,b)

#endif
						
#define GDBG_INIT	   	gdbg_init
#define GDBG_SHUTDOWN		gdbg_shutdown
#define GDBG_ERROR		gdbg_error
#define GDBG_GET_ERRORS		gdbg_get_errors
#define GDBG_SET_FILE		gdbg_set_file


FX_ENTRY void	FX_CALL gdbg_init(void);
FX_ENTRY void	FX_CALL gdbg_parse(const char *env);
FX_ENTRY void	FX_CALL gdbg_shutdown(void);

//KEENAN PORT
//FX_ENTRY void	FX_CALL gdbg_vprintf(const char *format, va_list);

FX_ENTRY void	FX_CALL gdbg_printf(const char *format, ...);
FX_ENTRY int	FX_CALL gdbg_info(const int level, const char *format, ...);
FX_ENTRY int	FX_CALL gdbg_info_more(const int level, const char *format, ...);
FX_ENTRY void	FX_CALL gdbg_error(const char *name, const char *format, ...);
FX_ENTRY int	FX_CALL gdbg_get_errors(void);
FX_ENTRY int	FX_CALL gdbg_set_file(const char *name);
FX_ENTRY int	FX_CALL gdbg_get_debuglevel(const int level);
FX_ENTRY void	FX_CALL gdbg_set_debuglevel(const int level, const int value);

// these routines allow for a library (like Glide) to get called back
//KEENAN PORT
//typedef void (*GDBGErrorProc)(const char* const procName,
//                              const char* const format,
//                              va_list     args);
//FX_ENTRY int FX_CALL gdbg_error_set_callback(GDBGErrorProc p);
//FX_ENTRY void FX_CALL gdbg_error_clear_callback(GDBGErrorProc p);

// these routines allow for some GUI code to get called once in a while
// so that it can keep the UI alive by reading the message queue
typedef void (*GDBGKeepAliveProc)(int adjust);
FX_ENTRY void FX_CALL gdbg_set_keepalive(GDBGKeepAliveProc p);

#endif /* !__GDEBUG_H__ */
