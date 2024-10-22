/* $NoKeywords:$ */
/**
 * @file
 *
 * ApcbLibV3.h
 *
 * AGESA PSP Customization Block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 192403 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
*******************************************************************************
*
*/

#ifndef _APCB_LIB_V3_H_
#define _APCB_LIB_V3_H_

#include "Porting.h"
#include <Addendum/Apcb/Inc/SSP/ApcbMemDdrPostPackageRepair.h>

#define INVALID_PRIORITY_LEVEL        0xFF
#define INVALID_BIOS_DIR_ENTRY        0xFF
#define INVALID_BINARY_INSTANCE       0xFF
#define INVALID_TYPE_INSTANCE         0xFF

typedef struct {
  UINT8                   PriorityLevel;
  UINT8                   BiosDirEntry;
  UINT8                   BinaryInstance;
  UINT8                   TypeInstance;
} PRIORITY_INSTANCE_MAPPING;

UINT8
ApcbCalcCheckSum (
  IN       VOID             *ApcbPtr,
  IN       UINT32           Length
  );

VOID *
GetApcbShadowCopy (
  IN OUT  UINT32  *Size
  );

VOID
SetApcbShadowCopy (
  IN      VOID * ApcbShadowAddress
  );

BOOLEAN
ApcbAcquireMutex (
  );

VOID
ApcbReleaseMutex (
  );

VOID
ApcbSetPurpose (
  IN       UINT8         Purpose
  );

UINT8
ApcbGetPurpose (
  VOID
  );

VOID
ApcbSetPriorityLevel (
  IN       UINT8         PriorityLevel
  );

BOOLEAN
ApcbGetBool (
  IN       UINT32        ApcbToken,
      OUT  BOOLEAN       *Value
  );

BOOLEAN
ApcbSetBool (
  IN       UINT32        ApcbToken,
  IN       BOOLEAN       Value
  );

BOOLEAN
ApcbGet8 (
  IN       UINT32        ApcbToken,
      OUT  UINT8         *Value
  );

BOOLEAN
ApcbSet8 (
  IN       UINT32        ApcbToken,
  IN       UINT8         Value
  );

BOOLEAN
ApcbGet16 (
  IN       UINT32        ApcbToken,
      OUT  UINT16        *Value
  );

BOOLEAN
ApcbSet16 (
  IN       UINT32        ApcbToken,
  IN       UINT16        Value
  );

BOOLEAN
ApcbGet32 (
  IN       UINT32        ApcbToken,
      OUT  UINT32        *Value
  );

BOOLEAN
ApcbSet32 (
  IN       UINT32        ApcbToken,
  IN       UINT32        Value
  );

BOOLEAN
ApcbGetType (
  IN       UINT16        GroupId,
  IN       UINT16        TypeId,
      OUT  UINT8         **DataBuf,
      OUT  UINT32        *DataSize
  );

BOOLEAN
ApcbSetType (
  IN       UINT16        GroupId,
  IN       UINT16        TypeId,
  IN       UINT8         *DataBuf,
  IN       UINT32        DataSize
  );

BOOLEAN
ApcbPurgeAllTokens (
  VOID
  );

BOOLEAN
ApcbPurgeAllTypes (
  VOID
  );

BOOLEAN
ApcbEnumerateTokens (
  VOID
  );

EFI_STATUS
AmdPspWriteBackApcbShadowCopy (
  VOID
  );

EFI_STATUS
ApcbGetDramPostPkgRepairEntries (
  IN OUT   APCB_DPPRCL_REPAIR_ENTRY **pApcbDppRepairEntries,
  IN OUT   UINT32                   *SizeInByte
  );

EFI_STATUS
ApcbClearDramPostPkgRepairEntry (
  VOID
  );

EFI_STATUS
ApcbAddDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  );

EFI_STATUS
ApcbRemoveDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  );

BOOLEAN
ApcbV2GetConfigToken (
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

BOOLEAN
ApcbV2SetConfigToken (
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

#endif


