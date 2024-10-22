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
#ifndef _DXE_FABRIC_RESOURCE_MANAGER_SERVICES_LIB_H_
#define _DXE_FABRIC_RESOURCE_MANAGER_SERVICES_LIB_H_

EFI_STATUS
EFIAPI
FabricResourceManagerServiceProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

#endif

