/* $Revision: 4 $ */

/*
 * CMOS Memory Driver
 */
#include	<stdio.h>
#include	<ioctl.h>
#include	<unistd.h>
#include	<goose/cmos.h>

char	goose_cmos_c_version[] = {"$Revision: 4 $"};

unsigned long	cmos_crc(unsigned long *, int);
unsigned long	crc(unsigned char *, int);

#define		CRC32_SIZE				sizeof(int)
#define		NUM_BYTES_IN_AUDIT	(sizeof(int)+CRC32_SIZE)
#define		NUM_BYTES_IN_ADJUST	sizeof(int)

int						cmos_configured = 0;
cmos_config_info_t	cmos_info;
int						*cmos_base_addr;
int						*audit_base_addr;
int						*adjustment_base_addr;
int						*generic_record_base_addr[MAX_GENERIC_RECORD_TABLES];
int						*user_base_addr;
int						*cmos_end_addr;

/*
** cmos_read() - This function is used to read a contiguous area of the
** CMOS into a user specified buffer.
*/
int cmos_read(int *src_addr, char *buf, int count)
{
	if(src_addr >= cmos_end_addr)
	{
		return(0);
	}
	if((src_addr + count) >= cmos_end_addr)
	{
		return(0);
	}
	if(_ioctl(3, FIOCSETCMOSADDR, (int)src_addr) < 0)
	{
		return(0);
	}
	return(read(3, buf, count));
}

/*
** cmos_write() - This function is used to write data from a user buffer
** into a contingous area of CMOS.
*/
int cmos_write(int *dest_addr, char *buf, int count)
{
	if(dest_addr >= cmos_end_addr)
	{
		return(0);
	}
	if((dest_addr + count) >= cmos_end_addr)
	{
		return(0);
	}
	if(_ioctl(3, FIOCSETCMOSADDR, (int)dest_addr))
	{
		return(0);
	}
	return(write(3, buf, count));
}

/*
** copy_cmos_info() - This function is used to copy the CMOS configuration
** information from the application.
*/
static void copy_cmos_info(struct cmos_config_info *ci)
{
	int	i;
	char	*from, *to;

	from = (char *)ci;
	to = (char *)&cmos_info;
	for(i = 0; i < sizeof(struct cmos_config_info); i++)
	{
		*to++ = *from++;
	}
}

/*
** config_cmos() - This function is used to configure the number of audits,
** number of adjustments, number of high score tables, number of entries in
** each high score table, size (in bytes) of each entry in each high score
** table, and the size of the user defined area of CMOS memory.
*/
int config_cmos(struct cmos_config_info *cci)
{
	int	i;
	int	hs_table_size;

	copy_cmos_info(cci);
	if(cmos_info.num_gcr_tables > MAX_GENERIC_RECORD_TABLES)
	{
		return(CMOS_TOO_MANY_HS_TABLES);
	}
	if(_ioctl(3, FIOCGETCMOSADDR, (int)&cmos_base_addr))
	{
		return(CMOS_CONFIG_BAD_ADDR);
	}
	if(_ioctl(3, FIOCGETCMOSSIZE, (int)&cmos_info.cmos_size))
	{
		return(CMOS_CONFIG_BAD_SIZE);
	}
	cci->cmos_size = cmos_info.cmos_size;
	cmos_end_addr = cmos_base_addr + cmos_info.cmos_size;
	audit_base_addr = cmos_base_addr;
	adjustment_base_addr = audit_base_addr + (cmos_info.max_audits * NUM_BYTES_IN_AUDIT);
	if(adjustment_base_addr >= cmos_end_addr)
	{
		return(CMOS_CONFIG_TOO_LARGE);
	}
	generic_record_base_addr[0] = adjustment_base_addr + ((cmos_info.max_adjustments + 1) * NUM_BYTES_IN_ADJUST);
	if(generic_record_base_addr[0] >= cmos_end_addr)
	{
		return(CMOS_CONFIG_TOO_LARGE);
	}
	for(i = 1; i < cmos_info.num_gcr_tables && i < 16; i++)
	{
		hs_table_size = cmos_info.gcr[i-1].entry_size + CRC32_SIZE;
		hs_table_size *= cmos_info.gcr[i-1].num_entries;
		generic_record_base_addr[i] = generic_record_base_addr[i-1] + hs_table_size;
		if(generic_record_base_addr[i] >= cmos_end_addr)
		{
			return(CMOS_CONFIG_TOO_LARGE);
		}
	}
	hs_table_size = cmos_info.gcr[i-1].entry_size + CRC32_SIZE;
	hs_table_size *= cmos_info.gcr[i-1].num_entries;
	user_base_addr = generic_record_base_addr[i-1] + hs_table_size;
	if(user_base_addr >= cmos_end_addr)
	{
		return(CMOS_CONFIG_TOO_LARGE);
	}
	cmos_configured = 1;
	return(CMOS_OK);
}

/*
** get_cmos_user_address() - This function returns the base address of the
** user area of CMOS memory.
*/
int *get_cmos_user_address(void)
{
	if(!cmos_configured)
	{
		return((int *)0);
	}
	return(user_base_addr);
}

/*
** get_cmos_user_size() - This function returns the size of the user area
** in the CMOS memory.
*/
int get_cmos_user_size(void)
{
	if(!cmos_configured)
	{
		return(0);
	}
	return(((int)cmos_end_addr - (int)user_base_addr) >> 2);
}


/*
** _check_audit() - This function is used to check the itegrity of the audit
** specified by the address passed to it.
*/
static int _check_audit(int *aud_addr)
{
	int	calc_crc;
	int	stored_crc;

	cmos_read((aud_addr + CRC32_SIZE), (char *)&stored_crc, CRC32_SIZE);
	calc_crc = cmos_crc((unsigned long *)aud_addr, CRC32_SIZE);
	if(stored_crc != calc_crc)
	{
		return(CMOS_AUDIT_ERROR);
	}
	return(CMOS_OK);
}

/*
** check_audit() - This function is used to check the itegrity of an
** audit using its audit number.
*/
int check_audit(int aud_num)
{
	if(aud_num < 0 || aud_num >= cmos_info.max_audits)
	{
		return(CMOS_INVALID_AUDIT_NUMBER);
	}
	return(_check_audit((cmos_base_addr + (aud_num * NUM_BYTES_IN_AUDIT))));
}

/*
** check_audits() - This function is used to check all of the audits in the
** CMOS.
*/
int check_audits(void)
{
	int	i;
	int	status;

	for(i = 0; i < cmos_info.max_audits; i++)
	{
		status = check_audit(i);
		if(status != CMOS_OK)
		{
			return(status);
		}
	}
	return(CMOS_OK);
}

/*
** clear_audits() - This function is used to clear all of the audits to
** 0.
*/
int clear_audits(void)
{
	int	i;

	for(i = 0; i < cmos_info.max_audits; i++)
	{
		set_audit(i, 0);
	}
	return(CMOS_OK);
}

/*
** get_audit() - This function is used to retrieve the value of an audit
*/
int get_audit(int aud_num, int *to)
{
	int	*aud_addr;

	if(aud_num < 0 || aud_num >= cmos_info.max_audits)
	{
		return(CMOS_INVALID_AUDIT_NUMBER);
	}
	aud_addr = (audit_base_addr + (aud_num * NUM_BYTES_IN_AUDIT));
	cmos_read(aud_addr, (char *)to, CRC32_SIZE);
	return(_check_audit(aud_addr));
}

/*
** set_audit() - This function is used to set the value of an audit in
** the CMOS memory.
*/
int set_audit(int audit_number, int data)
{
	int	check_data;
	int	*addr;
	int	status;

	if(audit_number < 0 || audit_number >= cmos_info.max_audits)
	{
		return(CMOS_INVALID_AUDIT_NUMBER);
	}
	else
	{
		/* Generate address of audit */
		addr = audit_base_addr + (audit_number * NUM_BYTES_IN_AUDIT);

		/* Check to see if it is OK */
		status = _check_audit(addr);

		/* If the audit is ok, generate and write new check data */
		/* for the audit to be written. */
		check_data = crc((unsigned char *)&data, CRC32_SIZE);
		cmos_write(addr+CRC32_SIZE, (char *)&check_data, CRC32_SIZE);

		/* Write the audit data */
		cmos_write(addr, (char *)&data, CRC32_SIZE);

		/* If the original status was bad - tell the user about it */
		if(status != CMOS_OK)
		{
			return(status);
		}

		/* Finally recheck the audit */
		return(_check_audit(addr));
	}
}

/*
** increment_audit() - This function is used to increment the value of an
** audit.
*/
int increment_audit(int audit_number)
{
	int	old_val;
	int	status;

	old_val = audit_number;
	status = get_audit(audit_number, &old_val);
	if(status != CMOS_OK)
	{
		return(status);
	}
	old_val++;
	return(set_audit(audit_number, old_val));
}

/*
** check_adjustments() - This function is used to check the itegrity of the
** adjustments table in CMOS.
*/
int check_adjustments(void)
{
	int	*addr;
	int	calc_crc;
	int	stored_crc;

	addr = adjustment_base_addr;
	calc_crc = cmos_crc((unsigned long *)adjustment_base_addr, (cmos_info.max_adjustments * NUM_BYTES_IN_ADJUST));
	addr = adjustment_base_addr + (cmos_info.max_adjustments*NUM_BYTES_IN_ADJUST);
	cmos_read(addr, (char *)&stored_crc, CRC32_SIZE);
	if(stored_crc != calc_crc)
	{
		return(CMOS_ADJUSTMENTS_INVALID);
	}
	return(CMOS_OK);
}
	

/*
** get_adjustment() - This function is used to get the value of an adjustment
** from the adjustments table in the CMOS.
*/
int get_adjustment(int adjustment_number, int *to)
{
	int	*adj_addr;
	int	status;

	if(adjustment_number < 0 || adjustment_number >= cmos_info.max_adjustments)
	{
		return(CMOS_INVALID_ADJUSTMENT_NUMBER);
	}
	status = check_adjustments();
	adj_addr = adjustment_base_addr + (adjustment_number * NUM_BYTES_IN_ADJUST);
	cmos_read(adj_addr, (char *)to, NUM_BYTES_IN_ADJUST);
	return(status);
}

/*
** set_adjustment() - This function is used to set the value of an adjustment
** in the adjustments table of the CMOS memory.
*/
int set_adjustment(int adjustment_number, int value)
{
	int	*addr;
	int	check_data;

	if(adjustment_number < 0 || adjustment_number >= cmos_info.max_adjustments)
	{
		return(CMOS_INVALID_ADJUSTMENT_NUMBER);
	}
	/* Generate address of adjustment */
	addr = adjustment_base_addr + (adjustment_number * NUM_BYTES_IN_ADJUST);

	/* Write the adjustment data */
	cmos_write(addr, (char *)&value, NUM_BYTES_IN_ADJUST);

	/* Generate the new check data */
	check_data = cmos_crc((unsigned long *)adjustment_base_addr, (cmos_info.max_adjustments * NUM_BYTES_IN_ADJUST));

	/* Get address of check data area for adjustments */
	addr = adjustment_base_addr + (cmos_info.max_adjustments * NUM_BYTES_IN_ADJUST);

	/* Write the new check data */
	cmos_write(addr, (char *)&check_data, CRC32_SIZE);

	/* Finally check the adjustments */
	return(check_adjustments());
}


/*
** check_generic_record() - This function is used to check the integrity of a generic
** record in a generic record table in CMOS.
*/
static int check_generic_record(int *addr, int size)
{
	int	calc_crc;
	int	stored_crc;

	/* Generate CRC based on what's in CMOS already */
	calc_crc = cmos_crc((unsigned long *)addr, size);

	/* Read the CRC stored in CMOS */
	cmos_read((addr+size), (char *)&stored_crc, CRC32_SIZE);

	/* Check to see if they match */
	if(calc_crc != stored_crc)
	{
		return(CMOS_GENERIC_TABLE_ERROR);
	}
	return(CMOS_OK);
}

/*
** get_generic_record() - This function is used to get the value of a generic
** entry from a generic record table in CMOS.
*/
int get_generic_record(int tn, int en, void *to)
{
	generic_cmos_record_info_t	*gcr;
	int	*addr;

	/* Check for valid high score table number */
	if(tn < 0 || tn >= cmos_info.num_gcr_tables)
	{
		return(CMOS_INVALID_GENERIC_TABLE_NUMBER);
	}

	/* Get high score table information */
	gcr = &cmos_info.gcr[tn];

	/* Check for valid high score entry number */
	if(en < 0 || en >= gcr->num_entries)
	{
		return(CMOS_INVALID_GENERIC_ENTRY_NUMBER);
	}

	/* Calculate address of entry */
	addr = generic_record_base_addr[tn] + (en * (gcr->entry_size + CRC32_SIZE));

	/* Get the data for the entry */
	cmos_read(addr, (char *)to, gcr->entry_size);

	/* Return the check status of the entry */
	return(check_generic_record(addr, gcr->entry_size));
}

/*
** set_generic_record() - This function is used to set a generic record
** in a generic record table in CMOS.
*/
int set_generic_record(int tn, int en, void *data)
{
	generic_cmos_record_info_t	*gcr;
	int	*addr;
	int	crc32;

	/* Check to see if this is valid high score table number */
	if(tn < 0 || tn >= cmos_info.num_gcr_tables)
	{
		return(CMOS_INVALID_GENERIC_TABLE_NUMBER);
	}

	/* Get the high score table information */
	gcr = &cmos_info.gcr[tn];

	/* Check to see if this is a valid entry number for the table */
	if(en < 0 || en >= gcr->num_entries)
	{
		return(CMOS_INVALID_GENERIC_ENTRY_NUMBER);
	}

	/* Get the address of the high scrore entry in CMOS */
	addr = generic_record_base_addr[tn] + ((gcr->entry_size + CRC32_SIZE) * en);

	/* Write the new data */
	cmos_write(addr, data, gcr->entry_size);

	/* Generate the check data */
	crc32 = crc(data, gcr->entry_size);

	/* Write the check data */
	cmos_write(addr + gcr->entry_size, (char *)&crc32, CRC32_SIZE);

	/* Check the entry and return its status */
	return(check_generic_record(addr, gcr->entry_size));
}
