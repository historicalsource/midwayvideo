/* pci_8xx.h

	Standard 53C8XX PCI Functions header
	Author: Andy Brown

	#include this file if you are linking the pci_8xx.c code into your
	program.
	pci.h must also be included if using the pci_8xx.c code.
	This file must always follow the pci.h include file.

*/

/* 53C8xx version id's and device id's */
#define		C810_DEVICE_ID			0x0001
#define		C810_VERSION_ID		0x00
#define		C810A_VERSION_ID		0x10
#define		C810ALV_VERSION_ID	0x20
#define		C810AP_DEVICE_ID		0x0005
#define		C815_DEVICE_ID			0x0004
#define		C820_DEVICE_ID			0x0002
#define		C825_DEVICE_ID			0x0003
#define		C825_VERSION_ID	 	0x00
#define		C825A_VERSION_ID		0x10
#define 		C860_DEVICE_ID			0x0006
#define		C860_VERSION_ID		0x00
#define		C860LV_VERSION_ID		0x10
#define		C875_DEVICE_ID			0x000F
#define		NULL_DEVICE_ID			0xFFFF

/* Return values from functions */
#define		C8XX_GOOD		0x00
#define		C8XX_NO_BASE	0x81

/* IO Base, Mem Base and SCRIPT RAM Base address offsets */
#define		C8XX_CONFIG_REG_IOB	0x10
#define		C8XX_CONFIG_REG_MEMB	0x14
#define		C8XX_CONFIG_REG_RAMB	0x18

/* 53C8XX register set */
#define		SCNTL0 		0x00
#define  	SCNTL1		0x01
#define  	SCNTL2		0x02
#define  	SCNTL3		0X03
#define  	SCID			0x04
#define  	SXFER			0x05
#define  	SDID			0x06
#define  	GPREG			0x07
#define  	SFBR			0x08
#define  	SOCL			0x09
#define  	SSID			0x0A
#define  	SBCL			0x0B
#define  	DSTAT			0x0C
#define  	SSTAT0		0x0D
#define  	SSTAT1		0x0E
#define  	SSTAT2		0x0F
#define  	DSA			0x10
#define  	ISTAT			0x14
#define  	CTEST0		0x18
#define  	CTEST1		0x19
#define  	CTEST2		0x1A
#define  	CTEST3		0x1B
#define  	TEMP			0x1C
#define  	DFIFO			0x20
#define  	CTEST4		0x21
#define  	CTEST5		0x22
#define  	CTEST6		0x23
#define  	DBC			0x24
#define  	DCMD			0x27
#define  	DNAD			0x28
#define  	DSP			0x2C
#define  	DSPS			0x30
#define  	SCRATCHA		0x34
#define  	SCRATCHA0	0x34
#define  	SCRATCHA1	0x35
#define  	SCRATCHA2	0x36
#define  	SCRATCHA3	0x37
#define  	DMODE			0x38
#define  	DIEN			0x39
#define  	SBR			0x3A
#define  	DCNTL			0x3B
#define  	ADDER			0x3C
#define  	SIEN0			0x40
#define  	SIEN1			0x41
#define  	SIST0			0x42
#define  	SIST1			0x43
#define  	SLPAR			0x44
#define  	SWIDE			0x45
#define  	MACNTL		0x46
#define  	GPCNTL		0x47
#define  	STIME0		0x48
#define  	STIME1		0x49
#define  	RESPID0		0x4A
#define  	RESPID1		0x4B
#define  	STEST0		0x4C
#define  	STEST1		0x4D
#define  	STEST2		0x4E
#define  	STEST3		0x4F
#define  	SIDL			0x50
#define  	SODL			0x54
#define  	SODL1			0x55
#define  	SBDL			0x58
#define  	SCRATCHB		0x5C
#define  	SCRATCHB0	0x5C
#define  	SCRATCHB1	0x5D
#define  	SCRATCHB2	0x5E
#define  	SCRATCHB3	0x5F
#define  	SCRATCHC		0x60
#define  	SCRATCHC0	0x60
#define  	SCRATCHC1	0x61
#define  	SCRATCHC2	0x62
#define  	SCRATCHC3	0x63
#define  	SCRATCHD		0x64
#define  	SCRATCHD0	0x64
#define  	SCRATCHD1	0x65
#define  	SCRATCHD2	0x66
#define  	SCRATCHD3	0x67
#define  	SCRATCHE		0x68
#define  	SCRATCHE0	0x68
#define  	SCRATCHE1	0x69
#define  	SCRATCHE2	0x6A
#define  	SCRATCHE3	0x6B
#define  	SCRATCHF		0x6C
#define  	SCRATCHF0	0x6C
#define  	SCRATCHF1	0x6D
#define  	SCRATCHF2	0x6E
#define  	SCRATCHF3	0x6F
#define  	SCRATCHG		0x70
#define  	SCRATCHG0	0x70
#define  	SCRATCHG1	0x71
#define  	SCRATCHG2	0x72
#define 		SCRATCHG3	0x73
#define  	SCRATCHH		0x74
#define  	SCRATCHH0	0x74
#define  	SCRATCHH1	0x75
#define  	SCRATCHH2	0x76
#define  	SCRATCHH3	0x77
#define  	SCRATCHI		0x78
#define  	SCRATCHI0	0x78
#define  	SCRATCHI1	0x79
#define  	SCRATCHI2	0x7A
#define  	SCRATCHI3	0x7B
#define  	SCRATCHJ		0x7C
#define  	SCRATCHJ0	0x7C
#define  	SCRATCHJ1	0x7D
#define  	SCRATCHJ2	0x7E
#define  	SCRATCHJ3	0x7F

