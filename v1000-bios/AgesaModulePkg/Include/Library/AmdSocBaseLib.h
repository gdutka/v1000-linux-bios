/*****************************************************************************
 *
 * Copyright (C) 2015-2022 Advanced Micro Devices, Inc. All rights reserved.
 *
 *******************************************************************************
 */

#ifndef _AMD_SOC_BASE_LIB_H_
#define _AMD_SOC_BASE_LIB_H_

#include <AmdSoc.h>

BOOLEAN
SocHardwareIdentificationCheck (
  IN    SOC_ID_STRUCT *SocId
  );

BOOLEAN
SocFamilyIdentificationCheck (
  IN    UINT32 SocFamilyID
  );

BOOLEAN
AmdPcdInit (
  IN       UINT32        NumberOfPcdEntries,
  IN       AMD_PCD_LIST  *AmdPcdList
  );

#endif  // _AMD_SOC_BASE_LIB_H_

