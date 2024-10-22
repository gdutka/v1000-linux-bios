
/* $NoKeywords:$ */
/**
 * @file
 *
 * PspP2Cmbox Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspP2Cmbox Driver
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "AmdPspP2CmboxV2.h"
#include <AmdPspSmmCommunication.h>
#include <Library/AmdPspFlashAccLib.h>
#include <Library/SmmMemLib.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPP2CMBOXV2_AMDPSPFLASHACCSMMHDLR_FILECODE

extern UINTN                                 mBlockSize;
extern PSP_NV_DESCRIPTOR                     mPspNvDb[];
extern EFI_HANDLE                            mPspSmmCommHandle;
extern ENTRY_REGION                          mWritableEntryRegions[MAX_WRITABLE_ENTRY_REGIONS];
extern UINT32                                mEntryRegionIndex;

BOOLEAN
IsWritableRegion (
  IN       UINTN                   AddressToWrite,
  IN OUT   UINTN                   NumBytes
  )
{
  UINTN                        i;
  UINTN                        FlashAddressEnd;
  UINTN                        EntryAddressEnd;

  // Overflow check
  FlashAddressEnd = AddressToWrite + NumBytes - 1;
  if (FlashAddressEnd <= AddressToWrite) {
    return FALSE;
  }

  // check writable
  for (i = 0; i < mEntryRegionIndex; i++) {
    EntryAddressEnd = mWritableEntryRegions[i].Address + mWritableEntryRegions[i].Size - 1;
    if ((EntryAddressEnd > mWritableEntryRegions[i].Address) &&
        (AddressToWrite >= mWritableEntryRegions[i].Address) &&
        (FlashAddressEnd <= EntryAddressEnd)) {
      return TRUE;
    }
  }
  return FALSE;
}

EFI_STATUS
PspFlashAccSmmCommunicateHandler (
  IN       EFI_HANDLE  DispatchHandle,
  IN       CONST VOID  *Context,
  IN OUT   VOID    *CommBuffer,
  IN OUT   UINTN   *CommBufferSize
  )
{
  PSP_SMM_COMMUNICATION_CMN *Buffer;
  PSP_SMM_COMM_RW_FLASH *RwBuffer;
  PSP_SMM_COMM_ERASEFLASH *EraseBuffer;
  PSP_SMM_COMM_RPMC_INCMC *RpmcIncMcBuffer;
  PSP_SMM_COMM_RPMC_REQMC *RpmcReqMcBuffer;
  EFI_STATUS Status;
  UINTN TempCommBufferSize;

  //
  // If input is invalid, stop processing this SMI
  //
  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  TempCommBufferSize = *CommBufferSize;

  if (TempCommBufferSize < sizeof (PSP_SMM_COMMUNICATION_CMN)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
    return EFI_INVALID_PARAMETER;
  }

  if (!SmmIsBufferOutsideSmmValid ((UINTN)CommBuffer, TempCommBufferSize)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("SMM communication data buffer in SMRAM or overflow!\n");
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_UNSUPPORTED;
  Buffer = (PSP_SMM_COMMUNICATION_CMN *)CommBuffer;
  IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler ID %x\n", Buffer->id);

  switch (Buffer->id) {
  case PSP_SMM_COMM_ID_GET_BLOCK_SIZE:
    if (TempCommBufferSize < sizeof (PSP_SMM_COMM_GETBLKSIZE)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    Status = PspGetFlashBlockSize (&((PSP_SMM_COMM_GETBLKSIZE *) CommBuffer)->BlockSize);
    break;
  case PSP_SMM_COMM_ID_READ_FLASH:
    if (TempCommBufferSize < sizeof (PSP_SMM_COMM_RW_FLASH)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    RwBuffer = (PSP_SMM_COMM_RW_FLASH *) CommBuffer;
    if (!SmmIsBufferOutsideSmmValid ((UINTN)RwBuffer->Buffer, RwBuffer->NumBytes)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("SMM communication data buffer in SMRAM or overflow!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    Status = PspReadFlash (RwBuffer->FlashAddress, &RwBuffer->NumBytes, RwBuffer->Buffer);
    break;
  case PSP_SMM_COMM_ID_WRITE_FALSH:
    if (TempCommBufferSize < sizeof (PSP_SMM_COMM_RW_FLASH)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    RwBuffer = (PSP_SMM_COMM_RW_FLASH *) CommBuffer;
    if (!SmmIsBufferOutsideSmmValid ((UINTN)RwBuffer->Buffer, RwBuffer->NumBytes)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("SMM communication data buffer in SMRAM or overflow!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    if (IsWritableRegion (RwBuffer->FlashAddress, RwBuffer->NumBytes)) {
      Status = PspWriteFlash (RwBuffer->FlashAddress, &RwBuffer->NumBytes, RwBuffer->Buffer);
    } else {
      Status = EFI_WRITE_PROTECTED;
      IDS_HDT_CONSOLE_PSP_TRACE ("Unwritable region detected: %lx-%lx\n",
        RwBuffer->FlashAddress, (RwBuffer->FlashAddress + RwBuffer->NumBytes - 1));
    }
    break;
  case PSP_SMM_COMM_ID_ERASE_FALSH:
    if(TempCommBufferSize < sizeof (PSP_SMM_COMM_ERASEFLASH)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    EraseBuffer = (PSP_SMM_COMM_ERASEFLASH *) CommBuffer;
    if (IsWritableRegion (EraseBuffer->FlashAddress, EraseBuffer->NumBytes)) {
      Status = PspEraseFlash (EraseBuffer->FlashAddress, &EraseBuffer->NumBytes);
    } else {
      Status = EFI_WRITE_PROTECTED;
      IDS_HDT_CONSOLE_PSP_TRACE ("Unwritable region detected: %lx-%lx\n",
        EraseBuffer->FlashAddress, (EraseBuffer->FlashAddress + EraseBuffer->NumBytes - 1));
    }
    break;
  case PSP_SMM_COMM_ID_CLOSE_INTERFACE:
    if (mPspSmmCommHandle != NULL) {
      Status = gSmst->SmiHandlerUnRegister (mPspSmmCommHandle);
    } else {
      Status = EFI_INVALID_PARAMETER;
      IDS_HDT_CONSOLE_PSP_TRACE ("mPspSmmCommHandle NULL\n");
    }
    break;
  case PSP_SMM_COMM_ID_RPMC_INCMC:
    if (TempCommBufferSize < sizeof (PSP_SMM_COMM_RPMC_INCMC)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    RpmcIncMcBuffer = (PSP_SMM_COMM_RPMC_INCMC *) CommBuffer;
    Status = PspRpmcIncMc (RpmcIncMcBuffer->CounterAddr, RpmcIncMcBuffer->CounterData, RpmcIncMcBuffer->Signature);
    break;
  case PSP_SMM_COMM_ID_RPMC_REQMC:
    if (TempCommBufferSize < sizeof (PSP_SMM_COMM_RPMC_REQMC)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("PspFlashAccSmmCommunicateHandler Command Buffer Size invalid!\n");
      Status = EFI_INVALID_PARAMETER;
      break;
    }
    RpmcReqMcBuffer = (PSP_SMM_COMM_RPMC_REQMC *) CommBuffer;
    Status = PspRpmcReqMc (RpmcReqMcBuffer->CounterAddr, RpmcReqMcBuffer->Tag, RpmcReqMcBuffer->Signature, RpmcReqMcBuffer->CounterData, RpmcReqMcBuffer->OutputSignature);
    break;
  default:
    ASSERT (FALSE);
  }
  return Status;
}
