//* $NoKeywords:$ */
/**
 * @file
 *
 * PSP Mailbox related functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 321005 $   @e \$Date: 2015-06-17 18:15:31 +0800 (Wed, 17 Jun 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#include "Uefi.h"
#include <Library/BaseMemoryLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspCommonLib.h>
#include <Library/AmdPspFtpmLib.h>
#include <Library/BaseLib.h>
#include <IndustryStandard/Tpm20.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDPSPFTPMLIB_AMDPSPFTPMLIB_FILECODE

VOID
DumpFtpmControlArea (
  IN       TPM2_CONTROL_AREA        *FtpmControlArea
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("Ftpm CmdAddr:0x%x CmdSize:0x%x RespAddr:0x%x RespSize:0x%x\n",
              (UINTN) FtpmControlArea->CommandAddress, FtpmControlArea->CommandSize,
              (UINTN) FtpmControlArea->ResponseAddress, FtpmControlArea->ResponseSize);
}

VOID
DumpFtpmCommandBuffer (
  IN       TPM2_CONTROL_AREA        *FtpmControlArea
  )
{
  UINT32 i;
  UINT8 *BufPtr;

  DumpFtpmControlArea (FtpmControlArea);

  IDS_HDT_CONSOLE_PSP_TRACE ("Command Buffer:\n");
  BufPtr = (UINT8*) (UINTN) (FtpmControlArea->CommandAddress);

  IDS_HDT_CONSOLE_PSP_TRACE ("00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n");

  for (i = 0; i < 0x80; i++, BufPtr++) {
    IDS_HDT_CONSOLE_PSP_TRACE ("%02x ", *(BufPtr));
    if ((i != 0) && ((i + 1) % 16 == 0)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\n");
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\n");
}

VOID
DumpFtpmResponseBuffer (
  IN       TPM2_CONTROL_AREA        *FtpmControlArea
  )
{
  UINT32 i;
  UINT8 *BufPtr;

  DumpFtpmControlArea (FtpmControlArea);

  IDS_HDT_CONSOLE_PSP_TRACE ("Response Buffer:\n");
  BufPtr = (UINT8*) (UINTN) (FtpmControlArea->ResponseAddress);

  IDS_HDT_CONSOLE_PSP_TRACE ("00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15\n");

  for (i = 0; i < 0x80; i++, BufPtr++) {
    IDS_HDT_CONSOLE_PSP_TRACE ("%02x ", *(BufPtr));
    if ((i != 0) && ((i + 1) % 16 == 0)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("\n");
    }
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\n");
}


/**
 *  Set Command,Response buffer registers, Buffer address equal to NULL will skip the setting
 *
 * @param[in]    CommandBuffer              Point to the TPM command buffer
 * @param[in]    CommandSize                Size of the TPM command buffer
 * @param[in]    ResponseBuffer             Point to the TPM response buffer
 * @param[in]    ResponseSize               Size of the TPM response buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 *
 */

EFI_STATUS
FtpmAssignMemory (
  IN     UINTN                CommandBuffer,
  IN     UINT32               CommandSize,
  IN     UINTN                ResponseBuffer,
  IN     UINT32               ResponseSize
  )
{

  IN TPM2_CONTROL_AREA        *FtpmControlArea = NULL;

  if (EFI_ERROR (
        GetFtpmControlArea ((VOID**)&FtpmControlArea)
        )
      ) {
    return EFI_UNSUPPORTED;
  }
  if (CommandBuffer) {
    FtpmControlArea->CommandAddress  = CommandBuffer;
    FtpmControlArea->CommandSize     = (UINT32)CommandSize;
  }

  if (ResponseBuffer) {
    FtpmControlArea->ResponseAddress = ResponseBuffer;
    FtpmControlArea->ResponseSize    = (UINT32)ResponseSize;
  }

  return (EFI_SUCCESS);
}

/**
 *  GET TPM related Info
 *
 * @param[in,out] FtpmStatus              Used to hold more detail info (Unused Currently)
 *
 * @return       EFI_SUCCESS              Ftpm function supported
 * @return       EFI_UNSUPPORTED          Ftpm function unsupported
 *
 */
EFI_STATUS
FtpmGetInfo (
  IN OUT   UINTN                *FtpmStatus
  )
{
  IN TPM2_CONTROL_AREA        *FtpmControlArea = NULL;
  ///< @todo see if we can return any meaningful data else remove this function
  if (EFI_ERROR (
        GetFtpmControlArea ((VOID**)&FtpmControlArea)
        )) {
      return EFI_UNSUPPORTED;
  }
  return (EFI_SUCCESS);
}
/**
 *  Check for a TPM command's completeness
 *
 *
 * @return       TRUE                Command complete
 * @return       FALSE               Command incomplete
 *
 */
BOOLEAN
FtpmCheckStatus (
  TPM2_CONTROL_AREA        *FtpmControlArea
  )
{
  //TPM command finished
  if ( (FtpmControlArea->StatusStart == 0) || (FtpmControlArea->StatusError)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 *  Send a TPM command
 *
 * @param[in]    CommandBuffer              Point to the TPM command buffer
 * @param[in]    CommandSize                Size of the TPM command buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 * @return       EFI_TIMEOUT                Command fail due the time out
 * @return       EFI_DEVICE_ERROR           Command fail due the error status set
 * @return       EFI_BUFFER_TOO_SMALL       Response buffer too small to hold the response
 *
 */
EFI_STATUS
FtpmSendCommand (
  IN     VOID                 *CommandBuffer,
  IN     UINT32                CommandSize
  )
{
  IN TPM2_CONTROL_AREA *FtpmControlArea = NULL;
  UINT32     Command;
  TPM2_COMMAND_HEADER  *TpmHdr;
  TPM_CC TpmCommandCode;
  BOOLEAN    ExeStatus;
  UINT32     v32;

  AGESA_TESTPOINT (TpPspfTpmSendCmdEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.Ftpm.SendCmd\n");
  //Exit if PSP fTPM set to disable
  if (PcdGet8 (PcdAmdPspSystemTpmConfig) != SYSTEM_TPM_CONFIG_PSP_FTPM) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPSP fTPM disabled, exit\n");
    return EFI_UNSUPPORTED;
  }

  //Validate Input parameters
  if ((CommandBuffer == NULL) || (CommandSize == 0)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tInvalid parameters (PSP.Ftpm.SendCmd)\n");
    return EFI_INVALID_PARAMETER;
  }


  //Always enable enable MMIO access, the CMD register may be disabled during PCI enumuration when plug some special cards
  SwitchPspMmioDecode (TRUE, &Command);

  if (EFI_ERROR (
        GetFtpmControlArea ((VOID**)&FtpmControlArea)
        )) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tFtpm Bar Error (PSP.Ftpm.SendCmd)\n");
    return EFI_UNSUPPORTED;
  }

  ASSERT (FtpmControlArea->CommandAddress);
  ASSERT (FtpmControlArea->CommandSize);
  CopyMem ((VOID *) (UINTN) FtpmControlArea->CommandAddress, CommandBuffer, CommandSize);

  //Alway use the pre-allocated buffer to hold the response
  ASSERT (FtpmControlArea->ResponseAddress);

  FtpmControlArea->ResponseSize =  FTPM_RESPONSE_BUFFER_SIZE;
  DEBUG_CODE (
    DumpFtpmCommandBuffer (FtpmControlArea);
  );

  SetMem ((VOID *) (UINTN) FtpmControlArea->ResponseAddress, FtpmControlArea->ResponseSize, 0x0);

  //Dump Ftpm Control Area
  //Issue the command
  FtpmControlArea->StatusStart = 1;

  // wait for Ftpm to finish
  TpmHdr = CommandBuffer;
  TpmCommandCode = SwapBytes32 (TpmHdr->commandCode);
  if ((TpmCommandCode == TPM_CC_CreatePrimary) ||
      (TpmCommandCode == TPM_CC_Create)) {
    // Time out in PcdAmdFtpmTimeout1 seconds
    ExeStatus = PspLibTimeOut (PcdGet64 (PcdAmdFtpmTimeout1), (FP_CONDITIONER) &FtpmCheckStatus, FtpmControlArea);
  } else {
    // Time out in PcdAmdFtpmTimeout2 seconds
    ExeStatus = PspLibTimeOut (PcdGet64 (PcdAmdFtpmTimeout2), (FP_CONDITIONER) &FtpmCheckStatus, FtpmControlArea);
  }

  if (ExeStatus == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\n Command time out (PSP.Ftpm.SendCmd)\n");
    //Restore Command register
    SwitchPspMmioDecode (FALSE, &Command);
    return EFI_TIMEOUT;
  }

  DEBUG_CODE (
    //Show post code, DeadLoop when timeout
    if (ExeStatus == FALSE) {
      v32 = 0x99009000;
      v32 |= TpmCommandCode;
      LibAmdIoWrite (AccessWidth32, 0x80, &v32, NULL);
      CpuDeadLoop ();
    }
  );

  if (FtpmControlArea->StatusError) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\n Command error (PSP.Ftpm.SendCmd)\n");
    return (EFI_DEVICE_ERROR);
  }

  //Restore Command register
  SwitchPspMmioDecode (FALSE, &Command);
  AGESA_TESTPOINT (TpPspfTpmSendCmdExit, NULL);
  return (EFI_SUCCESS);
}

/**
 *  Get a TPM command's response
 *
 * @param[in]    ResponseBuffer             Point to the TPM response buffer
 * @param[in]    ResponseSize               Size of the TPM response buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 * @return       EFI_TIMEOUT                Command fail due the time out
 * @return       EFI_DEVICE_ERROR           Command fail due the error status set
 * @return       EFI_BUFFER_TOO_SMALL       Response buffer too small to hold the response
 *
 */
EFI_STATUS
FtpmGetResponse (
  IN OUT   VOID                  *ResponseBuffer,
  IN OUT   UINT32                *ResponseSize
  )
{
  IN TPM2_CONTROL_AREA *FtpmControlArea = NULL;
  UINT32     Command;
  BOOLEAN    ExeStatus;

  AGESA_TESTPOINT (TpPspfTpmRcvCmdEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.Ftpm.GetResponse\n");
  //Exit if PSP fTPM set to disable
  if (PcdGet8 (PcdAmdPspSystemTpmConfig) != SYSTEM_TPM_CONFIG_PSP_FTPM) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPSP fTPM disabled, exit\n");
    return EFI_UNSUPPORTED;
  }

  //Validate Input parameters
  if ((ResponseBuffer == NULL) || (ResponseSize == NULL) || (*ResponseSize == 0)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tInvalid parameters (PSP.Ftpm.GetResponse)\n");
    return EFI_INVALID_PARAMETER;
  }

  if (GetFtpmControlArea ((VOID**)&FtpmControlArea) == FALSE) {
    return EFI_UNSUPPORTED;
  }

  //WA Always enable enable MMIO access, the CMD register may be disabled during PCI enumuration when plug some special cards
  SwitchPspMmioDecode (TRUE, &Command);

  ASSERT (FtpmControlArea->ResponseAddress);
  if (FtpmControlArea->ResponseAddress == 0) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tResponse Buffer Null (PSP.Ftpm.GetResponse)\n");
    DumpFtpmControlArea (FtpmControlArea);
    //Restore Command register
    SwitchPspMmioDecode (FALSE, &Command);
    return EFI_UNSUPPORTED;
  }

  // wait for Ftpm to finish
  // Time out in PcdAmdFtpmTimeout2 seconds
  ExeStatus = PspLibTimeOut (PcdGet64 (PcdAmdFtpmTimeout2), (FP_CONDITIONER) &FtpmCheckStatus, FtpmControlArea);
  if (ExeStatus == FALSE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\n Command time out (PSP.Ftpm.GetResponse)\n");
    //Restore Command register
    SwitchPspMmioDecode (FALSE, &Command);
    return EFI_TIMEOUT;
  }

  if (FtpmControlArea->StatusError) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\n Command error (PSP.Ftpm.GetResponse)\n");
    //Restore Command register
    SwitchPspMmioDecode (FALSE, &Command);
    return (EFI_DEVICE_ERROR);
  }

  DEBUG_CODE (
    DumpFtpmResponseBuffer (FtpmControlArea);
  );

  if (FtpmControlArea->ResponseSize) {
    CopyMem ( ResponseBuffer, (VOID *) (UINTN)FtpmControlArea->ResponseAddress, *ResponseSize);
  }
  FtpmControlArea->ResponseSize = FTPM_RESPONSE_BUFFER_SIZE;

  //Restore Command register
  SwitchPspMmioDecode (FALSE, &Command);
  AGESA_TESTPOINT (TpPspfTpmRcvCmdExit, NULL);
  return (EFI_SUCCESS);
}

/**
 *  Execute a TPM command
 *
 * @param[in]    CommandBuffer              Point to the TPM command buffer
 * @param[in]    CommandSize                Size of the TPM command buffer
 * @param[in]    ResponseBuffer             Point to the TPM response buffer
 * @param[in]    ResponseSize               Size of the TPM response buffer
 *
 * @return       EFI_SUCCESS                Command executed successfully
 * @return       EFI_UNSUPPORTED            Device unsupported
 * @return       EFI_TIMEOUT                Command fail due the time out
 * @return       EFI_DEVICE_ERROR           Command fail due the error status set
 * @return       EFI_BUFFER_TOO_SMALL       Response buffer too small to hold the response
 *
 */
EFI_STATUS
FtpmExecuteCommand (
  IN       VOID                 *CommandBuffer,
  IN       UINT32               CommandSize,
  IN OUT   VOID                 *ResponseBuffer,
  IN OUT   UINT32               *ResponseSize
  )
{
  EFI_STATUS Status;
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.Ftpm Execute Cmd\n");

  //Exit if PSP fTPM set to disable
  if (PcdGet8 (PcdAmdPspSystemTpmConfig) != SYSTEM_TPM_CONFIG_PSP_FTPM) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPSP fTPM disabled disabled, exit\n");
    return EFI_UNSUPPORTED;
  }

  Status = FtpmSendCommand (CommandBuffer, CommandSize);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tSend Command Error (PSP.Ftpm Execute Cmd)\n");
    return Status;
  }

  Status = FtpmGetResponse (ResponseBuffer, ResponseSize);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tSend Response Error (PSP.Ftpm Execute Cmd)\n");
    return Status;
  }
  return (EFI_SUCCESS);
}


