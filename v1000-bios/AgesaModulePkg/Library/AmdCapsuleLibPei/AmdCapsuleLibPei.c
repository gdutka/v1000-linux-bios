/* $NoKeywords:$ */
/**
 * @file
 *
 * Amd Capsule crelated functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309172 $   @e \$Date: 2014-12-10 21:13:24 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#include "Uefi.h"
#include <PiPei.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <AGESA.h>
#include <Library/IdsLib.h>

#include <Filecode.h>



extern EFI_GUID gCapsuleUpdateDetectedPpiGuid;

BOOLEAN
AmdCapsuleGetStatus (
  )
{
  EFI_STATUS Status;
  VOID *Ppi;
  CONST EFI_PEI_SERVICES  **PeiServices;
  
  PeiServices = GetPeiServicesTablePointer();
  //This PPI is installed in secured phase, when detected CAPSULE_UPDATE_SIGNATURE at RBX
  Status = (*PeiServices)->LocatePpi (PeiServices, &gCapsuleUpdateDetectedPpiGuid, 0, NULL, &Ppi);
  if (EFI_ERROR (Status)) {
     IDS_HDT_CONSOLE (MAIN_FLOW, "gCapsuleUpdateDetectedPpiGuid Not Found\n");
  } else {
     IDS_HDT_CONSOLE (MAIN_FLOW, "gCapsuleUpdateDetectedPpiGuid located\n");
  }
  return (EFI_ERROR (Status)) ? FALSE : TRUE;
}
