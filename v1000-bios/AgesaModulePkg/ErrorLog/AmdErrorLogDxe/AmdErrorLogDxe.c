/*
****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/


#include <Filecode.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/AmdErrorLogLib.h>
#include <Protocol/AmdErrorLogProtocol.h>
#include <Protocol/AmdErrorLogServiceProtocol.h>
#include "AmdErrorLogDxe.h"

#define FILECODE ERRORLOG_AMDERRORLOGDXE_AMDERRORLOGDXE_FILECODE

DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL mDxeAmdErrorLogServicesProtocol = {
  AMD_ERROR_LOG_SERVICE_PROTOCOL_REVISION,
  AmdAquireErrorLogDxe,
};

DXE_AMD_ERROR_LOG_PROTOCOL mDxeAmdErrorLogProtocol = {
  AMD_ERROR_LOG_PROTOCOL_REVISION,
  AmdErrorLogDxe,
  AmdErrorLogIpCompleteDxe,
};

extern  EFI_BOOT_SERVICES       *gBS;

//
// Driver Global Data
//
STATIC AMD_ERROR_BUFFER *ErrorLogBuffer;


/*---------------------------------------------------------------------------------------*/
/**
 *
 * This function logs AGESA errors into the Error log.
 *
 * It will put the information in a circular buffer consisting of 128 such log
 * entries. If the buffer gets full, then the next Error log entry will be written
 * over the oldest Error log entry.
 *
 * @param[in]   This         Error log service instance.
 * @param[in]   ErrorClass   The severity of the Erorr, its associated AGESA_STATUS.
 * @param[in]   ErrorInfo    Uniquely identifies the error.
 * @param[in]   DataParam1   Error specific additional data
 * @param[in]   DataParam2   Error specific additional data
 * @param[in]   DataParam3   Error specific additional data
 * @param[in]   DataParam4   Error specific additional data
 *
 */
EFI_STATUS
EFIAPI
AmdErrorLogDxe (
  IN  DXE_AMD_ERROR_LOG_PROTOCOL   *This,
  IN  AMD_STATUS ErrorClass,
  IN  UINT32 ErrorInfo,
  IN  UINT32 DataParam1,
  IN  UINT32 DataParam2,
  IN  UINT32 DataParam3,
  IN  UINT32 DataParam4
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  EFI_HANDLE       Handle;
  AMD_ERROR_ENTRY  AmdErrorEntry;

  Handle = NULL;

  AmdErrorEntry.ErrorClass = ErrorClass;
  AmdErrorEntry.ErrorInfo  = ErrorInfo;
  AmdErrorEntry.DataParam1 = DataParam1;
  AmdErrorEntry.DataParam2 = DataParam2;
  AmdErrorEntry.DataParam3 = DataParam3;
  AmdErrorEntry.DataParam4 = DataParam4;

  AddErrorLog (ErrorLogBuffer, &AmdErrorEntry);

  if (ErrorLogBuffer->Count == TOTAL_ERROR_LOG_BUFFERS) {
    // Publish Error Log Full Protocol
    Status = gBS->InstallProtocolInterface (
              &Handle,
              &gAmdErrorLogFullProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }

  return Status;
}

EFI_STATUS
EFIAPI
AmdErrorLogIpCompleteDxe (
  IN       DXE_AMD_ERROR_LOG_PROTOCOL   *This,
  IN CONST EFI_GUID                     *SiliconDriverId
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    Handle;

  Handle = NULL;

  // check here if all IP drivers complete before publish Error Log Available Protocol

  // Publish Error Log Available Protocol
  Status = gBS->InstallProtocolInterface (
            &Handle,
            &gAmdErrorLogAvailableProtocolGuid,
            EFI_NATIVE_INTERFACE,
            NULL
            );

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
AmdAquireErrorLogDxe (
  IN       DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;

  AquireErrorLog (ErrorLogBuffer, ErrorLogDataPtr);

  return Status;
}

/*********************************************************************************
 * Name: AmdErrorLogDxeInit
 *
 * Description
 *   Entry point of the AMD Error Log DXE driver
 *   Perform the configuration init, resource reservation, early post init
 *   and install all the supported protocol
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
EFIAPI
AmdErrorLogDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  EFI_HANDLE          Handle;
  AMD_ERROR_LOG_INFO_HOB *ErrorLogHob;

  DEBUG ((EFI_D_ERROR, "*****************************DXE Error Log Driver Entry*********************\n"));

  //
  // Initialize the configuration structure and private data area
  //

  //Get Error Log buffer from HOB
  ErrorLogHob = GetFirstGuidHob (&gErrorLogHobGuid);
  ErrorLogBuffer = &(ErrorLogHob->AmdErrorBuffer);

  Handle = ImageHandle;

  // Publish Error Log service Protocol For IP driver
  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gAmdErrorLogProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDxeAmdErrorLogProtocol
                );

  // Publish Error Log service Protocol for platform BIOS
  Status = gBS->InstallProtocolInterface (
                &Handle,
                &gAmdErrorLogServiceProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDxeAmdErrorLogServicesProtocol
                );

  // Publish Error Log Ready Protocol
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gAmdErrorLogReadyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );

  DEBUG ((EFI_D_ERROR, "*****************************DXE Error Log Driver Exit*********************\n"));

  return (Status);
}


