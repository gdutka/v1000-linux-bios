/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD SMBIOS Initialization Complete Protocol definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_SMBIOS_COMPLETE_PROTOCOL_H_
#define _AMD_SMBIOS_COMPLETE_PROTOCOL_H_

//----------------------------------------------------
//
//  AMD SMBIOS DXE Initialization Complete Protocol
//
//-------------------------------------------------------

/// @todo Style_Analyzer: Add Doxygen comment to struct construct
typedef struct _DXE_AMD_SMBIOS_INIT_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_SMBIOS_INIT_COMPLETE_PROTOCOL;

extern EFI_GUID gAmdSmbiosDxeInitCompleteProtocolGuid;

#endif // _AMD_SMBIOS_COMPLETE_PROTOCOL_H_
