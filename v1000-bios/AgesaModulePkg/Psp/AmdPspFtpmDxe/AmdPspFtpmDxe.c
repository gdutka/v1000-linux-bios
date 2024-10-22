/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP TPM related functions in DXE
 *
 * Contains PSP TPM interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 310042 $   @e \$Date: 2014-12-25 16:09:57 +0800 (Thu, 25 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspCommonLib.h>
#include <Library/AmdPspFtpmLib.h>
#include <Protocol/AmdPspFtpmProtocol.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPFTPMDXE_AMDPSPFTPMDXE_FILECODE


EFI_STATUS
GetFtpmStatus (
  IN       PSP_FTPM_PROTOCOL    *This,
  IN OUT   UINTN                *FtpmStatus
  )
{

  return (FtpmGetInfo (FtpmStatus));
}

EFI_STATUS
SendFtpmCommand (
  IN     PSP_FTPM_PROTOCOL    *This,
  IN     VOID                 *CommandBuffer,
  IN     UINT32               CommandSize
  )
{
  return FtpmSendCommand (
      CommandBuffer,
      CommandSize);

}

EFI_STATUS
GetFtpmResponse (
  IN       PSP_FTPM_PROTOCOL     *This,
  IN OUT   VOID                  *ResponseBuffer,
  IN OUT   UINT32                *ResponseSize
  )
{
  EFI_STATUS Status;

  if (!ResponseBuffer) {
    return (EFI_NOT_FOUND);
  }

  Status =  FtpmGetResponse (
                            ResponseBuffer,
                            ResponseSize
                          );
  return (Status);
}

EFI_STATUS
ExecuteFtpmCommand (
  IN       PSP_FTPM_PROTOCOL    *This,
  IN       VOID                 *CommandBuffer,
  IN       UINT32                CommandSize,
  IN OUT   VOID                 *ResponseBuffer,
  IN OUT   UINT32               *ResponseSize
  )
{
  return FtpmExecuteCommand (
                                CommandBuffer,
                                CommandSize,
                                ResponseBuffer,
                                ResponseSize
                               );
}


PSP_FTPM_PROTOCOL mAmdPspFtpmProtocol = {
  ExecuteFtpmCommand,
  GetFtpmStatus,
  SendFtpmCommand,
  GetFtpmResponse,
};

EFI_STATUS
EFIAPI
AmdPspFtpmDxeDriverEntry (
  IN EFI_HANDLE ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )

{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;
  UINTN                       AllocateSize;
  UINTN                       CommandBuffer;
  UINTN                       ResponseBuffer;

  AGESA_TESTPOINT (TpPspfTpmDxeEntry, NULL);
  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresent () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == 0) ||
      (PcdGet8 (PcdAmdPspSystemTpmConfig) != SYSTEM_TPM_CONFIG_PSP_FTPM)) {
    return EFI_SUCCESS;
  }

  CommandBuffer = 0;
  ResponseBuffer = 0;

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.FtpmDxe Enter\n");

  //
  // Allocate pages for TPM Control area (as well as command & response buffer)
  AllocateSize =  ( FTPM_COMMAND_BUFFER_SIZE  + FTPM_RESPONSE_BUFFER_SIZE );

  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (AllocateSize),
                  (EFI_PHYSICAL_ADDRESS *)&CommandBuffer);

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  gBS->SetMem ((VOID *)CommandBuffer, AllocateSize, 0);
  ResponseBuffer = (UINTN)CommandBuffer + FTPM_COMMAND_BUFFER_SIZE;
  Status = FtpmAssignMemory (
    (UINTN)CommandBuffer,
    FTPM_COMMAND_BUFFER_SIZE,
    (UINTN)ResponseBuffer,
    FTPM_RESPONSE_BUFFER_SIZE
  );
  IDS_HDT_CONSOLE_PSP_TRACE ("\tAllocate CmdBuf:0x%x ResBuf:0x%x\n", CommandBuffer, ResponseBuffer);


  //
  // Install protocol to pass Ftpm info to rest of BIOS drivers
  //
  Handle = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall PspTis protocol\n");
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdPspFtpmProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mAmdPspFtpmProtocol
                  );

  if (EFI_ERROR (Status)) {
    return (Status);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("FtpmDxe Exit\n");
  AGESA_TESTPOINT (TpPspfTpmDxeExit, NULL);
  return Status;
}

