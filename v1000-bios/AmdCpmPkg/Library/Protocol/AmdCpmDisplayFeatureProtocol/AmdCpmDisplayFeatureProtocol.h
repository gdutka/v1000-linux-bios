/*****************************************************************************
 *
 * Copyright (C) 2012-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 ******************************************************************************
 */

#ifndef _AMD_CPM_DISPLAY_FEATURE_PROTOCOL_H_
#define _AMD_CPM_DISPLAY_FEATURE_PROTOCOL_H_

#include <AmdCpmBase.h>
#include CPM_PROTOCOL_DEFINITION (AmdCpmTableProtocol)

//
// GUID definition
//
#define AMD_CPM_DISPLAY_FEATURE_PROTOCOL_GUID \
  { 0xdb4a79ac, 0x5bbb, 0x4625, 0x6a, 0x9e, 0xfe, 0xbf, 0x9d, 0x6d, 0x95, 0xeb }

extern EFI_GUID gAmdCpmDisplayFeatureProtocolGuid;


/// DXE Protocol Structure
typedef struct _AMD_CPM_DISPLAY_FEATURE_PROTOCOL {          // See the Forward Declaration above
  UINTN                           Revision;                 ///< Protocol Revision
  AMD_CPM_TABLE_PROTOCOL          *TableProtocolPtr;        ///< AMD CPM Table Protocol Instance in Dxe
  AMD_CPM_TABLE_PROTOCOL          *TableProtocolSmmPtr;     ///< AMD CPM Table Protocol Instance in Smm
  CPM_DISPLAY_FEATURE_PRIVATE     DisplayFeatureData;       ///< Private Data for Display Feature
} AMD_CPM_DISPLAY_FEATURE_PROTOCOL;

// current PPI revision
#define AMD_CPM_DISPLAY_FEATURE_PROTOCOL_REV  0x00

#endif
