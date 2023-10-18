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
** $Date: 9/02/98 12:39a $
*/
#ifndef _FX_MEMMAP_H_
#define _FX_MEMMAP_H_

/*
 * DeviceIoControl service table control codes for communication with
 * fxmemmap vxd. The codes here must match those in fxmemmap.asm.
 * ESPECIALLY fxmajor_ver and fx_minor_ver!!!
 */

#define   FX_MAJOR_VER          1
#define   FX_MINOR_VER          4

#define   DIOC_CLOSEHANDLE     -1     
#define   DIOC_GETVERSIONDWORD  0   
#define   GETAPPVERSIONDWORD    1   
#define   GETLINEARADDR         2   
#define   GETLDTSELECTOR        3
#define   FREELDTSELECTOR       4
#define   GETLINEARADDR_AUTO    5 /* Watches for & handles PASSTHRU,
                                     Otherwise identical to 2 */
#define   GETMSR                6 /* Get the contents of an MSR */
#define   SETMSR                7
#define   DECREMENTMUTEX        8
#define   SETPASSTHROUGHBASE    9
#define   SETADDRPERM           10

/* FxMemmap Int 2F API Codes */
#define   PROCMAPPHYS              1UL
#define   PROCGETMSR               2UL
#define   PROCSETMSR               3UL
#define   PROCGETVERSION           4UL
#define   PROCUNMAPPHYS            5UL
#define   PROCMAPPHYS16            6UL
#define   PROCGETMSR16             7UL
#define   PROCSETMSR16             8UL   
#define   PROCGETVERSION16         9UL
#define   PROCUNMAPPHYS16          10UL  /* NOT YET IMPLEMENTED */
#define   PROCINCREMENTMUTEX16     11UL
#define   PROCDECREMENTMUTEX16     12UL
#define   PROCSETPASSTHROUGHBASE16 13UL
#define   PROCOUTPUTDEBUGSTRING16  14UL
#define   PROCSETADDRPERM16        15UL

#endif /* _FX_MEMMAP_H_ */
