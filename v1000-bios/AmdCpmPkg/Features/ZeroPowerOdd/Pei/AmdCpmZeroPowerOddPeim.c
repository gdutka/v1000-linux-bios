/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#include <AmdCpmPei.h>


EFI_STATUS
EFIAPI
AmdCpmZeroPowerOddPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR mCpmZeroPowerOddPeimNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCpmGpioInitFinishedPpiGuid,
  AmdCpmZeroPowerOddPeim
};

/*----------------------------------------------------------------------------------------*/
/**
 * Set the trigger level of GEVENT pin.
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
AmdCpmZeroPowerOddPeim (
  IN       EFI_PEI_SERVICES             **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN       VOID                         *Ppi
  )
{
  AMD_CPM_TABLE_PPI                   *CpmTablePpiPtr;
  AMD_CPM_MAIN_TABLE                  *MainTablePtr;
  AMD_CPM_ZERO_POWER_ODD_TABLE        *ZeroPowerOddTablePtr;
  EFI_STATUS                          Status;

  RECORD_TIME_PEI (BeginAmdCpmZeroPowerOddPeimDriver);

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

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpZeroPowerOddPeimDriverBegin);

  MainTablePtr                    = CpmTablePpiPtr->MainTablePtr;
  ZeroPowerOddTablePtr            = CpmTablePpiPtr->CommonFunction.GetTablePtr (CpmTablePpiPtr, CPM_SIGNATURE_ZERO_POWER_ODD);

  if (ZeroPowerOddTablePtr) {
    if ((MainTablePtr->ZeroPowerOddEn & BIT0) && (ZeroPowerOddTablePtr->EventSource1 == 0)) {
      CpmTablePpiPtr->CommonFunction.SetGeventSciTrig (
                                                      CpmTablePpiPtr,
                                                      ZeroPowerOddTablePtr->EventPin1,
                                                      CpmTablePpiPtr->CommonFunction.GetGevent (CpmTablePpiPtr, ZeroPowerOddTablePtr->EventPin1) ? 0 : 1
                                                      );
    }
    if ((MainTablePtr->ZeroPowerOddEn & BIT1) && (ZeroPowerOddTablePtr->EventSource2 == 0)) {
      CpmTablePpiPtr->CommonFunction.SetGeventSciTrig (
                                                      CpmTablePpiPtr,
                                                      ZeroPowerOddTablePtr->EventPin2,
                                                      CpmTablePpiPtr->CommonFunction.GetGevent (CpmTablePpiPtr, ZeroPowerOddTablePtr->EventPin2) ? 0 : 1
                                                      );
    }
  }

  CpmTablePpiPtr->CommonFunction.PostCode (CpmTpZeroPowerOddPeimDriverEnd);

  RECORD_TIME_PEI (EndAmdCpmZeroPowerOddPeimDriver);

  return Status;
}


/*----------------------------------------------------------------------------------------*/
/**
 * Entry point of the AMD CPM Zero Power Odd PEIM driver
 *
 * This function registers the function to set the trigger level of GEVENT pin.
 *
 * @param[in]     FileHandle     Pointer to the firmware file system header
 * @param[in]     PeiServices    Pointer to Pei Services
 *
 * @retval        EFI_SUCCESS    Module initialized successfully
 * @retval        EFI_ERROR      Initialization failed (see error for more details)
 */
EFI_STATUS
EFIAPI
AmdCpmZeroPowerOddPeimEntryPoint (
  IN        CPM_PEI_FILE_HANDLE         FileHandle,
  IN        CPM_PEI_SERVICES            **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).NotifyPpi (PeiServices, &mCpmZeroPowerOddPeimNotify);

  return EFI_SUCCESS;
}
