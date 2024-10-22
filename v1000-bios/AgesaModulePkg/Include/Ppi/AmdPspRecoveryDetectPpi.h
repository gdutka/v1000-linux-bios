/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP PspRecovery Detect PPI GUID definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 *
 * Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 **/
#ifndef _AMD_PSP_RECOVERY_DETECTPPI_H_
#define _AMD_PSP_RECOVERY_DETECTPPI_H_

///
/// PPI prototype
///
/// Defines AMD_PSP_RECOVERY_DETECT_PPI, which will be published when PSP initialized recovery detected
///
typedef struct _AMD_PSP_RECOVERY_DETECT_PPI {
  UINT8       Version;     ///< PPI Version
} AMD_PSP_RECOVERY_DETECT_PPI;

extern EFI_GUID gAmdPspRecoveryDetectPpiGuid;

#endif

