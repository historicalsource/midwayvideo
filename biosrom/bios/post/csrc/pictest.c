/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    PICTEST.C                                                         บ
บ Author:  Jack Miller                                                       บ
บ Created: 11-Apr-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   PIC Microcontroller Interface and Utility Routines                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  11Apr97 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <compiler.h>
#include <machine/seattle.h>
#include "post.h"
#include "pic.h"


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                       Structure & Literal Defintions                       ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                               External Data                                ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
extern volatile _ASIC_REGS * const _ioasic;


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                             External Functions                             ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
extern mxU32    getSysTicks(void);


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                              Local Functions                               ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
static mxBool   sendPicCommand(mxU16 command, mxU16 cmndtype);
static mxBool   recvPicData(mxU8 *data);
static mxBool   sendPicData(mxU8 data);


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                Global Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/



/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                 Local Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                    Code                                    ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/

mxBool sendPicNOP(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Send the NOP command to the PIC Microcontroller - for testing.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{

    return (sendPicCommand(MC_NOP_0, MC_IMMEDIATE));

}

#if 0
mxBool wrPicNVRam(mxU32 addr, mxU32 data) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Write to a PIC/RTC NV RAM location.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
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
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Read from a PIC/RTC NV RAM location.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
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
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Send a command to the PIC Microcontroller.
  Handles protocol for both IMMEDIATE and COMPLEX command types.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
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
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Get data from the PIC Microcontroller.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
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
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Send data to the PIC Microcontroller.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
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
