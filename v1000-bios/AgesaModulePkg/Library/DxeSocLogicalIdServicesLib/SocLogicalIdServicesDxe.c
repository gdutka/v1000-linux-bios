/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/BaseSocketLogicalIdLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Filecode.h>
#include "SocLogicalIdServicesDxe.h"
#include "AGESA.h"

#define FILECODE LIBRARY_DXESOCLOGICALIDSERVICESLIB_SOCLOGICALIDSERVICESDXE_FILECODE

STATIC AMD_SOC_LOGICAL_ID_PROTOCOL  mSocLogicalIdProtocol = {
  SOC_LOGICAL_ID_PROTOCOL_REVISION,
  SocLogicalIdGetLogicalIdOnCurrentCore,
  SocLogicalIdGetLogicalIdOnGivenSocket
};

EFI_STATUS
EFIAPI
SocLogicalIdServiceProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  // Install Fabric Topology Services
  Status = gBS->InstallProtocolInterface (
                &ImageHandle,
                &gAmdSocLogicalIdProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mSocLogicalIdProtocol
                );

  return Status;
}

/**
 * This service retrieves the logical family and revision for the executing core.
 *
 * @param[in]  This                           A pointer to the AMD_SOC_LOGICAL_ID_PROTOCOL instance.
 * @param[out] LogicalId                      Pointer to the logical ID for the executing core.
 *
 * @retval EFI_SUCCESS                        The system topology information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnCurrentCore (
  IN       AMD_SOC_LOGICAL_ID_PROTOCOL  *This,
     OUT   SOC_LOGICAL_ID               *LogicalId
  )
{
  if (LogicalId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (BaseGetLogicalIdOnExecutingCore (LogicalId)) {
    return EFI_SUCCESS;
  } else {
    return EFI_DEVICE_ERROR;
  }
}

/**
 * This service retrieves the logical family and revision for a particular processor.
 *
 * If no processor is installed in Socket, then EFI_INVALID_PARAMETER is returned.
 *
 * @param[in]  This                           A pointer to the AMD_SOC_LOGICAL_ID_PROTOCOL instance.
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[out] LogicalId                      Pointer to the logical ID for the given socket.
 *
 * @retval EFI_SUCCESS                        The processor information was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnGivenSocket (
  IN       AMD_SOC_LOGICAL_ID_PROTOCOL  *This,
  IN       UINTN                        Socket,
     OUT   SOC_LOGICAL_ID               *LogicalId
  )
{
  AGESA_STATUS  Status;

  if (LogicalId == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Socket >= FabricTopologyGetNumberOfProcessorsPresent ()) {
    return EFI_INVALID_PARAMETER;
  }

  Status = BaseGetLogicalIdOnGivenSocket (Socket, LogicalId);
  if (Status == AGESA_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_DEVICE_ERROR;
  }
}


