//
// vmm.c - Virtual Memory Driver
//
// Copyright (c) 1998 by Midway Video Inc.
//
// $Revision: 2 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<pci0646.h>
#include	<ioctl.h>

#ifdef VMM

//#define	VMM_DEBUG

//
// Function prototypes
//
void write_tlb(int, tlb_entry_t *);
void unlock_tlb(int);
int get_random(void);
int probe_tlb(tlb_entry_t *);
static unsigned int virt2pfn(unsigned int);
static unsigned int virt2page(unsigned int);
static void save_disk_pointers(void **);
static void restore_disk_pointers(void **);

//
// Array used to map disk blocks to virtual addresses for VMM
//
extern void	*callback;
extern void	*proc;
extern int	__memory_size;
extern int	(*user_handler[])(int, unsigned int *);

static int	phys_mem_base;
static int	phys_mem_size;
static int	disk_map_entries;
static int	mem_map_entries;
static char	*core_mem;
static int	disk_sector_offset = 0;
static int	*mem_map;
static int	*disk_map;

void vmm_tlbl_handler(int cause, unsigned int *r_save)
{
	tlb_entry_t		tlb;
	int				index;
	unsigned int	bad_vaddr;

	bad_vaddr = r_save[CP0_BADVADDR];

	// Is the virtual address less than allowed ?
	if(bad_vaddr < VP_SIZE)
	{
		unhandled_exception("TLB Load - Virtual address too low");
		return;
	}

	// NO - Is the virtual address above the allocated core memory ?
	if(bad_vaddr >= (unsigned)core_mem)
	{
		unhandled_exception("TLB Load - Virtual address too high");
		return;
	}

	// Set up for the TLB probe
	tlb.entry_hi.asid = 0;
	tlb.entry_hi.vpn2 = r_save[CP0_BADVADDR] >> VPN_SHIFT;

	// Is this virtual page number in the TLB ?
	if((index = tlb_probe(&tlb)) < 0)
	{
		unhandled_exception("TLB Load - No VPN match");
		return;
	}

	// Get the entry
	get_tlb_entry(index, &tlb);

	// Are both of the page frames valid ?
	if((tlb.entry_lo0.v & tlb.entry_lo1.v) == PF_VALID)
	{
		unhandled_exception("TLB Load - both PFN's valid");
		return;
	}

	// Is the even page frame valid ?
	else if(tlb.entry_lo0.v != PF_VALID)
	{
		// NOPE - Make it valid
		tlb.entry_lo0.v = PF_VALID;
		tlb.entry_lo0.d = PF_WRITEABLE;
		tlb.entry_lo0.g = PF_GLOBAL;
		tlb.entry_lo0.c = PF_NONCOHERENT;
	}

	// Is the odd page frame valid ?
	if(tlb.entry_lo1.v != PF_VALID)
	{
		// NOPE - Make it valid
		tlb.entry_lo1.v = PF_VALID;
		tlb.entry_lo1.d = PF_WRITEABLE;
		tlb.entry_lo1.g = PF_GLOBAL;
		tlb.entry_lo1.c = PF_NONCOHERENT;
	}

	// Write the updated TLB entry
	write_tlb(index, &tlb);
}


void vmm_tlbs_handler(int cause, unsigned int *r_save)
{
	tlb_entry_t		tlb;
	int				index;
	unsigned int	bad_vaddr;

	bad_vaddr = r_save[CP0_BADVADDR];

	// Is the virtual address less than allowed ?
	if(bad_vaddr < VP_SIZE)
	{
		unhandled_exception("TLB Store - Virtual address too low");
		return;
	}

	// NO - Is the virtual address above the allocated core memory ?
	if(bad_vaddr >= (unsigned)core_mem)
	{
		unhandled_exception("TLB Store - Virtual address too high");
		return;
	}

	// Set up for the TLB probe
	tlb.entry_hi.asid = 0;
	tlb.entry_hi.vpn2 = r_save[CP0_BADVADDR] >> VPN_SHIFT;

	// Is this virtual page number in the TLB ?
	if((index = tlb_probe(&tlb)) < 0)
	{
		unhandled_exception("TLB Store - No VPN match");
		return;
	}

	// Get the entry
	get_tlb_entry(index, &tlb);

	// Are both of the page frames valid ?
	if((tlb.entry_lo0.v & tlb.entry_lo1.v) == PF_VALID)
	{
		unhandled_exception("TLB Store - Both PFN's valid");
		return;
	}

	// Is the even page frame valid ?
	else if(tlb.entry_lo0.v != PF_VALID)
	{
		// NOPE - Make it valid
		tlb.entry_lo0.v = PF_VALID;
		tlb.entry_lo0.d = PF_WRITEABLE;
		tlb.entry_lo0.g = PF_GLOBAL;
		tlb.entry_lo0.c = PF_NONCOHERENT;
	}

	// Is the odd page frame valid ?
	if(tlb.entry_lo1.v != PF_VALID)
	{
		// NOPE - Make it valid
		tlb.entry_lo1.v = PF_VALID;
		tlb.entry_lo1.d = PF_WRITEABLE;
		tlb.entry_lo1.g = PF_GLOBAL;
		tlb.entry_lo1.c = PF_NONCOHERENT;
	}

	// Write the updated TLB entry
	write_tlb(index, &tlb);
}

void vmm_tlbm_handler(int cause, unsigned int *r_save)
{
	// Has the user installed a handler for this ?
	unhandled_exception("TLB Mod - Write to write protected page");
}

static int get_disk_block(int cause, int *r_save)
{
	int	i;

	for(i = 0; i < disk_map_entries; i++)
	{
		if(disk_map[i] < 0)
		{
			disk_map[i] = 0;
			return(((i * VP_SIZE) / 512) + disk_sector_offset);
		}
	}

	unhandled_exception("TLB Refill - No disk blocks available");
}

static void unmark_disk_block(int block)
{
	block &= ~VP_STATE_MASK;
	block -= disk_sector_offset;
	block /= (VP_SIZE/512);
	disk_map[block] = -1;
}

static int get_tlb_index(tlb_entry_t *tlb, tlb_entry_t *old_tlb, int *index, int cause, int *r_save)
{
	int				i;
	unsigned int	vpn;
	unsigned int	pfn;
	unsigned int	vaddr;
	unsigned int	state;
	unsigned int	block;
	int				status;
	int				retry;
	tlb_entry_t		*cur_tlb;
	tlb_entry_t		tlb_table[MAX_TLB_ENTRIES];
	void				*ptrs[2];

	//
	// Get all of the TLB entries
	//
	cur_tlb = tlb_table;
	for(i = 0; i < MAX_TLB_ENTRIES; i++)
	{
		get_tlb_entry(i, cur_tlb);
		cur_tlb++;
	}

	//
	// First check for any existing VALID entries using the same page frame.
	// If one is found, it is the entry that will be replaced.  It MUST be
	// swapped out to disk.
	//
	cur_tlb = tlb_table;
	for(i = 0; i < MAX_TLB_ENTRIES; i++)
	{
		// Is entry valid and does it's page frame number match the page
		// frame number of the virtual address we want to map ?
		if(cur_tlb->entry_lo0.v && cur_tlb->entry_lo0.pfn == tlb->entry_lo0.pfn)
		{
			// YES

			// Copy the old entry information so the caller can use it
			memcpy(old_tlb, cur_tlb, sizeof(tlb_entry_t));

			// Tell the caller which entry number should be used
			*index = i;

			// Tell the caller to perform the swap
			return(DO_SWAP);
		}

		// Check next entry
		cur_tlb++;
	}

	//
	// If the above search does NOT find an entry to use it means that there
	// are no valid entries in the TLB that are using the same page frame.
	// Now, we check to see if there are any entries that are NOT used.  If
	// one is found, it can be used and no swap is needed.
	//
	cur_tlb = tlb_table;
	for(i = 0; i < MAX_TLB_ENTRIES; i++)
	{
		// Is this entry valid ?
		if(!cur_tlb->entry_lo0.v)
		{
			// NO

			// Copy the entry information so the caller can use it
			memcpy(old_tlb, cur_tlb, sizeof(tlb_entry_t));

			// Tell the caller which entry number should be used
			*index = i;

			// Tell caller no swap is needed
			return(NO_SWAP);
		}

		// Check next entry
		cur_tlb++;
	}

	//
	// If neither of the above find a TLB to use then we must check the used
	// physical memory map to find out if the pfn for the new entry is being
	// used by a TLB entry that got replaced but NOT swapped out.
	//
	for(vpn = 0; vpn < mem_map_entries; vpn++)
	{
		// Get the state
		state = (mem_map[vpn] >> VP_STATE_SHIFT) & 3;

		// Is the entry used ?
		if(state != VP_NOT_USED)
		{
			// YES - Is the page in memory ?
			if(state == VP_IN_MEMORY)
			{
				// YES - Get the page frame number
				pfn = mem_map[vpn] & ~VP_STATE_MASK;

				// Does it match the page frame number we are now accessing ?
				if(pfn == tlb->entry_lo0.pfn)
				{
					// Do the virtual page numbers match ?
					if(vpn == tlb->entry_hi.vpn2)
					{
						//
						// In this case the virtual page numbers match and the
						// data in memory for the matching virtual page number has
						// NOT been swapped out.  In this case we can simply pick
						// a random TLB entry to replace and NOT do any swapping
						// because we know we are NOT using an existing mapped
						// page frame number AND the data in memory associated with
						// the virtual page number is still valid
						//

						// Get a random entry number to use
						i = get_random();

						// Get the data for the entry
						get_tlb_entry(i, old_tlb);

						if(((mem_map[old_tlb->entry_hi.vpn2] >> VP_STATE_SHIFT) & 3) == VP_IN_USE)
						{
							mem_map[old_tlb->entry_hi.vpn2] &= ~(VP_STATE_MASK);
							mem_map[old_tlb->entry_hi.vpn2] |= (VP_IN_MEMORY << VP_STATE_SHIFT);
						}

						// Tell caller which entry number is being used
						*index = i;

						// No swap is needed because we are accessing the same
						// virtual address that the data in memory is associated
						// with.
						return(NO_SWAP);
					}

					// Virtual page number do NOT match
					else
					{
						//
						// In this case we are accessing a page frame number that
						// is in use by a different virtual page number.  The
						// physical memory associated with the old virtual page
						// number gets swapped out to the swap pack and the
						// mem_map entry for the old virtual page number is marked
						// as swapped out.
						//

						// Find a place on the swap pack to put the data
						block = get_disk_block(cause, r_save);

						// Calculate KSEG0 address of where the data is
						vaddr = ((mem_map[vpn] & ~VP_STATE_MASK) << PFN_SHIFT) | 0x80000000;

						// Save the callback and process pointers
						save_disk_pointers(ptrs);

						// Allow interrupts
						setup_debug_service();

						// Write the data to disk
						retry = 4;
						do
						{
							status = _SecWrites(block, (unsigned long *)vaddr, VP_SIZE / 512);
						} while(status && --retry);

						if(!retry)
						{
							unhandled_exception("TLB Refill - Disk write failure");
							return;
						}

						// Turn interrupts back off
						disable_interrupts();

						// Restore the callback and process pointers
						restore_disk_pointers(ptrs);

						// Mark the old virtual address as being swapped out
						mem_map[vpn] = block | (VP_ON_DISK << VP_STATE_SHIFT);

						// Get a random tlb entry index
						i = get_random();

						// Get the entry data
						get_tlb_entry(i, old_tlb);

						if(((mem_map[old_tlb->entry_hi.vpn2] >> VP_STATE_SHIFT) & 3) == VP_IN_USE)
						{
							mem_map[old_tlb->entry_hi.vpn2] &= ~(VP_STATE_MASK);
							mem_map[old_tlb->entry_hi.vpn2] |= (VP_IN_MEMORY << VP_STATE_SHIFT);
						}

						// Tell caller which entry number got picked
						*index = i;

						// Return no swap cause we already swapped out the data
						return(NO_SWAP);
					}
				}
			}
		}
	}

	//
	// Finally, if none of the above find an entry to use then we can just
	// pick a random entry number.  No swap is needed because we already know
	// there are no exiting valid entries using the same page frame number
	// and that there are no virtual page numbers using the same page frame
	// number that have NOT been swapped out.
	//

	// Get a random entry number to use
	i = get_random();

	// Get the data for the entry
	get_tlb_entry(i, old_tlb);

	// Mark the data associated with the entry as in memory but NOT swapped
	// When the refill handler detects a vpn with this bit set, it gets an
	// entry to use.
	mem_map[old_tlb->entry_hi.vpn2] &= ~VP_STATE_MASK;
	mem_map[old_tlb->entry_hi.vpn2] |= (VP_IN_MEMORY << VP_STATE_SHIFT);

	// Tell caller which entry we picked
	*index = i;

	// No swapping is needed
	return(NO_SWAP);
}

void vmm_tlb_exception_handler(int cause, unsigned int *r_save)
{
	unsigned int	bad_vaddr;
	unsigned int	vpn;
	unsigned int	pfn;
	int				swap;
	int				index;
	int				state;
	int				retry;
	int				status;
	unsigned int	paddr;
	int				block;
	tlb_entry_t		tlb;
	tlb_entry_t		old_tlb;
	void				*ptrs[2];

	// Get the virtual address that caused the exception
	bad_vaddr = r_save[CP0_BADVADDR];

	// Is the bad virtual address less than allowed ?
	if(bad_vaddr < VP_SIZE)
	{
		unhandled_exception("TLB Refill - Address too low");
		return;
	}

	// If the virtual address larger than the core pointer + 1 VP
	if(bad_vaddr > ((int)core_mem) + VP_SIZE)
	{
		unhandled_exception("TLB Refill - Access out of range");
		return;
	}

	// Get the virtual page number
	vpn = bad_vaddr >> VPN_SHIFT;

	// Is the virtual page number 0 ?
	if(!vpn)
	{
		unhandled_exception("TLB Refill - NULL virtual page number");
		return;
	}

	// Is the virtual page number in range ?
	if(vpn >= mem_map_entries)
	{
		unhandled_exception("TLB Refill - Virtual address too high");
		return;
	}

	// Get the page frame number
	pfn = virt2pfn(bad_vaddr);

	// Get the state of the virtual page
	state = mem_map[vpn] >> VP_STATE_SHIFT;
	state &= 3;

	// Has this virtual page ever been used OR is it in memory that has
	// NOT been swapped out ?
	if(state == VP_NOT_USED || state == VP_IN_MEMORY)
	{
		//
		// Virtual page number has never been used
		//

		// Set up the info for the new TLB entry
		tlb.entry_hi.asid = 0;
		tlb.entry_hi.vpn2 = vpn;

		tlb.entry_lo0.g = PF_GLOBAL;
		tlb.entry_lo0.v = 0;
		tlb.entry_lo0.d = PF_WRITEABLE;
		tlb.entry_lo0.c = PF_NONCOHERENT;
		tlb.entry_lo0.pfn = (pfn & ~1);
		tlb.entry_lo0.nu2 = 0;

		tlb.entry_lo1.g = PF_GLOBAL;
		tlb.entry_lo1.v = 0;
		tlb.entry_lo1.d = PF_WRITEABLE;
		tlb.entry_lo1.c = PF_NONCOHERENT;
		tlb.entry_lo1.pfn = (pfn & ~ 1) + 1;
		tlb.entry_lo1.nu2 = 0;

		if(pfn & 1)
		{
			tlb.entry_lo1.v = PF_VALID;
		}
		else
		{
			tlb.entry_lo0.v = PF_VALID;
		}

		swap = get_tlb_index(&tlb, &old_tlb, &index, cause, r_save);

		// If we are replacing and entry that has the same page frame number
		// as our new entry - then the data from the physical memory location
		// must be written to disk and the mem_map entry for the old entry
		// should be set to 0x80000000 | with the disk block number where it
		// was stored
		if(swap)
		{
			//
			// Match - swap page to disk
			//

			// Generate KSEG0 address of the data
			paddr = (old_tlb.entry_lo0.pfn << PFN_SHIFT)|0x80000000;

			// Get a disk block to use
			block = get_disk_block(cause, r_save);

			// Save the callback and process pointers
			save_disk_pointers(ptrs);

			// Allow interrupts
			setup_debug_service();

			// Write the data to the disk
			retry = 4;
			do
			{
				status = _SecWrites(block, (unsigned long *)paddr, VP_SIZE / 512);
			} while(status && --retry);

			if(!retry)
			{
				unhandled_exception("TLB Refill - Disk write failure");
				return;
			}

			// Turn interrupts back off
			disable_interrupts();

			// Restore the callback and process pointers
			restore_disk_pointers(ptrs);

			// Mark this usage
			mem_map[old_tlb.entry_hi.vpn2] = block | (VP_ON_DISK << VP_STATE_SHIFT);
		}

		// Write the new entry
		write_tlb(index, &tlb);

		// Mark this usage
		mem_map[tlb.entry_hi.vpn2] = tlb.entry_lo0.pfn|(VP_IN_USE << VP_STATE_SHIFT);
	}

	// Is virtual page in use ?
	else if(state == VP_IN_USE)
	{
		unhandled_exception("TLB Refill - VPN in use");
		return;
	}

	// Virtual page is on disk
	else
	{
		//
		// On disk - swap page being replaced to disk (if needed) and
		// swap page being mapped from the disk
		//

		// Set up the info for the new TLB entry
		tlb.entry_hi.asid = 0;
		tlb.entry_hi.vpn2 = vpn;

		tlb.entry_lo0.g = PF_GLOBAL;
		tlb.entry_lo0.v = 0;
		tlb.entry_lo0.d = PF_WRITEABLE;
		tlb.entry_lo0.c = PF_NONCOHERENT;
		tlb.entry_lo0.pfn = (pfn & ~1);
		tlb.entry_lo0.nu2 = 0;

		tlb.entry_lo1.g = PF_GLOBAL;
		tlb.entry_lo1.v = 0;
		tlb.entry_lo1.d = PF_WRITEABLE;
		tlb.entry_lo1.c = PF_NONCOHERENT;
		tlb.entry_lo1.pfn = (pfn & ~ 1) + 1;
		tlb.entry_lo1.nu2 = 0;

		if(pfn & 1)
		{
			tlb.entry_lo1.v = PF_VALID;
		}
		else
		{
			tlb.entry_lo0.v = PF_VALID;
		}

		swap = get_tlb_index(&tlb, &old_tlb, &index, cause, r_save);

		// If we are replacing and entry that has the same page frame number
		// as our new entry - then the data from the physical memory location
		// must be written to disk and the mem_map entry for the old entry
		// should be set to 0x80000000 | with the disk block number where it
		// was stored
		if(swap)
		{
			//
			// Match - swap page to disk
			//

			// Generate KSEG0 address of the data
			paddr = (old_tlb.entry_lo0.pfn << PFN_SHIFT)|0x80000000;

			// Get a disk block to use
			block = get_disk_block(cause, r_save);

			// Save the callback and process pointers
			save_disk_pointers(ptrs);

			// Allow interrupts
			setup_debug_service();

			// Write the data to the disk
			retry = 4;
			do
			{
				status = _SecWrites(block, (unsigned long *)paddr, VP_SIZE / 512);
			} while(status && --retry);

			if(!retry)
			{
				unhandled_exception("TLB Refill - Disk write failure");
				return;
			}

			// Turn interrupts back off
			disable_interrupts();

			// Restore the callback and process pointers
			restore_disk_pointers(ptrs);

			// Mark this usage
			mem_map[old_tlb.entry_hi.vpn2] = block | (VP_ON_DISK << VP_STATE_SHIFT);
		}

		// Write the new entry
		write_tlb(index, &tlb);

		//
		// Swap page from disk
		//
		// Generate KSEG0 address of where to put the data for this page
		paddr = (tlb.entry_lo0.pfn << PFN_SHIFT)|0x80000000;

		// Save the callback and process pointers
		save_disk_pointers(ptrs);

		// Allow interrupts
		setup_debug_service();

		// Get the data from the disk
		retry = 4;
		do
		{
			status = _SecReads(mem_map[vpn] & ~VP_STATE_MASK, (unsigned long *)paddr, VP_SIZE / 512);
		} while(status && --retry);

		if(!retry)
		{
			unhandled_exception("TLB Refill - Disk read failure");
			return;
		}

		// Turn interrupts back off
		disable_interrupts();

		// Restore the callback and process pointers

		// Mark disk block no longer used
		unmark_disk_block(mem_map[vpn]);

		// Mark this usage
		mem_map[tlb.entry_hi.vpn2] = tlb.entry_lo0.pfn|(VP_IN_USE << VP_STATE_SHIFT);
	}
}

__asm__("
	.set	noreorder
	.globl	write_tlb
write_tlb:
	mtc0	$4,$0
	lw		$8,0($5)
	lw		$9,4($5)
	lw		$10,8($5)
	mtc0	$8,$10
	mtc0	$9,$2
	mtc0	$10,$3
	jr		$31
	tlbwi

	.globl	get_tlb_entry
get_tlb_entry:
	mtc0	$4,$0
	nop
	nop
	tlbr
	nop
	nop
	mfc0	$8,$10
	mfc0	$9,$2
	mfc0	$10,$3
	sw		$8,0($5)
	sw		$9,4($5)
	jr		$31
	sw		$10,8($5)

	.globl	tlb_probe
tlb_probe:
	lw		$8,0($4)
	mtc0	$8,$10
	nop
	tlbp
	jr		$31
	mfc0	$2,$0

	.globl	get_random
get_random:
	mfc0	$2,$1
	nop
	jr		$31
	nop

	.globl	unlock_tlb
unlock_tlb:
	sll	$4,13
	mtc0	$4,$5
	jr		$31
	mtc0	$0,$6
	.set	reorder
");

void sbrk0(void *membase)
{
	int					i;
	tlb_entry_t			tlb;
	partition_table_t	*pt;
	int					swap_partition;

	// Unlock the TLB's
	unlock_tlb(VMM_PAGE_MASK);

	// Initialize the 48 TLB entries
	tlb.entry_hi.asid = 0;
	tlb.entry_hi.vpn2 = -1;

	tlb.entry_lo0.g = PF_GLOBAL;
	tlb.entry_lo0.v = 0;
	tlb.entry_lo0.d = 0;
	tlb.entry_lo0.c = PF_NONCOHERENT;
#if PAGE_MASK_BITS
	tlb.entry_lo0.nu1 = 0;
#endif
	tlb.entry_lo0.pfn = 0;
	tlb.entry_lo0.nu2 = 0;

	tlb.entry_lo1.g = PF_GLOBAL;
	tlb.entry_lo1.v = 0;
	tlb.entry_lo1.d = 0;
	tlb.entry_lo1.c = PF_NONCOHERENT;
#if PAGE_MASK_BITS
	tlb.entry_lo1.nu1 = 0;
#endif
	tlb.entry_lo1.pfn = 0;
	tlb.entry_lo1.nu2 = 0;

	// Set the 48 TLB Entries using the first 48 control structures
	for(i = 0; i < MAX_TLB_ENTRIES; i++)
	{
		write_tlb(i, &tlb);
	}

	// Initialize the number of disk map entries
	disk_map_entries = 0;

	// Initialize the disk if it has not already been initialized
	if(ide_init())
	{
		// Get the disk partition table
		pt = ide_get_partition_table();

		// Is the partition table pointer valid ?
		if(pt)
		{
retry:
			// YES - Search for a swap partition
			for(i = 0; i < pt->num_partitions; i++)
			{
				// Is this a swap partition ?
				if(pt->partition[i].partition_type == SWAP_PARTITION)
				{
					// YES - Figure out how many disk map entries are needed
					disk_map_entries = ((pt->partition[i].num_blocks * 512) / VP_SIZE) + 1;

					// Set the disk sector offset
					disk_sector_offset = pt->partition[i].starting_block;

					// Set the swap partition number
					swap_partition = i;

					// DONE
					break;
				}
			}
		}
	}

#ifdef VMM_DEBUG
	disk_map_entries = ((1 << 30) / VP_SIZE) + 1;
//	disk_map_entries = ((1 << 23) / VP_SIZE) + 1;
	disk_sector_offset = 10000;
#endif

	// Set the number of memory map entries based on the amount of memory
	// in the system
	mem_map_entries = (__memory_size / VP_SIZE) + (disk_map_entries ? disk_map_entries : 1);

	// Set the pointer to the memory map array
	mem_map = membase;

	// Set the pointer to the disk map array
	disk_map = (mem_map + mem_map_entries);

	// Set the pointer to start of physical memory
	phys_mem_base = (int)(disk_map + disk_map_entries) & 0x1fffffff;

	// Make sure the pointer is aligned to a virtual page size
	while(phys_mem_base & (VP_SIZE - 1))
	{
		phys_mem_base++;
	}

	// Is the physical memory base equal to or larger than 2 virtual
	// pages less than the top of physical memory
	if(phys_mem_base >= (__memory_size - (VP_SIZE * 2)))
	{
		// YES - This means the swap pack on the disk has so much space
		// that we don't have enough memory for the memory and disk maps.
		// Therefore - we cut the swap pace size in half and try again.
		pt->partition[swap_partition].num_blocks >>= 1;

		// Reset disk map entries
		disk_map_entries = 0;

		// Go recalculate disk map stuff over again
		goto retry;
	}

	// Set the size of the physical memory region
	if(VP_SIZE < 32768)
	{
		phys_mem_size = (__memory_size - phys_mem_base) - (VP_SIZE * 4);
	}
	else
	{
		phys_mem_size = (__memory_size - phys_mem_base) - VP_SIZE;
	}

	// Reset the memory map entries to the actual amount needed
	mem_map_entries = (phys_mem_size / VP_SIZE) + (disk_map_entries ? disk_map_entries : 1);

	// Initialize the memory maps
	for(i = 0; i < mem_map_entries; i++)
	{
		mem_map[i] = (VP_NOT_USED << VP_STATE_SHIFT);
	}

	// Initialize the disk maps
	for(i = 0; i < disk_map_entries; i++)
	{
		disk_map[i] = -1;
	}

	// Set the starting virtual pointer
	core_mem = (void *)VP_SIZE;
}

void *_sbrk(unsigned int size)
{
	void	*alloc;

	// Save the virtual address to give to caller
	alloc = core_mem;

	// Adjust the size to even multiple of page frame size
	size += (PF_SIZE - 1);
	size &= ~(PF_SIZE - 1);

	if(((int)core_mem + size) <= (VP_SIZE + getmemsize()))
	{
		core_mem += size;
	}
	else
	{
		alloc = (void *)0;
	}

	// Return the pointer
	return(alloc);
}

void vmm_init(void)
{
	// Initialize the virtual memory managment system
	sbrk0((void *)0x80200000);
}

static unsigned int virt2pfn(unsigned int virt_addr)
{
	// Mask off unused bits
	virt_addr &= ~PF_MASK;

	// Subtract off the base of where we start
	virt_addr -= VP_SIZE;

	// Limit to 6 Meg
	virt_addr %= phys_mem_size;

	// Shift up to base address
	virt_addr += phys_mem_base;

	// Shift to PFN area
	virt_addr >>= PFN_SHIFT;

	return(virt_addr);
}

static unsigned int virt2page(unsigned int virt_addr)
{
	// Subtract off the base of where we start
	virt_addr -= VP_SIZE;

	// Limit to 6 Meg
	virt_addr %= phys_mem_size;

	// Convert to page number
	virt_addr /= VP_SIZE;

	// Return page number
	return(virt_addr);
}

static void save_disk_pointers(void **ptrs)
{
	ptrs[0] = callback;
	ptrs[1] = proc;

	callback = (void *)0;
	proc = (void *)0;
}

static void restore_disk_pointers(void **ptrs)
{
	callback = ptrs[0];
	proc = ptrs[1];
}

int getpagesize(void)
{
	return(PF_SIZE);
}

int getmemsize(void)
{
	return((mem_map_entries - 1) * VP_SIZE);
}

int getcoreavail(void)
{
}

#ifdef VMM_DEBUG
void show_mem_map(void)
{
	int	i;

	for(i = 0; i < mem_map_entries; i++)
	{
		dphex(i);
		dputs(" ");
		dphex(mem_map[i]);
		dputs("\n");
	}
}
#endif
#endif

