/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Library
 *
 * Contains interface to the AMD AGESA library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/ApcbSspLib.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ApobCommonServiceLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        LIBRARY_APCBSSPLIB_DRAMPOSTPACKAGEREPAIR_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          G L O B A L        V A L U E S
 *----------------------------------------------------------------------------------------
 */

extern UINT8                     mApcbInstance;
extern BOOLEAN                   mAtRuntime;
extern BOOLEAN                   mApcbRecoveryFlag;

EFI_STATUS
InternalApcbLocateType (
  IN       UINT16   GroupId,
  IN       UINT16   TypeId,
  IN       UINT16   InstanceId,
  IN       UINT32   *Size,
  IN OUT   VOID     **TypeData
  );

/*++

Routine Description:

  This function retrieves the DRAM POST Package repair entries

--*/
EFI_STATUS
ApcbGetDramPostPkgRepairEntries (
  IN OUT   APCB_DPPRCL_REPAIR_ENTRY **pApcbDppRepairEntries,
  IN OUT   UINT32                   *SizeInByte
  )
{
  EFI_STATUS                Status;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    if (mApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, SizeInByte, (VOID *) pApcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:

  This function clears the DRAM POST Package repair entries

--*/
EFI_STATUS
ApcbClearDramPostPkgRepairEntry (
  VOID
  )
{
  EFI_STATUS                Status;
  APCB_DPPRCL_REPAIR_ENTRY  ApcbDppRepairEntries[2];
  APCB_HEADER               *ApcbShadowCopy;
  UINT32                    NewApcbSize;
  UINT8                     *NewApcb;

  Status  = EFI_SUCCESS;
  NewApcb = NULL;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    if (mApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  SetMem (&ApcbDppRepairEntries[0], sizeof (APCB_DPPRCL_REPAIR_ENTRY) * 2, 0);

  // Leave only one clean entry in the APCB data of APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR
  ApcbShadowCopy = GetApcbShadowCopy ();
  NewApcbSize = ApcbShadowCopy->SizeOfApcb + sizeof (APCB_DPPRCL_REPAIR_ENTRY) + ALIGN_SIZE_OF_TYPE;
  NewApcb = AllocateZeroPool (NewApcbSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewApcb, NewApcbSize);
  ASSERT (NewApcb != NULL);
  if (NewApcb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //Shadow Copy pointer will be updated in below routine
  Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&ApcbDppRepairEntries[0], sizeof (APCB_DPPRCL_REPAIR_ENTRY), NewApcb);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot replace APCB type data\n");
    return Status;
  }

  return EFI_SUCCESS;
}

VOID
CopyDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *DestEntry,
  IN       APCB_DPPRCL_REPAIR_ENTRY *SrcEntry
  )
{
  ASSERT (DestEntry != NULL);
  ASSERT (SrcEntry != NULL);

  DestEntry->Valid          = SrcEntry->Valid;
  DestEntry->Bank           = SrcEntry->Bank;
  DestEntry->RankMultiplier = SrcEntry->RankMultiplier;
  DestEntry->Device         = SrcEntry->Device;
  DestEntry->ChipSelect     = SrcEntry->ChipSelect;
  DestEntry->Column         = SrcEntry->Column;
  DestEntry->RepairType     = SrcEntry->RepairType;
  DestEntry->Row            = SrcEntry->Row;
  DestEntry->Socket         = SrcEntry->Socket;
  DestEntry->Channel        = SrcEntry->Channel;
}

BOOLEAN
CompareDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *DestEntry,
  IN       APCB_DPPRCL_REPAIR_ENTRY *SrcEntry
  )
{
  ASSERT (DestEntry != NULL);
  ASSERT (SrcEntry != NULL);

  if (DestEntry->Valid          == SrcEntry->Valid &&
      DestEntry->Bank           == SrcEntry->Bank &&
      DestEntry->RankMultiplier == SrcEntry->RankMultiplier &&
      DestEntry->Device         == SrcEntry->Device &&
      DestEntry->ChipSelect     == SrcEntry->ChipSelect &&
      DestEntry->Column         == SrcEntry->Column &&
      DestEntry->RepairType     == SrcEntry->RepairType &&
      DestEntry->Row            == SrcEntry->Row &&
      DestEntry->Socket         == SrcEntry->Socket &&
      DestEntry->Channel        == SrcEntry->Channel
  ) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*++

Routine Description:

  This function adds a DRAM POST Package repair entry

--*/
EFI_STATUS
ApcbAddDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  APCB_DPPRCL_REPAIR_ENTRY  *ApcbDppRepairEntries;
  APCB_DPPRCL_REPAIR_ENTRY  *NewApcbDppRepairEntries;
  UINT32                    SizeInByte;
  UINT16                    i;
  APCB_HEADER               *ApcbShadowCopy;
  UINT32                    NewApcbSize;
  UINT8                     *NewApcb;
  BOOLEAN                   EmptyEntryFound;
  UINT16                    NewEntryId;

  Status                  = EFI_SUCCESS;
  EmptyEntryFound         = FALSE;
  NewApcbDppRepairEntries = NULL;
  NewApcb                 = NULL;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    if (mApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, &SizeInByte, (VOID *) &ApcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  // Check if the target entry already exists
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (CompareDramPostPkgRepairEntry (&ApcbDppRepairEntries[i], Entry)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Duplicate DDR4 Post Package Repair Entry found\n");
      return EFI_SUCCESS;
    }
  }

  // Try adding the entry to the existing space first
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (0 == ApcbDppRepairEntries[i].Valid) {
      EmptyEntryFound = TRUE;
      CopyDramPostPkgRepairEntry (&ApcbDppRepairEntries[i], Entry);
      break;
    }
  }

  if (FALSE == EmptyEntryFound) {
    // Not enough space. Try increasing the size of the type data
    NewEntryId = (UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY);
    if (NewEntryId >= PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Too many Post Package Repair Entries requested.\n");
      return EFI_OUT_OF_RESOURCES;
    }
    NewApcbDppRepairEntries = AllocateZeroPool (SizeInByte + sizeof (APCB_DPPRCL_REPAIR_ENTRY));
    ASSERT (NewApcbDppRepairEntries != NULL);
    if (NewApcbDppRepairEntries == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (NewApcbDppRepairEntries, ApcbDppRepairEntries, SizeInByte);
    CopyDramPostPkgRepairEntry (&NewApcbDppRepairEntries[NewEntryId], Entry);

    ApcbShadowCopy = GetApcbShadowCopy ();
    NewApcbSize = ApcbShadowCopy->SizeOfApcb + sizeof (APCB_DPPRCL_REPAIR_ENTRY) + ALIGN_SIZE_OF_TYPE;
    NewApcb = AllocateZeroPool (NewApcbSize);
    IDS_HDT_CONSOLE_PSP_TRACE ("ALLOCATE[%x:%x]\n", NewApcb, NewApcbSize);
    ASSERT (NewApcb != NULL);
    if (NewApcb == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    //Shadow Copy pointer will be updated in below routine
    Status = ApcbReplaceType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, (UINT8 *)&NewApcbDppRepairEntries[0], SizeInByte + sizeof (APCB_DPPRCL_REPAIR_ENTRY), NewApcb);
    if (EFI_ERROR (Status)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Cannot replace APCB type data\n");
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/*++

Routine Description:

  This function removes a DRAM POST Package repair entry

--*/
EFI_STATUS
ApcbRemoveDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  APCB_DPPRCL_REPAIR_ENTRY  *ApcbDppRepairEntries;
  UINT32                    SizeInByte;
  UINT16                    i;
  BOOLEAN                   TargetEntryFound;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    if (mApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  Status = InternalApcbLocateType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, 0, &SizeInByte, (VOID *) &ApcbDppRepairEntries);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot locate DDR4 Post Package Repair Entries\n");
    return Status;
  }

  TargetEntryFound = FALSE;
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (CompareDramPostPkgRepairEntry (&ApcbDppRepairEntries[i], Entry)) {
      TargetEntryFound                        = TRUE;
      ApcbDppRepairEntries[i].Valid           = 0;
    }
  }

  if (!TargetEntryFound) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Cannot find the DDR4 Post Package Repair Entry\n");
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

