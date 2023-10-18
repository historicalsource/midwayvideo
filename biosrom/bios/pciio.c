//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 5 $
//
// $Author: Mlynch $
//
#include	<system.h>

int pciFindCardMulti(int vend_id, int dev_id, int *n, int slot_num)
{
	int	id;

	id = get_pci_config_reg(slot_num, 0) & 0xffff;
	if(id == vend_id)
	{
		return(1);
	}
	return(0);
}

#if (!(PHOENIX_SYS & VEGAS))
void *pciMapCardMulti(int vendorId, int device_id, int memsize, int *slot, int number)
{
	void	*addr;
	int	n;
	int	num_found = 0;
	int	i;

#if (!(PHOENIX_SYS & VEGAS))
	for(i = 6; i < 10; i++)
#else
	for(i = 0; i < 6; i++)
#endif
	{
		if(pciFindCardMulti(vendorId, device_id, &n, i))
		{
			if(num_found == number)
			{
				break;
			}
			++num_found;
		}
	}
#if (!(PHOENIX_SYS & VEGAS))
	if(i == 10)
#else
	if(i == 6)
#endif
	{
		*slot = i;
		return((void *)0);
	}
	addr = (void *)SST_BASE_ADDR;
	get_pci_config_reg(i, 4);
	put_pci_config_reg(i, 4, ((int)addr & 0x1fffffff));
	*slot = i;
	return(addr);
}
#else
void *pciMapCardMulti(int vendorId, int device_id, int memsize, int *slot, int number, int anum)
{
	void	*addr;
	int	n;
	int	num_found = 0;
	int	i;

	for(i = 0; i < 6; i++)
	{
		if(pciFindCardMulti(vendorId, device_id, &n, i))
		{
			if(num_found == number)
			{
				break;
			}
			++num_found;
		}
	}
	if(i == 6)
	{
		*slot = i;
		return((void *)0);
	}
	n = get_pci_config_reg(i, 4);
	n &= ~0xf;
	n |= 0xa8000000;
	addr = (void *)n;
	*slot = i;
	return(addr);
}
#endif


//
// This function is called by sysinit() to map all cards plugged into the
// PCI bus.
//
void pci_map_cards(void)
{
	int				slot;
	unsigned long	vd;
	int				reg_num;
	unsigned long	tmp;
	unsigned long	mem_base =  PCI_MEM_BASE & 0x1fffffff;
	unsigned long	io_base =  PCI_IO_BASE & 0x1fffffff;
	unsigned long	space;
	unsigned long	addr;
	unsigned long	mask;

	//
	// Scan all of the slots starting with the top slot
	//
	for(slot = 1; slot < 6; slot++)
	{
		//
		// Get the vendor and device id
		//
		vd = get_pci_config_reg(slot, 0);

		//
		// Did we get a vendor and device id ?
		//
		if(vd != 0xffffffff)
		{
			//
			// YES - Scan all 6 base address registers and set each one
			//
			for(reg_num = 0; reg_num < 6; reg_num++)
			{
				//
				// Reset the base address register
				//
				put_pci_config_reg(slot, 4 + reg_num, -1);

				//
				// Get the value from the base address register
				//
				tmp = get_pci_config_reg(slot, 4 + reg_num);
				mask = tmp;

				//
				// Is it valid at all ?
				//
				if(!tmp)
				{
					//
					// NOPE - unused try next register
					//
					continue;
				}

				//
				// Does this register want I/O space ?
				if(tmp & 1)
				{
					//
					// YES - Set the address we are going to write to the register
					//
					addr = io_base;

					//
					// Starting value to check for how much space is needed
					//
					space = 0x4;
					mask &= ~0x3;
				}
				else
				{
					//
					// Memory - set the address we are going to write to the register
					//
					addr = mem_base;

					//
					// Starting value to check for how much space is needed
					//
					space = 0x10;
					mask &= ~0xf;
				}

				mask = ~mask;

				//
				// Figure out how much space this register wants
				//
				while(space)
				{
					//
					// Is this bit set ?
					//
					if(!(space & tmp))
					{
						//
						// NOPE - Check next bit
						//
						space <<= 1;
					}
					else
					{
						//
						// Bit is set - done
						//
						break;
					}
				}

				//
				// Is space non-zero ?
				//
				if(!space)
				{
					// NOPE - ERROR
				}
				else
				{
					while(addr & mask)
					{
						addr++;
					}
					//
					// Write the physical address to the base address register
					//
					put_pci_config_reg(slot, reg_num + 4, addr);

					addr += space;

					//
					// I/O Space ?
					//
					if(tmp & 1)
					{
						//
						// YES - increment I/O base address
						//
						io_base = addr;
					}
					else
					{
						//
						// Memory space - increment memory base address
						//
						mem_base = addr;
					}
				}
			}
		}
	}
}
