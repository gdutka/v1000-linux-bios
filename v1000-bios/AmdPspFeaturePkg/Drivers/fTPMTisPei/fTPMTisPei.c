/** @file
  TIS (TPM Interface Specification) functions to access discrete TPM module.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseLib.h>
//#include <Library/Tpm2TisCommLib.h>

#include <Ppi/Tpm2InterfacePei.h>
#include <Ppi/AmdPspFtpmPpi.h>
#include <Library/AmdPspFtpmLib.h>
#include <Library/ChipsetConfigLib.h>
#include <ChipsetSetupConfig.h>
#include <Library/PcdLib.h>
#include <TpmPolicy.h>
#include <Guid/TpmInstance.h>
#include <Guid/AmdTpmDeviceFtpm.h>

EFI_STATUS
EFIAPI
PspFtpmPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );
#define CHANGE_STARTUP_MP                        ( 1 << 4  ) //BIT4  //Should define in TpmPolicy.h
EFI_PEI_NOTIFY_DESCRIPTOR   mPspFtpmPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPspFtpmPpiGuid,
  PspFtpmPpiCallback
};

EFI_STATUS
TisPeiCommand ( 
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  )
{
  return FtpmExecuteCommand (
    BufferIn,
    SizeIn,
    BufferOut,
    SizeOut
    );
}

EFI_STATUS
TisPeiRequestUseTpm (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This
  )
{
  UINTN              iTPMStatus;
  return FtpmGetInfo (&iTPMStatus);
}

EFI_STATUS
TisPeiSend (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn
  )
{
  EFI_STATUS           Status;
  Status = FtpmSendCommand (
                          BufferIn,
                          SizeIn
                          );
  return Status;
}

EFI_STATUS
TisPeiReceive (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  )
{
  EFI_STATUS Status;
  Status = FtpmGetResponse (
                            BufferOut,
                            SizeOut
                          );
  return Status;
}

PEI_TPM2_INTERFACE_PPI         mPeiTpm2InterfacePpi = {
  TisPeiCommand,
  TisPeiRequestUseTpm,
  TisPeiSend,
  TisPeiReceive
};

EFI_PEI_PPI_DESCRIPTOR   mInstallPeiTpm2InterfacePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpm2InterfacePpiGuid,
  &mPeiTpm2InterfacePpi
};

EFI_STATUS
TpmAutoDetectFtpm (
  IN EFI_PEI_SERVICES                  **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR         *NotifyDescriptor,
  IN VOID                              *Ppi
  )
{
  UINTN                                Size;
  DEBUG ((EFI_D_ERROR, "TpmAutoDetectFtpm\n"));

  Size = sizeof (EFI_GUID);  
  PcdSetPtr (PcdTpmInstanceGuid, &Size, &gEfiTpmDeviceInstanceTpm20FtpmGuid);
  
  return EFI_SUCCESS;
}

EFI_PEI_NOTIFY_DESCRIPTOR mTpmDeviceSeleted[] = {
  {(EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiTpmDeviceSelectedGuid,
  TpmAutoDetectFtpm}
};

/**
  Entry point of this module.

  @param[in] FileHandle   Handle of the file being invoked.
  @param[in] PeiServices  Describes the list of possible PEI Services.

  @return Status.

**/
EFI_STATUS
EFIAPI
PspFtpmPpiCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  EFI_STATUS                       Status;
  UINTN                            iTPMStatus;

  DEBUG ((EFI_D_ERROR, "PspFtpmPpiCallback\n"));

  if (PcdGet8 (PcdTpmType) != 2) {
      return EFI_SUCCESS;
  }

  if (EFI_ERROR (FtpmGetInfo (&iTPMStatus))) {
    DEBUG ((EFI_D_ERROR, "TPM not detected!\n"));
    return EFI_SUCCESS;
  }

//SetTpmPolicy = PcdGet32(PcdPeiTpmPolicy) | TPM2_STARTUP_IN_MP;
//PcdSet32(PcdPeiTpmPolicy, SetTpmPolicy);
  
  //
  // Install PPI
  //
  Status = PeiServicesInstallPpi (&mInstallPeiTpm2InterfacePpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Notify PPI for TrEEConfigPeim of Tpm Auto Detect feature
  //
  Status = PeiServicesNotifyPpi (mTpmDeviceSeleted);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PeimEntry (
  IN       EFI_PEI_FILE_HANDLE            FileHandle,
  IN CONST EFI_PEI_SERVICES               **PeiServices
  )
{
  EFI_STATUS                       Status;

  Status = PeiServicesRegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    Status = (**PeiServices).NotifyPpi (PeiServices, &mPspFtpmPpiCallback);
  }
  return Status;
}
