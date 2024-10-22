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
#include <Fch.h>
#include <ChipsetSmiTable.h>
#include <BiosRegionLockInfo.h>
#include <BiosRegionLockHelpFun.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/IoLib.h>

/**
  Lock Spi command in End of DXE .

  @param  Event                 Event whose notification function is being invoked.
  @param  Context               The pointer to the notification function's context,
                                which is implementation-dependent.

**/
VOID
EFIAPI
EndOfDxeLockSpi (
  IN EFI_EVENT                          Event,
  IN VOID                               *Context
  )
{
/* Arista remove
  UINT32                                SpiControl0Reg;
  UINT16                                SPIRestrictedCmd;
  UINTN                                 SpiRomBaseAddress;
  UINT32                                Data; 
  UINT32                                DataMask;

  SpiRomBaseAddress = (PciRead32 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REGA0))) & 0xFFFFFF00;
  //
  // Restricted Cmd "01"(Write status register) & "C7"(Chip erase)
  //
  SPIRestrictedCmd = MmioRead16 (SpiRomBaseAddress + 0x04);
  SPIRestrictedCmd |= (BIT0 + BIT8 + BIT9 + BIT10 + BIT14 + BIT15);
  MmioWrite16 (SpiRomBaseAddress + 0x04, SPIRestrictedCmd);

  //
  // Set SpiAccessMacRomEn and SpiHostAccessRomEn to 0 to enable SPI indirect access protect
  //
  SpiControl0Reg = MmioRead32 (SpiRomBaseAddress);
  SpiControl0Reg &= ~(BIT22 + BIT23);
  MmioWrite32 (SpiRomBaseAddress, SpiControl0Reg);

  //
  //S3Resume
  //
  SPIRestrictedCmd = 0x01C7;
  DataMask = 0;

  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint16,
    SpiRomBaseAddress + 0x04,
    &SPIRestrictedCmd,
    &DataMask
    );

  //
  // Set SpiAccessMacRomEn and SpiHostAccessRomEn to 0 to enable SPI indirect access protect
  //
  Data = 0;
  DataMask = (UINT32)(~(BIT22 + BIT23));

  S3BootScriptSaveMemReadWrite (
    S3BootScriptWidthUint32,
    SpiRomBaseAddress,
    &Data,
    &DataMask
    );
*/
  return;
}


EFI_STATUS
BiosRegionLockInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_HANDLE                            Handle;
  EFI_STATUS                            Status;
  BIOS_REGION_LOCK_INSTANCE            *BiosRegionLockInstance;
  BIOS_REGION_LOCK_PROTOCOL            *BiosRegionLock;
  EFI_EVENT                             EndOfDxeEvent;
  
  BiosRegionLockInstance = AllocateZeroPool (sizeof(BIOS_REGION_LOCK_INSTANCE));
  if (BiosRegionLockInstance == NULL) {
    DEBUG ((EFI_D_ERROR, "Allocate Pool Failure!\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  BiosRegionLock = &BiosRegionLockInstance->BiosRegionLock;
  BiosRegionLock->SetRegionByType      = SetRegionByType;
  BiosRegionLock->SetRegionByAddress   = SetRegionByAddress;
  BiosRegionLock->ClearRegionByType    = ClearRegionByType;
  BiosRegionLock->ClearRegionByAddress = ClearRegionByAddress;
  BiosRegionLock->Lock                 = Lock;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gEfiBiosRegionLockProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 BiosRegionLock
                 );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Install EfiBiosRegionLockProtocol Failure!\n"));
    gBS->FreePool (BiosRegionLockInstance);
  }
#ifdef EFI_DEBUG
  else {
    DEBUG ((EFI_D_ERROR, "Install EfiBiosRegionLockProtocol Success!\n"));
  }
#endif

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EndOfDxeLockSpi,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );

  return Status;
}
