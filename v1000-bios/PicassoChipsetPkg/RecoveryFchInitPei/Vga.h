#ifndef _PEI_VGA_H_
#define _PEI_VGA_H_

#define VGA_CLASSCODE              0x03000000

#define VGA_PREFECTABLE_BAR0       0x10
#define VGA_PREFECTABLE_BAR1       0x14
#define VGA_PREFECTABLE_BAR2       0x18
#define VGA_PREFECTABLE_BAR3       0x1C
#define VGA_IO_BAR4                0x20
#define VGA_NON_PREFECTABLE_BAR5   0x24

#define RES_IO_BASE        0x2000
#define VGA_IO_SIZE        0x100
#endif