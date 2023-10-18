#ifndef _STDIO_H_
#define _STDIO_H_

#define __need_size_t
#include <stddef.h>

#define __need___va_list
#include <stdarg.h>

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef SEEK_SET
#define	SEEK_SET	0	/* set file offset to offset */
#endif
#ifndef SEEK_CUR
#define	SEEK_CUR	1	/* set file offset to current plus offset */
#endif
#ifndef SEEK_END
#define	SEEK_END	2	/* set file offset to EOF plus offset */
#endif

/*
** io access  flags
*/
#define O_RDONLY    0x0000
#define O_WRONLY    0x0001
#define O_RDWR      0x0002
#define O_APPEND    0x0008      /* append (writes guaranteed at the end) */
#define O_CREAT     0x0100
#define O_TRUNC     0x0200

#ifdef __GNUC__
#define __VALIST __gnuc_va_list
#else
#define __VALIST char*
#endif

typedef long        fpos_t;

#define FOPEN_MAX       8
#define FILENAME_MAX    16

typedef struct _File {
  unsigned long     fMode;
  signed   long     fHandle;
  unsigned char     *fBuf;
  unsigned char     *fBEnd;
  unsigned char     *fNext;
  unsigned char     *fREnd;
  unsigned char     *fWEnd;
} FILE;

extern FILE     _files[];

/* values for fMode field in FILE struct */

#define _MRDONLY    0x0001
#define _MWRONLY    0x0002
#define _MAPPEND    0x0004
#define _MCREATE    0x0008
#define _MTRUNC     0x0010
#define _MBINARY    0x0020
#define _MTEXT      0x0040
#define _MEOF       0x0100
#define _MERROR     0x0200
#define _MLASTRD    0x0400
#define _MLASTWR    0x0800
#define _MDIRTY     0x1000

#define EOF         (-1)


int     sprintf(char *str, const char *fmt, ...);
int     printf(const char *fmt, ...);
int     vsprintf(char *str, const char *fmt, int *argptr);

int getchar(void);
int getcnow(void);
int getc(int fd);
char *get_a_str(char *buf, char *bufp);
char *get_push_str(char *buf);
int edit_line(char *strptr);
char *gets(char *buf);
void putchar(char c);
int put_char(int fd, char c);
void putc(char c, int fd);
void puts(const char *s);


extern FILE     *fopen(char *fname, char *mode);
extern int      fclose(FILE *fp);
extern size_t   fread(void *buf, size_t isize, size_t num, FILE *fp);
extern size_t   fwrite(void *buf, size_t isize, size_t num, FILE *fp);
extern int      fflush(FILE *stream);
extern int      fseek(FILE *stream, long offset, int origin);
extern long     ftell(FILE *stream);

#define feof(str)   ((str)->fMode & _MEOF)
#define ferror(str) ((str)->fMode & _MERROR)



#endif
