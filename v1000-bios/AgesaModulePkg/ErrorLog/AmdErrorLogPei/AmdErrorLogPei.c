/*
****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/
/*++
Module Name:

  AmdErrorLogPei.c
  Init Soc interface

Abstract:
--*/

#include <Filecode.h>
#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AmdErrorLogLib.h>
#include <Ppi/AmdErrorLogServicePpi.h>
#include <Ppi/AmdErrorLogPpi.h>
#include "AmdErrorLogPei.h"

#define FILECODE ERRORLOG_AMDERRORLOGPEI_AMDERRORLOGPEI_FILECODE

STATIC PEI_AMD_ERROR_LOG_READY_PPI mAmdErrorLogReadyPpi = {
  AMD_ERROR_LOG_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR mErrorLogReadyPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdErrorLogReadyPpiGuid,
  &mAmdErrorLogReadyPpi
};

STATIC PEI_AMD_ERROR_LOG_FULL_PPI mAmdErrorLogFullPpi = {
  AMD_ERROR_LOG_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR mErrorLogFullPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdErrorLogFullPpiGuid,
  &mAmdErrorLogFullPpi
};

STATIC PEI_AMD_ERROR_LOG_FULL_PPI mAmdErrorLogAvailablePpi = {
  AMD_ERROR_LOG_PPI_REVISION,
};

STATIC EFI_PEI_PPI_DESCRIPTOR mErrorLogAvailablePpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdErrorLogAvailablePpiGuid,
  &mAmdErrorLogAvailablePpi
};

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function logs AGESA Errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 16 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   PeiServices
 * @param[in]   ErrorClass   The severity of the Error, its associated AGESA_STATUS.
 * @param[in]   ErrorInfo    Uniquely identifies the Error.
 * @param[in]   DataParam1   Error specific additional data
 * @param[in]   DataParam2   Error specific additional data
 * @param[in]   DataParam3   Error specific additional data
 * @param[in]   DataParam4   Error specific additional data
 *
 */
EFI_STATUS
EFIAPI
AmdErrorLogPei (
  IN       PEI_AMD_ERROR_LOG_PPI   *This,
  IN       AMD_STATUS ErrorClass,
  IN       UINT32 ErrorInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  AMD_ERROR_ENTRY  AmdErrorEntry;
  ERROR_LOG_PPI_PRIVATE *Private;
  Private = (ERROR_LOG_PPI_PRIVATE*)This;

  AmdErrorEntry.ErrorClass = ErrorClass;
  AmdErrorEntry.ErrorInfo  = ErrorInfo;
  AmdErrorEntry.DataParam1 = DataParam1;
  AmdErrorEntry.DataParam2 = DataParam2;
  AmdErrorEntry.DataParam3 = DataParam3;
  AmdErrorEntry.DataParam4 = DataParam4;

  AddErrorLog (Private->ErrorLogBuffer, &AmdErrorEntry);
  
  if ((Private->ErrorLogBuffer)->Count == TOTAL_ERROR_LOG_BUFFERS) {
    // Publish Error Log Full Ppi
    Status = (*Private->PeiServices)->InstallPpi (
                                      (CONST EFI_PEI_SERVICES**)Private->PeiServices,
                                      &mErrorLogFullPpiList
                                      );
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function logs AGESA Errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 16 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   PeiServices
 * @param[in]   SiliconDriverId
 *
 */
EFI_STATUS
EFIAPI
AmdErrorLogIpCompletePei (
  IN       PEI_AMD_ERROR_LOG_PPI   *This,
  IN CONST EFI_GUID                *SiliconDriverId
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  ERROR_LOG_PPI_PRIVATE *Private;
  Private = (ERROR_LOG_PPI_PRIVATE*)This;

  // check here if all IP drivers complete before publish Error Log Available Ppi

  // Publish Error Log Available Ppi
  Status = (*Private->PeiServices)->InstallPpi (
                                    (CONST EFI_PEI_SERVICES**)Private->PeiServices,
                                    &mErrorLogAvailablePpiList
                                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function logs AGESA Errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 128 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   PeiServices
 * @param[out]  ErrorLogDataPtr
 *
 */
EFI_STATUS
EFIAPI
AmdAquireErrorLogPei (
  IN       PEI_AMD_ERROR_LOG_SERVICE_PPI   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  ERROR_LOG_SERVICE_PPI_PRIVATE *Private;
  Private = (ERROR_LOG_SERVICE_PPI_PRIVATE*)This;

  AquireErrorLog (Private->ErrorLogBuffer, ErrorLogDataPtr);
  //Reset Error Log buffer
  ErrorLogBufferInit (Private->ErrorLogBuffer);

  return Status;
}

/*++

Routine Description:

  Error Log Driver Entry. Initialize and publish Error Log and PPI

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
PeiAmdErrorInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  AMD_ERROR_LOG_INFO_HOB *ErrorLogHob;
  EFI_STATUS    Status = EFI_SUCCESS;
  ERROR_LOG_PPI_PRIVATE  *EventlogPrivate;
  ERROR_LOG_SERVICE_PPI_PRIVATE  *EventlogServicePrivate;
  AMD_ERROR_BUFFER *ErrorLogBuffer;
  EFI_PEI_PPI_DESCRIPTOR    *mErrorLogPpiList;
  EFI_PEI_PPI_DESCRIPTOR    *mErrorLogServicePpiList;

  DEBUG ((EFI_D_ERROR, "*****************************PEI Error Log Driver Entry*********************\n"));

  // Create Error Log GUID HOB to save Error Log buffer data
  Status = (*PeiServices)->CreateHob (
                             PeiServices,
                             EFI_HOB_TYPE_GUID_EXTENSION,
                             sizeof (AMD_ERROR_LOG_INFO_HOB),
                             (VOID**)&ErrorLogHob
                             );

  ASSERT_EFI_ERROR (Status);

  CopyMem (&ErrorLogHob->EfiHobGuidType.Name, &gErrorLogHobGuid, sizeof (EFI_GUID));
  ErrorLogBuffer = &(ErrorLogHob->AmdErrorBuffer);

  ErrorLogBufferInit (ErrorLogBuffer);


  // Publish Error Log Ppi for IP driver
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (ERROR_LOG_PPI_PRIVATE),
                             (VOID**)&EventlogPrivate
                             );
  ASSERT_EFI_ERROR (Status);

  EventlogPrivate->AmdErrorLogPpi.AmdErrorLogPei           = AmdErrorLogPei;
  EventlogPrivate->AmdErrorLogPpi.AmdErrorLogIpCompletePei = AmdErrorLogIpCompletePei;
  EventlogPrivate->PeiServices                             = (EFI_PEI_SERVICES **)PeiServices;
  EventlogPrivate->ErrorLogBuffer                          = ErrorLogBuffer;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&mErrorLogPpiList
                             );
  ASSERT_EFI_ERROR ( Status);

  mErrorLogPpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mErrorLogPpiList->Guid  = &gAmdErrorLogPpiGuid;
  mErrorLogPpiList->Ppi   = &(EventlogPrivate->AmdErrorLogPpi);

  Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               mErrorLogPpiList
                               );
  ASSERT_EFI_ERROR (Status);



  // Publish Error Log Service Ppi for Platform BIOS
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (ERROR_LOG_SERVICE_PPI_PRIVATE),
                             (VOID**)&EventlogServicePrivate
                             );
  ASSERT_EFI_ERROR (Status);

  EventlogServicePrivate->AmdErrorLogServicePpi.AmdAquireErrorLogPei = AmdAquireErrorLogPei;
  EventlogServicePrivate->PeiServices                                = (EFI_PEI_SERVICES **)PeiServices;
  EventlogServicePrivate->ErrorLogBuffer                             = ErrorLogBuffer;

  // Allocate memory for the PPI descriptor
  Status = (*PeiServices)->AllocatePool (
                             PeiServices,
                             sizeof (EFI_PEI_PPI_DESCRIPTOR),
                             (VOID **)&mErrorLogServicePpiList
                             );
  ASSERT_EFI_ERROR ( Status);

  mErrorLogServicePpiList->Flags = (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST);
  mErrorLogServicePpiList->Guid  = &gAmdErrorLogServicePpiGuid;
  mErrorLogServicePpiList->Ppi   = &(EventlogServicePrivate->AmdErrorLogServicePpi);

  Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               mErrorLogServicePpiList
                               );
  ASSERT_EFI_ERROR (Status);

  // Publish Error Log Ready Ppi
  Status = (*PeiServices)->InstallPpi (
                               PeiServices,
                               &mErrorLogReadyPpiList
                               );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR, "*****************************PEI Error Driver Exit*********************\n"));
  return (Status);
}


