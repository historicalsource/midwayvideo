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
*/

#ifndef _FXPHRLAP_H_
#define _FXPHRLAP_H_

int pciPhrlapGetSegmentLinearBase( USHORT selector, ULONG* lin_addrp );
int pciPhrlapMapPhysMemEndSegment( USHORT selector, ULONG phys_addr, ULONG page_cnt, ULONG* offsetp );
void pciPhrlapGetConfigInfo( CONFIG_INF* configp, UCHAR* swp_namep );
int pciPhrlapReadPageTableEntry( ULONG linadr, ULONG* ptep, ULONG* ptinfp );
int pciPhrlapWritePageTableEntry( ULONG linadr, ULONG pte, ULONG ptinf );
#endif /* _FXPHRLAP_H_ */
