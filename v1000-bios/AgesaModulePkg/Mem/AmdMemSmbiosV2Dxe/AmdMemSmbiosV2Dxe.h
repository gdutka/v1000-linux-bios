/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains definitions for AmdMemCzPei
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _AMD_MEM_SMBIOS_DXE_CZ_H_
#define _AMD_MEM_SMBIOS_DXE_CZ_H_
#include "MemDmi.h"
#include "Protocol/AmdSmbiosServicesProtocol.h"

EFI_STATUS
EFIAPI
GetMemDmiInfo (
  IN       AMD_MEM_SMBIOS_SERVICES_PROTOCOL       *This,
     OUT   DMI_INFO                               *MemDmiInfo
  );

#endif      /* _AMD_MEM_SMBIOS_DXE_CZ_H_ */

