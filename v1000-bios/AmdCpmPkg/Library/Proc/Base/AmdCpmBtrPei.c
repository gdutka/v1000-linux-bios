/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <AmdCpmPei.h>

#ifdef AMD_BIOS_BOOT_TIME_RECORD

#include CPM_PPI_DEFINITION (AmdCpmBootTimeRecordPpi)

EFI_STATUS
RecordTimeTicks (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN       UINT64               desc
  )
{
  EFI_STATUS Status;
  AMD_BOOT_TIME_RECORD_PPI *BootTimeRecordPpi;
  Status = (**PeiServices).LocatePpi ((CPM_PEI_SERVICES**)PeiServices, &gBootTimeRecordPpiGuid,
                                        0, NULL, (VOID**)&BootTimeRecordPpi);
  if (!EFI_ERROR (Status)) {
    BootTimeRecordPpi->AddRecord (PeiServices, BootTimeRecordPpi->Record, desc);
  }
  return Status;
}

#ifdef  IDS_PERFORMANCE_ENABLE
EFI_STATUS
RecordTimeTicksAGESAS3Resume (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN       VOID                 *InitResume
  )
{
  EFI_STATUS Status;
  AMD_BOOT_TIME_RECORD_PPI *BootTimeRecordPpi;
  Status = (**PeiServices).LocatePpi ((CPM_PEI_SERVICES**)PeiServices, &gBootTimeRecordPpiGuid, 0, NULL, (VOID**)&BootTimeRecordPpi);
  if (!EFI_ERROR (Status)) {
    BootTimeRecordPpi->CopyIdsPerformanceDataToIdsRecord (PeiServices, BootTimeRecordPpi->IdsS3Record, InitResume);
  }
  return Status;
}
#endif

EFI_STATUS
RecordTimeTicksS3Resume (
  IN       EFI_PEI_SERVICES     **PeiServices,
  IN       UINT64               desc
  )
{
  EFI_STATUS Status;
  AMD_BOOT_TIME_RECORD_PPI *BootTimeRecordPpi;

  Status = (**PeiServices).LocatePpi ((CPM_PEI_SERVICES**)PeiServices, &gPeiLastS3RestorePointPpiGuid, 0, NULL, NULL);

  if (!EFI_ERROR (Status)) {
    Status = (**PeiServices).LocatePpi ((CPM_PEI_SERVICES**)PeiServices, &gBootTimeRecordPpiGuid, 0, NULL, (VOID**)&BootTimeRecordPpi);
    if (!EFI_ERROR (Status)) {
      BootTimeRecordPpi->AddRecord (PeiServices, BootTimeRecordPpi->S3Record, desc);
    }
  }
  return Status;
}

#endif
