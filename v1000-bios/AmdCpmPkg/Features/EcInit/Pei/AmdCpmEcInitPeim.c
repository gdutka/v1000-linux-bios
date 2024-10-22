/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>


EFI_STATUS
EFIAPI
AmdCpmEcInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmEcInitPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmEcInitPeim
};

EFI_STATUS
EFIAPI
AmdCpmEcInitPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS              Status;
  AMD_CPM_EC_INIT_TABLE   *EcInitTablePtr;
  AMD_CPM_MAIN_TABLE      *MainTablePtr;
  AMD_CPM_TABLE_PPI       *CpmTablePpiPtr;
  UINT8                   Data;

  Status = (*PeiServices)->LocatePpi (
                                      (CPM_PEI_SERVICES**)PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
  if (EFI_ERROR (Status)) {
    return  Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpEcInitPeimDriverBegin);

  MainTablePtr    = CpmTablePpiPtr->MainTablePtr;
  EcInitTablePtr  = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_EC_INIT);

  if (EcInitTablePtr) {

    Data = (UINT8) MainTablePtr->Ec.Config.AcDcSwitchEn << 7;
    Status = CpmTablePpiPtr->CommonFunction.KbcWrite (0x60, 0xEA, &Data);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (MainTablePtr->Ec.Config.AcDcSwitchEn) {
      Data = EcInitTablePtr->AcTimer;
      Status = CpmTablePpiPtr->CommonFunction.KbcWrite (0x60, 0xEB, &Data);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Data = EcInitTablePtr->DcTimer;
      Status = CpmTablePpiPtr->CommonFunction.KbcWrite (0x60, 0xEC, &Data);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    if (MainTablePtr->Ec.Config.ColdBootCyclingEn) {
      Data = EcInitTablePtr->OnTimer;
      Status = CpmTablePpiPtr->CommonFunction.KbcWrite (0x60, 0xEF, &Data);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    Data = (UINT8)MainTablePtr->Ec.Config.ColdBootCyclingEn;
    Status = CpmTablePpiPtr->CommonFunction.KbcWrite (0x60, 0xED, &Data);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (MainTablePtr->Ec.Config.S5PlusEn) {
    if (CpmTablePpiPtr->CommonFunction.GetStrap (CpmTablePpiPtr) & BIT2) {
      Status = CpmTablePpiPtr->CommonFunction.KbcWrite (0x60, 0xA2, NULL);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpEcInitPeimDriverEnd);

  return Status;
}

EFI_STATUS
EFIAPI
AmdCpmEcInitPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmEcInitPeimNotify);

  return EFI_SUCCESS;
}
