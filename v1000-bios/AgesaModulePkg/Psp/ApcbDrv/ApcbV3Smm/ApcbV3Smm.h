/* $NoKeywords:$ */
/**
 * @file
 *
 * APCB DXE Driver Header
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  APCB
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include <Protocol/AmdApcbProtocol.h>

#ifndef _AMD_APCB_SMM_H_
#define _AMD_APCB_SMM_H_

VOID
mApcbSetActiveInstance (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             Instance
  );

EFI_STATUS
mApcbFlushData (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  );

EFI_STATUS
mApcbAcquireMutex (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  );

EFI_STATUS
mApcbReleaseMutex (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  );

EFI_STATUS
mApcbGetTokenBool (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        *PriorityLevel,
  IN       UINT32            Uid,
  IN OUT   BOOLEAN           *bValue
  );

EFI_STATUS
mApcbSetTokenBool (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel,
  IN       UINT32            Uid,
  IN       BOOLEAN           bValue
  );

EFI_STATUS
mApcbGetToken8 (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        *PriorityLevel,
  IN       UINT32            Uid,
  IN OUT   UINT8             *Value8
  );

EFI_STATUS
mApcbSetToken8 (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel,
  IN       UINT32            Uid,
  IN       UINT8             Value8
  );

EFI_STATUS
mApcbGetToken16 (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        *PriorityLevel,
  IN       UINT32            Uid,
  IN OUT   UINT16            *Value16
  );

EFI_STATUS
mApcbSetToken16 (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel,
  IN       UINT32            Uid,
  IN       UINT16            Value16
  );

EFI_STATUS
mApcbGetToken32 (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        *PriorityLevel,
  IN       UINT32            Uid,
  IN OUT   UINT32            *Value32
  );

EFI_STATUS
mApcbSetToken32 (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel,
  IN       UINT32            Uid,
  IN       UINT32            Value32
  );

EFI_STATUS
mApcbGetType (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        *PriorityLevel,
  IN       UINT16        GroupId,
  IN       UINT16        TypeId,
      OUT  UINT8         **DataBuf,
      OUT  UINT32        *DataSize
  );

EFI_STATUS
mApcbSetType (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel,
  IN       UINT16        GroupId,
  IN       UINT16        TypeId,
  IN       UINT8         *DataBuf,
  IN       UINT32        DataSize
  );

EFI_STATUS
mApcbPurgeAllTokens (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel
  );

EFI_STATUS
mApcbPurgeAllTypes (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT8                        PriorityLevel
  );

EFI_STATUS
mApcbGetDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN OUT   DRRP_REPAIR_ENTRY *Entry,
  IN OUT   UINT32            *NumOfEntries
  );

EFI_STATUS
mApcbClearDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This
  );

EFI_STATUS
mApcbAddDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  );

EFI_STATUS
mApcbRemoveDramPostPkgRepairEntry (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  );

EFI_STATUS
mApcbSetConfigParameter (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

EFI_STATUS
mApcbGetConfigParameter (
  IN       AMD_APCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

#endif //_AMD_APCB_SMM_H_

