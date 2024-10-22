/** @file
  TIS (TPM Interface Specification) functions to access discrete TPM module.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/Tpm2TisCommLib.h>

#include <Protocol/Tpm2Interface.h>
#include <Protocol/SmmBase2.h>
#include <Library/AmdPspFtpmLib.h>
#include <Library/ChipsetConfigLib.h>
#include <ChipsetSetupConfig.h>
#include <Protocol/SetupUtility.h>
#include <Library/PcdLib.h>
#include <TpmPolicy.h>

#define SKIP_CONTROL_AREA_INIT                   ( 1 << 13 ) //BIT13  //Should define in TpmPolicy.h

TPM2_INTERFACE_PROTOCOL                   mTisInterface;
TPM2_INTERFACE_PROTOCOL                   mSmmTisInterface;
UINTN mCommandAddress, mResponseAddress;

EFI_STATUS
EFIAPI
TisDxeCommand (
  IN     TPM2_INTERFACE_PROTOCOL          *This,
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
EFIAPI
TisDxeRequestUseTpm (
  IN  TPM2_INTERFACE_PROTOCOL             *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
TisDxeSend (
  IN  TPM2_INTERFACE_PROTOCOL             *This,
  IN  UINT8                               *BufferIn,
  IN  UINT32                              SizeIn
  )
{
  EFI_STATUS Status;
  Status = FtpmSendCommand (
                            BufferIn,
                            SizeIn
                            );

  return (Status);
}

EFI_STATUS
EFIAPI
TisDxeReceive (
  IN     TPM2_INTERFACE_PROTOCOL          *This,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  )
{
  EFI_STATUS Status;
  Status = FtpmGetResponse (
                            BufferOut,
                            SizeOut
                          );
  return (Status);
}


/**
  The driver's entry point. It publishes EFI TrEE Protocol.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.
**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN    EFI_HANDLE                        ImageHandle,
  IN    EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_SMM_BASE2_PROTOCOL                  *SmmBase;
  BOOLEAN                                 InSmm;
  EFI_STATUS                              Status;
  EFI_HANDLE                              Handle;
  UINTN                                   iTPMStatus;
  UINT32                                  SetTpmPolicy;

  if (EFI_ERROR(FtpmGetInfo (&iTPMStatus))) {
    DEBUG ((EFI_D_ERROR, "TPM not detected!\n"));
    return EFI_SUCCESS;
  }

  if (PcdGet8 (PcdTpmType) != 2) {
    DEBUG((EFI_D_INFO, "SCU fTPM is disabled\n"));
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (!InSmm) {
    //
    // In boot service
    //
    mTisInterface.SummitCommand  = TisDxeCommand;
    mTisInterface.RequestUse     = TisDxeRequestUseTpm;
    mTisInterface.Send           = TisDxeSend;
    mTisInterface.Receive        = TisDxeReceive;
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                     &Handle,
                     &gTpm2InterfaceGuid,
                     EFI_NATIVE_INTERFACE,
                     &mTisInterface
                     );
    ASSERT_EFI_ERROR (Status);

    SetTpmPolicy = PcdGet32(PcdDxeTpmPolicy) | SKIP_INSTALL_TCG_ACPI_TABLE | SKIP_CONTROL_AREA_INIT;
    PcdSet32(PcdDxeTpmPolicy, SetTpmPolicy);
  }
  
  return EFI_SUCCESS;
}
