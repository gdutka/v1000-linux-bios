/** @file

;******************************************************************************
;* Copyright (c) 2017, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DashSmm.h"

ASF_SET_SYSTEM_STATE_MSG     mSystemStateMessage;
UINT8                        mSlaveAddress;

/**
 Parse the sleep type (S0 S3 S4 S5) to system state of message

 @param [in]   SleepType      The system sleep type
 @param [out]  SysStateOfMsg  The system state of message

 @retval 0xff           Not found
 @retval Others         Found

**/
UINT8
ParseSleepState (
  IN  UINT8 SleepType
  )
{
  UINT8 SysStateOfMsg;

  switch (SleepType) {

  case SxS0:
    SysStateOfMsg = ASF_SYSTEM_STATE_S0;
    break;

  case SxS3:
    SysStateOfMsg = ASF_SYSTEM_STATE_S3;
    break;

  case SxS4:
    SysStateOfMsg = ASF_SYSTEM_STATE_S4;
    break;

  case SxS5:
    SysStateOfMsg = ASF_SYSTEM_STATE_S5;
    break;

  default:
    SysStateOfMsg = 0xFF;
    break;
  }

  return SysStateOfMsg;
}

/**
 Send ASF System State Message

 @param [in]   SysStateOfMsg    The System State of the system state message

**/
VOID
SetSystemMessage (
  UINT8 SysStateOfMsg
  )
{
  EFI_STATUS                        Status;
  MCTP_SMBUS_PROTOCOL               *MctpSmbus;

  mSystemStateMessage.SubCommand    = ASF_SET_SYSTEM_STATE_SUB_CMD;
  mSystemStateMessage.VersionNumber = ASF_SET_SYSTEM_STATE_VER_NUM;
  mSystemStateMessage.SystemState   = SysStateOfMsg;

  if (gSmst == NULL) {
    return;
  }
  Status = gSmst->SmmLocateProtocol (
                    &gMctpSmbusProtocolGuid,
                    NULL,
                    (VOID **)&MctpSmbus
                    );
  if (!EFI_ERROR(Status)) {
    MctpSmbus->SendMessage (MctpSmbus, mSlaveAddress, &mSystemStateMessage, sizeof (ASF_SET_SYSTEM_STATE_MSG), ASF_SET_SYSTEM_STATE_CMD);
  }
}

/**
  Sleep entry callback for SetSystemMessage to management controller using gEfiSmmSxDispatch2ProtocolGuid

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The sleep entry callabck was handled successfully.
  @retval EFI_UNSUPPORTED         The sleep entry callabck does not support the SetSystemMessage.

**/
EFI_STATUS
EFIAPI
SleepEntryCallBack2(
  IN EFI_HANDLE                     DispatchHandle,
  IN CONST VOID                     *Context         OPTIONAL,
  IN OUT VOID                       *CommBuffer      OPTIONAL,
  IN OUT UINTN                      *CommBufferSize  OPTIONAL
  )
{
  EFI_SMM_SX_REGISTER_CONTEXT      *EntryDispatchContext2;
  UINT8                            SysStateOfMsg;

  if(Context == NULL) {
    return EFI_UNSUPPORTED;
  }
  EntryDispatchContext2 = (EFI_SMM_SX_REGISTER_CONTEXT *)Context;

  SysStateOfMsg = ParseSleepState((UINT8) EntryDispatchContext2->Type);
  if (SysStateOfMsg == 0xFF) {
    return EFI_UNSUPPORTED;
  }
  SetSystemMessage(SysStateOfMsg);

  return EFI_SUCCESS;
}

/**
  The driver's entry point.

  It register callbacks for SetSystemMessage (s3/S4/S5) and set S0 state message to management controller.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DashSmmEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  MCTP_SMBUS_PROTOCOL                   *MctpSmbus;
  DASH_IO_CFG_PROTOCOL                  *DashIoCfg;
  EFI_SMM_SX_DISPATCH2_PROTOCOL         *SxDispatch2;
  EFI_SMM_SX_REGISTER_CONTEXT           EntryDispatchContext2;
  UINT8                                 SleepType;

  Status = gSmst->SmmLocateProtocol (&gDashIoCfgProtocolGuid, NULL, (VOID **)&DashIoCfg);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }
  mSlaveAddress = DashIoCfg->GetMcSmbusAddr();

  Status = gSmst->SmmLocateProtocol (&gMctpSmbusProtocolGuid, NULL, (VOID **)&MctpSmbus);
  if (EFI_ERROR(Status)) {
    return EFI_UNSUPPORTED;
  }
  SetSystemMessage(ASF_SYSTEM_STATE_S0);

  //
  // Get the Sx dispatch2 protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, (VOID **)&SxDispatch2);
  if (!EFI_ERROR(Status)) {
    for (SleepType = SxS3; SleepType <= SxS5; SleepType++) {
      EntryDispatchContext2.Type  = SleepType;
      EntryDispatchContext2.Phase = SxEntry;
      Status = SxDispatch2->Register (
                              SxDispatch2,
                              SleepEntryCallBack2,
                              &EntryDispatchContext2,
                              &Handle
                              );
      ASSERT_EFI_ERROR (Status);
    }
    return EFI_SUCCESS;
  }

  if(gBS == NULL) {
    return EFI_UNSUPPORTED;
  }

  return EFI_UNSUPPORTED;
}