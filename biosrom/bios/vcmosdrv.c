//
// Copyright (c) 1998 by Midway Video Inc.
//
// $Revision: 1 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>
#include	<ioctl.h>

/* Assorted definitions */
#define	CMOS_UNLOCK_BIT					0x8000
#define	MAX_GENERIC_RECORD_TABLES		16

/* Structure definitions */
typedef struct generic_cmos_record_info
{
	int	entry_size;
	int	num_entries;
} generic_cmos_record_info_t;

typedef struct cmos_config_info
{
	int	cmos_size;
	int	max_audits;
	int	max_adjustments;
	int	num_gcr_tables;
	int	user_data_size;
	generic_cmos_record_info_t	gcr[MAX_GENERIC_RECORD_TABLES];
} cmos_config_info_t;

/* CMOS function return codes */
#define	CMOS_OK											0
#define	CMOS_NOT_CONFIGURED							-1
#define	CMOS_TOO_MANY_HS_TABLES						-2
#define	CMOS_CONFIG_TOO_LARGE						-3
#define	CMOS_INVALID_OFFSET							-4
#define	CMOS_INVALID_IOCTL							-5
#define	CMOS_AUDIT_ERROR								-6
#define	CMOS_INVALID_AUDIT_NUMBER					-7
#define	CMOS_ADJUSTMENTS_INVALID					-8
#define	CMOS_INVALID_ADJUSTMENT_NUMBER			-9
#define	CMOS_INVALID_GENERIC_TABLE_NUMBER		-10
#define	CMOS_INVALID_GENERIC_ENTRY_NUMBER		-11
#define	CMOS_GENERIC_TABLE_ERROR					-12
#define	CMOS_CONFIG_BAD_ADDR							-13
#define	CMOS_CONFIG_BAD_SIZE							-14

unsigned long crc(unsigned char *, int);
void init_cmos_dump_record(void);

#define	CMOS_DEAD	1
#define	CMOS_ALIVE	2

static int						cmos_configured = 0;
static cmos_config_info_t	cmos_info;
static unsigned int	 		*audit_base_addr;
static unsigned int 			*adjustment_base_addr;
static unsigned char			*generic_record_base_addr[MAX_GENERIC_RECORD_TABLES];
static unsigned char			*user_base_addr;
static void						*cmos_end_addr;
static int						cmos_update = 0;
static unsigned char			cmos_shadow[CMOS_SIZE];
static int						cmos_status = CMOS_ALIVE;
static int						cmos_initialized = 0;
static unsigned char			*cmos_ram = (unsigned char *)CMOS_RAM_ADDR;


#define	CMOS_RESERVE		1024

//
// save_cmos() - Save the CMOS into the shadow RAM
//
static void save_cmos(void)
{
	register int	i;

	for(i = CMOS_RESERVE; i < CMOS_SIZE; i++)
	{
		cmos_shadow[i] = cmos_ram[i];
	}
}

//
// test_cmos() - Test the CMOS ram.  Return non-zero on fail
//
static int test_cmos(void)
{
	unsigned char	data = 1;
	unsigned int	i;

	//
	// Write incrementing pattern to CMOS memory
	//
	for(i = CMOS_RESERVE; i < CMOS_SIZE; i++)
	{
		//
		// Unlock CMOS memory
		//
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;

		//
		// Write data to CMOS
		//
		cmos_ram[i] = data;

		//
		// Increment data
		//
		++data;

		//
		// Is data 0 ?
		//
		if(!data)
		{
			//
			// YES - Reset data to 1
			//
			data = 1;
		}
	}

	//
	// Reset data to 1
	//
	data = 1;

	//
	// Read and check data in CMOS memory
	//
	for(i = CMOS_RESERVE; i < CMOS_SIZE; i++)
	{
		//
		// Does data match ?
		//
		if(cmos_ram[i] != data)
		{
			//
			// NOPE - Return ERROR
			//
			return(1);
		}

		//
		// Increment data
		//
		++data;

		//
		// Is data 0 ?
		//
		if(!data)
		{
			//
			// YES - Reset data to 1
			//
			data = 1;
		}
	}

	//
	// Return OK
	//
	return(0);
}


//
// update_cmos() - This function is used to update the CMOS Ram with the
// data from the CMOS shadow ram.
//
void update_cmos(void)
{
	register int	i;

	//
	// Is CMOS dead ?
	//
	if(cmos_status == CMOS_DEAD)
	{
		//
		// YES - Don't bother
		//
		return;
	}

	//
	// Are we suppose to update the CMOS
	//
	if(!cmos_update)
	{
		//
		// NOPE - Don't bother
		//
		return;
	}

	//
	// Turn off the CMOS update flag
	//
	cmos_update = 0;

	//
	// Update all bytes of CMOS that are different than the shadow RAM
	//
	for(i = CMOS_RESERVE; i < CMOS_SIZE; i++)
	{
		//
		// Does this location match the shadow ram ?
		//
		if(cmos_ram[i] != cmos_shadow[i])
		{
			//
			// NOPE - Unlock it for write
			//
			*((volatile char *)CMOS_UNLOCK_ADDR) = 0;

			//
			// Update the CMOS Ram from the shadow RAM
			//
			cmos_ram[i] = cmos_shadow[i];
		}
	}
}



//
// cmosinit() - CMOS driver initialization function
//
int cmosinit(void)
{
	register unsigned char	sec;

	//
	// Has CMOS already been initialized ?
	//
	if(!cmos_initialized)
	{
		//
		// NOPE - Mark CMOS for no updates
		//
		cmos_update = 0;

		//
		// Unlock the device write lock
		//
		*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

		//
		// Set the READ bit in the control register
		//
		*((unsigned char *)RTC_CONTROL_REG) = RTC_R_BIT;

		//
		// Get the current value of the seconds register
		//
		sec = *((unsigned char *)RTC_SECONDS_REG);

		//
		// Is the stop bit set
		//
		if(sec & RTC_ST_BIT)
		{
			//
			// YES - turn off the stop bit
			//
			sec &= ~RTC_ST_BIT;

			//
			// Unlock the device write lock
			//
			*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

			//
			// Turn on the Write bit
			//
			*((unsigned char *)RTC_CONTROL_REG) = RTC_W_BIT;

			//
			// Unlock the device write lock
			//
			*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

			//
			// Write back the seconds register
			//
			*((unsigned char *)RTC_SECONDS_REG) = sec;
		}

		//
		// Unlock the device write lock
		//
		*((unsigned char *)CMOS_UNLOCK_ADDR) = 0;

		//
		// Reset the control register
		//
		*((unsigned char *)RTC_CONTROL_REG) = 0;

		//
		// Save off the current CMOS memory
		//
		save_cmos();

		//
		// Test the CMOS memory - failed ?
		//
		if(test_cmos())
		{
			//
			// YES - Mark CMOS dead
			//
			cmos_status = CMOS_DEAD;

			//
			// Return fail
			//
			return(1);
		}

		//
		// Mark the CMOS for update
		//
		cmos_update = 1;

		//
		// Update the CMOS memory
		//
		update_cmos();

		//
		// Mark as initialized
		//
		cmos_initialized = 1;
	}

	//
	// Return OK
	//
	return(0);
}


//
// copy_cmos_info() - This function is used to copy the CMOS configuration
// information from the application.
//
static void copy_cmos_info(struct cmos_config_info *ci)
{
	register int	i;
	register char	*from, *to;

	from = (char *)ci;
	to = (char *)&cmos_info;
	for(i = 0; i < sizeof(struct cmos_config_info); i++)
	{
		*to++ = *from++;
	}
}

//
// config_cmos() - This function is used to configure the number of audits,
// number of adjustments, number of high score tables, number of entries in
// each high score table, size (in bytes) of each entry in each high score
// table, and the size of the user defined area of CMOS memory.
//
int config_cmos(struct cmos_config_info *cci)
{
	int	i;
	int	hs_table_size;

	//
	// Copy the info sent by the user
	//
	copy_cmos_info(cci);

	//
	// Attempting to create too many tables ?
	//
	if(cmos_info.num_gcr_tables > MAX_GENERIC_RECORD_TABLES)
	{
		//
		// YES - Return Error
		//
		cmos_configured = 0;
		return(CMOS_TOO_MANY_HS_TABLES);
	}

	//
	// Set the size of the CMOS memory
	//
	cci->cmos_size = CMOS_SIZE - CMOS_RESERVE;

	//
	// Calculate the ending address of the CMOS
	//
	cmos_end_addr = (void *)&cmos_shadow[CMOS_SIZE-1];

	//
	// Calculate the starting address of the AUDITS
	//
	audit_base_addr = (unsigned int *)&cmos_shadow[CMOS_RESERVE];

	//
	// Calculate the starting address of the ADJUSTMENTS
	//
	adjustment_base_addr = (unsigned int *)(audit_base_addr + (cci->max_audits * 2));

	//
	// To many AUDITS ?
	//
	if((unsigned int)adjustment_base_addr >= (unsigned int)cmos_end_addr)
	{
		//
		// YES - Return ERROR
		//
		cmos_configured = 0;
		return(CMOS_CONFIG_TOO_LARGE);
	}

	//
	// Calculate starting address of first user defined record area
	//
	generic_record_base_addr[0] = (unsigned char *)(adjustment_base_addr + cmos_info.max_adjustments + 1);

	//
	// Too many ADJUSTMENTS ?
	//
	if((unsigned int)generic_record_base_addr[0] >= (unsigned int)cmos_end_addr)
	{
		//
		// YES - Return ERROR
		//
		cmos_configured = 0;
		return(CMOS_CONFIG_TOO_LARGE);
	}

	//
	// Set up all of the user defined record areas
	//
	for(i = 1; i < cmos_info.num_gcr_tables && i < MAX_GENERIC_RECORD_TABLES; i++)
	{
		//
		// Calculate base address of the user defined record area
		//
		hs_table_size = cmos_info.gcr[i-1].entry_size + sizeof(int);
		hs_table_size *= cmos_info.gcr[i-1].num_entries;
		generic_record_base_addr[i] = (generic_record_base_addr[i-1] + hs_table_size);

		//
		// Last table extend beyond end of CMOS ?
		//
		if((unsigned int)generic_record_base_addr[i] >= (unsigned int)cmos_end_addr)
		{
			//
			// YES - return ERROR
			//
			cmos_configured = 0;
			return(CMOS_CONFIG_TOO_LARGE);
		}
	}

	//
	// Calculate address of user area in CMOS
	//
	hs_table_size = cmos_info.gcr[i-1].entry_size + sizeof(int);
	hs_table_size *= cmos_info.gcr[i-1].num_entries;
	user_base_addr = generic_record_base_addr[i-1] + hs_table_size;

	//
	// User base >= end of CMOS memory ?
	//
	if((unsigned int)user_base_addr >= (unsigned int)cmos_end_addr)
	{
		//
		// YES - Return ERROR
		//
		cmos_configured = 0;
		return(CMOS_CONFIG_TOO_LARGE);
	}

	//
	// Set configured flag
	//
	cmos_configured = 1;

	//
	// Return OK
	//
	return(CMOS_OK);
}

//
// get_cmos_user_address() - This function returns the base address of the
// user area of CMOS memory.
//
void *get_cmos_user_address(void)
{
	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// Nope - return ERROR
		//
		return((int *)0);
	}

	//
	// Return Base Address of user area of CMOS
	//
	return((void *)user_base_addr);
}

//
// get_cmos_user_size() - This function returns the size of the user area
// in the CMOS memory.
//
int get_cmos_user_size(void)
{
	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(0);
	}

	//
	// Return size (bytes) of user area of CMOS
	//
	return(((unsigned int)cmos_end_addr - (unsigned int)user_base_addr));
}


//
// _check_audit() - This function is used to check the itegrity of the audit
// specified by the address passed to it.
//
static int _check_audit(unsigned int *aud_addr)
{
	register unsigned int	calc_crc;

	calc_crc = crc((unsigned char *)aud_addr, sizeof(unsigned int));
	if(*(aud_addr+1) != calc_crc)
	{
		return(CMOS_AUDIT_ERROR);
	}
	return(CMOS_OK);
}

//
// check_audit() - This function is used to check the itegrity of an
// audit using its audit number.
//
int check_audit(int aud_num)
{
	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is audit number in range ?
	//
	if(aud_num < 0 || aud_num >= cmos_info.max_audits)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_AUDIT_NUMBER);
	}

	//
	// Return status of the requested audit number
	//
	return(_check_audit(audit_base_addr + (aud_num<<1)));
}

//
// check_audits() - This function is used to check all of the audits in the
// CMOS.
//
int check_audits(void)
{
	register int	i;
	register int	status;

	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Check status of all audits
	//
	for(i = 0; i < cmos_info.max_audits; i++)
	{
		status = check_audit(i);
		if(status != CMOS_OK)
		{
			return(status);
		}
	}

	//
	// All audits OK - return OK
	//
	return(CMOS_OK);
}

//
// clear_audits() - This function is used to clear all of the audits to
// 0.
//
int clear_audits(void)
{
	register int	i;

	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Clear all of the audits
	//
	for(i = 0; i < cmos_info.max_audits; i++)
	{
		set_audit(i, 0);
	}

	//
	// Check status of all audits and return
	//
	return(check_audits());
}

//
// get_audit() - This function is used to retrieve the value of an audit
//
int get_audit(int aud_num, int *to)
{
	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is the audit number in range ?
	//
	if(aud_num < 0 || aud_num >= cmos_info.max_audits)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_AUDIT_NUMBER);
	}

	//
	// Get value of audit
	//
	*to = (int)*(audit_base_addr + (aud_num << 1));

	//
	// Return status of audit
	//
	return(check_audit(aud_num));
}

//
// set_audit() - This function is used to set the value of an audit in
// the CMOS memory.
//
int set_audit(int audit_number, int data)
{
	register int	*addr;

	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is audit number in range ?
	//
	if(audit_number < 0 || audit_number >= cmos_info.max_audits)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_AUDIT_NUMBER);
	}

	//
	// Generate address of audit
	//
	addr = audit_base_addr + (audit_number << 1);

	//
	// Write the CRC for the audit
	//
	*(addr+1) = crc((unsigned char *)&data, sizeof(int));

	//
	// Write the audit data
	//
	*addr = data;

	//
	// Mark CMOS for update
	//
	cmos_update = 1;

	//
	// Check the status and return it
	//
	return(check_audit(audit_number));
}

//
// increment_audit() - This function is used to increment the value of an
// audit.
//
int increment_audit(int audit_number)
{
	int				old_val;
	register int	status;

	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is audit number in range ?
	//
	if(audit_number < 0 || audit_number >= cmos_info.max_audits)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_AUDIT_NUMBER);
	}

	//
	// Get old value of audit
	//
	old_val = audit_number;

	//
	// Get status of old value
	//
	status = get_audit(audit_number, &old_val);

	//
	// Bad value ?
	//
	if(status != CMOS_OK)
	{
		//
		// YES - Return ERROR
		//
		return(status);
	}

	//
	// Increment value
	//
	old_val++;

	//
	// Write it back and return status
	//
	return(set_audit(audit_number, old_val));
}

//
// check_adjustments() - This function is used to check the itegrity of the
// adjustments table in CMOS.
//
int check_adjustments(void)
{
	register int	*addr;
	register int	calc_crc;
	register int	stored_crc;

	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Calculate CRC value on adjustments
	//
	calc_crc = crc((unsigned char *)adjustment_base_addr, (cmos_info.max_adjustments * sizeof(int)));

	//
	// Get the stored CRC value
	//
	stored_crc = *(adjustment_base_addr + cmos_info.max_adjustments);

	//
	// Stored and calculated CRC's match ?
	//
	if(stored_crc != calc_crc)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_ADJUSTMENTS_INVALID);
	}

	//
	// Return OK
	//
	return(CMOS_OK);
}
	

//
// get_adjustment() - This function is used to get the value of an adjustment
// from the adjustments table in the CMOS.
//
int get_adjustment(int adjustment_number, int *to)
{
	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is adjustment number in range ?
	//
	if(adjustment_number < 0 || adjustment_number >= cmos_info.max_adjustments)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_ADJUSTMENT_NUMBER);
	}

	//
	// Get the Adjustment number requested
	//
	*to = *(adjustment_base_addr + adjustment_number);

	//
	// Return status of adjustments
	//
	return(check_adjustments());
}

//
// set_adjustment() - This function is used to set the value of an adjustment
// in the adjustments table of the CMOS memory.
//
int set_adjustment(int adjustment_number, int value)
{
	register int	check_data;

	//
	// Is CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is adjustment number in range ?
	//
	if(adjustment_number < 0 || adjustment_number >= cmos_info.max_adjustments)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_ADJUSTMENT_NUMBER);
	}

	//
	// Write the adjustment data
	//
	*(adjustment_base_addr + adjustment_number) = value;

	//
	// Generate the new check data
	//
	check_data = crc((unsigned char *)adjustment_base_addr, (cmos_info.max_adjustments * sizeof(int)));

	//
	// Write the new check data
	//
	*(adjustment_base_addr + cmos_info.max_adjustments) = check_data;

	//
	// Mark the CMOS for update
	//
	cmos_update = 1;

	//
	// Finally check the adjustments and return status
	//
	return(check_adjustments());
}


//
// check_generic_record() - This function is used to check the integrity of a generic
// record in a generic record table in CMOS.
//
static int check_generic_record(char *addr, int size)
{
	register unsigned int	calc_crc;
	unsigned int	stored_crc;

	//
	// Generate CRC based on what's in CMOS already
	//
	calc_crc = crc((unsigned char *)addr, size);

	//
	// Read the CRC stored in CMOS
	//
	memcpy(&stored_crc, addr+size, sizeof(unsigned int));

	//
	// Does stored and generated CRC match ?
	//
	if(calc_crc != stored_crc)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_GENERIC_TABLE_ERROR);
	}

	//
	// Return OK
	//
	return(CMOS_OK);
}

//
// get_generic_record() - This function is used to get the value of a generic
// entry from a generic record table in CMOS.
//
int get_generic_record(int tn, int en, void *to)
{
	register generic_cmos_record_info_t	*gcr;

	//
	// Is the CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is the table number within range ?
	//
	if(tn < 0 || tn >= cmos_info.num_gcr_tables)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_GENERIC_TABLE_NUMBER);
	}

	//
	// Get pointer to table information
	//
	gcr = &cmos_info.gcr[tn];

	//
	// Is the entry number valid ?
	//
	if(en < 0 || en >= gcr->num_entries)
	{
		//
		// Nope - Return ERROR
		//
		return(CMOS_INVALID_GENERIC_ENTRY_NUMBER);
	}

	//
	// Copy the table entry data to the buffer
	//
	memcpy(to, generic_record_base_addr[tn] + (en * (gcr->entry_size + 4)), gcr->entry_size);

	//
	// Check and return the status of this entry in this table
	//
	return(check_generic_record(generic_record_base_addr[tn] + (en * (gcr->entry_size + 4)), gcr->entry_size));
}

//
// set_generic_record() - This function is used to set a generic record
// in a generic record table in CMOS.
//
int set_generic_record(int tn, int en, void *data)
{
	register generic_cmos_record_info_t	*gcr;
	register	char								*addr;
	unsigned int					gen_crc;

	//
	// Is the CMOS configured ?
	//
	if(!cmos_configured)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_NOT_CONFIGURED);
	}

	//
	// Is the table number in range ?
	//
	if(tn < 0 || tn >= cmos_info.num_gcr_tables)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_GENERIC_TABLE_NUMBER);
	}

	//
	// Get pointer to table information
	//
	gcr = &cmos_info.gcr[tn];

	//
	// Is the entry number valid ?
	//
	if(en < 0 || en >= gcr->num_entries)
	{
		//
		// NOPE - Return ERROR
		//
		return(CMOS_INVALID_GENERIC_ENTRY_NUMBER);
	}

	//
	// Generate the address of the entry
	//
	addr = generic_record_base_addr[tn] + ((gcr->entry_size + 4) * en);

	//
	// Write the data to CMOS
	//
	memcpy(addr, data, gcr->entry_size);

	//
	// Generate and write the CRC on the data
	//
	gen_crc = crc((unsigned char *)data, gcr->entry_size);
	memcpy(addr + gcr->entry_size, &gen_crc, sizeof(int));

	//
	// Mark the CMOS for update
	//
	cmos_update = 1;

	//
	// Check the entry and return its status
	//
	return(check_generic_record(addr, gcr->entry_size));
}


void write_cmos_dump_record(cmos_dump_record_t *cd)
{
	register unsigned int	i;
	register unsigned char	*from = (unsigned char *)cd;

	cd->crc = crc((unsigned char *)&cd->watchdog_count, sizeof(cmos_dump_record_t) - sizeof(cd->crc));
	for(i = 0; i < sizeof(cmos_dump_record_t); i++)
	{
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		cmos_ram[i] = *from++;
	}
}

static int	cd_record_tries = 0;

int get_cmos_dump_record(cmos_dump_record_t *cd)
{
	unsigned char	*addr = (unsigned char *)CMOS_RAM_ADDR;
	unsigned int	count = sizeof(cmos_dump_record_t);
	unsigned long	calc_crc;
	unsigned char	*buf = (unsigned char *)cd;

	//
	// Read the data from CMOS
	//
	while(count--)
	{
		*buf++ = (char)*addr++;
	}

	//
	// Calculate CRC on the entire structure minus the first member
	//
	calc_crc = crc((char *)&cd->watchdog_count, sizeof(cmos_dump_record_t) - sizeof(cd->watchdog_count));

	//
	// Is calculated CRC and structure CRC the same ?
	//
	if(calc_crc != cd->crc)
	{
		//
		// NOPE - Is this the second time we have tried ?
		//
		if(cd_record_tries)
		{
			//
			// YES - Return error
			//
			return(-1);
		}

		//
		// Increment try counter
		//
		cd_record_tries++;

		//
		// Initialize the CMOS dump record
		//
		init_cmos_dump_record();

		//
		// Try to get it again (recursive)
		//
		get_cmos_dump_record(cd);
	}

	//
	// Reset try counter
	//
	cd_record_tries = 0;

	//
	// Return OK
	//
	return(0);
}

void init_cmos_dump_record(void)
{
	cmos_dump_record_t		cd;
	register unsigned char	*from = (unsigned char *)&cd;
	register unsigned int	i;

	disable_interrupts();

	*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
	*((volatile char *)RTC_WATCHDOG_REG) = 0;

	memset((char *)&cd, 0, sizeof(cmos_dump_record_t));
	cd.crc = crc((unsigned char *)&cd.watchdog_count, sizeof(cmos_dump_record_t) - sizeof(cd.crc));

	for(i = 0; i < sizeof(cmos_dump_record_t); i++)
	{
		*((volatile char *)CMOS_UNLOCK_ADDR) = 0;
		cmos_ram[i] = *from++;
	}

	enable_interrupts();
}

