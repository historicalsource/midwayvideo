/****************************************************************************/
/*                                                                          */
/* gt64010.c - Support functions for GT-64010                               */
/*                                                                          */
/* Written by:  Jason Skiles, Michael J. Lynch                              */
/* Version:     1.00                                                        */
/* Date:        10/27/95                                                    */
/*                                                                          */
/* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* Use, duplication, or disclosure is strictly forbidden unless approved    */
/* in writing by Williams Electronics Games Inc.                            */
/*                                                                          */
/* $Revision: 7 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<system.h>
#if (SYSTEM_CONTROLLER & GT64010)
#include	<io.h>
#include	<gt64010.h>
#include	<ioctl.h>

#if (PHOENIX_SYS & FLAGSTAFF)
#error gt64010.c - FLAGSTAFF system not supported yet
#endif

int			version_64010 = 0;
static int	version_determined = 0;
static int	gt_initialized = 0;

static void get_version(void)
{
	T_pci_internal_config_addr_data	ca;
	int										data;
	int										un_data;

	ca.data = GT_RESET;
	// Assume Rev. A part
	ca.d.reg_num = 2;
	ca.d.dev_num = 0;
	ca.d.access_ctrl = GT_ALLOW_ACCESS;
	*((volatile int *)GT_PCI_INT_CONF_ADDR) = GTREG(ca.data);
	data = *((volatile int *)GT_PCI_INT_CONF_DATA);
	un_data = GTREG(data);

	// If what we read was really the device
	// and vendor ID register then this is
	// NOT a Rev. A part
	if(un_data != 0x14611ab)
	{
		version_64010 = 1;
	}
	version_determined = 1;
}

int is_revision_a(void)
{
	return(version_64010);
}

/****************************************************************************/
/* get_pci_config_reg() - This function returns the value of the PCI config */
/* register specified by "reg_num" from the PCI device specified by         */
/* "dev_num".                                                               */
/****************************************************************************/
int get_pci_config_reg(int dev_num, int reg_num)
{
	T_pci_internal_config_addr_data	ca;
	int										data;

	if(!version_determined)
	{
		get_version();
	}
	ca.data = GT_RESET;
	ca.d.reg_num = reg_num;
	if(!(dev_num|version_64010))
	{
		ca.d.reg_num <<= 2;
	}
	ca.d.dev_num = dev_num;
	ca.d.access_ctrl = GT_ALLOW_ACCESS;
	*((volatile int *)GT_PCI_INT_CONF_ADDR) = GTREG(ca.data);
	data = *((volatile int *)GT_PCI_INT_CONF_DATA);

	// Check for a master abort (this is a hack)
	// For one reason or another the GT64010 generates a master abort
	// when the configuration space registers on the IDE controller are
	// read.  This is here only to take care of that situation.
	if(*((volatile int *)GT_INT_CAUSE) & 0x40000)
	{
		*((volatile int *)GT_INT_CAUSE) ^= 0x40000;
	}

	if(!dev_num)
	{
		return(GTREG(data));
	}
	return(data);
}


/****************************************************************************/
/* put_pci_config_reg() - This function writes the data specified by "data" */
/* to the PCI configuration register specified by "reg_num" on the device   */
/* specified by "dev_num".                                                  */
/****************************************************************************/
void put_pci_config_reg(int dev_num, int reg_num, int data)
{
	T_pci_internal_config_addr_data	ca;
	int										dtmp;

	if(!version_determined)
	{
		get_version();
	}
	dtmp = data;
	ca.data = GT_RESET;
	ca.d.reg_num = reg_num;
	if(!(dev_num|version_64010))
	{
		ca.d.reg_num <<= 2;
	}
	ca.d.dev_num = dev_num;
	ca.d.access_ctrl = GT_ALLOW_ACCESS;
	*((volatile int *)GT_PCI_INT_CONF_ADDR) = GTREG(ca.data);

	// Check for a master abort (this is a hack)
	// For one reason or another the GT64010 generates a master abort
	// when the configuration space registers on the IDE controller are
	// read.  This is here only to take care of that situation.
	if(*((volatile int *)GT_INT_CAUSE) & 0x40000)
	{
		*((volatile int *)GT_INT_CAUSE) ^= 0x40000;
	}

	if(!dev_num)
	{
		dtmp = GTREG(data);
	}
	*((volatile int *)GT_PCI_INT_CONF_DATA) = dtmp;
}

#ifndef TEST
int pciSetConfigData(int addr, int devnum, int *val)
{
	put_pci_config_reg(devnum,  (addr>>2), *val);
	return(1);
}
#else
typedef struct {
	unsigned long	regAddress;
	unsigned long	sizeInBytes;
	int				rwFlag;
} PciRegister;

int pciSetConfigData(PciRegister reg, int slot, unsigned int *from)
{
	unsigned int	val;
	unsigned int	v1;
	unsigned int	mask;
	int				i;

	v1 = get_pci_config_reg(slot, reg.regAddress >> 2);
	val = *from << ((reg.regAddress & 3) << 3);

	for(i = 0; i < reg.sizeInBytes; i++)
	{
		mask <<= 8;
		mask |= 0xff;
	}

	for(i = 0; i < (reg.regAddress & 3); i++)
	{
		mask <<= 8;
	}
	val |= (v1 & ~(mask << (reg.sizeInBytes << 3)));

	put_pci_config_reg(slot, reg.regAddress >> 2, val);

	return(1);
}
#endif

#ifndef TEST
int pciGetConfigData(int addr, int devnum, int *val)
{
	*val = get_pci_config_reg(devnum, (addr>>2));
	return(1);
}
#else
int pciGetConfigData(PciRegister reg, int slot, unsigned int *to)
{
	unsigned int	val;
	unsigned int	mask = 0xffffffff;

	val = get_pci_config_reg(slot, reg.regAddress >> 2);
	if(reg.sizeInBytes & 3)
	{
		val >>= ((reg.regAddress & 3) << 3);
		val &= (mask >> (reg.sizeInBytes << 3));
	}
	*to = val;

	return(1);
}

#endif


/****************************************************************************/
/* gt64010_init() - This function initializes the GT-64010 controller.      */
/****************************************************************************/
int gt64010_init(void)
{
	T_gt64010_pci_config_reg1	sc;
	int				i;

#if (PHOENIX_SYS & SA1)
/* Reset the PCI bus */
	*((volatile int *)PCI_RESET_ADDR) = 0;

/* Wait a bit */
	for(i = 0; i < 10000000; i++)
	{
		;
	}

/* First release the PCI bus from reset */
	*((volatile int *)PCI_RESET_ADDR) = -1;

/* Wait a bit */
	for(i = 0; i < 10000000; i++)
	{
		;
	}
#elif (PHOENIX_SYS & SEATTLE)
	// Set the PCI bus reset bit
	*((volatile int *)PCI_RESET_ADDR) &= ~(RST_PCI|RST_3DFX);

	// Wait a bit
	for(i = 0; i < 10000000; i++)
	{
		;
	}

	// Release the PCI bus reset bit
	*((volatile int *)PCI_RESET_ADDR) |= (RST_PCI|RST_3DFX);

	// Wait a bit
	for(i = 0; i < 10000000; i++)
	{
		;
	}

	// Set the PCI arbitor to park on the GT64010
	*((volatile int *)PCI_ARBITOR) = PCI_ARB_PARK_ON_GALILEO;
#endif

	/* First read the PCI Status and Command Register from the GT-64010 */
	sc.data = get_pci_config_reg(GT64010_DEVICE_NUMBER, PCI_CONFIG_REG1);

	/* Make the GT-64010 a PCI Master */
	sc.d.mas_en = GT_ENABLE;		/* GT-64010 is PCI master */
	sc.d.mem_en = GT_ENABLE;		/* Allow memory access */
	put_pci_config_reg(GT64010_DEVICE_NUMBER, PCI_CONFIG_REG1, sc.data);

	/* Clear out any pending interrupts */
	*((volatile int *)GT_INT_CAUSE) = 0;

	/* Set the PCI retry counter */
	*((volatile int *)GT_PCI_INT_TO_RETRY) = 0x70f;

	/* Enable a few interrupts */
	*((volatile int *)GT_INT_CPU_MASK) = 0x0010001e;


	i = 10000;
	do
	{
		*((volatile int *)GT_PCI_INT_COMMAND) = 1;
	} while( !(*((volatile int *)GT_PCI_INT_COMMAND) & 1) && --i);

	// Make sure the two timeout registers are set to maximums
	*((volatile int *)GT_PCI_INT_TO_RETRY) |= 0xffff;

	gt_initialized = 1;

	return(0);
}


/****************************************************************************/
/* get_pci_slot_configuration_info() - This function filles the structure   */
/* specified by "*to" with the 64 bytes of configuration data from the PCI  */
/* device specified by "dev_num".                                           */
/****************************************************************************/
void get_pci_slot_configuration_info(int *to, int dev_num)
{
	int				i;

	for(i = 0; i < 16; i++)
	{
		*to++ = get_pci_config_reg(dev_num, i);
	}
}


void disable_dma(int channel)
{
	volatile int	*dc_ptr;
	T_dma_control	dc;

	dc_ptr = (volatile int *)GT_DMA_CHAN0_CONTROL;
	dc_ptr += channel;
	dc.data = GTREG((*dc_ptr));
	dc.d.enable = GT_DISABLE;
	*dc_ptr = GTREG(dc.data);
}

int sysopen(struct iocntb *io)
{
	if(!gt_initialized)
	{
		return(-1);
	}
	return(0);
}

int sysioctl(register struct iocntb *io, int cmd, int arg)
{
	gt_reg_info_t	*ri;
	pci_reg_info_t	*pi;

	switch(cmd)
	{
		case FIOCGETGTREG:
		{
			ri = (gt_reg_info_t *)arg;
			ri->data = *((volatile int *)(ri->reg_addr | GT_64010_BASE));
			break;
		}
		case FIOCSETGTREG:
		{
			ri = (gt_reg_info_t *)arg;
			*((volatile int *)(ri->reg_addr | GT_64010_BASE)) = ri->data;
			break;
		}
		case FIOCGETPCICONFIGREG:
		{
			pi = (pci_reg_info_t *)arg;
#if (PHOENIX_SYS & SA1)
			if((pi->slot_num > 0 && pi->slot_num < 5) || pi->slot_num > 9)
			{
				return(PCI_INVALID_SLOT_NUM);
			}
#elif (PHOENIX_SYS & SEATTLE)
			if((pi->slot_num > 0 && pi->slot_num < 5) || pi->slot_num > 8)
			{
				return(PCI_INVALID_SLOT_NUM);
			}
#endif
			if(pi->reg_num < 0 || pi->reg_num > 15)
			{
				return(PCI_INVALID_REG_NUM);
			}
			pi->data = get_pci_config_reg(pi->slot_num, pi->reg_num);
			break;
		}
		case FIOCSETPCICONFIGREG:
		{
			pi = (pci_reg_info_t *)arg;
#if (PHOENIX_SYS & SA1)
			if((pi->slot_num > 0 && pi->slot_num < 5) || pi->slot_num > 9)
			{
				return(PCI_INVALID_SLOT_NUM);
			}
#elif (PHOENIX_SYS & SEATTLE)
			if((pi->slot_num > 0 && pi->slot_num < 5) || pi->slot_num > 8)
			{
				return(PCI_INVALID_SLOT_NUM);
			}
#endif
			if(pi->reg_num < 0 || pi->reg_num > 15)
			{
				return(PCI_INVALID_REG_NUM);
			}
			put_pci_config_reg(pi->slot_num, pi->reg_num, pi->data);
			break;
		}
		default:
		{
			return(PCI_INVALID_IOCTL);
		}
	}
	return(0);
}

#endif
