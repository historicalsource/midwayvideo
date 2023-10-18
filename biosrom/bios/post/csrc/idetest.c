/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    IDETEST.C                                                         บ
บ Author:  Jack Miller                                                       บ
บ Created: 14-Sep-1997                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ   IDE Device Driver                                                        บ
บ                                                                            บ
บ   This file contains the IDE device driver functions which talk to the     บ
บ   National Semiconductor PC87415 PCI-IDE interface chip.                   บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  14Sep97 JVM  Created.                                                     บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/
#include <stddef.h>
#include <machine/seattle.h>
#include "post.h"
#include "ata2.h"
#include "idefunc.h"
#include "idereg.h"


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                       Structure & Literal Defintions                       ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
#define DREQ_TIMEOUT    (2*60)      /* timeout for wait for DREQ */
#define BUSY_TIMEOUT    (5*60)      /* timeout for ide_wait_busy() */
#define RESET_TIMEOUT   (10*60)     /* timeout for ide_reset() */
#define POWERUP_TIMEOUT (20*60)     /* timeout for drive powerup */
#define RPM_TIMEOUT     (30*60)     /* timeout for rpm calculation */

#ifndef JUNK_ADDR
#define JUNK_ADDR (*(mxVU32*)(PHYS_TO_K1(PCI_IO_BASE+0x40C)))
#endif
#define JUNK_IT() do { mxU32 junk; junk = JUNK_ADDR; } while (0)


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                             External Functions                             ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
extern mxU8     PCI_ReadConfigByte(int dev, mxU8 addr);
extern void     PCI_WriteConfigByte(int dev, mxU8 addr, mxU8 value);
extern void     PCI_WriteConfigWord(int dev, mxU8 addr, mxU16 value);
extern void     PCI_WriteConfigDword(int dev, mxU8 addr, mxU32 value);

extern void     delay(mxU32 vblanks);


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                               External Data                                ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                              Local Functions                               ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
/* Prototypes for static functions in IDE.C */
static int read_status(void);
static int read_alt_status(void);
static int ide_send_command(int);
static int ide_wait_busy(void);
static int ide_hread_data(mxU32 *rdbuf, int nsectors);


static int ide_test_lba(int testmode);
static int ide_check_lba(int lba, int count);



/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                Global Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/
void ide_hard_reset(void);


/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                 Local Data                                 ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/

/* declare a sector buffer for internal use only! */
static mxU32 ide_sector_buffer[LONGS_PER_SECTOR];


static DeviceDesc *cur_device_ptr = NULL;
static DeviceDesc device_list[NUM_HDRIVES];
static int active_device;



/*
ฺฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฟ
ณ                                    Code                                    ณ
ภฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤฤู
*/

static int read_status(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Return the status of the IDE controller.
  This CLEARS the controller interrupt.

  returns: 0xFF - if no device initialized.
           contents of status register - if device initialized.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{

    if (!cur_device_ptr) {
        return (0xFF);
    } else {
        if (getGT64010Rev() == 1) {
            JUNK_IT();
        }
        return (cur_device_ptr->ideptr->csr);
    }
}


static int read_alt_status(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Return the status of the IDE controller.
  This does NOT CLEAR the controller interrupt.

  returns: 0xFF - if no device initialized.
           contents of alt status register  - if device initialized
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{

    if (!cur_device_ptr) {
        return (0xFF);
    } else {
        if (getGT64010Rev() == 1) {
            JUNK_IT();
        }
        return (*cur_device_ptr->alt_sts);
    }
}


int ide_check_devstat(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Continually check for a hard drive until a valid device is found or the
  pre-defined timeout period expires.

  returns: IDE_DEVICE_CONNECTED - Hard drive found.
           IDE_DEVICE_INVALID   - Hard drive not found.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxVU8   alt_status;
    int     status;
    mxVU32  timer;


    if (!cur_device_ptr) return (IDE_DEVICE_INVALID);

  /*
  ** Look for a hard drive until a valid device is found or
  ** until the timeout has expired.  The test looks at the
  ** IDE Alternate Status register, if it contains the value
  ** (0xff) then it is assumed that no hard drive is attached.
  */

    timer = getSysTicks() + BUSY_TIMEOUT;

    do {
        alt_status = read_alt_status();
 
        if ((alt_status == 0xFF) || (alt_status == 0x00)) {
            status = IDE_DEVICE_INVALID;
        } else {
            status = IDE_DEVICE_CONNECTED;
        }
    } while ((status == IDE_DEVICE_INVALID) && (getSysTicks() < timer));

    return (status);
}


static int ide_wait_busy(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Timed wait while drive is busy.

  returns: IDE_DEVICE_INVALID   - no device connected.
           IDE_DEVICE_CONNECTED - drive no longer busy and no timeout.
           IDE_DEVICE_TIMEOUT   - drive busy and timer expired.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxVU8   status;
    mxVU32  timer;


  /* Don't do anything if there's no device! */
    if (ide_check_devstat() != IDE_DEVICE_CONNECTED) return (IDE_DEVICE_INVALID);


    timer = getSysTicks() + BUSY_TIMEOUT;
    do {
        status = read_alt_status();
    } while ((status & IDE_STB_BUSY) && (getSysTicks() < timer));

    return ((status & IDE_STB_BUSY) ? IDE_DEVICE_TIMEOUT : IDE_DEVICE_CONNECTED);

}


#define DEVICE_SELECT_BIT   (4)

int ide_set_device(int device) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Set the IDE controller device number.
    0 - master on channel 1
    1 - master on channel 1
    2 - master on channel 2
    3 - master on channel 2

  returns: previous device number.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    int     old, tmp;

    if (device < 0 || device >= NUM_HDRIVES) return (-1);

    old            = active_device;
    cur_device_ptr = &device_list[device];
    active_device  = device;

    tmp = cur_device_ptr->ideptr->drive_head;
    tmp &= ~(1 << DEVICE_SELECT_BIT);
    cur_device_ptr->ideptr->drive_head = tmp | cur_device_ptr->select;

    return (old);
}


int ide_reset(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Reset the hard drive and the states
  of the IDE device driver variables.

  returns: (0) reset success
           (1) reset unsuccessful.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxVS32  timer;
    mxVU8   *ide_alt_stat;


    if (!cur_device_ptr) return (1);

    ide_hard_reset();

    ide_alt_stat = cur_device_ptr->alt_sts;

  /* set the SRST bit in the DEVICE CONTROL register */
    *ide_alt_stat = IDE_DCB_SOFTRESET;

    delay(1);

  /* clear SRST bit after device */
    *ide_alt_stat = 0x00;

  /* wait for device to indicate it's not busy */
    timer = RESET_TIMEOUT;

    while (read_alt_status() & IDE_STB_BUSY) {
        if (--timer <= 0) break;
        delay(1);
    }

  /* continue with reset only if previous code didn't timeout */
    if (timer > 0) {
      /* wait for the device to indicate it is ready */
        while (!(read_alt_status() & IDE_STB_READY)) {
            if (--timer <= 0) break;
            delay(1);
        }
    }

  /* return the success(0)/failure(1) of reset? */
    return ((timer > 0) ? 0 : 1);
}


void ide_hard_reset(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Reset the hard drive device electronics.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxU8    tmp;


    tmp = PCI_ReadConfigByte(PC87415_DEVICE_NUMBER, offsetof(N415_CfigRegs, n415_Control[0]));

    PCI_WriteConfigByte(PC87415_DEVICE_NUMBER, offsetof(N415_CfigRegs, n415_Control[0]), tmp | 0x4);
    delay(1);

    PCI_WriteConfigByte(PC87415_DEVICE_NUMBER, offsetof(N415_CfigRegs, n415_Control[0]), tmp);
    delay(1);
}


/*ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
    Literals and variables for 'ide_init()'
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
const mxU8   timing_regs[] = {
    offsetof(N415_CfigRegs, n415_C1D1_Dread),
    offsetof(N415_CfigRegs, n415_C1D1_Dwrite),
    offsetof(N415_CfigRegs, n415_C1D2_Dread),
    offsetof(N415_CfigRegs, n415_C1D2_Dwrite),
    offsetof(N415_CfigRegs, n415_C2D1_Dread),
    offsetof(N415_CfigRegs, n415_C2D1_Dwrite),
    offsetof(N415_CfigRegs, n415_C2D2_Dread),
    offsetof(N415_CfigRegs, n415_C2D2_Dwrite)
};


int ide_init(void) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
    Initialize the IDE device driver.

    returns: Status of first connected device.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxVU8       status;
    int         ix, jx;
    mxVU32      timer;
    DeviceDesc  *dev;

    extern void setup_harddrive(void);
    extern void restore_harddrive(void);


    if (cur_device_ptr) return (device_list[0].status);

    setup_harddrive();

    ide_hard_reset();           /* reset all 4 drives at the same time */

    for (ix = 0; ix < NUM_HDRIVES; ix++) {

        dev = &device_list[ix];
        dev->ideptr       = (ix & 2) ? (volatile struct ide_ctl *)(PHYS_TO_K1(PCI_IO_BASE+0x170))
                                     : (volatile struct ide_ctl *)(PHYS_TO_K1(PCI_IO_BASE+0x1F0));
        dev->alt_sts      = (ix & 2) ? (mxVU8*)(PHYS_TO_K1(PCI_IO_BASE+0x376))
                                     : (mxVU8*)(PHYS_TO_K1(PCI_IO_BASE+0x3F6));
        dev->select       = (ix & 1) ? (1 << DEVICE_SELECT_BIT) : 0;
        dev->busy         = 0;
        dev->dma_timing   = 0;
        dev->pio_timing   = 0;
        dev->cyls         = 0;
        dev->heads        = 0;
        dev->sectors      = 0;
        dev->lba_capacity = 0;
        dev->caps         = 0;                  /* device capabilities */
        dev->spc          = 0;                  /* sectors per cylinder */
        dev->status       = IDE_DEVICE_INVALID; /* assume invalid */

        ide_set_device(ix);

        if (read_alt_status() == 0xff) continue;    /* nfg, do next one */

      /*
      ** Wait for the device to come out of power-up reset.
      */
        timer = getSysTicks() + POWERUP_TIMEOUT;
        jx = 0;
        do {
            status = read_alt_status();

            if (!(status & IDE_STB_BUSY)) {
                if (++jx < (2*60)) continue;
                if (status == (IDE_STB_DRDY | IDE_STB_DSC)) break;
            }

            //if (getinput(0, SW_TEST)) break;

        } while (/*(status & IDE_STB_BUSY) && */ (getSysTicks() < timer));

        if (read_alt_status() == 0) continue;   /* probably not there, do next one */

        if (!(read_alt_status() & IDE_STB_BUSY)) {

            ide_wait_busy();

            if (!ide_identify(ide_sector_buffer)) {

                DriveID *id;
                volatile struct ide_ctl *ide;
                int high, low;

                id = (DriveID *)ide_sector_buffer;

                cur_device_ptr->cyls         = id->cyls;
                cur_device_ptr->heads        = id->heads;
                cur_device_ptr->sectors      = id->sectors;
                cur_device_ptr->spc          = id->sectors*id->heads;
                cur_device_ptr->dma_ns       = id->eide_dma_min;
                cur_device_ptr->pio_ns       = id->eide_pio_iordy;
                cur_device_ptr->max_multsect = id->max_multsect;
                cur_device_ptr->lba_capacity = id->lba_capacity;
                cur_device_ptr->caps         = id->capability;

                ide = cur_device_ptr->ideptr;

                ide->overlaid.bdata.precomp_error = 0x03;   /* set transfer mode - mode in scnt reg */
                ide->overlaid.bdata.scnt = 0x0B;            /* PIO mode 3, enable IORDY */
                ide->csr = IDE_CMD_BUFFERMODE;              /* set features */

                dev->status = IDE_DEVICE_CONNECTED;

#if 1
#if !PCI_SPEED
# define PCI_CYCLE_TIME	(30)				/* assume a 33MHZ PCI bus */
#else
# define PCI_CYCLE_TIME (10*100000000L/(PCI_SPEED))	/* compute cycle time in nanoseconds */
#endif
                low = cur_device_ptr->pio_ns % PCI_CYCLE_TIME;   /* round it up to multiple of clock */
                if (low) cur_device_ptr->pio_ns += PCI_CYCLE_TIME - low;
                low = cur_device_ptr->pio_ns / PCI_CYCLE_TIME;  /* compute cycle time in clocks */

              /* NOTE: Until we can get resolved what caps should be used on the IDE bus, we will
              ** have to minimize the IDE cycle times to 180 nanoseconds (6 clocks). We do this here just to
              ** make it easy.
              */
#if 0
                if (low < 3) low = 3;   /* minimize the value to 3 clocks */
#else
                if (low < 6) low = 6;   /* minimize the value to 8 clocks */
#endif
                if (low > 33) low = 33; /* maximize the value to 33 clocks */

                high = low / 2;
                low = high + (low & 1) - 1;
                low = (-high << 4) | (-low & 0xF);
                cur_device_ptr->pio_timing = low;   /* save this so we can display it later */

                PCI_WriteConfigByte(PC87415_DEVICE_NUMBER, timing_regs[ix*2], low);     /* set read timing */
                PCI_WriteConfigByte(PC87415_DEVICE_NUMBER, timing_regs[ix*2+1], low);   /* set write timing */
#endif
            }
        }
    }

    cur_device_ptr = NULL;

    restore_harddrive();

  /* return the status of device 0 */
    return (device_list[0].status);
}


static int ide_hread_data(mxU32 *rdbuf, int nsectors) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Use the host processor to read in one sector of data from the hard drive.
  Reads the hard drive's sector buffer.

  Returns:
    0 - success
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    int     ix, nloops;
    mxVU32  *ide_data_reg;
    volatile struct ide_ctl *ide_dev;

  /* set pointer to base of IDE controller registers */
    ide_dev = cur_device_ptr->ideptr;

  /* set pointer to IDE controller data register */
    ide_data_reg = &ide_dev->overlaid.ldata;

  /* Calculate how many semi-unrolled loops to perform */
    nloops = (LONGS_PER_SECTOR * nsectors) / 16;

  /* read in the specified number of sectors */
    for (ix = 0; ix < nloops; ix++) {

         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
         *rdbuf++ = *ide_data_reg;
     }

     return (0);
}


int ide_identify(mxU32 *rdbuf) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Send an IDE_CMD_IDENTIFY command to the IDE controller.
  The data returned is read directly using the host
  processor, not through an interrupt service routine.

  Returns:
    0 if no error occurred, otherwise a value
    indicating the type of error that occurred.
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    mxU8    ackint;
    mxVU32  timeout, timer;
    volatile struct ide_ctl *ide_dev;


  /* Don't do anything if there's no device! */
    if (ide_check_devstat() == IDE_DEVICE_INVALID) return (IDE_ERB_ABORTCMD);

  /* set pointer to base of IDE controller registers */
    ide_dev = cur_device_ptr->ideptr;

    ide_send_command(IDE_CMD_IDENTIFY);

  /*
  ** Wait for the drive to indicate data is waiting to be read
  */
    timeout = 0;
    timer  = getSysTicks() + DREQ_TIMEOUT;

    while (!(read_alt_status() & IDE_STB_DATAREQ) &&
           !(timeout = (getSysTicks() > timer)));

    /* read ID information only if 'wait for DREQ' loop didn't timeout */
    if (!timeout) {

        int ix;
        DriveID *id;

      /* acknowledge the HD interrupt and read the data */
        ackint = read_status();
        ide_hread_data(rdbuf, 1);
        id = (DriveID *)rdbuf;

        for (ix = 0; ix < sizeof(id->serial_no); ix += 2) { /* swap the bytes in the ASCII fields */
            int tmp;

            tmp = id->serial_no[ix];
            id->serial_no[ix] = id->serial_no[ix+1];
            id->serial_no[ix+1] = tmp;
        }

        for (ix = 0; ix < sizeof(id->model); ix += 2) {
            int tmp;

            tmp = id->model[ix];
            id->model[ix] = id->model[ix+1];
            id->model[ix+1] = tmp;
        }
    }

  /* If an error occurred -- return it, else return zero for no error */
    if (read_alt_status() & IDE_STB_ERROR) {
        if (getGT64010Rev() > 1) {
            return (ide_dev->overlaid.bdata.precomp_error);
        } else {
            return (1); /* can't read register 1 using the old Galileo chip */
        }
    } else {
        return(0);
    }
}


static int ide_send_command(int cmd) /*
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
  Send the specified command to the IDE controller.

  Usage:
    ide_send_command(cmd);

    U8 cmd:  command to write to IDE controller.

  Returns:
    IDE_DEVICE_INVALID or IDE_DEVICE_CONNECTED
ออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ*/
{
    volatile struct ide_ctl *ide_dev;


  /* Don't do anything if there's no device! */
    if (ide_check_devstat() != IDE_DEVICE_CONNECTED) return (IDE_DEVICE_INVALID);

  /* set pointer to base of IDE controller registers */
    ide_dev = cur_device_ptr->ideptr;

  /* wait until the drive is not busy, then send the command */
    ide_wait_busy();

    ide_dev->csr = cmd;

    return (IDE_DEVICE_CONNECTED);
}
