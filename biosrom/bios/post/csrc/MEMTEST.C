/*
浜様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様融
�                                                                            �
� File:    MEMTEST.C                                                         �
� Author:  Jack Miller                                                       �
� Created: 13-Dec-1996                                                       �
�                                                                            �
麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様郵
�                                                                            �
�   This file contains the Seattle System Memory testing functions.          �
�                                                                            �
�  Rather than performing this test here, the ROM bootstrap mechanism should �
�  test the system DRAM and pass it's test results (via the LLADDR register?)�
�  and this routine would just return that information back to the main      �
�  function.                                                                 �
�                                                                            �
麺様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様郵
� HISTORY:                                                                   �
�                                                                            �
�  13Dec96 JVM  Created.                                                     �
�                                                                            �
藩様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様夕
*/
#include <compiler.h>
#include <machine/seattle.h>
#include <machine/idtcpu.h>
#include "post.h"


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                       Structure & Literal Defintions                       �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
#define DRAM_BANK_SZ    0x800000



/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                               External Data                                �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
extern mxU32    _end;


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                Global Data                                 �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
int sysMemErrors[4];


/*
敖陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳朕
�                                    Code                                    �
青陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳陳潰
*/
mxBool testSysMem(void) /*
様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様様*/
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
