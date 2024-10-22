/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */


#include <Base.h>
#include <Uefi.h>

#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>

#include <Guid/MdeModuleHii.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>

#include <Library/AmdCbsSetupLib.h>
#include <Protocol/AmdCbsHookProtocol.h>
#include "AmdCbsVariable.h"
#include "AmdCbsFormID.h"
#include "AmdSoc.h"

#include "Porting.h"
#include "Addendum/Apcb/Inc/RV/APCB.h"
#include "ApcbCommon.h"
#include "Protocol/AmdApcbProtocol.h"
#include "Library/AmdPspBaseLibV2.h"
#include "Library/CbsBctSmmLib.h"

extern  EFI_GUID gAmdApcbSmmServiceProtocolGuid;

VOID
PrepareCmnTypeAttrib (
  UINT32 *Size,
  APCB_PARAM_ATTRIBUTE *ApcbParaAttrib,
  VOID *CbsVariable
  );

VOID
PrepareCmnTypeValue (
  UINT32 *Size,
  UINT8 *ApcbParaValue,
  VOID *CbsVariable
  );

EFI_STATUS
EFIAPI
CbsBctSetVariable (
  IN CBS_BCT_SMM_PROTOCOL    *This,
  IN UINT8                   *IfrData
)
{
  EFI_STATUS                      Status;
  BOOLEAN                         ApcbEntryExist;
  TYPE_ATTRIB                     TypeAttrib;
  UINT64                          OldApcbPtr;
  UINT32                          EntrySize;
  UINT64                          EntryDest;
  UINT32                          AttributeSize;
  UINT32                          ValueSize;
  UINT32                          TotalSizeExt;
  UINT8                           *DataStream;
  UINT8                           *ValueStream;
  UINT8                           *NewApcbPtr;
  AMD_APCB_SERVICE_PROTOCOL       *mApcbSmmServiceProtocol;

  NewApcbPtr = NULL;
  Status = EFI_SUCCESS;
  // Check the existence and size of APCB  entry 0x60
  ApcbEntryExist = BIOSEntryInfo (BIOS_APCB_INFO, INSTANCE_IGNORED, &TypeAttrib, &OldApcbPtr, &EntrySize, &EntryDest);
  if (ApcbEntryExist == FALSE) {
    DEBUG ((EFI_D_ERROR, "BCT Error: APCB Entry doesn't exist\n"));
    return EFI_UNSUPPORTED;
  }

  // Calculate the buffer needed to store the external byte stream
  AttributeSize = 0;
  ValueSize = 0;
  DataStream = NULL;
  PrepareCmnTypeAttrib (&AttributeSize, (APCB_PARAM_ATTRIBUTE *)DataStream, (CBS_CONFIG *)IfrData);
  PrepareCmnTypeValue (&ValueSize, DataStream, (CBS_CONFIG *)IfrData);
  TotalSizeExt = AttributeSize + ValueSize;
  TotalSizeExt += ((ALIGN_SIZE_OF_TYPE - (TotalSizeExt % ALIGN_SIZE_OF_TYPE)) & (ALIGN_SIZE_OF_TYPE - 1));

  if (0 != TotalSizeExt) {
    DEBUG ((EFI_D_ERROR, "Replace CBS Common Options\n"));
    // Allocate buffer for new Type data stream
    DataStream = NULL;
    DataStream = AllocateZeroPool (TotalSizeExt);
    if (DataStream == NULL) {
      DEBUG ((EFI_D_ERROR, "BCT Error: fail to allocate buffer for new type data stream\n"));
      return EFI_UNSUPPORTED;
    }

    // Create new Type data stream
    PrepareCmnTypeAttrib (&AttributeSize, (APCB_PARAM_ATTRIBUTE *)DataStream, (CBS_CONFIG *)IfrData);
    ValueStream = &DataStream[AttributeSize];
    PrepareCmnTypeValue (&ValueSize, ValueStream, (CBS_CONFIG *)IfrData);

    Status = gSmst->SmmLocateProtocol (
                &gAmdApcbSmmServiceProtocolGuid,
                NULL,
                (VOID**)&mApcbSmmServiceProtocol
                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mApcbSmmServiceProtocol->ApcbUpdateCbsData (mApcbSmmServiceProtocol, DataStream, TotalSizeExt, FALSE);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    // Free temporary buffer
    Status = gSmst->SmmFreePool (DataStream);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  return Status;
}

EFI_STATUS
EFIAPI
CbsBctWriteFlash (
)
{
  EFI_STATUS                      Status;
  AMD_APCB_SERVICE_PROTOCOL       *mApcbSmmServiceProtocol;

  Status = gSmst->SmmLocateProtocol (
              &gAmdApcbSmmServiceProtocolGuid,
              NULL,
              (VOID**)&mApcbSmmServiceProtocol
              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mApcbSmmServiceProtocol->ApcbFlushData (mApcbSmmServiceProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
