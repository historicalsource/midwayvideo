#include	<stdlib.h>
#include	<unistd.h>
#include <ctype.h>
#include	<string.h>
#include	<stdio.h>
#include	<dir.h>
#include	<sys\stat.h>

#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include	<fcntl.h>
#include <dos.h>
#include	<pd.h>

static partition_table_t	ptable;

static char						sector_buffer[512];

static char						proc[12];
static char						plat[12];

static struct _diskfree_t	ds;

static void memory_test(void)
{
	unsigned int	data;
	unsigned int	start_data;
	unsigned int	cdata;
	unsigned int	address;
	int				pass_count = 1;

	data = 1;
	while(1)
	{
		printf("\nWriting data for pass: %d\n", pass_count);
		start_data = data;
		for(address = 0xa0500000; address < 0xa0800000; address += 4)
		{
			if(!(address % 0x1000))
			{
				printf("Address: 0x%8.8X\r", address);
				fflush(stdout);
			}
			psyq_mem_write(address, data);
			data++;
		}
		printf("\nChecking data for pass: %d\n", pass_count);
		for(address = 0xa0500000; address < 0xa0800000; address += 4)
		{
			if(!(address % 0x1000))
			{
				printf("Address: 0x%8.8X\r", address);
				fflush(stdout);
			}
			psyq_mem_read(address, (unsigned long *)&cdata);
			if(cdata != start_data)
			{
				printf("\nDATA ERROR: address: 0x%X  0x%X -> 0x%X\n", address, start_data, cdata);
			}
			start_data++;
		}
		++pass_count;
	}
}

void main(int argc, char *argv[])
{
	int	i;
	int	sector;
	int	wr_data;
	int	rd_data;
	int	card_addr;
	int	major;
	int	minor;

	if(!check_driver())
	{
		return;
	}
memory_test();
	get_target_id(proc, plat);
	printf("%s\n", plat);
	printf("%s\n", proc);

	get_target_info(&card_addr);

	printf("%x\n", card_addr);

	get_target_version(&major, &minor);

	printf("%d.%d\n", major, minor);

#if 1
	for(wr_data = 0; wr_data <= 0xffffffff; wr_data++)
	{
		psyq_mem_write(0xb5100010, wr_data);
		usleep(50000);
		psyq_mem_read(0xb5100010, (unsigned long *)&rd_data);
		if((wr_data & 0xff) != (rd_data & 0xff))
		{
			printf("BAD: %x -> %x\n", (wr_data & 0xff), (rd_data & 0xff));
		}
	}
return;
#endif
	_pd_setdrive(0x100, &i);
	if(!_pd_getdiskfree(0x100, &ds))
	{
		printf("sectors_per_cluster: %d\n", ds.sectors_per_cluster);
		printf("avail_clusters: %d\n", ds.avail_clusters);
		printf("bytes_per_sector: %d\n", ds.bytes_per_sector);
		printf("total_clusters: %d\n", ds.total_clusters);

		printf("space avail: %d\n", ds.sectors_per_cluster * ds.avail_clusters * ds.bytes_per_sector);
		printf("total size: %d\n", ds.sectors_per_cluster * ds.total_clusters * ds.bytes_per_sector);
	}
	else
	{
		printf("BOGUS\n");
	}
return;
	_pd_setftime(i, 0x7656, 0x3210);
	_pd_open("SYSFONT.WMS", 0, &i);
	_pd_close(i);

	if(getptbl(&ptable))
	{
		if(ptable.magic_number != PART_MAGIC)
		{
			printf("Drive is NOT partitioned\n");
		}
		else
		{
			for(i = 0; i < ptable.num_partitions; i++)
			{
				printf("%d: %ld %ld\n", i, ptable.partition[i].starting_block, ptable.partition[i].num_blocks);
			}
		}
	}
	else
	{
		printf("Error in return of partition table\n");
	}

while(1)
{
	printf("\n\n");
	for(sector = 0; sector < 2000000; sector++)
	{
		printf("sector: %08d\r", sector);
		fflush(stdout);
		read_sectors(0, sector_buffer, sector, 1);
	}
}

	if(!write_sectors(0, sector_buffer, 0, 1))
	{
		printf("sector write error\n");
	}

	printf("\n\n");
	if(read_sectors(1, sector_buffer, 0, 1))
	{
		for(i = 0; i < 16; i++)
		{
			printf("%2.2x ", (unsigned int)sector_buffer[i]);
		}
	}

	printf("\n\n");
	if(read_sectors(0, sector_buffer, 0, 1))
	{
		for(i = 0; i < 16; i++)
		{
			printf("%2.2x ", (unsigned int)sector_buffer[i]);
		}
	}
}
