/** @file
  This driver provides Spi Lock interface in SMM mode

;*******************************************************************************
;* Copyright (c) 2015 - 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;*******************************************************************************
*/
#include "SpiLockSmi.h"

SPI_LOCK_SMI_PROTOCOL      mSpiLockSmiProtocol;

EFI_STATUS
SmmLockSpiAccess (
  IN       EFI_HANDLE                      DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT     *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT              *SwContext,
  IN OUT   UINTN                           *SizeOfSwContext
  );

EFI_STATUS
SmmUnlockSpiAccess (
  IN       EFI_HANDLE                      DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT     *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT              *SwContext,
  IN OUT   UINTN                           *SizeOfSwContext
  );

EFI_STATUS
SmmSpiProgram (
  IN       EFI_HANDLE                      DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT     *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT              *SwContext,
  IN OUT   UINTN                           *SizeOfSwContext
  );

EFI_STATUS
RelocateSecureFlashImageAddress (
  VOID
  );

EFI_STATUS
LockSpi (
  IN SPI_LOCK_SMI_PROTOCOL  *This
  );

EFI_STATUS
UnlockSpi (
  IN SPI_LOCK_SMI_PROTOCOL  *This
  );

STATIC
EFI_STATUS
GetSpiRomBaseAddress (
  OUT  UINTN  *SpiRomBaseAddress
  )
{
  *SpiRomBaseAddress = (PciRead32 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REGA0))) & 0xFFFFFF00;  // Raven: Bit[31:8]: Spi_eSpi_BaseAddr.

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
InstallSpiLockSmiProtocol (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 Handle;
  SPI_LOCK_SMI_PROTOCOL      *SpiLockSmiProtocol;
  UINTN                      SpiBar;

  Status = gSmst->SmmLocateProtocol(&gEfiSpiLockSmiProtocolGuid, NULL, (VOID**)&SpiLockSmiProtocol);

  if (EFI_ERROR (Status)) {
    GetSpiRomBaseAddress (&SpiBar);
    mSpiLockSmiProtocol.LockSpi = LockSpi;
    mSpiLockSmiProtocol.UnlockSpi = UnlockSpi;
    mSpiLockSmiProtocol.SpiBarAddress = SpiBar;
      
    Handle = NULL;
    Status = gSmst->SmmInstallProtocolInterface(
                       &Handle,
                       &gEfiSpiLockSmiProtocolGuid,
                       EFI_NATIVE_INTERFACE,
                       &mSpiLockSmiProtocol
                       );
 }
  
 return Status;
}

STATIC
EFI_STATUS
ForceTriggerSmiToClearSpiLockBits (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_CONTROL2_PROTOCOL             *SmmControl;
  UINT8                                 DataBuffer;
  UINT8                                 DataSize;

  Status = gBS->LocateProtocol (&gEfiSmmControl2ProtocolGuid, NULL, (VOID**)&SmmControl);
  
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  DataBuffer = SPI_ROM_PROTECT_UNLOCK;
  DataSize = 1;
  Status = SmmControl->Trigger (SmmControl, &DataBuffer, &DataSize, FALSE, 0);
  
  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
UnlockSpiI (
  IN  UINTN  SpiRomBaseAddress
  )
{
  UINT32  SpiControl0Reg;
  UINT8   SpiProtectLock;
  
  SpiControl0Reg = Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00);
  SpiProtectLock = Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D);

  if (((SpiControl0Reg & BIT22) == 0) || ((SpiControl0Reg & BIT23) == 0) || (SpiProtectLock & BIT5)) {
    SpiControl0Reg |= (BIT22 + BIT23);  // set to 1 is unlock
    Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00) = SpiControl0Reg;
    
    SpiProtectLock &= ~(BIT5);
    Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D) = SpiProtectLock;

    SpiProtectLock &= ~(BIT3 + BIT4);
    Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D) = SpiProtectLock;

    return EFI_SUCCESS;
  }

  return EFI_NOT_STARTED;
}

EFI_STATUS
LockSpiI (
  IN  UINTN  SpiRomBaseAddress
  )
{
  UINT32  SpiControl0Reg;
  UINT8   SpiProtectLock;
  
  SpiControl0Reg = Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00);
  SpiControl0Reg &= ~(BIT22 + BIT23);  // set to 0 is lock
  Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00) = SpiControl0Reg;

  SpiProtectLock = Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D);
  SpiProtectLock |= (BIT3 + BIT4 + BIT5);
  Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D) = SpiProtectLock;
  
  return EFI_SUCCESS;
}

EFI_STATUS
LockSpi (
  IN SPI_LOCK_SMI_PROTOCOL  *This
  )
{
  EFI_STATUS  Status;

  Status = LockSpiI (This->SpiBarAddress);
  
  return Status;
}

EFI_STATUS
UnlockSpi (
  IN SPI_LOCK_SMI_PROTOCOL  *This
  )
// EFI_SUCCESS: Clear
// EFI_NOT_STARTED: Not Clear
{
  EFI_STATUS  Status;

  Status = UnlockSpiI (This->SpiBarAddress);
  
  return Status;
}

STATIC
EFI_STATUS
CheckSpiLockStatus (
  VOID
  )
{
  EFI_STATUS  Status;
  UINT32      SpiControl0Reg;
  UINT8       SpiProtectLock;
  UINTN       SpiRomBaseAddress;
  //
  // Clear the SPI lock bits here.
  //
  Status = UnlockSpiI (mSpiLockSmiProtocol.SpiBarAddress);

  //
  // Check the Lock bits are all set to unlock state.
  // If either one is in lock state, then trigger a SMI to set them to unlock state.
  //
  GetSpiRomBaseAddress (&SpiRomBaseAddress);
  SpiControl0Reg = Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00);
  SpiProtectLock = Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D);

  if (((SpiControl0Reg & BIT22) == 0) || ((SpiControl0Reg & BIT23) == 0) || (SpiProtectLock & BIT5)) {
    Status = ForceTriggerSmiToClearSpiLockBits ();
  }

  //
  // If UnlockSpiI() or ForceTriggerSmiToClearSpiLockBits() return EFI_SUCCESS here, 
  // it means system boot in SPI locked state,
  // Some SPI ROM access might not work properly before this driver, 
  // System need a reset to reboot with SPI unlocked state.
  //
  if (!EFI_ERROR (Status)) {
    if (FeaturePcdGet(PcdSecureFlashSupported)) {
      RelocateSecureFlashImageAddress ();
    }
    gRT->ResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
  }

  return Status;
}

EFI_STATUS
SpiLockSmiInit (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status = EFI_SUCCESS;
  EFI_HANDLE                            Handle;
  FCH_SMM_SW_DISPATCH2_PROTOCOL         *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT           SwContext;
  UINT8                                 SpiLockStatus;

  IoWrite8 (FCH_IOMAP_REGCD4, SPI_LOCK_PROTECT_STATUS);
  SpiLockStatus = IoRead8 (FCH_IOMAP_REGCD5);
  SpiLockStatus &= ~(SPI_LOCK_SMI_READY);
  IoWrite8 (FCH_IOMAP_REGCD5, SpiLockStatus);
  
  Status = InstallSpiLockSmiProtocol ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  //  Locate the SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (&gFchSmmSwDispatch2ProtocolGuid, NULL, (VOID**)&AmdSwDispatch);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  SwContext.AmdSwValue = SPI_ROM_PROTECT_LOCK;
  SwContext.Order = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            SmmLockSpiAccess,
                            &SwContext,
                            &Handle
                            );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Register a SwSMI callback
  //
  SwContext.AmdSwValue = SPI_ROM_PROTECT_UNLOCK;
  SwContext.Order = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            SmmUnlockSpiAccess,
                            &SwContext,
                            &Handle
                            );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  //
  // Register a SwSMI callback
  //
  SwContext.AmdSwValue = SPI_ROM_PROTECT_PROGRAM;
  SwContext.Order = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            SmmSpiProgram,
                            &SwContext,
                            &Handle
                            );
  if (EFI_ERROR(Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  CheckSpiLockStatus ();

  //
  // SMI for Lock/Unlock SPI regisers are ready to use,
  // so set up the flag SPI_LOCK_SMI_READY.
  //
  IoWrite8 (FCH_IOMAP_REGCD4, SPI_LOCK_PROTECT_STATUS);
  SpiLockStatus = IoRead8 (FCH_IOMAP_REGCD5);
  SpiLockStatus |= (SPI_LOCK_SMI_READY);
  IoWrite8 (FCH_IOMAP_REGCD5, SpiLockStatus);

  return Status;
}

EFI_STATUS
SmmUnlockSpiAccess (
  IN       EFI_HANDLE                      DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT     *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT              *SwContext,
  IN OUT   UINTN                           *SizeOfSwContext
  )
{
  EFI_STATUS  Status;

  Status = UnlockSpiI (mSpiLockSmiProtocol.SpiBarAddress);

  return Status;

}

EFI_STATUS
SmmLockSpiAccess (
  IN       EFI_HANDLE                      DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT     *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT              *SwContext,
  IN OUT   UINTN                           *SizeOfSwContext
  )
{
  EFI_STATUS  Status;

  Status = LockSpiI (mSpiLockSmiProtocol.SpiBarAddress);

  return Status;

}

#define ROM_RANGE_UNIT_4K   0x1000
#define ROM_RANGE_UNIT_64K  0x10000

VOID
SetMemI (
  IN VOID     *Buffer,
  IN UINTN    Size,
  IN UINT8    Value
  )
{
  INT8  *pt;

  ASSERT (Buffer != NULL);

  pt = Buffer;
  while (Size--) {
    *(pt++) = Value;
  }
}

VOID
CopyMemI (
  IN VOID     *Dest,
  IN VOID     *Src,
  IN UINTN    len
  )
{
  CHAR8 *d;

  CHAR8 *s;

  d = Dest;
  s = Src;
  while (len--) {
    *(d++) = *(s++);
  }
}

EFI_STATUS
SmmSpiProgram (
  IN       EFI_HANDLE                      DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT     *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT              *SwContext,
  IN OUT   UINTN                           *SizeOfSwContext
  )
{
  EFI_STATUS                          Status;
  BIOS_REGION_LOCK_INSTANCE           *Private;
  BIOS_REGION_LOCK_PROTOCOL           *BiosRegionLock;
  BIOS_REGION_LOCK_BASE               *BiosLock;
  UINTN                               Index;
  FCH_ROM_PROTECT_REGISTER            RomProtectReg;
  UINTN                               RangeUnitValue;
  UINT32                              RomProtectReg32;
  UINTN                               SpiRomBaseAddress;
  UINT32                              SpiControl0Reg;
  UINT8                               SpiProtectLock;
  UINT16                              SpiRestrictedCmd;
  UINT8                               SpiLockStatus;  
  UINTN                               Data;
  UINTN                               DataMask;

  SpiRomBaseAddress = mSpiLockSmiProtocol.SpiBarAddress;
  UnlockSpiI(SpiRomBaseAddress);
  Status = gBS->LocateProtocol (&gEfiBiosRegionLockProtocolGuid, NULL, (VOID**)&BiosRegionLock);

  if (Status != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  
  Private = BIOS_REGION_LOCK_INSTANCE_FROM_THIS (BiosRegionLock);

  BiosLock = Private->BiosLock;

  for (Index = 0; Index < MAX_BIOS_REGION_LOCK; Index++) {
    SetMemI (&RomProtectReg, sizeof (FCH_ROM_PROTECT_REGISTER), 0);
    //
    // Collect RomBase, Range, ProtectType
    // For 0x15 and 0x17 APU,
    //   RomBase    : RomProtect[31:12]
    //   Reserved   : RomProtect[11]
    // Write Protect: RomProtect[10]
    // Read Portect : RomProtect[9]
    // Range Unit   : RomProtect[8]
    // Range        : RomProtect[7:0]
    //
    // Note : The following code also could be used for 0x17 APU due to 4k alignment.
    //
    RomProtectReg.RomBase = (BiosLock[Index].Base & 0xFFFFF000) >> 12;

    DEBUG ((EFI_D_INFO, "Rom Protect %d: => Base:%8x  Length:%8x\n", Index,  BiosLock[Index].Base, BiosLock[Index].Length));

    if (BiosLock[Index].Length > (ROM_RANGE_UNIT_4K * 255)) {
      //
      // Length > 1M - 4K (4K x 255) use range unit=64K
      //
      RomProtectReg.RangeUnit = 1;
      RangeUnitValue          = ROM_RANGE_UNIT_64K;
    } else {
      //
      // Length <= 1M - 4K use range unit=4K
      //
      RomProtectReg.RangeUnit = 0;
      RangeUnitValue          = ROM_RANGE_UNIT_4K;
    }

    RomProtectReg.Range = (UINT32)(BiosLock[Index].Length / RangeUnitValue);
    if (BiosLock[Index].Length % RangeUnitValue) {
      RomProtectReg.Range++;
    }

    RomProtectReg.WriteProtect = 1;

    CopyMemI (&RomProtectReg32, &RomProtectReg, sizeof (UINT32));

    //
    // Write protect value to Rom Protect 0 ~ Rom Protect 3
    //
    PciWrite32 (PCI_LIB_ADDRESS (FCH_LPC_BUS, FCH_LPC_DEV, FCH_LPC_FUNC, FCH_LPC_REG50 + (Index * 4)), RomProtectReg32);
  }

  //
  // Restricted Cmd "01"(Write status register) & "C7"(Chip erase)
  //
  Data = (BIT0 + BIT8 + BIT9 + BIT10 + BIT14 + BIT15);
  SpiRestrictedCmd = Mmio16 (SpiRomBaseAddress, 0x04);
  SpiRestrictedCmd |= Data;
  Mmio16 (SpiRomBaseAddress, 0x04) =  SpiRestrictedCmd;
  
  //
  //  Set Alt_SPI_CS[5:3] SpiProtectEn0, SpiProtectEn1 and SpiProtectLock to 1 to protect Spi memory access
  //
  Data = (BIT3 + BIT4 + BIT5);
  DataMask = 0xFF;
  SpiProtectLock = Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D);
  SpiProtectLock |= Data;
  Mmio8 (SpiRomBaseAddress, FCH_SPI_MMIO_REG1D) = SpiProtectLock;

  //
  // Set SpiAccessMacRomEn and SpiHostAccessRomEn to 0 to enable SPI indirect access protect
  //
  SpiControl0Reg = Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00);
  SpiControl0Reg &= ~(BIT22 + BIT23);
  Mmio32 (SpiRomBaseAddress, FCH_SPI_MMIO_REG00) = SpiControl0Reg;

  IoWrite8 (FCH_IOMAP_REGCD4, SPI_LOCK_PROTECT_STATUS);
  SpiLockStatus = IoRead8 (FCH_IOMAP_REGCD5);
  SpiLockStatus |= (SPI_PROTECT_ENABLED);
  IoWrite8 (FCH_IOMAP_REGCD5, SpiLockStatus);
  
  return EFI_SUCCESS;
}
