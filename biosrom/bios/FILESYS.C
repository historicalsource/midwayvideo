/*
** file: filesys.c
**
** This is Joe Linhoff's original file system code.  It is really dumb in
** that it can not deal with multiple drives (physical and/or logical) and
** it is very inefficient.  It also does NOT dynamically adjust the cluster
** size based on the size of the disk.  How stupid!!!  Also, subdirectories
** are NOT supported!!
**
** $Revision: 4 $
*/
#include <filesys.h>
#include	<ide.h>

//#define	FS_DEBUG

#ifdef FS_DEBUG
void dputs(char *);
void dphex(int);
#endif

#define VERSION (0x26)

/*
** NOTES:
**
** DISK ORGANIZATION BY SECTORS (documented here only!)
** ----------------------------------------------------
** (hard) 0-1 -- nothing
** (hard) 2 -- info sector -- see INFOSEC_
** (soft) 3 -- first boot sector
** (soft) 16 -- first fat sector
*/

/*
** defines
*/

/* boot defines */
#define FMT_BOOT_SEC				3			/* logical sector of boot code */
#define FMT_BOOT_CHS				0x4		/* cyl.16:head.4:sector.8 of boot code */
#define FMT_BOOT_LEN				64			/* sectors of boot code */
#define FMT_BOOT_ADR				0x18000	/* boot code load & run adr -- see BOOTFRAM note in boot.asm */

/* sector defines */
#define FMT_FIRSTFAT_SECTOR	FMT_BOOT_LEN+FMT_BOOT_SEC /* first sector of fat info */

/* misc defines */
#define FATCLUST_SIZE		2	/* 2 cluster entries will fit into 1 32 bit word */
#define S_FATCLUST_SIZE		1	/* shift to find 32 bit word offset */
#define M_FATCLUST_SIZE		1	/* mask to find 32 bit word offset */
#define FS_FAT_TYPE_16BIT	16	/* id for 16 bit per cluster entry in fat */
#define SECTORS_PER_FAT		4	/* sectors per fat table */
#define DIR_RESERVE 			1	/* words reserved at the end of a dir cluster */
#define FS_MAGIC_NUMBER		0x34011280 /* defined here & in boot.asm only! */
#define DIR_LINK (CLUSTER_SIZE-1)
#define sizeof32(_x_) (sizeof(_x_)/sizeof(unsigned long))
#define FSB_BUF_COUNT		32	/* number of fsb buffers */

/*
** FDirEntry
**
**	A dir cluster is an array of these.  Deleted entries are marked by
** zeroing the nm[0] field.  The whole cluster must be searched for a file
** to make it not necessary to re-pack after deletion.
**
** JFL 27 Apr 94
*/
typedef struct {
	unsigned long	nm[3];			/* packed ascii -- left justified, empty are 0 */
	unsigned long	size;				/* in 32 bit words */
	unsigned long	date;				/* last mod -- yr.6:mth.4:day.5:hour.5:min.6:sec.6 */
	unsigned long	flagcluster;	/* flag.8:0.8:cluster.16 */
} FDirEntry;

/*
** externs
*/

extern short SecReads(unsigned long sec,unsigned long *buf,unsigned long num);
extern short SecWrites(unsigned long sec,unsigned long *buf,unsigned long num);

/*
** PROTOTYPES
*/

static int	fsFormatInfo(unsigned long *sectors_p,unsigned long *clusters_p,unsigned long *sectorsforfat_p);
static unsigned long	fsClusterToFATSector(unsigned long cluster,unsigned long *firstcluster_p);
static void	fsNameToNM(char *name,unsigned long *nm);
static void	fsNMToName(unsigned long *nm,char *name);
static void	copy32(void *src,void *dst,unsigned long size);
static void	clear32(unsigned long *, unsigned long);
static void	set32(unsigned long *, unsigned long, unsigned long);
static unsigned long	fsClusterToFATSector(unsigned long cluster,unsigned long *firstcluster_p);
static int		fsNMCmp(unsigned long *nm1,unsigned long *nm2);
static int	fsClusterWrite(unsigned long cluster,unsigned long *buf);
static void	fsResetPrivates(void);

/*
** Globals
*/

DiskRec Disk;

/*
** Privates
*/

static unsigned long fatAllocNext; /* next cluster to alloc */
static unsigned long fdNextCluster; /* next dir cluster to search */

/*
** ---------------------------------------------------------------------
** misc
** ---------------------------------------------------------------------
*/

static void copy32(void *src, void *dst, register unsigned long size)
{
	register unsigned long	*s asm("$8") = (unsigned long *)src;
	register unsigned long	*d asm("$9") = (unsigned long *)dst;
	
	while(size--)
	{
		*d++ = *s++;
	}
} /* copy32() */

static void clear32(register unsigned long *dst, register unsigned long size)
{
	while(size--)
	{
		*dst++ = 0;
	}
} /* clear32() */

static void set32(register unsigned long *dst, register unsigned long size, register unsigned long val)
{
	while(size--)
	{
		*dst++ = val;
	}
} /* clear32() */

/*
** ---------------------------------------------------------------------
** buffer system
** ---------------------------------------------------------------------
*/

/*
** FSB
** file system buffer
**
** note: it is up to the caller to set the pri & dirty fields
**
** JFL 20 Jan 95; added pri
*/
typedef struct
{
	unsigned long	cluster;
	unsigned long	sec;
	unsigned long	dirty;	/* dirty flag -- value is added into pri when searching */
	unsigned long	pri;		/* priority .. higher is less volatile */
	unsigned long	buf[CLUSTER_SIZE];
} FSB;

#define FSBPRI_SYS	3	/* system pri */
#define FSBDIRTY_SYS	1	/* added into pri */
#define FSBPRI_USR	1	/* user pri */
#define FSBDIRTY_USR	3	/* added into pri */

/*
** FSBFAT
** fat buffer
**
** JFL 20 Jan 95
*/
typedef struct
{
	unsigned long	firstcluster;
	unsigned long	sec;
	int				dirty;
	unsigned long	buf[SECTOR_SIZE*SECTORS_PER_FAT];
} FSBFAT;

static FSB		fsb_t[FSB_BUF_COUNT];
static FSBFAT	fatbuf;

/*
** FSBInit
*/
static int FSBInit(void)
{
	register int		i;

	fatbuf.firstcluster = 0;
	fatbuf.dirty = 0;

	for(i= 0; i < FSB_BUF_COUNT; i++)
	{
		fsb_t[i].cluster = 0;	// cluster 0 signals no cluster
		fsb_t[i].dirty = 0;		// Mark clean
		fsb_t[i].pri = 0;			// set to lowest pri
	}
	return 0;
}

/*
** FSBFlushOne
*/
static int FSBFlushOne(FSB *fsb)
{
	int	ec;

	// Is it dirty
	if(!fsb->dirty)
	{
		// NOPE - do nothing
		return(0);
	}

	if((ec = SecWrites(fsb->sec, fsb->buf, SECTORS_PER_CLUSTER)) < 0)
	{
//		return(ec);
	}

	// Mark clean
	fsb->dirty = 0;

	// Return success
	return(0);
}

/*
** fsClusterWrite
*/
static int fsClusterWrite(unsigned long cluster, unsigned long *buf)
{
	return(SecWrites(FSClusterToSector(cluster), buf, SECTORS_PER_CLUSTER));
}

/*
** FSBNew
** find a new fsb to use (flush it if necessary)
** will return with dirty clear
*/
static int FSBNew(FSB **fsb_p)
{
	register int	ec;
	register FSB	*fsb;
	register FSB	*fsb2;
	register int	i;

	fsb = fsb_t;
 	for(i = 1, fsb2 = &fsb_t[1]; i < FSB_BUF_COUNT; i++, fsb2++)
	{
		/* find lowest pri */
		if((fsb2->pri + fsb2->dirty) < (fsb->pri + fsb->dirty))
		{
			fsb = fsb2;
		}
	}

	// flush old one if it needs it
	if((ec = FSBFlushOne(fsb)) < 0)
	{
		return(ec);
	}

	*fsb_p = fsb;
	return(0);
}

/*
** FSBGet
** read a cluster or find the fsb with the valid data in it
*/
static int FSBGet(unsigned long cluster, FSB **fsb_p)
{
	register int	ec;
	register int	i;
	FSB				*fsb = fsb_t;

	for(i = 0; i < FSB_BUF_COUNT; i++)
	{
		if(fsb->cluster == cluster)
		{
			*fsb_p = fsb;
			return(0);
		}
		fsb++;
	}

	if((ec = FSBNew(&fsb)) < 0)
	{
		return(ec);
	}

	// read in new cluster
	fsb->cluster = cluster;
	fsb->sec = FSClusterToSector(fsb->cluster);
	if((ec = SecReads(fsb->sec, fsb->buf, SECTORS_PER_CLUSTER)) < 0)
	{
		return(ec);
	}
	*fsb_p = fsb;
	return(0);
}

/*
** FSBFATFlushOne
*/
static int FSBFATFlushOne(void)
{
	register int	ec;

	if(!fatbuf.dirty)
	{
		return(0);
	}

	// write a sectors worth of data
	if((ec = SecWrites(fatbuf.sec, fatbuf.buf, SECTORS_PER_FAT)) < 0)
	{
//		return(ec);
	}

	fatbuf.dirty = 0;
	return(0);
}

/*
** FSBFATGet
*/
static int FSBFATGet(unsigned long cluster, FSBFAT **fat_p)
{
	register int	ec;

	if(fatbuf.firstcluster &&
		(cluster >= fatbuf.firstcluster) && 
		(cluster < fatbuf.firstcluster + (FATCLUST_SIZE * SECTOR_SIZE * SECTORS_PER_FAT)))
	{
		*fat_p = &fatbuf;
	}
	else
	{
		if((ec = FSBFATFlushOne()) < 0)
		{
			return(ec);
		}
		fatbuf.sec = fsClusterToFATSector(cluster, &fatbuf.firstcluster);
		if((ec = SecReads(fatbuf.sec, fatbuf.buf, SECTORS_PER_FAT)) < 0)
		{
			return(ec);
		}
	}
	*fat_p = &fatbuf;
	return(0);
}

/*
** FSBFlush
** flush both normal and fat buffers
*/
static int FSBFlush(void)
{
	register int	ec;
	register int	i;
	register FSB	*fsb = fsb_t;

	i = FSB_BUF_COUNT;
	while(i--)
	{
		if((ec = FSBFlushOne(fsb)) < 0)
		{
			return(ec);
		}
		++fsb;
	}

	if((ec = FSBFATFlushOne()) < 0)
	{
		return(ec);
	}

	return(0);
}

/*
** ---------------------------------------------------------------------
** file allocation table
** ---------------------------------------------------------------------
*/

/*
** FATMark
** change the fat table for <cluster> to <mark>
**
** note: 16 bit word ordering: BBBBAAAA
** where cluster AAAA comes before cluster BBBB
*/
static int FATMark(unsigned long cluster, unsigned long mark)
{
	register int				ec;
	register unsigned long	off;
	register unsigned long	*buf;
	register unsigned long	x;
	FSBFAT						*fat;
	
	if((ec = FSBFATGet(cluster, &fat)) < 0)
	{
		return(ec);
	}
	off = cluster - fat->firstcluster;
	buf = &fat->buf[off >> S_FATCLUST_SIZE];
	x = *buf;
	if((off & M_FATCLUST_SIZE) != 0)
	{
		mark <<= 16;	/* move mark to top */
		x &= 0xFFFF;	/* zero out top */
	}
	else
	{
		x &= ~0xFFFF;	/* zero out bottom */
	}
	x |= mark;
	*buf = x;
	fat->dirty = 1;
	return(0);
}

/*
** FATValue
*/
static int FATValue(unsigned long cluster, unsigned long *value_p)
{
	register int				ec;
	register unsigned long	off;
	register unsigned long	*buf;
	register unsigned long	x;
	FSBFAT						*fat;
	
	if((ec = FSBFATGet(cluster, &fat)) < 0)
	{
		return(ec);
	}
	off = cluster - fat->firstcluster;
	buf = &fat->buf[off >> S_FATCLUST_SIZE];
	x = *buf;
	if((off & M_FATCLUST_SIZE) != 0)
	{
		x >>= 16;		/* move data down */
	}
	else
	{
		x &= 0xFFFF;	/* zero top */
	}
	*value_p = x;
	return(0);
}

/*
** FATAlloc
** find an unused cluster and allocate it
*/
static int FATAlloc(unsigned long *cluster_p)
{
	register int				ec;
	register unsigned long	off;
	register unsigned long	*buf;
	register unsigned long	x;
	register unsigned long	cluster;
	register unsigned long	num;
	register int				i;
	FSBFAT						*fat;

	for(;;)
	{
		for(cluster = fatAllocNext; cluster < Disk.clusters;)
		{
			if((ec = FSBFATGet(cluster,&fat)) < 0)
			{
				return(ec);
			}

			off = cluster - fat->firstcluster;
			if((off & M_FATCLUST_SIZE) != 0)
			{
				/* cluster must start at first valid */
				cluster--;
				off = cluster - fat->firstcluster;
			}

			num = SECTOR_SIZE * SECTORS_PER_FAT * FATCLUST_SIZE / 2;
			num -= off >> S_FATCLUST_SIZE;
			buf = &fat->buf[off >> S_FATCLUST_SIZE];

			for(i = 0; i < num; i++, buf++)
			{
				x = *buf & 0xFFFF;
				if(x == CLUSTER_UNUSED)
				{
					*buf &= ~0xFFFF;	/* clear out bottom */
					fatAllocNext = cluster + 1;
					*cluster_p = cluster;
					fat->dirty = 1;
					return(0);
				}
				cluster++;
				x = *buf >> 16;
				if(x == CLUSTER_UNUSED)
				{
					*buf &= 0xFFFF;	/* clear out top */
					fatAllocNext = cluster + 1;
					*cluster_p = cluster;
					fat->dirty = 1;
					return(0);
				}
				cluster++;
			}
		}
		if(fatAllocNext == CLUSTER_FIRST_USER)
		{
			return(ERR_FILE_DISKFULL);
		}
		fatAllocNext=CLUSTER_FIRST_USER;
	}
}

/*
** FATChainFree
*/
static int FATChainFree(unsigned long cluster)
{
	register int	ec;
	unsigned long	chain;
	
	if(cluster < CLUSTER_FIRST_USER)
	{
		return(1);
	}

	chain = CLUSTER_FIRST_USER;
	while(chain >= CLUSTER_FIRST_USER)
	{
		if((ec = FATValue(cluster, &chain)) < 0)
		{
			return(ec);
		}
		if((ec = FATMark(cluster, CLUSTER_UNUSED)) < 0)
		{
			return(ec);
		}
		cluster = chain;
	}
	return(0);
}

/*
** FATAllocLinearClusters
** find a block of contiguous, free clusters
*/
static long FATAllocLinearClusters(unsigned long size, unsigned long *first_p)
{
	register int				ec;
	register unsigned long	cluster;
	register unsigned long	num;
	unsigned long				first;
	unsigned long				tmp;

	/* check if its too small for this routine */
	if(size < CLUSTER_SIZE)
	{
		return(0);
	}
	/* alloc 1st */
	if((ec = FATAlloc(&first)) < 0)
	{
		return(ec);
	}
	num = 1;						/* one cluster allocated */
	size -= CLUSTER_SIZE;	/* remaining size */
	cluster = first;
	while(size >= CLUSTER_SIZE)
	{
		/* check on status of next cluster */
		if((ec = FATValue(cluster+1, &tmp)) < 0)
		{
			return(ec);
		}
		/* if we run into anything other than unused, break */
		if(tmp != CLUSTER_UNUSED)
		{
			break;
		}
		/* alloc & link cluster */
		if((ec = FATMark(cluster, cluster+1)) < 0)
		{
			return(ec);
		}
		num++;						/* one more */
		cluster++;					/* next cluster */
		size -= CLUSTER_SIZE;	/* remaining size */
	}
	if(num)
	{		
		/* alloc last (so far) cluster */
		if((ec = FATMark(cluster, CLUSTER_CHAIN_END)) < 0)
		{
			return(ec);
		}
		*first_p = first;	/* first cluster of list */
		return(num);
	}
	return(0);
}

/*
** FATLinearClusters
** given a starting cluster and word size find the number of clusters
** that form a contiguous list of sectors
*/
static long FATLinearClusters(unsigned long cluster, unsigned long size, unsigned long *first_p, unsigned long *next_p)
{
	register int				ec;
	register unsigned long	first = cluster;
	register unsigned long	num = 0;
	unsigned long	chain;

	while(size >= CLUSTER_SIZE)
	{
		/* add up */
		num++;						/* one more cluster in chain */
		size -= CLUSTER_SIZE;	/* subtract out size for this cluster */

		/* link to next cluster */
		if((ec = FATValue(cluster, &chain)) < 0)
		{
			return(ec);
		}
		/* signals end of file -- check for exact end of file */
		if(chain < CLUSTER_FIRST_USER && size)
		{
			return(ERR_FILE_EOF);
		}
		/* check if link is next cluster */
		if(chain != (cluster + 1))
		{
			break;
		}
		cluster = chain;
	}
	if(num)
	{
		/* pass back to caller */
		*first_p = first;
		*next_p = chain;
	}
	return(num);
}

/*
** ---------------------------------------------------------------------
** file directory system
** ---------------------------------------------------------------------
*/

/*
** FDFind
** find the directory slot of the file <nm>
*/
static int FDFind(unsigned long *nm, unsigned long *cluster_p, unsigned long *off_p)
{
	register int				ec;
	register FDirEntry		*de;
	register unsigned long	i = fdNextCluster;
	register unsigned long	num;
	register unsigned long	off;
	FSB							*fsb;

	while(1)
	{
		/* get dir for this cluster */
		if((ec = FSBGet(i, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		/* search the whole cluster for a match -- there may be deletion holes */
		off = 0;
		de = (FDirEntry *)fsb->buf;
		num = (CLUSTER_SIZE - DIR_RESERVE) / sizeof32(FDirEntry);
		for(i = 0; i < num; i++)
		{
			if(fsNMCmp(nm, de->nm))
			{
				*cluster_p = fdNextCluster = fsb->cluster;
				*off_p = off;
				return(0);
			}
			/* offset in 32 bit units */
			off += sizeof32(FDirEntry);
			de++;
		}
		if((i = fsb->buf[DIR_LINK]) == 0)
		{
			i=ROOT_DIR_CLUSTER;
		}
		if(i == fdNextCluster)
		{
			return(1); /* no match found */
		}
	}
}

/*
** FDReplace
** add a new file to the current directory
*/
static int FDReplace(unsigned long *nm, unsigned long *cluster_p, unsigned long *off_p)
{
	register int				ec;
	register FDirEntry		*de;
	register unsigned long	cluster = Disk.lastdircluster;
	register unsigned long	num;
	register unsigned long	off;
	register unsigned long	free;
	unsigned long				i;
	FSB							*fsb;

	/* get root dir */
	for(;;)
	{
		/* load in dir cluster */
		if((ec = FSBGet(cluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		free = 0;
		off = 0;
		de = (FDirEntry *)fsb->buf;
		num = (CLUSTER_SIZE - DIR_RESERVE) / sizeof32(FDirEntry);
		for(i = 0; i < num; i++)
		{
			/* first unused or match */
			if(!de->nm[0])
			{
				*cluster_p = fsb->cluster;
				*off_p = off;
				if(free)
				{
					if((ec = FATChainFree(free)) < 0)
					{
						return(ec);
					}
					fsResetPrivates();
				}
				return(0);
			}
			if(fsNMCmp(nm, de->nm))
			{
				/* replace this file */
				free = de->flagcluster & 0xFFFF;
				*cluster_p = fsb->cluster;
				*off_p = off;
				if(free)
				{
					if((ec = FATChainFree(free)) < 0)
					{
						return(ec);
					}
					fsResetPrivates();
				}
				return(0);
			}
			/* offset in 32 bit units */
			off += sizeof32(FDirEntry);
			de++;
		} /* for */
		/* no match & no room, check if there is a link to another cluster */
		if((i = fsb->buf[DIR_LINK]) != 0)
		{
			cluster = i;
			continue;
		}
		/* create another cluster to link in the dir chain */
		if((ec = FATAlloc(&i)) < 0)
		{
			return(ec);
		}
		/* load in prev dir cluster & link to new dir cluster */
		if((ec = FSBGet(cluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		fsb->buf[DIR_LINK] = i;		/* set link */
		fsb->dirty = FSBDIRTY_SYS;	/* signal cluster is dirty */
		
		/* load & clear new cluster */
		cluster = i;
		if((ec = FSBGet(cluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		clear32((unsigned long *)fsb->buf, CLUSTER_SIZE);
		fsb->dirty = FSBDIRTY_SYS;	/* signal cluster is dirty */

		/* set so new creates start at this cluster */
		Disk.lastdircluster = cluster;
	}
}

/*
** ---------------------------------------------------------------------
** file system
** ---------------------------------------------------------------------
*/

/*
** fsResetPrivates
*/
static void fsResetPrivates(void)
{
	fatAllocNext = CLUSTER_FIRST_USER;	/* next cluster alloc */
	fdNextCluster = ROOT_DIR_CLUSTER;	/* next dir search cluster */
	Disk.lastdircluster = ROOT_DIR_CLUSTER;
}

/*
** FSInit
*/
int FSInit(void)
{
	register int 	ec;
	register int 	fail;
	unsigned long	sum;
	unsigned long	seed;
	unsigned long	sectors;
	unsigned long	clusters;
	unsigned long	sectorsforfat;
	FSB			 	*fsb;

	/*
	** INIT PRIVATES
	*/
#ifdef FS_DEBUG
	dputs("FSInit() - Entry\n");
#endif
	fsResetPrivates();
	fail = 0;
#ifdef FS_DEBUG
	dputs("Calling FSBInit\n");
#endif
	if((ec = FSBInit()) < 0)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - FSBInit Failure\n");
#endif
		return(ec);
	}
#ifdef FS_DEBUG
	dputs("Calling fsFormatInfo\n");
#endif
	/* compute sectors and clusters on disk */
	if((ec = fsFormatInfo(&sectors, &clusters, &sectorsforfat)) < 0)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - fsFormatInfo Failure\n");
#endif
		return(ec);
	}

	Disk.clusters = clusters;
	Disk.lastdircluster = ROOT_DIR_CLUSTER;
#ifdef FS_DEBUG
	dputs("Calling FSBNew\n");
#endif
	if((ec = FSBNew(&fsb)) < 0)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - FSBNew Failure\n");
#endif
		return(ec);
	}
#ifdef FS_DEBUG
	dputs("Calling SecReads (INFO Sector)\n");
#endif
	/* read in the info sector -- see INFOSEC_ enum above */
	if((ec = SecReads(INFO_SECTOR,fsb->buf,1)) < 0)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - SecReads Failure\n");
#endif
		return(ec);
	}
	if(fsb->buf[INFOSEC_MAGIC] != FS_MAGIC_NUMBER)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - Bad FS_MAGIC_NUMBER\n");
#endif
		fail = 1;
		ec = ERR_FILE_INIT1;
	}
	if(fsb->buf[INFOSEC_SECTORS_PER_CLUSTER] != SECTORS_PER_CLUSTER)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - Bad SECTORS_PER_CLUSTER\n");
#endif
		fail = 1;
		ec = ERR_FILE_INIT2;
	}
	/* quick sum */
	sum = 0;
	seed = 0;
#ifdef FS_DEBUG
	dputs("Calling FSBufSum\n");
#endif
	FSBufSum(fsb->buf, INFOSEC_HEADER_SUM, &sum, &seed);
	if(fsb->buf[INFOSEC_HEADER_SUM] != sum)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - Bad INFO Sector Checksum\n");
#endif
		fail = 1;
		ec = ERR_FILE_INIT3;
	}

	Disk.fatsector = fsb->buf[INFOSEC_FIRST_FAT_SECTOR];
	Disk.rootsector = Disk.fatsector + sectorsforfat;
	Disk.unlocksector = Disk.rootsector; /* default unlock after FAT */

	/* unlock sectors for writing */
	sum = fsb->buf[INFOSEC_UNLOCK_VERIFY] ^ 0xffffffff;
	if(sum == fsb->buf[INFOSEC_UNLOCK_SECTOR])
	{
		Disk.unlocksector = sum; /* unlock as requested */
	}

	if(fsb->buf[INFOSEC_ROOT_DIR_SECTOR] != Disk.rootsector)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - Bad Rootsector number\n");
#endif
		fail = 1;
		ec = ERR_FILE_INIT4;
	}
	if(fsb->buf[INFOSEC_FAT_TYPE] != FS_FAT_TYPE_16BIT)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - Bad FS_FAT_TYPE_16BIT\n");
#endif
		fail = 1;
		ec = ERR_FILE_INIT5;
	}
	if(fail)
	{
		return(ec);
	}

	/* set user mode */
#ifdef FS_DEBUG
	dputs("Calling FSSupervisor\n");
#endif
	if((ec = FSSupervisor(0)) < 0)
	{
#ifdef FS_DEBUG
		dputs("FSInit() - FSSupervisor failure\n");
#endif
		return(ec);
	}
#ifdef FS_DEBUG
	dputs("FSInit() - OK\n");
#endif
	return(0);
}

/*
** FSReset
** called to reset the fs/buffers/etc without writing anything to disk
*/
int FSReset(void)
{
	return(FSInit());
}

/*
** FSFinal
*/
int FSFinal(void)
{
	return(0);
}

/*
** FSVersion
*/
unsigned long FSVersion(void)
{
	return(VERSION);
}

/*
** fsWriteInfoSec
*/
static int fsWriteInfoSec(unsigned long bootsum)
{
	register int				ec;
	register unsigned long	*buf;
	unsigned long				sectors;
	unsigned long				clusters;
	unsigned long				sectorsforfat;
	unsigned long				seed;
	unsigned long				headersum;
	FSB							*fsb;

	if((ec = fsFormatInfo(&sectors, &clusters, &sectorsforfat)) < 0)
	{
		return(ec);
	}
	if((ec = FSBNew(&fsb)) < 0)
	{
		return(ec);
	}

	Disk.lastdircluster = ROOT_DIR_CLUSTER;
	Disk.fatsector = FMT_FIRSTFAT_SECTOR;
	Disk.rootsector = Disk.fatsector + sectorsforfat;

	/* setup & write out id sector */
	buf = fsb->buf;
	clear32((unsigned long *)buf, SECTOR_SIZE);
	buf[INFOSEC_MAGIC] = FS_MAGIC_NUMBER; /* magic number */
	buf[INFOSEC_SECTORS_PER_CLUSTER] = SECTORS_PER_CLUSTER;
	buf[INFOSEC_FIRST_FAT_SECTOR] = Disk.fatsector;
	buf[INFOSEC_ROOT_DIR_SECTOR] = Disk.rootsector;
	buf[INFOSEC_FAT_TYPE] = FS_FAT_TYPE_16BIT;
	buf[INFOSEC_BOOT_CHS] = FMT_BOOT_CHS;
	buf[INFOSEC_BOOT_LEN] = FMT_BOOT_LEN;
	buf[INFOSEC_BOOT_ADR] = FMT_BOOT_ADR;
	buf[INFOSEC_BOOT_SUM] = bootsum;
	buf[INFOSEC_UNLOCK_SECTOR] = 0xffffffff; /* invalidate to force default */
	buf[INFOSEC_UNLOCK_VERIFY] = 0xffffffff; /* invalidate to force default */

	/* sum of header */
	headersum = 0;
	seed = 0;
	FSBufSum(buf, INFOSEC_HEADER_SUM, &headersum, &seed);

	buf[INFOSEC_HEADER_SUM] = headersum;
	if((ec = SecWrites(INFO_SECTOR, fsb->buf, 1)) < 0)
	{
		return(ec);
	}

	fsb->dirty = fsb->cluster = 0; /* signal no data */
	return(0);
}

/*
** FSFormat
*/
int FSFormat(void)
{
	register int				ec;
	register unsigned long	*buf;
	register int				i;
	register unsigned long	len;
	register unsigned long	sec;
	unsigned long				sectors;
	unsigned long				clusters;
	unsigned long				sectorsforfat;
	FSB							*fsb;

	if((ec = fsFormatInfo(&sectors, &clusters, &sectorsforfat)) < 0)
	{
		return(ec);
	}
	if((ec = fsWriteInfoSec ( 0)) < 0)
	{
		return(ec);
	}
	if((ec = FSBNew(&fsb)) < 0)
	{
		return(ec);
	}
	/* fill sector buffer with all unused */
	buf = fsb->buf;
	for(i = 0; i < CLUSTER_SIZE; i++)
	{
		*buf++ = CLUSTER_UNUSED | CLUSTER_UNUSED << 16;
	}

	/* write out all the fats */
	sec = Disk.fatsector;
	for(i = 0; i < sectorsforfat;)
	{
		if(!(len = sectorsforfat - i))
		{
			break;
		}
		if(len > SECTORS_PER_CLUSTER)
		{
			len = SECTORS_PER_CLUSTER;
		}
		if((ec = SecWrites(sec, fsb->buf, len)) < 0)
		{
			return(ec);
		}
		i += len;
		sec += len;
	}

	/* create root directory */
	clear32((unsigned long *)fsb->buf, CLUSTER_SIZE);
	if((ec = SecWrites(Disk.rootsector, fsb->buf, SECTORS_PER_CLUSTER)) < 0)
	{
		return(ec);
	}

	/* make sure this isnt used */
	fsb->sec = 0;
	fsb->cluster = 0;
	fsb->dirty = 0;

	/* mark root dir cluster as being used */
	if((ec = FATMark(ROOT_DIR_CLUSTER, CLUSTER_CHAIN_END)) < 0)
	{
		return(ec);
	}
	FSBFlush();
	fsResetPrivates();
	return(0);
}

/*
** FSBufSum
** simple checksum routine -- can be continued across buffers
*/
void FSBufSum(unsigned long *buf, unsigned long size, unsigned long *sum_p, unsigned long *seed_p)
{
	register unsigned long	sum = *sum_p;
	register unsigned long	seed = *seed_p;
	register unsigned long	i;
	register unsigned long	x;

	if(!seed)
	{
		seed = 0x5a57313c;
	}
	for(i = 0; i < size; i++)
	{
		sum ^= *buf++;
		sum ^= seed;
		if((sum & 1) == 0)
		{
			seed ^= 0x5002;
		}
		/* roll sum right 2 */
		x = sum << (32-2);
		sum >>= 2;
		sum |= x;
		/* roll seed left 3 */
		x = seed >> (32-3);
		seed <<= 3;
		seed |= x;
	}
	*sum_p = sum;
	*seed_p = seed;
}


/*
** FSDelete
*/
int FSDelete(char *name)
{
	register int			ec;
	register FDirEntry	*de;
	unsigned long			cluster;
	unsigned long			off;
	unsigned long			nm[3];
	FSB						*fsb;

	/* find the file */
	fsNameToNM(name, nm);
	if((ec = FDFind(nm, &cluster, &off)) < 0)
	{
		return(ec);
	}
	if(ec)
	{
		return(0); /* not found */
	}

	/* zero the nm[0] field to delete the file */
	if((ec = FSBGet(cluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	de = (FDirEntry *)&fsb->buf[off];
	de->nm[0] = 0;
	fsb->dirty = FSBDIRTY_SYS;

	/* find the first cluster in the file cluster chain */
	cluster = de->flagcluster & 0xFFFF;
	if((ec = FATChainFree(cluster)) < 0)
	{
		return(ec);
	}
	if((ec = FSBFlush()) < 0)
	{
		return(ec);
	}

	/* when any files are deleted, restart at root dir */
	Disk.lastdircluster = ROOT_DIR_CLUSTER; /* moved below flush JFL 6/14/96 */
	fatAllocNext=CLUSTER_FIRST_USER;
	return(0);
}

/*
** FSCreate
*/
int FSCreate(FSFile *fh, char *name, unsigned long date, unsigned char flags)
{
	register int				ec;
	register FDirEntry		*de;
	unsigned long				nm[3];
	FSB							*fsb;

	// Convert name to upper case
	ec = 0;
	while(name[ec])
	{
		if(name[ec] >= 'a' && name[ec] <= 'z')
		{
			name[ec] &= ~0x20;
		}
		++ec;
	}

	fsNameToNM(name, nm);
	clear32((unsigned long *)fh, sizeof32(FSFile));
	if((ec = FDReplace(nm, &fh->dircluster, &fh->diroff)) < 0)
	{
		return(ec);
	}
	fh->replace = 1; /* signal we dont need to read in cluster before we write */

	/* find the size of the file & set first cluster */
	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	de = (FDirEntry *)&fsb->buf[fh->diroff];
	clear32((unsigned long *)de, sizeof32(FDirEntry));

	/* copy info in */
	de->nm[0] = nm[0];
	de->nm[1] = nm[1];
	de->nm[2] = nm[2];
	de->date = date;
	de->flagcluster = flags << 24; /* set flags up top and zero cluster */
	fsb->dirty = FSBDIRTY_SYS;
	return(0);
}

/*
** FSOpen
*/
int FSOpen(FSFile *fh, char *name)
{
	register int			ec;
	register FDirEntry	*de;
	unsigned long 			nm[3];
	FSB						*fsb;

	// Convert name to upper case
	ec = 0;
	while(name[ec])
	{
		if(name[ec] >= 'a' && name[ec] <= 'z')
		{
			name[ec] &= ~0x20;
		}
		++ec;
	}

	/* find the dir slot of the file */
	fsNameToNM(name, nm);
	clear32((unsigned long *)fh, sizeof32(FSFile));
	if((ec = FDFind(nm, &fh->dircluster, &fh->diroff)) < 0)
	{
		return(ec);
	}
	if(ec)
	{
		return(ERR_FILE_NOTFOUND);
	}

	/* find the size of the file & set first cluster */
	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	de = (FDirEntry *)&fsb->buf[fh->diroff];
	fh->size = de->size;
	fh->firstcluster = de->flagcluster & 0xffff;
	fh->cluster = fh->firstcluster;
	fh->date = (de->date >> 16);
	fh->time = (de->date & 0xffff);
	fh->attrib = (de->flagcluster >> 24);
	return(0);
}


/*
** FSReopen
** also used when closing to clear all other flags
*/
int FSReopen(FSFile *fh)
{
	FSFile	fhtmp;

	copy32(fh, &fhtmp, sizeof32(FSFile));
	clear32((unsigned long *)fh, sizeof32(FSFile));
	fh->size = fhtmp.size;
	fh->firstcluster = fh->cluster = fhtmp.firstcluster;
	fh->dircluster = fhtmp.dircluster;
	fh->diroff = fhtmp.diroff;
	return 0;
}

/*
** FSRename
*/
int FSRename(char *org, char *n)
{
	register int			ec;
	register FDirEntry	*de;
	FSFile					fsf;
	register FSFile		*fh = &fsf;
	unsigned long			nmorg[3];
	unsigned long			nmnew[3];
	FSB						*fsb;

	/* find the dir slot of the file */
	fsNameToNM(org, nmorg);
	fsNameToNM(n, nmnew);
	clear32((unsigned long *)fh, sizeof32(FSFile));
	if((ec = FDFind(nmorg, &fh->dircluster, &fh->diroff)) < 0)
	{
		return(ec);
	}
	if(ec)
	{
		return(ERR_FILE_NOTFOUND);
	}
	/* find the name entry */
	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	fsb->dirty = FSBDIRTY_SYS;
	de = (FDirEntry *)&fsb->buf[fh->diroff];
	de->nm[0] = nmnew[0];
	de->nm[1] = nmnew[1];
	de->nm[2] = nmnew[2];

	if((ec = FSBFlush()) < 0)
	{
		return(ec);
	}
	return(0);
}

/*
** FSStat() - The the file stats
*/
int FSGetFFblk(char *name, struct ffblk *f)
{
	register int			ec;
	register FDirEntry	*de;
	FSFile					fsf;
	register FSFile		*fh = &fsf;
	unsigned long			nm[3];
	FSB						*fsb;
	char						*tmp;

	/* find the dir slot of the file */
	fsNameToNM(name, nm);
	clear32((unsigned long *)fh, sizeof32(FSFile));
	if((ec = FDFind(nm, &fh->dircluster, &fh->diroff)) < 0)
	{
		return(ec);
	}
	if(ec)
	{
		return(ERR_FILE_NOTFOUND);
	}

	/* find the name entry */
	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}

	de = (FDirEntry *)&fsb->buf[fh->diroff];

	f->ff_attrib = (de->flagcluster >> 24) & 0xff;
	f->ff_ftime = (de->date & 0xffff);
	f->ff_fdate = (de->date >> 16);
	f->ff_fsize = de->size << 2;
	tmp = f->ff_name;
	while(*name)
	{
		*tmp++ = *name++;
	}
	*tmp = 0;

	return(0);
}

/*
** FSDateFlags
*/
int FSDateFlags(char *name, unsigned long date, unsigned char flags)
{
	register int			ec;
	register FDirEntry	*de;
	FSFile					fsf;
	register FSFile		*fh = &fsf;
	unsigned long			nm[3];
	FSB						*fsb;

	/* find the dir slot of the file */
	fsNameToNM(name, nm);
	clear32((unsigned long *)fh, sizeof32(FSFile));
	if((ec = FDFind(nm, &fh->dircluster, &fh->diroff)) < 0)
	{
		return(ec);
	}
	if(ec)
	{
		return(ERR_FILE_NOTFOUND);
	}

	/* find the name entry */
	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	fsb->dirty = FSBDIRTY_SYS;
	de = (FDirEntry *)&fsb->buf[fh->diroff];
	de->date = date;
	de->flagcluster &= 0xffffff;
	de->flagcluster |= flags << 24; /* set flags up top and zero cluster */

	if((ec = FSBFlush()) < 0)
	{
		return(ec);
	}
	return(0);
}


/*
** FSSetFtime
*/
int FSSetFtime(FSFile *fh, unsigned long date)
{
	register int			ec;
	register FDirEntry	*de;
	FSB						*fsb;

	/* find the name entry */
	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	fsb->dirty = FSBDIRTY_SYS;
	de = (FDirEntry *)&fsb->buf[fh->diroff];
	de->date = date;

	if((ec = FSBFlush()) < 0)
	{
		return(ec);
	}
	return(0);
}

int FSGetDateTime(FSFile *fh, unsigned short *date, unsigned short *time)
{
	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	*date = fh->date;
	*time = fh->time;
	return(0);
}

int FSSetDateTime(FSFile *fh, unsigned short date, unsigned short time)
{
	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	fh->date = date;
	fh->time = time;
	fh->dirty = 1;
	return(0);
}

int FSGetAttrib(FSFile *fh, unsigned char *attrib)
{
	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	*attrib = (char)fh->attrib;
	return(0);
}

int FSSetAttrib(FSFile *fh, unsigned char attrib)
{
	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	fh->attrib = (unsigned long)attrib;
	fh->dirty = 1;
	return(0);
}



/*
** FSSize
*/
int FSSize(FSFile *fh, unsigned long *size_p)
{
	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	*size_p = fh->size;
	return(0);
}

/*
** FSSeek
** move the file position and return the old position
*/
int FSSeek(FSFile *fh, unsigned long newpos, unsigned long *old_p)
{
	register int				ec;
	register unsigned long	clusters;
	unsigned long				x;

	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	
	/* are we just getting the position */
	*old_p = fh->pos;
	if(newpos == 0xffffffff)
	{
		return(0);
	}
	if(newpos > fh->size)
	{
		return(ERR_FILE_SEEK);
	}
	if(newpos < fh->pos)
	{
		fh->cluster = fh->firstcluster;
		fh->pos = 0;
	}
	/* figure number of clusters and offset */
	clusters = (newpos - fh->pos) / CLUSTER_SIZE; /* clusters forward */
	fh->off = newpos - ((newpos/CLUSTER_SIZE) * CLUSTER_SIZE);
	fh->pos = newpos;

	/* find first cluster */
	while(clusters > 0)
	{
		if((ec = FATValue(fh->cluster, &x)) < 0)
		{
			return(ec);
		}
		if(x < CLUSTER_FIRST_USER)
		{
			return(ERR_FILE_EOF);
		}
		fh->cluster = x;
		clusters--;
	}
	return(0);
}

/*
** FSWrite
*/
int FSWrite(FSFile *fh, unsigned long *buf, unsigned long size)
{
	register int				ec;
	register unsigned long	len;
	register FDirEntry		*de;
	register unsigned long	x;
	register unsigned long	num;
	unsigned long				cluster;
	FSB							*fsb;

	/* make sure fh is valid */
	if(!fh->dircluster || fh->dircluster == 0xffffffff)
	{
		return(ERR_FILE_INVALID);
	}

	/* check if this is the first cluster */
	if(!fh->cluster)
	{
		/* alloc a new cluster, and fill in the dir */
		if((ec = FATAlloc(&fh->firstcluster)) < 0)
		{
			return(ec);
		}
		if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		de = (FDirEntry *)&fsb->buf[fh->diroff];
		x = de->flagcluster & ~0xffff;
		de->flagcluster = x | fh->firstcluster;
		fsb->dirty = FSBDIRTY_SYS;
		fh->cluster = fh->firstcluster;
		fh->off = 0;
	}
	/* write the data */
	for(;size;)
	{
		/* figure how much to copy */
		len = size;
		if(len > CLUSTER_SIZE - fh->off)
		{
			len = CLUSTER_SIZE - fh->off;
		}

		/* check for end of cluster */
		if(!len)
		{
			/* get next cluster */
			if((ec = FATValue(fh->cluster, &cluster)) < 0)
			{
				return(ec);
			}
			/* signals end of file */
			if(cluster < CLUSTER_FIRST_USER)
			{
				if(size > CLUSTER_SIZE)
				{
					if((ec = FATAllocLinearClusters(size, &cluster)) < 0)
					{
						return(ec);
					}
					num = ec;
					if(!num)
					{
						return(ERR_FILE_GENERAL);
					}
					/* link last cluster in file to the just-allocated linear block */
					if((ec = FATMark(fh->cluster, cluster)) < 0)
					{
						return(ec);
					}
					x = FSClusterToSector(cluster);
					if((ec = SecWrites(x, buf, num * SECTORS_PER_CLUSTER)) < 0)
					{
						return(ec);
					}
					/* adjust pointers */
					fh->cluster = cluster + num - 1; /* last cluster of data */
					fh->off = CLUSTER_SIZE; /* offset into last cluster buf */
					num *= CLUSTER_SIZE; /* transfer size */
					buf = &buf[num]; /* callers buf */
					fh->pos += num; /* file mark */
					size -= num; /* amount remaining */
					continue;

				} /* multiple clusters */
				else
				{
					/* alloc a new cluster */
					if((ec = FATAlloc(&cluster)) < 0)
					{
						return(ec);
					}
					/* link previous cluster to the new cluster */
					if((ec = FATMark(fh->cluster, cluster)) < 0)
					{
						return(ec);
					}
				}
			}
			/* reset file marks */
			fh->cluster = cluster;
			fh->off = 0;
			continue; /* loop back */
		}

		/* get cluster & fill it */
		if((ec = FSBGet(fh->cluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_USR;
		copy32(buf, &fsb->buf[fh->off], len);
		fsb->dirty = FSBDIRTY_USR;

		/* adjust pointers */
		buf = &buf[len]; /* callers buf */
		fh->off += len; /* offset into cluster buffer */
		fh->pos += len; /* file mark */
		size -= len; /* amount remaining */
	}

	/* set size if necessary */
	if(fh->pos > fh->size)
	{
		fh->size = fh->pos;
		fh->dirty = 1;
	}
	return(0);
}

/*
** FSRead
*/
//
// I've modified this function because Joe is such a complete fucking moron.
// His original code would return EOF as soon as it detected that the EOF
// existed in the current cluster.  The problem with this is that there may
// be as much as (CLUSTER_SIZE - 1) words left to be read and if you wrote
// a read loop that reads until EOF not all of the data would be read.  I've
// modified this function to return the count of the number of bytes actually
// transferred, and to return EOF when an attempt to read beyond the end of
// the file is detected.  What this allows for is if you issue a read that
// requests more data than is available, you will get a short count back, and
// can assume you've read upto the end of the file.  If you then issue a
// subsequent read, you will get the EOF indication.  Joe is such an idiot.
// The least he could have done if he was going to use such a stupid and
// inefficent file system was to implement it correctly.  I really think Joe
// should go off and paint or something.  Anything but program!!!
//
int FSRead(FSFile *fh, unsigned long *buf, unsigned long size)
{
	register int				ec;
	register unsigned long	len;
	register unsigned long	num;
	unsigned long				tmp;
	FSB							*fsb;
	unsigned long				chain;
	int							amount_read = 0;

	/* check for an invalid file handle */
	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}
	/* check for a file that has been created, but not written to */
	if(fh->cluster < CLUSTER_FIRST_USER)
	{
		return(ERR_FILE_EOF);
	}

	// If the position is greater than or equal to the file size then
	// we are at the end of the file - so return that
	if(fh->pos >= fh->size)
	{
		return(ERR_FILE_EOF);
	}

	/* check if this request reads beyond the eof */
	if(size + fh->pos > fh->size)
	{
		size = fh->size - fh->pos; /* set size to all thats left */
	}
	/* read the data */
	for(;size;)
	{
		/* figure how much to copy */
		len = size;

		/* dont copy more than is in this cluster */
		if(len > CLUSTER_SIZE - fh->off)
		{
			len = CLUSTER_SIZE - fh->off;
		}

		/* check for end of cluster */
		if(!len)
		{
			/* get next cluster */
			if((ec = FATValue(fh->cluster, &chain)) < 0)
			{
				// Send back amount read (bytes)
				return(amount_read << 2);
			}

			/* signals end of file */
			if(chain < CLUSTER_FIRST_USER)
			{
				// Send back amount read (bytes)
				return(amount_read << 2);
			}

			/* check for possible multi-sector read */
			if((ec = FATLinearClusters(chain, size, &tmp, &chain)) < 0)
			{
				// Send back amount read (bytes)
				return(amount_read << 2);
			}
			num = ec;
			if(num)
			{
				tmp = FSClusterToSector(tmp);
				if((ec = SecReads(tmp, buf, num * SECTORS_PER_CLUSTER)) < 0)
				{
					// Send back amount read (bytes)
					return(amount_read << 2);
				}
				/* adjust pointers */
				num *= CLUSTER_SIZE; /* total size transferred */
				buf = &buf[num]; /* callers buffer */
				size -= num; /* amount left to transfer */
				fh->off = 0; /* offset into cluster buffer */
				fh->pos += num; /* file mark */
				fh->cluster = chain; /* next cluster */

				// Accumulate the amount read
				amount_read += num;
				continue; /* loop back */
			} /* directly load sectors */

			/* reset file marks */
			fh->cluster = chain; /* next cluster */
			fh->off = 0; /* offset into cluster buffer */

			continue; /* loop back */
		} /* no length signals end of cluster reached */

		/* read cluster into internal buffer */
		if((ec = FSBGet(fh->cluster, &fsb)) < 0)
		{
			// Send back amount read (bytes)
			return(amount_read << 2);
		}
		fsb->pri = FSBPRI_USR;

		/* copy it to the callers buffer */
		copy32(&fsb->buf[fh->off], buf, len);

		/* adjust pointers */
		buf = &buf[len]; /* callers buffer */
		fh->off += len; /* offset into cluster buffer */
		fh->pos += len; /* file mark */
		size -= len; /* amount left to transfer */

		// Accumlate the amount read
		amount_read += len;
	}

	// Send back amount read (bytes)
	return(amount_read << 2);
}

/*
** FSClose
*/
int FSClose(FSFile *fh)
{
	register int			ec;
	register FDirEntry	*de;
	FSB						*fsb;

	if(!fh->dircluster)
	{
		return(ERR_FILE_INVALID);
	}

	/* check if we need to re-write the dir info */
	if(fh->dirty)
	{
		/* file muse be opened with dir info -- do not use direct open! */
		if(fh->dircluster == 0xffffffff)
		{
			return(ERR_FILE_OPEN);
		}
		if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		de = (FDirEntry *)&fsb->buf[fh->diroff];
		de->size = fh->size;
		de->date = ((int)fh->date << 16) | ((int)fh->time & 0xffff);
		fsb->dirty = FSBDIRTY_SYS;
	}
	if((ec = FSBFlush()) < 0)
	{
		return(ec);
	}
	/* dont clear -- can't reopen -- clear32(fh,sizeof32(FSFile)); */
	FSReopen(fh);
	return(0);
}

/*
** FSDirFirst
*/
int FSDirFirst(FSDir *fd)
{
	clear32((unsigned long *)fd, sizeof32(FSDir));
	fd->curcluster = ROOT_DIR_CLUSTER;
	return(0);
}

/*
** FSDirNext
*/
int FSDirNext(FSDir *fd)
{
	register int			ec;
	register FDirEntry	*de;
	FSB						*fsb;

	if(fd->curcluster < CLUSTER_FIRST_USER)
	{
		return(ERR_FILE_EOF);
	}
	for(;;)
	{
		if(fd->curoff >= CLUSTER_SIZE - sizeof32(FDirEntry) - DIR_RESERVE)
		{
			/* read in the cluster */
			if((ec = FSBGet(fd->curcluster, &fsb)) < 0)
			{
				return(ec);
			}
			fsb->pri = FSBPRI_SYS;

			/* check if there is a link to another cluster */
			fd->curoff = 0;
			if((fd->curcluster = fsb->buf[DIR_LINK]) == 0)
			{
				return(ERR_FILE_EOF); /* no link, end of chain */
			}
		}
		/* read in the cluster */
		if((ec = FSBGet(fd->curcluster, &fsb)) < 0)
		{
			return(ec);
		}
		fsb->pri = FSBPRI_SYS;
		de = (FDirEntry *)&fsb->buf[fd->curoff];
		fd->curoff += sizeof32(FDirEntry); /* bump up for next time */
		if(de->nm[0])
		{
			/* pull out the data */
			fsNMToName(de->nm, fd->name);
			fd->size = de->size;
			fd->flags = de->flagcluster >> 24;
			fd->cluster = de->flagcluster & 0xffff;
			fd->date = de->date;
			return(0); /* found */
		} /* found */
	} /* for */
	return(ERR_FILE_EOF);
}

/*
** FSFlush
*/
int FSFlush(void)
{
	return(FSBFlush());
}

/*
** FSDate
*/
int FSDate(FSFile *fh, unsigned long *date_p)
{
	register int			ec;
	register FDirEntry	*de;
	FSB						*fsb;

	if((ec = FSBGet(fh->dircluster, &fsb)) < 0)
	{
		return(ec);
	}
	fsb->pri = FSBPRI_SYS;
	de = (FDirEntry *)&fsb->buf[fh->diroff];
	*date_p = de->date;
	return(0);
}

/*
** FSClusterToSector
*/
unsigned long FSClusterToSector(unsigned long cluster)
{
	register unsigned long	sec;

	sec = ((cluster - CLUSTER_FIRST_USER) * SECTORS_PER_CLUSTER) +	Disk.rootsector;
	return(sec);
}

/*
** FSDiskUsage
** find disk usage in 32bit words
*/
int FSDiskUsage(unsigned long *total_p, unsigned long *usage_p)
{
	register int				ec;
	register unsigned long	cluster;
	register unsigned long	used;
	unsigned long				v;
	
	used = CLUSTER_FIRST_USER; /* clusters below first user are used by fs */
	for(cluster = CLUSTER_FIRST_USER; cluster < Disk.clusters; cluster++)
	{
		if((ec = FATValue(cluster, &v)) < 0)
		{
			return(ec);
		}
		if(v != CLUSTER_UNUSED)
		{
			used++;
		}
	}
	*total_p = Disk.clusters * CLUSTER_SIZE; /* size in 32 bit words */
	*usage_p = used * CLUSTER_SIZE; /* size in 32 bit words */
	return(0);
}


/*
** FSGetDiskFree
** find disk usage in 32bit words
*/
int FSGetDiskFree(diskfree_t *df)
{
	register int				ec;
	register unsigned long	cluster;
	register unsigned long	used;
	register unsigned long	total_clusters;
	unsigned long				v;

	df->sectors_per_cluster = SECTORS_PER_CLUSTER;
	df->bytes_per_sector = 512;
	total_clusters = Disk.clusters;
	used = CLUSTER_FIRST_USER; /* clusters below first user are used by fs */
	for(cluster = CLUSTER_FIRST_USER; cluster < Disk.clusters; cluster++)
	{
		if((ec = FATValue(cluster, &v)) < 0)
		{
			return(ec);
		}
		if(v != CLUSTER_UNUSED)
		{
			used++;
		}
	}
	while((used >= 65536) || (total_clusters >= 65536))
	{
		used >>= 1;
		total_clusters >>= 1;
		df->sectors_per_cluster <<= 1;
	}
	df->total_clusters = total_clusters;
	df->avail_clusters = total_clusters - used;
	return(0);
}


/*
** FSSupervisor
*/
int FSSupervisor(unsigned long set)
{
	return(0);
}

/*
** fsFormatInfo
*/
static int fsFormatInfo(unsigned long *sectors_p, unsigned long *clusters_p, unsigned long *sectorsforfat_p)
{
	register unsigned long	sectors;
	register unsigned long	clusters;
	register unsigned long	sectorsforfat;

	/* compute sectors and clusters on disk */
	sectors = ide_get_partition_size();
	Disk.bytes = 512;
	clusters = sectors/SECTORS_PER_CLUSTER;
	sectorsforfat = (clusters+(SECTOR_SIZE*FATCLUST_SIZE)-1) /
		(SECTOR_SIZE*FATCLUST_SIZE);
	*sectors_p = sectors;
	*clusters_p = clusters;
	*sectorsforfat_p = sectorsforfat;
	return 0;
}

/*
** fsClusterToFATSector
*/
static unsigned long fsClusterToFATSector(unsigned long cluster, unsigned long *firstcluster_p)
{
	register unsigned long	sec;
	register unsigned long	firstcluster;

	sec = (cluster - CLUSTER_FIRST_USER)/(SECTOR_SIZE*FATCLUST_SIZE);
	firstcluster = sec * (SECTOR_SIZE*FATCLUST_SIZE) + CLUSTER_FIRST_USER;
	sec += Disk.fatsector;
	*firstcluster_p = firstcluster;
	return(sec);
}

/*
** fsNameToNM
*/
static void fsNameToNM(char *name, unsigned long *nm)
{
	register unsigned short	i,j;
	register unsigned char	c,s;

	s = 24;
	nm[0] = nm[1] = nm[2] = 0;
	for(j = 0, i = 0; i < FILE_NAME_SIZE; i++)
	{
		if(i && ((i & 3) == 0))
		{
			j++;
			s = 24;
		}
		c = *name++;
		if(!c)
		{
			break;
		}
//		if(c >= 'a' || c <= 'z')
//		{
//			c &= ~0x20;
//		}
		nm[j] |= c << s;
		s -= 8;
	}
}

/*
** fsNMToName
*/
static void fsNMToName(unsigned long *nm, char *name)
{
	register unsigned short	i,j;
	register unsigned char	c,s;

	s = 24;
	for(j = 0, i = 0; i < FILE_NAME_SIZE; i++)
	{
		if(i && ((i & 3) == 0))
		{
			j++;
			s = 24;
		}

		c = (nm[j] >> s) & 0xff;
		*name++ = c;
		s -= 8;
	}
}

/*
** fsNMCmp
*/
static int fsNMCmp(unsigned long *nm1, unsigned long *nm2)
{
	if(nm1[0] == nm2[0] && nm1[1] == nm2[1] && nm1[2] == nm2[2])
	{
		return(1);
	}
	return(0);
}

int _remove(char *name)
{
	return(FSDelete(name));
}

