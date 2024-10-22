/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU SMBIOS functions.
 *
 * Contains code for collecting SMBIOS information
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Ccx
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _CCX_ZEN_RV_SMBIOS_DXE_H_
#define _CCX_ZEN_RV_SMBIOS_DXE_H_

#pragma pack (push, 1)

EFI_STATUS
EFIAPI
CcxZenRvSmbiosServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  );

#pragma pack (pop)
#endif // _CCX_ZEN_RV_SMBIOS_DXE_H_

