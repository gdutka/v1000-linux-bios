/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Resource Manager Service definitions
 *
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
#ifndef _PEI_FABRIC_RESOURCE_MANAGER_SERVICES_LIB_H_
#define _PEI_FABRIC_RESOURCE_MANAGER_SERVICES_LIB_H_

EFI_STATUS
EFIAPI
FabricResourceManagerServicePpiInstall (
  IN       CONST EFI_PEI_SERVICES     **PeiServices
  );

#endif

