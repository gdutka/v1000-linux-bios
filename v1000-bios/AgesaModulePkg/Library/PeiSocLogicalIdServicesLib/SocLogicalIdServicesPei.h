/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _SOC_LOGICAL_ID_SERVICES_PEI_H_
#define _SOC_LOGICAL_ID_SERVICES_PEI_H_

EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnCurrentCore (
     OUT   SOC_LOGICAL_ID  *LogicalId
  );

EFI_STATUS
EFIAPI
SocLogicalIdGetLogicalIdOnGivenSocket (
  IN       UINTN           Socket,
     OUT   SOC_LOGICAL_ID  *LogicalId
  );

#endif

