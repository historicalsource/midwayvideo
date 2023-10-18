#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include <libc/dosio.h>
#include <go32.h>
#include <dpmi.h>
#include <errno.h>
#include <dos.h>
#include	"pd.h"

// First 32 are DOS next 32 are PHOENIX
int			sys = 0x21;

int check_driver(void)
{
	__dpmi_regs	r;
	char			cmd_line[256];
	char			*tmp;
	int			rechecking = 0;

recheck:	
	r.h.ah = 0x35;
	r.h.al = 0x7e;
	r.x.bx = 0;
	r.x.es = 0;
	
	// No cntl-c's while doing this
	setcbrk(0);

	__dpmi_int(DOS_INT, &r);

	// Cntl-c's OK now
	setcbrk(1);
	
	if(!(r.x.bx | r.x.es))
	{
		if(rechecking == 2)
		{
			printf("\nTBIOS2 failed to install\n");
			return(0);
		}
		else if(rechecking == 0)
		{
			printf("\nTBIOS2 is not installed - installing\n");
			tmp = getenv("TBIOS_ADR");
			if(!tmp)
			{
				printf("Environment variable \"TBIOS_ADR\" is not defined\n");
			}
			else
			{
				sprintf(cmd_line, "tbios2 /a%s", tmp);
				system(cmd_line);
			}
		}
		else
		{
			printf("Attempting to use default I/O Address\n");
			system("tbios2 /a380");
		}
		rechecking++;
		goto recheck;
	}
	return(1);
}

char *get_fname_base(const char *name)
{
	const char	*tmp;

	if(strstr(name, "phx:") || strstr(name, "PHX:"))
	{
		tmp = name;
		while(*tmp)
		{
			++tmp;
		}
		while(*tmp != ':' && *tmp != '\\')
		{
			--tmp;
		}
		++tmp;
	}
	else
	{
		return((char *)name);
	}
	return((char *)tmp);
}
