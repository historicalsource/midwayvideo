/*
** Copyright (c) 1996, 3Dfx Interactive, Inc.
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
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
**
** $Header: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/gsfc.h 1     9/02/98 12:36a Sapphire $
** $Log: /Releases/Banshee/GLOP/3Dfx/Devel/H3/glide3/src/gsfc.h $
** 
** 1     9/02/98 12:36a Sapphire
** 
** 4     8/02/98 5:01p Dow
** Glide Surface Extension
** 
** 3     7/13/98 9:57p Jdt
** Added to build.  First implementation of SetRenderSurface and
** CreateContext
** 
** 2     7/09/98 6:47p Dow
** 
** 1     7/09/98 11:37a Dow
** Initial Checkin
**
*/
#ifndef GSFC_H
#define GSFC_H


#include <3dfx.h>
#include <glide.h>


#define GR_SURFACE_EXTENSION		0x1000

typedef FxU32 GrSurfaceContextType_t;
#define GR_SURFACECONTEXT_WINDOWED      0
#define GR_SURFACECONTEXT_FULLSCREEN    1

typedef void * GrSurface_t;

typedef FxU32 GrSurfaceTexType_t;
#define GR_SURFACETEXTYPE_FB		0
#define GR_SURFACETEXTYPE_AGP		1

/* New Enumerants for GR_GET */
#define GR_SURFACE_SIZE			GR_SURFACE_EXTENSION | 0x1
#define GR_SURFACE_TEXTURE		GR_SURFACE_EXTENSION | 0x2


extern GrContext_t FX_CALL
grSurfaceCreateContext(GrSurfaceContextType_t t);

extern void FX_CALL
grSurfaceReleaseContext(GrContext_t ctx);

extern void FX_CALL
grSurfaceSetAuxRenderingSurface(GrSurface_t sfc);

extern void FX_CALL
grSurfaceSetRenderingSurface(GrSurface_t sfc);

extern void FX_CALL
grSurfaceSetAuxSurface(GrSurface_t sfc);

extern void FX_CALL
grSurfaceSetTextureSurface(GrSurface_t sfc);

extern GrSurface_t FX_CALL
grSurfaceAllocateTexture(FxU32 sizeInBytes);

extern FxBool FX_CALL
grSurfaceInitialzeSurfaces(FxU32 hWnd, GrScreenResolution_t sRez,
			   GrScreenRefresh_t sRef, GrSurfaceTexType_t tType,
			   FxU32 sTex, GrColorFormat_t cFmt, 
			   FxU32 nColor, FxU32 nAux) ;

extern FxBool FX_CALL
grSurfaceAllocAuxRenderingSurface(GrSurface_t gSurface, FxU32 width, 
				  FxU32 height, GrColorFormat_t cFmt);

extern void FX_CALL
grSurfaceBlit (GrSurface_t gSrcSfc, GrSurface_t gDstSfc, FxU32 srcX,
  FxU32 srY, FxU32 dstX, FxU32 dstY, FxU32 width, FxU32 height, float
  hScale, float vScale);

extern FxU32 FX_CALL
grSurfaceMipmapMemRequired(GrTexInfo *tInfo);

#endif                          /* GSFC_H not defined */
