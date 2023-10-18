#ifndef _IDE_H_
#define _IDE_H_

#include <compiler.h>

extern int      ide_init(void);
extern int      ide_set_device(int);
extern int      ide_reset(void);
extern int      ide_identify(mxU32 *readbuf);
extern int      ide_check_devstat(void);
extern mxU32    ide_get_rdstatus(void);
extern mxU32    ide_get_errstatus(void);
extern int      ide_get_rpm(void);
extern int      ide_get_hdinfo(mxU16 *nheads, mxU16 *ncylinders, mxU16 *nsectors);
extern int      ide_hread_sectors(mxU32 *rbuf, int head, int cyl, int snum, int scnt);
extern int      ide_write_sectors(mxU32 *wbuf, int head, int cyl, int snum, int scnt);
extern int      ide_hread_lsectors(mxU32 *rdbuf, int lba, int count);
extern int      ide_write_lsectors(mxU32 *wrbuf, int lba, int count);

#endif
