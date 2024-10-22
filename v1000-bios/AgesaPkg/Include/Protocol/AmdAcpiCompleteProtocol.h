/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD ACPI Initialization Complete Protocol definition
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
#ifndef _AMD_ACPI_COMPLETE_PROTOCOL_H_
#define _AMD_ACPI_COMPLETE_PROTOCOL_H_

//----------------------------------------------------
//
//  AMD ACPI DXE Initialization Complete Protocol
//
//-------------------------------------------------------

typedef struct _DXE_AMD_ACPI_INIT_COMPLETE_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_ACPI_INIT_COMPLETE_PROTOCOL;

extern EFI_GUID gAmdAcpiDxeInitCompleteProtocolGuid;

#endif // _AMD_ACPI_COMPLETE_PROTOCOL_H_
