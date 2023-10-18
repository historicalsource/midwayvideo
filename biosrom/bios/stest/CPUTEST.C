#include	<stdio.h>
#include	<system.h>
#include	"test.h"

//#define	DISK_WRITE_TESTS
//#define	MEMORY_PERFORMANCE

void clear_count(void);
int gcount(void);
void delay_us(int usec);
int get_buttons(void);

unsigned long	*except_regs;

extern int	screen_width;
extern int	screen_height;

#define	CPU_SCALE	0.45f
#define	CPU_TRANSX	(0.98f - CPU_SCALE)
#define	CPU_TRANSY	(0.95f - CPU_SCALE)
#define	CPU_W			((8.0f*32.0f)+6.0f)
#define	CPU_H			((7.0f*32.0f)+10.0f)

#define	NANOS_PER_TICK	13

#define	DISK_TEST_SECTORS	2048

void clear_count(void);
int	gcount(void);
void get_mem_size(void);

extern volatile int	__memory_size;
extern int				graphics_initialized;

static point_data_t	cpu_points[] = {
(0.0f / 743.0f), (0.0f / 743.0f),
(510.0f / 743.0f), (0.0f / 743.0f),
(510.0f / 743.0f), (390.0f / 743.0f),
(495.0f / 743.0f), (390.0f / 743.0f),
(495.0f / 743.0f), (414.0f / 743.0f),
(510.0f / 743.0f), (414.0f / 743.0f),
(510.0f / 743.0f), (570.0f / 743.0f),
(495.0f / 743.0f), (570.0f / 743.0f),
(495.0f / 743.0f), (576.0f / 743.0f),
(510.0f / 743.0f), (576.0f / 743.0f),
(510.0f / 743.0f), (625.0f / 743.0f),
(495.0f / 743.0f), (625.0f / 743.0f),
(495.0f / 743.0f), (649.0f / 743.0f),
(510.0f / 743.0f), (649.0f / 743.0f),
(510.0f / 743.0f), (743.0f / 743.0f),
(0.0f / 743.0f), (743.0f / 743.0f),
(0.0f /743.0f), (0.0f / 743.0f)
};

static point_data_t	cpu_rom_chip_points[] = {
(416.0f / 743.0f), (65.0f / 743.0f),
(449.0f / 743.0f), (65.0f / 743.0f),
(449.0f / 743.0f), (144.0f / 743.0f),
(416.0f / 743.0f), (144.0f / 743.0f),
(416.0f / 743.0f), (65.0f / 743.0f),
};

static point_data_t	cpu_expansion_rom_chip_points[] = {
(416.0f / 743.0f), (148.0f / 743.0f),
(449.0f / 743.0f), (148.0f / 743.0f),
(449.0f / 743.0f), (227.0f / 743.0f),
(416.0f / 743.0f), (227.0f / 743.0f),
(416.0f / 743.0f), (148.0f / 743.0f),
};

static point_data_t	cpu_cpu_chip_points[] = {
(52.0f / 743.0f), (187.0f / 743.0f),
(140.0f / 743.0f), (187.0f / 743.0f),
(140.0f / 743.0f), (276.0f / 743.0f),
(52.0f / 743.0f), (276.0f / 743.0f),
(52.0f / 743.0f), (187.0f / 743.0f),
};

static point_data_t	cpu_galileo_chip_points[] = {
(199.0f / 743.0f), (400.0f / 743.0f),
(259.0f / 743.0f), (400.0f / 743.0f),
(259.0f / 743.0f), (460.0f / 743.0f),
(199.0f / 743.0f), (460.0f / 743.0f),
(199.0f / 743.0f), (400.0f / 743.0f),
};

static point_data_t	cpu_sdram1_chip_points[] = {
(39.0f / 743.0f), (63.0f / 743.0f),
(63.0f / 743.0f), (63.0f / 743.0f),
(63.0f / 743.0f), (114.0f / 743.0f),
(39.0f / 743.0f), (114.0f / 743.0f),
(39.0f / 743.0f), (63.0f / 743.0f),
};

static point_data_t	cpu_sdram2_chip_points[] = {
(72.0f / 743.0f), (63.0f / 743.0f),
(96.0f / 743.0f), (63.0f / 743.0f),
(96.0f / 743.0f), (114.0f / 743.0f),
(72.0f / 743.0f), (114.0f / 743.0f),
(72.0f / 743.0f), (63.0f / 743.0f),
};

static point_data_t	cpu_sdram3_chip_points[] = {
(104.0f / 743.0f), (63.0f / 743.0f),
(128.0f / 743.0f), (63.0f / 743.0f),
(128.0f / 743.0f), (114.0f / 743.0f),
(104.0f / 743.0f), (114.0f / 743.0f),
(104.0f / 743.0f), (63.0f / 743.0f),
};

static point_data_t	cpu_sdram4_chip_points[] = {
(137.0f / 743.0f), (63.0f / 743.0f),
(161.0f / 743.0f), (63.0f / 743.0f),
(161.0f / 743.0f), (114.0f / 743.0f),
(137.0f / 743.0f), (114.0f / 743.0f),
(137.0f / 743.0f), (63.0f / 743.0f),
};

static point_data_t	cpu_ide_chip_points[] = {
(75.0f / 743.0f), (393.0f / 743.0f),
(112.0f / 743.0f), (393.0f / 743.0f),
(112.0f / 743.0f), (441.0f / 743.0f),
(75.0f / 743.0f), (441.0f / 743.0f),
(75.0f / 743.0f), (393.0f / 743.0f),
};

static point_data_t	cpu_pic_chip_points[] = {
(416.0f / 743.0f), (230.0f / 743.0f),
(449.0f / 743.0f), (230.0f / 743.0f),
(449.0f / 743.0f), (299.0f / 743.0f),
(416.0f / 743.0f), (299.0f / 743.0f),
(416.0f / 743.0f), (230.0f / 743.0f),
};

static point_data_t	cpu_ioasic_chip_points[] = {
(345.0f / 743.0f), (411.0f / 743.0f),
(407.0f / 743.0f), (411.0f / 743.0f),
(407.0f / 743.0f), (473.0f / 743.0f),
(345.0f / 743.0f), (473.0f / 743.0f),
(345.0f / 743.0f), (411.0f / 743.0f),
};

static point_data_t	cpu_rtc_clock_chip_points[] = {
(390.0f / 743.0f), (251.0f / 743.0f),
(406.0f / 743.0f), (251.0f / 743.0f),
(406.0f / 743.0f), (261.0f / 743.0f),
(390.0f / 743.0f), (261.0f / 743.0f),
(390.0f / 743.0f), (251.0f / 743.0f),
};

static point_data_t	cpu_cmos_ram_chip_points[] = {
(314.0f / 743.0f), (479.0f / 743.0f),
(334.0f / 743.0f), (479.0f / 743.0f),
(334.0f / 743.0f), (502.0f / 743.0f),
(314.0f / 743.0f), (502.0f / 743.0f),
(314.0f / 743.0f), (479.0f / 743.0f),
};

static point_data_t	cpu_fbi_chip_points[] = {
(199.0f / 743.0f), (538.0f / 743.0f),
(269.0f / 743.0f), (538.0f / 743.0f),
(269.0f / 743.0f), (607.0f / 743.0f),
(199.0f / 743.0f), (607.0f / 743.0f),
(199.0f / 743.0f), (538.0f / 743.0f),
};

static point_data_t	cpu_tmu_chip_points[] = {
(206.0f / 743.0f), (634.0f / 743.0f),
(269.0f / 743.0f), (634.0f / 743.0f),
(269.0f / 743.0f), (697.0f / 743.0f),
(206.0f / 743.0f), (697.0f / 743.0f),
(206.0f / 743.0f), (634.0f / 743.0f),
};

static point_data_t	cpu_fbi_mem0_chip_points[] = {
(24.0f / 743.0f), (537.0f / 743.0f),
(73.0f / 743.0f), (537.0f / 743.0f),
(73.0f / 743.0f), (562.0f / 743.0f),
(24.0f / 743.0f), (562.0f / 743.0f),
(24.0f / 743.0f), (537.0f / 743.0f),
};

static point_data_t	cpu_fbi_mem1_chip_points[] = {
(24.0f / 743.0f), (570.0f / 743.0f),
(73.0f / 743.0f), (570.0f / 743.0f),
(73.0f / 743.0f), (595.0f / 743.0f),
(24.0f / 743.0f), (595.0f / 743.0f),
(24.0f / 743.0f), (570.0f / 743.0f),
};

static point_data_t	cpu_fbi_mem2_chip_points[] = {
(24.0f / 743.0f), (603.0f / 743.0f),
(73.0f / 743.0f), (603.0f / 743.0f),
(73.0f / 743.0f), (627.0f / 743.0f),
(24.0f / 743.0f), (627.0f / 743.0f),
(24.0f / 743.0f), (603.0f / 743.0f),
};

static point_data_t	cpu_fbi_mem3_chip_points[] = {
(24.0f / 743.0f), (635.0f / 743.0f),
(73.0f / 743.0f), (635.0f / 743.0f),
(73.0f / 743.0f), (659.0f / 743.0f),
(24.0f / 743.0f), (659.0f / 743.0f),
(24.0f / 743.0f), (635.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem0_chip_points[] = {
(24.0f / 743.0f), (667.0f / 743.0f),
(73.0f / 743.0f), (667.0f / 743.0f),
(73.0f / 743.0f), (692.0f / 743.0f),
(24.0f / 743.0f), (692.0f / 743.0f),
(24.0f / 743.0f), (667.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem1_chip_points[] = {
(24.0f / 743.0f), (700.0f / 743.0f),
(73.0f / 743.0f), (700.0f / 743.0f),
(73.0f / 743.0f), (724.0f / 743.0f),
(24.0f / 743.0f), (724.0f / 743.0f),
(24.0f / 743.0f), (700.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem2_chip_points[] = {
(83.0f / 743.0f), (537.0f / 743.0f),
(131.0f / 743.0f), (537.0f / 743.0f),
(131.0f / 743.0f), (562.0f / 743.0f),
(83.0f / 743.0f), (562.0f / 743.0f),
(83.0f / 743.0f), (537.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem3_chip_points[] = {
(83.0f / 743.0f), (570.0f / 743.0f),
(131.0f / 743.0f), (570.0f / 743.0f),
(131.0f / 743.0f), (595.0f / 743.0f),
(83.0f / 743.0f), (595.0f / 743.0f),
(83.0f / 743.0f), (570.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem4_chip_points[] = {
(83.0f / 743.0f), (603.0f / 743.0f),
(131.0f / 743.0f), (603.0f / 743.0f),
(131.0f / 743.0f), (627.0f / 743.0f),
(83.0f / 743.0f), (627.0f / 743.0f),
(83.0f / 743.0f), (603.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem5_chip_points[] = {
(83.0f / 743.0f), (635.0f / 743.0f),
(131.0f / 743.0f), (635.0f / 743.0f),
(131.0f / 743.0f), (659.0f / 743.0f),
(83.0f / 743.0f), (659.0f / 743.0f),
(83.0f / 743.0f), (635.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem6_chip_points[] = {
(83.0f / 743.0f), (667.0f / 743.0f),
(131.0f / 743.0f), (667.0f / 743.0f),
(131.0f / 743.0f), (692.0f / 743.0f),
(83.0f / 743.0f), (692.0f / 743.0f),
(83.0f / 743.0f), (667.0f / 743.0f),
};

static point_data_t	cpu_tmu_mem7_chip_points[] = {
(83.0f / 743.0f), (700.0f / 743.0f),
(131.0f / 743.0f), (700.0f / 743.0f),
(131.0f / 743.0f), (724.0f / 743.0f),
(83.0f / 743.0f), (724.0f / 743.0f),
(83.0f / 743.0f), (700.0f / 743.0f),
};

static point_data_t	cpu_fifo0_chip_points[] = {
(378.0f / 743.0f), (112.0f / 743.0f),
(405.0f / 743.0f), (112.0f / 743.0f),
(405.0f / 743.0f), (144.0f / 743.0f),
(378.0f / 743.0f), (144.0f / 743.0f),
(378.0f / 743.0f), (112.0f / 743.0f),
};

static point_data_t	cpu_fifo1_chip_points[] = {
(378.0f / 743.0f), (154.0f / 743.0f),
(405.0f / 743.0f), (154.0f / 743.0f),
(405.0f / 743.0f), (188.0f / 743.0f),
(378.0f / 743.0f), (188.0f / 743.0f),
(378.0f / 743.0f), (154.0f / 743.0f),
};

static point_data_t	cpu_sound_dsp_chip_points[] = {
(332.0f / 743.0f), (311.0f / 743.0f),
(383.0f / 743.0f), (311.0f / 743.0f),
(383.0f / 743.0f), (362.0f / 743.0f),
(332.0f / 743.0f), (362.0f / 743.0f),
(332.0f / 743.0f), (311.0f / 743.0f),
};

chip_draw_info_t	cpu_chips[] = {
{sizeof(cpu_rom_chip_points)/sizeof(point_data_t), &cpu_rom_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_expansion_rom_chip_points)/sizeof(point_data_t), &cpu_expansion_rom_chip_points[0].idata, CHIP_STATUS_NOT_STUFFED},
{sizeof(cpu_cpu_chip_points)/sizeof(point_data_t), &cpu_cpu_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_galileo_chip_points)/sizeof(point_data_t), &cpu_galileo_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram1_chip_points)/sizeof(point_data_t), &cpu_sdram1_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram2_chip_points)/sizeof(point_data_t), &cpu_sdram2_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram3_chip_points)/sizeof(point_data_t), &cpu_sdram3_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_sdram4_chip_points)/sizeof(point_data_t), &cpu_sdram4_chip_points[0].idata, CHIP_STATUS_GOOD},
{sizeof(cpu_ide_chip_points)/sizeof(point_data_t), &cpu_ide_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_pic_chip_points)/sizeof(point_data_t), &cpu_pic_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_ioasic_chip_points)/sizeof(point_data_t), &cpu_ioasic_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_rtc_clock_chip_points)/sizeof(point_data_t), &cpu_rtc_clock_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_cmos_ram_chip_points)/sizeof(point_data_t), &cpu_cmos_ram_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_fbi_chip_points)/sizeof(point_data_t), &cpu_fbi_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_chip_points)/sizeof(point_data_t), &cpu_tmu_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_fbi_mem0_chip_points)/sizeof(point_data_t), &cpu_fbi_mem0_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_fbi_mem1_chip_points)/sizeof(point_data_t), &cpu_fbi_mem1_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_fbi_mem2_chip_points)/sizeof(point_data_t), &cpu_fbi_mem2_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_fbi_mem3_chip_points)/sizeof(point_data_t), &cpu_fbi_mem3_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem0_chip_points)/sizeof(point_data_t), &cpu_tmu_mem0_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem1_chip_points)/sizeof(point_data_t), &cpu_tmu_mem1_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem2_chip_points)/sizeof(point_data_t), &cpu_tmu_mem2_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem3_chip_points)/sizeof(point_data_t), &cpu_tmu_mem3_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem4_chip_points)/sizeof(point_data_t), &cpu_tmu_mem4_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem5_chip_points)/sizeof(point_data_t), &cpu_tmu_mem5_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem6_chip_points)/sizeof(point_data_t), &cpu_tmu_mem6_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_tmu_mem7_chip_points)/sizeof(point_data_t), &cpu_tmu_mem7_chip_points[0].idata, CHIP_STATUS_UNTESTED},
{sizeof(cpu_fifo0_chip_points)/sizeof(point_data_t), &cpu_fifo0_chip_points[0].idata, CHIP_STATUS_NOT_STUFFED},
{sizeof(cpu_fifo1_chip_points)/sizeof(point_data_t), &cpu_fifo1_chip_points[0].idata, CHIP_STATUS_NOT_STUFFED},
{sizeof(cpu_sound_dsp_chip_points)/sizeof(point_data_t), &cpu_sound_dsp_chip_points[0].idata, CHIP_STATUS_UNTESTED},
};


int cpu_rom_chip_num = 0;
int cpu_expansion_rom_chip_num = 1;
int cpu_cpu_chip_num = 2;
int cpu_galileo_chip_num = 3;
int cpu_sdram1_chip_num = 4;
int cpu_sdram2_chip_num = 5;
int cpu_sdram3_chip_num = 6;
int cpu_sdram4_chip_num = 7;
int cpu_ide_chip_num = 8;
int cpu_pic_chip_num = 9;
int cpu_ioasic_chip_num = 10;
int cpu_rtc_clock_chip_num = 11;
int cpu_cmos_ram_chip_num = 12;
int cpu_fbi_chip_num = 13;
int cpu_tmu_chip_num = 14;
int cpu_fbi_mem0_chip_num = 15;
int cpu_fbi_mem1_chip_num = 16;
int cpu_fbi_mem2_chip_num = 17;
int cpu_fbi_mem3_chip_num = 18;
int cpu_tmu_mem0_chip_num = 19;
int cpu_tmu_mem1_chip_num = 20;
int cpu_tmu_mem2_chip_num = 21;
int cpu_tmu_mem3_chip_num = 22;
int cpu_tmu_mem4_chip_num = 23;
int cpu_tmu_mem5_chip_num = 24;
int cpu_tmu_mem6_chip_num = 25;
int cpu_tmu_mem7_chip_num = 26;
int cpu_fifo0_chip_num = 27;
int cpu_fifo1_chip_num = 28;
int cpu_sound_dsp_chip_num = 29;

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

static void show_processor_info(void)
{
	int	config_reg = get_config_reg();
	int	prid = get_prid_reg();
	int	size;
	int	csize;

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
	printf("System Clock Ratio:           Divide by %d\n\n", ((config_reg >> 28) & 0x7) + 2);
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

#ifdef MEMORY_PERFORMANCE
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
#endif

void lowlevel_cputest(void)
{
	printf("\n\nLow Level CPU Tests\n");
	printf("Memory Size: %d (MB)\n\n", __memory_size / (1024*1024));
	show_processor_info();
	show_pci_devices();
}

static void cpu_led_test(void)
{
	int	i;
	int	tmp;

	set_fcolor(0xffff);
	printf("CPU LED Test - ");
	for(i = 0; i < 0x8; i++)
	{
		*((volatile char *)LED_ADDR) = i;
		tmp = *((volatile char *)LED_ADDR) & 0x7;
		if(tmp != i)
		{
			set_fcolor(0xf800);
			printf("FAIL - 0x%02.2X -> 0x%02.2X\n", i, tmp);
			set_fcolor(0xffff);
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
		set_fcolor(0xffff);
		cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
		return;
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

static unsigned char	sbuffer[512];
static unsigned char	sbuffer1[512];
volatile static int	disk_int_received;
extern void				(*callback)(int);

static void disk_int_handler(int status)
{
	disk_int_received++;
}

static void diskreadinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Disk Read Interrupt Test - ");
	disk_int_received = 0;
	callback = disk_int_handler;
	if(_SecReads(0, (unsigned long *)sbuffer, 1) < 0)
	{
		callback = (void *)0;
		set_fcolor(0xf800);
		printf("FAIL - READ\n");
		set_fcolor(0xffff);
		cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
		return;
	}
	for(i = 0; i < 50000000; i++)
	{
		if(get_buttons())
		{
			callback = (void *)0;
			set_fcolor(0xffe0);
			printf("ABORTED\n");
			set_fcolor(0xffff);
			return;
		}
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
	set_fcolor(0xffff);
	cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
}

#ifdef DISK_WRITE_TESTS
static void diskwriteinttest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Disk Write Interrupt Test - ");
	disk_int_received = 0;
	callback = disk_int_handler;
	if(_SecWrites(1000000, (unsigned long *)sbuffer, 1))
	{
		callback = (void *)0;
		set_fcolor(0xf800);
		printf("FAIL - WRITE\n");
		set_fcolor(0xffff);
		cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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
	set_fcolor(0xffff);
	cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
}
#endif


static void diskreadtest(void)
{
	int	i;

	set_fcolor(0xffff);
	printf("Disk Read Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(_SecReads(i, (unsigned long *)sbuffer, 1) < 0)
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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

	set_fcolor(0xffff);
	printf("Disk Write Test - ");
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(_SecReads(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
		if(_SecWrites(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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
		// Get the sector
		if(!SecReads(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL - Read\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}

		// Write the sector back
		if(!SecWrites(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	set_fcolor(0x07e0);
	printf("PASS\n");
	set_fcolor(0xffff);
}

#ifdef DISK_WRITE_TESTS
static void diskreadwritetest(void)
{
	int	i;
	int	j;
	unsigned char	data = 1;

	set_fcolor(0xffff);
	printf("Disk Write/Read Test - ");
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
		if(_SecWrites(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL DURING WRITE PASS\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
	}
	data = 1;
	for(i = 0; i < DISK_TEST_SECTORS; i++)
	{
		if(_SecReads(i, (unsigned long *)sbuffer, 1))
		{
			set_fcolor(0xf800);
			printf("FAIL DURING READ PASS\n");
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
		for(j = 0; j < 512; j++)
		{
			if(sbuffer[j] != data)
			{
				set_fcolor(0xf800);
				printf("FAIL - DATA - SECTOR: %d\n", i);
				set_fcolor(0xffff);
				cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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
			set_fcolor(0xffff);
			cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
			return;
		}
		for(j = 0; j < 512; j++)
		{
			if(sbuffer[j] != data)
			{
				set_fcolor(0xf800);
				printf("FAIL - DATA\n");
				set_fcolor(0xffff);
				cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_FAILED;
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
#endif

static void rom_checksum(void)
{
	int	i;
	int	cksum = 0;
	int	scksum;

	set_fcolor(0xffff);
	printf("ROM Checksum - ");
	for(i = 0; i < 524284; i++)
	{
		cksum += (int)*((volatile char *)(0xbfc00000 + i));
	}
	scksum = *((volatile int *)(0xbfc00000 + 524284));
	if(cksum != scksum)
	{
		set_fcolor(0xf800);
		printf("FAIL\n");
		cpu_chips[cpu_rom_chip_num].status = CHIP_STATUS_FAILED;
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

static int	drawing_initialized = 0;

static void init_cpu_drawing(void)
{
	int	i;

	if(drawing_initialized)
	{
		return;
	}

	drawing_initialized = 1;
	scale_up(sizeof(cpu_points)/sizeof(point_data_t), cpu_points, CPU_SCALE);
	translate(sizeof(cpu_points)/sizeof(point_data_t), cpu_points, CPU_TRANSX, CPU_TRANSY);
	to_int(sizeof(cpu_points)/sizeof(point_data_t), cpu_points);

	for(i = 0; i < sizeof(cpu_chips)/sizeof(chip_draw_info_t); i++)
	{
		scale_up(cpu_chips[i].num_points, (point_data_t *)cpu_chips[i].points, CPU_SCALE);
		translate(cpu_chips[i].num_points, (point_data_t *)cpu_chips[i].points, CPU_TRANSX, CPU_TRANSY);
		to_int(cpu_chips[i].num_points, (point_data_t *)cpu_chips[i].points);
	}
}

void draw_chip(chip_draw_info_t *cdi)
{
	set_fcolor(0);
	filled_rect(cdi->points[0], cdi->points[1], cdi->points[2]-cdi->points[0]+1, cdi->points[5]-cdi->points[1]+1);
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
	if(!cdi->num_points)
	{
		return;
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


void highlevel_cputest(void)
{
	int	fmem_test_status = 0;
	int	tmem_test_status = 0;
	int	i;

	init_cpu_drawing();
	if(graphics_initialized)
	{
		fmem_test_status = testFBIMem();
		if(get_buttons())
		{
			set_fcolor(0xffff);
			return;
		}
		tmem_test_status = testTMUMem();
		if(get_buttons())
		{
			set_fcolor(0xffff);
			return;
		}
	}
	printf("\f");
	set_fcolor(0xffff);
	printf("\n\nP.O.S.T - $Revision: 6 $\n\n");
	printf("Memory Size: %d (MB)\n\n", __memory_size / (1024*1024));
	draw_cpu();
	if((__memory_size / (1024*1024)) != 8)
	{
		cpu_chips[cpu_sdram1_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_sdram2_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_sdram3_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_sdram4_chip_num].status = CHIP_STATUS_FAILED;
		draw_chip(&cpu_chips[cpu_sdram1_chip_num]);
		draw_chip(&cpu_chips[cpu_sdram2_chip_num]);
		draw_chip(&cpu_chips[cpu_sdram3_chip_num]);
		draw_chip(&cpu_chips[cpu_sdram4_chip_num]);
	}
//	show_processor_info();
	if(screen_height > 256)
	{
		show_pci_devices();
	}
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
	if(fmem_test_status)
	{
		set_fcolor(0xffff);
		printf("FBI Memory Test - ");
		set_fcolor(0x07e0);
		printf("PASS\n");
		set_fcolor(0xffff);
		cpu_chips[cpu_fbi_mem0_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_fbi_mem1_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_fbi_mem2_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_fbi_mem3_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_fbi_chip_num].status = CHIP_STATUS_GOOD;
	}
	else
	{
		set_fcolor(0xffff);
		printf("FBI Memory Test - ");
		set_fcolor(0xf800);
		printf("FAIL\n");
		cpu_chips[cpu_fbi_mem0_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_fbi_mem1_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_fbi_mem2_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_fbi_mem3_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_fbi_chip_num].status = CHIP_STATUS_FAILED;
	}
	draw_chip(&cpu_chips[cpu_fbi_mem0_chip_num]);
	draw_chip(&cpu_chips[cpu_fbi_mem1_chip_num]);
	draw_chip(&cpu_chips[cpu_fbi_mem2_chip_num]);
	draw_chip(&cpu_chips[cpu_fbi_mem3_chip_num]);
	draw_chip(&cpu_chips[cpu_fbi_chip_num]);
	if(tmem_test_status)
	{
		set_fcolor(0xffff);
		printf("TMU Memory Test - ");
		set_fcolor(0x07e0);
		printf("PASS\n");
		set_fcolor(0xffff);
		cpu_chips[cpu_tmu_mem0_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem1_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem2_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem3_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem4_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem5_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem6_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_mem7_chip_num].status = CHIP_STATUS_GOOD;
		cpu_chips[cpu_tmu_chip_num].status = CHIP_STATUS_GOOD;
	}
	else
	{
		set_fcolor(0xffff);
		printf("TMU Memory Test - ");
		set_fcolor(0xf800);
		printf("FAIL\n");
		cpu_chips[cpu_tmu_mem0_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem1_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem2_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem3_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem4_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem5_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem6_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_mem7_chip_num].status = CHIP_STATUS_FAILED;
		cpu_chips[cpu_tmu_chip_num].status = CHIP_STATUS_FAILED;
	}
	draw_chip(&cpu_chips[cpu_tmu_mem0_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem1_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem2_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem3_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem4_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem5_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem6_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_mem7_chip_num]);
	draw_chip(&cpu_chips[cpu_tmu_chip_num]);

	cpu_chips[cpu_rom_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_rom_chip_num]);
	rom_checksum();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
	if(cpu_chips[cpu_rom_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_rom_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_rom_chip_num]);
	cpu_led_test();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
	cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_TESTING;
	draw_chip(&cpu_chips[cpu_ide_chip_num]);
	diskinittest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
	diskreadinttest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
#ifdef DISK_WRITE_TESTS
	diskwriteinttest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
#endif
	diskreadtest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
#ifdef DISK_WRITE_TESTS
	diskwritetest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
	diskreadwritetest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
#endif
	diskcachereadtest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
#ifdef DISK_WRITE_TESTS
	diskcachewritetest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		return;
	}
	diskcachereadwritetest();
	if(get_buttons())
	{
		set_fcolor(0xffff);
		set_fcolor(0xffff);
		return;
	}
#endif
	if(cpu_chips[cpu_ide_chip_num].status != CHIP_STATUS_FAILED)
	{
		cpu_chips[cpu_ide_chip_num].status = CHIP_STATUS_GOOD;
	}
	draw_chip(&cpu_chips[cpu_ide_chip_num]);
	if(!graphics_initialized)
	{
		set_fcolor(0xffff);
		return;
	}
	for(i = 0; i < sizeof(cpu_chips)/sizeof(chip_draw_info_t); i++)
	{
		if(cpu_chips[i].status == CHIP_STATUS_FAILED)
		{
			for(i = 0; i < 5000000; i++)
			{
				delay_us(1);
				if(get_buttons())
				{
					set_fcolor(0xffff);
					return;
				}
			}
		}
	}
	set_fcolor(0xffff);
#ifdef MEMORY_PERFORMANCE
	memperf();
#endif
}

static int	cb_stuck;
static int	pb12_stuck;
static int	pb34_stuck;

void post_test(void)
{
	int	sw;
	int	i;
	int	id;
	void	(*post_exit)(void) = (void (*)(void))0xbfc00008;

	disable_interrupts();
	unlock_ioasic();
	sw = ~*((volatile int *)IOASIC_DIP_SWITCHES) & 0xffff;
	if(!(sw & 0x4000))
	{
		cb_stuck = ~(~*((volatile int *)IOASIC_COIN_INPUT) & ((1<<2)|(1<<4)|(1<<5)|(1<<6)|(1<<9)|(1<<10)|(1<<11)|(1<<12)));
		pb12_stuck = ~(~*((volatile int *)IOASIC_PLAYER_12) & 0xf0f0);
		pb34_stuck = ~(~*((volatile int *)IOASIC_PLAYER_34) & 0xf0f0);
		disable_interrupts();
		get_mem_size();
		led_digit(0xd);
		gt64010_init();

		for(i = 6; i < 10; i++)
		{
			id = get_pci_config_reg(i, 0);
			if((id & 0xffff) == 0x1000)
			{
				break;
			}
		}
		if(i != 10)
		{
			return;
		}

		disable_interrupts();
		led_digit(0xe);
		lowlevel_cputest();
		disable_interrupts();
		led_digit(0xf);
		graphics_init();
		led_off();
		disable_interrupts();
		highlevel_cputest();
		if(!get_buttons())
		{
			siotest();
		}
		disable_ip(IDE_DISK_INT);
	}
	post_exit();
}

__asm__("
	.globl	clear_count
	.set		noreorder
clear_count:
	mtc0	$0,$9
	nop
	nop
	jr	$31
	nop
	.set		reorder

	.globl	gcount
	.set		noreorder
gcount:
	mfc0	$2,$9
	nop
	nop
	jr	$31
	nop
	.set		reorder
");

void delay_us(int usec)
{
	clear_count();
	usec *= 1000;
	usec /= 10;
	while(gcount() < usec) ;
}


void show_exc_info(char *str, int cause, unsigned int *r_save)
{
}

int get_buttons(void)
{
	int	cb;
	int	pb12;
	int	pb34;
	
	cb = ~*((volatile int *)IOASIC_COIN_INPUT) & ((1<<2)|(1<<4)|(1<<5)|(1<<6)|(1<<9)|(1<<10)|(1<<11)|(1<<12));
	cb &= cb_stuck;
	pb12 = ~*((volatile int *)IOASIC_PLAYER_12) & 0xf0f0;
	pb12 &= pb12_stuck;
	pb34 = ~*((volatile int *)IOASIC_PLAYER_34) & 0xf0f0;
	pb34 &= pb34_stuck;
	return((cb|pb12|pb34));
}
