/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdSocBaseLib.c
 *
 * Feature Soc common functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: SOC
 * @e \$Revision: 313790 $ @e \$Date: 2015-02-26 23:31:28 -0600 (Thu, 26 Feb 2015) $
 *
 **/
/*****************************************************************************
  *
  * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
  *
  *******************************************************************************
  *
 */

#include <Library/BaseLib.h>
#include <Include/Filecode.h>
#include <Library/PcdLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdSocBaseLib.h>

#define FILECODE LIBRARY_AMDSOCBASELIB_AMDSOCBASELIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function check SOC hardware identification ID
 *
 *     @param[in]   SocId  - the SOC_ID_STRUCT
 *
 *     @return          TRUE -  Hardware identification ID check pass.
 *     @return          FALSE - Hardware identification ID check fail.
 */

BOOLEAN
SocHardwareIdentificationCheck (
  IN    SOC_ID_STRUCT *SocId
  )
{
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );

  // Check Hardware Identification

  if (SocId->SocFamilyID != (CpuId.EAX_Reg & RAW_FAMILY_ID_MASK)) {
    return FALSE;
  }

  if (SocId->PackageType != ((CpuId.EBX_Reg >> 28) & 0xF)) {
    return FALSE;
  }

  return TRUE;
}

/**
 *
 *
 *      This function check SOC family identification ID
 *
 *     @param[in]   SocFamilyID  - FmailyId
 *
 *     @return          TRUE -  Hardware identification ID check pass.
 *     @return          FALSE - Hardware identification ID check fail.
 */

BOOLEAN
SocFamilyIdentificationCheck (
  IN    UINT32 SocFamilyID
  )
{
  CPUID_DATA CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );

  // Check Hardware Identification

  if (SocFamilyID != (CpuId.EAX_Reg & RAW_FAMILY_ID_MASK)) {
    return FALSE;
  }

  return TRUE;
}

BOOLEAN
AmdPcdInit(
  IN       UINT32               NumberOfPcdEntries,
  IN       AMD_PCD_LIST         *AmdPcdList
  )
{
  AMD_PCD_LIST *PlatformPcdList;
  AMD_PCD_PTR *Pcd_Ptr_Buff;
  UINT32 i;

  PlatformPcdList = AmdPcdList;
  for (i = 0 ; i < NumberOfPcdEntries ; i++) {
    switch (PlatformPcdList[i].AmdPcdDataType) {
    case PCD_BOOL:
      LibPcdSetExBool (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (BOOLEAN)PlatformPcdList[i].Value);
      break;
    case PCD_UINT8:
      LibPcdSetEx8 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT8)PlatformPcdList[i].Value);
      break;

    case PCD_UINT16:
      LibPcdSetEx16 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT16)PlatformPcdList[i].Value);
      break;

    case PCD_UINT32:
      LibPcdSetEx32 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT32)PlatformPcdList[i].Value);
      break;

    case PCD_UINT64:
      LibPcdSetEx64 (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, (UINT64)PlatformPcdList[i].Value);
      break;

    case PCD_PTR:
      Pcd_Ptr_Buff = (AMD_PCD_PTR *)(UINTN)PlatformPcdList[i].Value;
      LibPcdSetExPtr (PlatformPcdList[i].AmdConfigurationParameterPcdGuid, PlatformPcdList[i].AmdConfigurationParameterPcdTokenNumber, &(Pcd_Ptr_Buff->BufferSize), Pcd_Ptr_Buff->Buffer);
      break;
    }
  }
  return TRUE;
}

