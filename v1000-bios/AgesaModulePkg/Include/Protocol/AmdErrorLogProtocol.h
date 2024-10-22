/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_ERROR_LOG_PROTOCOL_H_
#define _AMD_ERROR_LOG_PROTOCOL_H_

#include <AmdErrorLog.h>

// Current protocol revision
#define AMD_ERROR_LOG_PROTOCOL_REVISION   0x00

///
/// Forward declaration for the DXE_AMD_ERROR_LOG_SERVICES_PROTOCOL
///
typedef struct _DXE_AMD_ERROR_LOG_PROTOCOL DXE_AMD_ERROR_LOG_PROTOCOL;

//
// Protocol Definitions
//

typedef
EFI_STATUS
(EFIAPI * AMD_ERROR_LOG_DXE) (
  IN  DXE_AMD_ERROR_LOG_PROTOCOL   *This,
  IN  AMD_STATUS ErrorClass,
  IN  UINT32 ErrorInfo,
  IN  UINT32 DataParam1,
  IN  UINT32 DataParam2,
  IN  UINT32 DataParam3,
  IN  UINT32 DataParam4
  );

typedef
EFI_STATUS
(EFIAPI * AMD_ERROR_LOG_IP_COMPLETE_DXE) (
  IN  DXE_AMD_ERROR_LOG_PROTOCOL   *This,
  IN CONST EFI_GUID            *SiliconDriverId
  );

//
// DXE protocol prototype
//
struct _DXE_AMD_ERROR_LOG_PROTOCOL {
  UINTN                          Revision;               ///< Revision Number
  AMD_ERROR_LOG_DXE              AmdErrorLogDxe;
  AMD_ERROR_LOG_IP_COMPLETE_DXE  AmdErrorLogIpCompleteDxe;
};

extern EFI_GUID gAmdErrorLogReadyProtocolGuid;
extern EFI_GUID gAmdErrorLogProtocolGuid;

#endif //_AMD_ERROR_LOG_PROTOCOL_H_

