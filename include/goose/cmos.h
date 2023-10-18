#ifndef	__CMOS_H__
#define	__CMOS_H__

#ifdef VERSIONS
char	goose_cmos_h_version[] = {"$Revision: 7 $"};
#endif

#include	<goose/adjust.h>

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



// Values returned in by the setup_cmos() function.
#define	CMOS_SETUP_OK						0
#define	CMOS_SETUP_AUDIT_FILE_OPEN		-1
#define	CMOS_SETUP_ADJUST_FILE_OPEN	-2
#define	CMOS_SETUP_DEAD					-3
#define	CMOS_SETUP_CONFIG_ERROR			-4
#define	CMOS_SETUP_TOO_MANY_AUDITS		-5
#define	CMOS_SETUP_TOO_MANY_ADJUST		-6

// Values returned in the individual cmos section status pointers
#define	CMOS_SETUP_SECTION_OK			CMOS_SETUP_OK
#define	CMOS_SETUP_RESTORED				1


// Definition of the structure used by the setup_cmos() function.
typedef struct setup_cmos_struct
{
	// Function called to write the game specific audit info to the
	// audits.fmt file
	void							(*write_audit_file)(FILE *);

	// Function called to write the game specific adjustment info to the
	// adjust.fmt file
	void							(*write_adjust_file)(FILE *);

	// Function called to reset the user defined record tables to their
	// default values.
	void							(*reset_records)(void);

	// Pointer to the application level defined CMOS configuration structure.
	struct cmos_config_info	*conf;

	// Pointer to the application level defined adjustment_info table
	adjustment_info_t			*ai;

	// Number of element in the application level defined adjustment_info_table
	int							ai_size;

	// Adjustment nember used for factory reset of user defined record tables
	int							rec_reset_adj_num;
} setup_cmos_struct_t;


// Definition of the structure used by the setup_cmos2() function.
typedef struct setup_cmos_struct2
{
	// Function called to write the game specific audit info to the
	// audits.fmt file
	void							(*write_audit_file)(FILE *);

	// Function called to write the game specific adjustment info to the
	// adjust.fmt file
	void							(*write_adjust_file)(FILE *);

	// Pointer to the application level defined CMOS configuration structure.
	struct cmos_config_info	*conf;

	// Pointer to the application level defined adjustment_info table
	adjustment_info_t			*ai;

	// Number of element in the application level defined adjustment_info_table
	int							ai_size;

	// Maximum audit number actually used
	int							top_audit;

	// Maximum adjustment number actually used
	int							top_adjustment;

	// Adjustment number used for factory reset of user defined record tables
	int							rec_reset_adj_num[MAX_GENERIC_RECORD_TABLES];

	// Functions used to restore each of the the user defined record tables
	void							(*reset_records[MAX_GENERIC_RECORD_TABLES])(void);
} setup_cmos_struct2_t;


/* CMOS function prototypes */
int cmos_read(int *src_addr, char *buf, int count);
int cmos_write(int *dest_addr, char *buf, int count);
int config_cmos(struct cmos_config_info *cci);
int *get_cmos_user_address(void);
int get_cmos_user_size(void);
int check_audit(int aud_num);
int check_audits(void);
int clear_audits(void);
int get_audit(int aud_num, int *to);
int set_audit(int audit_number, int data);
int increment_audit(int audit_number);
int check_adjustments(void);
int get_adjustment(int adjustment_number, int *to);
int set_adjustment(int adjustment_number, int value);
int get_generic_record(int tn, int en, void *to);
int set_generic_record(int tn, int en, void *data);
int setup_cmos(setup_cmos_struct_t *s, int *aud_stat, int *adj_stat, int *rec_stat);
int setup_cmos2(setup_cmos_struct2_t *s, int *aud_stat, int *adj_stat, int *rec_stat);


// Macro definitions for audits file
#ifndef ADD_MENU_ITEM
#define	ADD_MENU_ITEM(a, b) 	fprintf(fp, "\r\nitem \"%s\" itemhelp \"%s\" itemdone\r\n", (a), (b))

#define	ADD_AUDIT(a, b, c, d, e, f)	fprintf(fp, "startaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u auditdone\r\n", (a), (b), (c), (d), (e), (f))

#define	ADD_AUDIT_DIV(a, b, c, d, e, f, g)	fprintf(fp, "startaudit %d denomaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u auditdone\r\n", (a), (b), (c), (d), (e), (f), (g))

#define	ADD_AUDIT_XY(a, b, c, d, e, f, g, h, i, j)	fprintf(fp, "startaudit %d font %d pcolor %u prompt \"%s\" %s acolor %u ax %u ay %u px %u py %u auditdone\r\n", (a), (b), (c), (d), (e), (f), (g), (h), (i), (j))

#define	MESSAGE(a, b, c, d, e)	fprintf(fp, "startaudit 0 font %d pcolor %u \"%s\" px %u py %u auditdone\r\n", (a), (b), (c), (d), (e))

#define	PAGE_BREAK	fprintf(fp, "pagebreak\r\n")

#define	COLUMNS(a)	fprintf(fp, "columns %d\r\n", (a))

#define	LINESPACE(a)	fprintf(fp, "linespace %d\r\n", (a))

#define	PROMPT_JUSTIFY	fprintf(fp, "padjust %d\r\n", (a))

#define	AUDIT_JUSTIFY	fprintf(fp, "aadjust %d\r\n", (a))
#endif

#endif
