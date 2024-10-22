/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric ready to boot  definitions
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

 #ifndef _FABRIC_READY_TO_BOOT_H_
 #define _FABRIC_READY_TO_BOOT_H_

VOID
EFIAPI
FabricReadyToBoot (
  IN       EFI_EVENT Event,
  IN       VOID      *Context
  );

#endif
