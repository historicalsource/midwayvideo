//
// Copyright (c) 1997 by Midway Video Inc.
//
// $Revision: 12 $
//
// $Author: Mlynch $
//
#include	<system.h>
#include	<io.h>

#if defined(TTY_DRIVER)
extern int ttyinit(), ttyopen(), ttyread(), ttywrite(), ttyioctl();
#endif
extern int fileinit(), fileopen(), fileclose(), fileread(), filewrite(), fileioctl();
extern int ledinit(), ledopen(), ledclose(), ledread(), ledwrite();
extern int wdogwrite(), wdogioctl();
#if (PHOENIX_SYS & VEGAS)
extern int cmosinit();
#else
extern int cmosinit(), cmosread(), cmoswrite(), cmosioctl();
#endif
extern int ide_init();
extern int unlock_ioasic(), ioasicioctl();
#if (PHOENIX_SYS & VEGAS)
extern int coininit(), coinopen(), coinclose(), coinioctl();
extern int timerinit();
#endif
extern int timerioctl();
extern int pic_init(), pic_ioctl();
extern int sound_init(), sound_ioctl();
#define	ioasicinit	unlock_ioasic
#if (!(PHOENIX_SYS & VEGAS))
extern int gt64010_init(), sysopen(), sysioctl();
#else
extern int sysinit(), sysopen(), sysioctl();
#endif
#ifndef TEST
extern int scsi_init(), scsiopen();
#endif
#if (PHOENIX_SYS & SEATTLE)
extern int rdiag_init();
#endif
extern int nulldev(void);
extern int nodev(void);

/*
**	declarations for device table entries
*/

const struct dev_init_tab device_init[] = {
{
"led:",
"leds",
"led",
0,
0,
0,
0
},
{
"ioasic:",
"i/o subsystem",
"ioasic",
0,
0,
0,
0
},
{
"pic:",
"PIC Micro",
"pic",
0,
0,
0,
0
},
{
"pci:",
"pci bus",
"sysctrl",
0,
0,
0,
0
},
{
"timer:",
"timers",
"timer",
0,
0,
0,
0
},
#if defined(TTY_DRIVER)
{
"con:",		/* name of device */
"terminal",		/* dev description */
"tty",		/* driver name */
0,		/* controller number */
1,		/* unit number */
0,		/* partition number */
0x1fe00000	/* io base address */
},	
#endif
#if (PHOENIX_SYS & SEATTLE)
{
"mdiag:",
"embedded diagnostics",
"mdiag",
0,
0,
0,
0
},
#endif
{
"c:",
"ide disk",
"disk",
0,
0,
0,
0
},
{
"file:",
"file system",
"file",
0,
0,
0,
0
},
#ifndef TEST
{
"scsi:",
"debugger com",
"scsi",
0,
0,
0,
0
},
#endif
{
"wdog:",
"watchdog",
"wdog",
0,
0,
0,
0
},
{
"cmos:",
"nvram",
"cmos",
0,
0,
0,
0
},
{
"snd:",
"sound subsystem",
"sound",
0,
0,
0,
0
},
#if ((PHOENIX_SYS & VEGAS))
{
"coin:",
"coin counters",
"coinctr",
0,
0,
0,
0
},
#endif
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0 }
};

//
// NOTE - This stuff MUST be in system initialization sequence order
//
const struct dev_sw_tab device_table[] = {
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
wdogwrite,			// write
nulldev,				// init
nulldev,				// strategy
wdogioctl,			// ioctl
"wdog"				// driver name
},
{
ledopen,				// open
ledclose,			// close
ledread,				// read
ledwrite,			// write
ledinit,				// init
nulldev,				// strategy
nulldev,				// ioctl
"led"					// driver name
},
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
ioasicinit,			// init
nulldev,				// strategy
ioasicioctl,		// ioctl
"ioasic"				// driver name
},
{
sysopen,				// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
#if (!(PHOENIX_SYS & VEGAS))
gt64010_init,		// init
#else
sysinit,				// init
#endif
nulldev,				// strategy
sysioctl,			// ioctl
"sysctrl"			// driver name
},
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
pic_init,			// init
nulldev,				// strategy
pic_ioctl,			// ioctl
"pic"					// driver name
},
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
#if (!(PHOENIX_SYS & VEGAS))
nulldev,				// init
#else
timerinit,			// init
#endif
nulldev,				// strategy
timerioctl,			// ioctl
"timer"				// driver name
},
#if defined(TTY_DRIVER)
{
ttyopen,			   // open
nulldev,	   		// close
ttyread,			   // read
ttywrite,		   // write
ttyinit,			   // init
nulldev,	   		// strategy
ttyioctl,			// ioctl
"tty" 		   	// driver name
},
#endif
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
sound_init,			// init
nulldev,				// strategy
sound_ioctl,		// ioctl
"sound"				// driver name
},
{
nulldev,				// open
nulldev,				// close
#if (PHOENIX_SYS & VEGAS)
nulldev,				// read
nulldev,				// write
#else
cmosread,			// read
cmoswrite,			// write
#endif
cmosinit,			// init
nulldev,				// strategy
#if (PHOENIX_SYS & VEGAS)
nulldev,				// ioctl
#else
cmosioctl,			// ioctl
#endif
"cmos"				// driver name
},
#if (PHOENIX_SYS & SEATTLE)
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
nulldev,  			// write
rdiag_init,			// init
nulldev,				// strategy
nulldev,  			// ioctl
"mdiag"				// driver name
},
#endif
{
nulldev,				// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
ide_init,			// init
nulldev,				// strategy
nulldev,				// ioctl
"disk"				// driver name
},
{
fileopen,			// open
fileclose,			// close
fileread,			// read
filewrite,			// write
fileinit,			// init
nulldev,				// strategy
fileioctl,			// ioctl
"file"				// driver name
},
#if ((PHOENIX_SYS & VEGAS))
{
coinopen,			// open
coinclose,			// close
nulldev,				// read
nulldev,				// write
coininit,			// init
nulldev,				// strategy
coinioctl,			// ioctl
"coinctr"			// driver name
},
#endif
#ifndef TEST
{
scsiopen,			// open
nulldev,				// close
nulldev,				// read
nulldev,				// write
scsi_init,			// init
nulldev,				// strategy
nulldev,				// ioctl
"scsi"				// driver name
},
#endif
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 },
{ 0,0,0,0,0,0,0,0 }
};

void led_write(int, int);

/*
** init_drivers -- call all device driver initialization entry
** points.  Device driver init routines should basically cleanup
** all global storage.
*/
void init_drivers(void)
{
	register const struct dev_sw_tab *dt;
	register int	dnum = 1;

#if (PHOENIX_SYS & VEGAS)
	printf("Initializing Drivers\n");
#endif
	for(dt = device_table; dt->d_driver_name; dt++)
	{
#if (PHOENIX_SYS & VEGAS)
		set_fcolor(0xffe0);
		printf("%s ", dt->d_driver_name);
#endif
		if((*dt->d_init)() < 0)
		{
#if (PHOENIX_SYS & VEGAS)
			set_fcolor(0xf800);
			printf(" - FAILURE\n", dt->d_driver_name);
			set_fcolor(-1);
#else
			led_write(dnum, 1);
#endif
		}
		++dnum;
	}
#if (PHOENIX_SYS & VEGAS)
	set_fcolor(-1);
#endif
}

static int strcmp(char *s1, char *s2)
{
	if(!s1 && !s2)
	{
		return(1);
	}
	if(!s1 || !s2)
	{
		return(1);
	}
	while(*s1 && *s2)
	{
		if(*s1 != *s2)
		{
			return(*s1 - *s2);
		}
		++s1;
		++s2;
	}
	return(0);
}

/*
** find_dev -- search dev_init_tab
*/
const struct dev_init_tab *find_device(char *cp)
{
	register const struct dev_init_tab *dp;

	for(dp = device_init; dp->dev_name; dp++)
	{
		if(strcmp(dp->dev_name, cp) == 0)
		{
			return(dp);
		}
	}
	return((struct dev_init_tab *)0);
}

/*
** find_driver -- search device driver table 
**
**	entry:
**		pointer to string with driver name in it
**	returns:
**		pointer to driver table entry or if not
**		found it returns a NULL
*/
const struct dev_sw_tab *find_driver(char *cp)
{
	register const struct dev_sw_tab *dp;

	for(dp = device_table; dp->d_driver_name; dp++)
	{
		if(strcmp(dp->d_driver_name, cp) == 0)
		{
			return(dp);
		}
	}
	return((struct dev_sw_tab *)0);
}



int install_driver(struct dev_sw_tab *uds, struct dev_init_tab *udi, int override)
{
	register const struct dev_init_tab	*di;
	register const struct dev_sw_tab		*ds;

	// Does this device already exist ?
	if((di = find_device(udi->dev_name)) != NULL)
	{
		// YES - Are we overriding it ?
		if(!override)
		{
			// NOPE - Return error
			return(0);
		}
	}

	// Device does NOT already exist
	else
	{
		// Get last table entry that has no entry
		di = device_init;
		while(di->dev_name)
		{
			di++;
		}
	}

	// Is this the last entry in the list
	if(di == &device_init[(sizeof(device_init) / sizeof(struct dev_init_tab)) - 1])
	{
		// YES - Don't allow install
		return(0);
	}

	// Copy the user device init table
	memcpy(di, udi, sizeof(struct dev_init_tab));

	// Does this driver already exist ?
	if((ds = find_driver(udi->dev_drv_name)) == NULL)
	{
		// Find a slot in the driver table
		ds = device_table;
		while(ds->d_driver_name)
		{
			++ds;
		}
	}

	// Copy the user device driver table
	memcpy(ds, uds, sizeof(struct dev_sw_tab));

	// Is there an initialization function for the new driver ?
	if(ds->d_init)
	{
		// YES - call it
		ds->d_init();
	}

	// Return success
	return(1);
}

int uninstall_driver(char *dev_name)
{
	register struct dev_init_tab	*di;
	register struct dev_sw_tab		*ds;

	// Does this device exist ?
	if((di = (struct dev_init_tab *)find_device(dev_name)) == NULL)
	{
		// NOPE - Return fail
		return(0);
	}

	// Does the driver exist ?
	if((ds = (struct dev_sw_tab *)find_driver(di->dev_drv_name)) == NULL)
	{
		// NOPE - Return fail
		return(0);
	}

	// Does a device exist after this one ?
	if((di+1)->dev_name)
	{
		// YES - move all devices down
		while((di+1)->dev_name)
		{
			memcpy(di, di+1, sizeof(struct dev_init_tab));
			di++;
		}

		// Clear the last device name
		(di+1)->dev_name = NULL;
	}

	// Does a driver exist after this one ?
	if((ds+1)->d_driver_name)
	{
		// YES - move all drivers down
		while((ds+1)->d_driver_name)
		{
			memcpy(ds, ds+1, sizeof(struct dev_sw_tab));
			ds++;
		}

		// Clear the last driver name
		(ds+1)->d_driver_name = NULL;
	}

	// Return success
	return(1);
}


void clear_user_drivers(void)
{
	register struct dev_init_tab	*di;
	register struct dev_sw_tab		*ds;
	
	// Find the last system device
	if((di = (struct dev_init_tab *)find_device("snd:")) == NULL)
	{
		// Does NOT exist - return
		return;
	}
	else
	{
		++di;
	}

	// Find the last system driver
	if((ds = (struct dev_sw_tab *)find_driver("scsi")) == NULL)
	{
		// Does NOT exist - return
		return;
	}
	else
	{
		++ds;
	}

	// Clear all of the user devices
	while(di->dev_name)
	{
		di->dev_name = NULL;
		di++;
	}

	// Clear all of the user drivers
	while(ds->d_driver_name)
	{
		ds->d_driver_name = NULL;
		ds++;
	}
}


