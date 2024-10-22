/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Raven API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include "CcxZenRvIdsSyncMsr.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Protocol/MpService.h>
#include <Protocol/SmmControl2.h>
#include "Guid/EventGroup.h"
#include <IdsHookId.h>
#include "IdsNvIdRV.h"
#include "IdsNvDefRV.h"
#include "CcxZenRvIdsCustomPstates.h"
#include "Filecode.h"

#define FILECODE LIBRARY_CCXZENRVIDSHOOKLIB_DXE_CCXZENRVIDSHOOKLIBDXE_FILECODE
/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
EFI_EVENT                             CcxIdsZenDxeMpServiceEvent;

VOID                                  *CcxIdsZenForMpServiceEvent;

#define IDS_MAX_NUM_OF_SYNC_MSR 20
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
EFIAPI
CcxIdsZenRvDxeMpServiceCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

IDS_HOOK_STATUS
CcxZenRvDxeAfterApLaunch (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  UINT64            IdsNvValue;
  EFI_STATUS        Status;
  EFI_STATUS        CalledStatus;
  AMD_CONFIG_PARAMS StdHeader;

  Status = EFI_SUCCESS;

  if (CcxIsBsp (&StdHeader)) {
    IDS_HDT_CONSOLE (CPU_TRACE, "CcxZenRvDxeAfterApLaunch External Options\n");

    // L2 TLB Associativity
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_L2_TLB_WAY_ALLOC, &IdsNvValue) {
      if (IdsNvValue != IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_AUTO) {
        AsmMsrAndThenOr64 (0xC0011023, ~(UINT64) BIT45, LShiftU64 ((IdsNvValue & 1), 45));
      }
    }

    //Enable IBS
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_EN_IBS, &IdsNvValue) {
      switch (IdsNvValue) {
      case IDSOPT_CMN_CPU_EN_IBS_AUTO:
        break;
      case IDSOPT_CMN_CPU_EN_IBS_ENABLED:
        // MSRC001_1005[42]=1
        AsmMsrOr64 (0xC0011005, BIT42);
        // MSRC001_1020[54]=1
        AsmMsrOr64 (0xC0011020, BIT54);
        // MSRC001_1029[19]=1
        AsmMsrOr64 (0xC0011029, BIT19);
        break;
      case IDSOPT_CMN_CPU_EN_IBS_DISABLED:
        // MSRC001_1005[42]=0
        AsmMsrAnd64 (0xC0011005, ~((UINT64)BIT42));
        // MSRC001_1020[54]=0
        AsmMsrAnd64 (0xC0011020, ~((UINT64)BIT54));
        // MSRC001_1029[19]=0
        AsmMsrAnd64 (0xC0011029, ~((UINT64)BIT19));
        break;
      default:
        ASSERT (FALSE);
        break;
      }
    }

  }

  //
  // Set up call back after gEfiMpServiceProtocolGuid.
  //
  CalledStatus = gBS->CreateEventEx (
                           EVT_NOTIFY_SIGNAL,
                           TPL_NOTIFY,
                           CcxIdsZenRvDxeMpServiceCallBack,
                           NULL,
                           NULL,
                           &CcxIdsZenDxeMpServiceEvent
                           );
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = gBS->RegisterProtocolNotify (
                          &gEfiMpServiceProtocolGuid,
                          CcxIdsZenDxeMpServiceEvent,
                          &(CcxIdsZenForMpServiceEvent)
                          );
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  if (Status == EFI_SUCCESS) {
    return IDS_HOOK_SUCCESS;
  } else {
    return IDS_HOOK_ERROR;
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * CcxIdsZenRvDxeMpServiceCallBack
 *
 *
 *  Parameters:
 *    @param[in]     Event
 *    @param[in]     *Context
 *
 *    @retval        VOID
 */
VOID
EFIAPI
CcxIdsZenRvDxeMpServiceCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT16              i;
  IDS_BSC_AP_MSR_SYNC ApMsrSync[IDS_MAX_NUM_OF_SYNC_MSR];
  AMD_CONFIG_PARAMS   StdHeader;

  ZeroMem (ApMsrSync, sizeof (ApMsrSync));

  i = 0;

  ApMsrSync[i].RegisterAddress = 0xC0011005;
  ApMsrSync[i++].RegisterMask  = BIT42;

  ApMsrSync[i].RegisterAddress = 0xC0011020;
  ApMsrSync[i++].RegisterMask  = BIT54;

  ApMsrSync[i].RegisterAddress = 0xC0011023;
  ApMsrSync[i++].RegisterMask  = BIT45;

  ApMsrSync[i].RegisterAddress = 0xC0011029;
  ApMsrSync[i++].RegisterMask  = (BIT19);


  // End of sync-up list

  ASSERT (i < IDS_MAX_NUM_OF_SYNC_MSR);
  if (i >= IDS_MAX_NUM_OF_SYNC_MSR) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: IDS ApMsrSync overflow\n");
    IDS_DEADLOOP ()
  }
  CcxIdsZenDxeSyncMsr (ApMsrSync, &StdHeader);
}


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define CCX_IDS_HOOKS_INT
#else
  #include <Internal/CcxZenRvIdsHookLibIntDxe.h>
#endif

IDS_HOOK_ELEMENT CcxZenRvIdsHooks[] = {
  {
    IDS_HOOK_CCX_AFTER_AP_LAUNCH,
    &CcxZenRvDxeAfterApLaunch
  },
  {
    IDS_HOOK_CCX_CUSTOM_PSTATES,
    &CcxZenRvIdsDxeCustomPstates
  },
  CCX_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE CcxZenRvIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxZenRvIdsHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxZenRvIdsHookTable;
  return AGESA_SUCCESS;
}

