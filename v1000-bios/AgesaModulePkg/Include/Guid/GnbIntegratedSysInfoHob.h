/* $NoKeywords:$ */
/**
 * @file
 *
 * Gnb Frame Buffer Data Hob GUID definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Gnb
 * @e \$Revision: 309090 $   @e \$Date: 2017-11-13 04:30:05 +0800 (Mon, 13 Nov 2017) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _GNB_INTEGRATED_SYSINFO_HOB_H_
#define _GNB_INTEGRATED_SYSINFO_HOB_H_

#define _GNB_INTEGRATED_SYSINFO_HOB_GUID \
{ \
  0x0664BCCF, 0x0A93, 0x407C, 0xA5, 0x8B, 0x2A, 0x56, 0xE4, 0x88, 0x6A, 0x24 \
}
extern EFI_GUID gGnbIntegratedSysInfoHobGuid;

#include <NbioGfx.h>

#pragma pack (push, 1)

/// Memory descriptor structure for each memory rang
typedef struct {
  UINT64  Base;                             ///< Base address of memory rang
  UINT64  Size;                             ///< Size of memory rang
} NBIO_UMA_DESCRIPTOR;

/// Frame Buffer Data HOB data
typedef struct _NBIO_Frame_Buffer_DATA_HOB {
  EFI_HOB_GUID_TYPE              EfiHobGuidType;                             ///< GUID Hob type structure
  ATOM_FUSION_SYSTEM_INFO_V9     AtomFusionSystemInfoV9;                      ///< Platform Config Structure
  NBIO_UMA_DESCRIPTOR            UMAInfo;
} NBIO_Frame_Buffer_DATA_HOB;

#pragma pack (pop)

#endif

