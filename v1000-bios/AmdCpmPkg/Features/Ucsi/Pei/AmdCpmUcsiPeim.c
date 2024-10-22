/*****************************************************************************
 *
 * Copyright (C) 2018-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>


EFI_STATUS
EFIAPI
AmdCpmUcsiPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmUcsiPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmUcsiPeim
};

/*----------------------------------------------------------------------------------------*/
/**
 *
 * @param[in]     PeiServices       Pointer to Pei Services
 * @param[in]     NotifyDescriptor  The descriptor for the notification event
 * @param[in]     Ppi               Pointer to the PPI in question.
 *
 * @retval        EFI_SUCCESS       Module initialized successfully
 * @retval        EFI_ERROR         Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmUcsiPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  EFI_STATUS                            Status;
  AMD_CPM_TABLE_PPI                     *CpmTablePpiPtr;
  AMD_CPM_MAIN_TABLE                    *MainTablePtr;
  AMD_CPM_UCSI_TABLE                    *CpmUcsiTablePtr;
  UINT8                                 Value8;
  UINT8                                 Offset;

  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeim-Start\n"));
  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeim-From-"));
  DEBUG((DEBUG_INFO, "AmdCpmUcsiPeimEntryPoint-NotifyPpi-gAmdCpmGpioInitFinishedPpiGuid\n"));
  Value8 = 0;
  Offset = 0;

  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeim-LocatePpi-gAmdCpmTablePpiGuid\n"));
  Status = (*PeiServices)->LocatePpi (  (CPM_PEI_SERVICES**)PeiServices,
                                        &gAmdCpmTablePpiGuid,
                                        0,
                                        NULL,
                                        (VOID**)&CpmTablePpiPtr );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeim-End-1-Status = %r\n", Status));
    return Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpUcsiPeimDriverBegin);
  MainTablePtr    = CpmTablePpiPtr->MainTablePtr;
  CpmUcsiTablePtr = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_UCSI);

  if (CpmUcsiTablePtr && (MainTablePtr->UcsiEn == 1)) {
    if (CpmUcsiTablePtr->EcUcsiLocation == 0) {
      Offset = CpmUcsiTablePtr->EcVersionLocation;
      CpmTablePpiPtr->CommonFunction.KbcRead (CFG_CPM_ECRAM_BASE_ADDR, 0x80, &Offset, &Value8); // CFG_CPM_ECRAM_BASE_ADDR = PcdGet16 (EcRamBaseAddr)
      CpmUcsiTablePtr->Reserved[0] = Value8;
      Offset = CpmUcsiTablePtr->EcVersionLocation + 1;
      CpmTablePpiPtr->CommonFunction.KbcRead (CFG_CPM_ECRAM_BASE_ADDR, 0x80, &Offset, &Value8); // CFG_CPM_ECRAM_BASE_ADDR = PcdGet16 (EcRamBaseAddr)
      CpmUcsiTablePtr->Reserved[1] = Value8;
    } else {
      CpmUcsiTablePtr->Reserved[0] = CpmTablePpiPtr->CommonFunction.MmioRead8 (CpmUcsiTablePtr->EcUcsiLocation + (UINT32)(CpmUcsiTablePtr->EcVersionLocation));
      CpmUcsiTablePtr->Reserved[1] = CpmTablePpiPtr->CommonFunction.MmioRead8 (CpmUcsiTablePtr->EcUcsiLocation + (UINT32)(CpmUcsiTablePtr->EcVersionLocation + 1));
    }
  }

  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeim-End-2-Status = %r\n", Status));
  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpUcsiPeimDriverEnd);
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM UCSI PEIM driver
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmUcsiPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                            Status;
  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeimEntryPoint-Start\n"));

  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeimEntryPoint-NotifyPpi-gAmdCpmGpioInitFinishedPpiGuid\n"));
  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeimEntryPoint-For-AmdCpmUcsiPeim\n"));
  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmUcsiPeimNotify);

  DEBUG((DEBUG_INFO, "FEA-PEI-AmdCpmUcsiPeimEntryPoint-End\n"));
  return Status;
}
