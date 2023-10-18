//
// dskcache.c
//
// $Author: Mlynch $
//
// $Revision: 8 $
//
// This is the disk caching code.  This is a write through cache so there is
// no possible way for data to be written to the disk and the disk cache to
// be out of sync with the cached sectors.
//
#include	<ide.h>

#define	SECTORS_PER_BLOCK			256
#define	NUM_DISK_CACHE_BLOCKS	4

short _SecReads(unsigned long sec, unsigned long *buf, unsigned long num);
short _SecWrites(unsigned long sec, unsigned long *buf, unsigned long num);
void	puts(const char *);


typedef struct disk_cache_control
{
	int	usage_count;
	int	starting_block;
	char	*data_ptr;
} disk_cache_control_t;

static disk_cache_control_t	dcc[NUM_DISK_CACHE_BLOCKS];
static char							disk_cache[(SECTORS_PER_BLOCK*NUM_DISK_CACHE_BLOCKS*512) + 32];
static int							disk_cache_initialized = 0;

static void wcache_memcpy(unsigned int *to, unsigned int *from, int length)
{
	length >>= 5;
	while(length--)
	{
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;

		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
		*to++ = *from++;
	}
}

__asm__("
	.set	noreorder
	.globl	dcache_memcpy
dcache_memcpy:
	srl		$6,6
dmem_loop:
	beq		$6,$0,dmem_done
	subi		$6,1

	ld			$8,0($5)
	ld			$9,8($5)
	ld			$10,16($5)
	ld			$11,24($5)
	ld			$12,32($5)
	ld			$13,40($5)
	ld			$14,48($5)
	ld			$15,56($5)

	sd			$8,0($4)
	sd			$9,8($4)
	sd			$10,16($4)
	sd			$11,24($4)
	sd			$12,32($4)
	sd			$13,40($4)
	sd			$14,48($4)
	sd			$15,56($4)

	ld			$8,64($5)
	ld			$9,72($5)
	ld			$10,80($5)
	ld			$11,88($5)
	ld			$12,96($5)
	ld			$13,104($5)
	ld			$14,112($5)
	ld			$15,120($5)

	sd			$8,64($4)
	sd			$9,72($4)
	sd			$10,80($4)
	sd			$11,88($4)
	sd			$12,96($4)
	sd			$13,104($4)
	sd			$14,112($4)
	sd			$15,120($4)

	addiu		$4,128
	addiu		$5,128

	ld			$8,0($5)
	ld			$9,8($5)
	ld			$10,16($5)
	ld			$11,24($5)
	ld			$12,32($5)
	ld			$13,40($5)
	ld			$14,48($5)
	ld			$15,56($5)

	sd			$8,0($4)
	sd			$9,8($4)
	sd			$10,16($4)
	sd			$11,24($4)
	sd			$12,32($4)
	sd			$13,40($4)
	sd			$14,48($4)
	sd			$15,56($4)

	ld			$8,64($5)
	ld			$9,72($5)
	ld			$10,80($5)
	ld			$11,88($5)
	ld			$12,96($5)
	ld			$13,104($5)
	ld			$14,112($5)
	ld			$15,120($5)

	sd			$8,64($4)
	sd			$9,72($4)
	sd			$10,80($4)
	sd			$11,88($4)
	sd			$12,96($4)
	sd			$13,104($4)
	sd			$14,112($4)
	sd			$15,120($4)

	addiu		$4,128
	b			dmem_loop
	addiu		$5,128
dmem_done:
	jr			$31
	nop
");


static void cache_memcpy(unsigned int *to, unsigned int *from, int length)
{
	if(!((int)to & 4) && !((int)from & 4))
	{
		dcache_memcpy(to, from, length);
	}
	else
	{
		wcache_memcpy(to, from, length);
	}
}

void reinitialize_disk_cache(void)
{
	int	i;
	char	*buf_ptr;

	// Make sure the cache buffers are cache page aligned
	buf_ptr = disk_cache;
	while((int)buf_ptr & 0x1f)
	{
		++buf_ptr;
	}
	for(i = 0; i < NUM_DISK_CACHE_BLOCKS; i++)
	{
		dcc[i].usage_count = 0;
		dcc[i].starting_block = -1;
		dcc[i].data_ptr = buf_ptr;
		buf_ptr += (SECTORS_PER_BLOCK * 512);
	}
}

void init_disk_cache(void)
{
	int	i;

	if(!disk_cache_initialized)
	{
		reinitialize_disk_cache();
		disk_cache_initialized = 1;
	}
}

static disk_cache_control_t *get_block(int sector)
{
	register int						i;
	register int						lru = 0x7fffffff;
	register disk_cache_control_t	*dcc_ptr = dcc;
	register disk_cache_control_t	*dcc_lru = dcc;

	// Convert sector to block number
	sector >>= 8;

	// Set lru pointer
	dcc_lru = (disk_cache_control_t *)0;

	// Loop through control structure to see if block is already loaded
	for(i = 0; i < NUM_DISK_CACHE_BLOCKS; i++)
	{
		// Is the requested block in cache ?
		if(dcc_ptr->starting_block == sector)
		{
			// YES - found it
			// Increment the hit block usage count
			dcc_ptr->usage_count++;

			// Save the pointer
			dcc_lru = dcc_ptr;
		}
		else
		{
			// Decrement non-hit block usage count
			dcc_ptr->usage_count--;
		}

		// Next cache control block
		dcc_ptr++;
	}

	// Did we find the sector in cache ?
	if(dcc_lru)
	{
		// YES - return pointer to cache control block
		return(dcc_lru);
	}

	//
	// Requested sector is NOT in disk cache
	//

	// Set lru pointer
	dcc_lru = dcc;

	// Set the control block pointer
	dcc_ptr = dcc;

	// Search for least recently used cache block
	for(i = 0; i < NUM_DISK_CACHE_BLOCKS; i++)
	{
		// Is this blocks usage count less than least found so far ?
		if(dcc_ptr->usage_count < lru)
		{
			// YES - Set least found so far
			lru = dcc_ptr->usage_count;

			// Set lru pointer
			dcc_lru = dcc_ptr;
		}

		// Next cache control block
		dcc_ptr++;
	}

	// Increment the usage count for the cache control block
	dcc_lru->usage_count++;

	// Set the cache control block block number
	dcc_lru->starting_block = sector;

	// Read the data from the disk
	if(_SecReads(sector << 8, (unsigned long *)dcc_lru->data_ptr, SECTORS_PER_BLOCK) < 0)
	{
		dcc_lru->starting_block = -1;
		dcc_lru->usage_count -= 2;
		return((disk_cache_control_t *)0);
	}

	// Return the block
	return(dcc_lru);
}

short disk_cache_read(int sec, unsigned long *buf, int num)
{
	register int						sectors_to_transfer;
	register disk_cache_control_t	*dccp;
	register int						sector_offset;

	// Read all sectors requested
	while(num)
	{
		// Get a pointer to disk cache block
		dccp = get_block(sec);

		// Was there an error ?
		if(!dccp)
		{
			// YES - return fail
			return(-1);
		}

		// Mask off the high bits of the sector number
		sector_offset = sec & (SECTORS_PER_BLOCK - 1);

		// calculate the number of sectors available to transfer
		sectors_to_transfer = SECTORS_PER_BLOCK - sector_offset;

		// Transfer only as many as needed
		if(sectors_to_transfer > num)
		{
			sectors_to_transfer = num;
		}

		// Decrement number left to transfer
		num -= sectors_to_transfer;

		// Increment logical sector number
		sec += sectors_to_transfer;

		// Convert sectors to words
		sectors_to_transfer <<= 7;

		// Read the data from the cache
		cache_memcpy((unsigned int *)buf, (unsigned int *)(dccp->data_ptr + (sector_offset << 9)), sectors_to_transfer);

		// Increment the buffer pointer (words)
		buf += sectors_to_transfer;
	}

	return(1);
}

short disk_cache_write(int sec, unsigned long *buf, int num)
{
	register int						i;
	register int						sectors_to_transfer;
	register disk_cache_control_t	*dcc_ptr;
	register disk_cache_control_t	*dccp1;
	register int						block;
	register int						sector_offset;

	while(num)
	{
		// Figure out if the block is in cache
		// Mask off the low bits of the sector number
		block = sec >> 8;

		// Mask off the high bits of the sector number
		sector_offset = sec & (SECTORS_PER_BLOCK - 1);

		// First check to see if the sector is in the cache
		dcc_ptr = dcc;
		dccp1 = (disk_cache_control_t *)0;
		for(i = 0; i < NUM_DISK_CACHE_BLOCKS; i++)
		{
			// Is the requested sector in the cache ?
			if(dcc_ptr->starting_block == block)
			{
				// YES - found it
				// Save the pointer
				dccp1 = dcc_ptr;

				// Done
				break;
			}

			// Next disk cache control block
			dcc_ptr++;
		}

		// Calculate number of sectors to transfer
		sectors_to_transfer = SECTORS_PER_BLOCK - sector_offset;

		// Limit to number
		if(sectors_to_transfer > num)
		{
			sectors_to_transfer = num;
		}

		// Write the data to the disk
		if(_SecWrites(sec, buf, sectors_to_transfer) < 0)
		{
			// ERROR
			return(-1);
		}

		// Decrement number left
		num -= sectors_to_transfer;

		// Increment sector
		sec += sectors_to_transfer;

		// Convert sectors to words
		sectors_to_transfer <<= 7;

		// Sector(s) in cache - write data to cache block
		if(dccp1)
		{
			// Write the data to the cache
			cache_memcpy((unsigned int *)(dccp1->data_ptr + (sector_offset << 9)), (unsigned int *)buf, sectors_to_transfer);
		}

		// Increment the buffer pointer (words)
		buf += sectors_to_transfer;
	}

	return(1);
}

void flush_disk_cache(void)
{
	reinitialize_disk_cache();
}
