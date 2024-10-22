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
#ifndef _AMDCAPSULELIB_H_
#define _AMDCAPSULELIB_H_

extern EFI_GUID gCapsuleUpdateDetectedPpiGuid;

BOOLEAN
AmdCapsuleGetStatus (
  );


#endif // _AMDCAPSULELIB_H_
