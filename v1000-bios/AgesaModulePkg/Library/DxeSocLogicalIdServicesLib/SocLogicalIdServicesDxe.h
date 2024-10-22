/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _SOC_LOGICAL_ID_SERVICES_DXE_H_
#define _SOC_LOGICAL_ID_SERVICES_DXE_H_

EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnCurrentCore (
  IN       AMD_SOC_LOGICAL_ID_PROTOCOL  *This,
     OUT   SOC_LOGICAL_ID               *LogicalId
  );

EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnGivenSocket (
  IN       AMD_SOC_LOGICAL_ID_PROTOCOL                     *This,
  IN       UINTN                                           Socket,
     OUT   SOC_LOGICAL_ID                                  *LogicalId
  );

#endif

