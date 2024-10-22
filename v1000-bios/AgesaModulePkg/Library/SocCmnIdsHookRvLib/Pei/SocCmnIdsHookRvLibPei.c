/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD common SOC IDS functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */
#include "Uefi.h"
#include "PiPei.h"
#include "cpuRegisters.h"
#include "Library/AmdBaseLib.h"
#include <Library/BaseLib.h>
#include "Library/IdsLib.h"
#include "Library/AmdIdsHookLib.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/CcxDownCoreLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/AmdSocPcdInitPpi.h>
#include "IdsHookId.h"
#include "Filecode.h"

#define FILECODE LIBRARY_SOCCMNIDSHOOKRVLIB_PEI_SOCCMNIDSHOOKRVLIBPEI_FILECODE


#ifndef IDS_HOOK_INTERNAL_SUPPORT
  #define SOC_IDS_HOOKS_INT
#else
  #include "Internal/SocCmnIdsHookRvLibIntPei.h"
#endif

EFI_STATUS
EFIAPI
SocRvIdsDownCoreCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );


EFI_PEI_NOTIFY_DESCRIPTOR   mSocIdsAgesaPcdInitReadyCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdPcdInitReadyPpiGuid,
  SocRvIdsDownCoreCallback
};

IDS_HOOK_STATUS
SocRvIdsBeginningCmn (
  HOOK_ID   HookId,
  VOID      *Handle,
  VOID      *Data
  )
{
  EFI_PEI_SERVICES              **PeiServices;

  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

  (**PeiServices).NotifyPpi ((CONST EFI_PEI_SERVICES**)PeiServices, &mSocIdsAgesaPcdInitReadyCallback);

  return IDS_HOOK_SUCCESS;
}

EFI_STATUS
EFIAPI
SocRvIdsDownCoreCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  UINT8                                 DownCoreMode;
  UINT8                                 SmtMode;
  UINTN                                 EnabledThreadCount;
  UINTN                                 EnabledCoreCount;
  UINTN                                 DesiredCoreCount;
  CPUID_DATA                            CpuId;

  IDS_HDT_CONSOLE (CPU_TRACE, "SocRvIdsDownCoreCallback External Options\n");

  DownCoreMode = PcdGet8 (PcdAmdDownCoreMode);
  SmtMode = PcdGet8 (PcdAmdSmtMode);

  if ((DownCoreMode != CCX_DOWN_CORE_AUTO) || (SmtMode != 1)) {
    switch (DownCoreMode) {
    case CCX_DOWN_CORE_1_0:
      DesiredCoreCount = 1;
      break;
    case CCX_DOWN_CORE_2_0:
      DesiredCoreCount = 2;
      break;
    case CCX_DOWN_CORE_3_0:
      DesiredCoreCount = 3;
      break;
    default:
      DesiredCoreCount = 4;
      break;
    }

    AsmCpuid (
        AMD_CPUID_ASIZE_PCCOUNT,
        &(CpuId.EAX_Reg),
        &(CpuId.EBX_Reg),
        &(CpuId.ECX_Reg),
        &(CpuId.EDX_Reg)
        );

    EnabledThreadCount = (CpuId.ECX_Reg & 0xFF) + 1; // bit 7:0
    EnabledCoreCount = EnabledThreadCount / CcxGetThreadsPerCore ();

    if (((SmtMode == 0) && (CcxGetThreadsPerCore () > 1)) ||
        (DesiredCoreCount < EnabledCoreCount)) {
      IDS_HDT_CONSOLE (CPU_TRACE, "set PcdToggleAllPwrGoodOnCf9 to FALSE\n");
      PcdSetBool (PcdToggleAllPwrGoodOnCf9, FALSE);
    }
  }

  return EFI_SUCCESS;
}


IDS_HOOK_ELEMENT SocIdsCmnHooks[] = {
  {
    IDS_HOOK_BEGINNING_OF_AGESA,
    &SocRvIdsBeginningCmn
  },
  SOC_IDS_HOOKS_INT
  IDS_HOOKS_END
};

IDS_HOOK_TABLE SocIdsCmnHookTable = {
  IDS_HOOK_TABLE_HEADER_REV1_DATA,
  SocIdsCmnHooks
};


AGESA_STATUS
GetIdsHookTable (
  IDS_HOOK_TABLE **IdsHookTable
  )
{
  *IdsHookTable = &SocIdsCmnHookTable;
  return AGESA_SUCCESS;
}

