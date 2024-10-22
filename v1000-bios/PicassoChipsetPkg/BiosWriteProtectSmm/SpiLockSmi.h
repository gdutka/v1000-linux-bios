/** @file
  Definition for Spi lock smi.

;*******************************************************************************
;* Copyright (c) 2012 - 2016, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/
#ifndef _SPI_LOCK_SMI_H_
#define _SPI_LOCK_SMI_H_
#include <Uefi.h>
#include <MmioAccess.h>
#include <Fch.h>
#include <BiosRamUsage.h>
#include <Protocol/SmmControl2.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/SpiLockSmi.h>
#include <Protocol/BiosRegionLock.h>
#include <ChipsetSmiTable.h>
#include <BiosRegionLockInfo.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>

#pragma pack(1)

//
// The following definition could be used for both of 0x15 and 0x17 APU.
// "Reserved" is difference between 0x15 and 0x17.
//
typedef struct _FCH_ROM_PROTECT_REGISTER {
  UINT32  Range:8;
  UINT32  RangeUnit:1;
  UINT32  ReadProtect:1;
  UINT32  WriteProtect:1;
  UINT32  Reserved:1;
  UINT32  RomBase:20;
} FCH_ROM_PROTECT_REGISTER;

#pragma pack()

#endif //_SPI_LOCK_SMI_H_
