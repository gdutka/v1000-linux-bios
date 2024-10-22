/* $NoKeywords:$ */
/**
 * @file
 *
 * FchPcieWakeWA Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspSmm Driver
 * @e \$Revision: 330494 $   @e \$Date: 2015-11-09 17:22:37 +0800 (Mon, 09 Nov 2015) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/S3BootScriptLib.h>
#include "Library/AmdBaseLib.h"
#include "Library/IdsLib.h"
#include "AmdSmmCommunication.h"
#include "AmdS3SaveLibData.h"
#include "Filecode.h"
#include <Library/SmmMemLib.h>

#define FILECODE PSP_AMDPSPSMMV1_AMDPSPSMMV1_FILECODE
EFI_HANDLE                      mAmdSmmCommunicationHandle;

VOID
AmdS3SaveScriptSmmDispatch (
  IN OUT   VOID    *PrivateDataAddress
  )
{
  AMD_S3_LIB_PRIVATE_DATA *AmdS3LibPrivateData;
  AMD_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *AmdS3LibTblHdr;
  UINT8 *OpData;
  UINT16 OpCode;
  UINTN  TempCommBufferSize;

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3SaveScriptSmmDispatch Enter\n");

  //
  // If input is invalid, stop processing this handler
  //
  if (PrivateDataAddress == NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Error: PrivateDataAddress is NULL\n");
    return;
  }

  //Locate the global data structure via PCD
  AmdS3LibPrivateData = NULL;
  AmdS3LibPrivateData = PrivateDataAddress;

  if (!SmmIsBufferOutsideSmmValid((UINTN)PrivateDataAddress, sizeof (AMD_S3_LIB_PRIVATE_DATA))) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Parameter PrivateDataAddress(0x%p) not in SMRAM or overflow\n", PrivateDataAddress);
    return;
  }

  //Validate the save table
  AmdS3LibTblHdr = (AMD_S3_LIB_BOOT_SCRIPT_TABLE_HEADER *) AmdS3LibPrivateData->TableBase;
  if (AmdS3LibTblHdr == NULL) {
    return;
  }
  TempCommBufferSize = AmdS3LibTblHdr->TableLength;
  if (TempCommBufferSize < sizeof (AMD_S3_LIB_BOOT_SCRIPT_TABLE_HEADER)) {
    TempCommBufferSize = sizeof (AMD_S3_LIB_BOOT_SCRIPT_TABLE_HEADER);
  }
  if (!SmmIsBufferOutsideSmmValid((UINTN)AmdS3LibTblHdr, TempCommBufferSize)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Invalid parameter AmdS3LibTblHdr(0x%p), AmdS3LibTblHdr->TableLength(0x%x)\n",
                    AmdS3LibTblHdr, AmdS3LibTblHdr->TableLength);
    return;
  }
  ASSERT (AmdS3LibTblHdr->Sinature == AMD_S3LIB_BOOT_SCRIPT_TABLE_SIG);
  if (AmdS3LibTblHdr->Sinature != AMD_S3LIB_BOOT_SCRIPT_TABLE_SIG) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3LibTbl Signature Check Fail\n");
    return;
  }
  ASSERT (*((UINT32 *) (AmdS3LibPrivateData->TableBase + AmdS3LibTblHdr->TableLength)) == AMD_S3LIB_TABLE_WATCHER);
  if (*((UINT32 *) (AmdS3LibPrivateData->TableBase + AmdS3LibTblHdr->TableLength)) != AMD_S3LIB_TABLE_WATCHER) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Potential memory overflow observed in AmdS3LibTbl\n");
    return;
  }

  //Loop the table, and invoke S3 Script Lib to save the script data
  OpData = &AmdS3LibTblHdr->Data;
  OpCode = ((AMD_S3LIB_BOOT_SCRIPT_COMMON *) OpData)->OpCode;
  while (OpCode != AMD_S3LIB_TABLE_END) {
    switch (OpCode) {
    case AMD_S3LIB_BOOT_SCRIPT_IO_WRITE_OP:
      S3BootScriptSaveIoWrite (((AMD_S3LIB_BOOT_SCRIPT_IO_WRITE *) OpData)->Width,
                               ((AMD_S3LIB_BOOT_SCRIPT_IO_WRITE *) OpData)->Address,
                               1,
                               &((AMD_S3LIB_BOOT_SCRIPT_IO_WRITE *) OpData)->Value);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_IO_WRITE);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_IO_RMW_OP:
      S3BootScriptSaveIoReadWrite (((AMD_S3LIB_BOOT_SCRIPT_IO_RMW *) OpData)->Width,
                                   ((AMD_S3LIB_BOOT_SCRIPT_IO_RMW *) OpData)->Address,
                                   &((AMD_S3LIB_BOOT_SCRIPT_IO_RMW *) OpData)->Data,
                                   &((AMD_S3LIB_BOOT_SCRIPT_IO_RMW *) OpData)->DataMask);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_IO_RMW);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_IO_POLL_OP:
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_IO_POLL);
      S3BootScriptSaveIoPoll (((AMD_S3LIB_BOOT_SCRIPT_IO_POLL *) OpData)->Width,
                              ((AMD_S3LIB_BOOT_SCRIPT_IO_POLL *) OpData)->Address,
                              &((AMD_S3LIB_BOOT_SCRIPT_IO_POLL *) OpData)->Data,
                              &((AMD_S3LIB_BOOT_SCRIPT_IO_POLL *) OpData)->DataMask,
                              ((AMD_S3LIB_BOOT_SCRIPT_IO_POLL *) OpData)->Delay);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_MEM_WRITE_OP:
      S3BootScriptSaveMemWrite (((AMD_S3LIB_BOOT_SCRIPT_MEM_WRITE *) OpData)->Width,
                                ((AMD_S3LIB_BOOT_SCRIPT_MEM_WRITE *) OpData)->Address,
                                1,
                                &((AMD_S3LIB_BOOT_SCRIPT_MEM_WRITE *) OpData)->Value);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_MEM_WRITE);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_MEM_RMW_OP:
      S3BootScriptSaveMemReadWrite (((AMD_S3LIB_BOOT_SCRIPT_MEM_RMW *) OpData)->Width,
                                    ((AMD_S3LIB_BOOT_SCRIPT_MEM_RMW *) OpData)->Address,
                                    &((AMD_S3LIB_BOOT_SCRIPT_MEM_RMW *) OpData)->Data,
                                    &((AMD_S3LIB_BOOT_SCRIPT_MEM_RMW *) OpData)->DataMask);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_MEM_RMW);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_MEM_POLL_OP:
      S3BootScriptSaveMemPoll (((AMD_S3LIB_BOOT_SCRIPT_MEM_POLL *) OpData)->Width,
                               ((AMD_S3LIB_BOOT_SCRIPT_MEM_POLL *) OpData)->Address,
                               &((AMD_S3LIB_BOOT_SCRIPT_MEM_POLL *) OpData)->DataMask,
                               &((AMD_S3LIB_BOOT_SCRIPT_MEM_POLL *) OpData)->Data,
                               1,
                               ((AMD_S3LIB_BOOT_SCRIPT_MEM_POLL *) OpData)->LoopTimes);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_MEM_POLL);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_PCI_WRITE_OP:
      S3BootScriptSavePciCfg2Write (((AMD_S3LIB_BOOT_SCRIPT_PCI_WRITE *) OpData)->Width,
                                    ((AMD_S3LIB_BOOT_SCRIPT_PCI_WRITE *) OpData)->Segment,
                                    ((AMD_S3LIB_BOOT_SCRIPT_PCI_WRITE *) OpData)->Address,
                                    1,
                                    &((AMD_S3LIB_BOOT_SCRIPT_PCI_WRITE *) OpData)->Value);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_PCI_WRITE);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_PCI_RMW_OP:
      S3BootScriptSavePciCfg2ReadWrite (((AMD_S3LIB_BOOT_SCRIPT_PCI_RMW *) OpData)->Width,
                                        ((AMD_S3LIB_BOOT_SCRIPT_PCI_RMW *) OpData)->Segment,
                                        ((AMD_S3LIB_BOOT_SCRIPT_PCI_RMW *) OpData)->Address,
                                        &((AMD_S3LIB_BOOT_SCRIPT_PCI_RMW *) OpData)->Data,
                                        &((AMD_S3LIB_BOOT_SCRIPT_PCI_RMW *) OpData)->DataMask);
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_PCI_RMW);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_PCI_POLL_OP:
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_PCI_POLL);
      S3BootScriptSavePci2Poll (((AMD_S3LIB_BOOT_SCRIPT_PCI_POLL *) OpData)->Width,
                                ((AMD_S3LIB_BOOT_SCRIPT_PCI_POLL *) OpData)->Segment,
                                ((AMD_S3LIB_BOOT_SCRIPT_PCI_POLL *) OpData)->Address,
                                &((AMD_S3LIB_BOOT_SCRIPT_PCI_POLL *) OpData)->Data,
                                &((AMD_S3LIB_BOOT_SCRIPT_PCI_POLL *) OpData)->DataMask,
                                ((AMD_S3LIB_BOOT_SCRIPT_PCI_POLL *) OpData)->Delay);
      break;
    case AMD_S3LIB_BOOT_SCRIPT_DISPATCH_OP:
      if (AmdS3LibPrivateData->S3ScriptLock) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "Error: cannot dispatch S3 Boot Script after SMM Lock!\n");
        return;
      }
      OpData += sizeof (AMD_S3LIB_BOOT_SCRIPT_DISPATCH);
      S3BootScriptSaveDispatch2 (((AMD_S3LIB_BOOT_SCRIPT_DISPATCH *) OpData)->EntryPoint,
                                 ((AMD_S3LIB_BOOT_SCRIPT_DISPATCH *) OpData)->Context);
      break;
    default:
      ASSERT (FALSE);
      break;
    }
    OpCode = ((AMD_S3LIB_BOOT_SCRIPT_COMMON *) OpData)->OpCode;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3SaveScriptSmmDispatch Exit\n");
}

EFI_STATUS
AmdS3SaveSmmCommunicateHandler (
  IN       EFI_HANDLE  DispatchHandle,
  IN       CONST VOID  *Context,
  IN OUT   VOID    *CommBuffer,
  IN OUT   UINTN   *CommBufferSize
  )
{
  AMD_SMM_COMMUNICATION_CMN *CmnBuffer;
  AMD_SMM_COMMUNICATION_S3SCRIPT *S3ScriptBuffer;
  EFI_STATUS  Status;

  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3SaveSmmCommunicateHandler Enter\n");

  S3ScriptBuffer = NULL;
  //
  // If input is invalid, stop processing this SMI
  //
  if (CommBuffer == NULL || CommBufferSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!SmmIsBufferOutsideSmmValid((UINTN)CommBuffer, (UINTN)(*CommBufferSize))) {
    Status = EFI_INVALID_PARAMETER;
    return Status;
  }
  CmnBuffer = (AMD_SMM_COMMUNICATION_CMN *)CommBuffer;
  if (CmnBuffer->id == AMD_SMM_COMMUNICATION_ID_S3SCRIPT) {
    S3ScriptBuffer = (AMD_SMM_COMMUNICATION_S3SCRIPT *) CmnBuffer;
    AmdS3SaveScriptSmmDispatch (S3ScriptBuffer->PrivateDataAddress);
  }
  //Unregister the hanlder to make sure execute only once
  Status = gSmst->SmiHandlerUnRegister (mAmdSmmCommunicationHandle);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "AmdS3SaveSmmCommunicateHandler Unregister Fail\n");
  }
  return EFI_SUCCESS;
}


EFI_STATUS
AmdSmmCommunicationEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  //
  // We're now in SMM!
  //

  //Register SMM Handler for handle the communication from outside of SMM
  mAmdSmmCommunicationHandle = NULL;
  Status = gSmst->SmiHandlerRegister (AmdS3SaveSmmCommunicateHandler, &gAmdSmmCommunicationHandleGuid, &mAmdSmmCommunicationHandle);
  ASSERT (Status == EFI_SUCCESS);

  return EFI_SUCCESS;
}


