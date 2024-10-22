/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <PiPei.h>
#include <Ppi/SocLogicalIdPpi.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/BaseSocketLogicalIdLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Filecode.h>
#include "SocLogicalIdServicesPei.h"
#include "AGESA.h"

#define FILECODE LIBRARY_PEISOCLOGICALIDSERVICESLIB_SOCLOGICALIDSERVICESPEI_FILECODE

STATIC AMD_PEI_SOC_LOGICAL_ID_PPI  mSocLogicalIdServicesPpi = {
  SOC_LOGICAL_ID_PPI_REVISION,
  SocLogicalIdGetLogicalIdOnCurrentCore,
  SocLogicalIdGetLogicalIdOnGivenSocket
};

STATIC EFI_PEI_PPI_DESCRIPTOR mSocLogicalIdPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdSocLogicalIdPpiGuid,
  &mSocLogicalIdServicesPpi
};



EFI_STATUS
EFIAPI
SocLogicalIdServicePpiInstall (
  IN       CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  // Install SoC Logical ID Services
  Status = (**PeiServices).InstallPpi (PeiServices, &mSocLogicalIdPpiList);

  return Status;
}

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
EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnCurrentCore (
     OUT   SOC_LOGICAL_ID  *LogicalId
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
 * @param[in]  Socket                         Zero-based socket number to check.
 * @param[out] LogicalId                      Pointer to the logical ID for the given socket.
 *
 * @retval EFI_SUCCESS                        The given socket's logical ID was retrieved successfully.
 * @retval EFI_INVALID_PARAMETER              Socket is non-existent.
 * @retval EFI_INVALID_PARAMETER              All output parameter pointers are NULL.
 * @retval EFI_DEVICE_ERROR                   Unknown processor family found.
 *
 **/
EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnGivenSocket (
  IN       UINTN           Socket,
     OUT   SOC_LOGICAL_ID  *LogicalId
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


