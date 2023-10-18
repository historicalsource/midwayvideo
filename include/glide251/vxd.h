
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
** $Date: 4/20/98 1:21p $
*/

#ifndef _VXD_H_
#define _VXD_H_

#if defined(KERNEL) && !defined(KERNEL_NT)
#define WANTVXDWRAPS
#define DEBUG
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <debug.h>
#pragma VxD_LOCKED_CODE_SEG
//#pragma VxD_LOCKED_DATA_SEG
#endif /* #ifdef KERNEL */

#endif /* #ifndef _VXD_H_ */
