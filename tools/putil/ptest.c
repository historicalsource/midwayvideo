#include	<stdio.h>
#include <stdlib.h>
#include	<ctype.h>
#include <io.h>
#include <conio.h>
#include <dos.h>		
#include <process.h>           
#include <signal.h>             
#include <string.h>
#include <bios.h>
#include	<unistd.h>
#include	<pd.h>

#define	BUFFER_SIZE	32768

unsigned char	write_buf[BUFFER_SIZE];
unsigned char	read_buf[BUFFER_SIZE];

void set_memory(unsigned long start_address, unsigned long end_address, unsigned char data_val)
{
	unsigned long	address;
	int				i;
	unsigned long	amount;

	fprintf(stderr, "Setting Memory\r\n");

	for(i = 0; i < BUFFER_SIZE; i++)
	{
		write_buf[i] = data_val;
	}

	for(address = start_address; address < end_address; address += BUFFER_SIZE)
	{
		fprintf(stderr, "Address: 0x%08.8lX\r", address);

		amount = end_address - address;

		if(amount > BUFFER_SIZE)
		{
			amount = BUFFER_SIZE;
		}

		if(psyq_blk_write(address, write_buf, amount))
		{
			fprintf(stderr, "\r\nProblem writing data to address: 0x%lX\r\n", address);
			exit(1);
		}
	}

	fprintf(stderr, "Address: 0x%08.8lX\r\n", address);
}

void check_memory(unsigned long start_address, unsigned long end_address, unsigned char data_val)
{
	unsigned long	address;
	int				i;
	unsigned long	amount;

	fprintf(stderr, "Checking Memory\r\n");

	for(i = 0; i < BUFFER_SIZE; i++)
	{
		write_buf[i] = data_val;
	}

	for(address = start_address; address < end_address; address += BUFFER_SIZE)
	{
		fprintf(stderr, "Address: 0x%08.8lX\r", address);

		amount = end_address - address;

		if(amount > BUFFER_SIZE)
		{
			amount = BUFFER_SIZE;
		}

		if(psyq_blk_read(address, read_buf, amount))
		{
			fprintf(stderr, "\r\nProblem reading from address: 0x%lX\r\n", address);
			exit(1);
		}

		for(i = 0; i < amount; i++)
		{
			if(write_buf[i] != read_buf[i])
			{
				fprintf(stderr, "\r\nData Error - Address 0x%lX: 0x%02.2X -> 0x%02.2X\r\n", address + i, write_buf[i], read_buf[i]);
				exit(1);
			}
		}
	}

	fprintf(stderr, "Address: 0x%08.8lX\r\n", address);
}

void memory_test3(unsigned long start_address, unsigned long end_address)
{
	fprintf(stderr, "Memory Clear Test\r\n");
	set_memory(start_address, end_address, 0);
	check_memory(start_address, end_address, 0);
}


void memory_test4(unsigned long start_address, unsigned long end_address)
{
	fprintf(stderr, "Memory Set Test\r\n");
	set_memory(start_address, end_address, 0xff);
	check_memory(start_address, end_address, 0xff);
}

void memory_test5(unsigned long s1, unsigned long e1, unsigned char d1, unsigned long s2, unsigned long e2, unsigned char d2)
{
	fprintf(stderr, "Interbank Address Wrap Test\r\n");
	set_memory(s1, e1, d1);
	set_memory(s2, e2, d2);
	check_memory(s1, e1, d1);
}

void memory_test1(unsigned long start_address, unsigned long end_address)
{
	unsigned long	address;
	unsigned char	data_val = 1;
	int				i;
	unsigned long	amount;

	fprintf(stderr, "Data integrity test\r\n");

	for(address = start_address; address < end_address; address += BUFFER_SIZE)
	{
		fprintf(stderr, "Address: 0x%08.8lX\r", address);

		for(i = 0; i < BUFFER_SIZE; i++)
		{
			write_buf[i] = data_val;
			data_val++;
			if(!data_val)
			{
				data_val = 1;
			}
		}

		amount = end_address - address;

		if(amount > BUFFER_SIZE)
		{
			amount = BUFFER_SIZE;
		}

		if(psyq_blk_write(address, write_buf, amount))
		{
			fprintf(stderr, "\r\nProblem writing data to address: 0x%lX\r\n", address);
			exit(1);
		}

		if(psyq_blk_read(address, read_buf, amount))
		{
			fprintf(stderr, "\r\nProblem reading data from address: 0x%lX\r\n", address);
			exit(1);
		}

		for(i = 0; i < amount; i++)
		{
			if(write_buf[i] != read_buf[i])
			{
				fprintf(stderr, "\r\nData Error - Address 0x%lX: 0x%02.2X -> 0x%02.2X\r\n", address + i, write_buf[i], read_buf[i]);
				exit(1);
			}
		}
	}

	fprintf(stderr, "Address: 0x%08.8lX\r\n", address);
}




void memory_test2(unsigned long start_address, unsigned long end_address)
{
	unsigned long	address;
	unsigned char	data_val = 1;
	int				i;
	unsigned long	amount;

	fprintf(stderr, "Intrabank address wrap test\r\n");

	fprintf(stderr, "Write Pass\r\n");

	for(address = start_address; address < end_address; address += BUFFER_SIZE)
	{
		fprintf(stderr, "Address: 0x%08.8lX\r", address);

		for(i = 0; i < BUFFER_SIZE; i++)
		{
			write_buf[i] = data_val;
			data_val++;
			if(!data_val)
			{
				data_val = 1;
			}
		}

		amount = end_address - address;

		if(amount > BUFFER_SIZE)
		{
			amount = BUFFER_SIZE;
		}

		if(psyq_blk_write(address, write_buf, amount))
		{
			fprintf(stderr, "\r\nProblem writing data to address: 0x%lX\r\n", address);
			exit(1);
		}
	}

	fprintf(stderr, "Address: 0x%08.8lX\r\n", address);

	data_val = 1;
	fprintf(stderr, "Read Pass\r\n");
	for(address = start_address; address < end_address; address += BUFFER_SIZE)
	{
		fprintf(stderr, "Address: 0x%08.8lX\r", address);

		for(i = 0; i < BUFFER_SIZE; i++)
		{
			write_buf[i] = data_val;
			data_val++;
			if(!data_val)
			{
				data_val = 1;
			}
		}

		amount = end_address - address;

		if(amount > BUFFER_SIZE)
		{
			amount = BUFFER_SIZE;
		}

		if(psyq_blk_read(address, read_buf, amount))
		{
			fprintf(stderr, "\r\nProblem reading data from address: 0x%lX\r\n", address);
			exit(1);
		}

		for(i = 0; i < amount; i++)
		{
			if(write_buf[i] != read_buf[i])
			{
				fprintf(stderr, "\r\nData Error - Address 0x%lX: 0x%02.2X -> 0x%02.2X\r\n", address + i, write_buf[i], read_buf[i]);
				exit(1);
			}
		}
	}

	fprintf(stderr, "Address: 0x%08.8lX\r\n", address);
}

void button_test(void)
{
	unsigned long	p12;
	unsigned long	p34;
	unsigned long	misc;

	printf("\n\nPress any key when you want to quit\n\n");
	printf("P12   P34   MISC\n");
	while(1)
	{
		if(kbhit())
		{
			getch();
			break;
		}
		psyq_mem_read(0xb5000010, &p12);
		psyq_mem_read(0xb5000018, &p34);
		psyq_mem_read(0xb5000008, &misc);

		p12 &= 0xffff;
		p34 &= 0xffff;
		misc &= 0xffff;

		fprintf(stderr, "%04X  %04X  %04X\r", (int)p12, (int)p34, (int)misc);
	}
}
	
		

char	processor[128];
char	platform[128];

void main(int argc, char *argv[])
{
	unsigned long	data;
	unsigned long	cdata;
	int				c;

	// Check to make sure TBIOS is installed
	if(!check_driver())
	{
		printf("\n***** DRIVER ERROR *****\n");
		exit(1);
	}

	// Reset the target
	fprintf(stderr, "Resetting Target\r\n");
	if(psyq_reset())
	{
		fprintf(stderr, "Problem Resetting Target\r\n");
		exit(1);
	}

	// Wait for target to finish
	usleep(3000000);

	// Get the processor and platform
	get_target_id(processor, platform);

	// Check to what platform it is
	if(strstr(platform, "PHOENIX"))
	{
		printf("This system appears to be a Phoenix SA1 Is this correct (y/n):  ");
		fflush(stdout);
		c = getche();
		c = toupper(c);
		if(c != 'Y')
		{
			printf("\nHave your hardware checked\n");
			exit(1);
		}
	}
	else
	{
		printf("I don't recognize this platform: %s\n", platform);
		exit(1);
	}

	printf("\nUnlocking the IOASIC");
	fflush(stdout);
	if(!unlock_ioasic())
	{
		fprintf(stderr, " - FAILED\r\n");
		exit(1);
	}
	printf(" - UNLOCKED");

	printf("\nIs the seven segment display flashing (y/n):  ");
	fflush(stdout);
	while(1)
	{
		psyq_mem_write(0xb5100010, 0);
		usleep(250000);
		psyq_mem_write(0xb5100010, 0xff);
		usleep(250000);
		if(kbhit())
		{
			c = getche();
			c = toupper(c);
			if(c != 'Y')
			{
				printf("\nHave your hardware checked\n");
				exit(1);
			}
			else
			{
				break;
			}
		}
	}
	psyq_mem_write(0xb5100010, 0xff);

#if 0
	printf("\nIs coin counter 1 clicking (y/n):  ");
	fflush(stdout);
	while(1)
	{
		psyq_mem_write(0xb5000038, 0x10);
		psyq_mem_write(0xb5000038, 0x1);
		usleep(36000);
		psyq_mem_write(0xb5000038, 0x10);
		psyq_mem_write(0xb5000038, 0x0);
		usleep(54000);
		if(kbhit())
		{
			c = getche();
			c = toupper(c);
			if(c != 'Y')
			{
				printf("\nHave your hardware checked\n");
				exit(1);
			}
			else
			{
				break;
			}
		}
	}

	printf("\nIs coin counter 2 clicking (y/n):  ");
	fflush(stdout);
	while(1)
	{
		psyq_mem_write(0xb5000038, 0x10);
		psyq_mem_write(0xb5000038, 2);
		usleep(18000);
		psyq_mem_write(0xb5000038, 0x10);
		psyq_mem_write(0xb5000038, 0);
		usleep(18000);
		if(kbhit())
		{
			c = getche();
			c = toupper(c);
			if(c != 'Y')
			{
				printf("\nHave your hardware checked\n");
				exit(1);
			}
			else
			{
				break;
			}
		}
	}

	printf("\nAre both counters clicking (y/n):  ");
	fflush(stdout);
	while(1)
	{
		psyq_mem_write(0xb5000038, 0x10);
		psyq_mem_write(0xb5000038, 3);
		usleep(18000);
		psyq_mem_write(0xb5000038, 0x10);
		psyq_mem_write(0xb5000038, 0);
		usleep(18000);
		if(kbhit())
		{
			c = getche();
			c = toupper(c);
			if(c != 'Y')
			{
				printf("\nHave your hardware checked\n");
				exit(1);
			}
			else
			{
				break;
			}
		}
	}
#endif
	printf("\nDo you want to test the joysticks and other buttons (y/n):  ");
	fflush(stdout);
	c = getche();
	c = toupper(c);
	if(c == 'Y')
	{
		button_test();
	}

	printf("\nDo you want to run the memory tests (y/n):  ");
	fflush(stdout);
	c = getche();
	c = toupper(c);
	if(c != 'Y')
	{
		exit(1);
	}

	fprintf(stderr, "\r\n\r\nMemory Tests\r\n");

	// Run walking 1 data test
	fprintf(stderr, "Walking 1 data test - address 0x80040000\r\n");

	data = 1;
	while(data)
	{
		if(psyq_mem_write(0x80040000, data))
		{
			fprintf(stderr, "Problem writing data to target\r\n");
			exit(1);
		}

		if(psyq_mem_read(0x80040000, &cdata))
		{
			fprintf(stderr, "Problem reading data from target\r\n");
			exit(1);
		}

		if(data != cdata)
		{
			fprintf(stderr, "Data integrity problem\r\n");
			fprintf(stderr, "Address 0x80040000 - Bit: ");
			cdata = 0;
			while(!(data & 1))
			{
				cdata++;
				data >>= 1;
			}
			fprintf(stderr, "%ld\r\n", cdata);
			exit(1);
		}
		data <<= 1;
	}

	// Run walking 1 data test
	fprintf(stderr, "Walking 1 data test - address 0x80040004\r\n");
	data = 1;
	while(data)
	{
		if(psyq_mem_write(0x80040004, data))
		{
			fprintf(stderr, "Problem writing data to target\r\n");
			exit(1);
		}

		if(psyq_mem_read(0x80040004, &cdata))
		{
			fprintf(stderr, "Problem reading data from target\r\n");
			exit(1);
		}

		if(data != cdata)
		{
			fprintf(stderr, "Data integrity problem\r\n");
			fprintf(stderr, "Address 0x80040004 - Bit: ");
			cdata = 0;
			while(!(data & 1))
			{
				cdata++;
				data >>= 1;
			}
			fprintf(stderr, "%ld\r\n", cdata);
			exit(1);
		}
		data <<= 1;
	}

	// Run walking 0 data test
	fprintf(stderr, "Walking 0 data test - address 0x80040000\r\n");
	data = 0xfffffffe;
	while(data != 0xffffffff)
	{
		if(psyq_mem_write(0x80040000, data))
		{
			fprintf(stderr, "Problem writing data to target\r\n");
			exit(1);
		}

		if(psyq_mem_read(0x80040000, &cdata))
		{
			fprintf(stderr, "Problem reading data from target\r\n");
			exit(1);
		}

		if(data != cdata)
		{
			fprintf(stderr, "Data integrity problem\r\n");
			fprintf(stderr, "Address 0x80040000 - Bit: ");
			cdata = 0;
			while(data & 1)
			{
				cdata++;
				data >>= 1;
			}
			fprintf(stderr, "%ld\r\n", cdata);
			exit(1);
		}
		data <<= 1;
		data |= 1;
	}


	// Run walking 0 data test
	fprintf(stderr, "Walking 0 data test - address 0x80040004\r\n");
	data = 0xfffffffe;
	while(data != 0xffffffff)
	{
		if(psyq_mem_write(0x80040004, data))
		{
			fprintf(stderr, "Problem writing data to target\r\n");
			exit(1);
		}

		if(psyq_mem_read(0x80040004, &cdata))
		{
			fprintf(stderr, "Problem reading data from target\r\n");
			exit(1);
		}

		if(data != cdata)
		{
			fprintf(stderr, "Data integrity problem\r\n");
			fprintf(stderr, "Address 0x80040004 - Bit: ");
			cdata = 0;
			while(data & 1)
			{
				cdata++;
				data >>= 1;
			}
			fprintf(stderr, "%ld\r\n", cdata);
			exit(1);
		}
		data <<= 1;
		data |= 1;
	}

	fprintf(stderr, "Memory Bank 1\r\n");
	memory_test3(0x80040000, 0x80800000);
	memory_test4(0x80040000, 0x80800000);
	memory_test1(0x80040000, 0x80800000);
	memory_test2(0x80040000, 0x80800000);

	fprintf(stderr, "\r\nMemory Bank 2\r\n");
	memory_test3(0x81000000, 0x82000000);
	memory_test4(0x81000000, 0x82000000);
	memory_test1(0x81000000, 0x82000000);
	memory_test2(0x81000000, 0x82000000);

	fprintf(stderr, "\r\nBank 1 and 2 Tests\r\n");
	memory_test5(0x80040000, 0x80800000, 0, 0x81000000, 0x82000000, 0xff);
	memory_test5(0x81000000, 0x82000000, 0, 0x80040000, 0x80800000, 0xff);

	fprintf(stderr, "\r\nNo Errors Detected\r\n");

	exit(0);
}
