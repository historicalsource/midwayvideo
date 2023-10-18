#if defined(GOOSE_INIT_CODE)
#include	<stdio.h>
#include	<string.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<ioctl.h>
#include	<goose/goose.h>
#if defined(GLIDE3)
#include	<glide.h>
#else
#include	<glide/glide.h>
#endif

#if ! defined(GLIDE3)
/* register used as the pointer to the DMA buffers */
register char *cur_dma_ptr asm("$28");

static sst1VideoTimingStruct myVideoResolution = {
	25,			/* hSyncOn */
	636,		/* hSyncOff */
	5,			/* vSyncOn */
	434,		/* vSyncOff */
	90,			/* hBackPorch */
	35,			/* vBackPorch */
	512,		/* xDimension */
	384,		/* yDimension */
	96,			/* memOffset */
	0x0,		/* memFifoEntries_1MB */
	0x100,		/* memFifoEntries_2MB */
	0x100,		/* memFifoEntries_4MB */
	8,			/* tilesInX_Over2 */
	23,			/* vFifoThreshold */
	FXTRUE,		/* video16BPPIsOk */
	FXTRUE,		/* video24BPPIsOk */
	16.5161,	/* clkFreq16bpp */
	33.0323		/* clkFreq24bpp */
};
#endif

#define ERROR 0xEEEE

#define SND_CMD_SRAM_TEST    0x3A
#define SND_CMD_DRAM0_TEST   0x4A
#define SND_CMD_DRAM1_TEST   0x5A
#define SND_CMD_ROM_VERSION  0x6A
#define SND_CMD_ASIC_VER     0x7A
#define SND_CMD_ECHO         0x8A
#define SND_CMD_PMINT_SUM    0x9A  
#define SND_CMD_BONG         0xAA

/* completion codes (CC) for above diags */
#define SND_RTN_SRAM_PASSED   0xCC01
#define SND_RTN_DRAM0_PASSED  0xCC02
#define SND_RTN_DRAM1_PASSED  0xCC03
#define SND_RTN_BONG_FINISHED 0xCC04
#define SND_RTN_SRAM1_FAILED  0xEE01 
#define SND_RTN_SRAM2_FAILED  0xEE02
#define SND_RTN_SRAM3_FAILED  0xEE03
#define SND_RTN_DRAM0_FAILED  0xEE04 
#define SND_RTN_DRAM1_FAILED  0xEE05 

static void graphics_init(goose_init_info_t *);
static unsigned long get_text_crc(void);

unsigned int	get_text_addr(void);
unsigned int	get_text_end_addr(void);

static int				rows;
static int				columns;
static char				str_buffer[128];
static char				platform_name[128];
static char				proc_name[128];
static int				pspeed;
static int				cur_row = 4;

float				hres, vres;
unsigned long	start_crc32;
char __tend[] __attribute__((aligned (8), section ("textend"))) = {};


#define	CENTER(x) \
	{\
		int	__length = columns - strlen((char *)(x));\
		__length /= 2;\
		__length++;\
		to_printf("[%d;%dH%s", cur_row, __length, (char *)(x));\
		++cur_row;\
	}

#define	GREEN		to_printf("[32m")
#define	YELLOW	to_printf("[33m")
#define	CYAN		to_printf("[36m")
#define	RED		to_printf("[31m")
#define	MY_WHITE		to_printf("[37m")

static unsigned long get_text_crc(void)
{
	unsigned long	start_addr;
	unsigned long	num_bytes;

	start_addr = get_text_addr();
	num_bytes = get_text_end_addr();
	num_bytes -= start_addr;
	return(crc((unsigned char *)start_addr, num_bytes));
}
		

void goose_init(goose_init_info_t *gii)
{
	int	serial_number;
	int	game_number;
	int	month;
	int	day;
	int	year;
	int	dom;
	int	fd;
	int	boot_version;
	int	sdrc_version;
	int	opsys_version;
	int	port_status;
	int	pmint_sum;
	int	sram_status;
	int	dram_status;
	int	bong_status;
	int	i;
	int	aud_status;
	int	adj_status;
	int	rec_status[16];
	int	cmos_status;

	// Generate text section CRC
	start_crc32 = get_text_crc();

	// Setup 49 way joysticks ?
	if(gii->allow_49way)
	{
		// YES - set 'em up
		set_49way(1);
	}

#if defined(SEATTLE)
	// open the GT64010 system controller device
	gt_fd = open("pci:", O_RDWR | O_BINARY);
	if(gt_fd == -1)
	{
		fprintf(stderr, "%s() - Can not open system controller device\n",
			__FUNCTION__);
		while(1) ;
	}
	
	// allocate memory for the DMA buffers
	dma_buffer1 = malloc((gii->dma_buffer_size * 2) + 32);
	if(dma_buffer1 == NULL)
	{
		fprintf(stderr, "%s() - Can not allocate memory for DMA buffers\n",
			__FUNCTION__);
		while(1) ;
	}
	
	// Adjust the dma buffer pointer such that it aligned on a cache line
	// This is safe because these buffers are never freed (or they should
	// never be freed)
	while(((int)dma_buffer1 & 0x1F) != 0)
	{
		dma_buffer1++;
	}

	/ set the pointer for the second dma buffer
	dma_buffer2 = dma_buffer1 + gii->dma_buffer_size
	
	// Adjust the pointers to be mapped
	// If you are looking at this code and wondering what is going on here,
	// this is the straight poop.  The BIOS rom sets up 32 entries (64 pages)
	// of the TLB (translation lookaside buffer) to map virtual addresses
	// 0 to 8 Meg to physical addresses 0 to 8 Meg in 256k chunks.  TLB entries
	// 32 to 48 are setup to map virtual addresses 8 - 12 Meg to physical
	// addresses 8 - 12 meg.  TLB entries 1 through 31 are set up to be
	// cached non-coherent cacheable, whereas, all other TLB entries are set up
	// to be NOT accessible from the processor.  Setting TLB entry 0 to be
	// NOT accessible does 2 things.  First, it protects the debugger stub and
	//  BIOS code (loaded from the ROM) from accidently be overwritten by
	// a dereferenced write to a NULL pointer.  Secondly, it will cause a
	// TLB exception to be thrown if a data reference to a NULL pointer is
	// attempted.
	//
	// Now what is going on here is really an optimization used for scatter
	// gather type DMA devices.  DMA devices with scatter gather capability
	// require physical addresses to be used.  By mapping virtual addresses
	// to physical address the way it has been done, there is no need to do
	// a virtual to physical address conversion on addresses used by such
	// devices (as long as allocated addresses are preconverted).
	//
	(int)dma_buffer1 &= 0x1FFFFFFF;
	(int)dma_buffer2 &= 0x1FFFFFFF;
#endif

	// Initialize the graphics system
	graphics_init(gii);

	// Adjust starting row for low resolution
	if(vres < 384.0f)
	{
		cur_row = 2;
	}

#if defined(SEATTLE)
	// install the hardware interrupt handler
	install_interrupt_handler(VERTICAL_RETRACE_HANDLER, vblank_handler);

	if (stream_detect_fifos())
	{
		// Make sure that the I/O ASIC interrupts are off.
		inthand_disable_ioasic_interrupt();
		
	 	// Install the I/O ASIC hardware interrupt handler
		install_interrupt_handler(IOASIC_HANDLER, interrupt_ioasic_handler);
		
		// enable I/O ASIC interrupts
		inthand_enable_ioasic_interrupt();
	}

	// install the galileo dma channel 0 handler
	install_sys_handler(4, galileo_dma0_handler);
	
	// install handler for TLB refill
	install_tlb_handler(tlb_refill_handler);
	
#if ! defined(DEBUG)
	// install the interrupt handler for timer 0 of the GT64010
	install_sys_handler(11, galileo_timer3_handler);
	*((volatile int *)0xAC000C1C) |= 0x800;
#endif
	
	// enable PCI bus parity error detection
	*((volatile int *)0xAC000C1C) |= 0xC000;
	
	// install the divide by 0 exception handler
	install_fpu_handler(FPU_EXC_DIV0, div0_handler);
	
	// install the unimplemented operation exception handler
	install_fpu_handler(FPU_EXC_UNIMP, unimplemented_handler);
	
	// install the integer divide by 0 handler
	install_int_div0_handler(int_div0_handler);
	
	// install the integer divide overlow handler
	install_int_divo_handler(int_divo_handler);
#endif

	// Initialize text overlay for use outside process loop
	init_text_overlay(0);

	// Get the number of rows and columns for the text overlay
	rows = get_to_rows();
	columns = get_to_columns();

	// Clear the text overlay screen
	to_printf("\f");

	// Display the name and version of the game
	YELLOW;
	CENTER(gii->name);
	sprintf(str_buffer, "%s - %s - %s", gii->version, gii->build_date, gii->build_time);
	CENTER(str_buffer);
	cur_row++;
	MY_WHITE;
	CENTER("Copyright (c) 1999 by Midway Video Inc.");
	CENTER("All rights reserved");
	cur_row++;

	// Display the platform name, processor, speed, and secondary cache state
	CYAN;
	get_platform_name(platform_name, sizeof(platform_name));
	get_proc_name(proc_name, sizeof(proc_name));
	pspeed = get_cpu_speed();
	sprintf(str_buffer, "%s Platform with %d Mhz %s", platform_name, pspeed, proc_name);
	CENTER(str_buffer);

	/* open the pic device */
	if((fd = open("pic:", O_RDONLY)) < 0)
	{
		serial_number = -1;
		game_number = -1;
		dom = -1;
	}
	else
	{
		/* get the serial number */
		_ioctl(fd, FIOCGETSERIALNUMBER, (int)&serial_number);
		
		/* get the game number */
		_ioctl(fd, FIOCGETGAMENUMBER, (int)&game_number);
		
		/* get the date of manufactor code */
		_ioctl(fd, FIOCGETDOM, (int)&dom);
	}
	
	/* close the pic device */
	close(fd);

	if(gii->sc)
	{
		for(i = 0; i < gii->num_game_ids; i++)
		{
			if(game_number == gii->game_id[i])
			{
				break;
			}
		}
		if(i == gii->num_game_ids)
		{
			RED;
			CENTER("SECURITY FAILURE");
			CENTER("REBOOTING");
			__asm__("	teqi	$0,0");
		}
	}
	
	/* calculate the month, day, and year of manufacture */
	dom--;
	day = (dom % 31) + 1;
	month = ((dom % 372) / 31) + 1;
	year = (dom / 372) + 80;

	sprintf(str_buffer, "SN: %d - DOM: %02d/%02d/%d", serial_number, month, day, year);
	CENTER(str_buffer);	

	cur_row++;

	snd_reset();

#if ! defined(DEBUG)
	gii->snd_test_bypass = 0;
#endif

	GREEN;
	if(!gii->snd_test_bypass)
	{
		boot_version = snd_get_boot_version();
		boot_version &= 0xffff;
	
		sprintf(str_buffer, "Sound Boot Version:  %X.%02X",
			boot_version >> 8,
			boot_version & 0xff);
		CENTER(str_buffer);
	
		sdrc_version = snd_get_sdrc_version();
		sdrc_version &= 0xffff;
	
		sprintf(str_buffer, "Sound SDRC Version:  %X",
			sdrc_version);
		CENTER(str_buffer);
	
		port_status = snd_test_port();
		port_status &= 0xffff;

		sprintf(str_buffer, "Sound Port Status:  ");
		if(port_status == ERROR)
		{
			strcat(str_buffer, "BAD");
		}
		else
		{
			strcat(str_buffer, "GOOD");
		}
		CENTER(str_buffer);
	
		pmint_sum = snd_get_pmint_sum();
		pmint_sum &= 0xffff;
	
		sprintf(str_buffer, "Sound Checksum:  %X",
			pmint_sum);
		CENTER(str_buffer);
	
		snd_send_command(SND_CMD_SRAM_TEST);
		sram_status = snd_wait_for_completion();
		sram_status &= 0xffff;

		sprintf(str_buffer, "Sound SRAM:  ");	
		switch (sram_status) {
		case SND_RTN_SRAM_PASSED:
			strcat(str_buffer, "PASSED");
			break;
		case SND_RTN_SRAM1_FAILED:
			strcat(str_buffer, "SRAM 1 Failed");
			break;
		case SND_RTN_SRAM2_FAILED:
			strcat(str_buffer, "SRAM 2 Failed");
			break;
		case SND_RTN_SRAM3_FAILED:
			strcat(str_buffer, "SRAM 3 Failed");
			break;
		default:
			strcat(str_buffer, "Unknown error");
			break;
		}
		CENTER(str_buffer);
	
		snd_send_command(SND_CMD_DRAM0_TEST);
		dram_status = snd_wait_for_completion();
		dram_status &= 0xffff;
		
		sprintf(str_buffer, "Sound DRAM0:  ");
		switch (dram_status) {
		case SND_RTN_DRAM0_PASSED:
			strcat(str_buffer, "PASSED");
			break;
		case SND_RTN_DRAM0_FAILED:
			strcat(str_buffer, "FAILED");
			break;
		default:
			strcat(str_buffer, "Unknown error");
			break;
		}
		CENTER(str_buffer);

#if ! defined(SEATTLE)	
		snd_send_command(SND_CMD_DRAM1_TEST);
		dram_status = snd_wait_for_completion();
		dram_status &= 0xffff;
		
		sprintf(str_buffer, "Sound DRAM1:  ");
		switch (dram_status) {
		case SND_RTN_DRAM1_PASSED:
			strcat(str_buffer, "PASSED");
			break;
		case SND_RTN_DRAM1_FAILED:
			strcat(str_buffer, "FAILED");
			break;
		default:
			strcat(str_buffer, "Unknown error");
			break;
		}
		CENTER(str_buffer);
#endif
	
		snd_send_command(SND_CMD_BONG);
		bong_status = snd_wait_for_completion();
		bong_status &= 0xffff;
	
		sprintf(str_buffer, "Sound Tone Status:  ");
		if(bong_status == SND_RTN_BONG_FINISHED)
		{
			strcat(str_buffer, "GOOD");
		}
		else
		{
			strcat(str_buffer, "FAIL");
		}
		CENTER(str_buffer);
	}
	
	snd_load_opsys();

	if(!gii->snd_test_bypass)
	{
		opsys_version = snd_get_opsys_ver();
		opsys_version &= 0xffff;
	
		sprintf(str_buffer, "Sound OS Version:  %X.%02X",
			opsys_version >> 8,
			opsys_version & 0xff);
		CENTER(str_buffer);
	}
	cur_row++;

	// Initialize switch handling
	switch_init();

	// Clear stuck player switches
	clear_stuck_psw();

	// Initialize sprite system
	init_sprites();

#if defined(SEATTLE)
#ifdef DEBUG
	// allow lockups to occur
	enable_lockup();
	enable_tlb_dump();
#else
	// disable lockups
	disable_lockup();
	set_reboot_vectors();
#endif
#endif

	to_printf("\f");
	cur_row = 4;
	if(vres < 384.0)
	{
		cur_row = 2;
	}
	if(gii->sc)
	{
		MY_WHITE;
		CENTER("Setting up and testing CMOS");
		++cur_row;
		cmos_status = setup_cmos2(gii->sc, &aud_status, &adj_status, rec_status);

		switch(cmos_status)
		{
			case CMOS_SETUP_TOO_MANY_AUDITS:
			{
				RED;
				CENTER("CMOS top audit number is higher than max");
				__asm__("	teqi	$0,0");
				break;
			}
			case CMOS_SETUP_TOO_MANY_ADJUST:
			{
				RED;
				CENTER("CMOS top adjust number is higher than max");
				__asm__("	teqi	$0,0");
				break;
			}
			case CMOS_SETUP_CONFIG_ERROR:
			{
				RED;
				CENTER("CMOS configuration error");
				__asm__("	teqi	$0,0");
				break;
			}
			case CMOS_SETUP_DEAD:
			{
				RED;
				CENTER("CMOS Failure");
				break;
			}
			case CMOS_SETUP_AUDIT_FILE_OPEN:
			{
				RED;
				CENTER("Error opening audits file");
				break;
			}
			case CMOS_SETUP_ADJUST_FILE_OPEN:
			{
				RED;
				CENTER("Error opening adjust file");
				break;
			}
			case CMOS_SETUP_OK:
			{
				switch(aud_status)
				{
					case CMOS_SETUP_OK:
					{
						GREEN;
						CENTER("Audits OK");
						break;
					}
					case CMOS_SETUP_DEAD:
					{
						RED;
						CENTER("Audits BAD");
						break;
					}
					case CMOS_SETUP_RESTORED:
					{
						YELLOW;
						CENTER("Audits RESTORED");
						break;
					}
					default:
					{
						RED;
						CENTER("Audits Unknown error");
						break;
					}
				}
		
				switch(adj_status)
				{
					case CMOS_SETUP_OK:
					{
						GREEN;
						CENTER("Adjustments OK");
						break;
					}
					case CMOS_SETUP_DEAD:
					{
						RED;
						CENTER("Adjustments BAD");
						break;
					}
					case CMOS_SETUP_RESTORED:
					{
						YELLOW;
						CENTER("Adjustments RESTORED");
						break;
					}
					default:
					{
						RED;
						CENTER("Adjustments unknown error");
						break;
					}
				}
		

				for(i = 0; i < gii->sc->conf->num_gcr_tables; i++)
				{		
					switch(rec_status[i])
					{
						case CMOS_SETUP_OK:
						{
							GREEN;
							sprintf(str_buffer, "Table %d OK", i+1);
							break;
						}
						case CMOS_SETUP_DEAD:
						{
							RED;
							sprintf(str_buffer, "Table %d DEAD", i+1);
							break;
						}
						case CMOS_SETUP_RESTORED:
						{
							YELLOW;
							sprintf(str_buffer, "Table %d Restored", i+1);
							break;
						}
						default:
						{
							RED;
							sprintf(str_buffer, "Table %d - Unknown error", i+1);
							break;
						}
					}
					CENTER(str_buffer);
				}

				GREEN;
				CENTER("CMOS OK");
				break;
			}
			default:
			{
				RED;
				CENTER("CMOS unknown error\n");
				break;
			}
		}
	}

	cur_row++;
	MY_WHITE;
	sprintf(str_buffer, "Initializing %s...", gii->name);
	CENTER(str_buffer);

	// Initialize the sound system
	if(snd_init_multi(SND_OPSYS_0223))
	{
		RED;
		CENTER("Error loading sound OS");
	}

	if(snd_get_opsys_ver() == 0xeeee)
	{
		RED;
		CENTER("Error in sound system");
	}

	snd_bank_init();

	// If there is a user initialization function, call it
	if(gii->minit)
	{
		gii->minit();
	}

	// Create the main process
	qcreate("main", 0, gii->mproc, 0, 0, 0, 0);

	to_printf("\f");
	
	// Start up the process dispatcher
	process_dispatch();

	// Should NEVER get here
	RED;
	CENTER("Process dispatcher startup failure");
	CENTER("Rebooting");
	__asm__("	teqi	$0,0");
}


#if defined(GLIDE3)
void resetTextureSystem(void);
#endif

#if defined(VEGAS)
void enable_write_merge(void);
#endif

static void graphics_init(goose_init_info_t *gii)
{
#if ! defined(GLIDE3)
	GrHwConfiguration			hwconfig;
	sst1VideoTimingStruct	*vt = NULL;
	uint							dip;
	resolution_t				res = RESOLUTION_512x384;
	
	// initialize the dam buffer to use
	cur_dma_ptr = dma_buffer1;
	
	sst1InitGrxClk_Called = 0;
	

	// If NOT initializing the CMOS read the info from disk file.  This is the
	// case when this is being used by game diagnostics!!
	if(!gii->sc)
	{
		if((fp = fopen("info.res", "rb")) != (FILE *)0)
		{
			fread(gii->resolution, sizeof(resolution_t), 4, fp);
			fread(&gii->refresh, sizeof(refresh_t), 1, fp);
			fclose(fp);
		}
		else
		{
			fprintf(stderr, "%s() - Error opening info.res for read", __FUNCTION__);
			gii->resolution[0] = RESOLUTION_512x384;
			gii->refresh = REFRESH_57HZ;
		}
	}
	else
	{
		if((fp = fopen("info.res", "wb")) != (FILE *)0)
		{
			fwrite(gii->resolution, sizeof(resolution_t), 4, fp);
			fwrite(&gii->refresh, sizeof(refresh_t), 1, fp);
			fflush(fp);
			fclose(fp);
		}
		else
		{
			fprintf(stderr, "%s() - Error opening info.res for writting", __FUNCTION__);
		}
	}

	// Make sure the default resolution is valid
	switch(gii->resolution[0])
	{
		case RESOLUTION_400x256:
		case RESOLUTION_512x256:
		case RESOLUTION_512x384:
		{
			break;
		}
		default:
		{
			fprintf(stderr, "%s() - Default resolution is invalid", __FUNCTION__);
			__asm__("	teqi	$0,0");
		}
	}

	// Set the resolution based on the dipswiches
	dip = ~read_dip_direct() << DIP_SHIFT;
	res = gii->resolution[((dip_inputs_t *)&dip)->resolution];

	do
	{
		switch(res)
		{
			case RESOLUTION_400x256:
			{
				vt = &SST_VREZ_400X256_54;
				hres = 400.0f;
				vres = 256.0f;
				is_low_res = gii->low_res_scale;
				tsec = 54;
				break;
			}
			case RESOLUTION_512x256:
			{
				vt = &SST_VREZ_512X256_57;
				hres = 512.0f;
				vres = 256.0f;
				is_low_res = gii->low_res_scale;
				tsec = 57;
				break;
			}
			case RESOLUTION_512x384:
			{
				vt = &myVideoResolution;
				hres = 512.0f;
				vres = 384.0f;
				is_low_res = FALSE;
				tsec = 57;
				break;
			}
			default:
			{
				res = gii->resolution[0];
				break;
			}
		}
	} while(!vt);
	
	/* set up video timimg based on timing structure values */
	grSstVidMode(0, vt);
	
	/* initialize the library */
	grGlideInit();
	
	/* figure out what hardware is really out there */
	if(grSstQueryHardware(&hwconfig) == 0)
	{
		fprintf(stderr, "%s() - Graphics Hardware initialization failure\n", __FUNCTION__);
		__asm__("	teqi	$0,0");
	}
	
	/* select graphics card 0 */
	grSstSelect(0);
	
	/* open the graphics library and set resolution - NOTE:  The values passed */
	/* here are ignored because the grSstVideMode() call is used above. */
	if(grSstOpen(GR_RESOLUTION_400x256, GR_REFRESH_54Hz, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, GR_SMOOTHING_ENABLE, 2) == 0)
	{
		fprintf(stderr, "%s() - Graphics Hardware initialization failure\n", __FUNCTION__);
		__asm__("	teqi	$0,0");
	}
	
	/* set the global horizontal and vertical resolution variables */
	hres = vt->xDimension;
	vres = vt->yDimension;
	
	/* initialize the state of the rendering engine */
	grBufferClear(0xFF000000, 0, GR_WDEPTHVALUE_FARTHEST);
	grTexCombineFunction(GR_TMU0, GR_TEXTURECOMBINE_DECAL);
	guColorCombineFunction(GR_COLORCOMBINE_CCRGB);
	grDepthBufferMode(GR_DEPTHBUFFER_WBUFFER);
	grDepthBufferFunction(GR_CMP_LESS);
	grDepthMask(FXTRUE);

#else

	GrScreenResolution_t resolution = (GrScreenResolution_t)-1;
	int						refresh = GR_REFRESH_57Hz;
	resolution_t			res = RESOLUTION_512x384;
	FILE						*fp;
	unsigned int			dip;

	// Initialize Glide
	grGlideInit();

	// Set a default vertex layout
	grVertexLayout(GR_PARAM_XY,  GR_VERTEX_X_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_RGB, GR_VERTEX_R_OFFSET << 2, GR_PARAM_ENABLE);
	grVertexLayout(GR_PARAM_A,   GR_VERTEX_A_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Z,   GR_VERTEX_OOZ_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_W,   GR_VERTEX_OOW_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST0, GR_VERTEX_SOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q0,  GR_VERTEX_OOW_TMU0_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_ST1, GR_VERTEX_SOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);
	grVertexLayout(GR_PARAM_Q1,  GR_VERTEX_OOW_TMU1_OFFSET << 2, GR_PARAM_DISABLE);

	// Select the first card in the system
	grSstSelect(0);

	// If NOT initializing the CMOS read the info from disk file.  This is the
	// case when this is being used by game diagnostics!!
	if(!gii->sc)
	{
		if((fp = fopen("info.res", "rb")) != (FILE *)0)
		{
			fread(gii->resolution, sizeof(resolution_t), 4, fp);
			fread(&gii->refresh, sizeof(refresh_t), 1, fp);
			fclose(fp);
		}
		else
		{
			fprintf(stderr, "%s() - Error opening info.res for read", __FUNCTION__);
			gii->resolution[0] = RESOLUTION_512x384;
			gii->refresh = REFRESH_57HZ;
		}
	}
	else
	{
		if((fp = fopen("info.res", "wb")) != (FILE *)0)
		{
			fwrite(gii->resolution, sizeof(resolution_t), 4, fp);
			fwrite(&gii->refresh, sizeof(refresh_t), 1, fp);
			fflush(fp);
			fclose(fp);
		}
		else
		{
			fprintf(stderr, "%s() - Error opening info.res for writting", __FUNCTION__);
		}
	}

	// Make sure the default resolution is valid
	switch(gii->resolution[0])
	{
		case RESOLUTION_400x256:
		case RESOLUTION_512x256:
		case RESOLUTION_512x384:
		{
			break;
		}
		default:
		{
			fprintf(stderr, "%s() - Default resolution is invalid", __FUNCTION__);
			__asm__("	teqi	$0,0");
		}
	}

	// Set the resolution based on the dipswiches
	dip = ~read_dip_direct() << DIP_SHIFT;
	res = gii->resolution[((dip_inputs_t *)&dip)->resolution];

	do
	{
		switch(res)
		{
			case RESOLUTION_400x256:
			{
				resolution = GR_RESOLUTION_400x256;
				is_low_res = gii->low_res_scale;
				hres = 400.0f;
				vres = 256.0f;
				break;
			}
			case RESOLUTION_512x256:
			{
				resolution = GR_RESOLUTION_512x256;
				is_low_res = gii->low_res_scale;
				hres = 512.0f;
				vres = 256.0f;
				break;
			}
			case RESOLUTION_512x384:
			{
				resolution = GR_RESOLUTION_512x384;
				hres = 512.0f;
				vres = 384.0f;
				is_low_res = FALSE;
				break;
			}
			default:
			{
				res = gii->resolution[0];
				break;
			}
		}
	} while(resolution < 0);
	
	// Set the specified refresh rate
	switch(gii->refresh)
	{
		case REFRESH_55HZ:
		{
			refresh = GR_REFRESH_55Hz;
			break;
		}
		case REFRESH_56HZ:
		{
			refresh = GR_REFRESH_56Hz;
			break;
		}
		case REFRESH_57HZ:
		{
			refresh = GR_REFRESH_57Hz;
			break;
		}
		case REFRESH_58HZ:
		{
			refresh = GR_REFRESH_58Hz;
			break;
		}
		case REFRESH_59HZ:
		{
			refresh = GR_REFRESH_59Hz;
			break;
		}
		case REFRESH_60HZ:
		{
			refresh = GR_REFRESH_60Hz;
			break;
		}
		default:
		{
			refresh = GR_REFRESH_57Hz;
			break;
		}
	}

	// Initialize for the resolution and refresh rate
	grSstWinOpen(0, resolution, refresh, GR_COLORFORMAT_ARGB, GR_ORIGIN_LOWER_LEFT, 2, 1 );
	
	// Set the rendering buffer
	grRenderBuffer(GR_BUFFER_BACKBUFFER);

	// Initialize the state of the rendering engine
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);
	grBufferClear(0, 0, GR_WDEPTHVALUE_FARTHEST);
	grBufferSwap(1);

	// Initialize texture management
	resetTextureSystem();

	// Turn on write merging in NILE IV
	enable_write_merge();
#endif
}


__asm__("
	.set	noreorder
	.globl	get_text_addr
get_text_addr:
	la	$2,start
	jr	$31
	nop

	.globl	get_text_end_addr
get_text_end_addr:
	la	$2,__tend
	jr	$31
	nop
	.set	reorder
");
#endif
