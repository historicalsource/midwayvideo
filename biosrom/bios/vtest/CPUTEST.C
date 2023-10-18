#include	<stdio.h>
#include	<system.h>
#include	<ioctl.h>
#include	<glide.h>
#include	<pci0646.h>

//#define	CMC_TEST

typedef union point_data
{
	float	fdata;
	int	idata;
} point_data_t;

extern int	screen_width;
extern int	screen_height;

typedef struct	chip_draw_info
{
	int	num_points;
	int	*points;
	int	status;
} chip_draw_info_t;

#define	CHIP_STATUS_UNTESTED		0
#define	CHIP_STATUS_NOT_STUFFED	1
#define	CHIP_STATUS_GOOD			2
#define	CHIP_STATUS_FAILED		3
#define	CHIP_STATUS_TESTING		4

#define	CPU_SCALE					0.35f
#define	CPU_TRANSX					(0.95f - CPU_SCALE)
#define	CPU_TRANSY					(0.95f - CPU_SCALE)
#define	CPU_W	((8.0f*32.0f)+6.0f)
#define	CPU_H	((7.0f*32.0f)+10.0f)


#define	DISK_TEST_SECTORS	2048

static int get_scsi_device_number(void);
void clear_count(void);
int	gcount(void);
void get_mem_size(void);

extern volatile int	__memory_size;
extern int				graphics_initialized;

int	do_comprehensive_tests = 0;
int	burn_loop = 0;
static int	cpu_draw_initialized = 0;

static point_data_t	cpu_points[] = {
((0.0f*32.0f)+ 0.0f)/CPU_W, ((0.0f*32.0f)+ 0.0f)/CPU_H,
((8.0f*32.0f)+ 6.0f)/CPU_W, ((0.0f*32.0f)+ 0.0f)/CPU_H,
((8.0f*32.0f)+ 6.0f)/CPU_W, ((1.0f*32.0f)+ 6.0f)/CPU_H,
((7.0f*32.0f)+23.0f)/CPU_W, ((1.0f*32.0f)+ 6.0f)/CPU_H,
((7.0f*32.0f)+23.0f)/CPU_W, ((1.0f*32.0f)+11.0f)/CPU_H,
((8.0f*32.0f)+ 6.0f)/CPU_W, ((1.0f*32.0f)+11.0f)/CPU_H,
((8.0f*32.0f)+ 6.0f)/CPU_W, ((5.0f*32.0f)+ 2.0f)/CPU_H,
((7.0f*32.0f)+23.0f)/CPU_W, ((5.0f*32.0f)+ 2.0f)/CPU_H,
((7.0f*32.0f)+23.0f)/CPU_W, ((5.0f*32.0f)+ 8.0f)/CPU_H,
((8.0f*32.0f)+ 6.0f)/CPU_W, ((5.0f*32.0f)+ 8.0f)/CPU_H,
((8.0f*32.0f)+ 6.0f)/CPU_W, ((7.0f*32.0f)+10.0f)/CPU_H,
((0.0f*32.0f)+ 0.0f)/CPU_W, ((7.0f*32.0f)+10.0f)/CPU_H,
((0.0f*32.0f)+ 0.0f)/CPU_W, ((4.0f*32.0f)+23.0f)/CPU_H,
((0.0f*32.0f)+17.0f)/CPU_W, ((4.0f*32.0f)+23.0f)/CPU_H,
((0.0f*32.0f)+17.0f)/CPU_W, ((4.0f*32.0f)+18.0f)/CPU_H,
((0.0f*32.0f)+ 0.0f)/CPU_W, ((4.0f*32.0f)+18.0f)/CPU_H,
((0.0f*32.0f)+ 0.0f)/CPU_W, ((0.0f*32.0f)+27.0f)/CPU_H,
((0.0f*32.0f)+17.0f)/CPU_W, ((0.0f*32.0f)+27.0f)/CPU_H,
((0.0f*32.0f)+17.0f)/CPU_W, ((0.0f*32.0f)+22.0f)/CPU_H,
((0.0f*32.0f)+ 0.0f)/CPU_W, ((0.0f*32.0f)+22.0f)/CPU_H,
((0.0f*32.0f)+ 0.0f)/CPU_W, ((0.0f*32.0f)+ 0.0f)/CPU_H,
};

static point_data_t	cpu_rom_chip_points[] = {
((2.0f*32.0f)+04.0f)/CPU_W, ((1.0f*32.0f)+15.0f)/CPU_H,
((3.0f*32.0f)+26.0f)/CPU_W, ((1.0f*32.0f)+15.0f)/CPU_H,
((3.0f*32.0f)+26.0f)/CPU_W, ((2.0f*32.0f)+ 5.0f)/CPU_H,
((2.0f*32.0f)+04.0f)/CPU_W, ((2.0f*32.0f)+ 5.0f)/CPU_H,
((2.0f*32.0f)+04.0f)/CPU_W, ((1.0f*32.0f)+15.0f)/CPU_H
};

static point_data_t	cpu_expansion_rom_chip_points[] = {
((2.0f*32.0f)+04.0f)/CPU_W, ((0.0f*32.0f)+22.0f)/CPU_H,
((3.0f*32.0f)+26.0f)/CPU_W, ((0.0f*32.0f)+22.0f)/CPU_H,
((3.0f*32.0f)+26.0f)/CPU_W, ((1.0f*32.0f)+13.0f)/CPU_H,
((2.0f*32.0f)+04.0f)/CPU_W, ((1.0f*32.0f)+13.0f)/CPU_H,
((2.0f*32.0f)+04.0f)/CPU_W, ((0.0f*32.0f)+22.0f)/CPU_H
};

static point_data_t	cpu_led_chip_points[] = {
((4.0f*32.0f)+19.0f)/CPU_W, ((1.0f*32.0f)+ 5.0f)/CPU_H,
((5.0f*32.0f)+ 0.0f)/CPU_W, ((1.0f*32.0f)+ 5.0f)/CPU_H,
((5.0f*32.0f)+ 0.0f)/CPU_W, ((1.0f*32.0f)+30.0f)/CPU_H,
((4.0f*32.0f)+19.0f)/CPU_W, ((1.0f*32.0f)+30.0f)/CPU_H,
((4.0f*32.0f)+19.0f)/CPU_W, ((1.0f*32.0f)+ 5.0f)/CPU_H
};

static point_data_t	cpu_nile_chip_points[] = {
((2.0f*32.0f)+29.0f)/CPU_W, ((3.0f*32.0f)+11.0f)/CPU_H,
((4.0f*32.0f)+16.0f)/CPU_W, ((3.0f*32.0f)+11.0f)/CPU_H,
((4.0f*32.0f)+16.0f)/CPU_W, ((4.0f*32.0f)+31.0f)/CPU_H,
((2.0f*32.0f)+29.0f)/CPU_W, ((4.0f*32.0f)+31.0f)/CPU_H,
((2.0f*32.0f)+29.0f)/CPU_W, ((3.0f*32.0f)+11.0f)/CPU_H
};

static point_data_t	cpu_cpu_chip_points[] = {
((5.0f*32.0f)+10.0f)/CPU_W, ((3.0f*32.0f)+17.0f)/CPU_H,
((6.0f*32.0f)+15.0f)/CPU_W, ((3.0f*32.0f)+17.0f)/CPU_H,
((6.0f*32.0f)+15.0f)/CPU_W, ((4.0f*32.0f)+22.0f)/CPU_H,
((5.0f*32.0f)+10.0f)/CPU_W, ((4.0f*32.0f)+22.0f)/CPU_H,
((5.0f*32.0f)+10.0f)/CPU_W, ((3.0f*32.0f)+17.0f)/CPU_H
};

static point_data_t	cpu_sdram1_chip_points[] = {
((2.0f*32.0f)+11.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((3.0f*32.0f)+ 5.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((3.0f*32.0f)+ 5.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((2.0f*32.0f)+11.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((2.0f*32.0f)+11.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H
};

static point_data_t	cpu_sdram2_chip_points[] = {
((3.0f*32.0f)+ 7.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((4.0f*32.0f)+ 1.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((4.0f*32.0f)+ 1.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((3.0f*32.0f)+ 7.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((3.0f*32.0f)+ 7.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H
};

static point_data_t	cpu_sdram3_chip_points[] = {
((4.0f*32.0f)+ 3.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((4.0f*32.0f)+29.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((4.0f*32.0f)+29.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((4.0f*32.0f)+ 3.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((4.0f*32.0f)+ 3.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H
};

static point_data_t	cpu_sdram4_chip_points[] = {
((4.0f*32.0f)+31.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((5.0f*32.0f)+25.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H,
((5.0f*32.0f)+25.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((4.0f*32.0f)+31.0f)/CPU_W, ((6.0f*32.0f)+12.0f)/CPU_H,
((4.0f*32.0f)+31.0f)/CPU_W, ((5.0f*32.0f)+25.0f)/CPU_H
};

static point_data_t	cpu_ide_chip_points[] = {
((5.0f*32.0f)+ 7.0f)/CPU_W, ((1.0f*32.0f)+ 6.0f)/CPU_H,
((6.0f*32.0f)+ 7.0f)/CPU_W, ((1.0f*32.0f)+ 6.0f)/CPU_H,
((6.0f*32.0f)+ 7.0f)/CPU_W, ((1.0f*32.0f)+31.0f)/CPU_H,
((5.0f*32.0f)+ 7.0f)/CPU_W, ((1.0f*32.0f)+31.0f)/CPU_H,
((5.0f*32.0f)+ 7.0f)/CPU_W, ((1.0f*32.0f)+ 6.0f)/CPU_H
};

static chip_draw_info_t	cpu_chips[] = {
{sizeof(cpu_rom_chip_points)/sizeof(point_data_t), &cpu_rom_chip_points[0].idata, 0},
{sizeof(cpu_expansion_rom_chip_points)/sizeof(point_data_t), &cpu_expansion_rom_chip_points[0].idata, CHIP_STATUS_NOT_STUFFED},
{sizeof(cpu_sdram1_chip_points)/sizeof(point_data_t), &cpu_sdram1_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram2_chip_points)/sizeof(point_data_t), &cpu_sdram2_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram3_chip_points)/sizeof(point_data_t), &cpu_sdram3_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram4_chip_points)/sizeof(point_data_t), &cpu_sdram4_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_led_chip_points)/sizeof(point_data_t), &cpu_led_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_nile_chip_points)/sizeof(point_data_t), &cpu_nile_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_cpu_chip_points)/sizeof(point_data_t), &cpu_cpu_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_ide_chip_points)/sizeof(point_data_t), &cpu_ide_chip_points[0].idata, CHIP_STATUS_UNTESTED}
};


static char	*c_algs[] = {
"Non-coherent, write-through, no write allocate",
"Non-coherent, write-through, write allocate",
"Uncached",
"Non-coherent, write-back",
"Reserved",
"Reserved",
"Reserved",
"Reserved",
};

static char	*wb_rate[] = {
"DDDD",
"DDxDDx",
"DDxxDDxx",
"DxDxDxDx",
"DDxxxDDxxx",
"DDxxxxDDxxxx",
"DxxDxxDxxDxx",
"DDxxxxxDDxxxxx",
"DxxxDxxxDxxxDxxx",
"Reserved"
"Reserved"
"Reserved"
"Reserved"
"Reserved"
"Reserved"
"Reserved"
};

int get_config_reg(void);
int get_prid_reg(void);

__asm__("
	.set	noreorder
	.globl	get_config_reg
get_config_reg:
	jr	$31
	mfc0	$2,$16
	.set	reorder

	.set	noreorder
	.globl	get_prid_reg
get_prid_reg:
	jr	$31
	mfc0	$2,$15
	.set	reorder
");

// Search the PCI bus for the slot number of the SCSI card we are using
static int get_scsi_device_number (void)
{
	int	i;
	int	id;

	for(i = 0; i < 6; i++)
	{
		id = get_pci_config_reg(i, 0);
		if((id & 0xffff) == 0x1000)		/* 0x1000 = vendor id of symbios 53c810 */
		{
			return(i);
		}
	}
	return(-1);
}

static void show_processor_info(void)
{
	int	config_reg = get_config_reg();
	int	prid = get_prid_reg();
	int	size;
	int	csize;
	int	nile4_version;

	set_fcolor(0xffff);
	printf("Processor Information:\n");
	printf("Processor ID:                 %x\n", prid & 0xffff);
	printf("Kseg0 Algorithim:             %s\n", c_algs[config_reg & 7]);
	printf("Data Cache Line Size:         %d (bytes)\n", (config_reg & 0x10 ? 32 : 16));
	printf("Instruction Cache Line Size:  %d (bytes)\n", (config_reg & 0x20 ? 32 : 16));
	printf("Data Cache Size:              ");
	size = config_reg >> 6;
	size &= 7;
	size += 12;
	printf("%d (bytes)\n", (1<<size));
	printf("Instruction Cache Size:       ");
	size = config_reg >> 9;
	size &= 7;
	size += 12;
	printf("%d (bytes)\n", (1<<size));
	printf("Endianess:                    ");
	if(config_reg & 0x8000)
	{
		printf("BIG\n");
	}
	else
	{
		printf("LITTLE\n");
	}
	printf("Writeback data rate:          %s\n", wb_rate[(config_reg >> 24) & 0xf]);
	printf("System Clock Ratio:           Divide by %d\n", ((config_reg >> 28) & 0x7) + 2);
	nile4_version = get_pci_config_reg(0, 2) & 0xff;
	printf("NILE IV Revision:             %d\n\n", nile4_version);
}

static void show_pci_devices(void)
{
	int	slot;
	int	dv;
	int	class;

	printf("PCI Devices Detected\n");
	for(slot = 0; slot < 10; slot++)
	{
		dv = get_pci_config_reg(slot, 0);
		if(dv != -1 && dv != 0)
		{
			class = get_pci_config_reg(slot, 2);
			class >>= 8;
			class &= 0xffffff;
			if((class & 0xfffff0) == 0x010180)
			{
				class &= 0xffff00;
			}
			printf("Slot %d: ", slot);
			set_fcolor(0x07ff);
			printf("%s - %s\n", pciGetVendorName((dv & 0xffff)), pciGetClassName(class, (dv&0xffff)));
			set_fcolor(-1);
		}
	}
	printf("\n");
}

static void memperf(void)
{
	register int	i;
	int	t;
	int	tot;
	int	lag = 0;

	printf("\nMemory performance tests\n");

	clear_count();
	lag = gcount();

	tot = 0;
	__asm__("	la	$8,0xa0200000");
	for(i = 0; i < 1000; i++)
	{
		clear_count();
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		t = gcount();
		tot += ((t - lag) * NANOS_PER_TICK);
	}
	printf("Uncached Reads - %f (MB/s)\n", \
		(1.0f/(((float)tot/1000000000.0f)/128000.0f)/1000000.0f));

	tot = 0;
	__asm__("	la	$8,0xa0200000");
	for(i = 0; i < 1000; i++)
	{
		clear_count();
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,8($8)");
		__asm__("	sd	$0,16($8)");
		__asm__("	sd	$0,24($8)");
		__asm__("	sd	$0,32($8)");
		__asm__("	sd	$0,40($8)");
		__asm__("	sd	$0,48($8)");
		__asm__("	sd	$0,56($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,8($8)");
		__asm__("	sd	$0,16($8)");
		__asm__("	sd	$0,24($8)");
		__asm__("	sd	$0,32($8)");
		__asm__("	sd	$0,40($8)");
		__asm__("	sd	$0,48($8)");
		__asm__("	sd	$0,56($8)");
		t = gcount();
		tot += ((t - lag) * NANOS_PER_TICK);
	}
	printf("Uncached Writes - %f (MB/s)\n", \
		(1.0f/(((float)tot/1000000000.0f)/128000.0f)/1000000.0f));

	tot = 0;
	__asm__("	la	$8,0x80200000");
	for(i = 0; i < 1000; i++)
	{
		clear_count();
		__asm__("	cache	0xd,0($8)");
		__asm__("	cache	0x19,0($8)");
		__asm__("	cache	0xd,0($8)");
		__asm__("	cache	0x19,0($8)");
		t = gcount();
		tot += ((t - lag) * NANOS_PER_TICK);
	}
	printf("Burst Writes - %f (MB/s)\n", \
		(1.0f/(((float)tot/1000000000.0f)/128000.0f)/1000000.0f));

	tot = 0;
	__asm__("	la	$8,0x80200000");
	for(i = 0; i < 1000; i++)
	{
		clear_count();
		__asm__("	cache	0x15,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	cache	0x15,0($8)");
		__asm__("	ld	$0,0($8)");
		t = gcount();
		tot += ((t - lag) * NANOS_PER_TICK);
	}
	printf("Burst Reads - %f (MB/s)\n", \
		(1.0f/(((float)tot/1000000000.0f)/256000.0f)/1000000.0f));

	tot = 0;
	__asm__("	la	$8,0x80200000");
	for(i = 0; i < 1000; i++)
	{
		clear_count();
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		__asm__("	ld	$0,0($8)");
		t = gcount();
		tot += ((t - lag) * NANOS_PER_TICK);
	}
	printf("Cache Reads - %f (MB/s)\n", \
		(1.0f/(((float)tot/1000000000.0f)/128000.0f)/1000000.0f));

	tot = 0;
	__asm__("	la	$8,0x80200000");
	for(i = 0; i < 1000; i++)
	{
		clear_count();
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		__asm__("	sd	$0,0($8)");
		t = gcount();
		tot += ((t - lag) * NANOS_PER_TICK);
	}
	printf("Cache Writes - %f (MB/s)\n", \
		(1.0f/(((float)tot/1000000000.0f)/128000.0f)/1000000.0f));

}

static void pcicommtest(void)
{
	int	val;

	printf("PCI Communications Test - ");
	val = get_pci_config_reg(5, 0);
	if(val != 0x06461095)
	{
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}
	printf("PASS\n");
}

static void pci_data_test(void)
{
	int	slot;
	int	data;
	int	tmp;

	printf("PCI Data Test - ");
	for(slot = 0; slot < 6; slot++)
	{
		data = get_pci_config_reg(slot, 0);
		if(data == 0x06461095)
		{
			break;
		}
	}
	if(slot == 6)
	{
		printf("FAIL - Can not find IDE Controller\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}

	tmp = get_pci_config_reg(slot, 19);
	tmp |= 0x01010101;
	put_pci_config_reg(slot, 19, tmp);

	put_pci_config_reg(slot, 11, 0x00000000);
	tmp = get_pci_config_reg(slot, 11);
	if(tmp != 0x00000000)
	{
		printf("FAIL - %08.8X -> %08.8X\n", 0x00000000, tmp);
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}

	for(data = 1; data != 0; data <<= 1)
	{
		put_pci_config_reg(slot, 11, data);
		tmp = get_pci_config_reg(slot, 11);
		if(tmp != data)
		{
			printf("FAIL - %08.8X -> %08.8X\n", data, tmp);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
		}
	}

	data = 0x7fffffff;
	while(data != 0xffffffff)
	{
		put_pci_config_reg(slot, 11, data);
		tmp = get_pci_config_reg(slot, 11);
		if(tmp != data)
		{
			printf("FAIL - %08.8X -> %08.8X\n", data, tmp);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			return;
		}
		data >>= 1;
		data |= 0x80000000;
	}

	put_pci_config_reg(slot, 11, 0xffffffff);
	tmp = get_pci_config_reg(slot, 11);
	if(tmp != 0xffffffff)
	{
		printf("FAIL - %08.8X -> %08.8X\n", 0xffffffff, tmp);
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}

	put_pci_config_reg(slot, 11, 0xaa5555aa);
	tmp = get_pci_config_reg(slot, 11);
	if(tmp != 0xaa5555aa)
	{
		printf("FAIL - %08.8X -> %08.8X\n", 0xaa5555aa, tmp);
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}

	put_pci_config_reg(slot, 11, 0x55aaaa55);
	tmp = get_pci_config_reg(slot, 11);
	if(tmp != 0x55aaaa55)
	{
		printf("FAIL - %08.8X -> %08.8X\n", 0x55aaaa55, tmp);
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		return;
	}

	printf("PASS\n");
}

extern int	nile4_version;

void lowlevel_cputest(void)
{
	printf("\n\nLow Level CPU Tests\n");
	printf("Memory Size: %d (MB)\n\n", __memory_size / (1024*1024));
	show_processor_info();
	pcicommtest();
	pci_data_test();
	printf("\n");
	show_pci_devices();
}

static void cpu_led_test(void)
{
	int	i;
	int	tmp;

	set_fcolor(0xffff);
	printf("CPU LED Test - ");
	for(i = 0; i < 0x100; i++)
	{
		*((volatile char *)LED_ADDR) = i;
		tmp = *((volatile char *)LED_ADDR) & 0xff;
		if(tmp != i)
		{
			set_fcolor(0xf800);
			printf("FAIL - 0x%02.2X -> 0x%02.2X\n", i, tmp);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[6].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void diskinittest(void)
{
	set_fcolor(0xffff);
	printf("Disk Initialization Test - ");
	if(!ide_init())
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		set_fcolor(0xffff);
		cpu_chips[9].status = CHIP_STATUS_FAILED;
		return;
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static unsigned char	sbuffer[512 + 32];
volatile static int	disk_int_received;
extern void				(*callback)(int);

static void disk_int_handler(int status)
{
	disk_int_received++;
}

static unsigned char *get_sbuf_ptr(void)
{
	unsigned char	*b;

	b = sbuffer;
	while((int)b & 0x1f)
	{
		b++;
	}
	return(b);
}

static void diskreadinttest(void)
{
	int	i;
	unsigned char	*b;

	b = get_sbuf_ptr();
	set_fcolor(0xffff);
	printf("Disk Read Interrupt Test - ");
	disk_int_received = 0;
	callback = disk_int_handler;
	if(_SecReads(0, (unsigned long *)b, 1))
	{
		callback = (void *)0;
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		set_fcolor(0xffff);
		cpu_chips[9].status = CHIP_STATUS_FAILED;
		return;
	}
	for(i = 0; i < 50000000; i++)
	{
		if(disk_int_received)
		{
			callback = (void *)0;
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	callback = (void *)0;
	set_fcolor(0xf800);
	printf("FAIL - Interrupt NOT detected\n");
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
	set_fcolor(0xffff);
	cpu_chips[9].status = CHIP_STATUS_FAILED;
}


static void diskwriteinttest(void)
{
	int	i;
	unsigned char	*b;

	b = get_sbuf_ptr();
	set_fcolor(0xffff);
	printf("Disk Write Interrupt Test - ");
	disk_int_received = 0;
	callback = disk_int_handler;
	if(_SecWrites(0, (unsigned long *)b, 1))
	{
		callback = (void *)0;
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		set_fcolor(0xffff);
		cpu_chips[9].status = CHIP_STATUS_FAILED;
		return;
	}
	for(i = 0; i < 50000000; i++)
	{
		if(disk_int_received)
		{
			callback = (void *)0;
			set_fcolor(0x07e0);
			printf("PASS\n");
			set_fcolor(0xffff);
			return;
		}
	}
	callback = (void *)0;
	set_fcolor(0xf800);
	printf("FAIL - Interrupt NOT detected\n");
	if(do_comprehensive_tests)
	{
		while(1) ;
	}
	set_fcolor(0xffff);
	cpu_chips[9].status = CHIP_STATUS_FAILED;
}


static void diskreadtest(void)
{
	int	i;
	unsigned char	*b;

	b = get_sbuf_ptr();
	set_fcolor(0xffff);
	printf("Disk Read Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(_SecReads(i, (unsigned long *)b, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void diskcachereadtest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Disk Cache Read Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(!SecReads(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void diskwritetest(void)
{
	int	i;
	unsigned char	*b;

	b = get_sbuf_ptr();
	set_fcolor(0xffff);
	printf("Disk Write Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(_SecWrites(i, (unsigned long *)b, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void diskcachewritetest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Disk Cache Write Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(!SecWrites(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void diskreadwritetest(void)
{
	int	i;
	int	j;
	unsigned char	data = 1;
	unsigned char	*b;
	unsigned char	*bsave;

	b = get_sbuf_ptr();
	set_fcolor(0xffff);
	printf("Disk Write/Read Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		for(j = 0; j < 512; j++)
		{
			b[j] = data;
			data++;
			if(!data)
			{
				data = 1;
			}
		}
		if(_SecWrites(i, (unsigned long *)b, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	data = 1;
	b = get_sbuf_ptr();
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(_SecReads(i, (unsigned long *)b, 1))
		{
			set_fcolor(0xf800);
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
		for(j = 0; j < 512; j++)
		{
			if(b[j] != data)
			{
				set_fcolor(0xf800);
				printf("FAIL\n");
				if(do_comprehensive_tests)
				{
					while(1) ;
				}
				set_fcolor(0xffff);
				cpu_chips[9].status = CHIP_STATUS_FAILED;
				return;
			}
			data++;
			if(!data)
			{
				data = 1;
			}
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void diskcachereadwritetest(void)
{
	int	i;
	int	j;
	unsigned char	data = 1;

	set_fcolor(0xffff);
	printf("Disk Cache Write/Read Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		for(j = 0; j < 512; j++)
		{
			sbuffer[j] = data;
			data++;
			if(!data)
			{
				data = 1;
			}
		}
		if(!SecWrites(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL - WRITE\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	data = 1;
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(!SecReads(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL - READ\n");
			if(do_comprehensive_tests)
			{
				while(1) ;
			}
			set_fcolor(0xffff);
			cpu_chips[9].status = CHIP_STATUS_FAILED;
			return;
		}
		for(j = 0; j < 512; j++)
		{
			if(sbuffer[j] != data)
			{
				set_fcolor(0xf800);
				printf("FAIL - DATA - SECTOR: %d - OFFSET %d\n", i, j);
				if(do_comprehensive_tests)
				{
					while(1) ;
				}
				set_fcolor(0xffff);
				cpu_chips[9].status = CHIP_STATUS_FAILED;
				return;
			}
			data++;
			if(!data)
			{
				data = 1;
			}
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static void rom_checksum(void)
{
	int	i;
	int	cksum = 0;
	int	scksum;

	set_fcolor(0xffff);
	printf("ROM Checksum - ");
	for(i = 0; i < 524284; i++)
	{
		cksum += (int)*((volatile char *)(0x9fc00000 + i));
	}
	scksum = *((volatile int *)(0x9fc00000 + 524284));
	if(cksum != scksum)
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		cpu_chips[0].status = CHIP_STATUS_FAILED;
	}
	else
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
	}
	set_fcolor(0xffff);
}

static void expansion_rom_checksum(void)
{
	int	i;
	int	cksum = 0;
	int	scksum;

	set_fcolor(0xffff);
	printf("EXP ROM Checksum - ");
	if(*((volatile int *)0x81700000) != 0x04030201)
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		cpu_chips[1].status = CHIP_STATUS_FAILED;
		return;
	}
	for(i = 0; i < 524284; i++)
	{
		cksum += (int)*((volatile char *)(0x81700000 + i));
	}
	scksum = *((volatile int *)(0x81700000 + 524284));
	if(cksum != scksum)
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		if(do_comprehensive_tests)
		{
			while(1) ;
		}
		cpu_chips[1].status = CHIP_STATUS_FAILED;
	}
	else
	{
		set_fcolor(0x07e0);
		printf("PASS\n");
	}
	set_fcolor(0xffff);
}

static void scale_up(int num, point_data_t *pnts, float scale_factor)
{
	num >>= 1;
	while(num--)
	{
		pnts->fdata *= (scale_factor * (float)screen_width);
		++pnts;
		pnts->fdata *= (scale_factor * (float)screen_height);
		++pnts;
	}
}

static void to_int(int num, point_data_t *pnts)
{
	while(num--)
	{
		pnts->idata = (int)pnts->fdata;
		pnts++;
	}
}

static void center(int num, point_data_t *pnts, float scale_factor)
{
	float	factor = (1.0f - scale_factor) / 2.0f;

	num >>= 1;
	while(num--)
	{
		pnts->fdata += (factor * (float)screen_width);
		++pnts;
		pnts->fdata += (factor * (float)screen_height);
		++pnts;
	}
}

static void translate(int num, point_data_t *pnts, float xtrans, float ytrans)
{
	num >>= 1;
	while(num--)
	{
		pnts->fdata += (xtrans * (float)screen_width);
		++pnts;
		pnts->fdata += (ytrans * (float)screen_height);
		++pnts;
	}
}

static void init_cpu_drawing(void)
{
	if(cpu_draw_initialized)
	{
		return;
	}
	cpu_draw_initialized = 1;
	scale_up(sizeof(cpu_points)/sizeof(point_data_t), cpu_points, CPU_SCALE);
	translate(sizeof(cpu_points)/sizeof(point_data_t), cpu_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_points)/sizeof(point_data_t), cpu_points);
	scale_up(sizeof(cpu_rom_chip_points)/sizeof(point_data_t), cpu_rom_chip_points, CPU_SCALE);
	translate(sizeof(cpu_rom_chip_points)/sizeof(point_data_t), cpu_rom_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_rom_chip_points)/sizeof(point_data_t), cpu_rom_chip_points);
	scale_up(sizeof(cpu_expansion_rom_chip_points)/sizeof(point_data_t), cpu_expansion_rom_chip_points, CPU_SCALE);
	translate(sizeof(cpu_expansion_rom_chip_points)/sizeof(point_data_t), cpu_expansion_rom_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_expansion_rom_chip_points)/sizeof(point_data_t), cpu_expansion_rom_chip_points);
	scale_up(sizeof(cpu_sdram1_chip_points)/sizeof(point_data_t), cpu_sdram1_chip_points, CPU_SCALE);
	translate(sizeof(cpu_sdram1_chip_points)/sizeof(point_data_t), cpu_sdram1_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_sdram1_chip_points)/sizeof(point_data_t), cpu_sdram1_chip_points);
	scale_up(sizeof(cpu_sdram2_chip_points)/sizeof(point_data_t), cpu_sdram2_chip_points, CPU_SCALE);
	translate(sizeof(cpu_sdram2_chip_points)/sizeof(point_data_t), cpu_sdram2_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_sdram2_chip_points)/sizeof(point_data_t), cpu_sdram2_chip_points);
	scale_up(sizeof(cpu_sdram3_chip_points)/sizeof(point_data_t), cpu_sdram3_chip_points, CPU_SCALE);
	translate(sizeof(cpu_sdram3_chip_points)/sizeof(point_data_t), cpu_sdram3_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_sdram3_chip_points)/sizeof(point_data_t), cpu_sdram3_chip_points);
	scale_up(sizeof(cpu_sdram4_chip_points)/sizeof(point_data_t), cpu_sdram4_chip_points, CPU_SCALE);
	translate(sizeof(cpu_sdram4_chip_points)/sizeof(point_data_t), cpu_sdram4_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_sdram4_chip_points)/sizeof(point_data_t), cpu_sdram4_chip_points);
	scale_up(sizeof(cpu_led_chip_points)/sizeof(point_data_t), cpu_led_chip_points, CPU_SCALE);
	translate(sizeof(cpu_led_chip_points)/sizeof(point_data_t), cpu_led_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_led_chip_points)/sizeof(point_data_t), cpu_led_chip_points);
	scale_up(sizeof(cpu_nile_chip_points)/sizeof(point_data_t), cpu_nile_chip_points, CPU_SCALE);
	translate(sizeof(cpu_nile_chip_points)/sizeof(point_data_t), cpu_nile_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_nile_chip_points)/sizeof(point_data_t), cpu_nile_chip_points);
	scale_up(sizeof(cpu_cpu_chip_points)/sizeof(point_data_t), cpu_cpu_chip_points, CPU_SCALE);
	translate(sizeof(cpu_cpu_chip_points)/sizeof(point_data_t), cpu_cpu_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_cpu_chip_points)/sizeof(point_data_t), cpu_cpu_chip_points);
	scale_up(sizeof(cpu_ide_chip_points)/sizeof(point_data_t), cpu_ide_chip_points, CPU_SCALE);
	translate(sizeof(cpu_ide_chip_points)/sizeof(point_data_t), cpu_ide_chip_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_ide_chip_points)/sizeof(point_data_t), cpu_ide_chip_points);
}

static void draw_chip(chip_draw_info_t *cdi)
{
	if(cdi->status == CHIP_STATUS_UNTESTED)
	{
		set_fcolor(0xffff);
	}
	else if(cdi->status == CHIP_STATUS_NOT_STUFFED)
	{
		set_fcolor(0x7bef);
	}
	else if(cdi->status == CHIP_STATUS_GOOD)
	{
		set_fcolor(0x07e0);
	}
	else if(cdi->status == CHIP_STATUS_FAILED)
	{
		set_fcolor(0xf800);
	}
	else if(cdi->status == CHIP_STATUS_TESTING)
	{
		set_fcolor(0xffe0);
	}
	if(cdi->status <= CHIP_STATUS_NOT_STUFFED)
	{
		draw_polyline(cdi->num_points, cdi->points);
	}
	else
	{
		filled_rect(cdi->points[0], cdi->points[1], cdi->points[2]-cdi->points[0]+1, cdi->points[5]-cdi->points[1]+1);
	}
}

static void draw_cpu_chips(void)
{
	int	i;

	for(i = 0; i < sizeof(cpu_chips)/sizeof(chip_draw_info_t); i++)
	{
		draw_chip(&cpu_chips[i]);
	}
}

static void draw_cpu(void)
{
	set_fcolor(0xffff);
	draw_polyline(sizeof(cpu_points)/sizeof(point_data_t), &cpu_points[0].idata);
	draw_cpu_chips();
}

static void red_screen(void)
{
	int	i;

	set_fcolor((0x1f<<11));
	filled_rect(0, 0, screen_width, screen_height);
	for(i = 0; i < 3000000; i++)
	{
		delay_us(1);
	}
}

static void green_screen(void)
{
	int	i;

	set_fcolor((0x3f<<5));
	filled_rect(0, 0, screen_width, screen_height);
	for(i = 0; i < 3000000; i++)
	{
		delay_us(1);
	}
}

static void blue_screen(void)
{
	int	i;

	set_fcolor((0x1f<<0));
	filled_rect(0, 0, screen_width, screen_height);
	for(i = 0; i < 3000000; i++)
	{
		delay_us(1);
	}
}

static void white_screen(void)
{
	int	i;

	set_fcolor(0xffff);
	filled_rect(0, 0, screen_width, screen_height);
	for(i = 0; i < 3000000; i++)
	{
		delay_us(1);
	}
}

static void gray_screen(void)
{
	int	i;

	set_fcolor((0x0f<<11)|(0x1f<<5)|(0xf<<0));
	filled_rect(0, 0, screen_width, screen_height);
	for(i = 0; i < 3000000; i++)
	{
		delay_us(1);
	}
}

static col_bar_colors[] = {
(0x1f<<11),
(0x3f<<5),
(0x1f<<0),
0,
(0x1f<<11)|(0x3f<<5),
(0x1f<<11)|(0x1f<<0),
(0x3f<<5)|(0x1f<<0),
(0x1f<<11)|(0x3f<<5)|(0x1f<<0),
};

static void color_bars(void)
{
	int	i;
	int	bar_wide;

	bar_wide = screen_width / 8;
	for(i = 0; i < 8; i++)
	{
		set_fcolor(col_bar_colors[i]);
		filled_rect(i*bar_wide, 0, (i*bar_wide)+bar_wide, screen_height);
	}
	for(i = 0; i < 3000000; i++)
	{
		delay_us(1);
	}
}

static void do_kmalloc_test(void);
#ifdef VMM
void vmm_init(void);

static void init_part_table(void)
{
	partition_table_t	*pt;

	pt = ide_get_partition_table();
	pt->num_partitions = 3;
	pt->partition[1].starting_block = 16;
	pt->partition[1].num_blocks = 1000000;
	pt->partition[1].partition_type = 1;
	pt->partition[2].starting_block = pt->partition[1].starting_block + pt->partition[1].num_blocks;
	pt->partition[2].num_blocks = 65536;
	pt->partition[2].partition_type = 3;
}

static void memtest_tlb_cache_words(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned int	data = 1;
	unsigned int	sdata;
	unsigned int	tdata;
	unsigned int	*addr;
	static unsigned int	*base = (void *)0;
	static int				test_amount;
	int				error_detect;
	int				i;

init_part_table();
	if(!base)
	{
		vmm_init();
		test_amount = getmemsize();
		base = (unsigned int *)_sbrk(test_amount);
	}
	while(1)
	{
		printf("\f");
		printf("%f (MB) Word Wide Virtual Memory Test\n\n", (float)test_amount/(1024.0f*1024.0f));
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %f (MB)\n", (float)pass * ((float)test_amount/(1024.0f*1024.0f)));
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = base;
		printf("Write Pass\n");
		for(i = 0; i < test_amount/sizeof(data); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = base;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < test_amount/sizeof(data); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}
#endif

static void test04(void)
{
   GrVertex vtxA, vtxB, vtxC;
	int	i;

	/* Set up Render State - gouraud shading */
	grColorCombine( GR_COMBINE_FUNCTION_LOCAL,
		GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_LOCAL_ITERATED,
		GR_COMBINE_OTHER_NONE,
		FXFALSE );
    
	grBufferClear( 0x00, 0, GR_WDEPTHVALUE_FARTHEST );

	vtxA.x = ( 0.3f * 512.0f ), vtxA.y = ( 0.3f * 384.0f );
	vtxA.r = 255.0f, vtxA.g = 0.0f, vtxA.b = 0.0f;

	vtxB.x = ( 0.8f * 512.0f ), vtxB.y = ( 0.4f * 384.0f );
	vtxB.r = 0.0f, vtxB.g = 255.0f, vtxB.b = 0.0f;

	vtxC.x = ( 0.5f * 512.0f ), vtxC.y = ( 0.8f * 384.0f );
	vtxC.r = 0.0f, vtxC.g = 0.0f, vtxC.b = 255.0f;

	grDrawTriangle( &vtxA, &vtxB, &vtxC );

	grBufferSwap( 1 );

	for(i = 0; i < 1000000; i++)
	{
		delay_us(1);
	}
}

#define	RAMP

static void test_gray(void)
{
   GrVertex vtxA, vtxB, vtxC, vtxD;
	int	i;

	/* Set up Render State - gouraud shading */
	grColorCombine( GR_COMBINE_FUNCTION_LOCAL,
		GR_COMBINE_FACTOR_NONE,
		GR_COMBINE_LOCAL_ITERATED,
		GR_COMBINE_OTHER_NONE,
		FXFALSE );
    
	grBufferClear( 0x00, 0, GR_WDEPTHVALUE_FARTHEST );

	vtxA.x = ( 0.0f * 512.0f ), vtxA.y = ( 0.0f * 384.0f );
#ifdef RAMP
	vtxA.r = 0.0f, vtxA.g = 0.0f, vtxA.b = 0.0f;
#else
	vtxA.r = 255.0f, vtxA.g = 255.0f, vtxA.b = 255.0f;
#endif

	vtxB.x = ( 1.0f * 511.0f ), vtxB.y = ( 0.0f * 384.0f );
	vtxB.r = 255.0f, vtxB.g = 255.0f, vtxB.b = 255.0f;

	vtxC.x = ( 1.0f * 511.0f ), vtxC.y = ( 1.0f * 383.0f );
	vtxC.r = 255.0f, vtxC.g = 255.0f, vtxC.b = 255.0f;

	vtxD.x = ( 0.0f * 511.0f ), vtxD.y = ( 1.0f * 383.0f );
#ifdef RAMP
	vtxD.r = 0.0f, vtxD.g = 0.0f, vtxD.b = 0.0f;
#else
	vtxD.r = 255.0f, vtxD.g = 255.0f, vtxD.b = 255.0f;
#endif

	grDrawTriangle( &vtxA, &vtxB, &vtxC );
	grDrawTriangle( &vtxC, &vtxD, &vtxA );

	grBufferSwap( 1 );

while(1) ;
	for(i = 0; i < 1000000; i++)
	{
		delay_us(1);
	}
}

void highlevel_cputest(void)
{
	int	i;

//do_kmalloc_test();
	init_cpu_drawing();
//	enable_ip(NILE4_INT);
	if(graphics_initialized)
	{
		red_screen();
		blue_screen();
		white_screen();
		gray_screen();
		color_bars();
		test04();
	}

	printf("\f");
	set_fcolor(0xffff);
	printf("\nHigh Level CPU Tests\n\n");
	printf("Memory Size: %d (MB)\n\n", __memory_size / (1024*1024));
	draw_cpu();
	if((__memory_size / (1024*1024)) != 8)
	{
		cpu_chips[2].status = CHIP_STATUS_FAILED;
		cpu_chips[3].status = CHIP_STATUS_FAILED;
		cpu_chips[4].status = CHIP_STATUS_FAILED;
		cpu_chips[5].status = CHIP_STATUS_FAILED;
		draw_chip(&cpu_chips[2]);
		draw_chip(&cpu_chips[3]);
		draw_chip(&cpu_chips[4]);
		draw_chip(&cpu_chips[5]);
	}
	show_processor_info();
	show_pci_devices();
	cpu_chips[0].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[0]);
	rom_checksum();
	if(cpu_chips[0].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[0].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[0]);

	if(do_comprehensive_tests)
	{
		cpu_chips[1].status = CHIP_STATUS_TESTING;
		draw_chip(&cpu_chips[1]);
		expansion_rom_checksum();
		if(cpu_chips[1].status != CHIP_STATUS_FAILED)
		{
			cpu_chips[1].status = CHIP_STATUS_GOOD;
		}
		draw_chip(&cpu_chips[1]);
	}

	cpu_chips[6].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[6]);
	cpu_led_test();
	if(cpu_chips[6].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[6].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[6]);
	cpu_chips[9].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[9]);
	diskinittest();
	diskreadinttest();
	if(do_comprehensive_tests)
	{
		diskwriteinttest();
	}
	diskreadtest();
	if(do_comprehensive_tests)
	{
		diskwritetest();
		diskreadwritetest();
	}
	diskcachereadtest();
	if(do_comprehensive_tests)
	{
		diskcachewritetest();
		diskcachereadwritetest();
	}
	if(cpu_chips[9].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[9].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[9]);
	if(!graphics_initialized)
	{
		return;
	}
	for(i = 0; i < sizeof(cpu_chips)/sizeof(chip_draw_info_t); i++)
	{
		if(cpu_chips[i].status == CHIP_STATUS_FAILED)
		{
			for(i = 0; i < 5000000; i++)
			{
				delay_us(1);
			}
			break;
		}
	}
	set_fcolor(0xffff);
//	memperf();
}

static void memtest_cache_bytes(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned char	data = 1;
	unsigned char	sdata;
	unsigned char	tdata;
	unsigned char	*addr;
	int				error_detect;
	int				i;

	while(pass < 16)
	{
		printf("\f");
		printf("Byte Wide Memory Test (cached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned char *)0x80200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*2048); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned char *)0x80200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*2048); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_cache_shorts(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned short	data = 1;
	unsigned short	sdata;
	unsigned short	tdata;
	unsigned short	*addr;
	int				error_detect;
	int				i;

	while(pass < 16)
	{
		printf("\f");
		printf("Short Wide Memory Test (cached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned short *)0x80200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*1024); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned short *)0x80200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*1024); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_cache_words(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned int	data = 1;
	unsigned int	sdata;
	unsigned int	tdata;
	unsigned int	*addr;
	int				error_detect;
	int				i;

	while(pass < 16)
	{
		printf("\f");
		printf("Word Wide Memory Test (cached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned int *)0x80200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*512); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned int *)0x80200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*512); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_cache_dwords(void)
{
	unsigned long long	data = 1;
	unsigned long long	sdata;
	unsigned long long	tdata;
	unsigned long long	*addr;
	int						pass = 0;
	int						errors = 0;
	int						pass_errors = 0;
	int						error_detect;
	int						i;

	while(pass < 16)
	{
		printf("\f");
		printf("DWord Wide Memory Test (cached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned long long *)0x80200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*256); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr = data;
			addr++;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned long long *)0x80200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*256); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr;
			addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_nocache_bytes(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned char	data = 1;
	unsigned char	sdata;
	unsigned char	tdata;
	unsigned char	*addr;
	int				error_detect;
	int				i;

	while(pass < 4)
	{
		printf("\f");
		printf("Byte Wide Memory Test (uncached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned char *)0xa0200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*2048); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned char *)0xa0200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*2048); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_nocache_shorts(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned short	data = 1;
	unsigned short	sdata;
	unsigned short	tdata;
	unsigned short	*addr;
	int				error_detect;
	int				i;

	while(pass < 4)
	{
		printf("\f");
		printf("Short Wide Memory Test (uncached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned short *)0xa0200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*1024); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned short *)0xa0200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*1024); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_nocache_words(void)
{
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	unsigned int	data = 1;
	unsigned int	sdata;
	unsigned int	tdata;
	unsigned int	*addr;
	int				error_detect;
	int				i;

	while(pass < 4)
	{
		printf("\f");
		printf("Word Wide Memory Test (uncached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned int *)0xa0200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*512); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr++ = data;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned int *)0xa0200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*512); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static void memtest_nocache_dwords(void)
{
	unsigned long long	data = 1;
	unsigned long long	sdata;
	unsigned long long	tdata;
	unsigned long long	*addr;
	int				pass = 0;
	int				errors = 0;
	int				pass_errors = 0;
	int				error_detect;
	int				i;

	while(pass < 4)
	{
		printf("\f");
		printf("DWord Wide Memory Test (uncached space)\n\n");
		printf("Pass Number: %d\n", pass);
		printf("Bytes Tested: %d (MB)\n", pass * 4);
		printf("Bytes In error: %d\n", errors);
		printf("Passes with errors: %d\n\n", pass_errors);

		sdata = data;
		addr = (unsigned long long *)0xa0200000;
		printf("Write Pass\n");
		for(i = 0; i < (2048*256); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			*addr = data;
			addr++;
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);

		data = sdata;
		addr = (unsigned long long *)0xa0200000;
		printf("\nRead Pass\n");
		error_detect = 0;
		for(i = 0; i < (2048*256); i++)
		{
			if(!((int)addr & 0xffff))
			{
				printf("Address: 0x%08.8X\r", (int)addr);
			}
			tdata = *addr;
			addr++;
			if(tdata != data)
			{
				if(!error_detect)
				{
					error_detect = 1;
					pass_errors++;
				}
				errors++;
			}
			++data;
			if(!data)
			{
				data = 1;
			}
		}
		printf("Address: 0x%08.8X\r", (int)addr);
		pass++;
	}
	if(do_comprehensive_tests && pass_errors)
	{
		while(1) ;
	}
}

static unsigned char	dt_buf[(512*16)+32];
static unsigned char	dt_buf1[(512*16)+32];

static int memcmp(unsigned char *b, unsigned char *b1, int bytes)
{
	while(bytes--)
	{
		if(*b != *b1)
		{
			return(1);
		}
		b++;
		b1++;
	}
	return(0);
}

static void do_disk_torture_test(void)
{
	unsigned char	data = 1;
	int				i;
	int				offset;
	int				pass_count = 0;
	int				pre_write_seek_failures = 0;
	int				write_failures = 0;
	int				pre_read_seek_failures = 0;
	int				read_failures = 0;
	int				data_failures = 0;
	unsigned char	*dt = dt_buf;
	unsigned char	*dt1 = dt_buf1;

	lowlevel_cputest();
	led_digit(0xf);
//	flush_cache();
//	__asm__("	lui	$3,0x2000");
//	__asm__("	or		$29,$29,$3");
	graphics_init();
//	__asm__("	andi	$29,0xdfffffff");
//	flush_cache();
	led_off();
	if(!ide_init())
	{
		printf("DISK Initialization failure\n");
		while(1) ;
	}
	while((int)dt & 0x1f)
	{
		dt++;
	}
	while((int)dt1 & 0x1f)
	{
		dt1++;
	}
	while(1)
	{
		printf("\f");
		set_fcolor(0xffff);
		printf("Disk Torture Test\n\n");
		printf("Pass Number: %d\n", pass_count);
		printf("500000 Sectors/Pass\n");
		if(pre_write_seek_failures)
		{
			printf("Pre-write seek failures: %d\n", pre_write_seek_failures);
		}
		if(write_failures)
		{
			printf("Write failures: %d\n", write_failures);
		}
		if(pre_read_seek_failures)
		{
			printf("Pre-read seek failures: %d\n", pre_read_seek_failures);
		}
		if(read_failures)
		{
			printf("Read failures: %d\n", read_failures);
		}
		if(data_failures)
		{
			printf("Data failures: %d\n", data_failures);
		}
		for(offset = 0; offset < 500000; offset += 16)
		{
			// Fill in the write buffer
			for(i = 0; i < sizeof(dt_buf); i++)
			{
				dt[i] = data;
				data++;
				if(!data)
				{
					data = 1;
				}
			}

			// Seek to a random sector below block 1000000
			if(!(offset % (16*10)))
			{
				printf("Sector: %07d\r", offset);
			}
			if(!_SecReads((gcount() % 1000) * 1000, (unsigned long *)dt1, 1))
			{
				// Write the sector
				if(!_SecWrites(1000000 + offset, (unsigned long *)dt, 16))
				{
					// Seek to a random sector
					if(!_SecReads((gcount() % 1000) * 1000, (unsigned long *)dt1, 1))
					{
						// Read the target sectors
						if(!_SecReads(1000000 + offset, (unsigned long *)dt1, 16))
						{
							if(memcmp(dt, dt1, 16*512))
							{
								data_failures++;
							}
						}
						else
						{
							read_failures++;
						}
					}
					else
					{
						pre_read_seek_failures++;
					}
				}
				else
				{
					write_failures++;
				}
			}
			else
			{
				pre_write_seek_failures++;
			}
		}
		pass_count++;
	}
}

void post_test(void)
{
	void	(*post_exit)(void) = (void (*)(void))0xbfc00008;

	get_mem_size();
	led_digit(0xd);
	sysinit();
	led_digit(0xe);
	fpgaload();
	if(get_scsi_device_number() < 0)
	{
#ifndef CMC_TEST
		unlock_ioasic();
		if(*((volatile short *)IOASIC_DIP_SWITCHES) != 0 &&
			*((volatile short *)IOASIC_DIP_SWITCHES) != 1 &&
			*((volatile short *)IOASIC_DIP_SWITCHES) != 2)
		{
			if(!(*((volatile short *)IOASIC_DIP_SWITCHES) & 0x4000))
			{
				led_off();
				post_exit();
			}
		}
//led_off();
//post_exit();
		if(*((volatile short *)IOASIC_DIP_SWITCHES) == 0)
		{
			do_comprehensive_tests = 1;
			burn_loop = 0;
		}
		else if(*((volatile short *)IOASIC_DIP_SWITCHES) == 1)
		{
			do_comprehensive_tests = 1;
			burn_loop = 1;
		}
		else if(*((volatile short *)IOASIC_DIP_SWITCHES) == 2)
		{
			do_disk_torture_test();
		}
#else
		do_comprehensive_tests = 1;
		burn_loop = 1;
#endif
		lowlevel_cputest();
		led_digit(0xf);
		graphics_init();
		led_off();
		if(do_comprehensive_tests)
		{
			cpu_chips[1].status = 0;
		}
		do
		{
			highlevel_cputest();
			if(do_comprehensive_tests)
			{
				memtest_nocache_bytes();
				memtest_cache_bytes();
				memtest_nocache_shorts();
				memtest_cache_shorts();
				memtest_nocache_words();
				memtest_cache_words();
				memtest_nocache_dwords();
				memtest_cache_dwords();
			}
			siotest();
#ifdef VMM
//			memtest_tlb_cache_words();
#endif
		} while(do_comprehensive_tests && burn_loop);
	}
	led_off();
	post_exit();
}



#if 0
#define	MALLOC_TEST
#define	MIPS

#define getpagesize() 4096
//#define getpagesize() 16384

/*
 * The overhead on a block is at least 4 bytes.  When free, this space
 * contains a pointer to the next free block, and the bottom two bits must
 * be zero.  When in use, the first byte is set to MAGIC, and the second
 * byte is the size index.  The remaining bytes are for alignment.
 * If range checking is enabled then a second word holds the size of the
 * requested block, less 1, rounded up to a multiple of sizeof(RMAGIC).
 * The order of elements is critical: ov_magic must overlay the low order
 * bits of ov_next, and ov_magic can not be a valid ov_next bit pattern.
 */
union overhead
{
	union	overhead		*ov_next;	/* when free */
	struct
	{
		unsigned char	ovu_magic;	/* magic number */
		unsigned char	ovu_index;	/* bucket # */
#ifdef RCHECK
		unsigned short	ovu_rmagic;	/* range magic number */
		unsigned int	ovu_size;	/* actual block size */
#endif
#ifdef MIPS
	int					pad;
#endif
} ovu;

#define	ov_magic		ovu.ovu_magic
#define	ov_index		ovu.ovu_index
#define	ov_rmagic	ovu.ovu_rmagic
#define	ov_size		ovu.ovu_size
};

#define	MAGIC			0xef			/* magic # on accounting info */
#define	RMAGIC		0x5555		/* magic # on range info */

#ifdef RCHECK
#define	RSLOP			sizeof(unsigned short)
#else
#define	RSLOP			0
#endif

/*
 * nextf[i] is the pointer to the next free block of size 2^(i+3).  The
 * smallest allocatable block is 8 bytes.  The overhead information
 * precedes the data area returned to the user.
 */
#define	NBUCKETS			30

static union overhead	*nextf[NBUCKETS];

static int					pagesz; 			/* page size */
static int					pagebucket;		/* page size bucket */

#ifdef MALLOC_TEST
#define	MSTATS
#endif

#ifdef MSTATS
/*
 * nmalloc[i] is the difference between the number of mallocs and frees
 * for a given block size.
 */
static unsigned int		nmalloc[NBUCKETS];
#endif

static void					morecore(int bucket);

#if defined(DEBUG) || defined(RCHECK)
#define	ASSERT(p)	if(!(p)) botch(#p)
static void botch(char *s)
{
	printf("\r\nassertion botched: %s\r\n", s);
	*((int *)0) = 0;	// force fault; abort will probably fail if malloc's dead
}
#else
#define	ASSERT(p)
#endif

static char	_memheap	__attribute__ ((section ("memheap.bss"))) = 0;
static char	*memheap = &_memheap;

static void *sbrk(register int val);

void *kmalloc(register size_t nbytes)
{
	register int				n;
	register unsigned			amt;
	register union overhead	*op;
	register int				bucket;

	//
	// First time malloc is called, setup page size and
	// align break pointer so all data will be page aligned.
	//
	if(pagesz == 0)
	{
		pagesz = n = getpagesize();
		op = (union overhead *)sbrk(0);
		n = n - sizeof(*op) - ((int)op & (n - 1));
		if(n < 0)
		{
			n += pagesz;
		}
		if(n)
		{
			if(sbrk(n) == (void *)-1)
			{
				return(NULL);
			}
		}
		bucket = 0;
		amt = 8;
		while(pagesz > amt)
		{
			amt <<= 1;
			bucket++;
		}
		pagebucket = bucket;
	}

	//
	// Convert amount of memory requested into closest block size
	// stored in hash buckets which satisfies request.
	// Account for space used per block for accounting.
	//
	if(nbytes <= (n = pagesz - sizeof(*op) - RSLOP))
	{
#ifndef RCHECK
		amt = 8;				/* size of first bucket */
		bucket = 0;
#else
		amt = 16;			/* size of first bucket */
		bucket = 1;
#endif
		n = -(sizeof(*op) + RSLOP);
	}
	else
	{
		amt = pagesz;
		bucket = pagebucket;
	}
	while(nbytes > amt + n)
	{
		amt <<= 1;
		if(amt == 0)
		{
			return((void *)0);
		}
		bucket++;
	}

	//
	// If nothing in hash bucket right now,
	// request more memory from the system.
	//
	if((op = nextf[bucket]) == NULL)
	{
		morecore(bucket);
		if((op = nextf[bucket]) == NULL)
		{
			return((void *)0);
		}
	}

	// remove from linked list
	nextf[bucket] = op->ov_next;
	op->ov_magic = MAGIC;
	op->ov_index = bucket;
#ifdef MSTATS
	nmalloc[bucket]++;
#endif
#ifdef RCHECK
	//
	// Record allocated size of block and
	// bound space with magic numbers.
	//
	op->ov_size = (nbytes + RSLOP - 1) & ~(RSLOP - 1);
	op->ov_rmagic = RMAGIC;
	*(unsigned short *)((void *)(op + 1) + op->ov_size) = RMAGIC;
#endif
	return((void *)(op + 1));
}

//
// Allocate more memory to the indicated bucket.
//
static void morecore(register int bucket)
{
	register union overhead	*op;
	register int				sz;		// size of desired block
	register int				amt;		// amount to allocate
	register int				nblks;	// how many blocks we get

	sz = 1 << (bucket + 3);
#ifdef DEBUG
	ASSERT(sz > 0);
#else
	if(sz <= 0)
	{
		return;
	}
#endif
	if(sz < pagesz)
	{
		amt = pagesz;
		nblks = amt / sz;
	}
	else
	{
		amt = sz + pagesz;
		nblks = 1;
	}
//	op = (union overhead *)sbrk(amt);

//	amt += (getpagesize() - 1);
//	amt &= ~(getpagesize() - 1);
	memheap += amt;
	op = (union overhead *)memheap;
	if((unsigned)memheap > 0x807f8000)
	{
		// Return out of memory
		op = (void *)-1;
	}

	//
	// no more room!
	//
	if((int)op == -1)
	{
		return;
	}

	//
	// Add new memory allocated to that on
	// free list for this hash bucket.
	//
	nextf[bucket] = op;
	while(--nblks > 0)
	{
		op->ov_next = (union overhead *)((char *)op + sz);
		op = (union overhead *)((char *)op + sz);
	}
	op->ov_next = 0;
}

void free(register void *cp)
{   
	register int				size;
	register union overhead	*op;

	if(cp == NULL)
	{
		return;
	}
	op = (union overhead *)((char *)cp - sizeof(union overhead));
#ifdef DEBUG
	ASSERT(op->ov_magic == MAGIC);	// make sure it was in use
#else
	if(op->ov_magic != MAGIC)
	{
		return;								// sanity
	}
#endif
#ifdef RCHECK
	ASSERT(op->ov_rmagic == RMAGIC);
	ASSERT(*(unsigned short *)((char *)(op + 1) + op->ov_size) == RMAGIC);
#endif
	size = op->ov_index;
#ifdef DEBUG
	ASSERT(size < NBUCKETS);
#endif
	op->ov_next = nextf[size];			// also clobbers ov_magic
	nextf[size] = op;
#ifdef MSTATS
	nmalloc[size]--;
#endif
}

void *realloc(void *cp, size_t nbytes)
{   
	unsigned int	onb;
	int				i;
	union overhead	*op;
	char				*res;
	int				was_alloced = 0;

	if(cp == NULL)
	{
		return(kmalloc(nbytes));
	}
	op = (union overhead *)((char *)cp - sizeof (union overhead));
	if(op->ov_magic == MAGIC)
	{
		was_alloced++;
		i = op->ov_index;
	}
	else
	{
		return 0;
	}
	onb = 1 << (i + 3);
	if(onb < pagesz)
	{
		onb -= sizeof (*op) + RSLOP;
	}
	else
	{
		onb += pagesz - sizeof (*op) - RSLOP;
	}
	/* avoid the copy if same size block */
	if(was_alloced)
	{
		if(i)
		{
			i = 1 << (i + 2);
			if(i < pagesz)
			{
				i -= sizeof(*op) + RSLOP;
			}
			else
			{
				i += pagesz - sizeof(*op) - RSLOP;
			}
		}
		if(nbytes <= onb && nbytes > i)
		{
#ifdef RCHECK
			op->ov_size = (nbytes + RSLOP - 1) & ~(RSLOP - 1);
			*(unsigned short *)((char *)(op + 1) + op->ov_size) = RMAGIC;
#endif
			return(cp);
		}
		else
		{
			free(cp);
		}
	}
	if((res = kmalloc(nbytes)) == NULL)
	{
		return (NULL);
	}
	if(cp != res)
	{
		memcpy(res, cp, (nbytes < onb) ? nbytes : onb);
	}
	return(res);
}

#ifdef MSTATS
/*
 * mstats - print out statistics about malloc
 * 
 * Prints two lines of numbers, one showing the length of the free list
 * for each size category, the second showing the number of mallocs -
 * frees for each size category.
 */
void _mstats(char *s)
{
	int				i, j;
	union overhead	*p;
	int				totfree = 0, totused = 0;

	printf("Memory allocation statistics %s\nfree:\t", s);
	for(i = 0; i < NBUCKETS; i++)
	{
		for(j = 0, p = nextf[i]; p; p = p->ov_next, j++)
		{
			;
		}
		printf(" %d", j);
		totfree += j * (1 << (i + 3));
	}
	printf("\nused:\t");
	for(i = 0; i < NBUCKETS; i++)
	{
		printf(" %d", nmalloc[i]);
		totused += nmalloc[i] * (1 << (i + 3));
	}
	printf("\n\tTotal in use: %d, total free: %d\n", totused, totfree);
}
#endif

#ifdef MALLOC_TEST
static void do_kmalloc_test(void)
{
	void	*p;
	void	*p1;
	void	*p2;

	printf("\f\n");
	p = kmalloc(8);
	p = kmalloc(8);
	p = kmalloc(9);
	p = kmalloc(16);
	p = kmalloc(16);
	p = kmalloc(28);

	p = kmalloc(1024);
	p = kmalloc(1024);
	p = kmalloc(1024);
	p = kmalloc(1024);
	p1 = kmalloc(1024);
	p2 = kmalloc(1024);
	p = kmalloc(1024);

	p = kmalloc(65536);

#ifdef MSTATS
	_mstats("t1");
#endif
	free(p);
	free(p1);
	free(p2);
#ifdef MSTATS
	_mstats("t2");
#endif

while(1) ;
}

static void *sbrk(register int val)
{
	// Initializing ?
	if(!val)
	{
		// YES - align memheap pointer to page size
		(int)memheap += (getpagesize() - 1);
		(int)memheap &= ~(getpagesize() - 1);
	}
	else
	{
		val += (getpagesize() - 1);
		val &= ~(getpagesize() - 1);
		memheap += val;
	}

	// At end of memory ?
	if((unsigned)memheap > 0x807f8000)
	{
		// Return out of memory
		return((void *)-1);
	}

	// Return pointer
	return(memheap);
}
#endif
#endif
