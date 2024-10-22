/* $NoKeywords:$ */
/**
 * @file
 *
 * NBIO Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  NBIO
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

#ifndef _NBIO_SERVICES_PROTOCOL_H_
#define _NBIO_SERVICES_PROTOCOL_H_

// SMU init complete protocol
typedef struct _DXE_AMD_NBIO_SMU_INIT_COMPLETE_PROTOCOL {
  UINT32    Revision;               ///< Revision Number
} DXE_AMD_NBIO_SMU_INIT_COMPLETE_PROTOCOL;

// Current Protocol revision
#define AMD_NBIO_SERVICES_PROTOCOL_REVISION   0x00

extern EFI_GUID gAmdNbioSmuInitCompleteProtocolGuid;
#endif

