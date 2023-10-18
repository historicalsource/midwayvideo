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
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished  -
** rights reserved under the Copyright Laws of the United States.
**
**
** $Revision: 1 $ 
** $Date: 2/12/98 4:56p $ 
**
*/


#ifndef _FXTINI_H_
#define _FXTINI_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <3dfx.h>
#include "init.h"
#include <fxdll.h>

/*----------------------------------------------------------------------------
  DATA DEFINITIONS
  ----------------------------------------------------------------------------*/
/*-------------------------------------------------------------------
  Structure: InitContext
  Date: 10/9
  Implementor(s): jdt, murali
  Library: Init
  Description:
  Contains all device dependant functions for a given 3Dfx device
  Members:
  setVideo         - initilize video 
  restoreVideo     - undo setVideo
  enableTransport  - enable the command transport
  disableTransport - disable the command transport
  swapBuffers    - function which executes a buffer swap
  status         - function which returns the status word from the 3D subsystem
  busy           - returns true if hardware busy
  idle           - returns when 3D subsystem is idle
  getBufferPtr   - get a pointer to a frame buffer
  renderBuffer   - set the current buffer for rendering
  origin         - set the y origin 
  ioCtl          - CYA function 
  control        - OS response functions

  gamma          - initializes gamma table
  sliPciOwner    - sets ownership of PCI bus in SLI configuration

  info           - hardware description
  writeMethod    - function for doing command transport writes to hardware
  devNumber      - enumerated order of device on bus
  devPrivate     - Private data for the device type
  -------------------------------------------------------------------*/
typedef struct {
    FxBool       (*setVideo) (
                    FxU32                   hWnd,
                    GrScreenResolution_t    sRes,
                    GrScreenRefresh_t       vRefresh,
                    InitColorFormat_t       cFormat,
                    InitOriginLocation_t    yOrigin,
                    int                     nColBuffers,
                    int                     nAuxBuffers,
                    int                     *xres,
                    int                     *yres,
                    int                     *fbStride,
                    sst1VideoTimingStruct   *vidTimings);
    void         (*restoreVideo)( void );
    FxBool       (*enableTransport)( InitFIFOData *info );
    void         (*disableTransport)( void );

    void         (*swapBuffers)( FxU32 code );
    FxU32        (*status)( void );
    FxBool       (*busy)(void);
    void         (*idle)( void );
    void        *(*getBufferPtr)( InitBuffer_t buffer, int *strideBytes );
    void         (*renderBuffer)( InitBuffer_t buffer );
    void         (*origin)( InitOriginLocation_t origin );
    void         (*ioCtl)( FxU32 token, void *argument );
    FxBool       (*control) ( FxU32 code );
    FxBool       (*wrapFIFO) (InitFIFOData *fD);
   
    void         (*gamma)( double gamma );
    void         (*sliPciOwner)( FxU32 *regbase, FxU32 owner );

    InitDeviceInfo    info;
    InitWriteCallback *writeMethod;
    void              *devPrivate;
} InitContext;

/* Global current context */
extern InitContext *context;

void vgDriverInit( InitContext *context );
void vg96DriverInit( InitContext *context );
void h3DriverInit( InitContext *context );

#ifdef __cplusplus
}
#endif
#endif
