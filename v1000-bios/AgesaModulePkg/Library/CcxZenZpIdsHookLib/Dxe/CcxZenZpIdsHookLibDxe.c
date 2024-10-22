/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
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
#include "CcxZenZpIdsSyncMsr.h"
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Protocol/MpService.h>
#include <Protocol/SmmControl2.h>
#include "Guid/EventGroup.h"
#include <IdsHookId.h>
#include "IdsNvIdZP.h"
#include "IdsNvDefZP.h"
#include "CcxZenZpIdsCustomPstates.h"
#include "Filecode.h"

#define FILECODE LIBRARY_CCXZENZPIDSHOOKLIB_DXE_CCXZENZPIDSHOOKLIBDXE_FILECODE
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
CcxIdsZenZpDxeMpServiceCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

IDS_HOOK_STATUS
CcxZenZpDxeAfterApLaunch (
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
    IDS_HDT_CONSOLE (CPU_TRACE, "CcxZenZpDxeAfterApLaunch External Options\n");

    // RedirectForReturnDis
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_GEN_W_A05, &IdsNvValue) {
      if (IdsNvValue != IDSOPT_CMN_CPU_GEN_W_A05_AUTO) {
        AsmMsrAndThenOr64 (0xC0011029, ~(UINT64) BIT14, LShiftU64 ((IdsNvValue & 1), 14));
      }
    }

    // L2 TLB Associativity
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_L2_TLB_WAY_ALLOC, &IdsNvValue) {
      if (IdsNvValue != IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_AUTO) {
        AsmMsrAndThenOr64 (0xC0011023, ~(UINT64) BIT45, LShiftU64 ((IdsNvValue & 1), 45));
      }
    }

    // Disable uop Cache
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_OPCACHE_CTRL, &IdsNvValue) {
      switch (IdsNvValue) {
      case IDSOPT_CMN_CPU_OPCACHE_CTRL_AUTO:
        break;
      case IDSOPT_CMN_CPU_OPCACHE_CTRL_DISABLED:
        // MSR C001_1021[5] = 1
        AsmMsrOr64 (0xC0011021, BIT5);
        break;
      case IDSOPT_CMN_CPU_OPCACHE_CTRL_ENABLED:
        // MSR C001_1021[5] = 0
        AsmMsrAnd64 (0xC0011021, ~((UINT64)BIT5));
        break;
      default:
        ASSERT (FALSE);
        break;
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

    // Streaming Stores Control
    IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_STREAMING_STORES_CTRL, &IdsNvValue) {
      switch (IdsNvValue) {
      case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_AUTO:
        break;
      case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_ENABLED:
        // MSRC001_1020[28]=0
        AsmMsrAnd64 (0xC0011020, ~((UINT64)BIT28));
        break;
      case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_DISABLED:
        // MSRC001_1020[28]=1
        AsmMsrOr64 (0xC0011020, BIT28);
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
                           CcxIdsZenZpDxeMpServiceCallBack,
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
 * CcxIdsZenZpDxeMpServiceCallBack
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
CcxIdsZenZpDxeMpServiceCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT16              i;
  IDS_BSC_AP_MSR_SYNC ApMsrSync[IDS_MAX_NUM_OF_SYNC_MSR];
  AMD_CONFIG_PARAMS   StdHeader;

  ZeroMem (ApMsrSync, sizeof (ApMsrSync));

  i = 0;

  ApMsrSync[i].RegisterAddress = 0xC0011029;
  ApMsrSync[i++].RegisterMask  = (BIT19 | BIT14);

  ApMsrSync[i].RegisterAddress = 0xC0011023;
  ApMsrSync[i++].RegisterMask  = BIT45;

  ApMsrSync[i].RegisterAddress = 0xC0011005;
  ApMsrSync[i++].RegisterMask  = BIT42;

  ApMsrSync[i].RegisterAddress = 0xC0011020;
  ApMsrSync[i++].RegisterMask  = BIT54 | BIT28;

  ApMsrSync[i].RegisterAddress = 0xC0011021;
  ApMsrSync[i++].RegisterMask  = BIT5;

  ApMsrSync[i].RegisterAddress = 0xC0011022;
  ApMsrSync[i++].RegisterMask  = BIT16;

  ApMsrSync[i].RegisterAddress = 0xC001102B;
  ApMsrSync[i++].RegisterMask  = BIT0;

  // End of sync-up list

  ASSERT (i < IDS_MAX_NUM_OF_SYNC_MSR);
  if (i >= IDS_MAX_NUM_OF_SYNC_MSR) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: IDS ApMsrSync overflow\n");
    IDS_DEADLOOP ()
  }
  CcxIdsZenZpDxeSyncMsr (ApMsrSync, &StdHeader);
}


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define CCX_IDS_HOOKS_INT
#else
  #include <Internal/CcxZenZpIdsHookLibIntDxe.h>
#endif

IDS_HOOK_ELEMENT CcxZenZpIdsHooks[] = {
  {
    IDS_HOOK_CCX_AFTER_AP_LAUNCH,
    &CcxZenZpDxeAfterApLaunch
  },
  {
    IDS_HOOK_CCX_CUSTOM_PSTATES,
    &CcxZenZpIdsDxeCustomPstates
  },
  CCX_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE CcxZenZpIdsHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  CcxZenZpIdsHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &CcxZenZpIdsHookTable;
  return AGESA_SUCCESS;
}

