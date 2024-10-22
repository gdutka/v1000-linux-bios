/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _SOC_LOGICAL_ID_PPI_H_
#define _SOC_LOGICAL_ID_PPI_H_

#include "SocLogicalId.h"
//----------------------------------------------------
//
//  SOC Logical ID Service Functions
//
//-------------------------------------------------------

/*---------------------------------------------------------------------------------------*/
/**
 * This service retrieves the logical family and revision for the executing core.
 *
 * @param[out] LogicalId                      Pointer to the logical ID for the executing core.
 *
 * @retval EFI_SUCCESS                        The executing core's logical ID was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
typedef
EFI_STATUS
(EFIAPI *AMD_PEI_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_CURRENT_CORE) (
     OUT   SOC_LOGICAL_ID                                      *LogicalId
  );

/*---------------------------------------------------------------------------------------*/
/**
 * This service retrieves the logical family and revision for a particular processor.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 *
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[out] LogicalId                      Pointer to the logical ID for the given socket.
 *
 * @retval EFI_SUCCESS                        The given socket's logical ID was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
typedef
EFI_STATUS
(EFIAPI *AMD_PEI_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_GIVEN_SOCKET) (
     IN    UINTN                                               Socket,
     OUT   SOC_LOGICAL_ID                                      *LogicalId
  );

///
/// When installed, the SoC Logical ID PPI produces a collection of
/// services that provide the logical ID translation for installed
/// processors.
///
typedef struct _AMD_PEI_SOC_LOGICAL_ID_PPI {
  UINTN                                                   Revision;                   ///< Revision Number
  AMD_PEI_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_CURRENT_CORE   GetLogicalIdOnCurrentCore;  ///< Get logical revision of the executing core
  AMD_PEI_SOC_LOGICAL_ID_GET_LOGICAL_ID_ON_GIVEN_SOCKET   GetLogicalIdOnGivenSocket;  ///< Get logical revision of the given socket
} AMD_PEI_SOC_LOGICAL_ID_PPI;

extern EFI_GUID gAmdSocLogicalIdPpiGuid;

// Current PPI revision
#define SOC_LOGICAL_ID_PPI_REVISION   0x00

#endif

