/** @file
  Produces the Legacy Region Protocol.

  This generic implementation of the Legacy Region Protocol does not actually 
  perform any lock/unlock operations.  This module may be used on platforms 
  that do not provide HW locking of the legacy memory regions.  It can also 
  be used as a template driver for implementing the Legacy Region Protocol on
  a platform that does support HW locking of the legacy memory regions.

Copyright (c) 2009, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <PiDxe.h>
#include <Protocol/LegacyRegion.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>

#define PAM_C                         0xC0000
#define PAM_GRANULARITY               0x8000

#define SYS_CFG                       0xC0010010

#define ACCESS_TYPE_RD_DRAM           1
#define ACCESS_TYPE_RD_MMIO           2
#define ACCESS_TYPE_WR_DRAM           4
#define ACCESS_TYPE_WR_MMIO           8

typedef struct {
  UINT32  Msr;
  UINT8   AccessType;
} FIXED_MTRR_SETTING;

FIXED_MTRR_SETTING  mLockCtrl[8] = {
  {0x00000268, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
  {0x00000269, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
  {0x0000026A, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026B, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026C, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026D, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026E, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
  {0x0000026F, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
};

FIXED_MTRR_SETTING  mUnlockCtrl[8] = {
  {0x00000268, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x00000269, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026A, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026B, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026C, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026D, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026E, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
  {0x0000026F, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_DRAM},
};

FIXED_MTRR_SETTING  mDecodeOnCtrl[8] = {
  {0x00000268, ACCESS_TYPE_RD_DRAM},
  {0x00000269, ACCESS_TYPE_RD_DRAM},
  {0x0000026A, ACCESS_TYPE_RD_DRAM},
  {0x0000026B, ACCESS_TYPE_RD_DRAM},
  {0x0000026C, ACCESS_TYPE_RD_DRAM},
  {0x0000026D, ACCESS_TYPE_RD_DRAM},
  {0x0000026E, ACCESS_TYPE_RD_DRAM},
  {0x0000026F, ACCESS_TYPE_RD_DRAM},
};

FIXED_MTRR_SETTING  mDecodeOffCtrl[8] = {
  {0x00000268, ACCESS_TYPE_RD_MMIO},
  {0x00000269, ACCESS_TYPE_RD_MMIO},
  {0x0000026A, ACCESS_TYPE_RD_MMIO},
  {0x0000026B, ACCESS_TYPE_RD_MMIO},
  {0x0000026C, ACCESS_TYPE_RD_MMIO},
  {0x0000026D, ACCESS_TYPE_RD_MMIO},
  {0x0000026E, ACCESS_TYPE_RD_MMIO},
  {0x0000026F, ACCESS_TYPE_RD_MMIO},
};

/**
 Modify MSR for region specified to MODE stat.

 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         CtrlData            
 @param[in]         Granularity         

 @retval            None
*/
STATIC
EFI_STATUS
LegacyRegionManipulation (
  IN  UINT32                        Start,
  IN  UINT32                        Length,
  IN  FIXED_MTRR_SETTING            *CtrlData,
  OUT UINT32                        *Granularity
  )
{
  INT32                             Index;
  UINT32                            TempAddr;
  UINT64                            MsrValue;

  // 
  // Calcuate Granularity
  // 
  if (Granularity == NULL) return EFI_INVALID_PARAMETER;
  *Granularity = PAM_GRANULARITY;

  for (TempAddr = (Start / PAM_GRANULARITY) * PAM_GRANULARITY, Index = (TempAddr - PAM_C) / PAM_GRANULARITY;
       TempAddr <= (Start + Length);
       TempAddr += PAM_GRANULARITY, Index++) {

    if (Index >= 8) break;

    // 
    // Enable FixMtrr mode
    // 
    AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) | (1 << 19)));

    // 
    // Configure r/w attribute of fixed-size MTRRs below 1MB refer to mLockCtrl and mUnlockCtrl.
    // MSR0000_0[268:269] = C segment
    // MSR0000_0[26A:26B] = D segment
    // MSR0000_0[26C:26D] = E segment
    // MSR0000_0[26E:26F] = F segment
    // 
    MsrValue = AsmReadMsr64 (CtrlData[Index].Msr);
    
    if (CtrlData[Index].AccessType & ACCESS_TYPE_RD_DRAM) {
      MsrValue |= 0x1010101010101010;
    } else if (CtrlData[Index].AccessType & ACCESS_TYPE_RD_MMIO) {
      MsrValue &= 0xEFEFEFEFEFEFEFEF;
    }

    if (CtrlData[Index].AccessType & ACCESS_TYPE_WR_DRAM) {
      MsrValue |= 0x0808080808080808;
    } else if (CtrlData[Index].AccessType & ACCESS_TYPE_WR_MMIO) {
      MsrValue &= 0xF7F7F7F7F7F7F7F7;
    }

    AsmWriteMsr64 (CtrlData[Index].Msr, MsrValue);

    // 
    // Disable FixMtrr mode
    // 
    AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) & (~(1 << 19))));
  }

  return EFI_SUCCESS;
}

/**
  Sets hardware to decode or not decode a region.

  @param  This                  Indicates the EFI_LEGACY_REGION_PROTOCOL instance
  @param  Start                 Start of region to decode.
  @param  Length                Size in bytes of the region.
  @param  On                    Decode/nondecode flag.

  @retval EFI_SUCCESS           Decode range successfully changed.

**/
EFI_STATUS
EFIAPI
LegacyRegionDecode (
  IN EFI_LEGACY_REGION_PROTOCOL  *This,
  IN UINT32                      Start,
  IN UINT32                      Length,
  IN BOOLEAN                     *On
  )
{
  return EFI_SUCCESS;
}

/**
  Sets a region to read only.

  @param  This                  Indicates the EFI_LEGACY_REGION_PROTOCOL instance
  @param  Start                 Start of region to lock.
  @param  Length                Size in bytes of the region.
  @param  Granularity           Lock attribute affects this granularity in bytes.

  @retval EFI_SUCCESS           The region was made read only.

**/
EFI_STATUS
EFIAPI
LegacyRegionLock (
  IN  EFI_LEGACY_REGION_PROTOCOL  *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity OPTIONAL
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return LegacyRegionManipulation (Start, Length, mLockCtrl, Granularity);
}

/**
  Sets a region to read only and ensures that flash is locked from being
  inadvertently modified.

  @param  This                  Indicates the EFI_LEGACY_REGION_PROTOCOL instance
  @param  Start                 Start of region to lock.
  @param  Length                Size in bytes of the region.
  @param  Granularity           Lock attribute affects this granularity in bytes.

  @retval EFI_SUCCESS           The region was made read only and flash is locked.

**/
EFI_STATUS
EFIAPI
LegacyRegionBootLock (
  IN  EFI_LEGACY_REGION_PROTOCOL  *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity OPTIONAL
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return LegacyRegionManipulation (Start, Length, mLockCtrl, Granularity);
}

/**
  Sets a region to read-write.

  @param  This                  Indicates the EFI_LEGACY_REGION_PROTOCOL instance
  @param  Start                 Start of region to lock.
  @param  Length                Size in bytes of the region.
  @param  Granularity           Lock attribute affects this granularity in bytes.

  @retval EFI_SUCCESS           The region was successfully made read-write.

**/
EFI_STATUS
EFIAPI
LegacyRegionUnlock (
  IN  EFI_LEGACY_REGION_PROTOCOL  *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity OPTIONAL
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return LegacyRegionManipulation (Start, Length, mUnlockCtrl, Granularity);
}

//
// Module global for the handle the Legacy Region Protocol is installed
//
EFI_HANDLE                  mLegacyRegionHandle = NULL;

//
// Module global for the Legacy Region Protocol instance that is installed onto
// mLegacyRegionHandle
//
EFI_LEGACY_REGION_PROTOCOL  mLegacyRegion = {
  LegacyRegionDecode,
  LegacyRegionLock,
  LegacyRegionBootLock,
  LegacyRegionUnlock
};

/**
  The user Entry Point for module LegacyRegionDxe.  The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
LegacyRegionInstall (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  
  //
  // Make sure the Legacy Region Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiLegacyRegionProtocolGuid);
  
  //
  // Install the protocol on a new handle.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mLegacyRegionHandle,
                  &gEfiLegacyRegionProtocolGuid, &mLegacyRegion,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}