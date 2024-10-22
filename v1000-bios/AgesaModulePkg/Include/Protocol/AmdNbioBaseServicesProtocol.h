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

#ifndef _NBIO_BASE_SERVICES_PROTOCOL_H_
#define _NBIO_BASE_SERVICES_PROTOCOL_H_

// Current PROTOCOL revision
#define AMD_NBIO_DEBUG_OPTION_SERVICES_DXE_REVISION   0x00

///
/// Forward declaration for the NBIO_BASE_SERVICES_PROTOCOL
///
typedef struct _DXE_AMD_NBIO_DEBUG_SERVICES_PROTOCOL DXE_AMD_NBIO_DEBUG_SERVICES_PROTOCOL;


//
// Protocol Definitions
//
/**
  Returns the NBIO debug options configuration structure
  This
    A pointer to the DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL instance.
  DebugOptions
    A pointer to a pointer to store the address of the debug options configuration structure
**/
typedef
EFI_STATUS
(EFIAPI * AMD_NBIO_DEBUG_GET_DEBUG_CONFIG) (
  IN  DXE_AMD_NBIO_DEBUG_SERVICES_PROTOCOL     *This,
  OUT UINT32                                   **DebugOptions
);

///
/// The Dxe of Base Debug services
///
struct _DXE_AMD_NBIO_DEBUG_SERVICES_PROTOCOL {
  UINT32                                Revision;       ///< revision
  AMD_NBIO_DEBUG_GET_DEBUG_CONFIG       DebugGetConfig; ///<
};

extern EFI_GUID gAmdNbioDebugServicesProtocolGuid;

#endif

