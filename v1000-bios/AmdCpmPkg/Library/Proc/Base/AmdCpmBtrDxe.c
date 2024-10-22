/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <AmdCpmDxe.h>

#ifdef AMD_BIOS_BOOT_TIME_RECORD

#include CPM_PROTOCOL_DEFINITION (AmdCpmBootTimeRecordProtocol)

CPM_EXTERN_gBS;

EFI_STATUS
RecordTimeTicksDxe (
  IN UINT64                     desc
  )
{
  EFI_STATUS                            Status;
  AMD_BOOT_TIME_RECORD_PROTOCOL         *mBootTimeRecordProtocol;

  mBootTimeRecordProtocol = NULL;
  Status = gBS->LocateProtocol (&gBootTimeRecordProtocolGuid, NULL, (VOID**)&mBootTimeRecordProtocol);
  if (!EFI_ERROR (Status)) {
    mBootTimeRecordProtocol->AddRecord (desc);
  }
  return Status;
}

#ifdef IDS_PERFORMANCE_ENABLE
EFI_STATUS
RecordTimeTicksAGESA (
  IN OUT   VOID *InitLate
  )
{
  EFI_STATUS                            Status;
  AMD_BOOT_TIME_RECORD_PROTOCOL         *mBootTimeRecordProtocol;

  mBootTimeRecordProtocol = NULL;
  Status = gBS->LocateProtocol (&gBootTimeRecordProtocolGuid, NULL, (VOID**)&mBootTimeRecordProtocol);
  if (!EFI_ERROR (Status)) {
    mBootTimeRecordProtocol->CopyIdsPerformanceDataToIdsRecord (InitLate);
  }
  return EFI_SUCCESS;
}
#endif

#endif
