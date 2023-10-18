/**
*** file: filesys.h
**/
#ifndef __FILESYS_H__
#define __FILESYS_H__

#define SECTOR_SIZE		128	/* 32 bit words per sector */
//#define SECTORS_PER_CLUSTER	4	/* number of sectors per cluster */
#define SECTORS_PER_CLUSTER	8	/* number of sectors per cluster */
#define CLUSTER_SIZE		(SECTOR_SIZE*SECTORS_PER_CLUSTER)

/* the info sector -- defined: filesys.h filesys.inc bootrom.asm */
#define INFO_SECTOR		2	/* logical sector of info block */

#define CLUSTER_CHAIN_END		0
#define CLUSTER_UNUSED			1
#define CLUSTER_UNUSABLE		2
#define CLUSTER_FIRST_USER		3		/* first valid cluster */

#define ROOT_DIR_CLUSTER	CLUSTER_FIRST_USER

#define FILE_NAME_SIZE			12
#define FILE_NAME_PADDING		(FILE_NAME_SIZE&3)	/* to make it 32 bit mul */

/*
** INFOSEC_ -- 32 bit words -- documented: filesys.h filesys.inc bootrom.asm
**
** JFL 11 Jan 95
** JFL 16 Jan 96; added infosec header sum
** JFL 20 May 96; unlock sector
*/
enum {
	INFOSEC_MAGIC,		  	/* magic number */
	INFOSEC_SECTORS_PER_CLUSTER,	/* sectors per cluster */
	INFOSEC_FIRST_FAT_SECTOR,   	/* first FAT sector */
	INFOSEC_ROOT_DIR_SECTOR,    	/* root dir sector */
	INFOSEC_FAT_TYPE,	    	/* FAT type */
	INFOSEC_BOOT_CHS,	    	/* boot chs (chigh.8:clow.8:head.4:snum.8) */
	INFOSEC_BOOT_LEN,	    	/* boot sectors 1..256 (where 0==256) */
	INFOSEC_BOOT_ADR,	    	/* dsp load & run adr */
	INFOSEC_BOOT_SUM,	    	/* boot sum */
	INFOSEC_HEADER_SUM,	    	/* sum of all up to (not incl) here */
	INFOSEC_UNLOCK_SECTOR,	    	/* enable writing !not in bootrom.asm! */
	INFOSEC_UNLOCK_VERIFY	    	/* enable writing !not in bootrom.asm! */
};

/*
** DiskRec
**
**	WARNING: must match DISK_ in filesys.inc
**
** JFL 28 Apr 94
*/
typedef struct {
	unsigned long	bytes;
	unsigned long	sectors;
	unsigned long	heads;
	unsigned long	cylinders;
	unsigned long	clusters;
	unsigned long	rootsector;	/* sector of root and where clusters start */
	unsigned long	fatsector;	/* sector of fat */
	unsigned long	lastdircluster;	/* last dir cluster */
	unsigned long	unlocksector;	/* write enable */
} DiskRec;

/*
** FSFile
**
**	WARNING: must match FILE_ in filesys.inc
**
** JFL 28 Apr 94
*/
typedef struct {
	/* public */
	unsigned long	pos;   		/* current position of file mark */
	unsigned long	size;  		/* size of file */
	unsigned short	date;
	unsigned short	time;
	unsigned long	attrib;

	/* private */
	unsigned long	cluster; 	/* cluster of next file access */
	unsigned long	off;	 	/* offset of next file access */
	unsigned long	firstcluster;	/* first cluster of file -- helps make seeks faster */
	unsigned long	dircluster; 	/* cluster where dir entry is */
	unsigned long	diroff;	    	/* offset into cluster of dir entry */
	unsigned long	replace;    	/* when writing, dont read in prev data */
	unsigned long	dirty;	    	/* header is dirty & needs to be re-written */
} FSFile;

/*
** FSDir
**
**    WARNING: must match FSDIR_ in filesys.inc
**
** JFL 02 Feb 95
*/
typedef struct {
	/* public */
	unsigned long	size;
	unsigned long	date;
	unsigned long	cluster;
	unsigned long	flags;
	char	name[FILE_NAME_SIZE+FILE_NAME_PADDING];

	/* private */
	unsigned long	curcluster;
	unsigned long	curoff;
} FSDir;

typedef struct ffblk
{
	char				ff_reserved[21];
	unsigned char	ff_attrib;
	unsigned short	ff_ftime;
	unsigned short	ff_fdate;
	unsigned long	ff_fsize;
	char				ff_name[16];
} ffblk_t;

typedef struct diskfree
{
	unsigned short	total_clusters;
	unsigned short	avail_clusters;
	unsigned short	sectors_per_cluster;
	unsigned short	bytes_per_sector;
} diskfree_t;

/*
** externs
*/

int FSInit(void);
int FSFinal(void);
int FSReset(void);
int FSFormat(void);
int FSOpen(FSFile *, char *);
int FSOpenDirect(FSFile *, unsigned long, unsigned long);
int FSReopen(FSFile *);
int FSDate(FSFile *, unsigned long *);
int FSCreate(FSFile *, char *, unsigned long, unsigned char);
int FSDateFlags(char *, unsigned long, unsigned char);
int FSSetFtime(FSFile *, unsigned long);
int FSDelete(char *);
int FSSize(FSFile *, unsigned long *);
int FSSeek(FSFile *, unsigned long, unsigned long *);
int FSRead(FSFile *, unsigned long *, unsigned long);
int FSWrite(FSFile *, unsigned long *, unsigned long);
int FSRename(char *, char *);
int FSFlush(void);
int FSClose(FSFile *);
int FSDirFirst(FSDir *);
int FSDirNext(FSDir *);
int FSWriteBoot(unsigned long *, unsigned long);
int FSDiskUsage(unsigned long *, unsigned long *);
int FSSupervisor(unsigned long);
int FSGetDateTime(FSFile *, unsigned short *, unsigned short *);
int FSSetDateTime(FSFile *, unsigned short, unsigned short);
int FSGetAttrib(FSFile *, unsigned char *);
int FSSetAttrib(FSFile *, unsigned char);
int FSGetFFblk(char *, struct ffblk *);
unsigned long FSClusterToSector(unsigned long);
void  FSBufSum(unsigned long *, unsigned long, unsigned long *, unsigned long *);
unsigned long FSVersion(void);
int FSGetDiskFree(diskfree_t *);

extern DiskRec Disk;

/*
** ERRORS
**
** -1.....-8999 user
** -9000..-9099 system
** -9200..-9299 file
** -9300..-9399 memory
** -9400..-9499 network
** -9500..-9599 video
** -9600..-9699 process
*/

enum /* int16 */
{
	/* system */
	ERR_SYS_LAST=-9099,
	ERR_USERABORT,
	ERR_SYS_FIRST=-9000,

	/* file */
	ERR_FILE_LAST=-9299,
	ERR_FILE_EOF,
	ERR_FILE_SEEK,
	ERR_FILE_SIZE,
	ERR_FILE_READ,
	ERR_FILE_WRITE,
	ERR_FILE_OPEN,
	ERR_FILE_EXIST,
	ERR_FILE_DELETE,
	ERR_FILE_GENERAL,
	ERR_FILE_NOTFOUND,
	ERR_FILE_INVALID,
	ERR_FILE_DISKFULL,
	ERR_FILE_RENAME,
	ERR_FILE_INIT0,
	ERR_FILE_INIT1,
	ERR_FILE_INIT2,
	ERR_FILE_INIT3,
	ERR_FILE_INIT4,
	ERR_FILE_INIT5,
	ERR_FILE_INIT6,
	ERR_FILE_INIT7,
	ERR_FILE_INIT8,
	ERR_FILE_INIT9,
	ERR_FILE_FIRST=-9200,

	/* memory */
	ERR_MEM_LAST=-9399,
	ERR_MEM_ALLOC,
	ERR_MEM_CORRUPT,
	ERR_MEM_INTERNAL,
	ERR_MEM_CHECK,
	ERR_MEM_AVAIL,
	ERR_MEM_AVAIL_LOW,
	ERR_MEM_ALREADYFREE,
	ERR_MEM_FIRST=-9300,

	/* network */
	ERR_NET_LAST=-9499,
	ERR_NET_EMPTY,
	ERR_NET_RANGE,
	ERR_NET_OVERFLOW,
	ERR_NET_CORRUPT,
	ERR_NET_TIMEOUT,
	ERR_NET_RETRYFAIL,
	ERR_NET_FIRST=-9400,

	/* video */
	ERR_VID_LAST=-9599,
	ERR_VID_FIRST=-9500,

	/* process */
	ERR_PROC_LAST=-9699,
	ERR_PROC_FIRST=-9600,

	/* setup -- protection */
	ERR_SETUP_LAST=-9799,
	ERR_SETUP_USR,
	ERR_SETUP_ENV,
	ERR_SETUP_MISMATCH,
	ERR_SETUP_FIRST=-9700,

	/* disk raw */
	ERR_DISKRAW_LAST=-9899,
	ERR_DISKRAW_READ,
	ERR_DISKRAW_WRITE,
	ERR_DISKRAW_COLDSTART,
	ERR_DISKRAW_FIRST=-9800,

	/* host->target */
	ERR_HOST_LAST=-9999,
	ERR_HOST_TIMEOUT,
	ERR_HOST_FIRST=-9900,

};

#endif /* ndef FILESYS_H */
