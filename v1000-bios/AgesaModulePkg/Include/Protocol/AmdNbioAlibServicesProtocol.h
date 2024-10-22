/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric Pstate Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _NBIO_ALIB_SERVICES_PROTOCOL_H_
#define _NBIO_ALIB_SERVICES_PROTOCOL_H_

// Current PROTOCOL revision
#define AMD_ALIB_SERVICES_DXE_REVISION   0x00

///
/// Forward declaration for the NBIO_ALIB_SERVICES_PROTOCOL
///
typedef struct _DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL;


//
// Protocol Definitions
//
/**
  Returns the Alib address
  This
    A pointer to the DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL instance.
  DebugOptions
    A pointer to a pointer to store the address of ALib
**/
typedef
VOID
(EFIAPI * AMD_ALIB_SERVICES_DXE_CONFIG) (
  IN  DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL      *This,
  OUT VOID                                     **AlibAddress
);

///
/// The Dxe of ALIB services
///
struct _DXE_AMD_NBIO_ALIB_SERVICES_PROTOCOL {
  UINT32                                Revision;      ///< revision
  AMD_ALIB_SERVICES_DXE_CONFIG          AlibGetConfig; ///<
};

extern EFI_GUID gAmdNbioALibServicesProtocolGuid;

#endif

