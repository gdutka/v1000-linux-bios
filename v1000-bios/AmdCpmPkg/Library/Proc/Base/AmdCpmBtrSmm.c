/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */
#include <AmdCpmBase.h>

#ifdef AMD_BIOS_BOOT_TIME_RECORD

#include CPM_PROTOCOL_DEFINITION (AmdCpmBootTimeRecordProtocol)

EFI_STATUS
RecordTimeTicksSmm (
  IN EFI_BOOT_SERVICES          *mBS,
  IN UINT64                     desc
  )
{
  EFI_STATUS                            Status;
  AMD_BOOT_TIME_RECORD_PROTOCOL         *mBootTimeRecordProtocol;

  mBootTimeRecordProtocol = NULL;
  Status = mBS->LocateProtocol (&gBootTimeRecordProtocolGuid, NULL, (VOID**)&mBootTimeRecordProtocol);
  if (!EFI_ERROR (Status)) {
    mBootTimeRecordProtocol->AddRecord (desc);
  }
  return Status;
}

#endif
