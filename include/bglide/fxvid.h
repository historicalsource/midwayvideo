#ifndef __FX_VID_H__
#define __FX_VID_H__

/*
** Copyright (c) 1997, 3Dfx Interactive, Inc.
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

/* Get old resolution/refresh definitions */
#include <sst1vid.h>

/* XXX GMT: some of these are still machine dependent */
typedef struct {
    FxU32 hSyncOn;
    FxU32 hSyncOff;
    FxU32 vSyncOn;
    FxU32 vSyncOff;
    FxU32 hBackPorch;
    FxU32 vBackPorch;
    FxU32 xDimension;
    FxU32 yDimension;
    FxU32 memOffset;
    FxU32 memFifoEntries_1MB;
    FxU32 memFifoEntries_2MB;
    FxU32 memFifoEntries_4MB;
    FxU32 tilesInX_Over2;
    FxU32 vFifoThreshold;
    FxBool video16BPPIsOK;
    FxBool video24BPPIsOK;
    float clkFreq16bpp;
    float clkFreq24bpp;
} FxVideoTimingInfo;

#endif /* __FX_VID_H__ */



