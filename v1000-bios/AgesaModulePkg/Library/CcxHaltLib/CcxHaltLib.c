/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Halt Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 312770 $   @e \$Date: 2015-02-11 13:24:42 +0800 (Wed, 11 Feb 2015) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include <Library/AmdBaseLib.h>
#include <Library/CcxHaltLib.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXHALTLIB_CCXHALTLIB_FILECODE

/*---------------------------------------------------------------------------------------*/
/**
 * Update AP MTRR settings list according to PCD
 *
 *
 * @param[in, out]  ApMtrrSettingsList List of MTRR settings for AP
 *
 */
VOID
UpdateApMtrrSettings (
  IN OUT   AP_MTRR_SETTINGS  *ApMtrrSettingsList
  )
{
  while (ApMtrrSettingsList->MsrAddr != CPU_LIST_TERMINAL) {
    switch (ApMtrrSettingsList->MsrAddr) {
    case AMD_AP_MTRR_FIX64k_00000:
      if (PcdGet64 (PcdAmdFixedMtrr250) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr250);
      }
      break;
    case AMD_AP_MTRR_FIX16k_80000:
      if (PcdGet64 (PcdAmdFixedMtrr258) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr258);
      }
      break;
    case AMD_AP_MTRR_FIX16k_A0000:
      if (PcdGet64 (PcdAmdFixedMtrr259) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr259);
      }
      break;
    case AMD_AP_MTRR_FIX4k_C0000:
      if (PcdGet64 (PcdAmdFixedMtrr268) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr268);
      }
      break;
    case AMD_AP_MTRR_FIX4k_C8000:
      if (PcdGet64 (PcdAmdFixedMtrr269) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr269);
      }
      break;
    case AMD_AP_MTRR_FIX4k_D0000:
      if (PcdGet64 (PcdAmdFixedMtrr26A) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26A);
      }
      break;
    case AMD_AP_MTRR_FIX4k_D8000:
      if (PcdGet64 (PcdAmdFixedMtrr26B) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26B);
      }
      break;
    case AMD_AP_MTRR_FIX4k_E0000:
      if (PcdGet64 (PcdAmdFixedMtrr26C) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26C);
      }
      break;
    case AMD_AP_MTRR_FIX4k_E8000:
      if (PcdGet64 (PcdAmdFixedMtrr26D) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26D);
      }
      break;
    case AMD_AP_MTRR_FIX4k_F0000:
      if (PcdGet64 (PcdAmdFixedMtrr26E) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26E);
      }
      break;
    case AMD_AP_MTRR_FIX4k_F8000:
      if (PcdGet64 (PcdAmdFixedMtrr26F) != 0xFFFFFFFFFFFFFFFF) {
        ApMtrrSettingsList->MsrData = PcdGet64 (PcdAmdFixedMtrr26F);
      }
      break;
    default:
      break;
    }
    ApMtrrSettingsList++;
  }
}

