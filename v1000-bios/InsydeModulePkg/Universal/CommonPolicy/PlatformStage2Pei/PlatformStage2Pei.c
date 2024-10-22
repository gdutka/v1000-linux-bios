/** @file
  Provide common initialization and common interfaces for platform.

;******************************************************************************
;* Copyright (c) 2012 - 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <KernelSetupConfig.h>
#include <Ppi/RomCorrupt.h>
#include <Ppi/MemoryDiscovered.h>
#include <Guid/H2OCp.h>
#include <Library/H2OCpLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/PeiChipsetSvcLib.h>
#include <Library/KernelConfigLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableLib.h>

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyPlatformStage2Callback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
ForceEnterRecoveryEntry (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

static EFI_PEI_NOTIFY_DESCRIPTOR      mNotifyPpiList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gPeiRomCorruptPpiGuid,
    ForceEnterRecoveryEntry
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredPpiNotifyPlatformStage2Callback
  }
};

/**
  Provide the functionality of the PlatformPeim platform services.

  @param  FileHandle   Handle of the file being invoked.
                       Type EFI_PEI_FILE_HANDLE is defined in FfsFindNextFile().
  @param  PeiServices  General purpose services available to every PEIM.

  @retval EFI_SUCCESS  If the interface could be successfully installed
  @retval Others       Returned from PeiServicesInstallPpi()
**/
EFI_STATUS
EFIAPI
PeiInitPlatformStage2EntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                        Status;
  UINT8                             *SystemConfiguration;
  BOOLEAN                           SetupVariableExist  ;
  UINTN                             SetupDataSize;
  EFI_STATUS                        OemSvcStatus;

  SetupDataSize = GetSetupVariableSize();

  SetupVariableExist = FALSE;

  SystemConfiguration = AllocateZeroPool (SetupDataSize);
  if (SystemConfiguration == NULL) {
    DEBUG ((EFI_D_INFO, "SystemConfiguration allocate memory resource failed.\n"));
    return EFI_OUT_OF_RESOURCES;
  }


  Status = CommonGetVariable (
             SETUP_VARIABLE_NAME,
             &gSystemConfigurationGuid,
             &SetupDataSize,
             SystemConfiguration
             );
  if (!EFI_ERROR (Status)) {
  	SetupVariableExist = TRUE;
  }

  PeiCsSvcSetSetupVariableDefault ((VOID *)SystemConfiguration);

  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcModifySetupVarPlatformStage2 \n"));
  Status = OemSvcModifySetupVarPlatformStage2 (
             (VOID *)SystemConfiguration,
             SetupVariableExist
             );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcModifySetupVarPlatformStage2 Status: %r\n", Status));

  PeiCsSvcPlatformStage2Init ();

  //
  // Notify NotifyPpiList
  //
  Status = PeiServicesNotifyPpi (&mNotifyPpiList[0]);
  ASSERT_EFI_ERROR (Status);
  //
  // OemServices
  //
  DEBUG ((DEBUG_INFO, "OemKernelServices Call: OemSvcInitPlatformStage2 \n"));
  OemSvcStatus = OemSvcInitPlatformStage2 (
    (VOID *)SystemConfiguration,
    SetupVariableExist
    );
  DEBUG ((DEBUG_INFO, "OemKernelServices OemSvcInitPlatformStage2 Status: %r\n", OemSvcStatus));

  if (FeaturePcdGet (PcdH2OPeiCpInitPlatformStage2Supported)) {
    H2O_PEI_CP_INIT_PLATFORM_STAGE_2_DATA                      CpInitPlatformStage2Data;

    CpInitPlatformStage2Data.Size   = sizeof (H2O_PEI_CP_INIT_PLATFORM_STAGE_2_DATA);
    CpInitPlatformStage2Data.Status = H2O_CP_TASK_NORMAL;

    DEBUG ((EFI_D_INFO, "Checkpoint Trigger: %g\n", &gH2OPeiCpInitPlatformStage2Guid));
    H2OCpTrigger (&gH2OPeiCpInitPlatformStage2Guid, &CpInitPlatformStage2Data);
    DEBUG ((EFI_D_INFO, "Checkpoint Result: %x\n", CpInitPlatformStage2Data.Status));
  }

  return EFI_SUCCESS;
}
