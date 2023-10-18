#ifndef __FILESYS_H__
#include	<filesys.h>
#endif

#ifndef __FIND_H__
#define __FIND_H__

struct find_t {
	char					pattern[16] __attribute__((packed));
	int					mask __attribute__((packed));
	char					pad __attribute__((packed));
	char					attrib __attribute__((packed));
	unsigned short		wr_time __attribute__((packed));
	unsigned short		wr_date __attribute__((packed));
	long					size __attribute__((packed));
	char					file_name[13] __attribute__((packed));
};


#ifdef __FFIRST_C__
FSDir				fsd;
#else
extern FSDir	fsd;
#endif

int findnext(struct find_t *ft);
int findfirst(char *name, int attrib, struct find_t *ft);

#endif
