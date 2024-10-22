/*
****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*/

#ifndef _AMD_ERROR_LOG_PEI_H_
#define _AMD_ERROR_LOG_PEI_H_

//Publich AGESA interface
typedef struct _ERROR_LOG_SERVICE_PPI_PRIVATE
{
    PEI_AMD_ERROR_LOG_SERVICE_PPI    AmdErrorLogServicePpi;  ///< Pointer to Error Log Service Ppi
    EFI_PEI_SERVICES                 **PeiServices;          ///< Pointer to PeiServices
    AMD_ERROR_BUFFER                 *ErrorLogBuffer;        ///< Pointer to Internal error log buffer
} ERROR_LOG_SERVICE_PPI_PRIVATE;

//AGESA private interface 
typedef struct _ERROR_LOG_PPI_PRIVATE
{
    PEI_AMD_ERROR_LOG_PPI            AmdErrorLogPpi;         ///< Pointer to Error Log Ppi
    EFI_PEI_SERVICES                 **PeiServices;          ///< Pointer to PeiServices
    AMD_ERROR_BUFFER                 *ErrorLogBuffer;        ///< Pointer to Internal error log buffer
} ERROR_LOG_PPI_PRIVATE;

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
  );

EFI_STATUS
EFIAPI
AmdErrorLogIpCompletePei (
  IN       PEI_AMD_ERROR_LOG_PPI   *This,
  IN CONST EFI_GUID            *SiliconDriverId
  );

EFI_STATUS
EFIAPI
AmdAquireErrorLogPei (
  IN       PEI_AMD_ERROR_LOG_SERVICE_PPI   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  );

extern EFI_GUID gErrorLogHobGuid;

#endif // _AMD_SOC_AM4_PEI_H_


