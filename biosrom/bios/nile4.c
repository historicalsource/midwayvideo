/****************************************************************************/
/*                                                                          */
/* nile4.c - Support functions for NILE IV                                  */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* Version:     1.00                                                        */
/* Date:        10/27/95                                                    */
/*                                                                          */
/* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* Use, duplication, or disclosure is strictly forbidden unless approved    */
/* in writing by Williams Electronics Games Inc.                            */
/*                                                                          */
/* $Revision: 10 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

static int	nile_initialized = 0;
int	nile4_version;

int get_nile4_version(void)
{
	return(nile4_version);
}

int is_revision_a(void)
{
	return(1);
}

/****************************************************************************/
/* get_pci_config_reg() - This function returns the value of the PCI config */
/* register specified by "reg_num" from the PCI device specified by         */
/* "dev_num".                                                               */
/****************************************************************************/
int get_pci_config_reg(int dev_num, int reg_num)
{
	unsigned int	old_init_reg;
	int				val;

	// Is the slot number < 0 or > 10 ?
	if(dev_num < 0 || dev_num > 5)
	{
		return(-1);
	}

	// Is this targeted at the NILE IV
	if(!dev_num)
	{
		// YES - get the data from registers
		val = *((volatile int *)(NILE4_INTERNAL_PCI + (reg_num << 2)));

		// Done
		return;
	}

	// Save current value of register
	old_init_reg = *((volatile unsigned int *)NILE4_PCI_INIT1_LO_ADDR);

	// NOTE - WHILE DOING THIS DO NOT ALLOW INTERRUPTS THAT COULD CAUSE
	// AND UNDESIRABLE EFFECT IF A PCI CYCLE OCCURS IN THE INTERRUPT

	// Set configurtion command type and 32 bit access
	*((volatile int *)NILE4_PCI_INIT1_LO_ADDR) = (PCI_CONFIG_CMD_TYPE | PCI_32BIT);

	// Do the read to perform the configuration cycle read
	val = *((volatile int *)(0xa0000000 | PCIW1_ADDR + (reg_num << 2) + (1 << (dev_num + PCI_IDSEL_SHIFT))));

	// Reset the register
	*((volatile unsigned int *)NILE4_PCI_INIT1_LO_ADDR) = old_init_reg;

	// Return the value
	return(val);
}


/****************************************************************************/
/* put_pci_config_reg() - This function writes the data specified by "data" */
/* to the PCI configuration register specified by "reg_num" on the device   */
/* specified by "dev_num".                                                  */
/****************************************************************************/
void put_pci_config_reg(int dev_num, int reg_num, int data)
{
	unsigned int	old_init_reg;

	// Is the slot number < 0 or > 10 ?
	if(dev_num < 0 || dev_num > 5)
	{
		return;
	}

	// Is this targeted at the NILE IV
	if(!dev_num)
	{
		// YES - put data in registers
		*((volatile int *)(NILE4_INTERNAL_PCI + (reg_num << 2))) = data;

		// Done
		return;
	}

	// Save current value of register
	old_init_reg = *((volatile unsigned int *)NILE4_PCI_INIT1_LO_ADDR);

	// NOTE - WHILE DOING THIS DO NOT ALLOW INTERRUPTS THAT COULD CAUSE
	// AND UNDESIRABLE EFFECT IF A PCI CYCLE OCCURS IN THE INTERRUPT

	// Turn on all of these bits
	*((volatile int *)NILE4_PCI_INIT1_LO_ADDR) = (PCI_CONFIG_CMD_TYPE | PCI_32BIT);

	// Do the read to perform the configuration cycle read
	*((volatile int *)(0xa0000000 | PCIW1_ADDR + (reg_num << 2) + (1 << (dev_num + PCI_IDSEL_SHIFT)))) = data;

	// Reset the register
	*((volatile unsigned int *)NILE4_PCI_INIT1_LO_ADDR) = old_init_reg;
}

#ifndef TEST
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

int sysinit(void)
{
	int	i;

	if(nile_initialized)
	{
		return(1);
	}

	nile_initialized = 1;

	// Turn on the PCI warm and cold reset bits
	*((volatile int *)NILE4_PCI_CNTL_HI_ADDR) |= 0xc0000000;

	// Set Turn off the PCI clock control bits
	*((volatile int *)NILE4_PCI_CNTL_LO_ADDR) &= ~0xe;

	// Set the PCI clock source from external (33 Mhz)
	*((volatile int *)NILE4_PCI_CNTL_LO_ADDR) |= (PCI_CLOCK_SPEED << 1);

	// Wait a bit
	for(i = 0; i < 100000; i++) ;

	// Release the PCI hard reset
	*((volatile int *)NILE4_PCI_CNTL_HI_ADDR) &= ~0x80000000;

	// Wait a bit
	for(i = 0; i < 100000; i++) ;

	// Release the PLL_SYNC bit
	*((volatile int *)NILE4_PCI_CNTL_HI_ADDR) &= ~0x20000000;

	// Wait a bit (> 100ms)
	for(i = 0; i < 36000000; i++) ;

	// Release the warm reset bit
	*((volatile int *)NILE4_PCI_CNTL_HI_ADDR) &= ~0x40000000;

	// Wait a bit
	for(i = 0; i < 100000; i++) ;

	// Set up the PCI BAR registers
	*((volatile int *)NILE4_PCI_BAR0_ADDR) = (*((volatile int *)(NILE4_BASE_ADDR + 0x00)) & ~0xff) | 0x8;
	*((volatile int *)NILE4_PCI_BAR1_ADDR) = (*((volatile int *)(NILE4_BASE_ADDR + 0x08)) & ~0xff) | 0x8;
	*((volatile int *)NILE4_PCI_BAR2_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x10)) & ~0xff;
	*((volatile int *)NILE4_PCI_BAR3_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x18)) & ~0xff;
	*((volatile int *)NILE4_PCI_BAR4_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x20)) & ~0xff;
	*((volatile int *)NILE4_PCI_BAR5_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x28)) & ~0xff;
	*((volatile int *)NILE4_PCI_BAR6_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x30)) & ~0xff;
	*((volatile int *)NILE4_PCI_BAR7_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x38)) & ~0xff;
	*((volatile int *)NILE4_PCI_BAR8_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x40)) & ~0xff;
	*((volatile int *)NILE4_PCI_BARB_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x78)) & ~0xff;
	*((volatile int *)NILE4_PCI_BARC_ADDR) = *((volatile int *)(NILE4_BASE_ADDR + 0x70)) & ~0xff;

	// Set up the PCI Arbitor
	*((volatile int *)NILE4_PCI_ARBLO_ADDR) = ((0x3f<<0)|(0<<8)|(0<<16)|(0<<24)|(1<<28));
	*((volatile int *)NILE4_PCI_ARBHI_ADDR) = ((1<<0)|(1<<4)|(0xf<<8)|(0<<12)|(0<<16)|(5<<20));

	// Make sure the PCI Command register allows memory access and fast
	// back to back transactions
	// NOTE - Turning on fast back to back transactions make PCI hang
	*((volatile short *)NILE4_PCI_COMMAND_ADDR) |= (1<<1);

	// Set up the interrupt routing through the NILE IV.
	// IDE (PCI INTD)  -> INT0
	// SIO (PCI INTC)  -> INT1
	// All NILE IV     -> INT2
	// SCSI (PCI INTA) -> INT4
	// DEBUG SWITCH (PCI INTE) -> INT4

	// Don't let NILE IV drive interrupt lines	
	*((volatile int *)NILE4_INT_STAT1_HI_ADDR) &= ~((1 << 16)|(1 << 17)|(1 << 18)|(1 << 19)|(1 << 20)|(1 << 21));

	// All interrupts OFF
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) = 0;
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) = 0;

	// CPU Interface parity error -> INT2
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (2 << 0);

	// CPU No Target Decode -> INT2
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (2 << 4);

	// Memory Check Error -> INT2
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (2 << 8);

	// DMA Controller Interrupt -> INT2
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (2 << 12);

#if 0
	// Make sure all UART interrupts are disabled
	*((volatile int *)NILE4_UART_IER) = 0;

	// UART Interrupt -> INT2 and Enabled
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= ((2 << 16)|(1<<19));
#else
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= ((2 << 16));
#endif

	// Watchdog timer -> INT2
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (2 << 20);

	// Local Bus Ready Timeout -> INT2
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (2 << 28);

	// PCI INTD -> INT0 (PCI IDE Controller)
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= ((0 << 12) | (1 << 15));

	// PCI INTC -> INT1 (SIO board)
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= ((1 << 8) | (1 << 11));

	// PCI INTA -> INT4 (SCSI Debugging card)
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= ((4 << 0) | (1 << 3));

	// PCI INTE -> INT4 (Debug Switch interrupt)
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= ((4 << 16) | (1 << 19));

	// PCI SERR -> INT2
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= (2 << 24);

	// PCI Internal Error -> INT2
	*((volatile int *)NILE4_INT_CTRL_HI_ADDR) |= (2 << 28);

	// Make PCI INTC & INTD Level triggered and PCI INTA, INTB, & INTE edge
	// triggered.  All are active low.
	*((volatile int *)NILE4_PCI_INT_CTRL_ADDR) = ((1<<0)|(1<<2)|(3<<4)|(3<<6)|(1<<8));

	// Make sure all interrupts are cleared
	*((volatile int *)NILE4_INT_CLR_ADDR) = -1;

	// Let the NILE IV drive the processor interrupts
	*((volatile int *)NILE4_INT_STAT1_HI_ADDR) |= ((1 << 16)|(1 << 17)|(1 << 18)|(1 << 19)|(1 << 20));

	// Set up the NILE IV Timer to generate interrupts every 1ms
	start_wdog_timer(1000000 / NANOS_PER_TICK);

	// Enable the NILE IV Watchdog timer interrupt
	*((volatile int *)NILE4_INT_CTRL_LO_ADDR) |= (1 << 23);

	// Grab the version of the NILE IV
	nile4_version = get_pci_config_reg(0, 2) & 0xff;

	// Map all of the cards plugged into the PCI bus
	pci_map_cards();

	return(1);
}


int sysopen(struct iocntb *io)
{
	if(!nile_initialized)
	{
		return(-1);
	}
	return(0);
}

int sysioctl(register struct iocntb *io, int cmd, int arg)
{
	pci_reg_info_t	*pi;

	if(!nile_initialized)
	{
		return(-1);
	}
	switch(cmd)
	{
		case FIOCGETPCICONFIGREG:
		{
			pi = (pci_reg_info_t *)arg;
			if(pi->slot_num < 0 || pi->slot_num > 5)
			{
				return(PCI_INVALID_SLOT_NUM);
			}
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
			if(pi->slot_num < 0 || pi->slot_num > 5)
			{
				return(PCI_INVALID_SLOT_NUM);
			}
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

void enable_write_merge(void)
{
	*((volatile int *)NILE4_PCI_INIT1_LO_ADDR) |= 0xc0;
}

void disable_write_merge(void)
{
	*((volatile int *)NILE4_PCI_INIT1_LO_ADDR) &= ~0xc0;
}

