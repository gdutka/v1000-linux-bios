/** @file
  Dummy implementation of Legacy Region 2 Protocol.

  This generic implementation of the Legacy Region 2 Protocol does not actually 
  perform any lock/unlock operations.  This module may be used on platforms 
  that do not provide HW locking of the legacy memory regions.  It can also 
  be used as a template driver for implementing the Legacy Region 2 Protocol on
  a platform that does support HW locking of the legacy memory regions.

Copyright (c) 2009 - 2014, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <LegacyRegion2.h>

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

EFI_HANDLE mLegacyRegion2Handle = NULL;

EFI_LEGACY_REGION2_PROTOCOL mLegacyRegion2Interface = {
  LegacyRegion2Decode,
  LegacyRegion2Lock,
  LegacyRegion2BootLock,
  LegacyRegion2Unlock,
  LegacyRegionGetInfo
};

FIXED_MTRR_SETTING  mLockCtrl[8] = {
  {0x00000268, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
  {0x00000269, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
  {0x0000026A, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
  {0x0000026B, ACCESS_TYPE_RD_DRAM | ACCESS_TYPE_WR_MMIO},
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

FIXED_MTRR    mFixedMtrrTable[] = {
  {
    MSR_IA32_MTRR_FIX64K_00000,
    0,
    0x10000
  },
  {
    MSR_IA32_MTRR_FIX16K_80000,
    0x80000,
    0x4000
  },
  {
    MSR_IA32_MTRR_FIX16K_A0000,
    0xA0000,
    0x4000
  },
  {
    MSR_IA32_MTRR_FIX4K_C0000,
    0xC0000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_C8000,
    0xC8000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_D0000,
    0xD0000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_D8000,
    0xD8000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_E0000,
    0xE0000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_E8000,
    0xE8000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_F0000,
    0xF0000,
    0x1000
  },
  {
    MSR_IA32_MTRR_FIX4K_F8000,
    0xF8000,
    0x1000
  },
};

EFI_STATUS
EFIAPI
CpuConfigLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

VOID
EFIAPI
ReadyToLockEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                        Status;
  VOID                              *Interface;
  UINTN                             ProcessorNumber;
  MTRR_SETTINGS                     MtrrSettings;
  UINT32                            Index;

  Status = gBS->LocateProtocol (&gEfiDxeSmmReadyToLockProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  
  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  
  // Update mCpuConfigLibConfigConextBuffer ptr again.
  CpuConfigLibConstructor (NULL, NULL);
  
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) | (1 << 19)));
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigLibConfigConextBuffer->NumberOfProcessors; ProcessorNumber++) {
    WritePreSmmInitRegisterTable (ProcessorNumber, Msr, SYS_CFG, 19, 1, 1);
  }

  MtrrGetAllMtrrs (&MtrrSettings);

  for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
    for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigLibConfigConextBuffer->NumberOfProcessors; ProcessorNumber++) {
      WritePreSmmInitRegisterTable (ProcessorNumber, Msr, mFixedMtrrTable[Index].Msr, 0, 64, MtrrSettings.Fixed.Mtrr[Index]);
    }
  }

  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) & (~(1 << 19))));
  for (ProcessorNumber = 0; ProcessorNumber < mCpuConfigLibConfigConextBuffer->NumberOfProcessors; ProcessorNumber++) {
    WritePreSmmInitRegisterTable (ProcessorNumber, Msr, SYS_CFG, 19, 1, 0);
  }

  gBS->CloseEvent (Event);
}
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
/*++

Routine Description:
  Modify PAM registers for region specified to MODE state.

Arguments:

Returns: 

  EFI_SUCCESS - Legacy BIOS protocol installed

--*/
{
  INT32                             Index;
  UINT32                            TempAddr;
  UINT64                            MsrValue;

  // 
  // Calcuate Granularity
  // 
  if (Granularity != NULL) {
    *Granularity = PAM_GRANULARITY;
  }

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
  The user Entry Point for module LegacyRegionDxe.  The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Install (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   ReadyToLockEvent;
  VOID        *Registration;
  
  //
  // Make sure the Legacy Region 2 Protocol is not already installed in the system
  //
  ASSERT_PROTOCOL_ALREADY_INSTALLED (NULL, &gEfiLegacyRegion2ProtocolGuid);

  //
  // Install the protocol on a new handle.
  //
  // TODO - don't know why mLegacyRegion2Handle = NULL and then a new handle will be allocated.
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mLegacyRegion2Handle,
                  &gEfiLegacyRegion2ProtocolGuid, &mLegacyRegion2Interface,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);


  ReadyToLockEvent = EfiCreateProtocolNotifyEvent (
                       &gEfiDxeSmmReadyToLockProtocolGuid,
                       TPL_CALLBACK,
                       ReadyToLockEventNotify,
                       NULL,
                       &Registration
                       );
  ASSERT (ReadyToLockEvent != NULL);

  return EFI_SUCCESS;
}

/**
 Unloads an image.

 @param[in]         ImageHandle           Handle that identifies the image to be unloaded.
                    
 @retval            EFI_SUCCESS           The image has been unloaded.
 @retval            EFI_INVALID_PARAMETER One of the protocol interfaces was not previously installed on ImageHandle.
**/
EFI_STATUS 
EFIAPI
LegacyRegion2Unload (
  IN EFI_HANDLE         ImageHandle
  )
{
  EFI_STATUS  Status;
	
  //
  // Uninstall Driver Supported EFI Version Protocol from the image handle.
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  mLegacyRegion2Handle,
                  &gEfiLegacyRegion2ProtocolGuid, &mLegacyRegion2Interface,
                  NULL
                  );
	
  return Status;
}


/**
  Modify the hardware to allow (decode) or disallow (not decode) memory reads in a region.

  If the On parameter evaluates to TRUE, this function enables memory reads in the address range 
  Start to (Start + Length - 1).
  If the On parameter evaluates to FALSE, this function disables memory reads in the address range 
  Start to (Start + Length - 1).

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose attributes
                                should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address
                                was not aligned to a region's starting address or if the length
                                was greater than the number of bytes in the first region.
  @param  On[in]                Decode / Non-Decode flag.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Decode (
  IN  EFI_LEGACY_REGION2_PROTOCOL  *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity,
  IN  BOOLEAN                      *On
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  if (*On) {
    return LegacyRegionManipulation (Start, Length, mDecodeOnCtrl, Granularity);
  } else {
    return LegacyRegionManipulation (Start, Length, mDecodeOffCtrl, Granularity);
  }
}

/**
  Modify the hardware to disallow memory writes in a region.

  This function changes the attributes of a memory range to not allow writes.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Lock (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return LegacyRegionManipulation (Start, Length, mLockCtrl, Granularity);
}

/**
  Modify the hardware to disallow memory attribute changes in a region.

  This function makes the attributes of a region read only. Once a region is boot-locked with this 
  function, the read and write attributes of that region cannot be changed until a power cycle has
  reset the boot-lock attribute. Calls to Decode(), Lock() and Unlock() will have no effect.

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.
  @retval EFI_UNSUPPORTED       The chipset does not support locking the configuration registers in
                                a way that will not affect memory regions outside the legacy memory
                                region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2BootLock (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return LegacyRegionManipulation (Start, Length, mLockCtrl, Granularity);
}

/**
  Modify the hardware to allow memory writes in a region.

  This function changes the attributes of a memory range to allow writes.  

  @param  This[in]              Indicates the EFI_LEGACY_REGION_PROTOCOL instance.
  @param  Start[in]             The beginning of the physical address of the region whose
                                attributes should be modified.
  @param  Length[in]            The number of bytes of memory whose attributes should be modified.
                                The actual number of bytes modified may be greater than the number
                                specified.
  @param  Granularity[out]      The number of bytes in the last region affected. This may be less
                                than the total number of bytes affected if the starting address was
                                not aligned to a region's starting address or if the length was
                                greater than the number of bytes in the first region.

  @retval EFI_SUCCESS           The region's attributes were successfully modified.
  @retval EFI_INVALID_PARAMETER If Start or Length describe an address not in the Legacy Region.

**/
EFI_STATUS
EFIAPI
LegacyRegion2Unlock (
  IN  EFI_LEGACY_REGION2_PROTOCOL *This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  OUT UINT32                      *Granularity
  )
{
  if ((Start < 0xC0000) || ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }

  return LegacyRegionManipulation (Start, Length, mUnlockCtrl, Granularity);
}

/**
  Get region information for the attributes of the Legacy Region.

  This function is used to discover the granularity of the attributes for the memory in the legacy 
  region. Each attribute may have a different granularity and the granularity may not be the same
  for all memory ranges in the legacy region.  

  @param  This[in]              Indicates the EFI_LEGACY_REGION2_PROTOCOL instance.
  @param  DescriptorCount[out]  The number of region descriptor entries returned in the Descriptor
                                buffer.
  @param  Descriptor[out]       A pointer to a pointer used to return a buffer where the legacy
                                region information is deposited. This buffer will contain a list of
                                DescriptorCount number of region descriptors.  This function will
                                provide the memory for the buffer.

  @retval EFI_SUCCESS           The information structure was returned.
  @retval EFI_UNSUPPORTED       This function is not supported.

**/
EFI_STATUS
EFIAPI
LegacyRegionGetInfo (
  IN  EFI_LEGACY_REGION2_PROTOCOL   *This,
  OUT UINT32                        *DescriptorCount,
  OUT EFI_LEGACY_REGION_DESCRIPTOR  **Descriptor
  )
{
  return EFI_UNSUPPORTED;
}

