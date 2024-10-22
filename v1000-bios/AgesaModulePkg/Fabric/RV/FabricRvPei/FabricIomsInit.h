/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric IO master / slave initialization definitions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_IOMS_INIT_H_
#define _FABRIC_IOMS_INIT_H_

#include <Ppi/SocLogicalIdPpi.h>

VOID
FabricIomsInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  );

#endif

