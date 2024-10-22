/** @file
  This driver provides Bios Region Lock in DXE

;*******************************************************************************
;* Copyright (c) 2012 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/
#include <Uefi.h>
#include <MmioAccess.h>
#include <Fch.h>
#include <BiosRegionLockInfo.h>
#include <BiosRegionLockHelpFun.h>
#include <Library/DebugLib.h>
#include <ChipsetSmiTable.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/S3BootScriptLib.h>

EFI_STATUS
InitializeSpiProtectLockSMI(
  VOID
  );

EFI_STATUS
AddNewRegion (
  IN BIOS_REGION_LOCK_BASE  *BiosLock,
  IN UINTN                   Base,
  IN UINTN                   Length
  )
{
  INTN                  Index1;
  UINT8                 Index2;
  UINTN                 LocalBase;
  BOOLEAN               FlagOutOfResource = TRUE;
  BIOS_REGION_LOCK_BASE LockTemp[MAX_BIOS_REGION_LOCK + 1];
  UINTN                 MaxLengthPerSet;

  LocalBase = Base;
  MaxLengthPerSet = (UINTN)PcdGet32 (PcdFchLimitProtectSizePerSet);

  ZeroMem ((VOID *)LockTemp, sizeof(BIOS_REGION_LOCK_BASE) * (MAX_BIOS_REGION_LOCK + 1));
  CopyMem ((VOID *)LockTemp, (VOID *)BiosLock, sizeof(BIOS_REGION_LOCK_BASE) * MAX_BIOS_REGION_LOCK);

  if (Length > MaxLengthPerSet) {
    //
    // If input Length is larger than MaxLengthPerSet, split the input value into many groups.
    //
    for (Index1 = 0; Index1 < MAX_BIOS_REGION_LOCK; Index1 ++ ) {
      if (LockTemp[Index1].Length == 0) {
        break;
      }
    } 

    for (Index2 = 0; (Index2 < (Length / MaxLengthPerSet)) && (Index1 < MAX_BIOS_REGION_LOCK); Index2 ++ ) {
      LockTemp[Index1].Base   = LocalBase;
      LockTemp[Index1].Length = MaxLengthPerSet;
      LocalBase = LocalBase + MaxLengthPerSet;
      Index1 ++;
    }

    //
    // Wirte the remaining length
    //
    if ((Index1 >= MAX_BIOS_REGION_LOCK) && (Length > (MaxLengthPerSet * Index2))) {
      LockTemp[Index1].Base   = LocalBase;
      LockTemp[Index1].Length = Length - (MaxLengthPerSet * Index2);
    }

    FlagOutOfResource = FALSE;
    if (LockTemp[MAX_BIOS_REGION_LOCK].Length != 0) {
      FlagOutOfResource = TRUE;
    }
  } else {
    //
    // If input Length is less than MaxLengthPerSet, just find a empty set to fill the input value.
    //
    for (Index1 = 0; Index1 < MAX_BIOS_REGION_LOCK; Index1 ++ ) {
      if (LockTemp[Index1].Length == 0) {
        LockTemp[Index1].Base   = Base;
        LockTemp[Index1].Length = Length;
        FlagOutOfResource = FALSE;
        break;
      }
    } 
  }

  if (FlagOutOfResource) {
    return EFI_OUT_OF_RESOURCES;
  }

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR | EFI_D_INFO, "\nBiosRegionLock Array Status....\n"));
  for (Index1 = 0; Index1 < MAX_BIOS_REGION_LOCK; Index1 ++ ) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "%d => Base:%8x  Length:%8x\n", Index1, LockTemp[Index1].Base, LockTemp[Index1].Length));
  }
#endif

  CopyMem ((VOID *)BiosLock, (VOID *)LockTemp, sizeof(BIOS_REGION_LOCK_BASE) * MAX_BIOS_REGION_LOCK);

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
GetSpiRomBaseAddress (
  OUT  UINT32  *SpiRomBaseAddress
  )
{
  *SpiRomBaseAddress = (PciRead32 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REGA0))) & 0xFFFFFF00;  // Raven: Bit[31:8]: Spi_eSpi_BaseAddr.
                       
  return EFI_SUCCESS;
}

BOOLEAN
GetSpiProtectLockStatus (
  VOID
  )
{
  UINT32  SpiRomBaseAddress;
  UINT8   SpiProtectLock;

  GetSpiRomBaseAddress (&SpiRomBaseAddress);

  //
  // Check Alt_SPI_CS[5] SpiProtectLock to get lock status
  //
  SpiProtectLock = Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D);   //SB_SPI_MMIO_REG1D

  if (SpiProtectLock & BIT5) {
    return TRUE;
  }
  
  return FALSE;
}
EFI_STATUS
CreateS3ResumeScriptTable (
  VOID
  ) {
  UINTN      Index;
  UINT32     PRRegister;
  UINT32     SpiRomBaseAddress;
  UINTN      Data;
  UINTN      DataMask;
  
  for (Index = 0; Index < MAX_BIOS_REGION_LOCK; Index ++) {
    PRRegister = PciRead32(PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REG50 + (Index * 4)));

    S3BootScriptSavePciCfgWrite (
      S3BootScriptWidthUint32,
      S3_BOOT_SCRIPT_LIB_PCI_ADDRESS(FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REG50 + (Index * 4)),
      1,
      (VOID *) &PRRegister
      );
    
  }
  GetSpiRomBaseAddress (&SpiRomBaseAddress);

  //
  // Restricted Cmd "01"(Write status register) & "C7"(Chip erase)
  //
  Data = 0xC701;
  DataMask = 0;

  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint16,
    SpiRomBaseAddress + 0x04,
    &Data,
    &DataMask
    );

  //
  // Set Alt_SPI_CS[5:3] SpiProtectEn0, SpiProtectEn1 and SpiProtectLock to 1 to protect Spi memory access
  //
  Data = (BIT3 + BIT4 + BIT5);
  DataMask = 0xFF;

  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint8,
    SpiRomBaseAddress + FCH_SPI_MMIO_REG1D,
    &Data,
    &DataMask
    );

  //
  // Set SpiAccessMacRomEn and SpiHostAccessRomEn to 0 to enable SPI indirect access protect
  //
  Data = 0;
  DataMask = (UINTN)(~(BIT6 + BIT7));

  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint8,
    SpiRomBaseAddress + FCH_SPI_MMIO_REG00 + 2,
    &Data,
    &DataMask
    );

  return EFI_SUCCESS;  
}

