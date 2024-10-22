/*
****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/


#ifndef _AMD_ERROR_LOG_DXE_H_
#define _AMD_ERROR_LOG_DXE_H_

//
// Functions Prototypes
//
EFI_STATUS
EFIAPI
AmdErrorLogDxeInit (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  );

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
  );

EFI_STATUS
EFIAPI
AmdErrorLogIpCompleteDxe (
  IN  DXE_AMD_ERROR_LOG_PROTOCOL   *This,
  IN CONST EFI_GUID            *SiliconDriverId
  );

EFI_STATUS
EFIAPI
AmdAquireErrorLogDxe (
  IN       DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  );

extern EFI_GUID gErrorLogHobGuid;

#endif // _AMD_ERROR_LOG_DXE_H_

