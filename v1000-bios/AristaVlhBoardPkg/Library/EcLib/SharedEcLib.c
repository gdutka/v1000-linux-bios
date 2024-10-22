/** @file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SharedEcLib.h"


STATIC
VOID
ImcWait (
  IN BOOLEAN                      EnableWrites
  )
{
  UINT8                           DbReply;
  UINT8                           DbPortStatus;
  UINTN                           Address;

  //
  // Read EC strap status.
  //
  Address = ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG80;
  DbPortStatus = *((UINT8*) (UINTN) Address);

  //
  // EC strap enabled.
  //
  if (DbPortStatus & ChipSysEcEnable) {
    //
    // Enter scratch ROM.
    //
    if (EnableWrites) {
      IoWrite8 (MailBoxPort, 0x82);
      IoWrite8 (MailBoxPort + 1, 0x00);
      //
      // Sub-function value 0x0B4 - Enter scratch ROM
      //
      IoWrite8 (MailBoxPort, 0x83);
      IoWrite8 (MailBoxPort + 1, EC_ENTER_WAIT_COMMAND);
      IoWrite8 (MailBoxPort, 0x80);
      IoWrite8 (MailBoxPort + 1, 0x96);

      //
      // Wait IMC reply 0xFA - Function completed,
      // firmware may take up to 500ms to return the status.
      //
      do {
        IoWrite8 (MailBoxPort, 0x82);
        DbReply = IoRead8 (MailBoxPort + 1);
      } while (DbReply != EC_ENTER_WAIT_ACKNOWLEDGE);
    } else {
      //
      // Exit scratch ROM
      //
      IoWrite8 (MailBoxPort, 0x82);
      IoWrite8 (MailBoxPort + 1, 0x00);

      //
      // Sub-function value 0x0B5 - Exit scratch ROM
      //
      IoWrite8 (MailBoxPort, 0x83);
      IoWrite8 (MailBoxPort + 1, EC_LEAVE_WAIT_COMMAND);
      IoWrite8 (MailBoxPort, 0x80);
      IoWrite8 (MailBoxPort + 1, 0x96);

      //
      // Wait IMC reply 0xFA - Function completed,
      // firmware may take up to 500ms to return the status.
      //
      do {
        IoWrite8 (MailBoxPort, 0x82);
        DbReply = IoRead8 (MailBoxPort + 1);
      } while (DbReply != EC_LEAVE_WAIT_ACKNOWLEDGE);
    }
  }
}

/**
  Write data to Kbc data port

  @param[in]  CommandState        the Io to write.
  @param[in]  Data                The data which want write to Kbc data port

  @retval     EFI_SUCCESS         Write data to Kbc port successfully

**/
STATIC
EFI_STATUS
WriteKbc (
  IN UINT16                       CommandState,
  IN UINT8                        Data
  )
{
  IoWrite8 (CommandState, Data);
  return EFI_SUCCESS;
}

/**
  Read data from kbc data port

  @param[in]  CommandState        the Io to read.
  @param[in]  Data                IN OUT :The data which read from Kbc data port

  @retval     EFI_SUCCESS           Read data from Kbc port successfully.
  @retval     EFI_INVALID_PARAMETER The input parameter is invalid.

**/
STATIC
EFI_STATUS
ReadKbc (
  IN UINT16                       CommandState,
  IN OUT UINT8                    *Data
  )
{
  if (Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Data = IoRead8 (CommandState);
  return EFI_SUCCESS;
}

/**
  Wait for output buffer full.

  @param[in]  CommandState        The I/O to read.

  @retval     EFI_SUCCESS         Input buffer is full.
**/
STATIC
EFI_STATUS
WaitKbcObf (
  IN UINT16                       CommandState
  )
{
  UINT8                           EcCmdState;
  UINTN                           Index;

  EcCmdState = 0;
  for (Index = 0; Index < EC_TIME_OUT; ++Index) {
    EcCmdState = IoRead8 (CommandState);
    if (EcCmdState & EC_KEY_OBF) {
      return EFI_SUCCESS;
    }

    //
    // Dummy read for delay.
    //
    IoRead8 (0x80);
    IoRead8 (0x80);
    IoRead8 (0x80);
    IoRead8 (0x80);
  }

  return EFI_DEVICE_ERROR;
}

/**
  Wait for input buffer empty.

  @param[in]  CommandState      The I/O to read.

  @retval     EFI_SUCCESS       Input buffer is full.
**/
STATIC
EFI_STATUS
WaitKbcIbe (
  IN UINT16                     CommandState
  )
{
  UINT8                         EcCmdState;
  UINT16                        CommandData;
  UINTN                         Index;

  CommandData = CommandState - 4;
  for (Index = 0; Index < EC_TIME_OUT; ++Index) {
    EcCmdState = IoRead8 (CommandState);
    if (!(EcCmdState & EC_KEY_IBF)) {
      return EFI_SUCCESS;
    }

    //
    // Flush it for AC/DC issue.
    //
    IoRead8(CommandData);
    //
    // Dummy read for delay.
    //
    IoRead8(0x80);
    IoRead8(0x80);
    IoRead8(0x80);
    IoRead8(0x80);
  }

  return EFI_DEVICE_ERROR;
}


VOID
EcWrite (
  IN UINT8                        Index,
  IN UINT8                        Value
  )
{
  EFI_STATUS                      Status;

  Status = WaitKbcIbe (EC_CMD_STATUS_PORT);
  if (EFI_ERROR (Status)) {
    return;
  }

  WriteKbc (EC_CMD_STATUS_PORT, 0x81);
  Status = WaitKbcIbe (EC_CMD_STATUS_PORT);
  if (EFI_ERROR (Status)) {
    return;
  }

  WriteKbc (EC_CMD_DATA_PORT, Index);
  Status = WaitKbcIbe (EC_CMD_STATUS_PORT);
  if (EFI_ERROR (Status)) {
    return;
  }

  WriteKbc (EC_CMD_DATA_PORT, Value);
  Status = WaitKbcIbe (EC_CMD_STATUS_PORT);
}


UINT8
EcRead (
  IN UINT8                        Index
  )
{
  EFI_STATUS                      Status;
  UINT8                           Data;

  Status = WaitKbcIbe (EC_CMD_STATUS_PORT);
  if (EFI_ERROR (Status)) {
    return 0xFF;
  }

  WriteKbc (EC_CMD_STATUS_PORT, 0x80);

  Status = WaitKbcIbe (EC_CMD_STATUS_PORT);
  if (EFI_ERROR (Status)) {
    return 0xFF;
  }
  WriteKbc (EC_CMD_DATA_PORT, Index);

  Status = WaitKbcObf (EC_CMD_STATUS_PORT);
  if (EFI_ERROR (Status)) {
    return 0xFF;
  }
  ReadKbc (EC_CMD_DATA_PORT, &Data);
  return Data;
}


/**
  Platform specific function to enable/disable flash

  @param[in]  EnableEcIdle        Boolean to enable/disable flash.

  @retval     EFI_UNSUPPORTED     Returns unsupported by default.
  @retval     EFI_SUCCESS         The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED   The value of IN OUT parameter is changed.
**/
EFI_STATUS
EcIdle (
  IN BOOLEAN                      EnableEcIdle
  )
{
  ImcWait (EnableEcIdle);
  return EFI_SUCCESS;
}


/**
  Platform specific function to enable/disable flash

  @param[in]  EnableEcWait        Boolean to enable/disable flash.

  @retval     EFI_UNSUPPORTED     Returns unsupported by default.
  @retval     EFI_SUCCESS         The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED   The value of IN OUT parameter is changed.
**/
EFI_STATUS
EcWait (
  IN BOOLEAN                      EnableEcWait
  )
{
  ImcWait (EnableEcWait);
  return EFI_SUCCESS;
}


/**
  Platform specific function to enable/disable EC ACPI Mode

  @param[in]  EnableEcAcpiMode    Boolean to enable/disable ACPI mode.

  @retval     EFI_UNSUPPORTED     Returns unsupported by default.
  @retval     EFI_SUCCESS         The service is customized in the project.
  @retval     EFI_MEDIA_CHANGED   The value of IN OUT parameter is changed.
**/
EFI_STATUS
EcAcpiMode (
  IN BOOLEAN                      EnableEcAcpiMode
  )
{
  UINT8 Value8;

  Value8 = EcRead (EC_STATUS_INDEX);
  if (Value8 != 0xFF) {
    if (EnableEcAcpiMode) {
      Value8 |= EC_ACPI_ENABLE_BIT;
    } else {
      Value8 &= (~EC_ACPI_ENABLE_BIT);
    }

    EcWrite (EC_STATUS_INDEX, Value8);
    return EFI_SUCCESS;
  }
  return EFI_NOT_FOUND;
}
