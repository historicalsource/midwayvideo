/*
 *		$Archive: /video/tools/buildrom/types.h $
 *		$Revision: 1 $
 *		$Date: 9/30/97 10:50a $
 *
 *		Copyright (c) 1997 Midway Games Inc.
 *		All Rights Reserved
 *		This file is confidential and a trade secret of Midway Games Inc.
 *		Use, duplication, or disclosure is strictly forbidden unless approved
 *		in writing by Midway Games Inc.
 */

#ifndef __TYPES_H__
#define __TYPES_H__

/*
 *		DEFINES
 */

#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
#define FALSE			((bool)0)
#define TRUE			((bool)1)

/*
 *		TYPEDEFS
 */

typedef int bool;
typedef unsigned char uchar;
typedef signed char schar;
typedef unsigned short int ushort;
typedef unsigned int uint;
typedef unsigned long int ulong;

/*
 *		$History: types.h $
 * 
 * *****************  Version 1  *****************
 * User: Mlynch       Date: 9/30/97    Time: 10:50a
 * Created in $/video/tools/buildrom
 * Include files for the buildrom tool.
 */

#endif
