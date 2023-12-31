/*
浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
�                                                                            �
� File:    DCS2TEST.C                                                        �
� Author:  Jack Miller                                                       �
� Created: 12-Sep-1997                                                       �
�                                                                            �
麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様郵
�                                                                            �
�   DCS2 Sound System Test                                                   �
�                                                                            �
麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様郵
� HISTORY:                                                                   �
�                                                                            �
�  12Sep97 JVM  Created                                                      �
�                                                                            �
藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕
*/
#include <machine/seattle.h>
#include "dcs2def.h"
#include "post.h"


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                       Structure & Literal Defintions                       �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
#define SOUND_TIMEOUT   (2*60)


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


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                Global Data                                 �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                              Global Functions                              �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                              Local Functions                               �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
static void     dcs2SetupSound(void);
static int      dcs2SendSoundCommand(mxU32 command);
static int      dcs2GetSoundData(mxU32 *data);
static mxU32    dcs2WaitForSound(void);


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                 Local Data                                 �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
static mxU32    __dcs2Status;


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                    Code                                    �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
mxBool dcs2CPUDSPTest(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  DCS2 DSP Test Function
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU32   completion_code;    /* self-test info returned from snd system */


  /*
  ** Reset sound system and check handshaking
  */
    dcs2SetupSound();

  /*
  ** Request SDRC ASIC version number
  */
    if (dcs2SendSoundCommand(SOUND_BOOT_ROM_ASIC_VERSION_NUMBER) == ERROR) {
        return (FALSE);
    }
    completion_code = dcs2WaitForSound();

    return (TRUE);
}


mxBool dcs2BootROMTest(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  DCS2 DSP Test Function
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU32   completion_code;    /* self-test info returned from snd system */

  /*
  ** Request sound boot ROM version number
  */
    if (dcs2SendSoundCommand(SOUND_BOOT_ROM_VERSION_NUMBER) == ERROR) {
        return (FALSE);
    }
    completion_code = dcs2WaitForSound();

    return (TRUE);
}

mxBool dcs2DRAMTest(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  DCS2 DSP Test Function
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU32   completion_code;    /* self-test info returned from snd system */


  /*
  ** Sound system D/RAM Test - Bank 0
  */
    if (dcs2SendSoundCommand(SOUND_BOOT_ROM_DRAM_BANK0_TEST) == ERROR) {
        return (FALSE);
    }
    completion_code = dcs2WaitForSound();
    if (completion_code != SOUND_BOOT_ROM_RETURN_DRAM_BANK0_PASSED) return (FALSE);

    return (TRUE);
}

mxBool dcs2SRAMTest(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  DCS2 DSP Test Function
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU32   completion_code;    /* self-test info returned from snd system */

  /*
  ** Sound system static RAM Test
  */
    if (dcs2SendSoundCommand(SOUND_BOOT_ROM_SRAM_TEST) == ERROR) {
        return (FALSE);
    }
    completion_code = dcs2WaitForSound();
    if (completion_code != SOUND_BOOT_ROM_RETURN_SRAM_PASSED) return (FALSE);

    return (TRUE);
}


static void dcs2SetupSound(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Takes the sound section out of reset. This should start the sound boot
  ROM monitor code running.

  This differs from dcs2ResetSoundSystem() in that this does not check for
  the "alive" signal.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
  /*
  ** reset bit is bit 0 of I/O ASIC B500.0040
  ** 0 = reset, 1 = active, not reset
  */
    _ioasic->asicHostSndCtrl &= DCS2_DSP_RESET;

    delay(2);

    _ioasic->asicHostSndCtrl |= DCS2_DSP_ENABLE;

    delay(2);

  /*
  ** bit 15 of I/O ASIC B500.0078 enables
  ** 'C31 write back mode
  */
    _ioasic->asicHostSndCtrl |= TI320CX_MODE_ENABLE;
}

static int dcs2SendSoundCommand(mxU32 command) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  The sound monitor ROM code always returns an 0x0A whenever it is ready
  for a command or has completed a command and ready for another.
  This function looks for the 0x0A and sends out a command.

  Note that the sound monitor code is NOT running interrupts...
  so it sits with the completion code and will not
  issue another "ready for command" code until you finish off the
  cycle by reading the completion code.

  See the doc for the list of available sound boot ROM functions and
  completion codes.

  In this code, 'C31 write-back is on. If you don't do the dummy write,
  then the sound system will crash.

  Returns a 0 if successful, or a time out or non-zero error if not OK.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxVU32    timer;


    timer = getSysTicks() + SOUND_TIMEOUT;

  /*
  ** wait for the monitor ready signal to show up it should be
  ** there if we called this function, but wait for it nonetheless
  */
    while (!(_ioasic->asicHostSndStat & STH_DATA_RDY)) {
        if (getSysTicks() > timer) {
            __dcs2Status = 0xE1;
            return (ERROR);
        }
    }

  /*
  ** if the ready bit has gone high, then data is ready
  ** make sure it's the 0x0A
  */
    if ((_ioasic->asicSndHostData & MASK16) != SOUND_BOOT_ROM_READY_SIGNAL) {
        __dcs2Status = 0xE2;
        return (ERROR);
    }

  /* dummy write-back */
    _ioasic->asicSndHostData = 0x0000;

  /* send the actual command */
    _ioasic->asicHostSndData = command;

	return OK;
}


static int dcs2GetSoundData(mxU32 *data) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Returns a single 16-bit word from the sound-to-host port.

  Returns OK if successful or an ERROR if it times out.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxVU32      timer;


    timer = getSysTicks() + SOUND_TIMEOUT;

  /* wait for data to show up in port */
    while (!(_ioasic->asicHostSndStat & STH_DATA_RDY)) {
        if (getSysTicks() > timer) {
            *data = 0xEEEE;
            return (ERROR);
        }
    }

  /* read the data */
    *data = __dcs2Status = (mxU32)(_ioasic->asicSndHostData & MASK16);

  /*
  ** In 'C31 mode a read from the sound DSP
  ** must be followed by a write.
  ** This clears the data ready bit.
  */
    _ioasic->asicSndHostData = 0x0000;

    return (OK);
}


static mxU32 dcs2WaitForSound(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様
  Waits for the appropriate completion code from the sound board. Since
  each test can take a different amount of time, each command signals
  that it is done by sending a unique completion code.

  Returns the completion code, unless there is a timeout error.
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
{
    mxU32   completion_code;
    mxVU32  timer;


    timer = getSysTicks() + SOUND_TIMEOUT;

    while (!(_ioasic->asicHostSndStat & STH_DATA_RDY)) {
        if (getSysTicks() > timer) {
            __dcs2Status = 0xEEEE;
            return (ERROR);
        }
    }

    completion_code = __dcs2Status = (_ioasic->asicSndHostData & MASK16);

  /* required dummy write-back */
    _ioasic->asicSndHostData = 0x0000;

    return (completion_code);
}
