/*
浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
�                                                                            �
� File:    PICTEST.C                                                         �
� Author:  Jack Miller                                                       �
� Created: 11-Apr-1997                                                       �
�                                                                            �
麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様郵
�                                                                            �
�   PIC Microcontroller Interface and Utility Routines                       �
�                                                                            �
麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様郵
� HISTORY:                                                                   �
�                                                                            �
�  11Apr97 JVM  Created.                                                     �
�                                                                            �
藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕
*/
#include <compiler.h>
#include <machine/seattle.h>
#include "post.h"
#include "pic.h"


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                       Structure & Literal Defintions                       �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                               External Data                                �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
extern volatile _ASIC_REGS * const _ioasic;


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                             External Functions                             �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
extern mxU32    getSysTicks(void);


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                              Local Functions                               �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
static mxBool   sendPicCommand(mxU16 command, mxU16 cmndtype);
static mxBool   recvPicData(mxU8 *data);
static mxBool   sendPicData(mxU8 data);


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                Global Data                                 �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/



/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                 Local Data                                 �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                    Code                                    �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/

mxBool sendPicNOP(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Send the NOP command to the PIC Microcontroller - for testing.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{

    return (sendPicCommand(MC_NOP_0, MC_IMMEDIATE));

}

#if 0
mxBool wrPicNVRam(mxU32 addr, mxU32 data) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Write to a PIC/RTC NV RAM location.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU8    atmp, dtmp;
    int     ix;


    if (addr > PIC_NVRAM_SZ) return (FALSE);

    atmp = (mxU8)(addr * sizeof(mxU32));

    for (ix = 0; ix < sizeof(mxU32); ix++) {
        if (sendPicCommand(MC_WR_NVRAM, MC_COMPLEX)) {
            atmp = atmp+ix;
            if (!sendPicData((atmp & 0x0F) >> 0)) return (FALSE);
            if (!sendPicData((atmp & 0xF0) >> 4)) return (FALSE);

            dtmp = (mxU8)(data >> (ix * 8));
            if (!sendPicData((dtmp & 0x0F) >> 0)) return (FALSE);
            if (sendPicData((dtmp & 0xF0) >> 4)) return (FALSE);
        }
    }

    return (TRUE);
}


mxBool rdPicNVRam(mxU32 addr, mxU32 *data) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Read from a PIC/RTC NV RAM location.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU8    atmp;
    mxU8    dtmp;
    int     ix;


    if (addr > PIC_NVRAM_SZ) return (FALSE);

    atmp = (mxU8)(addr * sizeof(mxU32));
    *data = 0;

    for (ix = 0; ix < sizeof(mxU32); ix++) {
        if (sendPicCommand(MC_RD_NVRAM, MC_COMPLEX)) {
            atmp = atmp+ix;
            if (!sendPicData((atmp & 0x0F) >> 0)) return (FALSE);
            if (!sendPicData((atmp & 0xF0) >> 4)) return (FALSE);

            if (recvPicData(&dtmp)) return (FALSE);
            *data |= ((mxU32)dtmp << (ix * 8));
        }
    }

    return (TRUE);
}
#endif

static mxBool sendPicCommand(mxU16 command, mxU16 cmndtype) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Send a command to the PIC Microcontroller.
  Handles protocol for both IMMEDIATE and COMPLEX command types.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU16   response;
    mxU32   timer, timeout;


  /* Assume everything is O.K. */
    timeout = FALSE;

  /* Send command to PIC */
    _ioasic->asicHostPicCmnd = command;
    _ioasic->asicHostPicCmnd |= MC_REQ;

    timer = getSysTicks() + 2;

  /* Wait for MC_ACK */
    do {
        response = _ioasic->asicPicHostData;

        if (getSysTicks() > timer) {
            timeout = TRUE;
            break;
        }
    } while (!(response & MC_ACK));

  /* Clear command request */
    _ioasic->asicHostPicCmnd ^= MC_REQ;

    if (timeout) return (FALSE);

    response ^= (MC_ACK | MC_RESP); // remove MC_ACK bit and response bit

    if (response != command) return (FALSE);

    timer = getSysTicks() + 2;

  /* Wait for !MC_ACK */
    while (_ioasic->asicPicHostData & MC_ACK) {
        if (getSysTicks() > timer) return (FALSE);
    }

    if (cmndtype == MC_IMMEDIATE) {
        return (response == command);
    } else {
        return (_ioasic->asicPicHostData == MC_MORE);
    }
}

#if 0
static mxBool recvPicData(mxU8 *data) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Get data from the PIC Microcontroller.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU16   response;
    mxU32   timer, timeout;


  /* Assume everything is O.K. */
    timeout = FALSE;

  /* Send data request to PIC */
    _ioasic->asicHostPicCmnd |= MC_REQ;

    timer = getSysTicks() + 2;

  /* Wait for MC_ACK */
    do {
        response = _ioasic->asicPicHostData;

        if (getSysTicks() > timer) {
            timeout = TRUE;
            break;
        }
    } while (!(response & MC_ACK));

  /* Clear command request */
    _ioasic->asicHostPicCmnd ^= MC_REQ;

    if (timeout) return (FALSE);

    *data = (mxU8)(response ^ MC_ACK);  // remove MC_ACK bit

    timer = getSysTicks() + 2;

  /* Wait for !MC_ACK */
    while (_ioasic->asicPicHostData & MC_ACK) {
        if (getSysTicks() > timer) return (FALSE);
    }

    return (_ioasic->asicPicHostData == MC_MORE);
}


static mxBool sendPicData(mxU8 data) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Send data to the PIC Microcontroller.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU16   response;
    mxU32   timer, timeout;


  /* Assume everything is O.K. */
    timeout = FALSE;

  /* Send data request to PIC */
    _ioasic->asicHostPicCmnd = data;
    _ioasic->asicHostPicCmnd |= MC_REQ;

    timer = GetSysTicks() + 2;

  /* Wait for MC_ACK */
    do {
        response = _ioasic->asicPicHostData;

        if (getSysTicks() > timer) {
            timeout = TRUE;
            break;
        }
    } while (!(response & MC_ACK));

  /* Clear command request */
    _ioasic->asicHostPicCmnd ^= MC_REQ;

    if (timeout) return (FALSE);

    response ^= (MC_ACK | MC_RESP); // remove MC_ACK bit and response bit

    if (response != data) return (FALSE);

    timer = getSysTicks() + 2;

  /* Wait for !MC_ACK */
    while (_ioasic->asicPicHostData & MC_ACK) {
        if (getSysTicks() > timer) return (FALSE);
    }

    return (_ioasic->asicPicHostData == MC_MORE);
}
#endif
