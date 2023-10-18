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
** $Revision: 1 $
** $Date: 2/12/98 4:56p $
**
** SST-96 Initialization library
*/


#ifndef __INIT96_H__
#define __INIT96_H__

/* Partners' Chips */
#define AT3D            1       /* Alliance Semiconductor AT3D*/
#define T9685           2       /* Trident 9685 */
#define MR510           3       /* MRTI MR510 */
#define MX86251         4       /* Macronics MX 86251 */

#ifndef INIT96_LIB
#define INITVG96ENTRY(name, retType, args) FX_ENTRY retType FX_CALL name args
#else
#define INITVG96ENTRY(name, retType, args) FX_EXPORT retType FX_CSTYLE name args
#endif

typedef void Init96WriteMethod( FxU32 *addr, FxU32 data );

INITVG96ENTRY(init96MapBoard, FxBool,
              (void *rD, void *info, FxU16 vID, FxU16 dID) );
INITVG96ENTRY(init96SetVideo, FxBool ,
              (FxU32 hWnd, GrScreenResolution_t sRes,
               GrScreenRefresh_t vRefresh, InitColorFormat_t cFormat,
               InitOriginLocation_t yOrigin,int nColBuffers,
               int nAuxBuffers, InitRegisterDesc *rd, int *xres, 
               int *yres, int *fbStride)); 
INITVG96ENTRY(init96RestoreVideo, void, ( InitRegisterDesc *regDesc ));
INITVG96ENTRY(init96EnableTransport, FxBool, ( InitFIFOData *info, InitRegisterDesc* regDesc));
INITVG96ENTRY(init96DisableTransport, void, ( void ));
INITVG96ENTRY(init96Swap, void ,
              (FxU32 code, InitRegisterDesc *regDesc, 
               Init96WriteMethod *wcb) );
INITVG96ENTRY(init96Idle, void , (Init96WriteMethod *wcb) );
INITVG96ENTRY(init96GetBufferPtr, void*,
              (InitBuffer_t buffer, int *strideBytes )); 
INITVG96ENTRY(init96RenderBuffer, void,
              (InitBuffer_t buffer,Init96WriteMethod *wcb));
INITVG96ENTRY(init96Origin, void,
              (InitOriginLocation_t origin, Init96WriteMethod *wcb));
INITVG96ENTRY(init96Control, FxBool , (FxU32 code, InitRegisterDesc *rd) );
INITVG96ENTRY(init96WrapFIFO, FxBool ,
              (InitRegisterDesc *rd, InitFIFOData *fifoData) );
INITVG96ENTRY(init96GetFBVram, FxU32 , (void) );
#endif /* __INIT96_H__ */
