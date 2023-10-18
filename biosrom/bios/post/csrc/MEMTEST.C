/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    MEMTEST.C                                                         บ
บ Author:  Jack Miller                                                       บ
บ Created: 13-Dec-1996                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   This file contains the Seattle System Memory testing functions.          บ
บ                                                                            บ
บ  Rather than performing this test here, the ROM bootstrap mechanism should บ
บ  test the system DRAM and pass it's test results (via the LLADDR register?)บ
บ  and this routine would just return that information back to the main      บ
บ  function.                                                                 บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  13Dec96 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <compiler.h>
#include <machine/seattle.h>
#include <machine/idtcpu.h>
#include "post.h"


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                       Structure & Literal Defintions                       ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
#define DRAM_BANK_SZ    0x800000



/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                               External Data                                ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
extern mxU32    _end;


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                Global Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
int sysMemErrors[4];


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                    Code                                    ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
mxBool testSysMem(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    register mxBool memok;
    register mxU8   memval, memwval, memrval;
    register mxVU8  *membeg, *memend;


    sysMemErrors[0] = sysMemErrors[1] = sysMemErrors[2] = sysMemErrors[3] = 0;

    memwval = memrval = 1;
    memok = TRUE;

#if BIOS == MIKE_BIOS
    membeg = (mxVU8 *)(PHYS_TO_K1((mxU8 *)&_end));
    memend = (mxVU8 *)(PHYS_TO_K1(DRAM_BANK0_BASE+DRAM_BANK_SZ-BIOS_STACK_SZ));
#elif BIOS == JACK_BIOS
    membeg = (mxVU8 *)(PHYS_TO_K1(((mxU8 *)&_end)+BIOS_STACK_SZ));
    memend = (mxVU8 *)(PHYS_TO_K1(DRAM_BANK0_BASE+DRAM_BANK_SZ));
#else
#error Must define BIOS to be either MIKE_BIOS or JACK_BIOS
#endif

  /* Initialize memory with test pattern */
    do {
        *membeg = memwval;
        if (++memwval == 0) memwval = 1;
    } while (++membeg < memend);


#if BIOS == MIKE_BIOS
    membeg = (mxVU8 *)(PHYS_TO_K1((mxU8 *)&_end));
#elif BIOS == JACK_BIOS
    membeg = (mxVU8 *)(PHYS_TO_K1(((mxU8 *)&_end)+BIOS_STACK_SZ));
#else
#error Must define BIOS to be either MIKE_BIOS or JACK_BIOS
#endif

  /* Check memory for test pattern */
    do {
        memval = *membeg;

        if (memval != memrval) {
            switch ((mxU32)membeg & 0x00000007) {
              case 0:
              case 1:
                sysMemErrors[0]++;  break;
              case 2:
              case 3:
                sysMemErrors[1]++;  break;
              case 4:
              case 5:
                sysMemErrors[2]++;  break;
              case 6:
              case 7:
                sysMemErrors[3]++;  break;
            }

            memok = FALSE;
        }

        if (++memrval == 0) memrval = 1;

    } while (++membeg < memend);

    return (memok);
}
