/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiPei.h>
#include <Ppi/AmdPspFtpmPpi.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/AmdPspRecoveryDetectPpi.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdPspCommonLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Library/AmdPspFtpmLib.h>
#include <Library/FchBaseLib.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPFTPMPEI_AMDPSPFTPMPEI_FILECODE

EFI_STATUS
EFIAPI
FtpmMemoryDiscoveredPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

EFI_STATUS
GetFtpmStatus (
  IN       PSP_FTPM_PPI         *This,
  IN OUT   UINTN                *FtpmStatus
  )
{
  return (FtpmGetInfo (FtpmStatus));
}

EFI_STATUS
SendFtpmCommand (
  IN     PSP_FTPM_PPI         *This,
  IN     VOID                 *CommandBuffer,
  IN     UINTN                 CommandSize
  )
{
  return FtpmSendCommand (
      CommandBuffer,
      CommandSize);
}

EFI_STATUS
GetFtpmResponse (
  IN       PSP_FTPM_PPI          *This,
  IN OUT   VOID                  *ResponseBuffer,
  IN OUT   UINTN                 *ResponseSize
  )
{

  EFI_STATUS Status;
  Status = FtpmGetResponse (
                            ResponseBuffer,
                            ResponseSize
                          );
  return (Status);
}

EFI_STATUS
ExecuteFtpmCommand (
  IN       PSP_FTPM_PPI         *This,
  IN       VOID                 *CommandBuffer,
  IN       UINTN                CommandSize,
  IN OUT   VOID                 *ResponseBuffer,
  IN OUT   UINTN                *ResponseSize
  )
{
  return FtpmExecuteCommand (
                              CommandBuffer,
                              CommandSize,
                              ResponseBuffer,
                              ResponseSize
                             );
}


STATIC PSP_FTPM_PPI mAmdPspFtpmPpi = {
  ExecuteFtpmCommand,
  GetFtpmStatus,
  SendFtpmCommand,
  GetFtpmResponse,
};

STATIC PSP_FTPM_FACTORY_RESET_PPI mAmdPspFtpmFactoryResetPpi = {
  0x00000001ul
};

STATIC EFI_PEI_PPI_DESCRIPTOR mAmdPspFtpmPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspFtpmPpiGuid,
  &mAmdPspFtpmPpi
};

STATIC EFI_PEI_PPI_DESCRIPTOR mAmdPspFtpmFactoryResetPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspFtpmFactoryResetPpiGuid,
  &mAmdPspFtpmFactoryResetPpi
};

EFI_PEI_NOTIFY_DESCRIPTOR   mMemoryDicoverPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  FtpmMemoryDiscoveredPpiCallback
};

STATIC AMD_PSP_RECOVERY_DETECT_PPI mPspRecoveryDetectPpi = {
  0x01
};

STATIC EFI_PEI_PPI_DESCRIPTOR mPspRecoveryDetectedPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspRecoveryDetectPpiGuid,
  &mPspRecoveryDetectPpi
};

EFI_STATUS
EFIAPI
FtpmMemoryDiscoveredPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_STATUS                  Status;
  EFI_PHYSICAL_ADDRESS        FtpmCommandBuffer;
  EFI_PHYSICAL_ADDRESS        FtpmResponseBuffer;
  UINT32                      Caps;
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.FtpmPei.MemoryDiscoveredPpiCallback\n");

  //If PSP feature turn off, exit the callback to avoid hang when swap part
  if ((PcdGetBool (PcdAmdPspEnable) == 0) ||
      (PcdGet8 (PcdAmdPspSystemTpmConfig) != SYSTEM_TPM_CONFIG_PSP_FTPM)) {
    return EFI_SUCCESS;
  }

  AGESA_TESTPOINT (TpPspfTpmPeiMemCallbackEntry, NULL);
  //Send PSP mailbox command to ensure Ctrl Area have been initialed
  Status = CheckFtpmCaps (&Caps);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Get fTPM cap fail\n");
    return Status;
  }
  //Supported & factory reset can't be set at the same time
  ASSERT (Caps  != (PSP_CAP_TPM_SUPPORTED | PSP_CAP_TPM_REQ_FACTORY_RESET));

  if (Caps == PSP_CAP_TPM_SUPPORTED) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Allocate memory for FTPM\n");
    Status = (*PeiServices)->AllocatePages (
                                (CONST EFI_PEI_SERVICES**)PeiServices,
                                EfiBootServicesData,
                                EFI_SIZE_TO_PAGES (FTPM_COMMAND_BUFFER_SIZE + \
                                                    FTPM_RESPONSE_BUFFER_SIZE),
                                &FtpmCommandBuffer
                                );

    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Allocate Memory fail\n");
      return Status;
    }

    FtpmResponseBuffer      = FtpmCommandBuffer + FTPM_COMMAND_BUFFER_SIZE;

    SetMem ((VOID *) (UINTN) FtpmCommandBuffer, FTPM_COMMAND_BUFFER_SIZE + FTPM_RESPONSE_BUFFER_SIZE, 0);

    IDS_HDT_CONSOLE_PSP_TRACE ("AssignMemory for FTPM\n");
    Status = FtpmAssignMemory (
      (UINTN)FtpmCommandBuffer,
      FTPM_COMMAND_BUFFER_SIZE,
      (UINTN)FtpmResponseBuffer,
      FTPM_RESPONSE_BUFFER_SIZE
    );

    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("FtpmAssignMemory fail\n");
      return Status;
    }

    IDS_HDT_CONSOLE_PSP_TRACE ("\tAllocate CmdBuf:0x%lx ResBuf:0x%lx\n", FtpmCommandBuffer, FtpmResponseBuffer);

    IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall PspTis PPI\n");
    Status = (**PeiServices).InstallPpi ((CONST EFI_PEI_SERVICES**)PeiServices, &mAmdPspFtpmPpiList);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("InstallPpi fail\n");
    }
  } else if (Caps == PSP_CAP_TPM_REQ_FACTORY_RESET) {
    IDS_HDT_CONSOLE_PSP_TRACE ("FTPM FACTORY_RESET detected\n");
    //Install a PPI to notify SBIOS
    IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall fTPM factory reset PPI\n");
    Status = (**PeiServices).InstallPpi ((CONST EFI_PEI_SERVICES**)PeiServices, &mAmdPspFtpmFactoryResetPpiList);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("InstallPpi fail\n");
    }
  } else if (Caps == PSP_CAP_FTPM_NEED_RECOVERY) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Ftpm Recovery Flag Detected\n");
    Status = (**PeiServices).InstallPpi ((CONST EFI_PEI_SERVICES**)PeiServices, &mPspRecoveryDetectedPpiList);
    ASSERT (Status == EFI_SUCCESS);
  }
  AGESA_TESTPOINT (TpPspfTpmPeiMemCallbackExit, NULL);
  return Status;
}


/*++

Routine Description:

  PSP Driver Entry. Initialize PSP device and report DRAM info to PSP once found

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
AmdPspFtpmPeiDriverEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  AGESA_TESTPOINT (TpPspfTpmPeiEntry, NULL);
  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresent () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == 0) ||
      (PcdGet8 (PcdAmdPspSystemTpmConfig) != SYSTEM_TPM_CONFIG_PSP_FTPM)) {
    return EFI_SUCCESS;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.FtpmPei Enter\n");
  //
  // Check if Boot mode is boot then hook for MemoryDiscovery protocol
  //
  if ((FchReadSleepType () != 3) && (AmdCapsuleGetStatus () == FALSE)) {
    Status = (**PeiServices).NotifyPpi (PeiServices, &mMemoryDicoverPpiCallback);
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("FtpmPei Exit\n");
  AGESA_TESTPOINT (TpPspfTpmPeiExit, NULL);

  return Status;
}


