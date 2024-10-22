/* $NoKeywords:$ */
/**
 * @file
 *
 * Fabric NUMA domain definitions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Include
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2017-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _FABRIC_ACPI_DOMAIN_INFO_H_
#define _FABRIC_ACPI_DOMAIN_INFO_H_

#include <FabricRegistersRV.h>

#pragma pack (push, 1)

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  UINT32  Domain;
  UINT32  DramMapPair[MAX_CHANNELS_PER_DIE];
} DIE_INFO;

typedef struct {
  UINT32   SocketCount;
  UINT32   DieCount;
  UINT32   Socket;
  UINT32   Die;
  UINT32   SocketMask;
  UINT32   DieMask;
} DRAM_INFO;

#pragma pack (pop)
#endif // _FABRIC_ACPI_DOMAIN_INFO_H_

