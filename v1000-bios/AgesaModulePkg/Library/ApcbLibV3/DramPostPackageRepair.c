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
#include <Library/ApcbLibV3.h>
#include <Library/AmdSocBaseLib.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Porting.h>
#include <Addendum/Apcb/Inc/SSP/APCB.h>
#include <Addendum/Apcb/Inc/SSP/APOB.h>
#include <Library/ApobCommonServiceLib.h>
#include <Library/AmdPspApobLib.h>
#include <Filecode.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE        LIBRARY_APCBLIBV3_DRAMPOSTPACKAGEREPAIR_FILECODE

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

extern BOOLEAN                  mAtRuntime;

/*---------------------------------------------------------------------------------------*/
/**
 *      This function retrieves the DRAM POST Package repair entries
 *
 * @param[in]         pApcbDppRepairEntries         - Buffer of the DRAM POST Package repair entries
 * @param[in]         SizeInByte                    - The size of the array of the DRAM POST Package repair entries
 *
 * @retval            EFI_SUCCESS            - DRAM POST Package repair entries are successfully retrieved
 *                    EFI_UNSUPPORTED        - DRAM POST Package repair entries cannot be retrieved at this stage
 *                    EFI_NOT_FOUND          - DRAM POST Package repair entries cannot be found
 *
 */
EFI_STATUS
ApcbGetDramPostPkgRepairEntries (
  IN OUT   APCB_DPPRCL_REPAIR_ENTRY **EntryArray,
  IN OUT   UINT32                   *SizeInByte
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = ApobGetApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  if (!ApcbGetType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, (UINT8 **) EntryArray, SizeInByte)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to locate DDR4 Post Package Repair Entries\n");
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This function clears the DRAM POST Package repair entries
 *
 * @retval            EFI_SUCCESS            - DRAM POST Package repair entries are successfully cleared
 *                    EFI_UNSUPPORTED        - DRAM POST Package repair entries cannot be cleared at this stage
 *                    EFI_NOT_FOUND          - DRAM POST Package repair entries cannot be found
 *
 */
EFI_STATUS
ApcbClearDramPostPkgRepairEntry (
  VOID
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = ApobGetApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  if (ApcbSetType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, NULL, 0)) {
    return EFI_SUCCESS;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to locate DDR4 Post Package Repair Entries\n");

  return EFI_NOT_FOUND;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This function copies a DRAM POST Package repair entry
 *
 */
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

/*---------------------------------------------------------------------------------------*/
/**
 *      This function compares two DRAM POST Package repair entry
 *
 * @retval            TRUE            - Two DRAM POST Package repair entries are matched
 *                    FALSE           - Two DRAM POST Package repair entries are not matched
 *
 */
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

/*---------------------------------------------------------------------------------------*/
/**
 *      This function adds a DRAM POST Package repair entry
 *
 * @param[in]         Entry                  - The DRAM POST Package repair entry to be added
 *
 * @retval            EFI_SUCCESS            - DRAM POST Package repair entries are successfully cleared
 *                    EFI_UNSUPPORTED        - DRAM POST Package repair entries cannot be cleared at this stage
 *                    EFI_NOT_FOUND          - The type of DRAM POST Package repair entries cannot be found
 *
 */
EFI_STATUS
ApcbAddDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;
  APCB_DPPRCL_REPAIR_ENTRY  *EntryArray;
  APCB_DPPRCL_REPAIR_ENTRY  *NewEntryArray;
  UINT32                    SizeInByte;
  UINT16                    i;
  BOOLEAN                   EmptyEntryFound;
  UINT16                    NewEntryId;

  Status                  = EFI_SUCCESS;
  EmptyEntryFound         = FALSE;
  NewEntryArray           = NULL;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = ApobGetApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  if (!ApcbGetType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, (UINT8 **) &EntryArray, &SizeInByte)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to locate DDR4 Post Package Repair Entries\n");
    return EFI_NOT_FOUND;
  }

  // Check if the target entry already exists
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (CompareDramPostPkgRepairEntry (&EntryArray[i], Entry)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Duplicate DDR4 Post Package Repair Entry found\n");
      return EFI_SUCCESS;
    }
  }

  // Try adding the entry to the existing space first
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (0 == EntryArray[i].Valid) {
      EmptyEntryFound = TRUE;
      CopyDramPostPkgRepairEntry (&EntryArray[i], Entry);
      break;
    }
  }

  if (FALSE == EmptyEntryFound) {
    // Not enough space. Try increasing the size of the type data
    NewEntryId = (UINT16) SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY);
    if (NewEntryId >= PcdGet32 (PcdAmdMemCfgMaxPostPackageRepairEntries)) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Too many Post Package Repair Entries requested\n");
      return EFI_OUT_OF_RESOURCES;
    }
    NewEntryArray = AllocateZeroPool (SizeInByte + sizeof (APCB_DPPRCL_REPAIR_ENTRY));
    ASSERT (NewEntryArray != NULL);
    if (NewEntryArray == NULL) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to allocate buffer for a new DDR4 Post Package Repair Entry\n");
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem (NewEntryArray, EntryArray, SizeInByte);
    CopyDramPostPkgRepairEntry (&NewEntryArray[NewEntryId], Entry);

    if (!ApcbSetType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, (UINT8 *)NewEntryArray, SizeInByte + sizeof (APCB_DPPRCL_REPAIR_ENTRY))) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to find the type for Post Package Repair Entries\n");
      return EFI_NOT_FOUND;
    }
  }

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 *      This function removes a DRAM POST Package repair entry
 *
 * @param[in]         Entry                  - The DRAM POST Package repair entry to be added
 *
 * @retval            EFI_SUCCESS            - DRAM POST Package repair entry is successfully removed
 *                    EFI_UNSUPPORTED        - DRAM POST Package repair entry cannot be removed at this stage
 *                    EFI_NOT_FOUND          - DRAM POST Package repair entry cannot be found
 *
 */
EFI_STATUS
ApcbRemoveDramPostPkgRepairEntry (
  IN       APCB_DPPRCL_REPAIR_ENTRY *Entry
  )
{
  EFI_STATUS                Status;
  BOOLEAN                   ApcbRecoveryFlag;
  APCB_DPPRCL_REPAIR_ENTRY  *EntryArray;
  UINT32                    SizeInByte;
  UINT16                    i;
  BOOLEAN                   TargetEntryFound;

  Status  = EFI_SUCCESS;

  //Exit service, if recovery flag set
  if (mAtRuntime == FALSE) {
    Status = ApobGetApcbRecoveryFlag (&ApcbRecoveryFlag);
    if (ApcbRecoveryFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] APCB.RecoveryFlag Set, exit service\n");
      return EFI_UNSUPPORTED;
    }
  }

  //Exit service, if recovery flag set
  if (CheckPspRecoveryFlagV2 () == TRUE) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Recovery flag set, exit service\n");
    // ASSERT (FALSE); // Assertion in the debug build
    return EFI_UNSUPPORTED;
  }

  if (!ApcbGetType (APCB_GROUP_MEMORY, APCB_MEM_TYPE_DDR_POST_PACKAGE_REPAIR, (UINT8 **)&EntryArray, &SizeInByte)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to locate DDR4 Post Package Repair Entries\n");
    return EFI_NOT_FOUND;
  }

  TargetEntryFound = FALSE;
  for (i = 0; i < SizeInByte / sizeof (APCB_DPPRCL_REPAIR_ENTRY); i ++) {
    if (CompareDramPostPkgRepairEntry (&EntryArray[i], Entry)) {
      TargetEntryFound                        = TRUE;
      EntryArray[i].Valid                     = 0;
    }
  }

  if (!TargetEntryFound) {
    IDS_HDT_CONSOLE_PSP_TRACE ("[APCB Lib V3] Failed to find the DDR4 Post Package Repair Entry\n");
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

