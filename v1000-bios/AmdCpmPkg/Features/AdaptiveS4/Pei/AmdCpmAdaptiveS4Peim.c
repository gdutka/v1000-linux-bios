/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>


EFI_STATUS
EFIAPI
AmdCpmAdaptiveS4Peim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

#ifdef AGESA_V9
STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmAdaptiveS4PeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmAdaptiveS4Peim
};
#endif

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Adaptive S4 PEIM driver
 *
 * This function will set the flag if the system wakes up from Adaptive S4 and
 * RTC method is used.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmAdaptiveS4Peim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
  AMD_CPM_MAIN_TABLE                  *MainTablePtr;
  AMD_CPM_ADAPTIVE_S4_TABLE           *AdaptiveS4TablePtr;
  EFI_STATUS                          Status;
  UINT32                              Offset;
  UINT8                               BufferSignature0;
  UINT8                               BufferSignature1;
  UINT8                               BufferSignature2;
  UINT8                               BufferSignature3;

  RECORD_TIME_PEI (BeginAmdCpmAdaptiveS4PeimDriver);

  Status = (*PeiServices)->LocatePpi (
                                      PeiServices,
                                      &gAmdCpmTablePpiGuid,
                                      0,
                                      NULL,
                                      (VOID**)&CpmTablePpiPtr
                                      );
  if (EFI_ERROR (Status)) {
    return  Status;
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpAdaptiveS4PeimDriverBegin);

  MainTablePtr                  = CpmTablePpiPtr->MainTablePtr;
  AdaptiveS4TablePtr            = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_ADAPTIVE_S4);

  if (AdaptiveS4TablePtr) {
    if (MainTablePtr->AdaptiveS4En == 2) {
      if (CpmTablePpiPtr->CommonFunction.GetBootMode () == CPM_BOOT_MODE_S4) {
        // RTC Method
        Offset = MainTablePtr->AcpiMemIoBaseAddr + 0x100 * AdaptiveS4TablePtr->BufferType + AdaptiveS4TablePtr->BufferOffset;
        BufferSignature0 = CpmTablePpiPtr->CommonFunction.MmioRead8 (Offset + 0x00);
        BufferSignature1 = CpmTablePpiPtr->CommonFunction.MmioRead8 (Offset + 0x01);
        BufferSignature2 = CpmTablePpiPtr->CommonFunction.MmioRead8 (Offset + 0x02);
        BufferSignature3 = CpmTablePpiPtr->CommonFunction.MmioRead8 (Offset + 0x03);
        if (CpmTablePpiPtr->CommonFunction.IsRtcWakeup (CpmTablePpiPtr) && BufferSignature3 == 'A' && BufferSignature2 == 'S' && BufferSignature1 == '3' && (BufferSignature0 == 'Y' || BufferSignature0 == 'P')) {
          CpmTablePpiPtr->CommonFunction.MmioOr8 (Offset + 0x08, BIT1);
          CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x00, 'P');
        } else {
          CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x00, 0x00);
          CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x01, 0x00);
          CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x02, 0x00);
          CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x03, 0x00);
          CpmTablePpiPtr->CommonFunction.MmioAnd8 (Offset + 0x08, 0xFD);
        }
      } else if (CpmTablePpiPtr->CommonFunction.GetBootMode () == CPM_BOOT_MODE_S3 && CpmTablePpiPtr->CommonFunction.IsRtcWakeup (CpmTablePpiPtr)) {
      } else {
        // RTC Method
        Offset = MainTablePtr->AcpiMemIoBaseAddr + 0x100 * AdaptiveS4TablePtr->BufferType + AdaptiveS4TablePtr->BufferOffset;
        CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x00, 0x00);
        CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x01, 0x00);
        CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x02, 0x00);
        CpmTablePpiPtr->CommonFunction.MmioWrite8 (Offset + 0x03, 0x00);
        CpmTablePpiPtr->CommonFunction.MmioAnd8 (Offset + 0x08, 0xFD);
      }
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpAdaptiveS4PeimDriverEnd);

  RECORD_TIME_PEI (EndAmdCpmAdaptiveS4PeimDriver);

  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Adaptive S4 PEIM driver
 *
 * This function registers the functions to set HotPlug attribute in PCIe Topology Table
 * if Express Card supports.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */

EFI_STATUS
EFIAPI
AmdCpmAdaptiveS4PeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS                      Status;
#ifndef AGESA_V9
  EFI_PEI_NOTIFY_DESCRIPTOR       NotifyDescriptor;
  VOID                            *Ppi = NULL;
#endif

#ifdef AGESA_V9
  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmAdaptiveS4PeimNotify);
#else
  Status = AmdCpmAdaptiveS4Peim ((EFI_PEI_SERVICES**)PeiServices,
                               &NotifyDescriptor,
                               Ppi);
#endif
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return Status;
}
