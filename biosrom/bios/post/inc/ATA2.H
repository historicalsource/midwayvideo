#ifndef _ATA2_H_
#define _ATA2_H_

/*
ษออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออป
บ                                                                            บ
บ File:    ATA2.H                                                            บ
บ Author:  Jack Miller                                                       บ
บ          Dave Wagner                                                       บ
บ Created: 05-Jun-1996                                                       บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ                                                                            บ
บ     ATA-2 device interface specification manifest definitions.             บ
บ                                                                            บ
ฬออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออน
บ HISTORY:                                                                   บ
บ                                                                            บ
บ  28May96 JVM 1.00 - Created.                                               บ
บ                                                                            บ
บ                                                                            บ
ศออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออผ
*/

#include <compiler.h>

/*
** This is Atari's version of the ATA-2 interface controller registers.
*/

struct ide_ctl {

  union {

    mxU32       ldata;

    struct {

      mxU16     data;
      mxU16     pad;

    } wdata;

    struct {
      mxU8      data;
      mxU8      precomp_error;
      mxU8      scnt;
      mxU8      snum;
    } bdata;

  } overlaid;

  mxU8          lcylinder;
  mxU8          hcylinder;
  mxU8          drive_head;
  mxU8          csr;

} IDE_CTL;


/* ATA-2 disk controller commands */

#define IDE_CMD_RECALIBRATE     (0x10)
#define IDE_CMD_READ_SECTORS    (0x20)
#define IDE_CMD_WRITE_SECTORS   (0x30)
#define IDE_CMD_VERIFY_SECTORS  (0x40)
#define IDE_CMD_FORMAT_TRACK    (0x50)
#define IDE_CMD_SEEK            (0x70)
#define IDE_CMD_EXEC_DEV_DIAGS  (0x90)
#define IDE_CMD_INIT_DEV_PARAMS (0x91)
#define IDE_CMD_READ_MULTIPLE   (0xC4)
#define IDE_CMD_WRITE_MULTIPLE  (0xC5)
#define IDE_CMD_MULTIPLE_MODE   (0xC6)
#define IDE_CMD_DMA_READ        (0xC8)
#define IDE_CMD_READ_BUFFER     (0xE4)
#define IDE_CMD_WRITE_BUFFER    (0xE8)
#define IDE_CMD_IDENTIFY_DEVICE (0xEC)
#define IDE_CMD_SET_FEATURES    (0xEF)

/* Atari's aliases for some of the above */

#define IDE_CMD_SREAD           IDE_CMD_READ_SECTORS
#define IDE_CMD_SWRITE          IDE_CMD_WRITE_SECTORS
#define IDE_CMD_SVERIFY         IDE_CMD_VERIFY_SECTORS
#define IDE_CMD_FORMAT          IDE_CMD_FORMAT_TRACK
#define IDE_CMD_DIAGNOSTICS     IDE_CMD_EXEC_DEV_DIAGS
#define IDE_CMD_INITPARMS       IDE_CMD_INIT_DEV_PARAMS
#define IDE_CMD_MREAD           IDE_CMD_READ_MULTIPLE
#define IDE_CMD_MWRITE          IDE_CMD_WRITE_MULTIPLE
#define IDE_CMD_MULTIMODE       IDE_CMD_MULTIPLE_MODE
#define IDE_CMD_DMAREAD         IDE_CMD_DMA_READ
#define IDE_CMD_BREAD           IDE_CMD_READ_BUFFER
#define IDE_CMD_BWRITE          IDE_CMD_WRITE_BUFFER
#define IDE_CMD_IDENTIFY        IDE_CMD_IDENTIFY_DEVICE
#define IDE_CMD_BUFFERMODE      IDE_CMD_SET_FEATURES



/* ATA-2 Staus register bit definitions */

#define IDE_STB_BSY             (0x80)
#define IDE_STB_DRDY            (0x40)
#define IDE_STB_DF              (0x20)
#define IDE_STB_DSC             (0x10)
#define IDE_STB_DRQ             (0x08)
#define IDE_STB_CORR            (0x04)
#define IDE_STB_IDX             (0x02)
#define IDE_STB_ERR             (0x01)

/* Atari's aliases for the above */

#define IDE_STB_BUSY            IDE_STB_BSY
#define IDE_STB_READY           IDE_STB_DRDY
#define IDE_STB_WRFAULT         IDE_STB_DF
#define IDE_STB_SEEKDONE        IDE_STB_DSC
#define IDE_STB_DATAREQ         IDE_STB_DRQ
#define IDE_STB_CORRDATA        IDE_STB_CORR
#define IDE_STB_INDEX           IDE_STB_IDX
#define IDE_STB_ERROR           IDE_STB_ERR



/* ATA-2 Error register bit definitions */

#define IDE_ERB_RESERVED        (0x80)
#define IDE_ERB_UNC             (0x40)
#define IDE_ERB_MC              (0x20)
#define IDE_ERB_IDNF            (0x10)
#define IDE_ERB_MCR             (0x08)
#define IDE_ERB_ABRT            (0x04)
#define IDE_ERB_TK0NF           (0x02)
#define IDE_ERB_AMNF            (0x01)

/* Atari's aliases for the above */

#define IDE_ERB_BADBLOCK        IDE_ERB_RESERVED
#define IDE_ERB_UNCDATA         IDE_ERB_UNC
#define IDE_ERB_IDNFOUND        IDE_ERB_IDNF
#define IDE_ERB_ABORTCMD        IDE_ERB_ABRT
#define IDE_ERB_TK0NFOUND       IDE_ERB_TK0NF
#define IDE_ERB_AMNFOUND        IDE_ERB_AMNF



/* ATA-2 Device Control register bit definitions */

#define IDE_DCB_SOFTRESET       (0x06)  /* SRST = 1, nIEN = 1 */
#define IDE_DCB_IEN             (0x00)  /* SRST = 0, nIEN = 0 */



/* Device/Head register bit definitions */

#define DRIVE_HEAD_INFO         (0xA0)  /*sector size and master select */
#define CHS_MODE                (0x00)  /* bit 6 of Device/Head = 0 = CHS */
#define LBA_MODE                (0x40)  /* bit 6 of Device/Head = 1 = LBA */


/* General drive interface definitions */

#define BYTES_PER_SECTOR        (0x200) /* 512 */
#define WORDS_PER_SECTOR        (0x100) /* 256 */
#define LONGS_PER_SECTOR        (0x080) /* 128 */

#define IDE_INTS_OFF            (0x0)
#define IDE_INTS_ON             (0x1)
#define IDE_DEVICE_INVALID      (0x0)
#define IDE_DEVICE_CONNECTED    (0x1)
#define IDE_DEVICE_TIMEOUT      (0x2)

#endif
