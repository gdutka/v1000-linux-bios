/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement Custom Core Pstates
 *
 * Contains code that Custom Core Pstates
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

#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxPstatesLib.h>
#include <Addendum/Apcb/Inc/ZP/APOB.h>
#include <Library/AmdPspApobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CcxMpServicesLib.h>
#include <Library/CcxIdsCustomPstatesLib.h>
#include <Protocol/AmdCoreTopologyProtocol.h>
#include <Protocol/AmdNbioSmuServicesProtocol.h>
#include "CcxRegistersZp.h"
#include "CcxZenZpIdsCustomPstates.h"
#include "IdsNvIdZP.h"
#include "IdsNvDefZP.h"
#include "Filecode.h"

#define FILECODE LIBRARY_CCXZENZPIDSHOOKLIB_DXE_CCXZENZPIDSCUSTOMPSTATES_FILECODE

IDS_NV_ID IdsNvIdCustomPstate[] = {
  IDSNVID_CPU_PST_CUSTOM_P0,
  IDSNVID_CPU_PST_CUSTOM_P1,
  IDSNVID_CPU_PST_CUSTOM_P2,
  IDSNVID_CPU_PST_CUSTOM_P3,
  IDSNVID_CPU_PST_CUSTOM_P4,
  IDSNVID_CPU_PST_CUSTOM_P5,
  IDSNVID_CPU_PST_CUSTOM_P6,
  IDSNVID_CPU_PST_CUSTOM_P7
};

IDS_NV_ID IdsNvIdCustomFid[] = {
  IDSNVID_CPU_PST0_FID,
  IDSNVID_CPU_PST1_FID,
  IDSNVID_CPU_PST2_FID,
  IDSNVID_CPU_PST3_FID,
  IDSNVID_CPU_PST4_FID,
  IDSNVID_CPU_PST5_FID,
  IDSNVID_CPU_PST6_FID,
  IDSNVID_CPU_PST7_FID
};

IDS_NV_ID IdsNvIdCustomDid[] = {
  IDSNVID_CPU_PST0_DID,
  IDSNVID_CPU_PST1_DID,
  IDSNVID_CPU_PST2_DID,
  IDSNVID_CPU_PST3_DID,
  IDSNVID_CPU_PST4_DID,
  IDSNVID_CPU_PST5_DID,
  IDSNVID_CPU_PST6_DID,
  IDSNVID_CPU_PST7_DID
};

IDS_NV_ID IdsNvIdCustomVid[] = {
  IDSNVID_CPU_PST0_VID,
  IDSNVID_CPU_PST1_VID,
  IDSNVID_CPU_PST2_VID,
  IDSNVID_CPU_PST3_VID,
  IDSNVID_CPU_PST4_VID,
  IDSNVID_CPU_PST5_VID,
  IDSNVID_CPU_PST6_VID,
  IDSNVID_CPU_PST7_VID
};


/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/
/**
 * CcxZenZpIdsDxeCustomPstates - routine to read Custom Pstates information from IDS
 *
 *
 * @param[in]  HookId       IDS Hook Id
 * @param[in]  *Handle      Unused
 * @param[in]  *Data        Unused
 *
 * @retval     IDS_HOOK_STATUS
 *
 */
IDS_HOOK_STATUS
CcxZenZpIdsDxeCustomPstates (
  IN       HOOK_ID HookId,
  IN       VOID *Handle,
  IN       VOID *Data
  )
{
  UINT8   i;
  UINT64  IdsNvValue;
  IDS_CUSTOM_CORE_PSTATE  CustomPstate[PS_MAX_REG - PS_MIN_REG + 1];

  if ((!PcdGetBool (PcdOcDisable)) && (CcxIsBsp (NULL))) {
    IDS_HDT_CONSOLE (CPU_TRACE, "CcxZenZpIdsDxeCustomPstates Options\n");

    // Get P0 setting from PCD
    CustomPstate[0].Custom  = PcdGet8 (PcdAmdCcxP0Setting);
    if (CustomPstate[0].Custom == CCX_IDS_CORE_PSTATES_CUSTOM) {
      CustomPstate[0].CoreFid = PcdGet8 (PcdAmdCcxP0Fid);
      CustomPstate[0].CoreDid = PcdGet8 (PcdAmdCcxP0Did);
      CustomPstate[0].CoreVid = PcdGet8 (PcdAmdCcxP0Vid);
    }

    // Get others from IDS
    for (i = 1; i <= (PS_MAX_REG - PS_MIN_REG); i++) {
      CustomPstate[i].Custom = CCX_IDS_CORE_PSTATES_AUTO;
      IDS_NV_READ_SKIP (IdsNvIdCustomPstate[i], &IdsNvValue) {
        CustomPstate[i].Custom = (UINT8) IdsNvValue;
        if (CustomPstate[i].Custom == CCX_IDS_CORE_PSTATES_CUSTOM) {
          // Get FID
          IDS_NV_READ_SKIP (IdsNvIdCustomFid[i], &IdsNvValue) {
            CustomPstate[i].CoreFid = (UINT8) IdsNvValue;
          }
          // Get DID
          IDS_NV_READ_SKIP (IdsNvIdCustomDid[i], &IdsNvValue) {
            CustomPstate[i].CoreDid = (UINT8) IdsNvValue;
            if (CustomPstate[i].CoreDid > 0x1A) {
              CustomPstate[i].CoreDid &= 0xFE;
            }
          }
          // Get VID
          IDS_NV_READ_SKIP (IdsNvIdCustomVid[i], &IdsNvValue) {
            CustomPstate[i].CoreVid = (UINT8) IdsNvValue;
          }
        }
      }
    }

    CcxIdsCustomPstateSetMsrLib (CustomPstate);
  } // End of CcxIsBsp

  return IDS_HOOK_SUCCESS;
}

