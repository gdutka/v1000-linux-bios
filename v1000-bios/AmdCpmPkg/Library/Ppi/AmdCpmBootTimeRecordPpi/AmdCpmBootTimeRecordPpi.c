/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>

EFI_GUID  gBootTimeRecordPpiGuid = AMD_BOOT_TIME_RECORD_PPI_GUID;
EFI_GUID_STRING (&gBootTimeRecordPpiGuid, "AmdCpmBootTimeRecordPpi", "AMD CPM Boot Time Record PPI");
EFI_GUID  gPeiLastS3RestorePointPpiGuid = AMD_S3_RESUME_BEFORE_WAK_PPI_GUID;
EFI_GUID_STRING (&gPeiLastS3RestorePointPpiGuid, "AmdCpmS3ResumeLastPointRecordPpi", "AMD CPM S3 Resume Lst Point PPI");
