/****************************************************************************/
/*                                                                          */
/* gt64010.h - Header file for GT-64010 stuff                               */
/*                                                                          */
/* Written by:  Jason Skiles, Michael J. Lynch                              */
/* Version:     1.00                                                        */
/* Date:        10/27/95                                                    */
/*                                                                          */
/* Copyright (c) 1995 by Williams Electronics Games Inc.                    */
/* All Rights Reserved                                                      */
/*                                                                          */
/* Use, duplication, or disclosure is strictly forbidden unless approved    */
/* in writing by Williams Electronics Games Inc.                            */
/*                                                                          */
/* $Revision: 1 $                                                             */
/*                                                                          */
/****************************************************************************/
#ifndef __GT64010_H__
#define __GT64010_H__

#ifndef __SYSTEM_H__
#include	<system.h>
#endif

// GT64010 Registers
#define	GT_CPU_ICONFIG				(GT_64010_BASE + 0x000)

// GT64010 Processor Address Space Registers
#define	GT_PAR_RAS10_LOW 			(GT_64010_BASE + 0x008)
#define	GT_PAR_RAS10_HIGHT		(GT_64010_BASE + 0x010)
#define	GT_PAR_RAS32_LOW			(GT_64010_BASE + 0x018)
#define	GT_PAR_RAS32_HIGH			(GT_64010_BASE + 0x020)
#define	GT_PAR_CS20_LOW			(GT_64010_BASE + 0x028)
#define	GT_PAR_CS20_HIGH			(GT_64010_BASE + 0x030)
#define	GT_PAR_CS3_LOW				(GT_64010_BASE + 0x038)
#define	GT_PAR_C33_HIGH			(GT_64010_BASE + 0x040)
#define	GT_PAR_PCI_IO_LOW			(GT_64010_BASE + 0x048)
#define	GT_PAR_PCI_IO_HIGH		(GT_64010_BASE + 0x050)
#define	GT_PAR_PCI_MEM_LOW		(GT_64010_BASE + 0x058)
#define	GT_PAR_PCI_MEM_HIGH		(GT_64010_BASE + 0x060)
#define	GT_PAR_INTERNAL_DECODE	(GT_64010_BASE + 0x068)
#define	GT_PAR_BERROR_LOW		 	(GT_64010_BASE + 0x070)
#define	GT_PAR_BERROR_HIGH	 	(GT_64010_BASE + 0x078)

// GT64010 DRAM and Device Address Registers
#define	GT_DDAR_RAS0_LOW 			(GT_64010_BASE + 0x400)
#define	GT_DDAR_RAS0_HIGH			(GT_64010_BASE + 0x404)
#define	GT_DDAR_RAS1_LOW 			(GT_64010_BASE + 0x408)
#define	GT_DDAR_RAS1_HIGH			(GT_64010_BASE + 0x40c)
#define	GT_DDAR_RAS2_LOW 			(GT_64010_BASE + 0x410)
#define	GT_DDAR_RAS2_HIGH			(GT_64010_BASE + 0x414)
#define	GT_DDAR_RAS3_LOW 			(GT_64010_BASE + 0x418)
#define	GT_DDAR_RAS3_HIGH			(GT_64010_BASE + 0x41c)
#define	GT_DDAR_CS0_LOW  			(GT_64010_BASE + 0x420)
#define	GT_DDAR_CS0_HIGH 			(GT_64010_BASE + 0x424)
#define	GT_DDAR_CS1_LOW  			(GT_64010_BASE + 0x428)
#define	GT_DDAR_CS1_HIGH 			(GT_64010_BASE + 0x42c)
#define	GT_DDAR_CS2_LOW  			(GT_64010_BASE + 0x430)
#define	GT_DDAR_CS2_HIGH 			(GT_64010_BASE + 0x434)
#define	GT_DDAR_CS3_LOW  			(GT_64010_BASE + 0x438)
#define	GT_DDAR_CS3_HIGH 			(GT_64010_BASE + 0x43c)
#define	GT_DDAR_BOOT_LOW 			(GT_64010_BASE + 0x440)
#define	GT_DDAR_BOOT_HIGH			(GT_64010_BASE + 0x444)
#define	GT_DDAR_DERROR				(GT_64010_BASE + 0x470)

// GT64010 DRAM Configuration Registers
#define	GT_DRAM_CONFIG				(GT_64010_BASE + 0x448)

// GT64010 DRAM Parameters Registers
#define	GT_DRAM_BANK0_PARAM		(GT_64010_BASE + 0x44c)
#define	GT_DRAM_BANK1_PARAM		(GT_64010_BASE + 0x450)
#define	GT_DRAM_BANK2_PARAM		(GT_64010_BASE + 0x454)
#define	GT_DRAM_BANK3_PARAM		(GT_64010_BASE + 0x458)

// GT64010 Device Paramenters Registers
#define	GT_DEV_BANK0_PARAM		(GT_64010_BASE + 0x45c)
#define	GT_DEV_BANK1_PARAM		(GT_64010_BASE + 0x460)
#define	GT_DEV_BANK2_PARAM		(GT_64010_BASE + 0x464)
#define	GT_DEV_BANK3_PARAM		(GT_64010_BASE + 0x468)
#define	GT_DEV_BOOT_PARAM			(GT_64010_BASE + 0x46c)

// GT64010 DMA Record Registers
#define	GT_DMA_CHAN0_COUNT		(GT_64010_BASE + 0x800)
#define	GT_DMA_CHAN1_COUNT		(GT_64010_BASE + 0x804)
#define	GT_DMA_CHAN2_COUNT		(GT_64010_BASE + 0x808)
#define	GT_DMA_CHAN3_COUNT		(GT_64010_BASE + 0x80c)
#define	GT_DMA_CHAN0_SRC   		(GT_64010_BASE + 0x810)
#define	GT_DMA_CHAN1_SRC   		(GT_64010_BASE + 0x814)
#define	GT_DMA_CHAN2_SRC   		(GT_64010_BASE + 0x818)
#define	GT_DMA_CHAN3_SRC   		(GT_64010_BASE + 0x81c)
#define	GT_DMA_CHAN0_DST		 	(GT_64010_BASE + 0x820)
#define	GT_DMA_CHAN1_DST		 	(GT_64010_BASE + 0x824)
#define	GT_DMA_CHAN2_DST		 	(GT_64010_BASE + 0x828)
#define	GT_DMA_CHAN3_DST		 	(GT_64010_BASE + 0x82c)
#define	GT_DMA_CHAN0_NEXT			(GT_64010_BASE + 0x830)
#define	GT_DMA_CHAN1_NEXT			(GT_64010_BASE + 0x834)
#define	GT_DMA_CHAN2_NEXT			(GT_64010_BASE + 0x838)
#define	GT_DMA_CHAN3_NEXT			(GT_64010_BASE + 0x83c)

// GT64010 DMA Channel Control Registers
#define	GT_DMA_CHAN0_CONTROL		(GT_64010_BASE + 0x840)
#define	GT_DMA_CHAN1_CONTROL		(GT_64010_BASE + 0x844)
#define	GT_DMA_CHAN2_CONTROL		(GT_64010_BASE + 0x848)
#define	GT_DMA_CHAN3_CONTROL		(GT_64010_BASE + 0x84c)

// GT64010 DMA Arbiter Register
#define	GT_DMA_ARBITER				(GT_64010_BASE + 0x860)

// GT64010 Timer/Counter Registers
#define	GT_TC0						(GT_64010_BASE + 0x850)
#define	GT_TC1						(GT_64010_BASE + 0x854)
#define	GT_TC2						(GT_64010_BASE + 0x858)
#define	GT_TC3						(GT_64010_BASE + 0x85c)
#define	GT_TC_CONTROL				(GT_64010_BASE + 0x864)

// GT64010 PCI Internal Registers
#define	GT_PCI_INT_COMMAND		(GT_64010_BASE + 0xc00)
#define	GT_PCI_INT_TO_RETRY 		(GT_64010_BASE + 0xc04)
#define	GT_PCI_INT_RAS10_BSIZE	(GT_64010_BASE + 0xc08)
#define	GT_PCI_INT_RAS32_BSIZE	(GT_64010_BASE + 0xc0c)
#define	GT_PCI_INT_CS20_BSIZE	(GT_64010_BASE + 0xc10)
#define	GT_PCI_INT_CS3_BSIZE		(GT_64010_BASE + 0xc14)
#define	GT_PCI_INT_SERR_MASK		(GT_64010_BASE + 0xc28)
#define	GT_PCI_INT_INT_ACK		(GT_64010_BASE + 0xc34)
#define	GT_PCI_INT_CONF_ADDR		(GT_64010_BASE + 0xcf8)
#define	GT_PCI_INT_CONF_DATA		(GT_64010_BASE + 0xcfc)

// GT64010 Interrupt Registers
#define	GT_INT_CAUSE				(GT_64010_BASE + 0xc18)
#define	GT_INT_CPU_MASK			(GT_64010_BASE + 0xc1c)
#define	GT_INT_PCI_MASK			(GT_64010_BASE + 0xc24)

// GT64010 PCI Configuration Registers
#define	GT_PCI_CONF_DEV_ID			0x0
#define	GT_PCI_CONF_STAT_CMD			0x1
#define	GT_PCI_CONF_CC_REV			0x2
#define	GT_PCI_CONF_HT_LT_CL			0x3
#define	GT_PCI_CONF_RAS10_BASE		0x4
#define	GT_PCI_CONF_RAS32_BASE		0x5
#define	GT_PCI_CONF_CS20_BASE 		0x6
#define	GT_PCI_CONF_CS3_BASE		 	0x7
#define	GT_PCI_CONF_REGS_MEM_ADDR	0x8
#define	GT_PCI_CONF_REGS_IO_ADDR 	0xa
#define	GT_PCI_CONF_INT_PIN_LINE 	0xb


/* PCI CONFIGURATION REGISTER 0 */
typedef union pci_config_reg0
{
	unsigned data;
	struct
	{
		unsigned vendor_id : 16;	/* 15 - 0 */
		unsigned device_id : 16;	/* 31 - 16 */
	} d;
} T_pci_config_reg0;


/* GT64010 PCI CONFIGURATION REGISTER 1 */
typedef union gt64010_pci_config_reg1
{
	unsigned data;
	struct
	{
		unsigned io_en : 1;		/* 0 */
		unsigned mem_en : 1;		/* 1 */
		unsigned mas_en : 1;		/* 2 */
		unsigned not_used5 : 1;		/* 3 */
		unsigned mem_wr_inv : 1;	/* 4 */
		unsigned not_used4 : 1;		/* 5 */
		unsigned p_err_en : 1;		/* 6 */
		unsigned not_used3 : 1;		/* 7 */
		unsigned s_err_en : 1;		/* 8 */
		unsigned not_used2 : 7;		/* 15 - 9 */
		unsigned not_used : 7;		/* 22 - 16 */
		unsigned tar_fast_bb : 1;	/* 23 */
		unsigned data_par_det : 1;	/* 24 */
		unsigned dev_sel_time : 3;	/* 27 - 25 */
		unsigned tar_abort : 1;		/* 28 */
		unsigned mas_abort : 1;		/* 29 */
		unsigned sys_err : 1;		/* 30 */
		unsigned det_par_err : 1;	/* 31 */
	} d;
} T_gt64010_pci_config_reg1;


/* PCI CONFIGURATION REGISTER 2 */
typedef union pci_config_reg2
{
	unsigned data;
	struct
	{
		unsigned revision_id : 8;	/* 7 - 0 */
		unsigned device_class : 8;	/* 15 -8 */
		unsigned sub_class : 8;		/* 23 - 16 */
		unsigned base_class : 8;	/* 31 - 24 */
	} d;
} T_pci_config_reg2;


/* PCI CONFIGURATION REGISTER 3 */
typedef union pci_config_reg3
{
	unsigned data;
	struct
	{
		unsigned cache_line_size : 8;	/* 7 - 0 */
		unsigned latency_timer : 8;	/* 15 - 8 */
		unsigned header_type : 8;	/* 23 - 16 */
		unsigned BIST : 8;		/* 31 - 24 */
	} d;
} T_pci_config_reg3;


/* PCI CONFIGURATION REGISTER 15 */
typedef union pci_config_reg15
{
	unsigned data;
	struct
	{
		unsigned interrupt_line : 8;	/* 7 - 0 */
		unsigned interrupt_pin : 8;	/* 15 - 8 */
		unsigned minimum_grant : 8;	/* 23 - 16 */
		unsigned maximum_latency : 8 ;	/* 31 - 24 */
	} d;
} T_pci_config_reg15;


/* PCI CONFIGURATION REGISTERS */
typedef struct gt64010_pci_configuration_regs
{
	T_pci_config_reg0		reg0;
	T_gt64010_pci_config_reg1	reg1;
	T_pci_config_reg2		reg2;
	T_pci_config_reg3		reg3;
	unsigned			reg4;
	unsigned			reg5;
	unsigned			reg6;
	unsigned			reg7;
	unsigned			reg8;
	unsigned			reg9;
	unsigned			reserved_regs[5];
	T_pci_config_reg15		reg15;
} T_gt64010_pci_configuration_regs;
	

/* GT64010 PCI INTERNAL CONFIG ADDR REGISTER */
typedef union pci_internal_config_addr_data
{
	unsigned	data;
	struct
	{
		unsigned	unused : 2;		/* 1 - 0 */
		unsigned	reg_num : 6;		/* 7 - 2 */
		unsigned	func_num : 3;		/* 10 - 8 */
		unsigned	dev_num : 5;		/* 15 - 11 */
		unsigned	bus_num : 8;		/* 23 - 16 */
		unsigned	reserved : 7;		/* 30 - 24 */
		unsigned	access_ctrl : 1;	/* 31 */
	} d;
} T_pci_internal_config_addr_data;

/* GT64010 DMA CHANNEL CONTROL REGISTER DATA DEFINITION */
typedef union dma_control
{
	unsigned data;
	struct
	{
		unsigned reserved : 2;			/* 1 - 0 */
		unsigned src_direction : 2;		/* 3 - 2 */
		unsigned dest_direction : 2;		/* 5 - 4 */
		unsigned transfer_limit : 3;		/* 8 - 6 */
		unsigned chained_mode : 1;		/* 9 */
		unsigned interrupt_mode : 1;		/* 10 */
		unsigned transfer_mode : 1;		/* 11 */
		unsigned enable : 1;			/* 12 */
		unsigned fetch_next_record : 1;		/* 13 */
		unsigned status : 1;			/* 14 */
		unsigned not_used : 17;			/* 31 - 15 */
	} d;
} T_dma_control;

// Definition of GT64010 Dma Control structure bits
#define	GT_DMA_ACTIVE_STATUS				1
#define	GT_DMA_NOT_ACTIVE_STATUS		0
#define	GT_DMA_FORCE_FETCH_NEXT			1
#define	GT_DMA_TRANSFER_MODE_DEMAND	0
#define	GT_DMA_TRANSFER_MODE_BLOCK		1
#define	GT_DMA_INT_TERMINAL_COUNT		0
#define	GT_DMA_INT_CHAIN_END				1
#define	GT_DMA_CHAINED_MODE_ENBL		0
#define	GT_DMA_NON_CHAINED_MODE_ENBL	1
#define	GT_DMA_TRANSFER_LIMIT_4			0
#define	GT_DMA_TRANSFER_LIMIT_8			1
#define	GT_DMA_TRANSFER_LIMIT_16		3
#define	GT_DMA_TRANSFER_LIMIT_32		7
#define	GT_DMA_DEST_INCREMENT			0
#define	GT_DMA_DEST_DECREMENT			1
#define	GT_DMA_DEST_HOLD					2
#define	GT_DMA_SRC_INCREMENT				0
#define	GT_DMA_SRC_DECREMENT				1
#define	GT_DMA_SRC_HOLD					2


/* DMA Controller control register defines */
#define	GT_DMA_INCREMENT_SRC				(0<<2)
#define	GT_DMA_DECREMENT_SRC				(1<<2)
#define	GT_DMA_HOLD_SRC					(2<<2)
#define	GT_DMA_INCREMENT_DST				(0<<4)
#define	GT_DMA_DECREMENT_DST				(1<<4)
#define	GT_DMA_HOLD_DST					(2<<4)
#define	GT_DMA_4								(0<<6)
#define	GT_DMA_8								(1<<6)
#define	GT_DMA_16							(3<<6)
#define	GT_DMA_32							(7<<6)
#define	GT_DMA_CHAINED_MODE				(0<<9)
#define	GT_DMA_NONCHAINED_MODE			(1<<9)
#define	GT_DMA_INT_TERMINAL				(0<<10)
#define	GT_DMA_INT_NULL					(1<<10)
#define	GT_DMA_DEMAND_MODE				(0<<11)
#define	GT_DMA_BLOCK_MODE					(1<<11)
#define	GT_DMA_DISABLE						(0<<12)
#define	GT_DMA_ENABLE						(1<<12)
#define	GT_DMA_FETCH_NEXT					(1<<13)
#define	GT_DMA_NOT_ACTIVE					(0<<14)
#define	GT_DMA_ACTIVE						(1<<14)

/* GT64010 DMA ARBITER REGISTER DATA */
typedef union dma_arbiter
{
	unsigned data;
	struct
	{
		unsigned channel_1_0_priority : 2;	/* 1 - 0 */
		unsigned channel_3_2_priority : 2;	/* 3 - 2 */
		unsigned group_priority : 2;			/* 5 - 4 */
		unsigned priority_option : 1;			/* 6 */
		unsigned reserved : 25;					/* 31 - 7 */
	} d;
} T_dma_arbiter;

// Definition of dma arbiter register bits
#define	GT_ROUND_ROBIN				0
#define	GT_CHANNEL_1_PRIORITY	1
#define	GT_CHANNEL_0_PRIORITY	2
#define	GT_CHANNEL_3_PRIORITY	CHANNEL_1_PRIORITY
#define	GT_CHANNEL_2_PRIORITY	CHANNEL_0_PRIORITY
#define	GT_GROUP_23_PRIORITY		1
#define	GT_GROUP_01_PRIORITY		2
#define	GT_RELINQUISH				0
#define	GT_GRANT						1


/* GT64010 TIMER/COUNTER CONTROL REGISTER DATA */
typedef union timer_counter_control
{
	unsigned data;
	struct
	{
		unsigned tc0_enable : 1;	/* 0 */
		unsigned tc0_type : 1;		/* 1 */
		unsigned tc1_enable : 1;	/* 2 */
		unsigned tc1_type : 1;		/* 3 */
		unsigned tc2_enable : 1;	/* 4 */
		unsigned tc2_type : 1;		/* 5 */
		unsigned tc3_enable : 1;	/* 6 */
		unsigned tc3_type : 1;		/* 7 */
		unsigned reserved : 24;		/* 31 - 8 */
	} d;
} T_timer_counter_control;

// Definition of timer counter modes
#define	GT_TIMER_TYPE_COUNTER 	0
#define	GT_TIMER_TYPE_TIMER	 	1


// Assorted Definitions
#define	GTREG(X)						(X)
#define	GT_ENABLE					1
#define	GT_DISABLE					0
#define	GT_ALLOW_ACCESS			1
#define	GT_RESET						0
#define	GT_ACTIVE					1
#define	GT_NOT_ACTIVE				0

// Timer start value
#define	GT_TIMER_START_VAL		0x00ffffff

// Function prototypes
int get_pci_config_reg(int dev_num, int reg_num);
void put_pci_config_reg(int dev_num, int reg_num, int data);
int gt64010_init(void);
void get_pci_slot_configuration_info(int *to, int dev_num);
void disable_dma(int channel);


// Device Numbers
#define GT64010_DEVICE_NUMBER		0
#define PCI_SLOT_2_DEVICE_NUMBER	8
#define PCI_SLOT_1_DEVICE_NUMBER	7
#define PCI_SLOT_0_DEVICE_NUMBER	6


// GT64010 version number
#define	GT64010_REV					1

#endif	// __GT64010_H__
