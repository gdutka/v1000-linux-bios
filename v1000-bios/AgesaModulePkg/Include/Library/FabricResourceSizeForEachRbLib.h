/* $NoKeywords:$ */
/**
 * @file
 *
 * Low-level Fabric MMIO map manager Services library
 *
 * Contains interface to the fabric MMIO map manager library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _FABRIC_RESOURCE_SIZE_FOR_EACH_RB_LIB_H_
#define _FABRIC_RESOURCE_SIZE_FOR_EACH_RB_LIB_H_

#include "Porting.h"
#include "AMD.h"
#include <Uefi/UefiBaseType.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>
#pragma pack (push, 1)

/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */

EFI_STATUS
FabricGetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *ResourceSizeForEachRb
  );

EFI_STATUS
FabricSetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *UserRequest
  );

EFI_STATUS
FabricClearResourceSizeForEachRb (
  );

EFI_STATUS
FabricGetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  );

EFI_STATUS
FabricSetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  );

#pragma pack (pop)
#endif // _FABRIC_RESOURCE_SIZE_FOR_EACH_RB_LIB_H_
