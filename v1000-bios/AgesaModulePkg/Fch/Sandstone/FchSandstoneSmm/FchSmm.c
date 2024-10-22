/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "FchSmm.h"
#define FILECODE UEFI_SMM_FCHSMM_FCHSMM_FILECODE

#include <Library/BaseMemoryLib.h>

//FCH_INIT_PROTOCOL                   gFchInitInSmm;
FCH_DATA_BLOCK                      gFchDataInSmm;
UINT8                               *mFchPciIrqRoutingTable;

EFI_STATUS
FchSmmRegistrationCenter (
  VOID
  )
{
  EFI_STATUS     Status;

  Status = FchSmmRegisterSwSmi ();
  Status = FchSmmRegisterPowerButtonSmi ();
  Status = FchSmmRegisterPeriodicTimerSmi ();
  Status = FchSmmRegisterGpiSmi ();
  Status = FchSmmRegisterSxSmi ();
  Status = FchSmmRegisterIoTrapSmi ();
  return Status;
}

/*********************************************************************************
 * Name: FchSmmEntryPoint
 *
 * Description
 *   Entry point of the AMD FCH SMM driver
 *   copy FCH_INIT_PROTOCOL to SMM, and register Fch Smm callbacks
 *
 * Input
 *   ImageHandle : EFI Image Handle for the DXE driver
 *   SystemTable : pointer to the EFI system table
 *
 * Output
 *   EFI_SUCCESS : Module initialized successfully
 *   EFI_ERROR   : Initialization failed (see error for more details)
 *
 *********************************************************************************/
EFI_STATUS
FchSmmEntryPoint (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )

{
  FCH_INIT_PROTOCOL           *pFchCimxInitProtocol;
  VOID                        *Registration;
  EFI_STATUS                  Status;

//[-start-240426-IB18160212-modify-]//
  AGESA_TESTPOINT (TpFchSmmEntry, NULL);
  //
  // Initialize global variables
  //
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    0x100,
                    (VOID**)&mFchPciIrqRoutingTable
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (
                  &gFchInitProtocolGuid,
                  NULL,
                  (VOID**)&pFchCimxInitProtocol
                  );
  if (!EFI_ERROR (Status)) {
    CopyMem (
      &gFchDataInSmm,
      pFchCimxInitProtocol->FchPolicy,
      sizeof (FCH_DATA_BLOCK)
      );
  } else {
    return EFI_ABORTED;
  }
//[end-240426-IB18160212-modify]//

  //Copy OEM table pointed by Fch parameter
  Status = FchSmmCopyOemTable ();

  Status = FchSmmRegistrationCenter ();

  gSmst->SmmRegisterProtocolNotify (
           &gEdkiiSmmReadyToBootProtocolGuid,
           FchSmmRTBCallBack,
           &Registration
           );

  AGESA_TESTPOINT (TpFchSmmExit, NULL);
  return Status;
}

EFI_STATUS
EFIAPI
FchSmmRTBCallBack (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  UINT32          VidDid;
  UINT32          XhciBar0;

  FchSmnRead (0, FCH_SS_USB_SMN_PCICFG + 0x00, &VidDid, NULL);
  if (VidDid != 0xFFFFFFFF) {
    FchSmnRead (0, FCH_SS_USB_SMN_PCICFG + 0x10, &XhciBar0, NULL);
    XhciBar0 &= 0xFFFFFFF0;
    gFchDataInSmm.Usb.Usb0Bar = XhciBar0;
  } else {
    gFchDataInSmm.Usb.Usb0Bar = 0xFFFFFFFF;
 }

  FchSmnRead (0, FCH_SS_USB1_SMN_PCICFG + 0x00, &VidDid, NULL);
  if (VidDid != 0xFFFFFFFF) {
    FchSmnRead (0, FCH_SS_USB1_SMN_PCICFG + 0x10, &XhciBar0, NULL);
    XhciBar0 &= 0xFFFFFFF0;
    gFchDataInSmm.Usb.Usb1Bar = XhciBar0;
  } else {
    gFchDataInSmm.Usb.Usb1Bar = 0xFFFFFFFF;
 }

  return EFI_SUCCESS;
}

EFI_STATUS
FchSmmCopyOemTable (
  VOID
  )
{
  VOID                        *OemProgTblPtr;
  UINTN                       OemProgTblSize;
  UINT8                       i;
  ACPI_REG_WRITE              *pAcpiTbl;
  EFI_STATUS                  Status;

  Status = EFI_NOT_FOUND;                            //no table found by default
  //
  // Copy gFchDataInSmm.HwAcpi.OemProgrammingTablePtr to SMM space for SMM handler to access
  //
  OemProgTblSize = 0;
  i=0;
  pAcpiTbl = gFchDataInSmm.HwAcpi.OemProgrammingTablePtr;
  if (pAcpiTbl != NULL) {
    if ((pAcpiTbl->MmioReg == 0) && (pAcpiTbl->MmioBase == 0) && (pAcpiTbl->DataAndMask == 0xB0) && (pAcpiTbl->DataOrMask == 0xAC)) {
      // Signature Checking
      pAcpiTbl++;
      OemProgTblSize = sizeof (ACPI_REG_WRITE);
      for ( i = 1; pAcpiTbl->MmioBase < 0x1D; i++ ) {
        pAcpiTbl++;
      }
      OemProgTblSize += i*(sizeof (ACPI_REG_WRITE));
    }
    if (OemProgTblSize != 0) {
      Status = gSmst->SmmAllocatePool (
                        EfiRuntimeServicesData,
                        OemProgTblSize,
                        &OemProgTblPtr
                        );
      if (!EFI_ERROR (Status)) {
        CopyMem (
          OemProgTblPtr,
          gFchDataInSmm.HwAcpi.OemProgrammingTablePtr,
          OemProgTblSize
          );

        gFchDataInSmm.HwAcpi.OemProgrammingTablePtr = OemProgTblPtr;  //Update pointer in SMM
      }
    }
  }
  return Status;
}

