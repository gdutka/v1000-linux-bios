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
#include <PiDxe.h>
#include "AGESA.h"
#include <Library/HobLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>

#include <Filecode.h>



extern EFI_GUID gAmdCapsuleStatusHobGuid;

BOOLEAN
AmdCapsuleGetStatus (
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;

  GuidHob = NULL;
  GuidHob = GetFirstGuidHob (&gAmdCapsuleStatusHobGuid);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdCapsuleGetStatus  %x\n", (GuidHob == NULL) ? FALSE : TRUE);

  return (GuidHob == NULL) ? FALSE : TRUE;
}
