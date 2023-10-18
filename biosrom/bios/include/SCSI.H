
#define	SCSI_IO_BASE_ADDR   0xaa000000
#define	SCSI_MEM_BASE_ADDR  0xa9000000

/* Flags to determine whether the scsi card should be accessed
   by i/o mapping (non-zero) or else memory mapped */
   
#define USE_SCSI_IO 0

#if USE_SCSI_IO
#define SCSI_BASE_ADDR SCSI_IO_BASE_ADDR
#else
#define SCSI_BASE_ADDR SCSI_MEM_BASE_ADDR
#endif

/* Scsi ID for card */

#define SCSI_ID 0

