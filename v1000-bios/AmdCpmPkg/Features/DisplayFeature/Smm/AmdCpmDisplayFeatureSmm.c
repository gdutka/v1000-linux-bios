/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmSmm.h>

CPM_DEFINE_gBS;
CPM_DEFINE_gSmst;

AMD_CPM_TABLE_PROTOCOL                  *gCpmTableSmmProtocol;
AMD_CPM_DISPLAY_FEATURE_PROTOCOL        *gCpmDisplayFeatureSmmProtocol;
AMD_CPM_TABLE_PROTOCOL                  *gCpmTableSmmProtocol;

EFI_STATUS
AmdCpmDisplayFeatureSmmRegistrationCenter (
  VOID
  );


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Display Feature SMM driver
 *
 * This function registers the SMI handler to disable audio in dGPU when PowerXpress
 * is enabled and set SSID when resume from S3.
 *
 * @param[in]     ImageHandle    Pointer to the firmware file system header
 * @param[in]     SystemTable    Pointer to System table
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmDisplayFeatureSmmEntryPoint (
  IN        EFI_HANDLE                  ImageHandle,
  IN        EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS                            Status;

  If_CpmInSmm (ImageHandle, SystemTable, gBS, gSmst, Status) {

    //
    // Initialize global variables
    //
    Status = gSmst->SmmLocateProtocol (
                    &gAmdCpmTableSmmProtocolGuid,
                    NULL,
                    (VOID**)&gCpmTableSmmProtocol
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->LocateProtocol (
                    &gAmdCpmDisplayFeatureProtocolGuid,
                    NULL,
                    (VOID**)&gCpmDisplayFeatureSmmProtocol
                    );
    if (EFI_ERROR (Status)) {
      return EFI_ABORTED;
    }

    gCpmTableSmmProtocol->CommonFunction.PostCode (CpmTpDisplayFeatureSmmDriverBegin);

    gCpmDisplayFeatureSmmProtocol->TableProtocolSmmPtr = gCpmTableSmmProtocol;

    Status = AmdCpmDisplayFeatureSmmRegistrationCenter ();

    gCpmTableSmmProtocol->CommonFunction.PostCode (CpmTpDisplayFeatureSmmDriverEnd);
  }

  RECORD_TIME_DXE (EndAmdCpmDisplayFeatureSmmDriver);

  return Status;
}
