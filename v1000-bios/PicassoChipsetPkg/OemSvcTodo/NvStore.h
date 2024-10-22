//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;

#ifndef _NV_STORE_H
#define _NV_STORE_H
#pragma pack(1)

#define DEFAULT_BATTERY_VALUE           25600
#define PRIMARY_DISPLAY                 1
#define DEFAULT_BRIGHTNESS_LEVEL        60

#define PIPE_A_CRT                      0x01
#define PIPE_A_TV1                      0x02
#define PIPE_A_EFP1                     0x04
#define PIPE_A_LFP                      0x08
#define PIPE_A_TV2                      0x10
#define PIPE_A_EFP2                     0x20
#define PIPE_B_CRT                      PIPE_A_CRT  << 8
#define PIPE_B_TV1                      PIPE_A_TV1  << 8
#define PIPE_B_EFP1                     PIPE_A_EFP1 << 8
#define PIPE_B_LFP                      PIPE_A_LFP  << 8
#define PIPE_B_TV2                      PIPE_A_TV2  << 8
#define PIPE_B_EFP2                     PIPE_A_EFP2 << 8

#define DEFAULT_LCD_ADDRESS             0x400

typedef struct {
  UINT8 	SMIF;	// SMI Function Call (ASL to SMI via I/O Trap)
  UINT8 	PRM0;	// SMIF - Parameter 0
  UINT8 	PRM1;	// SMIF - Parameter 1
  UINT8 	BRTL; // Brightness Level
  UINT8 	TLST;	// Display Toggle List Selection
  UINT8 	IGDS;	// IGD State (Primary Display = 1)
  UINT16	LCDA;	// LCD Address (Default = 0x400)
  UINT16	CSTE;	// Current Display State
  UINT16	NSTE;	// Next Display State
  UINT16	CADL;	// Current Attached Device List
  UINT16	PADL;	// Previous Attached Device List
  UINT8   LIDS; // Lid State (Lid Open = 1)
  UINT8   PWRS; // Power State (AC Mode = 1)
  UINT32  BVAL; // 32-bit Battery Value returned in _BST
  UINT16  ADDL; // Available Display Device List
  UINT8   BCMD; // SMI command
  UINT8   SBFN; // Sub-function for SMI command
  UINT32  DID;  // Function ID for ATIF
  UINT8   INFO[0x100]; // Information buffer for ATIF
  UINT8   TOML; // Low byte of Top of memory register
  UINT8   TOMH; // Hight byte of Top of memory register
  UINT8   EnabledCpuBitMap; //A bit map to indicate which CPU has support PowerNow
  UINT8   Cpu0LowestState; //The Lowest P-state number of CPU0
  UINT8   Cpu1LowestState; //The Lowest P-state number of CPU1
  UINT8   Cpu0HighestState; //The Highest P-state number of CPU0
  UINT8   Cpu1HighestState; //The Highest P-state number of CPU1
  UINT32  RomSize;
  UINT8   MUXF;
  UINT8   PDDN; // Primary display device number
  UINT8   CNSB; //Connect Standby support
  UINT8   RDHW; //Reduced Hardware Support
} NV_STORE_STRUCT;

#pragma pack()

#endif
