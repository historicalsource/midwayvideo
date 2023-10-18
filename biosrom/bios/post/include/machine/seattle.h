#ifndef _SEATTLE_H_
#define _SEATTLE_H_

/*
имммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм╩
╨                                                                            ╨
╨ File:    SEATTLE.H                                                         ╨
╨ Author:  Jack Miller                                                       ╨
╨ Created: 25-Jan-1997                                                       ╨
╨                                                                            ╨
лмммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм╧
╨                                                                            ╨
╨     Memory and I/O map manifest definitions for the Seattle system.        ╨
╨                                                                            ╨
лмммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм╧
╨ HISTORY:                                                                   ╨
╨                                                                            ╨
╨  25Jan97 JVM  Created.                                                     ╨
╨                                                                            ╨
хмммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммммм╪
*/
#include <compiler.h>


#define __SEATTLE__ 1           /* This is the Seattle Development System */
#define CPU_SPEED   150000000   /* CPU clock speed in Hertz */


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                             System Memory Map                              Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define DRAM_BANK0_BASE             0x00000000

#define PCI_MEMORY_BASE             0x08000000
#define PCI_IO_BASE                 0x0A000000

#define GT64010_RESET_BASE          0x14000000
#define GT64010_BASE                0x0C000000

#define EXP0_BASE                   0x10000000
#define EXP1_BASE                   0x12000000

#define DCS2_BASE                   0x13000000

#define EXP2_BASE                   0x14000000

#define ONBOARD_IO_BASE             0x16000000

#define BOOTROM_BASE                0x1FC00000
#define EXPROM_BASE                 0x1FD00000


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                             System Control PLDs                            Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define IO_ASIC_BASE                (ONBOARD_IO_BASE + 0x00000000)
#define NVRAM_BASE                  (ONBOARD_IO_BASE + 0x00100000)
#define RESERVED1_BASE              (ONBOARD_IO_BASE + 0x00200000)
#define SPARE_CS0_BASE              (ONBOARD_IO_BASE + 0x00300000)
#define SPARE_CS1_BASE              (ONBOARD_IO_BASE + 0x00400000)
#define SPARE_CS2_BASE              (ONBOARD_IO_BASE + 0x00500000)
#define RESERVED2_BASE              (ONBOARD_IO_BASE + 0x00600000)

#define MISC_IO_BASE                (ONBOARD_IO_BASE + 0x00C00000)

#define NVRAM_UNLOCK_ADDR           (ONBOARD_IO_BASE + 0x01000000)

#define WATCH_DOG_TIMER_ADDR        (ONBOARD_IO_BASE + 0x01100000)

#define NMI_SEL_ENA_REG             (ONBOARD_IO_BASE + 0x01200000)
#define INTERRUPT_ENA_REG           (ONBOARD_IO_BASE + 0x01300000)
#define INTERRUPT_MAP_REG           (ONBOARD_IO_BASE + 0x01400000)
#define INTERRUPT_CAUSE_REG         (ONBOARD_IO_BASE + 0x01500000)
#define GENERAL_STAT_REG            (ONBOARD_IO_BASE + 0x01600000)

#define CLEAR_VSYNC_INT_ADDR        (ONBOARD_IO_BASE + 0x01700000)

#define CONTROL_LATCH_IO_REG        (ONBOARD_IO_BASE + 0x01800000)
#define CONTROL_LED_IO_REG          (ONBOARD_IO_BASE + 0x01900000)

#define RESET_REG                   (ONBOARD_IO_BASE + 0x01F00000)


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                       General Purpose Status Register                      Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define GPSR_NMI                    (1 << 12)
#define GPSR_AUX11_READ_BACK        (1 << 11)
#define GPSR_WATCH_DOG              (1 << 10)
#define GPSR_BATTERY_LOW            (1 << 9)
#define GPSR_VSYNC_INPUT            (1 << 8)
#define GPSR_VSYNC_LATCHED_IRQ      (1 << 7)
#define GPSR_DEBUG_SW               (1 << 6)
#define GPSR_RESERVED_5             (1 << 5)
#define GPSR_ADC_IRQ                (1 << 4)
#define GPSR_PCI_SLOT_IRQ           (1 << 3)
#define GPSR_RESERVED_2             (1 << 2)
#define GPSR_MISC_IO_IRQ            (1 << 1)
#define GPSR_ATARI_EXP              (1 << 0)


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                         NMI Select/Enable Register                         Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define NMI_ATARI_EXP               0x01
#define NMI_MISC_IO                 0x02
#define NMI_DEBUG_SW                0x03


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                          Interrupt Cause Register                          Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define ICR_VSYNC                   (1 << 7)
#define ICR_DEBUG_SW                (1 << 6)
#define ICR_RESERVED5               (1 << 5)
#define ICR_ADC                     (1 << 4)
#define ICR_PCI_SLOT                (1 << 3)
#define ICR_RESERVED2               (1 << 2)
#define ICR_MISC_IO                 (1 << 1)
#define ICR_ATARI_EXP               (1 << 0)


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                          Interrupt Enable Register                         Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define IER_VSYNC_POSITIVE          (1 << 8)
#define IER_VSYNC                   (1 << 7)
#define IER_DEBUG_SW                (1 << 6)
#define IER_RESERVED5               (1 << 5)
#define IER_ADC                     (1 << 4)
#define IER_PCI_SLOT                (1 << 3)
#define IER_RESERVED2               (1 << 2)
#define IER_MISC_IO                 (1 << 1)
#define IER_ATARI_EXP               (1 << 0)


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                           Interrupt Map Register                           Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
/* Interrupt Mapping */
/* macro usage: MAP_INT(MAP_EXP0, TO_CPUX) */

#define MAP_INT(device, cpuint)     (cpuint << device)

/* CPU Interrupt Inputs */
#define TO_CPUX                     0
#define TO_CPU3                     1
#define TO_CPU4                     2
#define TO_CPU5                     3

/* Interrupt Mapping Bits */
#define MAP_ATARI_EXP               0
#define MAP_MISC_IO                 2
#define MAP_RESERVED_5_4            4
#define MAP_PCI_SLOT                6
#define MAP_ADC                     8
#define MAP_RESERVED_11_10          10
#define MAP_DEBUG_SW                12
#define MAP_VSYNC_INPUT             14


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                          Control LED Register Bits                         Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define CTRL_LED_GRN                (1 << 2)
#define CTRL_LED_YEL                (1 << 1)
#define CTRL_LED_RED                (1 << 0)


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                       Control Latch I/O Register Bits                      Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define CLIO_TEST_POINT_1           (1 << 2)
#define CLIO_TEST_POINT_2           (1 << 1)
#define CLIO_ARB_ROUND_ROBIN        (1 << 0)
#define CLIO_ARB_PARK_GALILEO       0


/*
здддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддд©
Ё                             Reset Register Bits                            Ё
юдддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддддды
*/
#define ENABLE_SPARE                (1 << 5)
#define ENABLE_MISC_IO              (1 << 4)
#define ENABLE_3DFX                 (1 << 3)
#define ENABLE_PCI_IDE              (1 << 2)
#define ENABLE_IOASIC_PIC           (1 << 1)
#define ENABLE_ATARI_EXP            (1 << 0)


#endif  // _SEATTLE_H_
