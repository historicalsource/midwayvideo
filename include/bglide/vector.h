#ifndef __VECTOR_H__
#define __VECTOR_H__

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

#ifndef __ROM__
#include <stdio.h>
#endif

// Defines for different types of vector files 
#define VECTORID_MAX           2
#define VECTORID_CACHE         0
#define VECTORID_DECOMPRESSOR  1
#define VECTORID_CACHEm        1 << VECTORID_CACHE
#define VECTORID_DECOMPRESSORm 1 << VECTORID_DECOMPRESSOR

extern void openVectorFiles(); 
extern void closeVectorFiles( );
extern void dumpVector( );

//extern void openVectorFiles( 
//		CsimPrivate) *, 
//		unsigned int vectorGenMask );
//extern void closeVectorFiles( unsigned int vectorGenMask );
//extern void dumpVector( unsigned char * vectorData, unsigned int vectorType );

typedef struct vectorFiles { 
  FILE * vectorHandles[VECTORID_MAX];
  void (* dumpVector)();
} vectorFiles;

extern vectorFiles vectorf;

typedef struct {
  FxU32 data[2];
} CBlock;

typedef struct CacheOutput { 
  int u;
  int v;
  CBlock bank[ 4 ];
} CacheOutput;

typedef struct DecompOutput { 
  FxU32 Texel[4];
} DecompOutput;


#endif 
