/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Topology Service definitions
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
#ifndef _SMM_FABRIC_TOPOLOGY_SERVICES2_LIB_H_
#define _SMM_FABRIC_TOPOLOGY_SERVICES2_LIB_H_

EFI_STATUS
EFIAPI
FabricTopologyService2SmmProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

#endif

