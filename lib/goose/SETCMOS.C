/****************************************************************************/
/*                                                                          */
/* setcmos.c - CMOS setup hi level functions.                               */
/*                                                                          */
/* Written by:  Michael J. Lynch                                            */
/* Version:     1.00                                                        */
/*                                                                          */
/* Copyright (c) 1996 by Williams Electronics Games Inc.                    */
/* All Righos Reserved                                                      */
/*                                                                          */
/* $Revision: 7 $                                                             */
/*                                                                          */
/****************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<goose/cmos.h>
#include	<goose/goose.h>

char	goose_setcmos_c_version[] = {"$Revision: 7 $"};


// Saved pointer to the application passed pointer to the setup_cmos_struct
static setup_cmos_struct_t		*sct = NULL;
static setup_cmos_struct2_t	*sct2 = NULL;


// Function used to clear all of the audits to 0.
static void clear_all_audits(void)
{
	int	i;

	// Clear all of the audits to 0
	for(i = 0; i < sct->conf->max_audits; i++)
	{
		// Set this audit to 0
		set_audit(i, 0);
	}
}

static void clear_all_audits2(void)
{
	int	i;

	// Clear all of the audits to 0
	for(i = 0; i < sct2->conf->max_audits; i++)
	{
		// Set this audit to 0
		set_audit(i, 0);
	}
}

// Check all of the audits. Return 0 if OK, 1 if error.  This function
// attempts to reset any audits it finds that have errors.  If the audit
// can not be reset, an error is returned
static int check_all_audits(void)
{
	int	i;
	int	status = 0;
	int	val;

	// Check all of the audits
	for(i = 0; i < sct->conf->max_audits; i++)
	{
		// Get an audit - is it valid ?
		if(get_audit(i, &val))
		{
			// NOPE - reset the audit
			set_audit(i, 0);

			// Grab the audit again - Is it valid ?
			if(get_audit(i, &val))
			{
				// Nope - ERROR
#ifdef DEBUG
				fprintf(stderr, "Trouble with audit number: %d\r\n", i);
				lockup();
#endif
				status = 1;
			}
		}
	}

	// Return the status
	return(status);
}

static int check_all_audits2(void)
{
	int	i;
	int	status = 0;
	int	val;

	// Check all of the audits
	for(i = 0; i < sct2->conf->max_audits; i++)
	{
		// Get an audit - is it valid ?
		if(get_audit(i, &val))
		{
			// NOPE - reset the audit
			set_audit(i, 0);

			// Grab the audit again - Is it valid ?
			if(get_audit(i, &val))
			{
				// Nope - ERROR
#ifdef DEBUG
				fprintf(stderr, "Trouble with audit number: %d\r\n", i);
				lockup();
#endif
				status = 1;
			}
		}
	}

	// Return the status
	return(status);
}


// Check all of the records in one of the user defined tables in cmos
// specified by the table argment.  Return 1 if errors are detected,
// and 0 if no errors are deteected.
static int check_records(int table)
{
	int	i;
	int	status = 0;
	void	*buf;

	// Allocate memory for the buffer for record checking
	if((buf = malloc(sct->conf->gcr[table].entry_size)) == NULL)
	{
		return(1);
	}

	// Check all of the records in this table
	for(i = 0; i < sct->conf->gcr[table].num_entries; i++)
	{
		if(get_generic_record(table, i, buf))
		{
			status = 1;
		}
	}

	// Free the memory allocated
	free(buf);

	// Return the status
	return(status);
}

static int check_records2(int table)
{
	int	i;
	int	status = 0;
	void	*buf;

	// Allocate memory for the buffer for record checking
	if((buf = malloc(sct2->conf->gcr[table].entry_size)) == NULL)
	{
		return(1);
	}

	// Check all of the records in this table
	for(i = 0; i < sct2->conf->gcr[table].num_entries; i++)
	{
		if(get_generic_record(table, i, buf))
		{
			status = 1;
		}
	}

	// Free the memory allocated
	free(buf);

	// Return the status
	return(status);
}

// Check all of the records in all user defined record table in cmos.
// Return 0 if all records in all tables are ok, non-zero on error.
static int check_all_records(void)
{
	int	i;
	int	status = 0;

	// Check the all of the tables
	for(i = 0; i < sct->conf->num_gcr_tables; i++)
	{
		status |= (check_records(i) << i);
	}

	// Return the status
	return(status);
}



static void check_all_records2(int *rec_status)
{
	int	i;

	// Check the all of the tables
	for(i = 0; i < sct2->conf->num_gcr_tables; i++)
	{
		rec_status[i] = check_records(i);
	}
}


// This function is used to setup the system CMOS for use.  It verifies the
// data in CMOS, attempts to restore the CMOS if errors are detected in
// the audits, adjustments, and user defined record areas.  It returns a
// status of either OK, DEAD, or CONFIG_ERROR.  If a status of OK is returned
// the individual status' of each of the major CMOS sections (audits,
// adjustments, and user defined records) is returned in the aud_stat,
// adj_stat, and rec_stat pointer arguments.
int setup_cmos(setup_cmos_struct_t *s, int *aud_stat, int *adj_stat, int *rec_stat)
{
	FILE			*fp;
	int			i;
	int			val;
	static int	cmos_status = CMOS_SETUP_OK;

	// Has this function already been called?
	if(sct)
	{
		// YES - return the status from the last call
		return(cmos_status);
	}

	// Set status for audits, adjustments, and records to OK
	*aud_stat = CMOS_SETUP_OK;
	*adj_stat = CMOS_SETUP_OK;
	*rec_stat = CMOS_SETUP_OK;

	// Initialize the pointer
	sct = s;

	// Add 1 to max audits
	s->conf->max_audits++;

	// Open the audits.fmt file and write it
	if((fp = fopen("audits.fmt", "wb")) == (FILE *)0)
	{
#ifdef DEBUG
		fprintf(stderr, "Can not open file AUDITS.FMT\r\n");
		lockup();
#endif
		return(CMOS_SETUP_AUDIT_FILE_OPEN);
	}
	if(fp)
	{
		fprintf(fp, "cmos size %d audits %d adjustments %d gcrtables %d cmosdone\r\n",
			s->conf->cmos_size,
			s->conf->max_audits,
			s->conf->max_adjustments,
			s->conf->num_gcr_tables);

		for(i = 0; i < s->conf->num_gcr_tables; i++)
		{
			fprintf(fp, "cmosgcr entrysize %d numentries %d cmosgcrdone\r\n",
				s->conf->gcr[i].entry_size,
				s->conf->gcr[i].num_entries);
		}

		fprintf(fp, "\r\n");

		sct->write_audit_file(fp);
		fflush(fp);
		fclose(fp);
	}

	// Open the adjustments file and write it out (added 3-5-98 BRE)
	if((fp = fopen("adjust.fmt", "wb")) == (FILE *)0)
	{
#ifdef DEBUG
		fprintf(stderr, "Can not open file ADJUST.FMT\r\n");
		lockup();
#endif
		return(CMOS_SETUP_ADJUST_FILE_OPEN);
	}

	if(fp)
	{
		/* Write adjustment file */
		sct->write_adjust_file(fp);
		fflush(fp);
		fclose(fp);
	}

	// Configure the cmos based on info supplied by application. Config OK ?
	if(!config_cmos(s->conf))
	{
		// YES - Check all of the records
		if(check_all_records())
		{
			// FAIL - Is there a restore function ?
			if(sct->reset_records)
			{
				sct->reset_records();
				*rec_stat = CMOS_SETUP_RESTORED;
			}
		}

		// Check the adjustments
		if(check_system_adjustments(sct->ai_size, sct->ai))
		{
			// FAIL - restore factory adjustments
			restore_factory_adjustments(sct->ai_size, sct->ai);
			*adj_stat = CMOS_SETUP_RESTORED;
		}

		// Check audits
		if(check_all_audits())
		{
			// FAIL - restore audits
			clear_all_audits();
			*aud_stat = CMOS_SETUP_RESTORED;
		}

		// Was anything restored ?
		if(*rec_stat == CMOS_SETUP_RESTORED || *adj_stat == CMOS_SETUP_RESTORED || *aud_stat == CMOS_SETUP_RESTORED)
		{
			// YES - Recheck everything

			// Check the records
			if(check_all_records())
			{
				// FAIL - Records are dead
				*rec_stat = CMOS_SETUP_DEAD;
			}

			// Check the adjustments
			if(check_system_adjustments(sct->ai_size, sct->ai))
			{
				// FAIL - Adjustments are dead
				*adj_stat = CMOS_SETUP_DEAD;
			}

			// Check the audits
			if(check_all_audits())
			{
				// FAIL - Audits are dead
				*aud_stat = CMOS_SETUP_DEAD;
			}
		}

		// Are all three sections dead
		if(*rec_stat == CMOS_SETUP_DEAD && *adj_stat == CMOS_SETUP_DEAD && *aud_stat == CMOS_SETUP_DEAD)
		{
			// CMOS is dead
			cmos_status = CMOS_SETUP_DEAD;
			return(CMOS_SETUP_DEAD);
		}

		// Are the audits valid ?
		// This is used to detect changes in the number of audits!!
		if(*aud_stat != CMOS_SETUP_DEAD)
		{
			// YES - Is the last audit valid
			if(get_audit(sct->conf->max_audits - 1, &val))
			{
				// YES - Does the value match the max audits ?
				if(val != sct->conf->max_audits)
				{
					// NOPE - Then size of audit table changed and CMOS MUST be
					// reset.

					// Is there a record reset function ?
					if(sct->reset_records)
					{
						// YES - Call it
						sct->reset_records();
					}

					// Reset the adjustments
					restore_factory_adjustments(sct->ai_size, sct->ai);

					// Reset the factory restore adjustment
					set_adjustment(FACTORY_RESTORE_ADJ, 0);

					// Reset all of the audits
					clear_all_audits();

					// Set the audit marker					
					set_audit(sct->conf->max_audits - 1, sct->conf->max_audits);

					// Set the statuses of each of the sections
					*aud_stat = CMOS_SETUP_RESTORED;
					*adj_stat = CMOS_SETUP_RESTORED;
					*rec_stat = CMOS_SETUP_RESTORED;
				}
			}
		}

		// Are the adjustments valid ?
		if(*adj_stat != CMOS_SETUP_DEAD)
		{
			// YES - Get the factory restore adustment
			if(!get_adjustment(FACTORY_RESTORE_ADJ, &val))
			{
				// Is it set ?
				if(val)
				{
					// YES - Reset the records
					if(sct->reset_records)
					{
						sct->reset_records();
					}
					
				/* MIKE: I REMOVED THE RESTORE_FACTORY_ADJUSTMENTS */
				/*       ANN CLEAR ALL AUDITS BELOW - BRE */

				 //	// Restore the adjustments
				 //	restore_factory_adjustments(sct->ai_size, sct->ai);
				 //
					// Reset the factory restore adjustment
					set_adjustment(FACTORY_RESTORE_ADJ, 0);

				 //	// Clear the audits
				 //	clear_all_audits();

					// Set the statuses of each of the sections
					*aud_stat = CMOS_SETUP_RESTORED;
					*adj_stat = CMOS_SETUP_RESTORED;
					*rec_stat = CMOS_SETUP_RESTORED;
				}
			}

			// Is the clear records adjustment valid ?
			if(!get_adjustment(sct->rec_reset_adj_num, &val))
			{
				// YES - does it say to clear the records ?
				if(val)
				{
					// YES
					if(sct->reset_records)
					{
						// Clear the records
						sct->reset_records();
						*rec_stat = CMOS_SETUP_RESTORED;
					}

					// Reset the adjustment
					set_adjustment(sct->rec_reset_adj_num, 0);
				}
			}
		}

		// Are all of the records valid ?
		if(check_all_records())
		{
			// NOPE - Mark records as dead
			*rec_stat = CMOS_SETUP_DEAD;
		}

		// Are all of the adjustments valid ?
		if(check_system_adjustments(sct->ai_size, sct->ai))
		{
			// NOPE - Mark adjustments dead
			*adj_stat = CMOS_SETUP_DEAD;
		}

		// Are all of the audits valid ?
		if(check_all_audits())
		{
			*aud_stat = CMOS_SETUP_DEAD;
		}
	}
	else
	{
#ifdef DEBUG
		fprintf(stderr, "Error occured in trying to INIT. cmos\r\n");
		lockup();
#endif
		cmos_status = CMOS_SETUP_CONFIG_ERROR;
	}

	return(cmos_status);
}



// This function is used to setup the system CMOS for use.  It verifies the
// data in CMOS, attempts to restore the CMOS if errors are detected in
// the audits, adjustments, and user defined record areas.  It returns a
// status of either OK, DEAD, or CONFIG_ERROR.  If a status of OK is returned
// the individual status' of each of the major CMOS sections (audits,
// adjustments, and user defined records) is returned in the aud_stat,
// adj_stat, and rec_stat pointer arguments.
int setup_cmos2(setup_cmos_struct2_t *s, int *aud_stat, int *adj_stat, int *rec_stat)
{
	FILE			*fp;
	int			i;
	int			val;
	int			rec_status;
	static int	cmos_status2 = CMOS_SETUP_OK;

	// Set status for audits, adjustments, and records to OK
	*aud_stat = CMOS_SETUP_OK;
	*adj_stat = CMOS_SETUP_OK;
	for(i = 0; i < s->conf->num_gcr_tables; i++)
	{
		rec_stat[i] = CMOS_SETUP_OK;
	}

	if(!sct2)
	{
		// Add 1 to max audits
		s->conf->max_audits++;
	}

	sct2 = s;

	// Check to see that the top used are less or equal to max
	if(s->conf->max_audits < s->top_audit)
	{
		return(CMOS_SETUP_TOO_MANY_AUDITS);
	}
	if(s->conf->max_adjustments < s->top_adjustment)
	{
		return(CMOS_SETUP_TOO_MANY_ADJUST);
	}

	// Open the audits.fmt file and write it
	if((fp = fopen("audits.fmt", "wb")) == (FILE *)0)
	{
		return(CMOS_SETUP_AUDIT_FILE_OPEN);
	}
	if(fp)
	{
		fprintf(fp, "cmos size %d audits %d adjustments %d gcrtables %d cmosdone\r\n",
			s->conf->cmos_size,
			s->conf->max_audits,
			s->conf->max_adjustments,
			s->conf->num_gcr_tables);

		for(i = 0; i < s->conf->num_gcr_tables; i++)
		{
			fprintf(fp, "cmosgcr entrysize %d numentries %d cmosgcrdone\r\n",
				s->conf->gcr[i].entry_size,
				s->conf->gcr[i].num_entries);
		}

		fprintf(fp, "\r\n");

		s->write_audit_file(fp);
		fflush(fp);
		fclose(fp);
	}

	// Open the adjustments file and write it out (added 3-5-98 BRE)
	if((fp = fopen("adjust.fmt", "wb")) == (FILE *)0)
	{
		return(CMOS_SETUP_ADJUST_FILE_OPEN);
	}

	if(fp)
	{
		/* Write adjustment file */
		s->write_adjust_file(fp);
		fflush(fp);
		fclose(fp);
	}

	// Configure the cmos based on info supplied by application. Config OK ?
	if(!config_cmos(s->conf))
	{
		// Check audits
		if(check_all_audits2())
		{
			// FAIL - restore audits
			clear_all_audits2();
			*aud_stat = CMOS_SETUP_RESTORED;
		}

		// Check the adjustments
		if(check_system_adjustments(s->ai_size, s->ai))
		{
			// FAIL - restore factory adjustments
			restore_factory_adjustments(s->ai_size, s->ai);
			*adj_stat = CMOS_SETUP_RESTORED;
		}

		// Check all of the records
		rec_status = CMOS_SETUP_OK;
		for(i = 0; i < s->conf->num_gcr_tables; i++)
		{
			if(check_records2(i))
			{
				s->reset_records[i]();
				rec_stat[i] = CMOS_SETUP_RESTORED;
				rec_status = CMOS_SETUP_RESTORED;
			}
			else
			{
				rec_stat[i] = CMOS_SETUP_OK;
			}
		}

		// Was anything restored ?
		if(rec_status == CMOS_SETUP_RESTORED || *adj_stat == CMOS_SETUP_RESTORED || *aud_stat == CMOS_SETUP_RESTORED)
		{
			// YES - Recheck everything

			// Check the audits
			if(check_all_audits2())
			{
				// Audits are dead
				*aud_stat = CMOS_SETUP_DEAD;
			}

			// Check the adjustments
			if(check_system_adjustments(s->ai_size, s->ai))
			{
				// Adjustments are dead
				*adj_stat = CMOS_SETUP_DEAD;
			}

			// Check the records
			for(i = 0; i < s->conf->num_gcr_tables; i++)
			{
				if(check_records2(i))
				{
					rec_stat[i] = CMOS_SETUP_DEAD;
				}
			}
		}

		// Are all sections dead
		if(*adj_stat == CMOS_SETUP_DEAD && *aud_stat == CMOS_SETUP_DEAD)
		{
			for(i = 0; i < s->conf->num_gcr_tables; i++)
			{
				if(rec_stat[i] != CMOS_SETUP_DEAD)
				{
					break;
				}
			}

			if(i == s->conf->num_gcr_tables)
			{
				// CMOS is dead
				cmos_status2 = CMOS_SETUP_DEAD;
				return(CMOS_SETUP_DEAD);
			}
		}

		// This is used to detect changes in the number of audits!!
		// If the maximum number of audits changes, EVERYTHING must be
		// restored because EVERYTHING shifts in CMOS.
		if(*aud_stat != CMOS_SETUP_DEAD)
		{
			// Is the last audit valid
			if(get_audit(s->conf->max_audits-1, &val))
			{
				// YES - Does the value match the max audits ?
				if(val != s->conf->max_audits)
				{
					// Then size of audit table changed and CMOS MUST be reset.

					// Reset all of the audits
					clear_all_audits2();

					// Set the audit marker					
					set_audit(s->conf->max_audits - 1, s->conf->max_audits);

					*aud_stat = CMOS_SETUP_RESTORED;

					// Reset the adjustments
					restore_factory_adjustments(s->ai_size, s->ai);

					// Reset the factory restore adjustment
					set_adjustment(FACTORY_RESTORE_ADJ, 0);

					*adj_stat = CMOS_SETUP_RESTORED;

					// Reset all of the user defined tables
					for(i = 0; i < s->conf->num_gcr_tables; i++)
					{
						s->reset_records[i]();
						rec_stat[i] = CMOS_SETUP_RESTORED;
						set_adjustment(s->rec_reset_adj_num[i], 0);
					}
				}
			}
		}

		// If adjustments are valid, check for diagnostics requested restore
		// of adjustments and/or user defined tables
		if(*adj_stat != CMOS_SETUP_DEAD)
		{
			// Get the factory restore adustment
			if(get_adjustment(FACTORY_RESTORE_ADJ, &val))
			{
				// Restore the adjustments
				restore_factory_adjustments(s->ai_size, s->ai);

				// Reset the factory restore adjustment
				set_adjustment(FACTORY_RESTORE_ADJ, 0);

				// Set the statuses of each of the sections
				*aud_stat = CMOS_SETUP_RESTORED;
			}
			else if(val)
			{
				// Restore the adjustments
				restore_factory_adjustments(s->ai_size, s->ai);

				// Reset the factory restore adjustment
				set_adjustment(FACTORY_RESTORE_ADJ, 0);

				// Set the statuses of each of the sections
				*aud_stat = CMOS_SETUP_RESTORED;
			}

			// Reset user defined tables
			for(i = 0; i < s->conf->num_gcr_tables; i++)
			{
				if(get_adjustment(s->rec_reset_adj_num[i], &val))
				{
					s->reset_records[i]();
					rec_stat[i] = CMOS_SETUP_RESTORED;
					set_adjustment(s->rec_reset_adj_num[i], 0);
				}
				else if(val)
				{
					s->reset_records[i]();
					rec_stat[i] = CMOS_SETUP_RESTORED;
					set_adjustment(s->rec_reset_adj_num[i], 0);
				}
			}
		}

		// Are all of the audits valid ?
		if(check_all_audits2())
		{
			// Mark audits dead
			*aud_stat = CMOS_SETUP_DEAD;
		}

		// Are all of the adjustments valid ?
		if(check_system_adjustments(s->ai_size, s->ai))
		{
			// Mark adjustments dead
			*adj_stat = CMOS_SETUP_DEAD;
		}

		// Are all of the records valid ?
		for(i = 0; i < s->conf->num_gcr_tables; i++)
		{
			if(check_records2(i))
			{
				rec_stat[i] = CMOS_SETUP_DEAD;
			}
		}
	}
	else
	{
		cmos_status2 = CMOS_SETUP_CONFIG_ERROR;
	}

	return(cmos_status2);
}


