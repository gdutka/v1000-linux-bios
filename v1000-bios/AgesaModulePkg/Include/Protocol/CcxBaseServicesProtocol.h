/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX Base Services Protocol prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Protocol
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * Copyright (C) 2017-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _CCX_BASE_SERVICES_PROTOCOL_H_
#define _CCX_BASE_SERVICES_PROTOCOL_H_


/// Forward declaration for the CCX_BASE_SERVICES_PROTOCOL
typedef struct _CCX_BASE_SERVICES_PROTOCOL CCX_BASE_SERVICES_PROTOCOL;

typedef
UINT8
(EFIAPI *CCX_GET_MAX_PHYS_ADDR) (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  );

typedef
UINT8
(EFIAPI *CCX_GET_MAX_PROC_CAP) (
  IN       CCX_BASE_SERVICES_PROTOCOL *This
  );

/// When installed, the CCX Base Services Protocol produces a collection
/// of services that return various information related to the CCX
struct _CCX_BASE_SERVICES_PROTOCOL {
  UINTN                   Revision;            ///< Revision number
  CCX_GET_MAX_PHYS_ADDR   CcxGetMaxPhysAddr;   ///< Get maximum physical address
  CCX_GET_MAX_PROC_CAP    CcxGetMaxProcCap;    ///< Get maximum processor capacity
};

extern EFI_GUID gAmdCcxBaseServicesProtocolGuid;

#endif
