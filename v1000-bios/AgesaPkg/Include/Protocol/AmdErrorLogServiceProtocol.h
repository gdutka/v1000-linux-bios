/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_ERROR_LOG_SERVICE_PROTOCOL_H_
#define _AMD_ERROR_LOG_SERVICE_PROTOCOL_H_

#include <AmdErrorLog.h>

// Current protocol revision
#define AMD_ERROR_LOG_SERVICE_PROTOCOL_REVISION   0x00

///
/// Forward declaration for the DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL
///
typedef struct _DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL;

//
// Protocol Definitions
//
typedef
EFI_STATUS
(EFIAPI * AMD_AQUIRE_ERROR_LOG_DXE) (
  IN       DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL   *This,
  OUT      ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  );

//
// DXE protocol prototype
//
struct _DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL {
  UINTN                          Revision;               ///< Revision Number
  AMD_AQUIRE_ERROR_LOG_DXE       AmdAquireErrorLogDxe ;
};

extern EFI_GUID gAmdErrorLogServiceProtocolGuid;
extern EFI_GUID gAmdErrorLogFullProtocolGuid;
extern EFI_GUID gAmdErrorLogAvailableProtocolGuid;


#endif //_AMD_ERROR_LOG_SERVICE_PROTOCOL_H_

