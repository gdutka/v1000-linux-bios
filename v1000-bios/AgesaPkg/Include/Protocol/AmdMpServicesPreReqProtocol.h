/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD MP Services Pre-requisite Protocol definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Protocol
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_MP_SERVICES_PREREQ_PROTOCOL_H_
#define _AMD_MP_SERVICES_PREREQ_PROTOCOL_H_

//----------------------------------------------------
//
//  AMD MP Services Pre-requisite Protocol
//
//-------------------------------------------------------

/// @todo Style_Analyzer: Add Doxygen comment to struct construct
typedef struct _DXE_AMD_MP_SERVICES_PREREQ_PROTOCOL {
  UINTN    Revision;               ///< Revision Number
} DXE_AMD_MP_SERVICES_PREREQ_PROTOCOL;

#define AMD_MP_SERVICES_PREREQ_PROTOCOL_REVISION 0x00

extern EFI_GUID gAmdMpServicesPreReqProtocolGuid;

#endif // _AMD_MP_SERVICES_PREREQ_PROTOCOL_H_
