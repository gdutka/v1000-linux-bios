/** @file
 This DXE driver is to initialize EFI_SMM_ACCESS2_PROTOCOL.

 The EFI_SMM_ACCESS2_PROTOCOL abstracts the location and characteristics
 of SMRAM. The principal functionality found in the memory controller 
 includes the following:
   Exposing the SMRAM to all non-SMM agents, or the "open" state.
   Shrouding the SMRAM to all but the SMM agents, or the "closed" state.
   Preserving the system integrity, or "locking" the SMRAM, such
   that the settings cannot be perturbed by either boot service or
   runtime agents.

***************************************************************************
* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include "SmmAccess2.h"
#include <Library/VariableLib.h>
#include <ChipsetSetupConfig.h>

//
// Declare Global Variable for every sub function uses.
//
static EFI_SMM_ACCESS2_PRIVATE          mSmmAccess2;

/**
  CpuOpenSMRAM - read/write A0000-BFFFF 

  @param         VOID                  None.

  @retval        VOID                  None.
**/
VOID
EFIAPI
DoSmmOpen (
  VOID
  )
{
  UINT64          Data64;

  //
  // Clear ASeg and TSeg Valid
  //
  Data64 = AsmReadMsr64 (0xC0010113); //SMM_MASKHLR
  Data64 &= (~BIT0); //SMMAValid
  Data64 &= (~BIT1); //SMMTValid
  AsmWriteMsr64 (0xC0010113, Data64);

  // 
  // Enable FixMtrr mode
  //
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) | (1 << 19)));

  //
  // Enable read/write from A0000
  //
  Data64 = AsmReadMsr64 (AMD_AP_MTRR_FIX16k_A0000);
  Data64 |= 0x1010101010101010;
  Data64 |= 0x0808080808080808;
  AsmWriteMsr64 (AMD_AP_MTRR_FIX16k_A0000, Data64);

  // 
  // Disable FixMtrr mode
  // 
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) & (~(1 << 19))));
}

/**
  Opens the SMRAM area to be accessible by a boot-service driver.

  This function "opens" SMRAM so that it is visible while not inside of SMM. The function should 
  return EFI_UNSUPPORTED if the hardware does not support hiding of SMRAM. The function 
  should return EFI_DEVICE_ERROR if the SMRAM configuration is locked.

  @param[in] This           The EFI_SMM_ACCESS2_PROTOCOL instance.

  @retval EFI_SUCCESS       The operation was successful.
  @retval EFI_UNSUPPORTED   The system does not support opening and closing of SMRAM.
  @retval EFI_DEVICE_ERROR  SMRAM cannot be opened, perhaps because it is locked.
**/
EFI_STATUS
EFIAPI
Open2 (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_ACCESS2_PRIVATE               *SmmAccess2;
  UINTN                                 Index;
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  BOOLEAN                               OpenState;

  SmmAccess2 = EFI_SMM_ACCESS2_PRIVATE_FROM_THIS (This);
  MpService  = SmmAccess2->MpService;

  if (SmmAccess2->SmmAccess2.LockState) {
    DEBUG ((EFI_D_WARN, "(Open) LockState is true!\n"));
    Status = EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < SmmAccess2->NumberRegions; Index++) {
    if (SmmAccess2->SmramDesc[Index].RegionState & EFI_SMRAM_LOCKED) {
      DEBUG ((DEBUG_WARN, "(Open) RegionState is locked!\n"));
      continue;
    }

    SmmAccess2->SmramDesc[Index].RegionState &= ~(EFI_SMRAM_CLOSED | EFI_ALLOCATED);
    SmmAccess2->SmramDesc[Index].RegionState |= EFI_SMRAM_OPEN;
  }

  //
  // Do Open for all APs
  //
  DoSmmOpen ();
  Status = MpService->StartupAllAPs (
                        MpService,
                        (EFI_AP_PROCEDURE) DoSmmOpen,
                        FALSE,
                        NULL,
                        0,
                        NULL,
                        NULL
                        );
  ASSERT_EFI_ERROR (Status);

  //
  // Find out if any regions are still open
  //
  OpenState = FALSE;
  for (Index = 0; Index < SmmAccess2->NumberRegions; Index++) {
    if ((SmmAccess2->SmramDesc[Index].RegionState & EFI_SMRAM_OPEN) == EFI_SMRAM_OPEN) {
      OpenState = TRUE;
      break;
    }
  }

  SmmAccess2->SmmAccess2.OpenState = OpenState;
  
  return EFI_SUCCESS;
}

/**
  CpuSmramWP - write protect from A0000-BFFFF 

  @param         VOID                  None.

  @retval        VOID                  None.
**/
VOID
EFIAPI
DoSmmClose (
  VOID
  )
{
  UINT64          Data64;

  // 
  // Enable FixMtrr mode
  //
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) | (1 << 19)));

  //
  // Disable read/write from A0000
  //
  Data64 = AsmReadMsr64 (AMD_AP_MTRR_FIX16k_A0000);
  Data64 &= 0xEFEFEFEFEFEFEFEF;
  Data64 &= 0xF7F7F7F7F7F7F7F7;
  AsmWriteMsr64 (AMD_AP_MTRR_FIX16k_A0000, Data64);

  // 
  // Disable FixMtrr mode
  // 
  AsmWriteMsr64 (SYS_CFG, (AsmReadMsr64 (SYS_CFG) & (~(1 << 19))));

  //
  // Now it is leave SMI, we need to set SMMAValid and SMMTValid bit in SMM_MASKHLR
  //
  Data64 = AsmReadMsr64 (0xC0010113); //SMM_MASKHLR
  Data64 |= BIT0; //SMMAValid
  Data64 |= BIT1; //SMMTValid
  AsmWriteMsr64 (0xC0010113, Data64);
}

/**
  Inhibits access to the SMRAM.

  This function "closes" SMRAM so that it is not visible while outside of SMM. The function should 
  return EFI_UNSUPPORTED if the hardware does not support hiding of SMRAM.

  @param[in] This           The EFI_SMM_ACCESS2_PROTOCOL instance.

  @retval EFI_SUCCESS       The operation was successful.
  @retval EFI_UNSUPPORTED   The system does not support opening and closing of SMRAM.
  @retval EFI_DEVICE_ERROR  SMRAM cannot be closed.
**/
EFI_STATUS
EFIAPI
Close2 (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_ACCESS2_PRIVATE               *SmmAccess2;
  UINTN                                 Index;
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  BOOLEAN                               OpenState;

  SmmAccess2 = EFI_SMM_ACCESS2_PRIVATE_FROM_THIS (This);
  MpService  = SmmAccess2->MpService;

  if (SmmAccess2->SmmAccess2.LockState) {
    DEBUG ((EFI_D_WARN, "(Open) LockState is true!\n"));
    Status = EFI_DEVICE_ERROR;
  }

  for (Index = 0; Index < SmmAccess2->NumberRegions; Index++) {
    if (SmmAccess2->SmramDesc[Index].RegionState & (EFI_SMRAM_LOCKED | EFI_SMRAM_CLOSED)) {
      DEBUG ((DEBUG_WARN, "(Close) RegionState is locked or closed!\n"));
      continue;
    }

    SmmAccess2->SmramDesc[Index].RegionState &= ~EFI_SMRAM_OPEN;
    SmmAccess2->SmramDesc[Index].RegionState |= (EFI_SMRAM_CLOSED | EFI_ALLOCATED);
  }

  //
  // Do Close for all APs
  //
  DoSmmClose ();
  Status = MpService->StartupAllAPs (
                        MpService,
                        (EFI_AP_PROCEDURE) DoSmmClose,
                        FALSE,
                        NULL,
                        0,
                        NULL,
                        NULL
                        );
  ASSERT_EFI_ERROR (Status);

  //
  // Find out if any regions are still open
  //
  OpenState = FALSE;
  for (Index = 0; Index < SmmAccess2->NumberRegions; Index++) {
    if ((SmmAccess2->SmramDesc[Index].RegionState & EFI_SMRAM_OPEN) == EFI_SMRAM_OPEN) {
      OpenState = TRUE;
      break;
    }
  }

  SmmAccess2->SmmAccess2.OpenState = OpenState;

  return EFI_SUCCESS;
}

/**
  Setting the bit0 of MSRC001_0015 Hardware Configuration (HWCR) to do SMM code lock.

  @param         VOID                  None.

  @retval        VOID                  None.
**/
VOID
EFIAPI
DoSmmLock (
  VOID
  )
{
  UINT64          Data64;

  Data64 = AsmReadMsr64 (HWCR);
  Data64 |= BIT0; //SMM_LOCK
  AsmWriteMsr64 (HWCR, Data64);
}

/**
  Inhibits access to the SMRAM.

  This function prohibits access to the SMRAM region.  This function is usually implemented such 
  that it is a write-once operation. 

  @param[in] This          The EFI_SMM_ACCESS2_PROTOCOL instance.

  @retval EFI_SUCCESS      The device was successfully locked.
  @retval EFI_UNSUPPORTED  The system does not support locking of SMRAM.
**/
EFI_STATUS
EFIAPI
Lock2 (
  IN EFI_SMM_ACCESS2_PROTOCOL  *This
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_ACCESS2_PRIVATE               *SmmAccess2;
  UINTN                                 Index;
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  CHIPSET_CONFIGURATION                 SetupVariable;
  UINTN                                 VariableSize;

  SmmAccess2 = EFI_SMM_ACCESS2_PRIVATE_FROM_THIS (This);
  MpService  = SmmAccess2->MpService;

  if (SmmAccess2->SmmAccess2.OpenState) {
    DEBUG ((EFI_D_WARN, "(Lock) OpenState is true!\n"));
    Status = EFI_DEVICE_ERROR;
  }
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  Status = CommonGetVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &VariableSize,
             &SetupVariable
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if(SetupVariable.SMMCoreLock == 1){ 
    for (Index = 0; Index < SmmAccess2->NumberRegions ;Index++) {
      SmmAccess2->SmramDesc[Index].RegionState |= EFI_SMRAM_LOCKED;
    }

    //
    // Do Lock for all APs
    //
    DoSmmLock ();
    Status = MpService->StartupAllAPs (
                          MpService,
                          (EFI_AP_PROCEDURE) DoSmmLock,
                          FALSE,
                          NULL,
                          0,
                          NULL,
                          NULL
                          );
    ASSERT_EFI_ERROR (Status);

    SmmAccess2->SmmAccess2.LockState = TRUE;
  }
  return EFI_SUCCESS;
}

/**
  Queries the memory controller for the possible regions that will support SMRAM.

  @param[in]     This           The EFI_SMM_ACCESS2_PROTOCOL instance.
  @param[in,out] SmramMapSize   A pointer to the size, in bytes, of the SmramMemoryMap buffer.
  @param[in,out] SmramMap       A pointer to the buffer in which firmware places the current memory map.

  @retval EFI_SUCCESS           The chipset supported the given resource.
  @retval EFI_BUFFER_TOO_SMALL  The SmramMap parameter was too small.  The current buffer size 
                                needed to hold the memory map is returned in SmramMapSize.
**/
EFI_STATUS
EFIAPI
GetCapabilities2 (
  IN CONST EFI_SMM_ACCESS2_PROTOCOL  *This,
  IN OUT UINTN                       *SmramMapSize,
  IN OUT EFI_SMRAM_DESCRIPTOR        *SmramMap
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_ACCESS2_PRIVATE               *SmmAccess2;
  UINTN                                 Size;

  SmmAccess2 = EFI_SMM_ACCESS2_PRIVATE_FROM_THIS (This);

  Size = SmmAccess2->NumberRegions * sizeof (EFI_SMRAM_DESCRIPTOR);

  if (*SmramMapSize >= Size) {
    CopyMem (SmramMap, SmmAccess2->SmramDesc, Size);
    Status = EFI_SUCCESS;
  } else {
    DEBUG ((EFI_D_WARN, "SmmCapabilities: SmramMapSize is too small !\n"));
    Status = EFI_BUFFER_TOO_SMALL;
  }

  *SmramMapSize = Size;

  return Status;
}

/**
  Entry point of this driver

  @param[in]     ImageHandle           Handle of driver image
  @param[in]     SystemTable           Point to EFI_SYSTEM_TABLE

  @retval        EFI_OUT_OF_RESOURCES  Can not allocate memory resource.
  @retval        EFI_NOT_FOUND         GetNextGuidHob Failure or 
                                       HobLibConstructor: EfiGetSystemConfigurationTable Failure.
  @retval        EFI_DEVICE_ERROR      Can not install the protocol instance.
  @retval        EFI_SUCCESS           Success to initialize.
**/
EFI_STATUS
EFIAPI
InitializeSmmAccess2 (
  IN       EFI_HANDLE                   ImageHandle,
  IN       EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  EFI_HOB_GUID_TYPE                     *GuidHob;
  EFI_SMRAM_HOB_DESCRIPTOR_BLOCK        *DescriptorBlock;

  //
  // PostCode = 0x44, Setup SMM ACCESS2 service  
  //
  POST_CODE (DXE_SMMACCESS);

  //
  // Get MpService at first
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  (VOID **) &MpService
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Initialize Global variables
  //
  ZeroMem (&mSmmAccess2, sizeof (EFI_SMM_ACCESS2_PRIVATE));

  mSmmAccess2.Signature = EFI_SMM_ACCESS2_PRIVATE_SIGNATURE;
  mSmmAccess2.Handle    = NULL;
  mSmmAccess2.MpService = MpService;

  //
  // Get Smram region information definded in PEI phase.
  //
  GuidHob = GetFirstGuidHob (&gEfiSmmPeiSmramMemoryReserveGuid);
  if (GuidHob == NULL) {
    DEBUG ((DEBUG_ERROR, "SmramMemoryReserve HOB not found\n"));
    return EFI_NOT_FOUND;
  }

  DescriptorBlock = (EFI_SMRAM_HOB_DESCRIPTOR_BLOCK *)GET_GUID_HOB_DATA (GuidHob);
  if (DescriptorBlock == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Initialize Smram describing
  //
  mSmmAccess2.SmramDesc = AllocateZeroPool ((DescriptorBlock->NumberOfSmmReservedRegions) * sizeof (EFI_SMRAM_DESCRIPTOR));
  if (mSmmAccess2.SmramDesc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < DescriptorBlock->NumberOfSmmReservedRegions; Index++) {
    mSmmAccess2.SmramDesc[Index].PhysicalStart = DescriptorBlock->Descriptor[Index].PhysicalStart;
    mSmmAccess2.SmramDesc[Index].CpuStart      = DescriptorBlock->Descriptor[Index].CpuStart;
    mSmmAccess2.SmramDesc[Index].PhysicalSize  = DescriptorBlock->Descriptor[Index].PhysicalSize;
    mSmmAccess2.SmramDesc[Index].RegionState   = DescriptorBlock->Descriptor[Index].RegionState;
  }

  mSmmAccess2.NumberRegions               = Index;
  mSmmAccess2.SmmAccess2.Open             = Open2;
  mSmmAccess2.SmmAccess2.Close            = Close2;
  mSmmAccess2.SmmAccess2.Lock             = Lock2;
  mSmmAccess2.SmmAccess2.GetCapabilities  = GetCapabilities2;
  mSmmAccess2.SmmAccess2.LockState        = FALSE; // ToDo check Lockstate
  mSmmAccess2.SmmAccess2.OpenState        = FALSE; // ToDo check OpenState

  //
  // Install protocol interfaces
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSmmAccess2.Handle,
                  &gEfiSmmAccess2ProtocolGuid,
                  &mSmmAccess2.SmmAccess2,
                  NULL
                  );

  return Status;
}
