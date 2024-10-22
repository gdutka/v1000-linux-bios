/* $NoKeywords:$ */
/**
 * @file
 *
 * Contains definitions for ApcbLib
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#ifndef _APCB_ZP_LIB_H_
#define _APCB_ZP_LIB_H_
#include "Porting.h"
#include <Addendum/Apcb/Inc/ZP/APCB.h>

EFI_STATUS
ApcbReplaceType (
  IN     UINT16          GroupId,
  IN     APCB_PARAM_TYPE ApcbParamType,
  IN     UINT16          InstanceId,
  IN     UINT8           *TypeDataStream,
  IN     UINT32          TypeDataSize,
  IN OUT UINT8           *NewApcb
  );

EFI_STATUS
ApcbGetType (
  IN     UINT16          GroupId,
  IN     APCB_PARAM_TYPE ApcbParamType,
  IN     UINT16          InstanceId,
  IN     UINT8           *TypeDataStream,
  IN OUT UINT32          *TypeDataSize
  );

EFI_STATUS
AmdPspApcbGetCfgParameter (
  IN       APCB_PARAM_CONFIG_TOKEN TokenId,
  IN OUT   UINT32           *SizeInByte,
  IN OUT   UINT64          *Value
  );

EFI_STATUS
AmdPspApcbSetCfgParameter (
  IN       APCB_PARAM_CONFIG_TOKEN TokenId,
  IN       UINT32           *SizeInByte,
  IN       UINT64          *Value
  );

EFI_STATUS
AmdPspApcbGetCbsCmnParameter (
  IN       UINT16          TokenId,
  IN OUT   UINT32          *SizeInByte,
  IN OUT   UINT64          *Value
  );

EFI_STATUS
AmdPspApcbSetCbsCmnParameter (
  IN       UINT16          TokenId,
  IN OUT   UINT32          *SizeInByte,
  IN OUT   UINT64          *Value
  );

VOID *
GetApcbShadowCopy (
  );

VOID
SetApcbShadowCopy (
  VOID * ApcbShadowAddress
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

UINT8
ApcbCalcCheckSum (
  IN       VOID             *ApcbPtr,
  IN       UINT32           Length
  );

#endif      /* _APCB_ZP_LIB_H_ */

