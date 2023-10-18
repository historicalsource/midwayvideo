/*
 * UNLOCK.C
 */
#include	<unistd.h>
#include	<dos.h>
#include	"pd.h"

#define IOASIC_RESET   0xB5800000
#define IOASIC_SW_DIP  0xB5000000
#define IOASIC_SW_P12  0xB5000010
#define IOASIC_SW_P34  0xB5000018
#define IOASIC_CONTROL 0xB5000078
#define IOASIC_STATUS  0xB5000070
#define IOASIC_HTS_CTRL 0xB5000040

#define UNLOCK_TIMEOUT	1000

/***************************************************************************/
/*                                                                         */
/* FUNCTION: wait_ioasic()                                                 */
/*                                                                         */
/* Waits for the I/O ASIC to toggle the bit saying it is ready for the     */
/* next data word.                                                         */
/*                                                                         */
/***************************************************************************/

static void wait_ioasic(void)
{
	unsigned long	pdata;

	do
	{
		psyq_mem_read(IOASIC_SW_DIP, &pdata);
	} while(pdata & 0x8000);
}

/***************************************************************************/
/*                                                                         */
/* FUNCTION: unlock_ioasic()                                               */
/*                                                                         */
/* Unlocks the I/O ASIC.                                                   */
/*                                                                         */
/* Returns OK or ERROR.                                                    */
/*                                                                         */
/* (c) 1997 Midway Games, Inc.                                             */
/*                                                                         */
/***************************************************************************/

int unlock_ioasic(void)
{
	unsigned long	pdata;
	int				i;
	int				timeout;

	/* reset the I/O ASIC to put it in a known state */
	psyq_mem_write(IOASIC_RESET, 0);
	psyq_mem_write(IOASIC_RESET, 1);

	/* send "host arm" sequence to I/O ASIC */
	/* this is a six word "unlock" sequence */
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P34, 0x002B); 
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P34, 0x0093); 
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P34, 0x00A7);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P34, 0x004E);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P34, 0x0001);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P34, 0x001E);


	/* now send an eight word "seed" sequence */
	/* this builds the "CLL host variable" */
	for(i = 0; i < 8; i++)
	{
		wait_ioasic();
		psyq_mem_write(IOASIC_SW_P12, 0x1234);
	}

	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P12, 0x0042);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P12, 0x0065);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_P12, 0x00E3);

	for(i = 0; i < 4; i++)
	{
		wait_ioasic();
		psyq_mem_write(IOASIC_SW_P12, 0x0000);
	}

	wait_ioasic();
	psyq_mem_write(IOASIC_SW_DIP, 0x0054);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_DIP, 0x0029);
	wait_ioasic();
	psyq_mem_write(IOASIC_SW_DIP, 0x00E2);


	/* when main ctrl goes to zero it is unlocked */
	timeout = UNLOCK_TIMEOUT;
	do
	{
		psyq_mem_read(IOASIC_CONTROL, &pdata);
		if(!timeout)
		{
			return(0);
		}
		--timeout;
	} while((pdata & 0x00FF) != 0x0000); 

   /* page 8 of I/O ASIC functional spec */
	/* 'C31 write-back mode on - bit 15 */
   /* LED off - bit 14 high turns it off */
	/* all other don't care or zero - disables all interrupts */

   /* turn off LED */
   /* set 'C31 write-back mode */
	psyq_mem_write(IOASIC_CONTROL, 0xC000);

   return(1);

}




